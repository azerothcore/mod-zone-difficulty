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
    ZoneDifficultyInfo[DUEL_INDEX].HealingNerfPct = 1;
    ZoneDifficultyInfo[DUEL_INDEX].AbsorbNerfPct = 1;
    ZoneDifficultyInfo[DUEL_INDEX].MeleeDamageBuffPct = 1;
    ZoneDifficultyInfo[DUEL_INDEX].SpellDamageBuffPct = 1;

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_info"))
    {
        do
        {
            uint32 mapId = (*result)[0].Get<uint32>();
            ZoneDifficultyData data;
            data.HealingNerfPct = (*result)[1].Get<float>();
            data.AbsorbNerfPct = (*result)[2].Get<float>();
            data.MeleeDamageBuffPct = (*result)[3].Get<float>();
            data.SpellDamageBuffPct = (*result)[4].Get<float>();
            data.Enabled = (*result)[5].Get<bool>();
            sZoneDifficulty->ZoneDifficultyInfo[mapId] = data;

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
            float absorb = 1;
            uint32 mapId = target->GetMapId();
            //if the player who is responsible for the target is in a duel, apply values for PvP
            if (target->GetAffectingPlayer()->duel && target->GetAffectingPlayer()->duel->State == DUEL_STATE_IN_PROGRESS)
            {
                absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyInfo[DUEL_INDEX].HealingNerfPct;
            }
            else if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
            {
                absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyInfo[mapId].HealingNerfPct;
            }

            if !absorb = 1
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
                        std::list<AuraEffect*> AuraEffectList  = target->GetAuraEffectsByType(SPELL_AURA_SCHOOL_ABSORB);

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
            //if the player who is responsible for the target is in a duel, apply values for PvP
            else if (target->GetAffectingPlayer()->duel && target->GetAffectingPlayer()->duel->State == DUEL_STATE_IN_PROGRESS))
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

        if (sZoneDifficulty->IsValidNerfTarget(target) && !attacker->IsPlayer())
        {
            if (spellInfo)
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
            //if the player who is responsible for the target is in a duel, apply values for PvP
            else if (target->GetAffectingPlayer()->duel && target->GetAffectingPlayer()->duel->State == DUEL_STATE_IN_PROGRESS))
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
            //if the player who is responsible for the target is in a duel, apply values for PvP
            else if (target->GetAffectingPlayer()->duel && target->GetAffectingPlayer()->duel->State == DUEL_STATE_IN_PROGRESS))
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
