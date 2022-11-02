# Zone Difficulty Module

[English](README.md) | [Español](README_ES.md)


## What is the purpose of this module?
This module provides a way to nerf or debuff players in order to increase the difficulty of certain dungeons or balance battlegrounds.

## How to use this module?
This module reads data from the `zone_difficulty_info` table to apply nerfs/debuffs. For example, you can nerf healing inside the Temple of Ahn'Qiraj dungeon by 90% with the following query:

```sql
INSERT INTO `zone_difficulty_info` (`MapId`, `HealingNerfValue`, `Enabled`, `Comment`)
VALUES
(531, '0.10', 1, 'AQ40 Healing Nerf');
```

The output will be multiplied by the value you input in `HealingNerfValue` as it follows:
```
output = output * HealingNerfValue
```

## Changing values

You can edit the `zone_difficulty_info_content.sql` file to apply changes. They will be applied by the autoupdater the next time you restart your server.
Alternatively, you may edit the `zone_difficulty_info` table in your `world` database and use the `reload config` command to reload the values.
