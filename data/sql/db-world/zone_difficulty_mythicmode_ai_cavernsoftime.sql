DELETE FROM `zone_difficulty_mythicmode_ai` WHERE CreatureEntry IN (21136, 21137, 21138, 21139, 17879, 17881, 18701);
INSERT INTO `zone_difficulty_mythicmode_ai` (`CreatureEntry`, `Chance`, `Spell`, `Spellbp0`, `Spellbp1`, `Spellbp2`, `Target`, `TargetArg`, `TargetArg2`, `Delay`, `Cooldown`, `Repetitions`, `Enabled`, `TriggeredCast`, `Comment`) VALUES
-- Black Morrass Trash
(21136, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Chronomancer, Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% chance.'),
(21137, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Assassin, Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% chance.'),
(21138, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Executioner Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% chance.'),
(21139, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Vanquisher, Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% chance.'),
-- Black Morrass Bosses
(17879, 100, 55948, 0, 0, 0, @TARGET_SELF, 0, 0, 15000, 10000, 0, 1, 1, 'Chrono Lord Deja, Black Morrass. Grow on self after 15s every 10s.'),
(17881, 100, 32663, 0, 0, 0, @TARGET_SELF, 0, 0, 15000, 8000, 0, 1, 1, 'Aeonus, Black Morrass. Summon Dark Vortex after 15s every 8s.'),
(18701, 100, 7, 0, 0, 0, @TARGET_SELF, 0, 2, 60000, 0, 1, 1, 1, 'Dark Vortex, summoned by Aeonus Black, Morrass (Line above). Kill self after 60s.'),
(18701, 30, 9906, 0, 0, 0, @TARGET_SELF, 0, 2, 3000, 10000, 0, 1, 1, 'Dark Vortex, summoned by Aeonus Black, Morrass (Line above). Reflect for 5s every 10s after 3s. 30% Chance.'),
(18701, 30, 63240, 0, 0, 0, @TARGET_SELF, 0, 2, 1000, 20000, 0, 1, 1, 'Dark Vorte, summoned by Aeonus Black, Morrass (Line above). Thorns for 10s every 20s after 1s. 30% Chance.');