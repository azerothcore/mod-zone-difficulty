/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Chat.h"
#include "ZoneDifficulty.h"

using namespace Acore::ChatCommands;

class zone_difficulty_commandscript : public CommandScript
{
public:
    zone_difficulty_commandscript() : CommandScript("zone_difficulty_commandscript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable zonedifficultyTable =
        {
            { "tiers", HandleMythicTiers, SEC_GAMEMASTER, Console::Yes },
            { "showpoints", HandleMythicShowPoints, SEC_GAMEMASTER, Console::Yes },
            { "updatepoints", HandleMythicUpdatePoints, SEC_ADMINISTRATOR, Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "mythic", zonedifficultyTable },
        };

        return commandTable;
    }

    static bool HandleMythicShowPoints(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player)
            return false;

        if (!player->IsConnected())
        {
            std::string name;
            sCharacterCache->GetCharacterNameByGuid(player->GetGUID(), name);
            handler->SendErrorMessage(LANG_PLAYER_NOT_EXIST_OR_OFFLINE, name);
            return false;
        }

        bool hasAnyScore = false;
        for (int i = 1; i <= TYPE_MAX_TIERS; ++i)
        {
            if (uint32 score = player->GetConnectedPlayer()->GetPlayerSetting(ModZoneDifficultyString + "score", i).value)
            {
                handler->SendSysMessage(Acore::StringFormat("Found score {} for {} (Tier ID: {})", score, sZoneDifficulty->GetContentTypeString(i), i));
                hasAnyScore = true;
            }
        }

        if (!hasAnyScore)
            handler->SendSysMessage("Player doesn't have any score in any category.");

        return true;
    }

    static bool HandleMythicUpdatePoints(ChatHandler* handler, Optional<PlayerIdentifier> player, uint32 index, int32 points)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player)
            return false;

        if (!player->IsConnected())
        {
            std::string name;
            sCharacterCache->GetCharacterNameByGuid(player->GetGUID(), name);
            handler->SendErrorMessage(LANG_PLAYER_NOT_EXIST_OR_OFFLINE, name);
            return false;
        }

        int32 score = player->GetConnectedPlayer()->GetPlayerSetting(ModZoneDifficultyString + "score", index).value + points;

        if (score < 0)
            score = 0;

        player->GetConnectedPlayer()->UpdatePlayerSetting(ModZoneDifficultyString + "score", index, score);

        if (points < 0)
            handler->SendSysMessage(Acore::StringFormat("Player {} has lost {} points in category {}.", player->GetName(), -points, sZoneDifficulty->GetContentTypeString(index)));
        else
            handler->SendSysMessage(Acore::StringFormat("Player {} has gained {} points in category {}.", player->GetName(), points, sZoneDifficulty->GetContentTypeString(index)));

        return true;
    }

    static bool HandleMythicTiers(ChatHandler* handler)
    {
        for (int i = 1; i <= TYPE_MAX_TIERS; ++i)
            handler->SendSysMessage(Acore::StringFormat("{} - {}", i, sZoneDifficulty->GetContentTypeString(i), i));

        return true;
    }
};

void AddSC_zone_difficulty_commandscript()
{
    new zone_difficulty_commandscript();
}
