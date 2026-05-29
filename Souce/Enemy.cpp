// =============================================================================
// Enemy.cpp
// 各種敵キャラクターの挙動、移動AI、ダメージ・ステータス更新処理の実装ファイル
// =============================================================================
#include "Enemy.h"
#include "Constants.h"
#include "DxLib.h"
#include <cmath>

namespace
{
	// 指定範囲内のランダムな浮動小数点数を返すヘルパー関数
	float RandomRange(float minVal, float maxVal)
	{
		if (maxVal <= minVal)
		{
			return minVal;
		}
		const int steps = 1000;
		const int roll = GetRand(steps + 1);
		return minVal + (maxVal - minVal) * ((float)roll / (float)steps);
	}

	// 指定されたスピードで、ランダムな方向の速度ベクトル（vx, vy）を生成する
	void PickRandomDirection(float speed, float& outVx, float& outVy)
	{
		const float angle = RandomRange(0.0f, 6.2831853f);
		outVx = cosf(angle) * speed;
		outVy = sinf(angle) * speed;
	}
}

// 敵の徘徊速度と方向をランダムに再選択する
void Enemy::PickRandomVelocity()
{
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX);
	PickRandomDirection(m_speed, m_vx, m_vy);
}

// 被弾逃走などの状態から、通常のランダム徘徊移動に戻す
void Enemy::ReturnToNormalMovement()
{
	m_moveMode = EnemyMoveMode::Normal;
	PickRandomVelocity();
}

// 指定した座標から遠ざかる（逃走する）移動ベクトルを設定する
void Enemy::StartFleeFrom(float fromX, float fromY)
{
	float dx = m_x - fromX;
	float dy = m_y - fromY;
	const float len = sqrtf(dx * dx + dy * dy);

	if (len < 0.001f)
	{
		const float angle = RandomRange(0.0f, 6.2831853f);
		dx = cosf(angle);
		dy = sinf(angle);
	}
	else
	{
		dx /= len;
		dy /= len;
	}

	// 逃走用のスピード範囲から速度を設定し、逃走移動モードに変更
	m_speed = RandomRange(ENEMY_FLEE_SPEED_MIN, ENEMY_FLEE_SPEED_MAX);
	m_vx = dx * m_speed;
	m_vy = dy * m_speed;
	m_moveMode = EnemyMoveMode::Flee;
}

// 通常の敵の初期化処理
void Enemy::ResetNormal(float x, float y, int hpMin, int hpMax)
{
	m_type = EnemyType::Normal;
	m_x = x;
	m_y = y;
	m_radius = ENEMY_RADIUS;
	m_attackCooldown = 0;
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;

	if (hpMax < hpMin)
	{
		hpMax = hpMin;
	}
	const int range = hpMax - hpMin + 1;
	m_hp = (range > 1) ? (GetRand(range) + hpMin) : hpMin;
	m_maxHp = m_hp;
	m_justDied = false;
	m_moveMode = EnemyMoveMode::Normal;
	PickRandomVelocity();
}

// 中ボスの初期化処理
void Enemy::ResetMidBoss(float x, float y, int hp)
{
	m_type = EnemyType::MidBoss;
	m_x = x;
	m_y = y;
	m_radius = MID_BOSS_RADIUS;
	m_hp = hp;
	m_maxHp = hp;
	m_justDied = false;
	m_attackCooldown = 30; // 出現直後の攻撃待機フレーム
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX * 0.8f);
	PickRandomDirection(m_speed, m_vx, m_vy);
}

// 大ボスの初期化処理
void Enemy::ResetBoss(float x, float y, int hp)
{
	m_type = EnemyType::Boss;
	m_x = x;
	m_y = y;
	m_radius = BOSS_RADIUS;
	m_hp = hp;
	m_maxHp = hp;
	m_justDied = false;
	m_attackCooldown = 20; // 出現直後の攻撃待機フレーム
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN * 0.5f, ENEMY_SPEED_MAX * 0.6f);
	PickRandomDirection(m_speed, m_vx, m_vy);
}

// タンクタイプの敵の初期化処理
void Enemy::ResetTank(float x, float y, int hpMin, int hpMax)
{
	m_type = EnemyType::Tank;
	m_x = x;
	m_y = y;
	m_radius = ENEMY_RADIUS * 1.15f; // 通常より少し大きめのサイズ
	m_attackCooldown = 0;
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;

	if (hpMax < hpMin)
	{
		hpMax = hpMin;
	}
	const int range = hpMax - hpMin + 1;
	const int baseHp = (range > 1) ? (GetRand(range) + hpMin) : hpMin;
	m_hp = baseHp + 35; // 高い耐久力（HPボーナス）
	m_maxHp = m_hp;
	m_justDied = false;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) * 0.7f; // 移動スピードはやや遅め
	PickRandomDirection(m_speed, m_vx, m_vy);
}

// アサルトタイプの敵の初期化処理
void Enemy::ResetAssault(float x, float y, int hpMin, int hpMax)
{
	m_type = EnemyType::Assault;
	m_x = x;
	m_y = y;
	m_radius = ENEMY_RADIUS * 0.85f; // 通常より少し小さめのサイズ
	m_attackCooldown = 0;
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;

	if (hpMax < hpMin)
	{
		hpMax = hpMin;
	}
	const int range = hpMax - hpMin + 1;
	const int baseHp = (range > 1) ? (GetRand(range) + hpMin) : hpMin;
	m_hp = (int)(baseHp * 0.4f); // 耐久力は低め
	if (m_hp < 1) m_hp = 1;
	m_maxHp = m_hp;
	m_justDied = false;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) * 1.25f; // 高速で移動
	PickRandomDirection(m_speed, m_vx, m_vy);
}

// メディックタイプの敵の初期化処理
void Enemy::ResetMedic(float x, float y, int hpMin, int hpMax)
{
	m_type = EnemyType::Medic;
	m_x = x;
	m_y = y;
	m_radius = ENEMY_RADIUS;
	m_attackCooldown = 0;
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;

	if (hpMax < hpMin)
	{
		hpMax = hpMin;
	}
	const int range = hpMax - hpMin + 1;
	m_hp = (range > 1) ? (GetRand(range) + hpMin) : hpMin;
	m_maxHp = m_hp;
	m_justDied = false;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) * 0.9f;
	PickRandomDirection(m_speed, m_vx, m_vy);
}

// ボス系キャラクターであるかを判定する（中ボス・大ボスが対象）
bool Enemy::IsBossType() const
{
	return m_type == EnemyType::MidBoss || m_type == EnemyType::Boss;
}

// ボス系が弾攻撃を試行する処理
void Enemy::TryShoot(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks)
{
	if (!IsBossType() || !IsAlive())
	{
		return;
	}

	// クールダウン中の場合はタイマーを進めて終了
	if (m_attackCooldown > 0)
	{
		--m_attackCooldown;
		return;
	}

	// 敵の種類に応じて攻撃間隔を選択
	const int interval = (m_type == EnemyType::Boss)
		? BOSS_ATTACK_INTERVAL
		: MID_BOSS_ATTACK_INTERVAL;

	// 空いている攻撃バッファを見つけ、プレイヤー座標へ向けて射撃
	for (int i = 0; i < maxAttacks; ++i)
	{
		if (!attacks[i].active)
		{
			attacks[i].Spawn(m_x, m_y, playerX, playerY);
			m_attackCooldown = interval;
			return;
		}
	}
}

// 敵キャラクターの毎フレーム更新処理
void Enemy::Update(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks, bool hasBlackhole, float bhX, float bhY)
{
	if (!IsAlive())
	{
		return;
	}

	// ドットダメージ（継続ダメージ）の更新
	if (m_dotTimer > 0)
	{
		--m_dotTimer;
		++m_dotTickTimer;
		if (m_dotTickTimer >= 60) // 60フレーム（約1秒）に1回ダメージを適用
		{
			TakeDamage(m_dotDamagePerTick);
			m_dotTickTimer = 0;
		}
	}

	// ボス系射撃処理の試行
	TryShoot(playerX, playerY, attacks, maxAttacks);

	// アサルトタイプは、通常移動時に常にプレイヤーの座標を追尾する
	if (m_type == EnemyType::Assault && m_moveMode == EnemyMoveMode::Normal)
	{
		float dx = playerX - m_x;
		float dy = playerY - m_y;
		float len = sqrtf(dx * dx + dy * dy);
		if (len > 0.1f)
		{
			m_vx = (dx / len) * m_speed;
			m_vy = (dy / len) * m_speed;
		}
	}

	// スロウ効果による移動速度制限の適用
	float speedMult = 1.0f;
	if (m_slowTimer > 0)
	{
		--m_slowTimer;
		speedMult = 0.4f; // 移動速度を60%カット（0.4倍速）
	}

	// 座標の更新
	m_x += m_vx * speedMult;
	m_y += m_vy * speedMult;

	// ブラックホール吸引スキルの影響下にあれば、その中心点へ向けて引き寄せる
	if (hasBlackhole)
	{
		float dx = bhX - m_x;
		float dy = bhY - m_y;
		const float dist = sqrtf(dx * dx + dy * dy);
		if (dist > 5.0f)
		{
			dx /= dist;
			dy /= dist;
			m_x += dx * 3.5f; // 引き寄せ速度
			m_y += dy * 3.5f;
		}
	}

	// 画面境界での衝突・跳ね返り処理
	const float margin = m_radius;
	bool hitWall = false;

	if (m_x < margin) { m_x = margin; hitWall = true; }
	else if (m_x > SCREEN_WIDTH - margin) { m_x = SCREEN_WIDTH - margin; hitWall = true; }
	if (m_y < margin) { m_y = margin; hitWall = true; }
	else if (m_y > SCREEN_HEIGHT - margin) { m_y = SCREEN_HEIGHT - margin; hitWall = true; }

	// 壁に当たった場合、逃走モードなら通常徘徊に戻し、通常状態なら新しいランダム移動ベクトルを選択
	if (hitWall)
	{
		if (m_moveMode == EnemyMoveMode::Flee)
		{
			ReturnToNormalMovement();
		}
		else
		{
			PickRandomVelocity();
		}
	}
}

// ダメージを受ける処理
void Enemy::TakeDamage(int amount)
{
	if (m_hp <= 0)
	{
		return;
	}
	m_hp -= amount;
	if (m_hp <= 0)
	{
		m_hp = 0;
		m_justDied = true; // 死亡エフェクト発動用フラグを設定
	}
}

// 氷弾が命中した際の処理（命中地点から逆方向へ逃走）
void Enemy::OnHitByIce(float fromX, float fromY)
{
	if (!IsAlive())
	{
		return;
	}
	StartFleeFrom(fromX, fromY);
}

// スロウ効果時間を設定する
void Enemy::ApplySlow(int durationFrames)
{
	m_slowTimer = durationFrames;
}

// 継続ダメージを設定する
void Enemy::ApplyDot(int durationFrames, int damagePerSecond)
{
	m_dotTimer = durationFrames;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = damagePerSecond;
}

// 回復を受ける処理
void Enemy::Heal(int amount)
{
	if (!IsAlive()) return;
	m_hp += amount;
	if (m_hp > m_maxHp)
	{
		m_hp = m_maxHp; // 最大HPを超えないようにクリップ
	}
}

// メディック用の目的地追跡処理（味方の座標へ向かって移動方向を固定する）
void Enemy::GuideTowards(float tx, float ty)
{
	if (m_type == EnemyType::Medic && m_moveMode == EnemyMoveMode::Normal)
	{
		float dx = tx - m_x;
		float dy = ty - m_y;
		float len = sqrtf(dx * dx + dy * dy);
		if (len > 0.1f)
		{
			m_vx = (dx / len) * m_speed;
			m_vy = (dy / len) * m_speed;
		}
	}
}

// 敵キャラクターの描画
void Enemy::Draw() const
{
	if (!IsAlive())
	{
		return;
	}

	int bodyColor;
	int outlineColor;
	const char* label = nullptr;

	// 敵の種類や移動モードに合わせて配色とラベルテキストを設定
	switch (m_type)
	{
	case EnemyType::MidBoss:
		bodyColor = GetColor(200, 100, 255);
		outlineColor = GetColor(80, 20, 120);
		label = "MID";
		break;
	case EnemyType::Boss:
		bodyColor = GetColor(255, 60, 60);
		outlineColor = GetColor(120, 0, 0);
		label = "BOSS";
		break;
	case EnemyType::Tank:
		bodyColor = (m_moveMode == EnemyMoveMode::Flee) ? GetColor(170, 170, 170) : GetColor(110, 110, 110);
		outlineColor = GetColor(50, 50, 50);
		label = "TANK";
		break;
	case EnemyType::Assault:
		bodyColor = (m_moveMode == EnemyMoveMode::Flee) ? GetColor(255, 180, 100) : GetColor(255, 120, 30);
		outlineColor = GetColor(150, 60, 0);
		label = "ASLT";
		break;
	case EnemyType::Medic:
		bodyColor = (m_moveMode == EnemyMoveMode::Flee) ? GetColor(140, 255, 140) : GetColor(40, 200, 40);
		outlineColor = GetColor(10, 100, 10);
		label = "MEDC";
		break;
	default:
		bodyColor = (m_moveMode == EnemyMoveMode::Flee)
			? GetColor(255, 140, 140)
			: GetColor(220, 80, 80);
		outlineColor = GetColor(120, 20, 20);
		break;
	}

	// タンクタイプは頑強さを表すために四角形で描画し、それ以外は円形で描画する
	if (m_type == EnemyType::Tank)
	{
		DrawBox((int)(m_x - m_radius), (int)(m_y - m_radius), (int)(m_x + m_radius), (int)(m_y + m_radius), bodyColor, TRUE);
		DrawBox((int)(m_x - m_radius), (int)(m_y - m_radius), (int)(m_x + m_radius), (int)(m_y + m_radius), outlineColor, FALSE);
	}
	else
	{
		DrawCircle((int)m_x, (int)m_y, (int)m_radius, bodyColor, TRUE);
		DrawCircle((int)m_x, (int)m_y, (int)m_radius, outlineColor, FALSE);
	}

	// HPの値をキャラクターの頭上にテキスト表示
	const int hpColor = GetColor(255, 255, 200);
	DrawFormatString((int)m_x - 12, (int)m_y - (int)m_radius - 24, hpColor, "%d", m_hp);

	// ボスや特殊敵の種別を示すラベル名表示
	if (label != nullptr)
	{
		DrawFormatString((int)m_x - 18, (int)m_y + (int)m_radius + 4,
			GetColor(255, 220, 120), "%s", label);
	}
}
