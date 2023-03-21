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
    uint32 Achievement;
};

struct ZoneDifficultyHAI
{
    uint8 Chance;
    uint32 Spell;
    int32 Spellbp0;
    int32 Spellbp1;
    int32 Spellbp2;
    uint8 Target;
    uint8 TargetArg;
    std::chrono::milliseconds Delay;
    std::chrono::milliseconds Cooldown;
    uint8 Repetitions;
};

int32 const DUEL_INDEX = 0x7FFFFFFF;
int32 const DUEL_AREA = 2402;       // Forbidding Sea (Wetlands)

uint32 const NPC_TEXT_LEADER_NORMAL = 91301;
uint32 const NPC_TEXT_OTHER = 91302;
uint32 const NPC_TEXT_LEADER_HARD = 91303;
uint32 const NPC_TEXT_LEADER_FINAL = 91304;
uint32 const NPC_TEXT_OFFER = 91305;
uint32 const NPC_TEXT_CATEGORY = 91306;
uint32 const NPC_TEXT_ITEM = 91307;
uint32 const NPC_TEXT_CONFIRM = 91308;
uint32 const NPC_TEXT_GRANT = 91309;
uint32 const NPC_TEXT_DENIED = 91310;
uint32 const NPC_TEXT_SCORE = 91311;

int32 const MODE_NORMAL = 1;
int32 const MODE_HARD = 64;

// EVENT_GROUP is used for unit->m_Events.AddEventAtOffset
uint8 const EVENT_GROUP = 64;

int32 const TYPE_VANILLA = 1;
int32 const TYPE_RAID_MC = 2;
int32 const TYPE_RAID_ONY = 3;
int32 const TYPE_RAID_BWL = 4;
int32 const TYPE_RAID_ZG = 5;
int32 const TYPE_RAID_AQ20 = 6;
int32 const TYPE_RAID_AQ40 = 7;
int32 const TYPE_HEROIC_TBC = 8;
int32 const TYPE_RAID_T4 = 9;
int32 const TYPE_RAID_T5 = 10;
int32 const TYPE_RAID_T6 = 11;
int32 const TYPE_HEROIC_WOTLK = 12;
int32 const TYPE_RAID_T7 = 13;
int32 const TYPE_RAID_T8 = 14;
int32 const TYPE_RAID_T9 = 15;
int32 const TYPE_RAID_T10 = 16;

uint32 const ITEMTYPE_MISC = 1;
uint32 const ITEMTYPE_CLOTH = 2;
uint32 const ITEMTYPE_LEATHER = 3;
uint32 const ITEMTYPE_MAIL = 4;
uint32 const ITEMTYPE_PLATE = 5;
uint32 const ITEMTYPE_WEAPONS = 6;

uint8 const TARGET_SELF = 1;
uint8 const TARGET_VICTIM = 2;                      // current target
uint8 const TARGET_HOSTILE_AGGRO_FROM_TOP = 3;      // count TargetArg highest aggro from top
uint8 const TARGET_HOSTILE_AGGRO_FROM_BOTTOM = 4;   // count TargetArg lowest aggro from bottom
uint8 const TARGET_HOSTILE_RANDOM = 5;              // any random player from the threat list
uint8 const TARGET_HOSTILE_RANDOM_NOT_TOP = 6;      // any random player from the threat list except the current target
uint8 const TARGET_PLAYER_DISTANCE = 18;            // all players within TargetArg range

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
    void SendItem(Player* player, uint32 category, uint32 itemType, uint32 id);
    std::list<Unit*> GetTargetList(Unit* unit, uint32 entry, uint32 key);
    void HardmodeEvent(Unit* unit, uint32 entry, uint32 key);
    bool HasNormalMode(int8 mode) { return (mode & MODE_NORMAL) == MODE_NORMAL; }
    bool HasHardMode(int8 mode) { return (mode & MODE_HARD) == MODE_HARD; }
    [[nodiscard]] bool IsValidNerfTarget(Unit* target);
    [[nodiscard]] bool VectorContainsUint32(std::vector<uint32> vec, uint32 element);
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
    std::map<uint32, std::string> ItemIcons;

    typedef std::map<uint32, std::map<uint32, ZoneDifficultyNerfData> > ZoneDifficultyNerfDataMap;
    ZoneDifficultyNerfDataMap NerfInfo;
    std::map<uint32, float> SpellNerfOverrides;
    typedef std::map<uint32, std::vector<uint32> > ZoneDifficultyDisablesMap;
    ZoneDifficultyDisablesMap DisallowedBuffs;
    typedef std::map<uint32, bool> ZoneDifficultyHardmodeInstDataMap;
    ZoneDifficultyHardmodeInstDataMap HardmodeInstanceData;
    typedef std::map<uint32, std::vector<ZoneDifficultyHardmodeMapData> > ZoneDifficultyHardmodeLootMap;
    ZoneDifficultyHardmodeLootMap HardmodeLoot;
    typedef std::map<uint32, std::map<uint32, uint32> > ZoneDifficultyDualUintMap;
    ZoneDifficultyDualUintMap HardmodeScore;
    typedef std::map<uint32, std::map<uint32, std::vector<ZoneDifficultyRewardData> > > ZoneDifficultyRewardMap;
    ZoneDifficultyRewardMap Rewards;
    typedef std::map<uint32, std::vector<ZoneDifficultyHAI> > ZoneDifficultyHAIMap;
    ZoneDifficultyHAIMap HardmodeAI;
};

#define sZoneDifficulty ZoneDifficulty::instance()

#endif
