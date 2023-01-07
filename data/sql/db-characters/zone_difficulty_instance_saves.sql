CREATE TABLE IF NOT EXISTS `zone_difficulty_instance_saves` (
    `InstanceID` INT NOT NULL DEFAULT 0,
    `HardmodeOn` TINYINT NOT NULL DEFAULT 0,
    `HardmodePossible` TINYINT NOT NULL DEFAULT 1,
	PRIMARY KEY (`InstanceID`)
);
