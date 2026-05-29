// =============================================================================
// Ally.h
// =============================================================================
#pragma once
#include "PlayerSettings.h"
#include "Constants.h"
#include "Enemy.h"
#include "IceProjectile.h"

class Ally
{
public:
	void Reset(float x, float y);
	void Update(float px, float py, const Enemy* enemies, int activeEnemyCount, IceProjectile* iceShots, int maxShots, const EnemyAttack* attacks, int maxAttacks);
	void Draw() const;

	bool IsActive() const { return m_active; }
	void SetActive(bool active) { m_active = active; }
	
	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetRadius() const { return m_radius; }
	ColorPreset GetColorPreset() const { return m_color; }

	// アクティブスキルのゲッター
	bool IsSkillActive() const { return m_skillActive; }
	int GetSkillActiveTimer() const { return m_skillActiveTimer; }
	int GetSkillCooldownTimer() const { return m_skillCooldownTimer; }
	
	// ブラックホールの引き込み用変数
	float GetBlackholeX() const { return m_blackholeX; }
	float GetBlackholeY() const { return m_blackholeY; }
	bool IsBlackholeActive() const { return m_skillActive && (m_color == ColorPreset::Black); }

	// 爆発エフェクト用のパラメータ
	bool IsExplosionActive() const { return m_drawExplosion; }
	float GetExplosionX() const { return m_explosionX; }
	float GetExplosionY() const { return m_explosionY; }
	float GetExplosionRadius() const { return m_explosionDrawRadius; }

private:
	void TryAutoFire(const Enemy* enemies, int activeEnemyCount, IceProjectile* iceShots, int maxShots);
	void TriggerSkill(const Enemy* enemies, int activeEnemyCount);
	void GetActiveSkillDetails(ColorPreset preset, int& outDuration, int& outCooldown, const char*& outName, const char*& outDesc) const;
	const Enemy* FindNearestAliveEnemy(const Enemy* enemies, int activeEnemyCount) const;

	float m_x = 0.0f;
	float m_y = 0.0f;
	float m_vx = 0.0f;
	float m_vy = 0.0f;
	float m_radius = PLAYER_RADIUS;
	ColorPreset m_color = ColorPreset::Blue;
	bool m_active = false;

	// 自律移動AI用の目標座標パラメータ
	float m_destX = 0.0f;
	float m_destY = 0.0f;
	int m_destTimer = 0;

	// 射撃のクールダウン
	int m_autoFireCooldown = 0;

	// スキルのアクティブ・クールダウン用タイマー
	bool m_skillActive = false;
	int m_skillActiveTimer = 0;
	int m_skillCooldownTimer = 0;

	// スキル演出用のパラメータ
	float m_blackholeX = 0.0f;
	float m_blackholeY = 0.0f;
	
	float m_explosionX = 0.0f;
	float m_explosionY = 0.0f;
	float m_explosionDrawRadius = 0.0f;
	int m_explosionDrawTimer = 0;
	bool m_drawExplosion = false;
};
