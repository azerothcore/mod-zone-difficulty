-- ************************************************************************
-- Possible values for `Target`:
SET @TARGET_SELF = 1;                       -- Self cast.
SET @TARGET_VICTIM = 2;                     -- Our current target. (ie: highest aggro)
SET @TARGET_HOSTILE_SECOND_AGGRO = 3;       -- Second highest aggro.
SET @TARGET_HOSTILE_LAST_AGGRO = 4;         -- Lowest aggro.
SET @TARGET_HOSTILE_RANDOM = 5;             -- Just any random player on our threat list.
SET @TARGET_HOSTILE_RANDOM_NOT_TOP = 6;     -- Just any random player on our threat list except the current target.
-- ************************************************************************

DROP TABLE IF EXISTS `zone_difficulty_hardmode_ai`;
CREATE TABLE `zone_difficulty_hardmode_ai` (
    `CreatureEntry` INT NOT NULL DEFAULT 0,
    `Chance` TINYINT NOT NULL DEFAULT 100,          -- 0-100% chance for the creature to gain this spell
    `Spell` INT NOT NULL,                           -- spell id
    `Target` TINYINT NOT NULL DEFAULT 1,            -- see above
    `Delay` INT NOT NULL DEFAULT 1,                 -- time in ms before first cast
    `Cooldown` INT NOT NULL DEFAULT 1,              -- time in ms between casts
    `Repetitions` TINYINT NOT NULL DEFAULT 0,       -- 0 = forever, 1 = just once
    `Enabled` TINYINT DEFAULT 1,                    -- 0 = disabled, 1 = enabled
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_hardmode_ai`;
INSERT INTO `zone_difficulty_hardmode_ai` (`CreatureEntry`, `Chance`, `Spell`, `Target`, `Delay`, `Cooldown`, `Repetitions`, `Enabled`, `Comment`) VALUES
(18831, 100, 19784, @TARGET_HOSTILE_RANDOM, 30000, 5000, 0, 1, 'Maulgar, Gruul\'s Lair. Dark Iron Bomb on a random player every 5s after 30s.'),
(19044, 0, 0, 0, 0, 0 ,0, 1, 'Gruul, Gruul\'s Lair.'),
(17257, 0, 0, 0, 0, 0 ,0, 1, 'Magtheridon, Magtheridon\'s Lair.');
