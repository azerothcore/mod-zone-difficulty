DROP TABLE IF EXISTS `zone_difficulty_disallowed_buffs`;
CREATE TABLE `zone_difficulty_disallowed_buffs` (
    `MapID` INT NOT NULL DEFAULT 0,
    `DisallowedBuffs` TEXT,
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT,
    PRIMARY KEY (`MapID`)
);

DELETE FROM `zone_difficulty_disallowed_buffs`;
INSERT INTO `zone_difficulty_disallowed_buffs` (`MapID`, `DisallowedBuffs`, `Enabled`, `Comment`) VALUES
# (229, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackrock Spire: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(409, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Molten Core: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(249, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Onyxia\'s Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(469, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackwing Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(509, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ruins of Ahn\'Qiraj: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(309, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Zul Gurub: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(531, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Temple of Ahn\'Qiraj: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder');
