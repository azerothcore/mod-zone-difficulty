SET @PRICE := 24;
SET @PRICE2 := 30;
DELETE FROM `zone_difficulty_mythicmode_rewards` WHERE `ContentType` = 12;
INSERT INTO `zone_difficulty_mythicmode_rewards` (`ContentType`, `ItemType`, `Entry`, `Price`, `Enchant`, `EnchantSlot`, `Achievement`, `Enabled`, `Comment`) VALUES
(12, 1, 33281, @PRICE, 0, 0, 691, 1, 'Brooch of Nature\'s Mercy - Akil\'zon'),
(12, 1, 33293, @PRICE, 0, 0, 691, 1, 'Signet of Ancient Magics - Akil\'zon'),
(12, 1, 33297, @PRICE, 0, 0, 691, 1, 'The Savage\'s Choker - Halazzi'),
(12, 1, 33592, @PRICE, 0, 0, 691, 1, 'Cloak of Ancient Rituals - Malacrass'),
(12, 1, 33829, @PRICE, 0, 0, 691, 1, 'Hex Shrunken Head - Malacrass'),
(12, 1, 34029, @PRICE, 0, 0, 691, 1, 'Tiny Voodoo Mask - Malacrass'),
(12, 1, 33828, @PRICE, 0, 0, 691, 1, 'Tome of Diabolic Remedy - Malacrass'),
(12, 1, 33830, @PRICE2, 0, 0, 691, 1, 'Ancient Aqir Artifact - Zul\'jin'),
(12, 1, 33831, @PRICE2, 0, 0, 691, 1, 'Berserker\'s Call - Zul\'jin'),
(12, 1, 33466, @PRICE2, 0, 0, 691, 1, 'Loop of Cursed Bones - Zul\'jin'),

(12, 2, 33285, @PRICE, 0, 0, 691, 1, 'Fury of the Ursine - Nalorakk'),
(12, 2, 33203, @PRICE, 0, 0, 691, 1, 'Robes of Heavenly Purpose - Nalorakk'),
(12, 2, 33357, @PRICE, 0, 0, 691, 1, 'Footpads of Madness - Jan\'alai'),
(12, 2, 33317, @PRICE, 0, 0, 691, 1, 'Robe of Departed Spirits - Halazzi'),
(12, 2, 33453, @PRICE, 0, 0, 691, 1, 'Hood of Hexing - Malacrass'),
(12, 2, 33463, @PRICE, 0, 0, 691, 1, 'Hood of the Third Eye - Malacrass'),
(12, 2, 33471, @PRICE2, 0, 0, 691, 1, 'Two-toed Sandals - Zul\'jin'),

(12, 3, 33211, @PRICE, 0, 0, 691, 1, 'Bladeangel\'s Money Belt - Nalorakk'),
(12, 3, 33356, @PRICE, 0, 0, 691, 1, 'Helm of Natural Regeneration - Jan\'alai'),
(12, 3, 33329, @PRICE, 0, 0, 691, 1, 'Shadowtooth Trollskin Cuirass - Jan\'alai'),
(12, 3, 33322, @PRICE, 0, 0, 691, 1, 'Shimmer-pelt Vest - Halazzi'),
(12, 3, 33300, @PRICE, 0, 0, 691, 1, 'Shoulderpads of Dancing Blades - Halazzi'),
(12, 3, 33479, @PRICE2, 0, 0, 691, 1, 'Grimgrin Faceguard - Zul\'jin'),

(12, 4, 33286, @PRICE, 0, 0, 691, 1, 'Mojo-mender\'s Mask - Akil\'zon'),
(12, 4, 33206, @PRICE, 0, 0, 691, 1, 'Pauldrons of Primal Fury - Nalorakk'),
(12, 4, 33328, @PRICE, 0, 0, 691, 1, 'Arrow-fall Chestguard - Jan\'alai'),
(12, 4, 33533, @PRICE, 0, 0, 691, 1, 'Avalanche Leggings - Halazzi'),
(12, 4, 33432, @PRICE, 0, 0, 691, 1, 'Coif of the Jungle Stalker - Malacrass'),
(12, 4, 33464, @PRICE, 0, 0, 691, 1, 'Hex Lord\'s Voodoo Pauldrons - Malacrass'),
(12, 4, 33469, @PRICE2, 0, 0, 691, 1, 'Hauberk of the Empire\'s Champion - Zul\'jin'),

(12, 5, 33215, @PRICE, 0, 0, 691, 1, 'Bloodstained Elven Battlevest - Akil\'zon'),
(12, 5, 33216, @PRICE, 0, 0, 691, 1, 'Chestguard of Hidden Purpose - Akil\'zon'),
(12, 5, 33191, @PRICE, 0, 0, 691, 1, 'Jungle Stompers - Nalorakk'),
(12, 5, 33327, @PRICE, 0, 0, 691, 1, 'Mask of Introspection - Nalorakk'),
(12, 5, 33303, @PRICE, 0, 0, 691, 1, 'Skullshatter Warboots - Halazzi'),
(12, 5, 33299, @PRICE, 0, 0, 691, 1, 'Spaulders of the Advocate - Halazzi'),
(12, 5, 33421, @PRICE, 0, 0, 691, 1, 'Battleworn Tuskguard - Malacrass'),
(12, 5, 33446, @PRICE, 0, 0, 691, 1, 'Girdle of Stromgarde\'s Hope - Malacrass'),
(12, 5, 33473, @PRICE2, 0, 0, 691, 1, 'Chestguard of the Warlord - Zul\'jin'),

(12, 6, 33214, @PRICE, 0, 0, 691, 1, 'Akil\'zon\'s Talonblade - Akil\'zon'),
(12, 6, 33283, @PRICE, 0, 0, 691, 1, 'Amani Punisher - Akil\'zon'),
(12, 6, 33640, @PRICE, 0, 0, 691, 1, 'Fury - Nalorakk'),
(12, 6, 33326, @PRICE, 0, 0, 691, 1, 'Bulwark of the Amani Empire - Jan\'alai'),
(12, 6, 33332, @PRICE, 0, 0, 691, 1, 'Enamelled Disc of Mojo - Jan\'alai'),
(12, 6, 33354, @PRICE, 0, 0, 691, 1, 'Wub\'s Cursed Hexblade - Jan\'alai'),
(12, 6, 33389, @PRICE, 0, 0, 691, 1, 'Dagger of Bad Mojo - Malacrass'),
(12, 6, 33388, @PRICE, 0, 0, 691, 1, 'Heartless - Malacrass'),
(12, 6, 33298, @PRICE, 0, 0, 691, 1, 'Prowler\' Strikeblade - Malacrass'),
(12, 6, 33465, @PRICE, 0, 0, 691, 1, 'Staff of Primal Fury - Malacrass'),
(12, 6, 33474, @PRICE2, 0, 0, 691, 1, 'Ancient Amani Longbow - Zul\'jin'),
(12, 6, 33467, @PRICE2, 0, 0, 691, 1, 'Blade of Twisted Visions - Zul\'jin'),
(12, 6, 33476, @PRICE2, 0, 0, 691, 1, 'Cleaver of the Unforgiving - Zul\'jin'),
(12, 6, 33468, @PRICE2, 0, 0, 691, 1, 'Dark Blessing - Zul\'jin'),
(12, 6, 33478, @PRICE2, 0, 0, 691, 1, 'Jin\'rohk, The Great Apocalypse - Zul\'jin');
