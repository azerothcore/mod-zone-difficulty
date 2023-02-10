DELETE FROM `creature_loot_template` WHERE `LootMode` = 64 AND `GroupId` = 64;

INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(17257, 37, 0, 100, 0, 64, 64, 1, 1, 'Magtheridon - Hardmode'),
(18831, 37, 0, 100, 0, 64, 64, 1, 1, 'High King Maulgar - Hardmode'),
(19044, 37, 0, 100, 0, 64, 64, 1, 1, 'Gruul the Dragonkiller - Hardmode'),
(18433, 37, 0, 100, 0, 64, 64, 1, 1, 'Omor the Unscarred- Hardmode');
