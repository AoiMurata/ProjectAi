// =============================================================================
// MainScene.h
// =============================================================================
#pragma once

#include "SceneBase.h"
#include "Constants.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyAttack.h"
#include "IceProjectile.h"

enum class MainPhase
{
	Charge,
	Battle
};

class MainScene : public SceneBase
{
public:
	void OnEnter() override;
	void OnExit() override;
	SceneType Update() override;
	void Draw() override;

private:
	void SpawnEnemies();
	void UpdateEnemies();
	void UpdateEnemyAttacks();
	void UpdateIceProjectiles();
	void CheckIceEnemyCollisions();
	void CheckEnemyAttackPlayer();
	void StartBattlePhase();
	SceneType UpdateChargePhase();
	SceneType UpdateBattlePhase();
	void TryAutoFire();
	bool SpawnPlayerShot(float targetX, float targetY);
	bool AnyActiveIce() const;
	int CountAliveEnemies() const;
	int GetRemainingShots() const;
	void AddChargedShots();
	void ApplyAttackEffect(EnemyAttackEffect effect);
	const Enemy* FindNearestAliveEnemy(float fromX, float fromY) const;

	static bool CirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2);

	MainPhase m_phase = MainPhase::Charge;
	Player m_player;
	Enemy m_enemies[MAX_ENEMIES];
	EnemyAttack m_enemyAttacks[MAX_ENEMY_ATTACKS];
	IceProjectile m_iceShots[MAX_CHARGE_SHOTS];
	int m_shotBudget = 0;
	int m_shotsFired = 0;
	int m_autoFireCooldown = 0;
	int m_chargeFrame = 0;
	int m_stunFrames = 0;
	int m_activeEnemyCount = 0;
};
