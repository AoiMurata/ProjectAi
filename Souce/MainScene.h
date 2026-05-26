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
#include "PlayerSettings.h"
#include "Ally.h"

enum class MainPhase
{
	Charge,
	Battle
};

enum class PauseState
{
	None,
	Main,
	Settings
};

struct AssaultExplosion
{
	float x = 0.0f;
	float y = 0.0f;
	float radius = 0.0f;
	int timer = 0;
	bool active = false;
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

	// Pause menu methods
	SceneType UpdatePauseMenu();
	void DrawPauseMenu() const;

	// Active skill helper
	void GetActiveSkillDetails(ColorPreset preset, int& outDuration, int& outCooldown, const char*& outName, const char*& outDesc) const;

	MainPhase m_phase = MainPhase::Charge;
	Player m_player;
	Ally m_ally;
	Enemy m_enemies[MAX_ENEMIES];
	EnemyAttack m_enemyAttacks[MAX_ENEMY_ATTACKS];
	IceProjectile m_iceShots[MAX_CHARGE_SHOTS];
	int m_shotBudget = 0;
	int m_shotsFired = 0;
	int m_autoFireCooldown = 0;
	int m_chargeFrame = 0;
	int m_stunFrames = 0;
	int m_activeEnemyCount = 0;

	// Pause menu variables
	bool m_isPaused = false;
	PauseState m_pauseState = PauseState::None;
	int m_pauseMenuIndex = 0;
	int m_pauseHoveredMenuIndex = -1;

	// Click to start flow
	bool m_roundStarted = false;

	// Active skills tracking
	bool m_skillActive = false;
	int m_skillActiveTimer = 0;
	int m_skillCooldownTimer = 0;
	ColorPreset m_currentSkillColor = ColorPreset::Blue;

	// Skill specific rendering variables
	float m_blackholeX = 0.0f;
	float m_blackholeY = 0.0f;
	int m_blackholeTimer = 0;

	float m_explosionX = 0.0f;
	float m_explosionY = 0.0f;
	float m_explosionDrawRadius = 0.0f;
	bool m_drawExplosion = false;
	int m_explosionDrawTimer = 0;

	AssaultExplosion m_assaultExplosions[10];
};
