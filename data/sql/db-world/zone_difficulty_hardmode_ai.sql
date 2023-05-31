-- ************************************************************************
-- Possible values for `Target`:
SET @TARGET_NONE = 0;                       -- For spells without target.
SET @TARGET_SELF = 1;                       -- Self cast.
SET @TARGET_VICTIM = 2;                     -- Our current target. (ie: highest aggro)
SET @TARGET_HOSTILE_AGGRO_FROM_TOP= 3;      -- Position (TargetArg2) in the threat list counted from top. TargetArg = max/min range. Fallback to GetVictim().
SET @TARGET_HOSTILE_AGGRO_FROM_BOTTOM = 4;  -- Position (TargetArg2) in the threat list counted from bottom. TargetArg = max/min range. Fallback to GetVictim().
SET @TARGET_HOSTILE_RANDOM = 5;             -- Just any random player on our threat list. TargetArg = max/min range.
SET @TARGET_HOSTILE_RANDOM_NOT_TOP = 6;     -- Just any random player on our threat list except the current target. TargetArg = max/min range.
SET @TARGET_PLAYER_DISTANCE = 18;           -- All players in range. TargetArg = max/min range.
-- ************************************************************************

DROP TABLE IF EXISTS `zone_difficulty_hardmode_ai`;
CREATE TABLE `zone_difficulty_hardmode_ai` (
    `CreatureEntry` INT NOT NULL DEFAULT 0,
    `Chance` TINYINT NOT NULL DEFAULT 100,          -- 0-100% chance for the creature to gain this spell
    `Spell` INT NOT NULL,                           -- spell id
    `Spellbp0` INT NOT NULL DEFAULT 0,              -- custom spell value bp0
    `Spellbp1` INT NOT NULL DEFAULT 0,              -- custom spell value bp1
    `Spellbp2` INT NOT NULL DEFAULT 0,              -- custom spell value bp2
    `Target` TINYINT NOT NULL DEFAULT 1,            -- see above
    `TargetArg` INT NOT NULL DEFAULT 0,             -- optional argument for the target. Max/min range
    `TargetArg2` INT NOT NULL DEFAULT 0,            -- optional argument for the target. Counter in the threat list for position
    `Delay` INT NOT NULL DEFAULT 1,                 -- time in ms before first cast
    `Cooldown` INT NOT NULL DEFAULT 1,              -- time in ms between casts
    `Repetitions` TINYINT NOT NULL DEFAULT 0,       -- 0 = forever, 1 = just once. Room for future counters.
    `Enabled` TINYINT DEFAULT 1,                    -- 0 = disabled, 1 = enabled
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_hardmode_ai`;
INSERT INTO `zone_difficulty_hardmode_ai` (`CreatureEntry`, `Chance`, `Spell`, `Spellbp0`, `Spellbp1`, `Spellbp2`, `Target`, `TargetArg`, `TargetArg2`, `Delay`, `Cooldown`, `Repetitions`, `Enabled`, `Comment`) VALUES
(18831, 100, 19784, 0, 0, 0, @TARGET_HOSTILE_RANDOM, 0, 0, 30000, 5000, 0, 1, 'Maulgar, Gruul\'s Lair. Dark Iron Bomb on a random player after 30s every 5s.'),
(18832, 100, 6726, 0, 0, 0, @TARGET_PLAYER_DISTANCE, 0, 50, 28000, 30000, 0, 1, 'Krosh Firehand, Gruul\'s Lair. 5sec Silence on all players in 50m after 28s every 30s.'),
(18834, 100, 69969, 0, 0, 0, @TARGET_PLAYER_DISTANCE, 0, 50, 58000, 60000, 0, 1, 'Olm the Summoner, Gruul\'s Lair. Curse of Doom (12s) on all players in 50m after 58s every 60s.'),
(19044, 100, 39965, 500, 0, 0, @TARGET_PLAYER_DISTANCE, 0, 10, 33000, 30000, 0, 1, 'Gruul, Gruul\'s Lair. Frost Grenade on all players in 10m after 33s every 30s.'),
(19044, 100, 51758, 0, 0, 0, @TARGET_SELF, 0, 0, 25000, 120000, 0, 1, 'Gruul, Gruul\'s Lair. Fire Reflection on self after 25s every 120s.'),
(19044, 100, 51763, 0, 0, 0, @TARGET_SELF, 0, 0, 55000, 120000, 0, 1, 'Gruul, Gruul\'s Lair. Frost Reflection on self after 55s every 120s.'),
(19044, 100, 51764, 0, 0, 0, @TARGET_SELF, 0, 0, 85000, 120000, 0, 1, 'Gruul, Gruul\'s Lair. Shadow Reflection on self after 85s every 120s.'),
(19044, 100, 51766, 0, 0, 0, @TARGET_SELF, 0, 0, 115000, 120000, 0, 1, 'Gruul, Gruul\'s Lair. Arcane Reflection on self after 115s every 120s.'),
(19389, 30, 20508, 0, 0, 0, @TARGET_HOSTILE_RANDOM_NOT_TOP, 0, 0, 5000, 12000, 0, 1, 'Lair Brute, Gruul\'s Lair. Charge with AE knockback on a random player except the MT after 5s every 12s.'),
(21350, 30, 851, 0, 0, 0, @TARGET_HOSTILE_RANDOM_NOT_TOP, 0, 0, 5000, 2000, 0, 1, 'Gronn-Priest, Gruul\'s Lair. Sheep on a random player except the MT after 5s every 2s.'),
(17257, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 1, 'Magtheridon, Magtheridon\'s Lair.'),
(21350, 100, 38019, 0, 0, 0, @TARGET_SELF, 0, 0, 5000, 10000, 0, 1, 'Gronn-Priest, Gruul\'s Lair. Summon Water Elemental after 5s every 10s.');
