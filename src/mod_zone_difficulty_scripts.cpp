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

bool ZoneDifficulty::ShouldNerfAbsorb(uint32 mapId, Unit* target)
{
    if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
    {
        uint32 phaseMask = target->GetPhaseMask()
        //iterate over all values in the key [mapId][$a]
        for_each (ZoneDifficultyInfo.begin, ZoneDifficultyInfo.end, [](uint32 a)
        {
            if (a & phaseMask)
            {
                return true;
            }
        }
    }
    else if (target->GetAffectingPlayer()->duel && target->GetAffectingPlayer()->duel->State == DUEL_STATE_IN_PROGRESS)
    {
        //iterate over all values in the key [mapId][$a]
        for_each (ZoneDifficultyInfo.begin, ZoneDifficultyInfo.end, [](uint32 a)
        {
            if (a & phaseMask)
            {
                return true;
            }
        }
    }
    return false;
}

/*
    Check if the target is in a duel while residing in the DUEL_AREA and their opponent is a valid object.
    Used to determine when the duel-specific nerfs should be applied.
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
            if (sZoneDifficulty->ShouldNerfAbsorb(mapId, target))
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
                                        ChatHandler(target->ToPlayer()->GetSession()).PSendSysMessage("Spell: %s (%u) Base Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, eff->GetAmount());
                                    }
                                }
                            }

                            int32 absorb = 0;
                            if (sZoneDifficulty->ShouldNerfInDuels(target))
                            {
                                absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX].HealingNerfPct;
                            }

                            else if (sZoneDifficulty->SpellNerfOverrides.find(mapId) != sZoneDifficulty->SpellNerfOverrides.end())
                            {
                                absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyInfo[mapId].HealingNerfPct;
                            }

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
                                        ChatHandler(target->ToPlayer()->GetSession()).PSendSysMessage("Spell: %s (%u) Post Nerf Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, eff->GetAmount());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    void ModifyHealReceived(Unit* target, Unit* healer, uint32& heal, SpellInfo const* spellInfo) override
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
            if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
            {
                if (sZoneDifficulty->ZoneDifficultyInfo[mapId].Enabled)
                {
                    if (spellInfo)
                    {
                        if (sZoneDifficulty->SpellNerfOverrides.find(mapId) != sZoneDifficulty->SpellNerfOverrides.end())
                        {
                            heal = heal * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id];
                            return;
                        }
                    }

                    heal = heal * sZoneDifficulty->ZoneDifficultyInfo[mapId].HealingNerfPct;
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                heal = heal * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX].HealingNerfPct;
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
            if (spellInfo && !attacker->IsPlayer())
            {
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
                            ChatHandler(target->ToPlayer()->GetSession()).PSendSysMessage("Spell: %s (%u) Before Nerf Value: %i (%f)", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage, sZoneDifficulty->ZoneDifficultyInfo[target->GetMapId()].SpellDamageBuffPct);
                        }
                    }
                }
            }

            if (!attacker->IsPlayer())
            {
                uint32 mapId = target->GetMapId();
                if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyInfo[mapId].SpellDamageBuffPct;
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX].SpellDamageBuffPct;
            }

            if (sZoneDifficulty->IsDebugInfoEnabled)
            {
                if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                {
                    if (player->GetSession())
                    {
                        ChatHandler(target->ToPlayer()->GetSession()).PSendSysMessage("Spell: %s (%u) Post Nerf Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage);
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
            if (!attacker->IsPlayer())
            {
                uint32 mapId = target->GetMapId();
                if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
                {
                    damage = damage * sZoneDifficulty->ZoneDifficultyInfo[mapId].MeleeDamageBuffPct;
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                damage = damage * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX].MeleeDamageBuffPct;
            }
        }
    }
};

class mod_zone_difficulty_worldscript : public WorldScript
{
public:
    mod_zone_difficulty_worldscript() : WorldScript("mod_zone_difficulty_worldscript") { }

    void OnAfterConfigLoad(bool reload) override
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
