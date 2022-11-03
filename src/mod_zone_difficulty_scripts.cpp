/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "ZoneDifficulty.h"

enum Spells
{
    SPELL_BEACON_OF_LIGHT     = 53652, // Holy Light... each procs a different spell...
    SPELL_BEACON_OF_LIGHT_2   = 53654, // Flash of Light, Holy Shock
    SPELL_ANCESTRAL_AWAKENING = 52752,
    SPELL_DIVINE_AEGIS        = 47753,
    SPELL_SWIFTMEND           = 18562,
    SPELL_ANDOROV_HEALING     = 25516,
    SPELL_GLYPH_OF_PWS        = 56160
};

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

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_info"))
    {
        do
        {
            uint32 mapId = (*result)[0].Get<uint32>();
            if (!sMapStore.LookupEntry(mapId))
            {
                LOG_ERROR("modules", "ModZoneDifficulty: Invalid map issued. Map {} does not exist. Skipping.", (*result)[0].Get<uint32>());
                continue;
            }

            ZoneDifficultyData data;
            data.HealingNerfPct = (*result)[1].Get<float>();
            data.AbsorbNerfPct = (*result)[2].Get<float>();
            data.Enabled = (*result)[3].Get<bool>();
            sZoneDifficulty->ZoneDifficultyInfo[mapId] = data;

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
            uint32 mapId = target->GetMapId();
            if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
            {
                if (SpellInfo const* spellInfo = aura->GetSpellInfo())
                {
                    if (spellInfo->Id == SPELL_DIVINE_AEGIS)
                    {
                        return;
                    }

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

                            int32 absorb = eff->GetAmount() * sZoneDifficulty->ZoneDifficultyInfo[target->GetMapId()].HealingNerfPct;
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
                switch (spellInfo->Id)
                {
                    // Don't apply reductions to those spells, as they are procs
                    // and the source spell is already nerfed.
                    case SPELL_BEACON_OF_LIGHT:
                    case SPELL_BEACON_OF_LIGHT_2:
                    case SPELL_ANCESTRAL_AWAKENING:
                    case SPELL_SWIFTMEND:
                    case SPELL_ANDOROV_HEALING: // Don't nerf this one so we don't alter the usual strategy.
                        return;
                    default:
                        break;
                }

                // Skip spells not affected by vulnerability (potions) and bandages
                if (spellInfo->HasAttribute(SPELL_ATTR0_NO_IMMUNITIES) || spellInfo->Mechanic == MECHANIC_BANDAGE)
                {
                    return;
                }
            }

            uint32 mapId = target->GetMapId();
            if (sZoneDifficulty->ZoneDifficultyInfo.find(mapId) != sZoneDifficulty->ZoneDifficultyInfo.end())
            {
                if (sZoneDifficulty->ZoneDifficultyInfo[target->GetMapId()].Enabled)
                {
                    heal = heal * sZoneDifficulty->ZoneDifficultyInfo[target->GetMapId()].HealingNerfPct;
                }
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
