DROP TABLE IF EXISTS `zone_difficulty_loot_objects`;
CREATE TABLE `zone_difficulty_loot_objects` (
    `MapID` INT NOT NULL DEFAULT 0,
    `SourceEntry` INT NOT NULL,
    -- 0 = no override. Only required if the loot for an encounter is obtained
    -- from e.g. a chest instead of a corpse.
    `OverrideGO` INT NOT NULL DEFAULT 0,
	PRIMARY KEY (`MapID`, `SourceEntry`, `OverrideGO`)
);

INSERT INTO `zone_difficulty_loot_objects` (`MapID`, `SourceEntry`, `OverrideGO`) VALUES
(544, 17257, 0),    -- Magtheridon, Magtheridon's Lair
(565, 18831, 0),    -- High King Maulgar, Gruul's Lair
(565, 19044, 0),    -- Gruul, Gruul's Lair
(543, 17537, 0);    -- Vazruden, Hellfire Ramparts
