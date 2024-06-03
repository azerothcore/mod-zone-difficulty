-- Column to flag log entry for GM/Dev players.
ALTER TABLE zone_difficulty_encounter_logs
ADD COLUMN IF NOT EXISTS IsDev TINYINT(3) NULL DEFAULT 0 COMMENT '';