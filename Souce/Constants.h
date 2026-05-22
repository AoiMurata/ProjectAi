// =============================================================================
// Constants.h
// =============================================================================
#pragma once

/// This file contains all the constant values used throughout the game, such as
constexpr int SCREEN_WIDTH  = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int TARGET_FPS = 60;

// Player constants
constexpr float PLAYER_SPEED   = 6.0f;
constexpr float PLAYER_RADIUS  = 18.0f;
constexpr int   PLAYER_MAX_HP  = 5;

// Shot constants
constexpr float ICE_SPEED      = 14.0f;
constexpr float ICE_RADIUS     = 10.0f;
constexpr int   ICE_DAMAGE_BASE = 1;
constexpr float HOMING_TURN_RATE_BASE = 0.04f;
constexpr float HOMING_TURN_RATE_PER_LEVEL = 0.018f;
constexpr int   SPECIAL_SKILL_MAX_LEVEL = 10;
constexpr int   AUTO_FIRE_INTERVAL = 15;
constexpr int   AUTO_FIRE_INTERVAL_MIN = 5;
constexpr int   FIRE_RATE_SKILL_REDUCTION = 2;


// Enemy constants
constexpr float ENEMY_RADIUS       = 22.0f;
constexpr float MID_BOSS_RADIUS    = 32.0f;
constexpr float BOSS_RADIUS        = 42.0f;
constexpr int   ENEMY_HP_MIN       = 4;
constexpr int   ENEMY_HP_MAX       = 10;
constexpr int   ENEMY_HP_BONUS_MIN = 5;
constexpr int   ENEMY_HP_BONUS_MAX = 7;
constexpr int   ROUND_HIGH_HP_THRESHOLD = 10;
constexpr int   ENEMY_HP_BONUS_HIGH_MIN = 20;
constexpr int   ENEMY_HP_BONUS_HIGH_MAX = 30;

// Enemy speed increases with rounds, but is capped to prevent it from becoming unmanageable.
constexpr float ENEMY_SPEED_MIN  = 2.0f;
constexpr float ENEMY_SPEED_MAX  = 7.0f;
constexpr float ENEMY_FLEE_SPEED_MIN = 4.0f;
constexpr float ENEMY_FLEE_SPEED_MAX = 9.0f;
constexpr int   MAX_ENEMIES      = 7;

//	Boss constants
constexpr int   MID_BOSS_BASE_HP = 100;
constexpr int   BOSS_BASE_HP     = 200;
constexpr int   MID_BOSS_HP_GAIN_PER_APPEARANCE = 150;
constexpr int   BOSS_HP_GAIN_PER_APPEARANCE = 200;
constexpr int   MID_BOSS_ROUND_INTERVAL = 5;
constexpr int   BOSS_ROUND_INTERVAL = 10;


// Attack constants
constexpr int   MID_BOSS_ATTACK_INTERVAL = 90;
constexpr int   BOSS_ATTACK_INTERVAL = 60;
constexpr float ENEMY_ATTACK_SPEED = 7.0f;
constexpr float ENEMY_ATTACK_RADIUS = 12.0f;
constexpr int   MAX_ENEMY_ATTACKS = 40;
constexpr int   PLAYER_STUN_FRAMES = TARGET_FPS;
constexpr int   SHOT_DRAIN_ON_HIT = 3;


// Scoring constants
constexpr int   SCORE_PER_ROUND = 500;
constexpr int   SCORE_PER_REMAINING_SHOT = 10;
constexpr int   SCORE_BOSS_BONUS = 2000;
constexpr int   SCORE_MID_BOSS_BONUS = 1000;
// Charging constants
constexpr int   CHARGE_TIME_SECONDS = 10;
constexpr int   CHARGE_TIME_FRAMES  = CHARGE_TIME_SECONDS * TARGET_FPS;
constexpr int   MAX_CHARGE_SHOTS    = 200;
