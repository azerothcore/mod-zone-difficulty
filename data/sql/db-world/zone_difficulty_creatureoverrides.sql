-- ************************************************************************
-- Possible values for Target
SET @TARGET_SELF = 1;											-- Self cast.
SET @TARGET_VICTIM = 2;										-- Our current target. (ie: highest aggro)
SET @TARGET_HOSTILE_SECOND_AGGRO = 3;		-- Second highest aggro.
SET @TARGET_HOSTILE_LAST_AGGRO = 4;			-- Lowest aggro.
SET @TARGET_HOSTILE_RANDOM = 5; 					-- Just any random player on our threat list.
SET @TARGET_HOSTILE_RANDOM_NOT_TOP = 6;	-- Just any random player on our threat list except the current target.

DROP TABLE IF EXISTS `zone_difficulty_creatureoverrides`;
CREATE TABLE `zone_difficulty_creatureoverrides` (
    `CreatureEntry` INT NOT NULL DEFAULT 0,
    `HPModifier` FLOAT NOT NULL DEFAULT 1,
    `Chance` TINYINT NOT NULL DEFAULT 100,
    `Spell` INT NOT NULL,
    `Target` TINYINT NOT NULL DEFAULT 1,
    `Delay` INT NOT NULL DEFAULT 1,
    `Cooldown` INT NOT NULL DEFAULT 1,
    `Repetitions` TINYINT NOT NULL DEFAULT 0,           -- 0 = forever, 1 = just once
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_creatureoverrides`;
INSERT INTO `zone_difficulty_creatureoverrides` (`CreatureEntry`, `HPModifier`, `Chance`,  `Spell`, `Target`, `Delay`, `Cooldown`, `Repetitions`, `Enabled`, `Comment`) VALUES
(1128001, 1, 0, 0, 0, 0, 0, 0, 1, 'Chromie NPC - prevent changing hp.'),
(18831, 2.5, 100, 19784, @TARGET_HOSTILE_RANDOM, 30000, 5000, 0, 1, 'Maulgar, Gruul\'s Lair HPx2.5 Dark Iron Bomb on a random player every 5s after 30s.'),
(19044, 2.8, 0, 0, 0, 0, 0 ,0, 1, 'Gruul, Gruul\'s Lair HPx2.8'),
(17257, 2.5, 0, 0, 0, 0, 0 ,0, 1, 'Magtheridon, Magtheridon\'s Lair HPx2.5');
