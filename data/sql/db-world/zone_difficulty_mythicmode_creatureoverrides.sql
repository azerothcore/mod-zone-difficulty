DROP TABLE IF EXISTS `zone_difficulty_mythicmode_creatureoverrides`;
CREATE TABLE `zone_difficulty_mythicmode_creatureoverrides` (
    `CreatureEntry` INT NOT NULL DEFAULT 0,
    `HPModifier` FLOAT NOT NULL DEFAULT 1,
    `HPModifierNormal` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT UNSIGNED DEFAULT 1,
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_mythicmode_creatureoverrides` WHERE `CreatureEntry` IN (1128001, 18831, 19044, 17257, 17881);
INSERT INTO `zone_difficulty_mythicmode_creatureoverrides` (`CreatureEntry`, `HPModifier`, `HPModifierNormal`, `Enabled`, `Comment`) VALUES
(1128001, 1, 1, 1, 'Chromie NPC - prevent changing hp.'),
(18831, 1.8, 1, 1, 'Maulgar, Gruul\'s Lair HPx1.8'),
(19044, 1.8, 1, 1, 'Gruul, Gruul\'s Lair HPx1.8'),
(17257, 1.8, 1, 1, 'Magtheridon, Magtheridon\'s Lair HPx1.8'),
(17881, 3.2, 1, 1, 'Aeonus, Black Morass HPx3.2');
