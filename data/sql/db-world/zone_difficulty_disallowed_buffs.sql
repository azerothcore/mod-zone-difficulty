DROP TABLE IF EXISTS `zone_difficulty_disallowed_buffs`;
CREATE TABLE `zone_difficulty_disallowed_buffs` (
    `MapID` INT NOT NULL DEFAULT 0,
    `DisallowedBuffs` STRING NOT NULL DEFAULT "",
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT DEFAULT "",
    PRIMARY KEY (`MapID`)
);

DELETE FROM `zone_difficulty_disallowed_buffs`;
INSERT INTO `zone_difficulty_disallowed_buffs` (`MapID`, `DisallowedBuffs`, `Enabled`, `Comment`) VALUES
(531, '22888 24425', 1, 'Forbid in AQ40: Rallying Cry of the Dragonslayer, Spirit of Zandalar');
