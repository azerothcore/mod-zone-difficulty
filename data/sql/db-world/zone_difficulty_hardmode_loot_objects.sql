DROP TABLE IF EXISTS `zone_difficulty_instance_data`;
CREATE TABLE `zone_difficulty_instance_data` (
    `MapID` INT NOT NULL DEFAULT 0,
    `SourceEntry` INT NOT NULL,
    -- 0 = no override. Only required if the loot for an encounter is obtained
    -- from e.g. a chest instead of a corpse.
    `OverrideGO` INT NOT NULL DEFAULT 0,
    `InstanceType` TINYINT NOT NULL DEFAULT 0,
	PRIMARY KEY (`MapID`, `SourceEntry`)
);

INSERT INTO `zone_difficulty_instance_data` (`MapID`, `SourceEntry`, `OverrideGO`, `InstanceType`) VALUES
-- TBC Raids
(544, 17257, 0, 1),    -- Magtheridon, Magtheridon's Lair
(565, 18831, 0, 1),    -- High King Maulgar, Gruul's Lair
(565, 19044, 0, 1),    -- Gruul, Gruul's Lair
-- TBC 5man Heroics
(269, 17881, 0, 0),    -- Aeonus, The Black Morass
(540, 16808, 0, 0),    -- Warchief Kargath Bladefist, Shattered Halls
(542, 17377, 0, 0),    -- Keli'dan the Breaker, Blood Furnace
(543, 18433, 0, 0),    -- Omor the Unscarred, Hellfire Ramparts
(545, 17798, 0, 0),    -- Warlord Kalithresh, The Steamvault
(546, 17882, 0, 0),    -- The Black Stalker, The Underbog
(547, 17942, 0, 0),    -- Quagmirran, Slave Pens
(552, 20912, 0, 0),    -- Harbinger Sykriss, The Arcatraz
(553, 17977, 0, 0),    -- Warp Splinter, The Botanica
(554, 19220, 0, 0),    -- Pathaleon the Calculator, The Mechanar
(555, 18708, 0, 0),    -- Murmur, Shadow Labyrinth
(556, 18473, 0, 0),    -- Talon King Ikiss, Sethekk Halls
(557, 18344, 0, 0),    -- Nexus-Prince Shaffar, Mana-Tombs
(558, 18373, 0, 0),    -- Talon King Ikiss, Auchenai Crypts
(560, 18096, 0, 0),    -- The Escape From Durnholde
(585, 24664, 0, 0);    -- Karl-Heinz Sonnenfurz, Magister's Terrace
