/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Config.h"
#include "Chat.h"
#include "MapMgr.h"
#include "Pet.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "StringConvert.h"
#include "TaskScheduler.h"
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
            int8 mode = (*result)[6].Get<int8>();
            if (mode == MODE_NORMAL)
            {
                data.HealingNerfPct = (*result)[2].Get<float>();
                data.AbsorbNerfPct = (*result)[3].Get<float>();
                data.MeleeDamageBuffPct = (*result)[4].Get<float>();
                data.SpellDamageBuffPct = (*result)[5].Get<float>();
                data.Enabled = data.Enabled | mode;
                sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][phaseMask] = data;
            }
            else if (mode == MODE_HARD)
            {
                data.HealingNerfPctHard = (*result)[2].Get<float>();
                data.AbsorbNerfPctHard = (*result)[3].Get<float>();
                data.MeleeDamageBuffPctHard = (*result)[4].Get<float>();
                data.SpellDamageBuffPctHard = (*result)[5].Get<float>();
                data.Enabled = data.Enabled | mode;
                sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][phaseMask] = data;
            }

            // duels do not check for phases. Only 0 is allowed.
            if (mapId == DUEL_INDEX && phaseMask != 0)
            {
                LOG_INFO("sql", "Table `zone_difficulty_info` for criteria (duel mapId: {}) has wrong value ({}), must be 0 for duels.", mapId, phaseMask);
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
                        LOG_INFO("sql", "Disabling buffs for spell '{}' is invalid, skipped.", spell);
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
            ZoneDifficultyLootableObjects data;
            uint32 mapId = (*result)[0].Get<uint32>();
            data.SourceEntry = (*result)[1].Get<uint32>();
            data.GameobjectEntry= (*result)[2].Get<uint32>();

            sZoneDifficulty->HardmodeLoot[mapId].push_back(data);
            LOG_INFO("sql", "New creature for map {} with entry: {}", mapId, data.SourceEntry);

            if (mapId <= 0 || data.SourceEntry <= 0)
            {
                LOG_INFO("sql", "Table `zone_difficulty_loot_objects` for criteria MapId: {} OR Entry: {} has wrong value. Must be > 0.", mapId, data.SourceEntry);
            }

        } while (result->NextRow());
    }
}

/* Loads the HardmodeInstanceData from the database.
*  Fetch from zone_difficulty_instance_saves.
*
*  `InstanceID` INT NOT NULL DEFAULT 0,
*  `HardmodeOn` TINYINT NOT NULL DEFAULT 0,
*  `HardmodePossible` TINYINT NOT NULL DEFAULT 1,
*
*  Exclude data not in the IDs stored in GetInstanceIDs() and delete
*  zone_difficulty_instance_saves for instances that no longer exist.
*/
void ZoneDifficulty::LoadHardmodeInstanceData()
{
    // debugging
    std::vector<bool> instanceIDs = sMapMgr->GetInstanceIDs();
    for (int i = 0; i < int(instanceIDs.size()); i++)
    {
        LOG_INFO("sql", "ZoneDifficulty::LoadHardmodeInstanceData: id {} exists: {}:", i, instanceIDs[i]);
    }
    //end debugging
    if (QueryResult result = CharacterDatabase.Query("SELECT * FROM zone_difficulty_instance_saves"))
    {
        do
        {
            uint32 instanceId = (*result)[0].Get<uint32>();
            bool HardmodeOn = (*result)[1].Get<bool>();
            bool HardmodePossible = (*result)[2].Get<bool>();

            if (instanceIDs[instanceId] == true)
            {
                LOG_INFO("sql", "Loading from DB for instanceId {}: HardmodeOn = {}, HardmodePossible = {}", instanceId, HardmodeOn, HardmodePossible);
                sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn = HardmodeOn;
                sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodePossible = HardmodePossible;
            }
            else
            {
                CharacterDatabase.Execute("DELETE FROM zone_difficulty_instance_saves WHERE InstanceID = {}", instanceId);
            }


        } while (result->NextRow());
    }
}

/*  Sends a whisper to all members of the player's raid in the same instance as the creature.
 *
 * @param message The message which should be sent to the <Player>.
 * @param creature The creature who sends the whisper.
 * @param player The object of the player, whose whole group should receive the message.
 */
void ZoneDifficulty::SendWhisperToRaid(std::string message, Creature* creature, Player* player)
{
    Group::MemberSlotList const& members = player->GetGroup()->GetMemberSlots();
    for (auto member : members)
    {
        Player* mplr = ObjectAccessor::FindConnectedPlayer(member.guid);
        if (creature && mplr && mplr->GetMap()->GetInstanceId() == player->GetMap()->GetInstanceId())
        {
            creature->Whisper(message, LANG_UNIVERSAL, player);
        }
    }
}


/*  Check if the target is a player, a pet or a guardian.
 *
 * @param target The affected <Unit>
 * @return The result as bool. True for <Player>, <Pet> or <Guardian>.
 */
bool ZoneDifficulty::IsValidNerfTarget(Unit* target)
{
    return target->IsPlayer() || target->IsPet() || target->IsGuardian();
}


/*  Checks if the element is one of the values in the vector.
 *
 * @param vec A vector
 * @param element One element which can potentially be part of the values in the vector
 * @return The result as bool
 */
bool ZoneDifficulty::VectorContains(std::vector<uint32> vec, uint32 element)
{
    return find(vec.begin(), vec.end(), element) != vec.end();
}


/*  Checks if the target is in a duel while residing in the DUEL_AREA and their opponent is a valid object.
 *  Used to determine when the duel-specific nerfs should be applied.
 *
 * @param target The affected <Unit>
 * @return The result as bool
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

/*  Find the lowest phase for the target's mapId, which has a db entry for the target's map
 *  and at least partly matches the target's phase.
 *
 *  `mapId` can be the id of a map or `DUEL_INDEX` to use the duel specific settings.
 *  Return -1 if none found.
 *
 * @param mapId
 * @param phaseMask Bitmask of all phases where the unit is currently visible
 * @return the lowest phase which should be altered for this map and the unit is visible in
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

/*  Store the HardmodeInstanceData in the database for the given instance id.
 *  zone_difficulty_instance_saves is used to store the data.
 *
 *  @param InstanceID INT NOT NULL DEFAULT 0,
 *  @param HardmodeOn TINYINT NOT NULL DEFAULT 0,
 *  @param HardmodePossible TINYINT NOT NULL DEFAULT 1,
 */
void ZoneDifficulty::SaveHardmodeInstanceData(uint32 instanceId)
{
    if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) == sZoneDifficulty->HardmodeInstanceData.end())
    {
        LOG_INFO("sql", "ZoneDifficulty::SaveHardmodeInstanceData: InstanceId {} not found in HardmodeInstanceData.", instanceId);
        return;
    }

    CharacterDatabase.Execute("REPLACE INTO zone_difficulty_instance_saves (InstanceID, HardmodeOn, HardmodePossible) VALUES ({}, {}, {})", instanceId, sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn, sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodePossible);
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
                            int8 mode = sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].Enabled;
                            if (matchingPhase != -1)
                            {
                                Map* map = target->GetMap();
                                if ((mode & MODE_NORMAL) == MODE_NORMAL)
                                {
                                    absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].AbsorbNerfPct;
                                }
                                if ((mode & MODE_HARD) == MODE_HARD && sZoneDifficulty->HardmodeInstanceData[target->GetMap()->GetInstanceId()].HardmodeOn == true)
                                {
                                    if (map->IsRaid() ||
                                       (map->IsHeroic() && map->IsDungeon()))
                                    {
                                        absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].AbsorbNerfPctHard;
                                    }
                                }
                            }
                            else if (sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].Enabled > 0 && nerfInDuel)
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
                int8 mode = sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].Enabled;
                if (matchingPhase != -1)
                {
                    Map* map = target->GetMap();
                    if ((mode & MODE_NORMAL) == MODE_NORMAL)
                    {
                        heal = heal * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].HealingNerfPct;
                    }
                    if ((mode & MODE_HARD) == MODE_HARD && sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn == true)
                    {
                        if (map->IsRaid() ||
                           (map->IsHeroic() && map->IsDungeon()))
                        {
                            heal = heal * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].HealingNerfPctHard;
                        }
                    }
                }
                else if (sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].Enabled > 0 && nerfInDuel)
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
                        ChatHandler(player->GetSession()).PSendSysMessage("A dot tick will be altered. Pre Nerf Value: %i", damage);
                    }
                }
            }

            if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end() && matchingPhase != -1)
            {
                int8 mode = sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].Enabled;
                Map* map = target->GetMap();
                if ((mode & MODE_NORMAL) == MODE_NORMAL)
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPct;
                }
                if ((mode & MODE_HARD) == MODE_HARD && sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn == true)
                {
                    if (map->IsRaid() ||
                       (map->IsHeroic() && map->IsDungeon()))
                    {
                        damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPctHard;
                    }
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                if (sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].Enabled > 0)
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].SpellDamageBuffPct;
                }
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
                            ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Before Nerf Value: %i (%f Normal Mode)", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage, sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPct);
                            ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Before Nerf Value: %i (%f Hard Mode)", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage, sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPctHard);
                        }
                    }
                }
            }

            if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyNerfInfo.end() && matchingPhase != -1)
            {
                int8 mode = sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].Enabled;
                Map* map = target->GetMap();
                if ((mode & MODE_NORMAL) == MODE_NORMAL)
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPct;
                }
                if ((mode & MODE_HARD) == MODE_HARD && sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn == true)
                {
                    if (map->IsRaid() ||
                       (map->IsHeroic() && map->IsDungeon()))
                    {
                        damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].SpellDamageBuffPctHard;
                    }
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                if (sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].Enabled > 0)
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].SpellDamageBuffPct;
                }
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
                int8 mode = sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].Enabled;
                Map* map = target->GetMap();
                if ((mode & MODE_NORMAL) == MODE_NORMAL)
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].MeleeDamageBuffPct;
                }
                if ((mode & MODE_HARD) == MODE_HARD && sZoneDifficulty->HardmodeInstanceData[target->GetMap()->GetInstanceId()].HardmodeOn == true)
                {
                    if (map->IsRaid() ||
                       (map->IsHeroic() && map->IsDungeon()))
                    {
                        damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[mapId][matchingPhase].MeleeDamageBuffPctHard;
                    }
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                if (sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].Enabled > 0)
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyNerfInfo[DUEL_INDEX][0].MeleeDamageBuffPct;
                }
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
        sZoneDifficulty->LoadHardmodeInstanceData();
    }
};

class mod_zone_difficulty_globalscript : public GlobalScript
{
public:
    mod_zone_difficulty_globalscript() : GlobalScript("mod_zone_difficulty_globalscript") { }

    void OnBeforeSetBossState(uint32 id, EncounterState newState, EncounterState oldState, Map* instance)
    {
        LOG_INFO("sql", "OnBeforeSetBossState: bossId = {}, newState = {}, oldState = {}, MapId = {}, InstanceId = {}", id, newState, oldState, instance->GetId(), instance->GetInstanceId());
    }

    void OnInstanceIdRemoved(uint32 instanceId) override
    {
        LOG_INFO("sql", "OnInstanceIdRemoved: instanceId = {}", instanceId);
        if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) != sZoneDifficulty->HardmodeInstanceData.end())
        {
            sZoneDifficulty->HardmodeInstanceData.erase(instanceId);
        }

        CharacterDatabase.Execute("DELETE FROM zone_difficulty_instance_saves WHERE InstanceID = {};", instanceId);
    }

    void OnAfterUpdateEncounterState(Map* map, EncounterCreditType /*type*/, uint32 /*creditEntry*/, Unit* source, Difficulty /*difficulty_fixed*/, DungeonEncounterList const* /*encounters*/, uint32 /*dungeonCompleted*/, bool /*updated*/) override
    {
        if (!source)
        {
            LOG_INFO("sql", "source is a nullptr in OnAfterUpdateEncounterState");
            return;
        }

        if (sZoneDifficulty->HardmodeInstanceData.find(map->GetInstanceId()) != sZoneDifficulty->HardmodeInstanceData.end())
        {
            LOG_INFO("sql", "Encounter completed. Map relevant.");
            // Give additional loot, if the encounter was in hardmode.
            if (sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn == true)
            {
                uint32 mapId = map->GetId();
                uint32 GameobjectEntry = 0;
                if (sZoneDifficulty->HardmodeLoot.find(mapId) == sZoneDifficulty->HardmodeLoot.end())
                {
                    LOG_INFO("sql", "No additional loot stored in map with id {}.", map->GetInstanceId());
                    return;
                }

                //iterate over all listed creature entries for that map id and see, if a go is to be looted instead
                for (auto value : sZoneDifficulty->HardmodeLoot[mapId])
                {
                    if (value.GameobjectEntry != 0 && value.SourceEntry == source->GetEntry())
                    {
                        GameobjectEntry = value.GameobjectEntry;
                        break;
                    }
                }

                if (GameobjectEntry == 0)
                {
                    LOG_INFO("sql", "Hardmode for instance id {} is {}.", map->GetInstanceId(), sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn);
                    source->ToCreature()->AddLootMode(64);
                    source->ToCreature()->loot.clear();
                    source->ToCreature()->loot.FillLoot(source->ToCreature()->GetCreatureTemplate()->lootid, LootTemplates_Creature, source->ToCreature()->GetLootRecipient(), false, false, source->ToCreature()->GetLootMode(), source->ToCreature());
                    LOG_INFO("sql", "Encounter {} completed. Loot mode: {}", source->GetName(), source->ToCreature()->GetLootMode());
                }
                else
                {
                    GameObject* go = source->FindNearestGameObject(GameobjectEntry, 200.0f, true);
                    if (go)
                    {
                        go->AddLootMode(64);
                        go->loot.clear();
                        go->loot.FillLoot(go->GetGOInfo()->GetLootId(), LootTemplates_Gameobject, go->GetLootRecipient(), false, false, go->GetLootMode(), go);
                        LOG_INFO("sql", "Encounter {} completed. Adding loot to {} Loot mode: {}", source->GetName(), go->GetName(), source->ToCreature()->GetLootMode());
                    }
                    // if the gameobject is not spawned yet, wait one tick
                    else
                    {
                        source->m_Events.AddEventAtOffset([GameobjectEntry, source]()
                        {
                            if (!source)
                            {
                                LOG_INFO("sql", "2nd try: source is a nullptr in OnAfterUpdateEncounterState");
                                return;
                            }

                            GameObject* go = source->FindNearestGameObject(GameobjectEntry, 200.0f, true);
                            if (go)
                            {
                                go->AddLootMode(64);
                                go->loot.clear();
                                go->loot.FillLoot(go->GetGOInfo()->GetLootId(), LootTemplates_Gameobject, go->GetLootRecipient(), false, false, go->GetLootMode(), go);
                                LOG_INFO("sql", "2nd try: Encounter {} completed. Adding loot to {} Loot mode: {}", source->GetName(), go->GetName(), source->ToCreature()->GetLootMode());
                            }
                        }, 10ms);
                    }
                }
                return;
            }
            // Must set HardmodePossible to false, if the encounter wasn't in hardmode.
            else
            {
                LOG_INFO("sql", "Hardmode for instance id {} is {}. Setting HardmodePossible to false.", map->GetInstanceId(), sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn);
                sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodePossible = false;
                sZoneDifficulty->SaveHardmodeInstanceData(map->GetInstanceId());
            }
        }
    }
};

class mod_zone_difficulty_instancemapscript : public AllMapScript
{
public:
    mod_zone_difficulty_instancemapscript() : AllMapScript("mod_zone_difficulty_instancemapscript") { }

    void OnBeforeCreateInstanceScript(InstanceMap* instanceMap, InstanceScript* /*instanceData*/, bool /*load*/, std::string /*data*/, uint32 completedEncounterMask)
    {
        // debug start
        for (auto mapdata : sZoneDifficulty->HardmodeLoot)
        {
            LOG_INFO("sql", "Maps with loot data: {}", mapdata.first);
        }
        // debug end


        if (sZoneDifficulty->HardmodeLoot.find(instanceMap->GetId()) == sZoneDifficulty->HardmodeLoot.end())
        {
            LOG_INFO("sql", "New instance not handled because there is no hardmode data for map id: {}", instanceMap->GetId());
            return;
        }

        if (completedEncounterMask == 0)
        {
            LOG_INFO("sql", "Initializing instance with HardmodePossible == true for instanceId: {} with mapId: {}", instanceMap->GetInstanceId(), instanceMap->GetId());
            sZoneDifficulty->HardmodeInstanceData[instanceMap->GetInstanceId()].HardmodePossible = true;
        }
    }
};

class mod_zone_difficulty_dungeonmaster : public CreatureScript
{
public:
    mod_zone_difficulty_dungeonmaster() : CreatureScript("mod_zone_difficulty_dungeonmaster") { }

    struct mod_zone_difficulty_dungeonmasterAI : public ScriptedAI
    {
        mod_zone_difficulty_dungeonmasterAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            if (me->GetMap() && me->GetMap()->IsHeroic() && !me->GetMap()->IsRaid())
            {
                _scheduler.Schedule(15s, [this](TaskContext context)
                    {
                        me->Yell("If you want a challenge, please talk to me soon adventurer!", LANG_UNIVERSAL);
                    });
                _scheduler.Schedule(45s, [this](TaskContext context)
                    {
                        me->Yell("I will take my leave then and offer my services to other adventurers. See you again!", LANG_UNIVERSAL);
                    });
                _scheduler.Schedule(60s, [this](TaskContext context)
                    {
                        me->DespawnOrUnsummon();
                    });
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _scheduler.Update(diff);
        }

    private:
        TaskScheduler _scheduler;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mod_zone_difficulty_dungeonmasterAI(creature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        uint32 instanceId = player->GetMap()->GetInstanceId();
        if (action == 100)
        {
            LOG_INFO("sql", "Try turn on");
            bool CanTurnOn = true;

            // Forbid turning harmode on ...
            // ...if a single encounter was completed on normal mode
            if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) != sZoneDifficulty->HardmodeInstanceData.end())
            {
                if (sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodePossible == false)
                {
                    LOG_INFO("sql", "Hardmode is not Possible for instanceId {}: {}", instanceId, sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodePossible);
                    CanTurnOn = false;
                    creature->Whisper("I am sorry, time-traveler. You can not return to this version of the time-line anymore. You must complete all previous encounters the challenging way.", LANG_UNIVERSAL, player);
                }
            }
            // ... if there is an encounter in progress
            else if (player->GetInstanceScript() && player->GetInstanceScript()->IsEncounterInProgress())
            {
                LOG_INFO("sql", "IsEncounterInProgress");
                CanTurnOn = false;
                creature->Whisper("I am sorry, time-traveler. You can not return to this version of the time-line currently. There is already a battle in progress." , LANG_UNIVERSAL, player);
            }

            if (CanTurnOn == true)
            {
                LOG_INFO("sql", "Turn on hardmode for id {}", instanceId);
                sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn = true;
                sZoneDifficulty->SaveHardmodeInstanceData(instanceId);
                sZoneDifficulty->SendWhisperToRaid("We're switching to the challenging version of the history lesson now. (Hard mode)", creature, player);
            }

            CloseGossipMenuFor(player);
        }
        else if (action == 101)
        {
            LOG_INFO("sql", "Turn off hardmode for id {}", instanceId);
            sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn = false;
            sZoneDifficulty->SaveHardmodeInstanceData(instanceId);
            sZoneDifficulty->SendWhisperToRaid("We're switching to the cinematic version of the history lesson now. (Normal mode)", creature, player);
            CloseGossipMenuFor(player);
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        LOG_INFO("sql", "OnGossipHello");
        uint32 npctext = NPC_TEXT_OTHER;
        if (Group* group = player->GetGroup())
        {
            LOG_INFO("sql", "OnGossipHello Has Group");
            if (group->IsLeader(player->GetGUID()))
            {
                LOG_INFO("sql", "OnGossipHello Is Leader");
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Please Chromie, let us re-experience how all the things really happened back then. (Hard mode)", GOSSIP_SENDER_MAIN, 100);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I think we will be fine with the cinematic version from here. (Normal mode)", GOSSIP_SENDER_MAIN, 101);

                if (sZoneDifficulty->HardmodeInstanceData[player->GetMap()->GetInstanceId()].HardmodeOn == true)
                {
                    npctext = NPC_TEXT_LEADER_HARD;
                }
                else
                {
                    npctext = NPC_TEXT_LEADER_NORMAL;
                }

            }
            else
            {
                creature->Whisper("I will let the leader of your group decide about this subject. You will receive a notification, when they make a new request to me.", LANG_UNIVERSAL, player);
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
    new mod_zone_difficulty_instancemapscript();
    new mod_zone_difficulty_dungeonmaster();
}
