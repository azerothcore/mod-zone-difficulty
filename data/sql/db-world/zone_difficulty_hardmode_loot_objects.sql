DROP TABLE IF EXISTS `zone_difficulty_loot_objects`;
CREATE TABLE `zone_difficulty_loot_objects` (
    `MapID` INT NOT NULL DEFAULT 0,
    `Entry` INT NOT NULL DEFAULT 0,
    `Type` TINYINT NOT NULL DEFAULT 1,  -- 1 = Creature, 2 = GameObject
	PRIMARY KEY (`MapID`, `Entry`, `Type`)
);

INSERT INTO `zone_difficulty_loot_objects` (`MapID`, `Entry`, `Type`) VALUES
(565, 18831, 1),    -- High King Maulgar, Gruul's Lair
(565, 19044, 1);    -- Gruul, Gruul's Lair
