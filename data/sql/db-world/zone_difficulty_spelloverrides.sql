DROP TABLE IF EXISTS `zone_difficulty_spelloverrides`;
CREATE TABLE `zone_difficulty_spelloverrides` (
    `SpellID` INT NOT NULL DEFAULT 0,
    `NerfValue` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_spelloverrides` WHERE `SpellID` IN (53652, 53654, 52752, 47753, 18562, 25516);
INSERT INTO `zone_difficulty_spelloverrides` (`SpellID`, `NerfValue`, `Enabled`, `Comment`) VALUES
(53652, 1, 1, 'Beacon of Light - Skip nerf.'),
(53654, 1, 1, 'Beacon of Light 2 (Flash of Light, Holy Shock) - Skip nerf.'),
(52752, 1, 1, 'Ancestral Awakening - Skip nerf.'),
(47753, 1, 1, 'Divine Aegis - Skip nerf.'),
(18562, 1, 1, 'Swiftmend - Skip nerf.'),
(25516, 1, 1, 'AQ20 Andorov Healing - Skip nerf.'),
(36240, 1, 1, 'Gruul Cave In - Skip buff.');
