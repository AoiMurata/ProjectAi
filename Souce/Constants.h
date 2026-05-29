// =============================================================================
// Constants.h
// ゲーム全体で使用される各種定数（サイズ、速度、スコア、上限値など）を定義するヘッダーファイル
// =============================================================================
#pragma once

// 画面解像度およびフレームレート
constexpr int SCREEN_WIDTH  = 1280; // 画面幅
constexpr int SCREEN_HEIGHT = 720;  // 画面高
constexpr int TARGET_FPS = 60;      // 目標FPS

// プレイヤーに関する定数
constexpr float PLAYER_SPEED   = 6.0f;  // プレイヤーの基本移動速度
constexpr float PLAYER_RADIUS  = 18.0f; // プレイヤーの当たり判定の半径
constexpr int   PLAYER_MAX_HP  = 5;     // プレイヤーの最大HP

// プレイヤーの攻撃（氷の弾）に関する定数
constexpr float ICE_SPEED      = 14.0f;               // 氷弾の基本移動速度
constexpr float ICE_RADIUS     = 10.0f;               // 氷弾の判定半径
constexpr int   ICE_DAMAGE_BASE = 1;                  // 氷弾の基本ダメージ
constexpr float HOMING_TURN_RATE_BASE = 0.04f;        // ホーミング旋回速度の初期値
constexpr float HOMING_TURN_RATE_PER_LEVEL = 0.018f;  // ホーミング旋回速度のレベル毎の上昇量
constexpr int   SPECIAL_SKILL_MAX_LEVEL = 10;         // 特殊スキルの最大レベル上限
constexpr int   AUTO_FIRE_INTERVAL = 15;              // 基本自動射撃フレーム間隔
constexpr int   AUTO_FIRE_INTERVAL_MIN = 5;           // 自動射撃間隔の最小値（限界速度）
constexpr int   FIRE_RATE_SKILL_REDUCTION = 2;        // スキル取得による射撃間隔の短縮値（フレーム数）

// 敵キャラクターに関する定数
constexpr float ENEMY_RADIUS       = 22.0f; // 一般敵の判定半径
constexpr float MID_BOSS_RADIUS    = 32.0f; // 中ボスの判定半径
constexpr float BOSS_RADIUS        = 42.0f; // 大ボスの判定半径
constexpr int   ENEMY_HP_MIN       = 4;     // ラウンド1における一般敵の最小HP
constexpr int   ENEMY_HP_MAX       = 10;    // ラウンド1における一般敵の最大HP
constexpr int   ENEMY_HP_BONUS_MIN = 5;     // 一般敵の追加HPボーなスの下限（ラウンドクリア毎）
constexpr int   ENEMY_HP_BONUS_MAX = 7;     // 一般敵の追加HPボーなスの上限（ラウンドクリア毎）
constexpr int   ROUND_HIGH_HP_THRESHOLD = 10; // 敵HP上昇率が飛躍的に高まるラウンド閾値
constexpr int   ENEMY_HP_BONUS_HIGH_MIN = 20; // 閾値超え後の追加HPボーナス下限
constexpr int   ENEMY_HP_BONUS_HIGH_MAX = 30; // 閾値超え後の追加HPボーナス上限

// 敵の移動スピード（ゲーム進行とともに難易度が高くなりすぎないように制限されています）
constexpr float ENEMY_SPEED_MIN  = 2.0f;      // 一般敵の徘徊速度下限
constexpr float ENEMY_SPEED_MAX  = 7.0f;      // 一般敵の徘徊速度上限
constexpr float ENEMY_FLEE_SPEED_MIN = 4.0f;  // 被弾時の逃走速度下限
constexpr float ENEMY_FLEE_SPEED_MAX = 9.0f;  // 被弾時の逃走速度上限
constexpr int   MAX_ENEMIES      = 80;        // 同時に出現する敵の最大上限数

// ボス（中ボス・大ボス）に関する定数
constexpr int   MID_BOSS_BASE_HP = 100;                 // 中ボスの基本初期HP
constexpr int   BOSS_BASE_HP     = 200;                 // 大ボスの基本初期HP
constexpr int   MID_BOSS_HP_GAIN_PER_APPEARANCE = 150;  // 中ボス出現毎のHP上昇補正
constexpr int   BOSS_HP_GAIN_PER_APPEARANCE = 200;      // 大ボス出現毎のHP上昇補正
constexpr int   MID_BOSS_ROUND_INTERVAL = 5;            // 中ボスが出現するラウンド間隔（5, 10, 15...）
constexpr int   BOSS_ROUND_INTERVAL = 10;               // 大ボスが出現するラウンド間隔（10, 20, 30...）

// 敵の攻撃（弾）に関する定数
constexpr int   MID_BOSS_ATTACK_INTERVAL = 90; // 中ボスの射撃クールダウン（フレーム）
constexpr int   BOSS_ATTACK_INTERVAL = 60;     // 大ボスの射撃クールダウン（フレーム）
constexpr float ENEMY_ATTACK_SPEED = 7.0f;     // 敵弾の移動速度
constexpr float ENEMY_ATTACK_RADIUS = 12.0f;   // 敵弾の判定半径
constexpr int   MAX_ENEMY_ATTACKS = 40;        // 画面上に存在できる敵弾の最大上限数
constexpr int   PLAYER_STUN_FRAMES = TARGET_FPS; // 黄色の弾（スタン）による硬直時間（1秒）
constexpr int   SHOT_DRAIN_ON_HIT = 3;         // ピンクの弾被弾によるプレイヤー残弾の減少数

// スコア加算に関する定数
constexpr int   SCORE_PER_ROUND = 500;          // ラウンドクリア時の基本加算スコア
constexpr int   SCORE_PER_REMAINING_SHOT = 10;  // クリア時の余剰残弾数ボーナス（1発当たり）
constexpr int   SCORE_BOSS_BONUS = 2000;        // 大ボス討伐時のボーナススコア
constexpr int   SCORE_MID_BOSS_BONUS = 1000;     // 中ボス討伐時のボーナススコア

// チャージ（ボール準備）フェーズに関する定数
constexpr int   CHARGE_TIME_SECONDS = 10;                               // チャージ時間（秒）
constexpr int   CHARGE_TIME_FRAMES  = CHARGE_TIME_SECONDS * TARGET_FPS; // チャージ時間（フレーム換算）
constexpr int   MAX_CHARGE_SHOTS    = 600;                              // 最大ストック可能弾数上限
