CREATE TABLE `zone_difficulty_completion_logs` (
    `guid` INT UNSIGNED NOT NULL,
    `type` TINYINT NOT NULL,
    `mode` TINYINT NOT NULL DEFAULT 0,
    PRIMARY KEY (`guid`, `type`)
);
