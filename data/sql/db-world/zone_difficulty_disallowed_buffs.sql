CREATE TABLE IF NOT EXISTS `zone_difficulty_disallowed_buffs` (
    `MapID` INT NOT NULL DEFAULT 0,
    `DisallowedBuffs` TEXT,
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT,
    PRIMARY KEY (`MapID`)
);

DELETE FROM `zone_difficulty_disallowed_buffs` WHERE `MapID` IN (
-- BGs
-- Vanilla
30, 489, 529,

-- TBC
566,

-- WotLK
607, 628,

-- Arenas
-- TBC
559, 562, 572,

-- WotLK
617, 618,

-- Raids
-- Vanilla
409, 249, 469, 509, 309, 531,

-- TBC
532, 565, 544, 548, 550, 534, 564, 568, 580,

-- WotLK
533, 615, 616, 624, 603, 649, 631, 724,

-- Dungeons
-- TBC
543, 542, 540, 547, 546, 545, 557, 558, 556, 555, 560, 269, 554, 553, 552, 585,

-- WotLK
574, 576, 601, 619, 600, 608, 604, 599, 602, 578, 575, 595, 650, 632, 658, 668);
INSERT INTO `zone_difficulty_disallowed_buffs` (`MapID`, `DisallowedBuffs`, `Enabled`, `Comment`) VALUES
-- BGs
-- Vanilla
(30, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Alterac Valley: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(489, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Warsong Gulch: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(529, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Arathi Basin: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- TBC
(566, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Eye of the Storm: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- WotLK
(607, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Strand of the Ancients: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(628, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Isle of Conquest: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- Arenas
-- TBC
(559, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ring of Trials: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(562, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blade\'s Edge Arena: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(572, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ruins of Lordaeron: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- WotLK
(617, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Dalaran Arena: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(618, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ring of Valor: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- Raids
-- Vanilla
(409, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Molten Core: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(249, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Onyxia\'s Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(469, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackwing Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(509, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ruins of Ahn\'Qiraj: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(309, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Zul Gurub: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(531, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Temple of Ahn\'Qiraj: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- TBC
(532, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Karazhan: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(565, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Gruul\'s Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(544, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Magtheridon\'s Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(548, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Serpentshrine Cavern: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(550, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Tempest Keep: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(534, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Battle for Mount Hyjal: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(564, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Black Temple: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(568, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Zul\'Aman: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(580, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Sunwell Plateau: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- WotLK
(533, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Naxxramas: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(615, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Obsidian Sanctum: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(616, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Eye of Eternity: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(624, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Vault of Archavon: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(603, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ulduar: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(649, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Trial of the Crusader: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(631, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Icecrown Citadel: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(724, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Ruby Sanctum: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- Dungeons
-- Vanilla
-- (389, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ragefire Chasm: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (36, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Deadmines: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (43, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Wailing Caverns: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (33, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Shadowfang Keep: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (48, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackfathom Deeps: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (34, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Stockade: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (90, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Gnomeregan: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (47, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Razorfen Kraul: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (189, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Scarlet Monastery: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (129, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Razorfen Downs: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (70, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Uldaman: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (209, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Zul\'Farrak: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (349, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Maraudon: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (109, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Sunken Temple: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (230, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackrock Depths: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (229, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackrock Spire: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (429, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Dire Maul: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (289, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Scholomance: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
-- (329, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Stratholme: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- TBC
(543, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Hellfire Ramparts: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(542, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Blood Furnace: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(540, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Shattered Halls: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(547, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Slave Pens: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(546, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Underbog: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(545, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Steamvault: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(557, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Mana-Tombs: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(558, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Auchenai Crypts: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(556, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Sethekk Halls: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(555, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Shadow Labyrinth: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(560, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Old Hillsbrad Foothills: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(269, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Black Morass: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(554, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Mechanar: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(553, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Botanica: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(552, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Arcatraz: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(585, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Magisters\' Terrace: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),

-- WotLK
(574, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Utgarde Keep: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(576, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Nexus: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(601, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Azjol-Nerub: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(619, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ahn\'kahet: The Old Kingdom: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(600, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Drak\'Tharon Keep: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(608, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Violet Hold: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(604, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Gundrak: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(599, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Halls of Stone: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(602, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Halls of Lightning: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(578, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Oculus: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(575, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Utgarde Pinnacle: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(595, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Culling of Stratholme: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(650, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Trial of the Champion: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(632, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in The Forge of Souls: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(658, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Pit of Saron: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(668, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Halls of Reflection: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder');
