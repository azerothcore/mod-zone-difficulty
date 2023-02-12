CREATE TABLE IF NOT EXISTS `zone_difficulty_instance_saves` (
    `InstanceID` INT NOT NULL DEFAULT 0,
    `HardmodeOn` TINYINT NOT NULL DEFAULT 0,
    `HardmodePossible` TINYINT NOT NULL DEFAULT 1,
	PRIMARY KEY (`InstanceID`)
);

CREATE TABLE IF NOT EXISTS zone_difficulty_hardmode_score(
    `GUID` INT NOT NULL DEFAULT 0,
    `Type` TINYINT NOT NULL DEFAULT 0,
    `Score` TINYINT NOT NULL DEFAULT 0,
	PRIMARY KEY (`GUID`, `Type`)
);