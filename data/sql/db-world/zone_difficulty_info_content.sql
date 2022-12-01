DELETE FROM `zone_difficulty_info`;
INSERT INTO `zone_difficulty_info` (`MapId`, `PhaseMask`, `HealingNerfValue`, `AbsorbNerfValue`, `MeleeDmgBuffValue`, `SpellDmgBuffValue`, `Enabled`, `Comment`) VALUES
-- Raids
(531, 0, '0.50', '0.50', '1.50', '1.30', 1, 'AQ40 Healing 50% / Absorb 50% Nerf / 50% physical & 30% spell damage buff'),
(509, 0, '0.50', '0.50', '1.60', '1.30', 1, 'AQ20 Healing 50% / Absorb 50% Nerf / 60% physical & 30% spell damage buff'),
(309, 0, '0.50', '0.50', '1.50', '1.10', 1, 'ZG20 Healing 50% / Absorb 50% Nerf / 50% physical & 10% spell damage buff'),
(469, 0, '0.50', '0.50', '1.50', '1.50', 1, 'BWL Healing 50% / Absorb 50% Nerf / 50% physical & 50% spell damage buff'),
(249, 0, '0.50', '0.50', '1.40', '1.40', 1, 'ONY Healing 50% / Absorb 50% Nerf / 40% physical & 40% spell damage buff'),
(409, 0, '0.50', '0.50', '1.50', '1.50', 1, 'MC Healing 50% / Absorb 50% Nerf / 50% physical & 50% spell damage buff'),
-- Battlegrounds
(30, 0, '0.80', '0.80', '0.70', '0.70', 1, 'AV Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(489, 0, '0.80', '0.80', '0.70', '0.70', 1, 'WSG Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(529, 0, '0.80', '0.80', '0.70', '0.70', 1, 'AB Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(556, 0, '0.80', '0.80', '0.70', '0.70', 1, 'EotS Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
-- Duels in Forbidding Sea (Wetlands)
(2147483647, 0, '0.80', '0.80', '0.70', '0.70', 1, 'Zone 2402 Duel Healing 20% / Absorb 20% Nerf / 30% physical & spell damage nerf');
