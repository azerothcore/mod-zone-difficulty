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
(22917, 3.2, 2.3, 1, 'Illidan, Black Temple HPx3.2');
