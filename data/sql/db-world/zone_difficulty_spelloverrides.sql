DROP TABLE IF EXISTS `zone_difficulty_spelloverrides`;
CREATE TABLE `zone_difficulty_spelloverrides` (
    `SpellID` INT NOT NULL DEFAULT 0,
    `MapId` INT NOT NULL DEFAULT 0,            -- ID of the map for this line to be relevant. 0 for all maps
    `NerfValue` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT DEFAULT 1,               -- Modes for this multiplier to be enabled. 1 = normal 64 = mythic
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_spelloverrides` WHERE `SpellID` IN (53652, 53654, 52752, 47753, 18562, 25516, 36240, 31850, 31851, 31852, 66235, 39878, 42052, 40314, 40175, 40175, 41303, 41360, 40827, 40869, 40870, 40871, 41001, 42005, 41078, 41131);
INSERT INTO `zone_difficulty_spelloverrides` (`SpellID`, `MapId`, `NerfValue`, `Enabled`, `Comment`) VALUES
(53652, 0, 1, 1, 'Beacon of Light - Skip nerf.'),
(53654, 0, 1, 1, 'Beacon of Light 2 (Flash of Light, Holy Shock) - Skip nerf.'),
(52752, 0, 1, 1, 'Ancestral Awakening - Skip nerf.'),
(47753, 0, 1, 1, 'Divine Aegis - Skip nerf.'),
(18562, 0, 1, 1, 'Swiftmend - Skip nerf.'),
(25516, 0, 1, 1, 'AQ20 Andorov Healing - Skip nerf.'),
(36240, 0, 1, 1, 'Gruul Cave In - Skip buff.'),
(34435, 0, 1, 1, 'Magtheridon - Warder - Rain of Fire Skip buff.'),
(31850, 0, 1, 1, 'Ardent Defender - Absorb - Skip nerf'),
(31851, 0, 1, 1, 'Ardent Defender - Absorb - Skip nerf'),
(31852, 0, 1, 1, 'Ardent Defender - Absorb - Skip nerf'),
(66235, 0, 1, 1, 'Ardent Defender - Heal - Skip nerf'),
(39878, 564, 1, 1, 'Black Temple - Najentus - Tidal Burst Effect'),
(42052, 564, 1, 1, 'Black Temple - Supremus - Volcanic Geyser'),
(40314, 564, 1, 1, 'Black Temple - Vengeful Spirit - Spirit Volley'),
(40175, 564, 1, 1, 'Black Temple - Vengeful Spirit - Spirit Chains'),
(40157, 564, 1, 1, 'Black Temple - Vengeful Spirit - Spirit Lance'),
(41303, 564, 1, 1, 'Black Temple - Reliquary of the Lost - Essence of Suffering - Soul Drain'),
(41360, 564, 1, 1, 'Black Temple - Promenade Sentinel - Arcane charge'), -- may not work
(40827, 564, 1, 1, 'Black Temple - Mother Shahraz - Sinful Beam'),
(40869, 564, 1, 1, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(40870, 564, 1, 1, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(40871, 564, 1, 1, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(41001, 564, 1, 1, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(42005, 564, 1, 1, 'Black Temple - Gurtogg Bloodboil - Bloodboil'),
(41078, 564, 1.1, 1, 'Black Temple - Illidan - Shadow Blast'),
(41131, 564, 1.1, 1, 'Black Temple - Illidan - Flame Burst');
