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

const int32 DUEL_INDEX = 0x7FFFFFFF;
const int32 DUEL_AREA = 2402;       // Forbidding Sea (Wetlands)
const uint32 NPC_TEXT_LEADER_NORMAL = 91301;
const uint32 NPC_TEXT_OTHER = 91302;
const uint32 NPC_TEXT_LEADER_HARD = 91303;
const uint32 NPC_TEXT_LEADER_FINAL = 91304;
const int32 MODE_NORMAL = 1;
const int32 MODE_HARD = 64;
const int32 TYPE_HEROIC_TBC = 1;
const int32 TYPE_RAID_T4 = 2;
const int32 TYPE_RAID_T5 = 3;
const int32 TYPE_RAID_T6 = 4;
const int32 TYPE_HEROIC_WOTLK = 5;
const int32 TYPE_RAID_T7 = 6;
const int32 TYPE_RAID_T8 = 7;
const int32 TYPE_RAID_T9 = 8;
const int32 TYPE_RAID_T10 = 9;

class ZoneDifficulty
{
public:
    static ZoneDifficulty* instance();

    void LoadMapDifficultySettings();
    void SaveHardmodeInstanceData(uint32 instanceId);
    void LoadHardmodeInstanceData();
    void LoadHardmodeScoreData();
    void SendWhisperToRaid(std::string message, Creature* creature, Player* player);
    void GrantHardmodeScore(Map* map, uint32 type);
    [[nodiscard]] bool IsValidNerfTarget(Unit* target);
    [[nodiscard]] bool VectorContains(std::vector<uint32> vec, uint32 element);
    [[nodiscard]] bool IsHardmodeMap(uint32 mapid);
    [[nodiscard]] bool ShouldNerfInDuels(Unit* target);
    [[nodiscard]] int32 GetLowestMatchingPhase(uint32 mapId, uint32 phaseMask);

    bool IsEnabled{ false };
    bool IsDebugInfoEnabled{ false };
    float HardmodeHpModifier{ 2.0 };
    std::vector<uint32> DailyHeroicQuests;
    std::map<uint32, uint32> HeroicQuestMapList;
    std::map<uint32, uint8> Expansion;

    typedef std::map<uint32, std::map<uint32, ZoneDifficultyNerfData> > ZoneDifficultyNerfDataMap;
    ZoneDifficultyNerfDataMap ZoneDifficultyNerfInfo;
    std::map<uint32, float> SpellNerfOverrides;
    typedef std::map<uint32, std::vector<uint32> > ZoneDifficultyDisablesMap;
    ZoneDifficultyDisablesMap DisallowedBuffs;
    typedef std::map<uint32, ZoneDifficultyHardmodeInstData> ZoneDifficultyHardmodeInstDataMap;
    ZoneDifficultyHardmodeInstDataMap HardmodeInstanceData;
    typedef std::map<uint32, std::vector<ZoneDifficultyHardmodeMapData> > ZoneDifficultyHardmodeLootMap;
    ZoneDifficultyHardmodeLootMap HardmodeLoot;
    typedef std::map<uint32, std::vector<uint32> > ZondeDifficultyScoreMap;
    ZondeDifficultyScoreMap ZoneDifficultyHardmodeScore;
};

#define sZoneDifficulty ZoneDifficulty::instance()

#endif
