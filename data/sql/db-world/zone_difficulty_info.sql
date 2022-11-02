DROP TABLE IF EXISTS `zone_difficulty_info`;
CREATE TABLE `zone_difficulty_info` (
    `MapID` INT NOT NULL DEFAULT 0,
    `HealingNerfValue` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT
);
