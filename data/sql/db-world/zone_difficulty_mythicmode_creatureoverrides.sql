DROP TABLE IF EXISTS `zone_difficulty_mythicmode_creatureoverrides`;
CREATE TABLE `zone_difficulty_mythicmode_creatureoverrides` (
    `CreatureEntry` INT NOT NULL DEFAULT 0,
    `HPModifier` FLOAT NOT NULL DEFAULT 1,
    `HPModifierNormal` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT UNSIGNED DEFAULT 1,
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_mythicmode_creatureoverrides`;
INSERT INTO `zone_difficulty_mythicmode_creatureoverrides` (`CreatureEntry`, `HPModifier`, `HPModifierNormal`, `Enabled`, `Comment`) VALUES
(1128001, 1, 1, 1, 'Chromie NPC - prevent changing hp.'),
(18831, 1.8, 1, 1, 'Maulgar, Gruul\'s Lair HPx1.8'),
(19044, 1.8, 1, 1, 'Gruul, Gruul\'s Lair HPx1.8'),
(17257, 1.8, 1, 1, 'Magtheridon, Magtheridon\'s Lair HPx1.8'),
(17881, 3.2, 1, 1, 'Aeonus, Black Morass HPx3.2'),
-- Serpentshrine Cavern
(21216, 1.5, 1, 1, 'Hydross, SSC HPx3.5'),
(21217, 1.5, 1, 1, 'Lurker, SSC HPx3.5'),
(21215, 1.5, 1, 1, 'Leotheras, SSC HPx3.5'),
(21213, 1.5, 1, 1, 'Morogrim, SSC HPx3.5'),
(21214, 1.5, 1, 1, 'Karathress, SSC HPx3.5'),
(21964, 1.5, 1, 1, 'Caribdis, SSC HPx2.5'),
(21966, 1.5, 1, 1, 'Sharkkis, SSC HPx2.5'),
(21965, 1.5, 1, 1, 'Tidalvess, SSC HPx2.5'),
(21212, 1.5, 1, 1, 'Vashj, SSC HPx3.5'),
-- Hyjal
(17767, 1.5, 1, 1, 'Rage Winterchill HPx3.5'),
(17808, 1.5, 1, 1, 'Anetheron SSC HPx3.5'),
(17888, 1.5, 1, 1, 'Kazrogal, SSC HPx3.5'),
(17842, 1.5, 1, 1, 'Azgalor, SSC HPx3.5'),
(17968, 1.5, 1, 1, 'Archimonde, SSC HPx3.5'),
-- Black Temple
(22887, 3.5, 2.2, 1, 'Najentus, Black Temple HPx3.5'),
(22898, 3.5, 2.2, 1, 'Supremus, Black Temple HPx3.5'),
(22841, 3.5, 2.2, 1, 'Shade of Akama, Black Temple HPx3.5'),
(22871, 3.2, 2.2, 1, 'Teron Gorefiend, Black Temple HPx3.2'),
(23111, 1, 1, 1, 'Shadowy Construct - prevent changing hp.'),
(22948, 3.2, 2.2, 1, 'Gurtogg Bloodboil, Black Temple HPx3.2'),
(22856, 3, 2.2, 1, 'Reliquary of the Lost, Black Temple HPx3'),
(23418, 3, 2.2, 1, 'Essence of Suffering, Black Temple HPx3'),
(23419, 2, 1.5, 1, 'Essence of Desire, Black Temple HPx2'),
(23420, 3, 2.2, 1, 'Essence of Anger, Black Temple HPx3'),
(22947, 3.5, 2.3, 1, 'Mother Shahraz, Black Temple HPx3.5'),
(23426, 3.5, 2.3, 1, 'Illidari Council, Black Temple HPx3.5'),
(22949, 3.5, 2.3, 1, 'Gathios, Black Temple HPx3.5'),
(22950, 3.5, 2.3, 1, 'Zerevor, Black Temple HPx3.5'),
(22951, 3.5, 2.3, 1, 'Malande, Black Temple HPx3.5'),
(22952, 3.5, 2.3, 1, 'Veras, Black Temple HPx3.5'),
(22917, 3.2, 2.3, 1, 'Illidan, Black Temple HPx3.2'),
-- Zul'aman
(23574, 3.5, 2.3, 1, 'Akilzon 3.5x mythic 2.3x normal hp'),
(23578, 3.5, 2.3, 1, 'Janalai 3.5x mythic 2.3x normal hp'),
(24239, 3.5, 2.3, 1, 'Hexlord 3.5x mythic 2.3x normal hp'),
(23577, 3.5, 2.3, 1, 'Halazzi (lynx) 3.5x mythic 2.3x normal hp'),
(24144, 3.5, 2.3, 1, 'Halazzi (troll) 3.5x mythic 2.3x normal hp'),
(24143, 3.5, 2.3, 1, 'Spirit of the Lynx 3.5x mythic 2.3x normal hp'),
(23576, 3.5, 2.3, 1, 'Narolakk 3.5x mythic 2.3x normal hp'),
(23863, 3.5, 2.5, 1, 'Zuljin 3.5x mythic 2.5x normal hp'),
-- Sunwell Plateau
(24850, 3.5, 2.3, 1, 'Kalecgos 3.5x mythic 2.3x normal hp'),
(24891, 3.5, 2.3, 1, 'Kalec 3.5x mythic 2.3x normal hp'),
(24892, 3.5, 2.3, 1, 'Sarthrovarr 3.5x mythic 2.3x normal hp'),
(24882, 2.3, 1.8, 1, 'Brutallus 2.3x mythic 1.8x normal hp'),
(25038, 2.3, 1.8, 1, 'Felmyst 2.3x mythic 1.8x normal hp'),
(25166, 3.5, 2.5, 1, 'Alythess 3.5x mythic 2.5x normal hp'),
(25165, 3.5, 2.5, 1, 'Sacrolash 3.5x mythic 2.5x normal hp'),
(25741, 3.5, 2.5, 1, 'Muru 3.5x mythic 2.5x normal hp'),
(25840, 3.5, 2.5, 1, 'Entropius 3.5x mythic 2.5x normal hp'),
(25315, 3.5, 2.5, 1, 'Kiljaeden 3.5x mythic 2.5x normal hp');

