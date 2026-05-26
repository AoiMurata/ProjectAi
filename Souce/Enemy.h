// =============================================================================
// Enemy.h
// =============================================================================
#pragma once

#include "EnemyAttack.h"

enum class EnemyMoveMode
{
	Normal,
	Flee
};

enum class EnemyType
{
	Normal,
	MidBoss,
	Boss,
	Tank,
	Assault,
	Medic
};

class Enemy
{
public:
	void ResetNormal(float x, float y, int hpMin, int hpMax);
	void ResetMidBoss(float x, float y, int hp);
	void ResetBoss(float x, float y, int hp);
	void ResetTank(float x, float y, int hpMin, int hpMax);
	void ResetAssault(float x, float y, int hpMin, int hpMax);
	void ResetMedic(float x, float y, int hpMin, int hpMax);
	void Update(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks, bool hasBlackhole = false, float bhX = 0.0f, float bhY = 0.0f);
	void Draw() const;

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetRadius() const { return m_radius; }
	int GetHp() const { return m_hp; }
	bool IsAlive() const { return m_hp > 0; }
	EnemyType GetType() const { return m_type; }
	bool IsBossType() const;

	void TakeDamage(int amount);
	void OnHitByIce(float fromX, float fromY);

	void ApplySlow(int durationFrames);
	void ApplyDot(int durationFrames, int damagePerSecond);

	void Heal(int amount);
	void GuideTowards(float tx, float ty);
	bool GetJustDied() const { return m_justDied; }
	void ClearJustDied() { m_justDied = false; }
	int GetMaxHp() const { return m_maxHp; }

	int m_healTimer = 0;

private:
	void PickRandomVelocity();
	void StartFleeFrom(float fromX, float fromY);
	void ReturnToNormalMovement();
	void TryShoot(float playerX, float playerY, EnemyAttack* attacks, int maxAttacks);

	float m_x = 0.0f;
	float m_y = 0.0f;
	float m_vx = 0.0f;
	float m_vy = 0.0f;
	float m_speed = 0.0f;
	float m_radius = 0.0f;
	int m_hp = 0;
	int m_maxHp = 0;
	bool m_justDied = false;
	int m_attackCooldown = 0;
	EnemyMoveMode m_moveMode = EnemyMoveMode::Normal;
	EnemyType m_type = EnemyType::Normal;

	// Active Skill status effects
	int m_slowTimer = 0;
	int m_dotTimer = 0;
	int m_dotTickTimer = 0;
	int m_dotDamagePerTick = 0;
};
