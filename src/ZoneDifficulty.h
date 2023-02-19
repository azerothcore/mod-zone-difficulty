#ifndef DEF_ZONEDIFFICULTY_H
#define DEF_ZONEDIFFICULTY_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

struct ZoneDifficultyNerfData
{
    float HealingNerfPct;
    float AbsorbNerfPct;
    float SpellDamageBuffPct;
    float MeleeDamageBuffPct;
    int8 Enabled;
    float HealingNerfPctHard;
    float AbsorbNerfPctHard;
    float SpellDamageBuffPctHard;
    float MeleeDamageBuffPctHard;
};

struct ZoneDifficultyHardmodeInstData
{
    bool HardmodeOn;
    bool HardmodePossible;
};

struct ZoneDifficultyHardmodeMapData
{
    uint32 EncounterEntry;
    uint32 OverrideGO;
    uint32 RewardType;
};

struct ZoneDifficultyRewardData
{
    uint32 Entry;
    uint32 Price;
    uint32 Enchant;
    uint8 EnchantSlot;
};

const int32 DUEL_INDEX = 0x7FFFFFFF;
const int32 DUEL_AREA = 2402;       // Forbidding Sea (Wetlands)

const uint32 NPC_TEXT_LEADER_NORMAL = 91301;
const uint32 NPC_TEXT_OTHER = 91302;
const uint32 NPC_TEXT_LEADER_HARD = 91303;
const uint32 NPC_TEXT_LEADER_FINAL = 91304;
const uint32 NPC_TEXT_OFFER = 91305;
const uint32 NPC_TEXT_CATEGORY = 91306;
const uint32 NPC_TEXT_ITEM = 91307;
const uint32 NPC_TEXT_CONFIRM = 91308;
const uint32 NPC_TEXT_GRANT = 91309;
const uint32 NPC_TEXT_DENIED = 91310;
const uint32 NPC_TEXT_SCORE = 91311;

const int32 MODE_NORMAL = 1;
const int32 MODE_HARD = 64;

const int32 TYPE_VANILLA = 1;
const int32 TYPE_RAID_MC = 2;
const int32 TYPE_RAID_ONY = 3;
const int32 TYPE_RAID_BWL = 4;
const int32 TYPE_RAID_ZG = 5;
const int32 TYPE_RAID_AQ20 = 6;
const int32 TYPE_RAID_AQ40 = 7;
const int32 TYPE_HEROIC_TBC = 8;
const int32 TYPE_RAID_T4 = 9;
const int32 TYPE_RAID_T5 = 10;
const int32 TYPE_RAID_T6 = 11;
const int32 TYPE_HEROIC_WOTLK = 12;
const int32 TYPE_RAID_T7 = 13;
const int32 TYPE_RAID_T8 = 14;
const int32 TYPE_RAID_T9 = 15;
const int32 TYPE_RAID_T10 = 16;

const uint32 ITEMTYPE_MISC = 1;
const uint32 ITEMTYPE_CLOTH = 2;
const uint32 ITEMTYPE_LEATHER = 3;
const uint32 ITEMTYPE_MAIL = 4;
const uint32 ITEMTYPE_PLATE = 5;
const uint32 ITEMTYPE_WEAPONS = 6;

const std::string REWARD_MAIL_SUBJECT = "Chromie's Reward for you";
const std::string REWARD_MAIL_BODY = "Enjoy your new item!";

class ZoneDifficulty
{
public:
    static ZoneDifficulty* instance();

    void LoadMapDifficultySettings();
    void SaveHardmodeInstanceData(uint32 instanceId);
    void LoadHardmodeInstanceData();
    void LoadHardmodeScoreData();
    void SendWhisperToRaid(std::string message, Creature* creature, Player* player);
    std::string GetItemTypeString(uint32 type);
    std::string GetContentTypeString(uint32 type);
    void AddHardmodeScore(Map* map, uint32 type);
    void DeductHardmodeScore(Player* player, uint32 type, uint32 score);
    void SendItem(Player* player, uint32 category, uint32 itemtype, uint32 id);
    [[nodiscard]] bool IsValidNerfTarget(Unit* target);
    [[nodiscard]] bool VectorContains(std::vector<uint32> vec, uint32 element);
    [[nodiscard]] bool IsHardmodeMap(uint32 mapid);
    [[nodiscard]] bool ShouldNerfInDuels(Unit* target);
    [[nodiscard]] int32 GetLowestMatchingPhase(uint32 mapId, uint32 phaseMask);

    bool IsEnabled{ false };
    bool IsDebugInfoEnabled{ false };
    float HardmodeHpModifier{ 2.0 };
    std::vector<uint32> DailyHeroicQuests;
    std::map<uint32, uint32> HeroicTBCQuestMapList;
    std::map<uint32, uint8> Expansion;
    std::map<uint32, float> CreatureOverrides;
    std::map<uint32, uint32> EncountersInProgress;

    typedef std::map<uint32, std::map<uint32, ZoneDifficultyNerfData> > ZoneDifficultyNerfDataMap;
    ZoneDifficultyNerfDataMap NerfInfo;
    std::map<uint32, float> SpellNerfOverrides;
    typedef std::map<uint32, std::vector<uint32> > ZoneDifficultyDisablesMap;
    ZoneDifficultyDisablesMap DisallowedBuffs;
    typedef std::map<uint32, ZoneDifficultyHardmodeInstData> ZoneDifficultyHardmodeInstDataMap;
    ZoneDifficultyHardmodeInstDataMap HardmodeInstanceData;
    typedef std::map<uint32, std::vector<ZoneDifficultyHardmodeMapData> > ZoneDifficultyHardmodeLootMap;
    ZoneDifficultyHardmodeLootMap HardmodeLoot;
    typedef std::map<uint32, std::map<uint32, uint32> > ZoneDifficultyDualUintMap;
    ZoneDifficultyDualUintMap HardmodeScore;
    typedef std::map<uint32, std::map<uint32, std::vector<ZoneDifficultyRewardData> > > ZoneDifficultyRewardMap;
    ZoneDifficultyRewardMap Rewards;
};

#define sZoneDifficulty ZoneDifficulty::instance()

#endif
