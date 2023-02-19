/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Config.h"
#include "Chat.h"
#include "ItemTemplate.h"
#include "MapMgr.h"
#include "Pet.h"
#include "Player.h"
#include "PoolMgr.h"
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

    sZoneDifficulty->ZoneDifficultyRewards.clear();

    // Default values for when there is no entry in the db for duels (index 0xFFFFFFFF)
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].HealingNerfPct = 1;
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].AbsorbNerfPct = 1;
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].MeleeDamageBuffPct = 1;
    ZoneDifficultyNerfInfo[DUEL_INDEX][0].SpellDamageBuffPct = 1;

    // Heroic Quest -> MapId Translation
    HeroicQuestMapList[542] = 11362;
    HeroicQuestMapList[543] = 11354;
    HeroicQuestMapList[547] = 11368;
    HeroicQuestMapList[546] = 11369;
    HeroicQuestMapList[557] = 11373;
    HeroicQuestMapList[558] = 11374;
    HeroicQuestMapList[560] = 11378;
    HeroicQuestMapList[556] = 11372;
    HeroicQuestMapList[585] = 11499;
    HeroicQuestMapList[555] = 11375;
    HeroicQuestMapList[540] = 11363;
    HeroicQuestMapList[552] = 11388;
    HeroicQuestMapList[269] = 11382;
    HeroicQuestMapList[553] = 11384;
    HeroicQuestMapList[554] = 11386;
    HeroicQuestMapList[545] = 11370;

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

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_instance_data"))
    {
        do
        {
            ZoneDifficultyHardmodeMapData data;
            uint32 MapID = (*result)[0].Get<uint32>();
            data.EncounterEntry = (*result)[1].Get<uint32>();
            data.OverrideGO = (*result)[2].Get<uint32>();
            data.RewardType = (*result)[3].Get<uint8>();

            sZoneDifficulty->HardmodeLoot[MapID].push_back(data);
            LOG_INFO("sql", "New creature for map {} with entry: {}", MapID, data.EncounterEntry);

            Expansion[MapID] = data.RewardType;

        } while (result->NextRow());
    }

    if (QueryResult result = WorldDatabase.Query("SELECT entry FROM `pool_quest` WHERE `pool_entry`=356"))
    {
        do
        {
            sZoneDifficulty->DailyHeroicQuests.push_back((*result)[0].Get<uint32>());
            LOG_INFO("sql", "Adding daily heroic quest with id {}.", (*result)[0].Get<uint32>());
        } while (result->NextRow());
    }

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_creatureoverrides"))
    {
        do
        {
            uint32 creatureentry = (*result)[0].Get<uint32>();
            float hpmodifier = (*result)[1].Get<float>();
            bool enabled = (*result)[2].Get<bool>();

            if (enabled)
            {
                sZoneDifficulty->ZoneDifficultyCreatureOverrides[creatureentry] = hpmodifier;
                LOG_INFO("sql", "New creature with entry: {} has exception for hp: {}", creatureentry, hpmodifier);
            }
        } while (result->NextRow());
    }

    if (QueryResult result = WorldDatabase.Query("SELECT ContentType, ItemType, Entry, Price, Enchant, EnchantSlot, Enabled FROM zone_difficulty_hardmode_rewards"))
    {
        // debug
        uint32 i = 0;
        // end debug
        do
        {
            // debug
            ++i;
            // end debug
            ZoneDifficultyRewardData data;
            uint32 contenttype = (*result)[0].Get<uint32>();
            uint32 itemtype = (*result)[1].Get<uint32>();
            data.Entry = (*result)[2].Get<uint32>();
            data.Price = (*result)[3].Get<uint32>();
            data.Enchant = (*result)[4].Get<uint32>();
            data.EnchantSlot = (*result)[5].Get<uint8>();
            bool enabled = (*result)[6].Get<bool>();

            if (enabled)
            {
                sZoneDifficulty->ZoneDifficultyRewards[contenttype][itemtype].push_back(data);
                LOG_INFO("sql", "Loading item with entry {} has enchant {} in slot {}.", data.Entry, data.Enchant, data.EnchantSlot);
            }
            LOG_INFO("sql", "Total items in vector: {}.", i);
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
            uint32 InstanceId = (*result)[0].Get<uint32>();
            bool HardmodeOn = (*result)[1].Get<bool>();
            bool HardmodePossible = (*result)[2].Get<bool>();

            if (instanceIDs[InstanceId] == true)
            {
                LOG_INFO("sql", "Loading from DB for instanceId {}: HardmodeOn = {}, HardmodePossible = {}", InstanceId, HardmodeOn, HardmodePossible);
                sZoneDifficulty->HardmodeInstanceData[InstanceId].HardmodeOn = HardmodeOn;
                sZoneDifficulty->HardmodeInstanceData[InstanceId].HardmodePossible = HardmodePossible;
            }
            else
            {
                CharacterDatabase.Execute("DELETE FROM zone_difficulty_instance_saves WHERE InstanceID = {}", InstanceId);
            }


        } while (result->NextRow());
    }
}

/* @brief Loads the score data from the database.
*  Fetch from zone_difficulty_hardmode_score.
*
*  `CharacterGuid` INT NOT NULL DEFAULT 0,
*  `Type` TINYINT NOT NULL DEFAULT 0,
*  `Score` INT NOT NULL DEFAULT 0,
**
*/
void ZoneDifficulty::LoadHardmodeScoreData()
{
    if (QueryResult result = CharacterDatabase.Query("SELECT * FROM zone_difficulty_hardmode_score"))
    {
        do
        {
            uint32 GUID = (*result)[0].Get<uint32>();
            uint8 Type = (*result)[1].Get<uint8>();
            uint32 Score = (*result)[2].Get<uint32>();

            LOG_INFO("sql", "Loading from DB for player with GUID {}: Type = {}, Score = {}", GUID, Type, Score);
            sZoneDifficulty->ZoneDifficultyHardmodeScore[GUID][Type] = Score;

        } while (result->NextRow());
    }
}

/* @brief Sends a whisper to all members of the player's raid in the same instance as the creature.
 *
 * @param message The message which should be sent to the <Player>.
 * @param creature The creature who sends the whisper.
 * @param player The object of the player, whose whole group should receive the message.
 */
void ZoneDifficulty::SendWhisperToRaid(std::string message, Creature* creature, Player* player)
{
    Group::MemberSlotList const& members = player->GetGroup()->GetMemberSlots();
    for (auto& member : members)
    {
        Player* mplr = ObjectAccessor::FindConnectedPlayer(member.guid);
        if (creature && mplr && mplr->GetMap()->GetInstanceId() == player->GetMap()->GetInstanceId())
        {
            LOG_INFO("sql", "Player {} should receive a whisper.", mplr->GetName());
            creature->Whisper(message, LANG_UNIVERSAL, mplr);
        }
    }
}

std::string ZoneDifficulty::GetItemTypeString(uint32 type)
{
    std::string typestring;
    switch (type)
    {
    case ITEMTYPE_MISC:
        typestring = "Back, Finger, Neck, and Trinket";
        break;
    case ITEMTYPE_CLOTH:
        typestring = "Cloth";
        break;
    case ITEMTYPE_LEATHER:
        typestring = "Leather";
        break;
    case ITEMTYPE_MAIL:
        typestring = "Mail";
        break;
    case ITEMTYPE_PLATE:
        typestring = "Plate";
        break;
    case ITEMTYPE_WEAPONS:
        typestring = "Weapons and Shields";
        break;
    default:
        LOG_ERROR("sql", "Unknown type {} in ZoneDifficulty::GetItemTypeString.", type);
    }
    return typestring;
}

std::string ZoneDifficulty::GetContentTypeString(uint32 type)
{
    std::string typestring;
    switch (type)
    {
    case TYPE_VANILLA:
        typestring = "for Vanilla dungeons.";
        break;
    case TYPE_RAID_MC:
        typestring = "for Molten Core.";
        break;
    case TYPE_RAID_ONY:
        typestring = "for Onyxia.";
        break;
    case TYPE_RAID_BWL:
        typestring = "for Blackwing Lair.";
        break;
    case TYPE_RAID_ZG:
        typestring = "for Zul Gurub.";
        break;
    case TYPE_RAID_AQ20:
        typestring = "for Ruins of Ahn'Qiraj.";
        break;
    case TYPE_RAID_AQ40:
        typestring = "for Temple of Ahn'Qiraj.";
        break;
    case TYPE_HEROIC_TBC:
        typestring = "for Heroic TBC dungeons.";
        break;
    case TYPE_RAID_T4:
        typestring = "for T4 Raids.";
        break;
    case TYPE_RAID_T5:
        typestring = "for T5 Raids.";
        break;
    case TYPE_RAID_T6:
        typestring = "for T6 Raids.";
        break;
    case TYPE_HEROIC_WOTLK:
        typestring = "for Heroic WotLK dungeons.";
        break;
    case TYPE_RAID_T7:
        typestring = "for T7 Raids.";
        break;
    case TYPE_RAID_T8:
        typestring = "for T8 Raids.";
        break;
    case TYPE_RAID_T9:
        typestring = "for T9 Raids.";
        break;
    case TYPE_RAID_T10:
        typestring = "for T10 Raids.";
        break;
    default:
        typestring = "-";
    }
    return typestring;
}

/* @brief Grants every player in the group one score for the hardmode.
 *
 * @param map The map where the player is currently.
 * @param type The type of instance the score is awarded for.
 */
void ZoneDifficulty::AddHardmodeScore(Map* map, uint32 type)
{
    if (!map || type > 255)
    {
        LOG_ERROR("sql", "No object for map or wrong value for type: {} in AddHardmodeScore.", type);
        return;
    }
    LOG_INFO("sql", "Called AddHardmodeScore for map id: {} and type: {}", map->GetId(), type);
    Map::PlayerList const& PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
    {
        Player* player = i->GetSource();
        if (sZoneDifficulty->ZoneDifficultyHardmodeScore.find(player->GetGUID().GetCounter()) == sZoneDifficulty->ZoneDifficultyHardmodeScore.end())
        {
            sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type] = 1;
        }
        else if (sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()].find(type) == sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()].end())
        {
            sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type] = 1;
        }
        else
        {
            sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type] = sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type] + 1;
        }
        LOG_INFO("sql", "Player {} new score: {}", player->GetName(), sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type]);

        std::string typestring = sZoneDifficulty->GetContentTypeString(type);
        ChatHandler(player->GetSession()).PSendSysMessage("You have received hardmode score %s New score: %i", typestring, sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type]);
        CharacterDatabase.Execute("REPLACE INTO zone_difficulty_hardmode_score VALUES({}, {}, {})", player->GetGUID().GetCounter(), type, sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type]);
    }
}

/* @brief Reduce the score of players when they pay for rewards.
 *
 * @param player The one who pays with their score.
 * @param type The type of instance the score is deducted for.
 */
void DeductHardmodeScore(Player* player, uint32 type, uint32 score)
{
    // NULL check happens in the calling function
    LOG_INFO("sql", "Reducing score with type {} from player with guid {} by {}.", type, player->GetGUID().GetCounter(), score);
    sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type] = sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][type] - score;
}

/* @brief Send and item to the player using the data from ZoneDifficultyRewards.
 *
 * @param player The recipient of the mail.
 * @param category The content level e.g. TYPE_HEROIC_TBC.
 * @param itemtype The type of the item e.g. ITEMTYPE_CLOTH.
 * @param id the id in the vector.
 */
void SendItem(Player* player, uint32 category, uint32 itemtype, uint32 id)
{
    ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][id].Entry);
    if (!itemTemplate)
    {
        LOG_ERROR("sql", "itemTemplate could not be constructed in sZoneDifficulty->SendItem for category {}, itemtype {}, id {}.", category, itemtype, id);
        return;
    }

    ObjectGuid::LowType senderGuid = player->GetGUID().GetCounter();

    // fill mail
    MailDraft draft(REWARD_MAIL_SUBJECT, REWARD_MAIL_BODY);
    MailSender sender(MAIL_NORMAL, senderGuid, MAIL_STATIONERY_GM);
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    if (Item* item = Item::CreateItem(sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][id].Entry, 1, player))
    {
        if (sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][id].EnchantSlot != 0 && sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][id].Enchant != 0)
        {
            item->SetEnchantment(EnchantmentSlot(sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][id].EnchantSlot), sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][id].Enchant, 0, 0, player->GetGUID());
            player->ApplyEnchantment(item, EnchantmentSlot(sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][id].EnchantSlot), true, true, true);
        }
        item->SaveToDB(trans); // save for prevent lost at next mail load, if send fail then item will deleted
        draft.AddItem(item);
    }
    draft.SendMailTo(trans, MailReceiver(player, senderGuid), sender);
    CharacterDatabase.CommitTransaction(trans);
}

/* @brief Check if the map has assigned any data to tune it.
 *
 * @param map The ID of the <Map> to check.
 * @return The result as bool.
 */
bool ZoneDifficulty::IsHardmodeMap(uint32 mapid)
{
    if (sZoneDifficulty->HardmodeLoot.find(mapid) == sZoneDifficulty->HardmodeLoot.end())
    {
        return false;
    }
    return true;
}

/* @brief Check if the target is a player, a pet or a guardian.
 *
 * @param target The affected <Unit>
 * @return The result as bool. True for <Player>, <Pet> or <Guardian>.
 */
bool ZoneDifficulty::IsValidNerfTarget(Unit* target)
{
    return target->IsPlayer() || target->IsPet() || target->IsGuardian();
}

/* @brief Checks if the element is one of the values in the vector.
 *
 * @param vec A vector
 * @param element One element which can potentially be part of the values in the vector
 * @return The result as bool
 */
bool ZoneDifficulty::VectorContains(std::vector<uint32> vec, uint32 element)
{
    return find(vec.begin(), vec.end(), element) != vec.end();
}

/* @brief Checks if the target is in a duel while residing in the DUEL_AREA and their opponent is a valid object.
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

/* @brief Find the lowest phase for the target's mapId, which has a db entry for the target's map
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

/* @brief Store the HardmodeInstanceData in the database for the given instance id.
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
        sZoneDifficulty->HardmodeHpModifier = sConfigMgr->GetOption<float>("ModZoneDifficulty.Hardmode.HpModifier", 2);
        sZoneDifficulty->LoadMapDifficultySettings();
    }

    void OnStartup() override
    {
        sZoneDifficulty->LoadHardmodeInstanceData();
        sZoneDifficulty->LoadHardmodeScoreData();
    }
};

class mod_zone_difficulty_globalscript : public GlobalScript
{
public:
    mod_zone_difficulty_globalscript() : GlobalScript("mod_zone_difficulty_globalscript") { }

    void OnBeforeSetBossState(uint32 id, EncounterState newState, EncounterState oldState, Map* instance) override
    {
        LOG_INFO("sql", "OnBeforeSetBossState: bossId = {}, newState = {}, oldState = {}, MapId = {}, InstanceId = {}", id, newState, oldState, instance->GetId(), instance->GetInstanceId());
        if (!sZoneDifficulty->IsHardmodeMap(instance->GetId()))
        {
            LOG_INFO("sql", "OnBeforeSetBossState: Instance not handled because there is no hardmode loot data for map id: {}", instance->GetId());
            return;
        }
        if (oldState == 1 && newState == 3)
        {
            sZoneDifficulty->HardmodeInstanceData[instance->GetId()].HardmodePossible = false;
            if (sZoneDifficulty->HardmodeInstanceData[instance->GetId()].HardmodeOn == true)
            {
                for (auto player : instance->GetPlayers())
                {
                    //todo: Send to all players
                }

            }
        }
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
            LOG_INFO("sql", "Encounter completed. Map relevant. Checking for source: {}", source->GetEntry());
            // Give additional loot, if the encounter was in hardmode.
            if (sZoneDifficulty->HardmodeInstanceData[map->GetInstanceId()].HardmodeOn == true)
            {
                uint32 mapId = map->GetId();
                if (!sZoneDifficulty->IsHardmodeMap(mapId))
                {
                    LOG_INFO("sql", "No additional loot stored in map with id {}.", map->GetInstanceId());
                    return;
                }

                bool SourceAwardsHardmodeLoot = false;
                //iterate over all listed creature entries for that map id and see, if the encounter should yield hardmode loot and if a go is to be looted instead
                for (auto value : sZoneDifficulty->HardmodeLoot[mapId])
                {
                    if (value.EncounterEntry == source->GetEntry())
                    {
                        SourceAwardsHardmodeLoot = true;
                        break;
                    }
                }

                if (!SourceAwardsHardmodeLoot)
                {
                    return;
                }

                if (map->IsHeroic() && map->IsNonRaidDungeon())
                {
                    sZoneDifficulty->AddHardmodeScore(map, sZoneDifficulty->Expansion[mapId]);
                }
                else if (map->IsRaid())
                {
                    sZoneDifficulty->AddHardmodeScore(map, sZoneDifficulty->Expansion[mapId]);
                }
                else
                {
                    LOG_INFO("sql", "Map with id {} is not a raid or a dungeon. Hardmode loot not granted.", map->GetInstanceId());
                }
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


        if (!sZoneDifficulty->IsHardmodeMap(instanceMap->GetId()))
        {
            LOG_INFO("sql", "New instance not handled because there is no hardmode loot data for map id: {}", instanceMap->GetId());
            return;
        }

        if (completedEncounterMask == 0)
        {
            LOG_INFO("sql", "Initializing instance with HardmodePossible == true for instanceId: {} with mapId: {}", instanceMap->GetInstanceId(), instanceMap->GetId());
            sZoneDifficulty->HardmodeInstanceData[instanceMap->GetInstanceId()].HardmodePossible = true;
        }
    }
};

class mod_zone_difficulty_rewardnpc : public CreatureScript
{
public:
    mod_zone_difficulty_rewardnpc() : CreatureScript("mod_zone_difficulty_rewardnpc") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        LOG_INFO("sql", "OnGossipSelectRewardNpc action: {}", action);
        ClearGossipMenuFor(player);
        uint32 npctext;
        if (action == 999999)
        {
            for (int i = 1; i <= 16; ++i)
            {
                std::string whisper;
                whisper.append("You score is ");
                if (sZoneDifficulty->ZoneDifficultyHardmodeScore.find(player->GetGUID().GetCounter()) == sZoneDifficulty->ZoneDifficultyHardmodeScore.end())
                {
                    whisper.append("0 ");
                }
                else if (sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()].find(i) == sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()].end())
                {
                    whisper.append("0 ");
                }
                else
                {
                    whisper.append(std::to_string(sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][i])).append(" ");
                }
                whisper.append(sZoneDifficulty->GetContentTypeString(i));
                creature->Whisper(whisper, LANG_UNIVERSAL, player);
            }
        }
        // player has selected a content type
        else if (action < 100)
        {
            npctext = NPC_TEXT_CATEGORY;
            for (auto& itemtype : sZoneDifficulty->ZoneDifficultyRewards[action])
            {
                LOG_INFO("sql", "typedata.first is {}", itemtype.first);
                std::string gossip;
                std::string typestring = sZoneDifficulty->GetItemTypeString(itemtype.first);
                gossip.append("I am interested in ").append(typestring).append(" items.");
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, gossip, GOSSIP_SENDER_MAIN, itemtype.first + (action * 100));
            }
        }
        else if (action < 1000)
        {
            npctext = NPC_TEXT_ITEM;
            uint32 category = 0;
            uint32 counter = action;
            while (counter > 99)
            {
                ++category;
                counter = counter - 100;
            }
            LOG_INFO("sql", "Building gossip with category {} and counter {}", category, counter);
            auto rewards = sZoneDifficulty->ZoneDifficultyRewards[category][counter];

            for (int i = 0; i < sZoneDifficulty->ZoneDifficultyRewards[category][counter].size(); ++i)
            {
                LOG_INFO("sql", "Adding gossip option for entry {}", sZoneDifficulty->ZoneDifficultyRewards[category][counter][i].Entry);
                ItemTemplate const* proto = sObjectMgr->GetItemTemplate(sZoneDifficulty->ZoneDifficultyRewards[category][counter][i].Entry);
                std::string name = proto->Name1;
                if (ItemLocale const* leftIl = sObjectMgr->GetItemLocale(sZoneDifficulty->ZoneDifficultyRewards[category][counter][i].Entry))
                {
                    ObjectMgr::GetLocaleString(leftIl->Name, player->GetSession()->GetSessionDbcLocale(), name);
                }

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, name, GOSSIP_SENDER_MAIN, (1000 * category) + (100 * counter) + i);
                LOG_INFO("sql", "AddingGossipItem with action {}", (1000 * category) + (100 * counter) + i);
            }
        }
        else if (action < 100000)
        {
            uint32 category = 0;
            uint32 itemtype = 0;
            uint32 counter = action;
            uint32 i;
            while (counter > 999)
            {
                ++category;
                counter = counter - 1000;
            }
            while (counter > 99)
            {
                ++itemtype;
                counter = counter - 100;
            }
            LOG_INFO("sql", "Handling item with category {}, itemtype {}, counter {}", category, itemtype, counter);

            // Check if the player has enough score in the respective category.
            uint32 availablescore = 0;
            if (sZoneDifficulty->ZoneDifficultyHardmodeScore.find(player->GetGUID().GetCounter()) != sZoneDifficulty->ZoneDifficultyHardmodeScore.end())
            {
                if (sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()].find(category) != sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()].end())
                {
                    availablescore = sZoneDifficulty->ZoneDifficultyHardmodeScore[player->GetGUID().GetCounter()][category];
                }
            }

            if (availablescore < sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][counter].Price)
            {
                npctext = NPC_TEXT_DENIED;
                SendGossipMenuFor(player, npctext, creature);
                std::string whisper;
                whisper.append("I am sorry, time-traveler. This item costs ");
                whisper.append(std::to_string(sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][counter].Price));
                whisper.append(" but you only have ");
                whisper.append(std::to_string(sZoneDifficulty->ZoneDifficultyHardmodeScore[category][player->GetGUID().GetCounter()]));
                whisper.append(" ");
                whisper.append(sZoneDifficulty->GetContentTypeString(category));
                creature->Whisper(whisper, LANG_UNIVERSAL, player);
                return true;
            }

            npctext = NPC_TEXT_CONFIRM;
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][counter].Entry);
            std::string gossip;
            std::string name = proto->Name1;
            if (ItemLocale const* leftIl = sObjectMgr->GetItemLocale(sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][counter].Entry))
            {
                ObjectMgr::GetLocaleString(leftIl->Name, player->GetSession()->GetSessionDbcLocale(), name);
            }
            gossip.append("Yes, ").append(name).append(" is the item i want.");
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, gossip, GOSSIP_SENDER_MAIN, 100000 + (1000 * category) + (100 * itemtype) + counter);
            LOG_INFO("sql", "AddingGossipItem with action {}", 100000 + (1000 * category) + (100 * itemtype) + counter);
        }
        else
        {
            npctext = NPC_TEXT_GRANT;
            uint32 category = 0;
            uint32 itemtype = 0;
            uint32 counter = action;
            uint32 i;
            counter = counter - 100000;
            while (counter > 999)
            {
                ++category;
                counter = counter - 1000;
            }
            while (counter > 99)
            {
                ++itemtype;
                counter = counter - 100;
            }
            LOG_INFO("sql", "Sending item with category {}, itemtype {}, counter {}", category, itemtype, counter);
            DeductHardmodeScore(player, category, sZoneDifficulty->ZoneDifficultyRewards[category][itemtype][counter].Price);
            SendItem(player, category, itemtype, counter);
        }

        SendGossipMenuFor(player, npctext, creature);
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        LOG_INFO("sql", "OnGossipHelloRewardNpc");
        uint32 npctext = NPC_TEXT_OFFER;
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Can you please remind me of my score?", GOSSIP_SENDER_MAIN, 999999);

        for (auto& typedata : sZoneDifficulty->ZoneDifficultyRewards)
        {
            LOG_INFO("sql", "typedata.first is {}", typedata.first);
            std::string gossip;
            std::string typestring = sZoneDifficulty->GetContentTypeString(typedata.first);
            gossip.append("I want to redeem rewards ").append(typestring);
            LOG_INFO("sql", "typestring is: {} gossip is: {}", typestring, gossip);
            // typedata.first is the ContentType
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, gossip, GOSSIP_SENDER_MAIN, typedata.first);
        }

        SendGossipMenuFor(player, npctext, creature);
        return true;
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
            LOG_INFO("sql", "mod_zone_difficulty_dungeonmasterAI: Reset happens.");
            if (me->GetMap() && me->GetMap()->IsHeroic() && !me->GetMap()->IsRaid())
            {
                LOG_INFO("sql", "We're inside a heroic 5man now.");
                for (auto& quest : sZoneDifficulty->DailyHeroicQuests)
                {
                    LOG_INFO("sql", "Checking quest {} and MapId {}", quest, me->GetMapId());
                    if (sPoolMgr->IsSpawnedObject<Quest>(quest))
                    {
                        if (sZoneDifficulty->HeroicQuestMapList[me->GetMapId()] == quest)
                        {
                            LOG_INFO("sql", "mod_zone_difficulty_dungeonmasterAI: Quest with id {} is active.", quest);
                            me->SetPhaseMask(1, true);

                            _scheduler.Schedule(15s, [this](TaskContext /*context*/)
                                {
                                    me->Yell("If you want a challenge, please talk to me soon adventurer!", LANG_UNIVERSAL);
                                });
                            _scheduler.Schedule(45s, [this](TaskContext /*context*/)
                                {
                                    me->Yell("I will take my leave then and offer my services to other adventurers. See you again!", LANG_UNIVERSAL);
                                });
                            _scheduler.Schedule(60s, [this](TaskContext /*context*/)
                                {
                                    me->DespawnOrUnsummon();
                                });
                            break;
                        }
                    }
                }
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
                    creature->Whisper("I am sorry, time-traveler. You can not return to this version of the time-line anymore. You have already completed one of the .", LANG_UNIVERSAL, player);
                }
            }
            // ... if there is an encounter in progress
            else if (player->GetInstanceScript() && player->GetInstanceScript()->IsEncounterInProgress())
            {
                LOG_INFO("sql", "IsEncounterInProgress");
                CanTurnOn = false;
                creature->Whisper("I am sorry, time-traveler. You can not return to this version of the time-line currently. There is already a battle in progress.", LANG_UNIVERSAL, player);
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
            if (sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodePossible == true)
            {
                LOG_INFO("sql", "Turn off hardmode for id {}", instanceId);
                sZoneDifficulty->HardmodeInstanceData[instanceId].HardmodeOn = false;
                sZoneDifficulty->SaveHardmodeInstanceData(instanceId);
                sZoneDifficulty->SendWhisperToRaid("We're switching to the cinematic version of the history lesson now. (Normal mode)", creature, player);
                CloseGossipMenuFor(player);
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Yes, i am sure. I know we can not go back to the harder version anymore, but we still want to stick with the less challenging route.", GOSSIP_SENDER_MAIN, 102);
                SendGossipMenuFor(player, NPC_TEXT_LEADER_FINAL, creature);
            }
        }
        else if (action == 102)
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
        LOG_INFO("sql", "OnGossipHelloChromie");
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

class mod_zone_difficulty_allcreaturescript : public AllCreatureScript
{
public:
    mod_zone_difficulty_allcreaturescript() : AllCreatureScript("mod_zone_difficulty_allcreaturescript") { }

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    {
        // Heavily inspired by https://github.com/azerothcore/mod-autobalance/blob/1d82080237e62376b9a030502264c90b5b8f272b/src/AutoBalance.cpp
        if (!creature || !creature->GetMap())
        {
            return;
        }

        if ((creature->IsHunterPet() || creature->IsPet() || creature->IsSummon()) && creature->IsControlledByPlayer())
        {
            return;
        }

        uint32 mapid = creature->GetMapId();
        if (sZoneDifficulty->ZoneDifficultyNerfInfo.find(mapid) == sZoneDifficulty->ZoneDifficultyNerfInfo.end())
        {
            return;
        }

        if (!creature->IsAlive())
        {
            return;
        }

        Map* map = creature->GetMap();
        if (!map->IsRaid() &&
            (!(map->IsHeroic() && map->IsDungeon())))
        {
            return;
        }

        CreatureTemplate const* creatureTemplate = creature->GetCreatureTemplate();
        //skip critters and special creatures (spell summons etc.) in instances
        if (creatureTemplate->maxlevel <= 1)
        {
            return;
        }

        CreatureBaseStats const* origCreatureStats = sObjectMgr->GetCreatureBaseStats(creature->GetLevel(), creatureTemplate->unit_class);
        uint32 baseHealth = origCreatureStats->GenerateHealth(creatureTemplate);
        uint32 newHp;
        uint32 entry = creature->GetEntry();
        LOG_INFO("sql", "Checking hp for creature with entry {}.", entry);
        if (sZoneDifficulty->ZoneDifficultyCreatureOverrides.find(entry) == sZoneDifficulty->ZoneDifficultyCreatureOverrides.end())
        {
            if (creature->IsDungeonBoss())
            {
                return;
            }
            newHp = round(baseHealth * sZoneDifficulty->HardmodeHpModifier);
        }
        else
        {
            newHp = round(baseHealth * sZoneDifficulty->ZoneDifficultyCreatureOverrides[entry]);
        }

        uint32 phaseMask = creature->GetPhaseMask();
        int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(creature->GetMapId(), phaseMask);
        int8 mode = sZoneDifficulty->ZoneDifficultyNerfInfo[mapid][matchingPhase].Enabled;
        if (matchingPhase != -1)
        {
            if ((mode & MODE_HARD) == MODE_HARD && sZoneDifficulty->HardmodeInstanceData[creature->GetMap()->GetInstanceId()].HardmodeOn == true)
            {
                if (creature->GetMaxHealth() == newHp)
                {
                    return;
                }
                LOG_INFO("sql", "Modify creature hp for hard mode: {} to {}", baseHealth, newHp);
                bool hpIsFull = false;
                if (creature->GetHealthPct() >= 100)
                {
                    hpIsFull = true;
                }
                creature->SetMaxHealth(newHp);
                creature->SetCreateHealth(newHp);
                creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)newHp);
                if (hpIsFull)
                {
                    creature->SetHealth(newHp);
                }
                creature->UpdateAllStats();
                return;
            }

            if (sZoneDifficulty->HardmodeInstanceData[creature->GetMap()->GetInstanceId()].HardmodeOn == false)
            {
                if (creature->GetMaxHealth() == newHp)
                {
                    LOG_INFO("sql", "Modify creature hp for normal mode: {} to {}", baseHealth, baseHealth);
                    bool hpIsFull = false;
                    if (creature->GetHealthPct() >= 100)
                    {
                        hpIsFull = true;
                    }
                    creature->SetMaxHealth(baseHealth);
                    creature->SetCreateHealth(baseHealth);
                    creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)baseHealth);
                    if (hpIsFull)
                    {
                        creature->SetHealth(baseHealth);
                    }
                    creature->UpdateAllStats();
                    return;
                }
            }
        }
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
    new mod_zone_difficulty_rewardnpc();
    new mod_zone_difficulty_dungeonmaster();
    new mod_zone_difficulty_allcreaturescript();
}
