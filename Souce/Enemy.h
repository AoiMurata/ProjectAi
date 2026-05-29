// =============================================================================
// Enemy.h
// 敵キャラクター（各種バリエーション含む）の定義を行うヘッダーファイル
// =============================================================================
#pragma once

#include "EnemyAttack.h"

// 敵の移動モード
enum class EnemyMoveMode
{
	Normal, // 通常の徘徊状態
	Flee    // 被弾した際にプレイヤーの弾の軌道から逃走する状態
};

// 敵の種類（各バリエーション）
enum class EnemyType
{
	Normal,  // 通常敵
	MidBoss, // 中ボス（弾を発射する）
	Boss,    // 大ボス（弾を頻繁に発射する巨大な敵）
	Tank,    // タンク（HPが高く四角形の防御重視の敵）
	Assault, // アサルト（高スピードでプレイヤーへ突進する敵。死亡時にプレイヤーの所持弾を削る）
	Medic    // メディック（他の被弾した味方敵に向かって移動し、周囲の味方を回復させる敵）
};

// 敵キャラクターを管理するクラス
class Enemy
{
public:
	// 各種敵の初期設定（座標とHPを設定）
	void ResetNormal(float x, float y, int hpMin, int hpMax);
	void ResetMidBoss(float x, float y, int hp);
	void ResetBoss(float x, float y, int hp);
	void ResetTank(float x, float y, int hpMin, int hpMax);
	void ResetAssault(float x, float y, int hpMin, int hpMax);
	void ResetMedic(float x, float y, int hpMin, int hpMax);

	// 敵の毎フレーム更新処理（プレイヤー座標、弾攻撃配列、ブラックホール重力の効果など）
	void Update(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks, bool hasBlackhole = false, float bhX = 0.0f, float bhY = 0.0f);
	
	// 敵キャラクターの描画処理
	void Draw() const;

	// X座標を取得
	float GetX() const { return m_x; }
	// Y座標を取得
	float GetY() const { return m_y; }
	// 当たり判定の半径を取得
	float GetRadius() const { return m_radius; }
	// 現在のHPを取得
	int GetHp() const { return m_hp; }
	// 生存しているか判定
	bool IsAlive() const { return m_hp > 0; }
	// 敵の種類を取得
	EnemyType GetType() const { return m_type; }
	// ボス系（中ボス、大ボス）かどうかを判定
	bool IsBossType() const;

	// ダメージを与える処理
	void TakeDamage(int amount);
	// 氷の攻撃が命中した時の処理（逃走モードへの遷移）
	void OnHitByIce(float fromX, float fromY);

	// スロウ効果（アクティブスキル効果）の適用
	void ApplySlow(int durationFrames);
	// 継続ダメージ（アクティブスキル効果）の適用
	void ApplyDot(int durationFrames, int damagePerSecond);

	// HP回復処理
	void Heal(int amount);
	// 特定座標（主に回復対象の味方）に向けて移動ガイドする処理
	void GuideTowards(float tx, float ty);
	
	// 「今死んだばかりか（死亡エフェクト判定）」を取得
	bool GetJustDied() const { return m_justDied; }
	// 死亡判定フラグをクリア
	void ClearJustDied() { m_justDied = false; }
	// 最大HPを取得
	int GetMaxHp() const { return m_maxHp; }

	int m_healTimer = 0; // 回復の処理間隔を計測するタイマー

private:
	// ランダムな移動速度と方向を決定する
	void PickRandomVelocity();
	// 特定の座標から逆方向に逃走を開始する
	void StartFleeFrom(float fromX, float fromY);
	// 通常の徘徊移動に戻る
	void ReturnToNormalMovement();
	// 射撃（弾攻撃）の試行（ボス系のみ）
	void TryShoot(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks);

	float m_x = 0.0f;       // 現在X座標
	float m_y = 0.0f;       // 現在Y座標
	float m_vx = 0.0f;      // X方向の移動速度速度ベクトル
	float m_vy = 0.0f;      // Y方向の移動速度速度ベクトル
	float m_speed = 0.0f;   // 移動スピードの絶対値
	float m_radius = 0.0f;  // 当たり判定の半径
	int m_hp = 0;           // 現在HP
	int m_maxHp = 0;        // 最大HP
	bool m_justDied = false; // 死亡した瞬間のフレームであることを示すフラグ
	int m_attackCooldown = 0; // 攻撃のクールダウンタイマー
	EnemyMoveMode m_moveMode = EnemyMoveMode::Normal; // 現在の移動状態
	EnemyType m_type = EnemyType::Normal;             // 敵の種類

	// アクティブスキル（ステータス異常）用の変数
	int m_slowTimer = 0;         // スロウ効果の持続タイマー
	int m_dotTimer = 0;          // ドット（継続ダメージ）の持続タイマー
	int m_dotTickTimer = 0;      // 1秒に1回ダメージを与えるための計測タイマー
	int m_dotDamagePerTick = 0;  // 1回のドットダメージ量
};
