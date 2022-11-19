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

int DUEL_INDEX = 0x7FFFFFFF;

class ZoneDifficulty
{
public:
    static ZoneDifficulty* instance();

    void LoadMapDifficultySettings();
    [[nodiscard]] bool IsValidNerfTarget(Unit* target);
    [[nodiscard]] bool ShouldNerfInDuels(Unit* target);
    [[nodiscard]] bool ShouldNerfAbsorb(uint32 mapId, Unit* target);

    bool IsEnabled{ false };
    bool IsDebugInfoEnabled{ false };

    typedef std::map<uint32, ZoneDifficultyData> ZoneDifficultyDataMap;
    ZoneDifficultyDataMap ZoneDifficultyInfo;
    std::map<uint32, float> SpellNerfOverrides;

};

enum ModDiffSettings
{
    SETTING_DUEL_DEBUFF = 0
};

#define sZoneDifficulty ZoneDifficulty::instance()

#endif
