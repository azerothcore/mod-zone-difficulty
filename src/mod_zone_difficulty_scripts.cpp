/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "GridNotifiers.h"
#include "Pet.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "StringConvert.h"
#include "Tokenize.h"
#include "Unit.h"
#include "ZoneDifficulty.h"

ZoneDifficulty* ZoneDifficulty::instance()
{
    static ZoneDifficulty instance;
    return &instance;
}

void ZoneDifficulty::LoadMapDifficultySettings()
{
    if (!sZoneDifficulty->IsEnabled)
    {
        return;
    }

    // Default values for when there is no entry in the db for duels (index 0xFFFFFFFF)
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].HealingNerfPct = 1;
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].AbsorbNerfPct = 1;
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].MeleeDamageBuffPct = 1;
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].SpellDamageBuffPct = 1;

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_info"))
    {
        do
        {
            uint32 mapId = (*result)[0].Get<uint32>();
            uint32 phaseMask = (*result)[1].Get<uint32>();
            ZoneDifficultyNerfData data;
            data.HealingNerfPct = (*result)[2].Get<float>();
            data.AbsorbNerfPct = (*result)[3].Get<float>();
            data.MeleeDamageBuffPct = (*result)[4].Get<float>();
            data.SpellDamageBuffPct = (*result)[5].Get<float>();
            data.Enabled = (*result)[6].Get<bool>();
            sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][phaseMask] = data;

            // duels do not check for phases. Only 0 is allowed.
            if (mapId == DUEL_INDEX && phaseMask != 0)
            {
                LOG_ERROR("sql.sql", "Table `zone_difficulty_info` for criteria (duel mapId: {}) has wrong value ({}), must be 0 for duels.", mapId, phaseMask);
            }

        } while (result->NextRow());
    }

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_spelloverrides"))
    {
        do
        {
            if ((*result)[2].Get<bool>())
            {
                sZoneDifficulty->SpellNerfOverrides[(*result)[0].Get<uint32>()] = (*result)[1].Get<float>();
            }

        } while (result->NextRow());
    }
    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_disallowed_buffs"))
    {
        do
        {
            std::vector<uint32> debuffs;
            uint32 mapid;
            if ((*result)[2].Get<bool>())
            {
                std::string spellString = (*result)[1].Get<std::string>();
                std::vector<std::string_view> tokens = Acore::Tokenize(spellString, ' ', false);

                mapid = (*result)[0].Get<uint32>();
                for (auto token : tokens)
                {
                    if (token.empty())
                    {
                        continue;
                    }

                    uint32 spell;
                    if ((spell = Acore::StringTo<uint32>(token).value()))
                    {
                        debuffs.push_back(spell);
                    }
                    else
                    {
                        LOG_ERROR("sql.sql", "Disabling buffs for spell '{}' is invalid, skipped.", spell);
                    }
                }
                sZoneDifficulty->DisallowedBuffs[mapid] = debuffs;
            }
        } while (result->NextRow());
    }
    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_loot_objects"))
    {
        do
        {
            uint32 mapId = (*result)[0].Get<uint32>();
            uint32 entry = (*result)[1].Get<uint32>();
            uint32 objecttype = (*result)[2].Get<int16>();
            if (objecttype == 1)
            {
                sZoneDifficulty->HardmodeCreatureLoot[mapId].push_back(entry);
            }
            else if (objecttype == 2)
            {
                sZoneDifficulty->HardmodeGameobjectLoot[mapId].push_back(entry);
            }
            else
            {
                LOG_ERROR("sql.sql", "Table `zone_difficulty_loot_objects` objecttype: has wrong value ({}), must be 1 for creatures or 2 for gameobjects.", objecttype);
            }

            if (mapId <= 0 || entry <= 0)
            {
                LOG_ERROR("sql.sql", "Table `zone_difficulty_loot_objects` for criteria MapId: {} OR Entry: {} has wrong value. Must be > 0.", mapId, entry);
            }

        } while (result->NextRow());
    }
}

bool ZoneDifficulty::IsValidNerfTarget(Unit* target)
{
    return target->IsPlayer() || target->IsPet() || target->IsGuardian();
}

bool ZoneDifficulty::VectorContains(std::vector<uint32> vec, uint32 element)
{
    return find(vec.begin(), vec.end(), element) != vec.end();
}

/*
 *  Check if the target is in a duel while residing in the DUEL_AREA and their opponent is a valid object.
 *  Used to determine when the duel-specific nerfs should be applied.
 */
bool ZoneDifficulty::ShouldNerfInDuels(Unit* target)
{
    if (target->GetAreaId() != DUEL_AREA)
    {
        return false;
    }

    if (target->ToTempSummon() && target->ToTempSummon()->GetSummoner())
    {
        target = target->ToTempSummon()->GetSummoner()->ToUnit();
    }

    if (!target->GetAffectingPlayer())
    {
        return false;
    }

    if (!target->GetAffectingPlayer()->duel)
    {
        return false;
    }

    if (target->GetAffectingPlayer()->duel->State != DUEL_STATE_IN_PROGRESS)
    {
        return false;
    }

    if (!target->GetAffectingPlayer()->duel->Opponent)
    {
        return false;
    }

    return true;
}

/*
 *  Find the lowest phase for the target's mapId, which has a db entry for the target's map
 *  and at least partly matches the target's phase.
 *  `mapId` can be the id of a map or `DUEL_INDEX` to use the duel specific settings.
 *  Return -1 if none found.
 */
int32 ZoneDifficulty::GetLowestMatchingPhase(uint32 mapId, uint32 phaseMask)
{
    // Check if there is an entry for the mapid at all
    if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end())
    {

        // Check if 0 is assigned as a phase to cover all phases
        if (sZoneDifficulty->ZoneDifficultyNerfInfo[mapId].find(0) != sZoneDifficulty->ZoneDifficultyNerfInfo[mapId].end())
        {
            return 0;
        }

        // Check all $key in [mapId][$key] if they match the target's visible phases
        for (auto const& [key, value] : sZoneDifficulty->ZoneDifficultyNerfInfo[mapId])
        {
            if (key & phaseMask)
            {
                return key;
            }
        }
    }
    return -1;
}

/*
 *  Store the HardmodeInstanceData in the database for the given instance id.
 *  zone_difficulty_instance_saves is used to store the data.
 *  `InstanceID` INT NOT NULL DEFAULT 0,
 *  `HardmodeOn` TINYINT NOT NULL DEFAULT 0,
 *  `HardmodePossible` TINYINT NOT NULL DEFAULT 1,
 */
 void ZoneDifficulty::SaveHardmodeInstanceData(uint32 instanceId)
{
    if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) == sZoneDifficulty->HardmodeInstanceData.end())
    {
        LOG_ERROR("sql.sql", "ZoneDifficulty::SaveHardmodeInstanceData: InstanceId {} not found in HardmodeInstanceData.", instanceId);
        return;
    }

    CharacterDatabase.Execute("REPLACE INTO zone_difficulty_instance_saves (InstanceID, HardmodeOn, CompletedEncounterOnNormal) VALUES (%u, %u, %u)", instanceId, sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn, sZoneDifficulty->HardmodeInstanceData[instanceId].CompletedEncounterOnNormal);
}

class mod_zone_difficulty_unitscript : public UnitScript
{
public:
    mod_zone_difficulty_unitscript() : UnitScript("mod_zone_difficulty_unitscript") { }

    void OnAuraApply(Unit* target, Aura* aura) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            bool nerfInDuel = sZoneDifficulty->ShouldNerfInDuels(target);

            //Check if the map of the target is subject of a nerf at all OR if the target is subject of a nerf in a duel
            if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end() || nerfInDuel)
            {
                if (SpellInfo const* spellInfo = aura->GetSpellInfo())
                {
                    // Skip spells not affected by vulnerability (potions)
                    if (spellInfo->HasAttribute(SPELL_ATTR0_NO_IMMUNITIES))
                    {
                        return;
                    }

                    if (spellInfo->HasAura(SPELL_AURA_SCHOOL_ABSORB))
                    {
                        std::list<AuraEffect*> AuraEffectList = target->GetAuraEffectsByType(SPELL_AURA_SCHOOL_ABSORB);

                        for (AuraEffect* eff : AuraEffectList)
                        {
                            if ((eff->GetAuraType() != SPELL_AURA_SCHOOL_ABSORB) || (eff->GetSpellInfo()->Id != spellInfo->Id))
                            {
                                continue;
                            }

                            if (sZoneDifficulty->IsDebugInfoEnabled)
                            {
                                if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                                {
                                    if (player->GetSession())
                                    {
                                        ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Base Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, eff->GetAmount());
                                    }
                                }
                            }

                            int32 absorb = eff->GetAmount();
                            uint32 phaseMask = target->GetPhaseMask();
                            int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
                            if (sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].Enabled && (matchingPhase != -1))
                            {
                                absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].AbsorbNerfPct;
                            }
                            else if (nerfInDuel)
                            {
                                absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].AbsorbNerfPct;
                            }

                            //This check must be last and override duel and map adjustments
                            if (sZoneDifficulty->SpellNerfOverrides.find(spellInfo->Id) != sZoneDifficulty->SpellNerfOverrides.end())
                            {
                                absorb = eff->GetAmount() * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id];
                            }

                            eff->SetAmount(absorb);

                            if (sZoneDifficulty->IsDebugInfoEnabled)
                            {
                                if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                                {
                                    if (player->GetSession())
                                    {
                                        ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Post Nerf Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, eff->GetAmount());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void ModifyHealReceived(Unit* target, Unit* /*healer*/, uint32& heal, SpellInfo const* spellInfo) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            if (spellInfo)
            {
                // Skip spells not affected by vulnerability (potions) and bandages
                if (spellInfo->HasAttribute(SPELL_ATTR0_NO_IMMUNITIES) || spellInfo->Mechanic == MECHANIC_BANDAGE)
                {
                    return;
                }
            }

            uint32 mapId = target->GetMapId();
            bool nerfInDuel = sZoneDifficulty->ShouldNerfInDuels(target);
            //Check if the map of the target is subject of a nerf at all OR if the target is subject of a nerf in a duel
            if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end() || sZoneDifficulty->ShouldNerfInDuels(target))
            {
                //This check must be first and skip the rest to override everything else.
                if (spellInfo)
                {
                    if (sZoneDifficulty->SpellNerfOverrides.find(mapId) != sZoneDifficulty->SpellNerfOverrides.end())
                    {
                        heal = heal * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id];
                        return;
                    }
                }

                uint32 phaseMask = target->GetPhaseMask();
                int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
                if (sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].Enabled && matchingPhase != -1)
                {
                    heal = heal * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].HealingNerfPct;
                }
                else if (nerfInDuel)
                {
                    heal = heal * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].HealingNerfPct;
                }
            }
        }
    }

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* spellInfo) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }

        bool isDot = false;

        if (spellInfo)
        {
            for (auto const& eff : spellInfo->GetEffects())
            {
                if (eff.ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE || eff.ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE_PERCENT)
                {
                    isDot = true;
                }
            }
        }

        if (!isDot)
        {
            return;
        }

        // Disclaimer: also affects disables boss adds buff.
        if (sConfigMgr->GetOption<bool>("ModZoneDifficulty.SpellBuff.OnlyBosses", false))
        {
            if (attacker->ToCreature() && !attacker->ToCreature()->IsDungeonBoss())
            {
                return;
            }
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            uint32 phaseMask = target->GetPhaseMask();
            int32 matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);

            if (sZoneDifficulty->IsDebugInfoEnabled)
            {
                if (Player* player = attacker->ToPlayer())
                {
                    if (player->GetSession())
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("A dot tick was altered. Pre Nerf Value: %i", damage);
                    }
                }
            }

            if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end() && matchingPhase != -1)
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPct;
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].SpellDamageBuffPct;
            }

            if (sZoneDifficulty->IsDebugInfoEnabled)
            {
                if (Player* player = attacker->ToPlayer())
                {
                    if (player->GetSession())
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("A dot tick was altered. Post Nerf Value: %i", damage);
                    }
                }
            }
        }
    }

    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* spellInfo) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }

        // Disclaimer: also affects disables boss adds buff.
        if (sConfigMgr->GetOption<bool>("ModZoneDifficulty.SpellBuff.OnlyBosses", false))
        {
            if (attacker->ToCreature() && !attacker->ToCreature()->IsDungeonBoss())
            {
                return;
            }
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            uint32 phaseMask = target->GetPhaseMask();
            int32 matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
            if (spellInfo)
            {
                //This check must be first and skip the rest to override everything else.
                if (sZoneDifficulty->SpellNerfOverrides.find(spellInfo->Id) != sZoneDifficulty->SpellNerfOverrides.end())
                {
                    damage = damage * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id];
                    return;
                }

                if (sZoneDifficulty->IsDebugInfoEnabled)
                {
                    if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                    {
                        if (player->GetSession())
                        {
                            ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Before Nerf Value: %i (%f)", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage, sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPct);
                        }
                    }
                }
            }

            if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end() && matchingPhase != -1)
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPct;
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].SpellDamageBuffPct;
            }

            if (sZoneDifficulty->IsDebugInfoEnabled)
            {
                if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                {
                    if (player->GetSession())
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Post Nerf Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage);
                    }
                }
            }
        }
    }

    void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }

        // Disclaimer: also affects disables boss adds buff.
        if (sConfigMgr->GetOption<bool>("ModZoneDifficulty.MeleeBuff.OnlyBosses", false))
        {
            if (attacker->ToCreature() && !attacker->ToCreature()->IsDungeonBoss())
            {
                return;
            }
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            uint32 phaseMask = target->GetPhaseMask();
            int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
            if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end() && matchingPhase != -1)
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].MeleeDamageBuffPct;
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].MeleeDamageBuffPct;
            }
        }
    }
};

class mod_zone_difficulty_playerscript : public PlayerScript
{
public:
    mod_zone_difficulty_playerscript() : PlayerScript("mod_zone_difficulty_playerscript") { }

    void OnMapChanged(Player* player) override
    {
        uint32 mapId = player->GetMapId();
        if (sZoneDifficulty->DisallowedBuffs.find(mapId) != sZoneDifficulty->DisallowedBuffs.end())
        {
            for (auto aura : sZoneDifficulty->DisallowedBuffs[mapId])
            {
                player->RemoveAura(aura);
            }
        }
    }
};

class mod_zone_difficulty_petscript : public PetScript
{
public:
    mod_zone_difficulty_petscript() : PetScript("mod_zone_difficulty_petscript") { }

    void OnPetAddToWorld(Pet* pet) override
    {
        uint32 mapId = pet->GetMapId();
        if (sZoneDifficulty->DisallowedBuffs.find(mapId) != sZoneDifficulty->DisallowedBuffs.end())
        {
            pet->m_Events.AddEventAtOffset([mapId, pet]()
                {
                    for (uint32 aura : sZoneDifficulty->DisallowedBuffs[mapId])
                    {
                        pet->RemoveAurasDueToSpell(aura);
                    }
                }, 2s);
        }
    }
};

class mod_zone_difficulty_worldscript : public WorldScript
{
public:
    mod_zone_difficulty_worldscript() : WorldScript("mod_zone_difficulty_worldscript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sZoneDifficulty->IsEnabled = sConfigMgr->GetOption<bool>("ModZoneDifficulty.Enable", false);
        sZoneDifficulty->IsDebugInfoEnabled = sConfigMgr->GetOption<bool>("ModZoneDifficulty.DebugInfo", false);
        sZoneDifficulty->LoadMapDifficultySettings();
    }

    void OnStartup() override
    {
        sZoneDifficulty->IsEnabled = sConfigMgr->GetOption<bool>("ModZoneDifficulty.Enable", false);
        sZoneDifficulty->IsDebugInfoEnabled = sConfigMgr->GetOption<bool>("ModZoneDifficulty.DebugInfo", false);
        sZoneDifficulty->LoadMapDifficultySettings();
    }
};

class mod_zone_difficulty_globalscript : public GlobalScript
{
public:
    mod_zone_difficulty_globalscript() : GlobalScript("mod_zone_difficulty_globalscript") { }

    void OnInstanceIdRemoved(uint32 instanceId)
    {
        if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) != sZoneDifficulty->HardmodeInstanceData.end())
        {
            sZoneDifficulty->HardmodeInstanceData.erase(instanceId);
        }

            CharacterDatabase.Execute("DELETE FROM zone_difficulty_instance_saves WHERE InstanceID = %u;", instanceId);
    }

    void OnAfterUpdateEncounterState(Map* map, EncounterCreditType /*type*/, uint32 /*creditEntry*/, Unit* /*source*/, Difficulty /*difficulty_fixed*/, DungeonEncounterList const* /*encounters*/, uint32 /*dungeonCompleted*/, bool /*updated*/) override
    {
        LOG_ERROR("sql.sql", "Encounter completed");
        // todo: Apparently this doesn't fire.
        // Must set CompletedEncounterOnNormal to true, if the encounter wasn't in hardmode.
        if (sZoneDifficulty->HardmodeInstanceData.find(map->GetInstanceId()) != sZoneDifficulty->HardmodeInstanceData.end())
        {
            if (sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn == true)
            {
                return;
            }
        }
        sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].CompletedEncounterOnNormal = true;
        sZoneDifficulty->SaveHardmodeInstanceData(map->GetInstanceId());
    }
};

class mod_zone_difficulty_allcreaturescript : public AllCreatureScript
{
public:
    mod_zone_difficulty_allcreaturescript() : AllCreatureScript("mod_zone_difficulty_allcreaturescript") { }

    void OnCreatureAddWorld(Creature* creature) override
    {
        // does the map have any creature which is supposed to have an improved lootmode?
        if (sZoneDifficulty->HardmodeCreatureLoot.find(creature->GetMap()->GetId()) != sZoneDifficulty->HardmodeGameobjectLoot.end())
        {
            return;
        }

        // is the creature in the list of entries to receive improved loot?
        uint32 mapId = creature->GetMap()->GetId();
        if (!sZoneDifficulty->VectorContains(sZoneDifficulty->HardmodeCreatureLoot[mapId], creature->GetEntry()))
        {
            return;
        }

        // is the hardmode turned on for this instance?
        uint32 instanceId = creature->GetMap()->GetInstanceId();
        if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) != sZoneDifficulty->HardmodeInstanceData.end())
        {
            if (sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn == true)
            {
                creature->AddLootMode(64);
                sZoneDifficulty->HardmodeGameobjectsGUIDMap[instanceId].push_back(creature->GetGUID());
            }
        }
    }
};

class mod_zone_difficulty_allgameobjectscript : public AllGameObjectScript
{
public:
    mod_zone_difficulty_allgameobjectscript() : AllGameObjectScript("mod_zone_difficulty_allgameobjectscript") {}

    void OnGameObjectAddWorld(GameObject* go) override
    {
        // does the map have any go which is supposed to have an improved lootmode?
        if (sZoneDifficulty->HardmodeGameobjectLoot.find(go->GetMap()->GetId()) != sZoneDifficulty->HardmodeGameobjectLoot.end())
        {
            return;
        }

        // is the go in the list of entries to receive improved loot?
        uint32 mapId = go->GetMap()->GetId();
        if (!sZoneDifficulty->VectorContains(sZoneDifficulty->HardmodeGameobjectLoot[mapId], go->GetEntry()))
        {
            return;
        }

        // is the hardmode turned on for this instance?
        uint32 instanceId = go->GetMap()->GetInstanceId();
        if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) != sZoneDifficulty->HardmodeInstanceData.end())
        {
            if (sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn == true)
            {
                go->AddLootMode(64);
                sZoneDifficulty->HardmodeGameobjectsGUIDMap[instanceId].push_back(go->GetGUID());
            }
        }
    }
};

class mod_zone_difficulty_dungeonmaster : public CreatureScript
{
public:
    mod_zone_difficulty_dungeonmaster() : CreatureScript("mod_zone_difficulty_dungeonmaster") { }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        uint32 instanceId = player->GetMap()->GetInstanceId();
        if (action == 100)
        {
            LOG_ERROR("sql.sql", "Try turn on");
            bool CanTurnOn = true;

            // Forbid turning harmode on ...
            // ...if a single encounter was completed on normal mode
            if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) != sZoneDifficulty->HardmodeInstanceData.end())
            {
                if (sZoneDifficulty->HardmodeInstanceData[instanceId].CompletedEncounterOnNormal == true)
                {
                    LOG_ERROR("sql.sql", "CompletedEncounterOnNormal");
                    CanTurnOn = false;
                    // todo: Give Feedback
                }
            }
            // ... if there is an encounter in progress
            else if (player->GetInstanceScript() && player->GetInstanceScript()->IsEncounterInProgress())
            {
                LOG_ERROR("sql.sql", "IsEncounterInProgress");
                CanTurnOn = false;
                // todo: Give Feedback
            }

            if (CanTurnOn == true)
            {
                LOG_ERROR("sql.sql", "Turn on hardmode for id {}", instanceId);
                sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn = true;
                sZoneDifficulty->SaveHardmodeInstanceData(instanceId);
                uint32 mapId = player->GetMap()->GetId();

                // does the map have any creature which is supposed to have an improved lootmode?
                if (sZoneDifficulty->HardmodeCreatureLoot.find(mapId) != sZoneDifficulty->HardmodeCreatureLoot.end())
                {
                    CloseGossipMenuFor(player);
                    return false;
                }

                // find all creatures/GOs with their entry listed in sZoneDifficulty->HardmodeCreatureLoot[mapId] and AddLootMode(64)
                std::list<Creature*> creatures;
                for (auto entry : sZoneDifficulty->HardmodeCreatureLoot[player->GetMap()->GetId()])
                {
                    player->GetCreaturesWithEntryInRange(creatures, 50000.0f, entry);
                }
                std::list<GameObject*> gameobjects;
                for (auto entry : sZoneDifficulty->HardmodeGameobjectLoot[player->GetMap()->GetId()])
                {
                    player->GetGameObjectListWithEntryInGrid(gameobjects, entry, 50000.0f);
                }

                //iterate over `creatures` and `gameobjects` and AddLootMode(64)
                for (auto creature : creatures)
                {
                    creature->AddLootMode(64);
                }
                for (auto go : gameobjects)
                {
                    go->AddLootMode(64);
                }

                // todo: Give Feedback
            }

            CloseGossipMenuFor(player);
        }
        else if (action == 101)
        {
            LOG_ERROR("sql.sql", "Turn off hardmode for id {}", instanceId);
            ZoneDifficultyHardmodeInstData data = sZoneDifficulty->HardmodeInstanceData[instanceId];
            sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn = false;
            sZoneDifficulty->SaveHardmodeInstanceData(instanceId);

            // Remove Extra Lootmode from all stored objects
            for (auto guid : sZoneDifficulty->HardmodeGameobjectsGUIDMap[instanceId])
            {
                player->GetMap()->GetGameObject(guid)->ResetLootMode();
            }
            // todo: Give Feedback
            CloseGossipMenuFor(player);
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        LOG_ERROR("sql.sql", "567");
        uint32 npctext = NPC_TEXT_OTHER;
        if (Group* group = player->GetGroup())
        {
            LOG_ERROR("sql.sql", "570");
            if (group->IsLeader(player->GetGUID()))
            {
                LOG_ERROR("sql.sql", "573");
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Please Chromie, let us re-experience how all the things really happened back then. (Hard mode)", GOSSIP_SENDER_MAIN, 100);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I think we will be fine with the cinematic version from here. (Normal mode)", GOSSIP_SENDER_MAIN, 101);
                npctext = NPC_TEXT_LEADER;
            }
            else
            {
                //todo: Leave a message that only the leader can do this
            }
        }
        SendGossipMenuFor(player, npctext, creature);
        return true;
    }
};

// Add all scripts in one
void AddModZoneDifficultyScripts()
{
    new mod_zone_difficulty_unitscript();
    new mod_zone_difficulty_playerscript();
    new mod_zone_difficulty_petscript();
    new mod_zone_difficulty_worldscript();
    new mod_zone_difficulty_globalscript();
    new mod_zone_difficulty_allcreaturescript();
    new mod_zone_difficulty_allgameobjectscript();
    new mod_zone_difficulty_dungeonmaster();
}
