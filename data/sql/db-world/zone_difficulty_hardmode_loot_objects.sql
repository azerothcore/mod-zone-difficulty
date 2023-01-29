DROP TABLE IF EXISTS `zone_difficulty_loot_objects`;
CREATE TABLE `zone_difficulty_loot_objects` (
    `MapID` INT NOT NULL DEFAULT 0,
    `SourceEntry` INT NOT NULL,
    `OverrideGO` INT NOT NULL DEFAULT 0,  -- 0 = no override
	PRIMARY KEY (`MapID`, `Entry`, `Type`)
);

INSERT INTO `zone_difficulty_loot_objects` (`MapID`, `Entry`, `Type`) VALUES
(565, 18831, 0),    -- High King Maulgar, Gruul's Lair
(565, 19044, 0);    -- Gruul, Gruul's Lair
