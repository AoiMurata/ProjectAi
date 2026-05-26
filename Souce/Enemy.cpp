// =============================================================================
// Enemy.cpp
// =============================================================================
#include "Enemy.h"
#include "Constants.h"
#include "DxLib.h"
#include <cmath>

namespace
{
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

	void PickRandomDirection(float speed, float& outVx, float& outVy)
	{
		const float angle = RandomRange(0.0f, 6.2831853f);
		outVx = cosf(angle) * speed;
		outVy = sinf(angle) * speed;
	}
}

void Enemy::PickRandomVelocity()
{
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX);
	PickRandomDirection(m_speed, m_vx, m_vy);
}

void Enemy::ReturnToNormalMovement()
{
	m_moveMode = EnemyMoveMode::Normal;
	PickRandomVelocity();
}

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

	m_speed = RandomRange(ENEMY_FLEE_SPEED_MIN, ENEMY_FLEE_SPEED_MAX);
	m_vx = dx * m_speed;
	m_vy = dy * m_speed;
	m_moveMode = EnemyMoveMode::Flee;
}

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

void Enemy::ResetMidBoss(float x, float y, int hp)
{
	m_type = EnemyType::MidBoss;
	m_x = x;
	m_y = y;
	m_radius = MID_BOSS_RADIUS;
	m_hp = hp;
	m_maxHp = hp;
	m_justDied = false;
	m_attackCooldown = 30;
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX * 0.8f);
	PickRandomDirection(m_speed, m_vx, m_vy);
}

void Enemy::ResetBoss(float x, float y, int hp)
{
	m_type = EnemyType::Boss;
	m_x = x;
	m_y = y;
	m_radius = BOSS_RADIUS;
	m_hp = hp;
	m_maxHp = hp;
	m_justDied = false;
	m_attackCooldown = 20;
	m_slowTimer = 0;
	m_dotTimer = 0;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = 0;
	m_healTimer = 0;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN * 0.5f, ENEMY_SPEED_MAX * 0.6f);
	PickRandomDirection(m_speed, m_vx, m_vy);
}

void Enemy::ResetTank(float x, float y, int hpMin, int hpMax)
{
	m_type = EnemyType::Tank;
	m_x = x;
	m_y = y;
	m_radius = ENEMY_RADIUS * 1.15f;
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
	m_hp = baseHp + 35;
	m_maxHp = m_hp;
	m_justDied = false;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) * 0.7f;
	PickRandomDirection(m_speed, m_vx, m_vy);
}

void Enemy::ResetAssault(float x, float y, int hpMin, int hpMax)
{
	m_type = EnemyType::Assault;
	m_x = x;
	m_y = y;
	m_radius = ENEMY_RADIUS * 0.85f;
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
	m_hp = (int)(baseHp * 0.4f);
	if (m_hp < 1) m_hp = 1;
	m_maxHp = m_hp;
	m_justDied = false;
	m_moveMode = EnemyMoveMode::Normal;
	m_speed = RandomRange(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) * 1.25f;
	PickRandomDirection(m_speed, m_vx, m_vy);
}

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

bool Enemy::IsBossType() const
{
	return m_type == EnemyType::MidBoss || m_type == EnemyType::Boss;
}

void Enemy::TryShoot(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks)
{
	if (!IsBossType() || !IsAlive())
	{
		return;
	}

	if (m_attackCooldown > 0)
	{
		--m_attackCooldown;
		return;
	}

	const int interval = (m_type == EnemyType::Boss)
		? BOSS_ATTACK_INTERVAL
		: MID_BOSS_ATTACK_INTERVAL;

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

void Enemy::Update(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks, bool hasBlackhole, float bhX, float bhY)
{
	if (!IsAlive())
	{
		return;
	}

	// Update DOT ticks
	if (m_dotTimer > 0)
	{
		--m_dotTimer;
		++m_dotTickTimer;
		if (m_dotTickTimer >= 60) // Ticks once every 1 second
		{
			TakeDamage(m_dotDamagePerTick);
			m_dotTickTimer = 0;
		}
	}

	TryShoot(playerX, playerY, attacks, maxAttacks);

	// Assault type tracks towards player if normal movement
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

	// Apply slow effect
	float speedMult = 1.0f;
	if (m_slowTimer > 0)
	{
		--m_slowTimer;
		speedMult = 0.4f; // 60% slow down
	}

	m_x += m_vx * speedMult;
	m_y += m_vy * speedMult;

	// Pull toward black hole gravity center if active
	if (hasBlackhole)
	{
		float dx = bhX - m_x;
		float dy = bhY - m_y;
		const float dist = sqrtf(dx * dx + dy * dy);
		if (dist > 5.0f)
		{
			dx /= dist;
			dy /= dist;
			m_x += dx * 3.5f; // Pull speed
			m_y += dy * 3.5f;
		}
	}

	const float margin = m_radius;
	bool hitWall = false;

	if (m_x < margin) { m_x = margin; hitWall = true; }
	else if (m_x > SCREEN_WIDTH - margin) { m_x = SCREEN_WIDTH - margin; hitWall = true; }
	if (m_y < margin) { m_y = margin; hitWall = true; }
	else if (m_y > SCREEN_HEIGHT - margin) { m_y = SCREEN_HEIGHT - margin; hitWall = true; }

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
		m_justDied = true;
	}
}

void Enemy::OnHitByIce(float fromX, float fromY)
{
	if (!IsAlive())
	{
		return;
	}
	StartFleeFrom(fromX, fromY);
}

void Enemy::ApplySlow(int durationFrames)
{
	m_slowTimer = durationFrames;
}

void Enemy::ApplyDot(int durationFrames, int damagePerSecond)
{
	m_dotTimer = durationFrames;
	m_dotTickTimer = 0;
	m_dotDamagePerTick = damagePerSecond;
}

void Enemy::Heal(int amount)
{
	if (!IsAlive()) return;
	m_hp += amount;
	if (m_hp > m_maxHp)
	{
		m_hp = m_maxHp;
	}
}

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

void Enemy::Draw() const
{
	if (!IsAlive())
	{
		return;
	}

	int bodyColor;
	int outlineColor;
	const char* label = nullptr;

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

	const int hpColor = GetColor(255, 255, 200);
	DrawFormatString((int)m_x - 12, (int)m_y - (int)m_radius - 24, hpColor, "%d", m_hp);

	if (label != nullptr)
	{
		DrawFormatString((int)m_x - 18, (int)m_y + (int)m_radius + 4,
			GetColor(255, 220, 120), "%s", label);
	}
}
