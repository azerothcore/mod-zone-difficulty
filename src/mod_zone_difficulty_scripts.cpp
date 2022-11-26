/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
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
    ZoneDifficultyInfo[DUEL_INDEX][0].HealingNerfPct = 1;
    ZoneDifficultyInfo[DUEL_INDEX][0].AbsorbNerfPct = 1;
    ZoneDifficultyInfo[DUEL_INDEX][0].MeleeDamageBuffPct = 1;
    ZoneDifficultyInfo[DUEL_INDEX][0].SpellDamageBuffPct = 1;

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_info"))
    {
        do
        {
            uint32 mapId = (*result)[0].Get<uint32>();
            uint32 phaseMask = (*result)[1].Get<uint32>();
            ZoneDifficultyData data;
            data.HealingNerfPct = (*result)[2].Get<float>();
            data.AbsorbNerfPct = (*result)[3].Get<float>();
            data.MeleeDamageBuffPct = (*result)[4].Get<float>();
            data.SpellDamageBuffPct = (*result)[5].Get<float>();
            data.Enabled = (*result)[6].Get<bool>();
            sZoneDifficulty->ZoneDifficultyInfo[mapId][phaseMask] = data;

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
                sZoneDifficulty->SpellNerfOverrides[(*result)[0].Get<uint32>()] = (*result)[1].Get<uint32>();
            }

        } while (result->NextRow());
    }
}

bool ZoneDifficulty::IsValidNerfTarget(Unit* target)
{
    return target->IsPlayer() || target->IsPet() || target->IsGuardian();
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
    if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
    {

        // Check if 0 is assigned as a phase to cover all phases
        if (sZoneDifficulty->ZoneDifficultyInfo[mapId].find(0) != sZoneDifficulty->ZoneDifficultyInfo[mapId].end())
        {
            return 0;
        }

        // Check all $key in [mapId][$key] if they match the target's visible phases
        for (auto const& [key, value] : sZoneDifficulty->ZoneDifficultyInfo[mapId])
        {
            if (key & phaseMask)
            {
                return key;
            }
        }
    }
    return -1;
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
                if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end() || nerfInDuel)
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

                                int32 absorb;
                                uint32 phaseMask = target->GetPhaseMask();
                                int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
                                if (sZoneDifficulty->ZoneDifficultyInfo[mapId][matchingPhase].Enabled && matchingPhase != -1)
                                {
                                    absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyInfo[mapId][matchingPhase].AbsorbNerfPct;
                                }
                                else if (nerfInDuel)
                                {
                                    absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX][0].AbsorbNerfPct;
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

    void ModifyHealReceived(Unit* target, Unit* /*healer*/ , uint32& heal, SpellInfo const* spellInfo) override
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
            if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end() || sZoneDifficulty->ShouldNerfInDuels(target))
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
                if (sZoneDifficulty->ZoneDifficultyInfo[mapId][matchingPhase].Enabled && matchingPhase != -1)
                {
                    heal = heal * sZoneDifficulty->ZoneDifficultyInfo[mapId][matchingPhase].HealingNerfPct;
                }
                else if (nerfInDuel)
                {
                    heal = heal * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX][0].HealingNerfPct;
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
                            ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Before Nerf Value: %i (%f)", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage, sZoneDifficulty->ZoneDifficultyInfo[mapId][matchingPhase].SpellDamageBuffPct);
                        }
                    }
                }
            }

            if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end() && matchingPhase != -1)
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyInfo[mapId][matchingPhase].SpellDamageBuffPct;
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX][0].SpellDamageBuffPct;
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
            if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end() && matchingPhase != -1)
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyInfo[mapId][matchingPhase].MeleeDamageBuffPct;
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX][0].MeleeDamageBuffPct;
            }
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

// Add all scripts in one
void AddModZoneDifficultyScripts()
{
    new mod_zone_difficulty_unitscript();
    new mod_zone_difficulty_worldscript();
}
