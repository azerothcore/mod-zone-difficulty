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

const int32 DUEL_INDEX = 0x7FFFFFFF;
const int32 DUEL_AREA = 2402;       // Forbidding Sea (Wetlands)
const uint32 NPC_TEXT_LEADER_NORMAL = 91301;
const uint32 NPC_TEXT_OTHER = 91302;
const uint32 NPC_TEXT_LEADER_HARD = 91303;
const uint32 NPC_TEXT_LEADER_FINAL = 91304;
const int8 MODE_NORMAL = 1;
const int8 MODE_HARD = 64;
const int8 TYPE_HEROIC_TBC = 1;

class ZoneDifficulty
{
public:
    static ZoneDifficulty* instance();

    void LoadMapDifficultySettings();
    void SaveHardmodeInstanceData(uint32 instanceId);
    void LoadHardmodeInstanceData();
    void LoadHardmodeScoreData();
    void SendWhisperToRaid(std::string message, Creature* creature, Player* player);
    void GrantHardmodeScore(Map* map);
    [[nodiscard]] bool IsValidNerfTarget(Unit* target);
    [[nodiscard]] bool VectorContains(std::vector<uint32> vec, uint32 element);
    [[nodiscard]] bool ShouldNerfInDuels(Unit* target);
    [[nodiscard]] int32 GetLowestMatchingPhase(uint32 mapId, uint32 phaseMask);

    bool IsEnabled{ false };
    bool IsDebugInfoEnabled{ false };
    std::vector<uint32> DailyHeroicQuests;
    std::map<uint32, uint32> HeroicQuestMapList;
    std::map<uint32, uint32> PlayerHeroicScore;

    typedef std::map<uint32, std::map<uint32, ZoneDifficultyNerfData> > ZoneDifficultyNerfDataMap;
    ZoneDifficultyNerfDataMap ZoneDifficultyNerfInfo;
    std::map<uint32, float> SpellNerfOverrides;
    typedef std::map<uint32, std::vector<uint32> > ZoneDifficultyDisablesMap;
    ZoneDifficultyDisablesMap DisallowedBuffs;
    typedef std::map<uint32, ZoneDifficultyHardmodeInstData> ZoneDifficultyHardmodeInstDataMap;
    ZoneDifficultyHardmodeInstDataMap HardmodeInstanceData;
    typedef std::map<uint32, std::vector<uint32> > ZoneDifficultyHardmodeLootMap;
    ZoneDifficultyHardmodeLootMap HardmodeLoot;
};

#define sZoneDifficulty ZoneDifficulty::instance()

#endif
