DELETE FROM `zone_difficulty_info`;
INSERT INTO `zone_difficulty_info` (`MapId`, `HealingNerfValue`, `AbsorbNerfValue`, `MeleeDmgBuffValue`, `SpellDmgBuffValue`, `Enabled`, `Comment`) VALUES
(531, '0.10', '0.10', '1.40', '1.40', 1, 'AQ40 Healing 90% / Absorb 90% Nerf / 40% physical & spell damage buff'),
(2147483647, '0.10', '0.10', '0.5', '0.5', 1, 'Duel Healing 90% / Absorb 90% Nerf / 50% physical & spell damage nerf');
