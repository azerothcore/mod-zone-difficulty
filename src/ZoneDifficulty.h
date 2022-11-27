#ifndef DEF_ZONEDIFFICULTY_H
#define DEF_ZONEDIFFICULTY_H

#include "Player.h"
#include "Config.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

struct ZoneDifficultyData
{
    float HealingNerfPct;
    float AbsorbNerfPct;
    float SpellDamageBuffPct;
    float MeleeDamageBuffPct;
    bool Enabled;
};

const int32 DUEL_INDEX = 0x7FFFFFFF;
const int32 DUEL_AREA = 2402;       // Forbidding Sea (Wetlands)

class ZoneDifficulty
{
public:
    static ZoneDifficulty* instance();

    void LoadMapDifficultySettings();
    [[nodiscard]] bool IsValidNerfTarget(Unit* target);
    [[nodiscard]] bool ShouldNerfInDuels(Unit* target);
    [[nodiscard]] int32 GetLowestMatchingPhase(uint32 mapId, uint32 phaseMask);

    bool IsEnabled{ false };
    bool IsDebugInfoEnabled{ false };

    typedef std::map<uint32, std::map<uint32, ZoneDifficultyData> > ZoneDifficultyDataMap;
    ZoneDifficultyDataMap ZoneDifficultyInfo;
    std::map<uint32, float> SpellNerfOverrides;
    typedef std::map<uint32, std::vector<uint32> > ZoneDifficultyDisablesMap;
    ZoneDifficultyDisablesMap DisallowedBuffs;

};

#define sZoneDifficulty ZoneDifficulty::instance()

#endif
