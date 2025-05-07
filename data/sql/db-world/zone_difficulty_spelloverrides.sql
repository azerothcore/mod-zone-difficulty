DROP TABLE IF EXISTS `zone_difficulty_spelloverrides`;
CREATE TABLE `zone_difficulty_spelloverrides` (
    `SpellID` INT NOT NULL DEFAULT 0,
    `MapId` INT NOT NULL DEFAULT 0,            -- ID of the map for this line to be relevant. 0 for all maps
    `NerfValue` FLOAT NOT NULL DEFAULT 1,
    `ModeMask` TINYINT DEFAULT 1,               -- Modes for this multiplier to be enabled. 1 = normal 64 = mythic 0 = disabled
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_spelloverrides` WHERE `SpellID` IN (53652, 53654, 52752, 47753, 18562, 25516, 36240, 31850, 31851, 31852, 66235, 37641, 37036, 36132, 37120, 33031, 34121, 35181, 36971, 31249, 31250, 31304, 31341, 31944, 31608, 39878, 42052, 40314, 40175, 40175, 41303, 41360, 40827, 40869, 40870, 40871, 41001, 42005, 41078, 41131);
INSERT INTO `zone_difficulty_spelloverrides` (`SpellID`, `MapId`, `NerfValue`, `ModeMask`, `Comment`) VALUES
(53652, 0, 1, 65, 'Beacon of Light - Skip nerf.'),
(53654, 0, 1, 65, 'Beacon of Light 2 (Flash of Light, Holy Shock) - Skip nerf.'),
(52752, 0, 1, 65, 'Ancestral Awakening - Skip nerf.'),
(47753, 0, 1, 65, 'Divine Aegis - Skip nerf.'),
(18562, 0, 1, 65, 'Swiftmend - Skip nerf.'),
(25516, 0, 1, 65, 'AQ20 Andorov Healing - Skip nerf.'),
(36240, 0, 1, 65, 'Gruul Cave In - Skip buff.'),
(34435, 0, 1, 65, 'Magtheridon - Warder - Rain of Fire Skip buff.'),
(31850, 0, 1, 65, 'Ardent Defender - Absorb - Skip nerf'),
(31851, 0, 1, 65, 'Ardent Defender - Absorb - Skip nerf'),
(31852, 0, 1, 65, 'Ardent Defender - Absorb - Skip nerf'),
(66235, 0, 1, 65, 'Ardent Defender - Heal - Skip nerf'),
(37641, 548, 1, 65, 'Serpentshrine Cavern - Leotheras - Whirlwind (dot)'),
(37036, 550, 0.20, 65, 'Tempest Keep - Master Engineer Telonicus Bomb Damage Nerf'),
(36132, 550, 1, 65, 'Tempest Keep - Bloodwarder Marshal \'Whirlwind\''),
(37120, 550, 1, 65, 'Tempest Keep - Tempest-Smith \'Fragmentation Bomb\''),
(33031, 550, 1.15, 65, 'Tempest Keep - Solarian \'Arcane Missiles\''),
(34121, 550, 1, 65, 'Tempest Keep - Al\'ar Flame Buffet'),
(35181, 550, 2, 65, 'Tempest Keep - Al\'ar Dive Bomb spread damage'),
(36971, 550, 1, 65, 'Tempest Keep - Grand Astromancer Capernian - Fireball'),
(31249, 534, 1, 65, 'The Battle For Mount Hyjal - Rage Winterchill - Icebolt'),
(31250, 534, 1, 65, 'The Battle For Mount Hyjal - Rage Winterchill - Frost Nova'),
(31304, 534, 1, 65, 'The Battle For Mount Hyjal - Azgalor infernal - Immolation'),
(31341, 534, 1, 65, 'The Battle For Mount Hyjal - Azgalor - Rain of Fire (effect)'),
(31944, 534, 1, 65, 'The Battle For Mount Hyjal - Archimonde - Doomfire'),
(31608, 534, 1, 65, 'The Battle For Mount Hyjal - Abomination - Disease Cloud (tick debuff)'),
(39878, 564, 1, 65, 'Black Temple - Najentus - Tidal Burst Effect'),
(42052, 564, 1, 65, 'Black Temple - Supremus - Volcanic Geyser'),
(40314, 564, 1, 65, 'Black Temple - Vengeful Spirit - Spirit Volley'),
(40175, 564, 1, 65, 'Black Temple - Vengeful Spirit - Spirit Chains'),
(40157, 564, 1, 65, 'Black Temple - Vengeful Spirit - Spirit Lance'),
(41303, 564, 1, 65, 'Black Temple - Reliquary of the Lost - Essence of Suffering - Soul Drain'),
(41360, 564, 1, 65, 'Black Temple - Promenade Sentinel - Arcane charge'), -- may not work
(40827, 564, 1, 65, 'Black Temple - Mother Shahraz - Sinful Beam'),
(40869, 564, 1, 65, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(40870, 564, 1, 65, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(40871, 564, 1, 65, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(41001, 564, 1, 65, 'Black Temple - Mother Shahraz - Fatal Attraction'),
(42005, 564, 1, 65, 'Black Temple - Gurtogg Bloodboil - Bloodboil'),
(41078, 564, 1.1, 65, 'Black Temple - Illidan - Shadow Blast'),
(41131, 564, 1.1, 65, 'Black Temple - Illidan - Flame Burst'),
(43137, 568, 1, 65, 'Zul Aman - Zuljin - Zap'),
(43121, 568, 1, 65, 'Zul Aman - Zuljin - Cyclone'),
(43140, 568, 1, 65, 'Zul Aman - Janalai - Flame Breath'),
(45662, 580, 1, 65, 'Felmyst - Encapsulate'),
(45329, 580, 1, 65, 'Lady Sacrolash - Shadow Nova'),
(45342, 580, 1, 65, 'Grand Warlock Alythess - Conflagration (Direct hit)'),
(46768, 580, 1, 65, 'Grand Warlock Alythess - Conflagration (Area hit)');
