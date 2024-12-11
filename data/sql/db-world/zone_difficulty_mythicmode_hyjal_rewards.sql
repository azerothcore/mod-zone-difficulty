SET @PRICE := 15;
SET @PRICE2 := 20;
DELETE FROM `zone_difficulty_mythicmode_rewards` WHERE `ContentType` = 18;
INSERT INTO `zone_difficulty_mythicmode_rewards` (`ContentType`, `ItemType`, `Entry`, `Price`, `Enchant`, `EnchantSlot`, `Achievement`, `Enabled`, `Comment`) VALUES
(18, 2, 30913, @PRICE2, 0, 0, 695, 1, 'Robes of Rhonin - Archimonde'),
(18, 2, 30885, @PRICE, 0, 0, 695, 1, 'Archbishop\'s Slippers - Anetheron'),
(18, 2, 30894, @PRICE, 0, 0, 695, 1, 'Blue Suede Shoes - Kazrogal'),
(18, 2, 30912, @PRICE2, 0, 0, 695, 1, 'Leggings of Eternity - Archimonde'),
(18, 2, 30916, @PRICE, 0, 0, 695, 1, 'Leggings of Channeled Elements - Kazrogal'),
(18, 2, 30884, @PRICE, 0, 0, 695, 1, 'Hatefury Mantle - Anetheron'),
(18, 2, 30888, @PRICE, 0, 0, 695, 1, 'Anetheron\'s Noose - Anetheron'),
(18, 2, 30895, @PRICE, 0, 0, 695, 1, 'Angelista\'s Sash - Kazrogal'),
(18, 2, 30870, @PRICE, 0, 0, 695, 1, 'Cuffs of Devastation - Rage Winterchill'),
(18, 2, 30871, @PRICE, 0, 0, 695, 1, 'Bracers of Martyrdom - Rage Winterchill'),

(18, 3, 30905, @PRICE2, 0, 0, 695, 1, 'Midnight Chestguard - Archimonde'),
(18, 3, 30899, @PRICE, 0, 0, 695, 1, 'Don Rodrigo\'s Poncho - Azgalor'),
(18, 3, 30886, @PRICE, 0, 0, 695, 1, 'Enchanted Leather Sandals - Anetheron'),
(18, 3, 30891, @PRICE, 0, 0, 695, 1, 'Black Featherlight Boots - Kazrogal'),
(18, 3, 30898, @PRICE, 0, 0, 695, 1, 'Shady Dealer\'s Pantaloons - Azgalor'),
(18, 3, 30917, @PRICE, 0, 0, 695, 1, 'Razorfury Mantle - Kazrogal'),
(18, 3, 30879, @PRICE, 0, 0, 695, 1, 'Don Alejandro\'s Money Belt - Anetheron'),
(18, 3, 30914, @PRICE, 0, 0, 695, 1, 'Belt of the Crescent Moon - Kazrogal'),
(18, 3, 30863, @PRICE, 0, 0, 695, 1, 'Deadly Cuffs - Rage Winterchill'),
(18, 3, 30868, @PRICE, 0, 0, 695, 1, 'Rejuvenating Bracers - Rage Winterchill'),

(18, 4, 30887, @PRICE, 0, 0, 695, 1, 'Golden Links of Restoration - Anetheron'),
(18, 4, 30907, @PRICE2, 0, 0, 695, 1, 'Mail of Fevered Pursuit - Archimonde'),
(18, 4, 30880, @PRICE, 0, 0, 695, 1, 'Quickstrider Moccasins - Anetheron'),
(18, 4, 30873, @PRICE, 0, 0, 695, 1, 'Stillwater Boots - Rage Winterchill'),
(18, 4, 30900, @PRICE, 0, 0, 695, 1, 'Bow-stitched Leggings - Azgalor'),
(18, 4, 30893, @PRICE, 0, 0, 695, 1, 'Sun-touched Chain Leggings - Kaz\'rogal'),
(18, 4, 30892, @PRICE, 0, 0, 695, 1, 'Beast-tamer\'s Shoulders - Kaz\'rogal'),
(18, 4, 30919, @PRICE, 0, 0, 695, 1, 'Valestalker Girdle - Kaz\'rogal'),
(18, 4, 30864, @PRICE, 0, 0, 695, 1, 'Bracers of the Pathfinder - Rage Winterchill'),
(18, 4, 30869, @PRICE, 0, 0, 695, 1, 'Howling Wind Bracers - Rage Winterchill');
