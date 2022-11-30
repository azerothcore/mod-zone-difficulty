DELETE FROM `zone_difficulty_info`;
INSERT INTO `zone_difficulty_info` (`MapId`, `PhaseMask`, `HealingNerfValue`, `AbsorbNerfValue`, `MeleeDmgBuffValue`, `SpellDmgBuffValue`, `Enabled`, `Comment`) VALUES
(531, 0, '0.50', '0.50', '1.50', '1.30', 1, 'AQ40 Healing 50% / Absorb 50% Nerf / 50% physical & 30% spell damage buff'),
(509, 0, '0.50', '0.50', '1.60', '1.30', 1, 'AQ20 Healing 50% / Absorb 50% Nerf / 60% physical & 30% spell damage buff'),
(309, 0, '0.50', '0.50', '1.50', '1.10', 1, 'ZG20 Healing 50% / Absorb 50% Nerf / 50% physical & 10% spell damage buff'),
(469, 0, '0.50', '0.50', '1.50', '1.50', 1, 'BWL  Healing 50% / Absorb 50% Nerf / 50% physical & 50% spell damage buff'),
(249, 0, '0.50', '0.50', '1.40', '1.40', 1, 'ONY  Healing 50% / Absorb 50% Nerf / 40% physical & 40% spell damage buff'),
(409, 0, '0.50', '0.50', '1.50', '1.50', 1, 'MC   Healing 50% / Absorb 50% Nerf / 50% physical & 50% spell damage buff');
