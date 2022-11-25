# Zone Difficulty Module

[English](README.md) | [Español](README_ES.md)


## What is the purpose of this module?
This module provides a way to nerf or debuff players in order to increase the difficulty of certain dungeons or balance battlegrounds/duels.

## How to use this module?
This module reads data from the `zone_difficulty_info` and `zone_difficulty_spelloverrides` table.
`_info` serves to apply nerfs/debuffs per map and per phase. For example, you can nerf healing inside the Temple of Ahn'Qiraj dungeon for all phases (0 = Skip PhaseMask check, 6 = phases 2 and 4) by 90% with the following query:

```sql
INSERT INTO `zone_difficulty_info` (`MapId`, `PhaseMask`, HealingNerfValue`, `Enabled`, `Comment`)
VALUES
(531, 0, '0.10', 1, 'AQ40 Healing Nerf');
```

The output will be multiplied by the value you input in `HealingNerfValue` as it follows:

Using `MapId` 2147483647 will be used for all targets in duels while they're in the zone hardcoded as `DUEL_AREA` (default 2402: Forbidding Sea, Wetlands).

```
output = output * HealingNerfValue
```

You can also prevent certain spells from being affected at all. See `zone_difficulty_spelloverrides.sql` for examples.

## Changing values

You can edit the `zone_difficulty_info_content.sql` file to apply changes. They will be applied by the autoupdater the next time you restart your server.
Alternatively, you may edit the `zone_difficulty_info` table in your `world` database and use the `reload config` command to reload the values.

## Authors

- [Nyeriah](https://github.com/Nyeriah)
- [Honey](https://github.com/55Honey)
