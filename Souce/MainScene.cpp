// =============================================================================
// MainScene.cpp
// =============================================================================
#include "MainScene.h"
#include "InputManager.h"
#include "GameSession.h"
#include "Constants.h"
#include "DxLib.h"
#include <cmath>

void MainScene::OnEnter()
{
	m_phase = MainPhase::Charge;
	m_shotBudget = 0;
	m_shotsFired = 0;
	m_autoFireCooldown = 0;
	m_chargeFrame = 0;
	m_stunFrames = 0;

	m_player.Reset(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.75f);

	for (auto& attack : m_enemyAttacks)
	{
		attack.active = false;
	}
	for (auto& shot : m_iceShots)
	{
		shot.active = false;
	}

	SpawnEnemies();

	// Reset pause state
	m_isPaused = false;
	m_pauseState = PauseState::None;
	m_pauseMenuIndex = 0;
	m_pauseHoveredMenuIndex = -1;

	// Reset round start
	m_roundStarted = false;

	// Reset active skill states
	m_skillActive = false;
	m_skillActiveTimer = 0;
	m_skillCooldownTimer = 0;
	m_currentSkillColor = PlayerSettings::GetSelectedPreset();

	if (GameSession::GetRound() > 20)
	{
		m_ally.Reset(m_player.GetX() - 80.0f, m_player.GetY() + 40.0f);
		m_ally.SetActive(true);
	}
	else
	{
		m_ally.SetActive(false);
	}

	// Reset skill visual variables
	m_blackholeTimer = 0;
	m_drawExplosion = false;
	m_explosionDrawTimer = 0;

	for (auto& exp : m_assaultExplosions)
	{
		exp.active = false;
	}
}

void MainScene::OnExit()
{
}

void MainScene::SpawnEnemies()
{
	for (auto& enemy : m_enemies)
	{
		enemy.ResetNormal(0.0f, 0.0f, 0, 0);
	}

	const int round = GameSession::GetRound();
	const RoundSpawnType spawnType = GameSession::GetRoundSpawnType(round);
	const int hpMin = GameSession::GetEnemyHpMin();
	const int hpMax = GameSession::GetEnemyHpMax();

	// Round increases enemy volume: start 7, +1 every 5 rounds
	int totalEnemies = 7 + (round - 1) / 5;
	if (round > 20)
	{
		totalEnemies = (int)(totalEnemies * 1.8f);
	}
	if (totalEnemies > MAX_ENEMIES)
	{
		totalEnemies = MAX_ENEMIES;
	}

	m_activeEnemyCount = totalEnemies;

	int medicLimit = (round <= 10) ? 1 : 3;
	int spawnedMedics = 0;

	// Helper lambda to pick type and reset enemy
	auto spawnMix = [&](int i, float x, float y) {
		int roll = GetRand(99);
		if (roll < 60)
		{
			m_enemies[i].ResetNormal(x, y, hpMin, hpMax);
		}
		else
		{
			// named/special: Tank, Assault, Medic
			if (round <= 10)
			{
				if (spawnedMedics < medicLimit)
				{
					m_enemies[i].ResetMedic(x, y, hpMin, hpMax);
					++spawnedMedics;
				}
				else
				{
					m_enemies[i].ResetNormal(x, y, hpMin, hpMax);
				}
			}
			else
			{
				// choose between Tank, Assault, Medic
				int choice = GetRand(2);
				if (choice == 2)
				{
					if (spawnedMedics < medicLimit)
					{
						m_enemies[i].ResetMedic(x, y, hpMin, hpMax);
						++spawnedMedics;
					}
					else
					{
						// fallback to Tank or Assault
						int choice2 = GetRand(1);
						if (choice2 == 0)
						{
							m_enemies[i].ResetTank(x, y, hpMin, hpMax);
						}
						else
						{
							m_enemies[i].ResetAssault(x, y, hpMin, hpMax);
						}
					}
				}
				else if (choice == 0)
				{
					m_enemies[i].ResetTank(x, y, hpMin, hpMax);
				}
				else
				{
					m_enemies[i].ResetAssault(x, y, hpMin, hpMax);
				}
			}
		}
	};

	if (spawnType == RoundSpawnType::Boss)
	{
		m_enemies[0].ResetBoss(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.28f, GameSession::GetBossHp());
		for (int i = 1; i < totalEnemies; ++i)
		{
			int c = (i - 1) % 6;
			int r = (i - 1) / 6;
			float ex = 190.0f + c * 180.0f;
			float ey = 120.0f + r * 70.0f;
			// Avoid overlaying too close to the boss at the center
			if (fabsf(ex - SCREEN_WIDTH * 0.5f) < 100.0f && ey < SCREEN_HEIGHT * 0.35f)
			{
				ey += 120.0f;
			}
			spawnMix(i, ex, ey);
		}
	}
	else if (spawnType == RoundSpawnType::MidBoss)
	{
		m_enemies[0].ResetMidBoss(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.30f, GameSession::GetMidBossHp());
		for (int i = 1; i < totalEnemies; ++i)
		{
			int c = (i - 1) % 6;
			int r = (i - 1) / 6;
			float ex = 190.0f + c * 180.0f;
			float ey = 120.0f + r * 70.0f;
			// Avoid overlaying too close to the midboss
			if (fabsf(ex - SCREEN_WIDTH * 0.5f) < 100.0f && ey < SCREEN_HEIGHT * 0.38f)
			{
				ey += 120.0f;
			}
			spawnMix(i, ex, ey);
		}
	}
	else
	{
		for (int i = 0; i < totalEnemies; ++i)
		{
			int c = i % 6;
			int r = i / 6;
			float ex = 190.0f + c * 180.0f;
			float ey = 120.0f + r * 70.0f;
			spawnMix(i, ex, ey);
		}
	}
}

void MainScene::UpdateEnemies()
{
	const float px = m_player.GetX();
	const float py = m_player.GetY();

	const bool pBHActive = m_skillActive && (m_currentSkillColor == ColorPreset::Black);
	const bool aBHActive = m_ally.IsActive() && m_ally.IsBlackholeActive();

	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		if (m_enemies[i].IsAlive())
		{
			bool bhActive = false;
			float bhX = 0.0f;
			float bhY = 0.0f;

			if (pBHActive && aBHActive)
			{
				float pDx = m_blackholeX - m_enemies[i].GetX();
				float pDy = m_blackholeY - m_enemies[i].GetY();
				float pDistSq = pDx * pDx + pDy * pDy;

				float aDx = m_ally.GetBlackholeX() - m_enemies[i].GetX();
				float aDy = m_ally.GetBlackholeY() - m_enemies[i].GetY();
				float aDistSq = aDx * aDx + aDy * aDy;

				bhActive = true;
				if (pDistSq < aDistSq)
				{
					bhX = m_blackholeX;
					bhY = m_blackholeY;
				}
				else
				{
					bhX = m_ally.GetBlackholeX();
					bhY = m_ally.GetBlackholeY();
				}
			}
			else if (pBHActive)
			{
				bhActive = true;
				bhX = m_blackholeX;
				bhY = m_blackholeY;
			}
			else if (aBHActive)
			{
				bhActive = true;
				bhX = m_ally.GetBlackholeX();
				bhY = m_ally.GetBlackholeY();
			}

			m_enemies[i].Update(px, py, m_enemyAttacks, MAX_ENEMY_ATTACKS, bhActive, bhX, bhY);
		}
	}
}

void MainScene::UpdateEnemyAttacks()
{
	for (auto& attack : m_enemyAttacks)
	{
		attack.Update();
	}
}

void MainScene::StartBattlePhase()
{
	m_phase = MainPhase::Battle;
	m_autoFireCooldown = 0;
}

void MainScene::AddChargedShots()
{
	const int perClick = GameSession::GetChargePerClick();
	for (int i = 0; i < perClick && m_shotBudget < GameSession::GetMaxChargeShots(); ++i)
	{
		++m_shotBudget;
	}
}

SceneType MainScene::UpdateChargePhase()
{
	m_player.Update(true);

	if (!m_roundStarted)
	{
		// Wait for click or enter/space to start
		if (InputManager::CheckDownMouse(MOUSE_INPUT_LEFT) == 1 ||
			InputManager::CheckDownMouse(MOUSE_INPUT_RIGHT) == 1 ||
			InputManager::CheckDownKey(KEY_INPUT_RETURN) == 1 ||
			InputManager::CheckDownKey(KEY_INPUT_SPACE) == 1)
		{
			m_roundStarted = true;
		}

		// Consume the inputs so they don't count towards charging
		int button = 0, clickX = 0, clickY = 0;
		while (GetMouseInputLog(&button, &clickX, &clickY) == 0) {}

		return SceneType::None;
	}

	int button = 0;
	int clickX = 0;
	int clickY = 0;
	while (GetMouseInputLog(&button, &clickX, &clickY) == 0)
	{
		const bool clicked = (button & MOUSE_INPUT_LEFT) != 0 ||
			(button & MOUSE_INPUT_RIGHT) != 0;
		if (clicked)
		{
			AddChargedShots();
		}
	}

	++m_chargeFrame;
	if (m_chargeFrame >= CHARGE_TIME_FRAMES)
	{
		StartBattlePhase();
	}

	return SceneType::None;
}

const Enemy* MainScene::FindNearestAliveEnemy(float fromX, float fromY) const
{
	const Enemy* nearest = nullptr;
	float bestDistSq = 0.0f;
	bool foundTank = false;

	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		const Enemy& enemy = m_enemies[i];
		if (!enemy.IsAlive())
		{
			continue;
		}

		const bool isTank = (enemy.GetType() == EnemyType::Tank);

		if (foundTank && !isTank)
		{
			continue;
		}

		const float dx = enemy.GetX() - fromX;
		const float dy = enemy.GetY() - fromY;
		const float distSq = dx * dx + dy * dy;

		if (!foundTank && isTank)
		{
			nearest = &enemy;
			bestDistSq = distSq;
			foundTank = true;
		}
		else if (nearest == nullptr || distSq < bestDistSq)
		{
			nearest = &enemy;
			bestDistSq = distSq;
		}
	}

	return nearest;
}

bool MainScene::SpawnPlayerShot(float targetX, float targetY)
{
	const bool homing = GameSession::HasHoming();
	const int shotCount = GameSession::GetMultiShotCount();
	const float px = m_player.GetX();
	const float py = m_player.GetY();
	int spawned = 0;

	for (int s = 0; s < shotCount; ++s)
	{
		const float spread = (shotCount > 1)
			? ((float)s - (float)(shotCount - 1) * 0.5f) * 28.0f
			: 0.0f;
		const float tx = targetX + spread;
		const float ty = targetY + spread * 0.4f;

		for (auto& shot : m_iceShots)
		{
			if (!shot.active)
			{
				shot.Spawn(px, py, tx, ty, homing);
				++spawned;
				break;
			}
		}
	}

	return spawned > 0;
}

void MainScene::TryAutoFire()
{
	const bool infiniteAmmo = m_skillActive && (m_currentSkillColor == ColorPreset::Green);

	if (!infiniteAmmo && m_shotsFired >= m_shotBudget)
	{
		return;
	}
	
	if (m_autoFireCooldown > 0)
	{
		return;
	}

	const Enemy* target = FindNearestAliveEnemy(m_player.GetX(), m_player.GetY());
	if (target == nullptr)
	{
		return;
	}

	if (SpawnPlayerShot(target->GetX(), target->GetY()))
	{
		if (!infiniteAmmo)
		{
			++m_shotsFired;
		}
		m_autoFireCooldown = GameSession::GetAutoFireInterval();
	}
}

void MainScene::UpdateIceProjectiles()
{
	const Enemy* target = FindNearestAliveEnemy(m_player.GetX(), m_player.GetY());
	const bool hasTarget = (target != nullptr);

	for (auto& shot : m_iceShots)
	{
		if (!shot.active)
		{
			continue;
		}

		float tx = shot.x + shot.vx;
		float ty = shot.y + shot.vy;
		if (hasTarget)
		{
			tx = target->GetX();
			ty = target->GetY();
		}
		shot.Update(tx, ty, hasTarget, GameSession::GetHomingTurnRate());
	}
}

void MainScene::CheckIceEnemyCollisions()
{
	const int damage = GameSession::GetIceDamage();

	for (auto& shot : m_iceShots)
	{
		if (!shot.active)
		{
			continue;
		}

		for (int i = 0; i < m_activeEnemyCount; ++i)
		{
			Enemy& enemy = m_enemies[i];
			if (!enemy.IsAlive())
			{
				continue;
			}

			if (CirclesOverlap(shot.x, shot.y, shot.radius,
				enemy.GetX(), enemy.GetY(), enemy.GetRadius()))
			{
				int finalDamage = damage;
				if (shot.isAllyShot)
				{
					finalDamage = (int)(damage * 1.35f);
					if (finalDamage < 1) finalDamage = 1;
				}

				enemy.TakeDamage(finalDamage);
				enemy.OnHitByIce(m_player.GetX(), m_player.GetY());

				// Eキーのアクティブスキル命中効果を適用
				bool isSkillActive = false;
				ColorPreset skillColor = ColorPreset::Blue;

				if (shot.isAllyShot)
				{
					isSkillActive = m_ally.IsActive() && m_ally.IsSkillActive();
					skillColor = m_ally.GetColorPreset();
				}
				else
				{
					isSkillActive = m_skillActive;
					skillColor = m_currentSkillColor;
				}

				if (isSkillActive)
				{
					if (skillColor == ColorPreset::Blue)
					{
						enemy.ApplySlow(3 * TARGET_FPS); // 3秒間スロウを付与
					}
					else if (skillColor == ColorPreset::Pink)
					{
						// 被弾した敵から200px以内にいる生存中の他の敵に同等のダメージを与える
						float ex = enemy.GetX();
						float ey = enemy.GetY();
						const float pinkRadius = 200.0f;
						for (int j = 0; j < m_activeEnemyCount; ++j)
						{
							Enemy& otherEnemy = m_enemies[j];
							if (otherEnemy.IsAlive() && &otherEnemy != &enemy)
							{
								float dx = otherEnemy.GetX() - ex;
								float dy = otherEnemy.GetY() - ey;
								float distSq = dx * dx + dy * dy;
								if (distSq <= pinkRadius * pinkRadius)
								{
									otherEnemy.TakeDamage(finalDamage);
									otherEnemy.OnHitByIce(m_player.GetX(), m_player.GetY());
								}
							}
						}
					}
					else if (skillColor == ColorPreset::Orange)
					{
						enemy.ApplyDot(3 * TARGET_FPS, finalDamage); // 3秒間継続ダメージ（DOT）を付与
					}
					else if (skillColor == ColorPreset::Yellow && !shot.isChain)
					{
						// チェイン：跳ね返り先となる生存している最も近い「他の」敵を検索
						const Enemy* nextTarget = nullptr;
						float nextBestDistSq = 0.0f;
						for (int j = 0; j < m_activeEnemyCount; ++j)
						{
							const Enemy& nextEnemy = m_enemies[j];
							if (!nextEnemy.IsAlive() || &nextEnemy == &enemy)
							{
								continue;
							}
							const float dx = nextEnemy.GetX() - shot.x;
							const float dy = nextEnemy.GetY() - shot.y;
							const float distSq = dx * dx + dy * dy;
							if (nextTarget == nullptr || distSq < nextBestDistSq)
							{
								nextTarget = &nextEnemy;
								nextBestDistSq = distSq;
							}
						}

						if (nextTarget != nullptr)
						{
							// チェインショットをスポーン
							for (auto& chainShot : m_iceShots)
							{
								if (!chainShot.active)
								{
									chainShot.Spawn(shot.x, shot.y, nextTarget->GetX(), nextTarget->GetY(), GameSession::HasHoming(), shot.isAllyShot);
									chainShot.isChain = true; // 無限に跳ね返るのを防ぐため、チェインショットとしてマーク
									break;
								}
							}
						}
					}
				}

				shot.active = false;
				break;
			}
		}
	}
}

void MainScene::ApplyAttackEffect(EnemyAttackEffect effect)
{
	if (effect == EnemyAttackEffect::Stun)
	{
		m_stunFrames = PLAYER_STUN_FRAMES;
	}
	else
	{
		m_shotBudget -= SHOT_DRAIN_ON_HIT;
		if (m_shotBudget < m_shotsFired)
		{
			m_shotBudget = m_shotsFired;
		}
		if (m_shotBudget < 0)
		{
			m_shotBudget = 0;
		}
	}
}

void MainScene::CheckEnemyAttackPlayer()
{
	for (auto& attack : m_enemyAttacks)
	{
		if (!attack.active)
		{
			continue;
		}

		if (CirclesOverlap(attack.x, attack.y, attack.radius,
			m_player.GetX(), m_player.GetY(), m_player.GetRadius()))
		{
			ApplyAttackEffect(attack.effect);
			attack.active = false;
		}
	}
}

bool MainScene::AnyActiveIce() const
{
	for (const auto& shot : m_iceShots)
	{
		if (shot.active && !shot.isAllyShot)
		{
			return true;
		}
	}
	return false;
}

int MainScene::CountAliveEnemies() const
{
	int count = 0;
	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		if (m_enemies[i].IsAlive())
		{
			++count;
		}
	}
	return count;
}

int MainScene::GetRemainingShots() const
{
	int remaining = m_shotBudget - m_shotsFired;
	return (remaining < 0) ? 0 : remaining;
}

bool MainScene::CirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2)
{
	const float dx = x2 - x1;
	const float dy = y2 - y1;
	const float sumR = r1 + r2;
	return (dx * dx + dy * dy) <= (sumR * sumR);
}

SceneType MainScene::UpdateBattlePhase()
{
	if (m_ally.IsActive())
	{
		m_ally.Update(m_player.GetX(), m_player.GetY(), m_enemies, m_activeEnemyCount, m_iceShots, GameSession::GetMaxChargeShots(), m_enemyAttacks, MAX_ENEMY_ATTACKS);
	}

	// Update active skill cooldown
	if (m_skillCooldownTimer > 0)
	{
		--m_skillCooldownTimer;
	}

	// Update active skill duration
	if (m_skillActive)
	{
		--m_skillActiveTimer;
		if (m_skillActiveTimer <= 0)
		{
			m_skillActive = false;
			// Cooldown starts only after duration ends
			int dur, cd;
			const char* sName;
			const char* sDesc;
			GetActiveSkillDetails(m_currentSkillColor, dur, cd, sName, sDesc);
			m_skillCooldownTimer = cd;
		}
	}

	// Update explosion drawing timer
	if (m_drawExplosion)
	{
		m_explosionDrawRadius += 340.0f / 30.0f; // Expand up to 350px in 30 frames
		--m_explosionDrawTimer;
		if (m_explosionDrawTimer <= 0)
		{
			m_drawExplosion = false;
		}
	}

	// Update assault explosions animation
	for (auto& exp : m_assaultExplosions)
	{
		if (exp.active)
		{
			exp.radius += 110.0f / 30.0f; // Expand up to 120px in 30 frames
			--exp.timer;
			if (exp.timer <= 0)
			{
				exp.active = false;
			}
		}
	}

	// Check for active skill trigger (E key)
	if (!m_skillActive && m_skillCooldownTimer <= 0 && m_stunFrames <= 0)
	{
		if (InputManager::CheckDownKey(KEY_INPUT_E) == 1)
		{
			m_currentSkillColor = PlayerSettings::GetSelectedPreset();
			int dur, cd;
			const char* sName;
			const char* sDesc;
			GetActiveSkillDetails(m_currentSkillColor, dur, cd, sName, sDesc);

			if (m_currentSkillColor == ColorPreset::Brown)
			{
				// Trigger explosion instantly
				m_explosionX = m_player.GetX();
				m_explosionY = m_player.GetY();
				m_drawExplosion = true;
				m_explosionDrawRadius = 10.0f;
				m_explosionDrawTimer = 30;

				const int expDamage = GameSession::GetIceDamage() * 5;
				const float expRadius = 350.0f;

				for (int i = 0; i < m_activeEnemyCount; ++i)
				{
					Enemy& enemy = m_enemies[i];
					if (!enemy.IsAlive())
					{
						continue;
					}

					float dx = enemy.GetX() - m_explosionX;
					float dy = enemy.GetY() - m_explosionY;
					float dist = sqrtf(dx * dx + dy * dy);
					if (dist <= expRadius + enemy.GetRadius())
					{
						enemy.TakeDamage(expDamage);
						enemy.OnHitByIce(m_explosionX, m_explosionY);
					}
				}

				m_skillActive = true;
				m_skillActiveTimer = 1; // Ends immediately on the next frame to start cooldown
			}
			else if (m_currentSkillColor == ColorPreset::Black)
			{
				// Singularity: Spawn black hole at player position
				m_blackholeX = m_player.GetX();
				m_blackholeY = m_player.GetY();
				m_blackholeTimer = dur;
				m_skillActive = true;
				m_skillActiveTimer = dur;
			}
			else
			{
				m_skillActive = true;
				m_skillActiveTimer = dur;
			}
		}
	}

	if (m_stunFrames > 0)
	{
		--m_stunFrames;
	}
	else if (m_autoFireCooldown > 0)
	{
		--m_autoFireCooldown;
	}

	// Speed multiplier for White color preset
	float speedMultiplier = 1.0f;
	if (m_skillActive && m_currentSkillColor == ColorPreset::White)
	{
		speedMultiplier = 1.4f;
	}

	// Update Medic targeting guidance before updating enemies
	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		Enemy& enemy = m_enemies[i];
		if (enemy.IsAlive() && enemy.GetType() == EnemyType::Medic)
		{
			Enemy* lowestAlly = nullptr;
			float lowestHpRatio = 1.0f;
			for (int j = 0; j < m_activeEnemyCount; ++j)
			{
				Enemy& other = m_enemies[j];
				if (other.IsAlive() && &other != &enemy)
				{
					float ratio = (float)other.GetHp() / other.GetMaxHp();
					if (ratio < lowestHpRatio)
					{
						lowestHpRatio = ratio;
						lowestAlly = &other;
					}
				}
			}

			if (lowestAlly != nullptr && lowestHpRatio < 1.0f)
			{
				enemy.GuideTowards(lowestAlly->GetX(), lowestAlly->GetY());
			}
		}
	}

	m_player.Update(m_stunFrames <= 0, speedMultiplier);
	UpdateEnemies();

	// Update Medic healing pulses
	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		Enemy& enemy = m_enemies[i];
		if (enemy.IsAlive() && enemy.GetType() == EnemyType::Medic)
		{
			enemy.m_healTimer++;
			if (enemy.m_healTimer >= 60)
			{
				enemy.m_healTimer = 0;
				float mx = enemy.GetX();
				float my = enemy.GetY();
				for (int j = 0; j < m_activeEnemyCount; ++j)
				{
					Enemy& other = m_enemies[j];
					if (other.IsAlive() && &other != &enemy)
					{
						float dx = other.GetX() - mx;
						float dy = other.GetY() - my;
						float dist = sqrtf(dx*dx + dy*dy);
						if (dist <= 160.0f)
						{
							int amount = (int)(other.GetMaxHp() * 0.10f);
							if (amount < 1) amount = 1;
							other.Heal(amount);
						}
					}
				}
			}
		}
	}

	UpdateEnemyAttacks();
	TryAutoFire();
	UpdateIceProjectiles();
	CheckIceEnemyCollisions();

	// Check for Assault enemy deaths
	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		Enemy& enemy = m_enemies[i];
		if (enemy.GetType() == EnemyType::Assault && enemy.GetJustDied())
		{
			enemy.ClearJustDied();
			float ex = enemy.GetX();
			float ey = enemy.GetY();

			// Spawn visual explosion
			for (auto& exp : m_assaultExplosions)
			{
				if (!exp.active)
				{
					exp.x = ex;
					exp.y = ey;
					exp.radius = 10.0f;
					exp.timer = 30;
					exp.active = true;
					break;
				}
			}

			// Player ammo drain check
			float px = m_player.GetX();
			float py = m_player.GetY();
			float dist = sqrtf((px - ex)*(px - ex) + (py - ey)*(py - ey));
			if (dist <= 120.0f + m_player.GetRadius())
			{
				int remaining = m_shotBudget - m_shotsFired;
				int drain = (int)(remaining * 0.04f);
				if (drain < 1 && remaining > 0)
				{
					drain = 1;
				}
				m_shotBudget -= drain;
				if (m_shotBudget < m_shotsFired)
				{
					m_shotBudget = m_shotsFired;
				}
			}
		}
	}

	CheckEnemyAttackPlayer();

	if (CountAliveEnemies() == 0)
	{
		const bool bossDefeated = (GameSession::GetRoundSpawnType(
			GameSession::GetRound()) == RoundSpawnType::Boss);
		GameSession::OnRoundCleared(GetRemainingShots(), bossDefeated);
		return SceneType::SkillPick;
	}

	const bool allShotsSpent = (m_shotsFired >= m_shotBudget);
	const bool noIceOnScreen = !AnyActiveIce();

	if (allShotsSpent && noIceOnScreen)
	{
		GameSession::SetGameOver();
		return SceneType::Result;
	}

	return SceneType::None;
}

SceneType MainScene::Update()
{
	// Toggle Pause with Escape key
	if (InputManager::CheckDownKey(KEY_INPUT_ESCAPE) == 1)
	{
		if (m_isPaused)
		{
			if (m_pauseState == PauseState::Settings)
			{
				m_pauseState = PauseState::Main;
			}
			else
			{
				m_isPaused = false;
				m_pauseState = PauseState::None;
			}
		}
		else
		{
			m_isPaused = true;
			m_pauseState = PauseState::Main;
			m_pauseMenuIndex = 0;
			m_pauseHoveredMenuIndex = -1;
		}
	}

	if (m_isPaused)
	{
		return UpdatePauseMenu();
	}

	if (m_phase == MainPhase::Charge)
	{
		return UpdateChargePhase();
	}
	return UpdateBattlePhase();
}

void MainScene::Draw()
{
	SetBackgroundColor(40, 50, 70);

	// 1. キャラクターの下にブラックホールの重力特異点を描画
	if (m_skillActive && m_currentSkillColor == ColorPreset::Black)
	{
		const float rad = 80.0f + sinf((float)m_skillActiveTimer * 0.15f) * 10.0f;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		DrawCircle((int)m_blackholeX, (int)m_blackholeY, (int)rad, GetColor(20, 10, 35), TRUE);
		DrawCircle((int)m_blackholeX, (int)m_blackholeY, (int)rad, GetColor(130, 60, 210), FALSE);
		DrawCircle((int)m_blackholeX, (int)m_blackholeY, (int)(rad * 0.65f), GetColor(10, 5, 20), TRUE);
		DrawCircle((int)m_blackholeX, (int)m_blackholeY, (int)(rad * 0.3f), GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (m_ally.IsActive() && m_ally.IsBlackholeActive())
	{
		const float rad = 80.0f + sinf((float)m_ally.GetSkillActiveTimer() * 0.15f) * 10.0f;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		DrawCircle((int)m_ally.GetBlackholeX(), (int)m_ally.GetBlackholeY(), (int)rad, GetColor(20, 10, 35), TRUE);
		DrawCircle((int)m_ally.GetBlackholeX(), (int)m_ally.GetBlackholeY(), (int)rad, GetColor(130, 60, 210), FALSE);
		DrawCircle((int)m_ally.GetBlackholeX(), (int)m_ally.GetBlackholeY(), (int)(rad * 0.65f), GetColor(10, 5, 20), TRUE);
		DrawCircle((int)m_ally.GetBlackholeX(), (int)m_ally.GetBlackholeY(), (int)(rad * 0.3f), GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	m_player.Draw();

	if (m_ally.IsActive())
	{
		m_ally.Draw();
	}

	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		m_enemies[i].Draw();
	}

	if (m_phase == MainPhase::Battle)
	{
		for (auto& attack : m_enemyAttacks)
		{
			attack.Draw();
		}
		for (auto& shot : m_iceShots)
		{
			shot.Draw();
		}
	}

	// 2. 自己爆発のアクティブ時にブラストリングを描画
	if (m_drawExplosion)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 140);
		DrawCircle((int)m_explosionX, (int)m_explosionY, (int)m_explosionDrawRadius, GetColor(255, 110, 30), TRUE);
		DrawCircle((int)m_explosionX, (int)m_explosionY, (int)m_explosionDrawRadius, GetColor(255, 220, 90), FALSE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (m_ally.IsActive() && m_ally.IsExplosionActive())
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 140);
		DrawCircle((int)m_ally.GetExplosionX(), (int)m_ally.GetExplosionY(), (int)m_ally.GetExplosionRadius(), GetColor(255, 110, 30), TRUE);
		DrawCircle((int)m_ally.GetExplosionX(), (int)m_ally.GetExplosionY(), (int)m_ally.GetExplosionRadius(), GetColor(255, 220, 90), FALSE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// Draw assault death explosion visual effects
	for (const auto& exp : m_assaultExplosions)
	{
		if (exp.active)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 130 * exp.timer / 30);
			DrawCircle((int)exp.x, (int)exp.y, (int)exp.radius, GetColor(255, 90, 40), TRUE);
			DrawCircle((int)exp.x, (int)exp.y, (int)exp.radius, GetColor(255, 200, 100), FALSE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	const int textColor = GetColor(255, 255, 255);
	const RoundSpawnType spawnType = GameSession::GetRoundSpawnType(GameSession::GetRound());

	DrawFormatString(SCREEN_WIDTH - 300, 20, GetColor(255, 220, 120),
		"Round %d  Score: %d", GameSession::GetRound(), GameSession::GetScore());

	if (spawnType == RoundSpawnType::Boss)
	{
		DrawFormatString(SCREEN_WIDTH - 300, 45, GetColor(255, 100, 100), "!!! BOSS ROUND !!!");
	}
	else if (spawnType == RoundSpawnType::MidBoss)
	{
		DrawFormatString(SCREEN_WIDTH - 300, 45, GetColor(200, 140, 255), "!! MID-BOSS ROUND !!");
	}
	else
	{
		DrawFormatString(SCREEN_WIDTH - 300, 45, GetColor(180, 200, 220),
			"Enemy HP: %d-%d", GameSession::GetEnemyHpMin(), GameSession::GetEnemyHpMax());
	}

	if (GameSession::GetSpecialSkillLevel(SpecialSkillType::MultiShot) > 0)
	{
		DrawFormatString(SCREEN_WIDTH - 300, 70, GetColor(120, 255, 200),
			"Multi Shot Lv.%d (%d balls)", GameSession::GetSpecialSkillLevel(SpecialSkillType::MultiShot),
			GameSession::GetMultiShotCount());
	}
	if (GameSession::GetSpecialSkillLevel(SpecialSkillType::Homing) > 0)
	{
		DrawFormatString(SCREEN_WIDTH - 300, 90, GetColor(120, 255, 200),
			"Homing Lv.%d", GameSession::GetSpecialSkillLevel(SpecialSkillType::Homing));
	}

	// 3. バトルフェーズ中にアクティブスキルのHUDを描画
	if (m_phase == MainPhase::Battle)
	{
		const ColorPreset currentPreset = PlayerSettings::GetSelectedPreset();
		int dur, cd;
		const char* sName;
		const char* sDesc;
		GetActiveSkillDetails(currentPreset, dur, cd, sName, sDesc);

		DrawFormatString(SCREEN_WIDTH - 300, 130, GetColor(200, 220, 240), "------------------------");
		DrawFormatString(SCREEN_WIDTH - 300, 150, GetColor(180, 200, 230), "Skill [E]: %s", sName);

		// スキルの進行状況バー
		int barX = SCREEN_WIDTH - 300;
		int barY = 175;
		int barW = 240;
		int barH = 20;

		DrawBox(barX, barY, barX + barW, barY + barH, GetColor(40, 50, 65), TRUE);
		DrawBox(barX, barY, barX + barW, barY + barH, GetColor(80, 100, 125), FALSE);

		if (m_skillActive)
		{
			float progress = (float)m_skillActiveTimer / (float)dur;
			if (progress < 0.0f) progress = 0.0f;
			if (progress > 1.0f) progress = 1.0f;

			int fillW = (int)(progress * barW);
			int activeColor = PlayerSettings::GetPresetBodyColor(currentPreset);
			DrawBox(barX + 2, barY + 2, barX + 2 + fillW - 4, barY + barH - 2, activeColor, TRUE);

			const float sec = (float)m_skillActiveTimer / (float)TARGET_FPS;
			DrawFormatString(barX + 10, barY + 3, GetColor(255, 255, 255), "ACTIVE (%.1fs left)", sec);
		}
		else if (m_skillCooldownTimer > 0)
		{
			float progress = 1.0f - (float)m_skillCooldownTimer / (float)cd;
			if (progress < 0.0f) progress = 0.0f;
			if (progress > 1.0f) progress = 1.0f;

			int fillW = (int)(progress * barW);
			DrawBox(barX + 2, barY + 2, barX + 2 + fillW - 4, barY + barH - 2, GetColor(110, 110, 110), TRUE);

			const float sec = (float)m_skillCooldownTimer / (float)TARGET_FPS;
			DrawFormatString(barX + 10, barY + 3, GetColor(200, 200, 200), "COOLDOWN (%.1fs left)", sec);
		}
		else
		{
			static int glowFrame = 0;
			++glowFrame;
			const float glow = sinf((float)glowFrame * 0.1f) * 0.5f + 0.5f;
			const int readyColor = GetColor((int)(100 + glow * 50), (int)(180 + glow * 40), (int)(220 + glow * 35));
			DrawBox(barX + 2, barY + 2, barX + barW - 2, barY + barH - 2, readyColor, TRUE);

			DrawFormatString(barX + 10, barY + 3, GetColor(255, 255, 255), "READY (Press E!)");
		}

		DrawFormatString(SCREEN_WIDTH - 300, 205, GetColor(160, 170, 180), "%s", sDesc);

		// 3.5. 味方NPCのスキルHUDを描画
		if (m_ally.IsActive())
		{
			const ColorPreset allyPreset = m_ally.GetColorPreset();
			int aDur, aCd;
			const char* aName;
			const char* aDesc;
			GetActiveSkillDetails(allyPreset, aDur, aCd, aName, aDesc);

			DrawFormatString(SCREEN_WIDTH - 300, 230, GetColor(200, 220, 240), "------------------------");
			DrawFormatString(SCREEN_WIDTH - 300, 250, GetColor(180, 230, 200), "Ally Skill: %s", aName);

			int aBarX = SCREEN_WIDTH - 300;
			int aBarY = 275;
			int aBarW = 240;
			int aBarH = 20;

			DrawBox(aBarX, aBarY, aBarX + aBarW, aBarY + aBarH, GetColor(40, 50, 65), TRUE);
			DrawBox(aBarX, aBarY, aBarX + aBarW, aBarY + aBarH, GetColor(80, 100, 125), FALSE);

			if (m_ally.IsSkillActive())
			{
				float progress = (float)m_ally.GetSkillActiveTimer() / (float)aDur;
				if (progress < 0.0f) progress = 0.0f;
				if (progress > 1.0f) progress = 1.0f;

				int fillW = (int)(progress * aBarW);
				int activeColor = PlayerSettings::GetPresetBodyColor(allyPreset);
				DrawBox(aBarX + 2, aBarY + 2, aBarX + 2 + fillW - 4, aBarY + aBarH - 2, activeColor, TRUE);

				const float sec = (float)m_ally.GetSkillActiveTimer() / (float)TARGET_FPS;
				DrawFormatString(aBarX + 10, aBarY + 3, GetColor(255, 255, 255), "ACTIVE (%.1fs left)", sec);
			}
			else if (m_ally.GetSkillCooldownTimer() > 0)
			{
				float progress = 1.0f - (float)m_ally.GetSkillCooldownTimer() / (float)aCd;
				if (progress < 0.0f) progress = 0.0f;
				if (progress > 1.0f) progress = 1.0f;

				int fillW = (int)(progress * aBarW);
				DrawBox(aBarX + 2, aBarY + 2, aBarX + 2 + fillW - 4, aBarY + aBarH - 2, GetColor(110, 110, 110), TRUE);

				const float sec = (float)m_ally.GetSkillCooldownTimer() / (float)TARGET_FPS;
				DrawFormatString(aBarX + 10, aBarY + 3, GetColor(200, 200, 200), "COOLDOWN (%.1fs left)", sec);
			}
			else
			{
				static int aGlowFrame = 0;
				++aGlowFrame;
				const float glow = sinf((float)aGlowFrame * 0.1f) * 0.5f + 0.5f;
				const int readyColor = GetColor((int)(100 + glow * 50), (int)(180 + glow * 40), (int)(220 + glow * 35));
				DrawBox(aBarX + 2, aBarY + 2, aBarX + aBarW - 2, aBarY + aBarH - 2, readyColor, TRUE);

				DrawFormatString(aBarX + 10, aBarY + 3, GetColor(255, 255, 255), "READY (Auto Cast)");
			}

			DrawFormatString(SCREEN_WIDTH - 300, 305, GetColor(160, 170, 180), "%s", aDesc);
		}
	}

	if (m_stunFrames > 0)
	{
		DrawFormatString(40, 120, GetColor(255, 200, 80),
			"STUNNED! (%d)", (m_stunFrames + TARGET_FPS - 1) / TARGET_FPS);
	}

	if (m_phase == MainPhase::Charge)
	{
		const int framesLeft = CHARGE_TIME_FRAMES - m_chargeFrame;
		const int secondsLeft = (framesLeft + TARGET_FPS - 1) / TARGET_FPS;

		DrawFormatString(40, 40, textColor, "Charge time! Click to store ice balls");
		DrawFormatString(40, 80, GetColor(200, 240, 255),
			"Charged: %d / %d  (+%d per click)",
			m_shotBudget, GameSession::GetMaxChargeShots(), GameSession::GetChargePerClick());

		// Draw Circular Timer Gauge at the Top-Center
		int cx = SCREEN_WIDTH / 2;
		int cy = 60;
		int r = 26;
		float ratio = (float)framesLeft / CHARGE_TIME_FRAMES;
		if (ratio < 0.0f) ratio = 0.0f;
		if (ratio > 1.0f) ratio = 1.0f;

		// Background ring
		DrawCircle(cx, cy, r + 4, GetColor(30, 40, 50), TRUE);
		DrawCircle(cx, cy, r + 4, GetColor(80, 100, 120), FALSE);

		// Sector segment
		int segments = 60;
		float angleLimit = ratio * 2.0f * 3.14159265f;
		int fillCol = (secondsLeft <= 3) ? GetColor(255, 100, 100) : GetColor(100, 200, 255);

		for (int i = 0; i < segments; ++i)
		{
			float angle1 = (float)i / segments * 2.0f * 3.14159265f;
			float angle2 = (float)(i + 1) / segments * 2.0f * 3.14159265f;
			if (angle1 < angleLimit)
			{
				if (angle2 > angleLimit) angle2 = angleLimit;
				int x1 = cx + (int)(cosf(angle1 - 3.14159265f / 2.0f) * r);
				int y1 = cy + (int)(sinf(angle1 - 3.14159265f / 2.0f) * r);
				int x2 = cx + (int)(cosf(angle2 - 3.14159265f / 2.0f) * r);
				int y2 = cy + (int)(sinf(angle2 - 3.14159265f / 2.0f) * r);
				DrawTriangle(cx, cy, x1, y1, x2, y2, fillCol, TRUE);
			}
		}
		// Small inner core for premium donut look
		DrawCircle(cx, cy, r - 8, GetColor(30, 40, 50), TRUE);
		DrawCircle(cx, cy, r - 8, GetColor(80, 100, 120), FALSE);
		
		// Draw remaining seconds in the center of the donut!
		DrawFormatString(cx - 6, cy - 8, GetColor(255, 255, 255), "%d", secondsLeft);
	}
	else
	{
		DrawFormatString(40, 40, textColor, "WASD: Move  |  Auto ice attack");
		DrawFormatString(40, 80, GetColor(200, 230, 255),
			"Shots left: %d / %d  |  Enemies: %d",
			GetRemainingShots(), m_shotBudget, CountAliveEnemies());
		DrawFormatString(40, 110, GetColor(255, 180, 200),
			"Pink shot = drain balls  |  Yellow = stun 1 sec");
	}

	// 4. Click to start prompt overlay
	if (m_phase == MainPhase::Charge && !m_roundStarted)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(10, 15, 25), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		static int pulseFrame = 0;
		++pulseFrame;
		const float pulse = sinf((float)pulseFrame * 0.08f) * 0.5f + 0.5f;
		const int pulseColor = GetColor((int)(200 + pulse * 55), (int)(180 + pulse * 75), (int)(50 + pulse * 205));

		DrawFormatString(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 40, pulseColor, "CLICK TO START ROUND!");
		DrawFormatString(SCREEN_WIDTH / 2 - 130, SCREEN_HEIGHT / 2 + 10, GetColor(180, 200, 220), "Round %d starting...", GameSession::GetRound());
		DrawFormatString(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 + 55, GetColor(140, 160, 180), "Click or Press Enter/Space to start charging ice shots!");
	}

	// 5. Draw pause menu overlay if paused
	if (m_isPaused)
	{
		DrawPauseMenu();
	}
}

void MainScene::GetActiveSkillDetails(ColorPreset preset, int& outDuration, int& outCooldown, const char*& outName, const char*& outDesc) const
{
	switch (preset)
	{
	case ColorPreset::Blue:
		outDuration = 1 * TARGET_FPS;
		outCooldown = 5 * TARGET_FPS;
		outName = "Frost Touch";
		outDesc = "Hits slow enemies (3s)";
		break;
	case ColorPreset::Green:
		outDuration = 2 * TARGET_FPS;
		outCooldown = 10 * TARGET_FPS;
		outName = "Bullet Save";
		outDesc = "Infinite ammo (2s)";
		break;
	case ColorPreset::Yellow:
		outDuration = 2 * TARGET_FPS;
		outCooldown = 10 * TARGET_FPS;
		outName = "Chain Shot";
		outDesc = "Hits chain to next enemy";
		break;
	case ColorPreset::Pink:
		outDuration = 1 * TARGET_FPS;
		outCooldown = 13 * TARGET_FPS;
		outName = "Shared Pain";
		outDesc = "Hits damage nearby enemies";
		break;
	case ColorPreset::Orange:
		outDuration = 2 * TARGET_FPS;
		outCooldown = 8 * TARGET_FPS;
		outName = "Acid Burn";
		outDesc = "Hits apply DOT (3s)";
		break;
	case ColorPreset::Black:
		outDuration = 3 * TARGET_FPS;
		outCooldown = 20 * TARGET_FPS;
		outName = "Black Hole";
		outDesc = "Pull enemies to center";
		break;
	case ColorPreset::White:
		outDuration = 10 * TARGET_FPS;
		outCooldown = 15 * TARGET_FPS;
		outName = "Overdrive";
		outDesc = "Speed x1.4 (10s)";
		break;
	case ColorPreset::Brown:
		outDuration = 1;
		outCooldown = 12 * TARGET_FPS;
		outName = "Self-Explosion";
		outDesc = "Explode dealing x5 damage";
		break;
	default:
		outDuration = 0;
		outCooldown = 0;
		outName = "None";
		outDesc = "";
		break;
	}
}

SceneType MainScene::UpdatePauseMenu()
{
	if (m_pauseState == PauseState::Settings)
	{
		if (InputManager::CheckDownKey(KEY_INPUT_RETURN) == 1 ||
			InputManager::CheckDownKey(KEY_INPUT_ESCAPE) == 1)
		{
			m_pauseState = PauseState::Main;
		}
		return SceneType::None;
	}

	const int PAUSE_MENU_COUNT = 3;

	int mouseX = 0;
	int mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);

	const int boxW = 380;
	const int boxH = 280;
	const int boxY = SCREEN_HEIGHT / 2 - boxH / 2;

	const int itemW = 280;
	const int itemH = 36;
	const int itemGap = 50;
	const int itemStartX = SCREEN_WIDTH / 2 - itemW / 2;
	const int itemStartY = boxY + 80;

	m_pauseHoveredMenuIndex = -1;
	for (int i = 0; i < PAUSE_MENU_COUNT; ++i)
	{
		const int x = itemStartX;
		const int y = itemStartY + i * itemGap;
		if (mouseX >= x && mouseX <= x + itemW && mouseY >= y && mouseY <= y + itemH)
		{
			m_pauseHoveredMenuIndex = i;
			m_pauseMenuIndex = i;
		}
	}

	if (InputManager::CheckDownKey(KEY_INPUT_W) == 1)
	{
		m_pauseMenuIndex = (m_pauseMenuIndex + PAUSE_MENU_COUNT - 1) % PAUSE_MENU_COUNT;
	}
	if (InputManager::CheckDownKey(KEY_INPUT_S) == 1)
	{
		m_pauseMenuIndex = (m_pauseMenuIndex + 1) % PAUSE_MENU_COUNT;
	}

	auto activatePauseMenu = [&](int index) -> SceneType
	{
		switch (index)
		{
		case 0: // Resume
			m_isPaused = false;
			m_pauseState = PauseState::None;
			break;
		case 1: // Settings
			m_pauseState = PauseState::Settings;
			break;
		case 2: // Return to Title
			m_isPaused = false;
			m_pauseState = PauseState::None;
			return SceneType::Title;
		}
		return SceneType::None;
	};

	if (InputManager::CheckDownMouse(MOUSE_INPUT_LEFT) == 1 && m_pauseHoveredMenuIndex >= 0)
	{
		return activatePauseMenu(m_pauseHoveredMenuIndex);
	}
	if (InputManager::CheckDownKey(KEY_INPUT_RETURN) == 1)
	{
		return activatePauseMenu(m_pauseMenuIndex);
	}

	return SceneType::None;
}

void MainScene::DrawPauseMenu() const
{
	// Translucent backdrop
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(15, 20, 30), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int boxW = 380;
	const int boxH = 280;
	const int boxX = SCREEN_WIDTH / 2 - boxW / 2;
	const int boxY = SCREEN_HEIGHT / 2 - boxH / 2;

	// Pause frame
	DrawBox(boxX, boxY, boxX + boxW, boxY + boxH, GetColor(35, 45, 60), TRUE);
	DrawBox(boxX, boxY, boxX + boxW, boxY + boxH, GetColor(100, 140, 220), FALSE);

	if (m_pauseState == PauseState::Settings)
	{
		// Settings view
		DrawFormatString(SCREEN_WIDTH / 2 - 60, boxY + 25, GetColor(255, 220, 120), "SETTINGS");
		DrawFormatString(SCREEN_WIDTH / 2 - 130, boxY + 90, GetColor(220, 230, 255), "Current Color: %s", PlayerSettings::GetPresetName(PlayerSettings::GetSelectedPreset()));
		DrawFormatString(SCREEN_WIDTH / 2 - 130, boxY + 130, GetColor(180, 190, 210), "Press Enter or ESC to return");
		PlayerSettings::DrawPreview(SCREEN_WIDTH / 2, boxY + 190, 20);
	}
	else
	{
		// Pause menu view
		DrawFormatString(SCREEN_WIDTH / 2 - 40, boxY + 25, GetColor(255, 220, 120), "PAUSED");

		const int PAUSE_MENU_COUNT = 3;
		const char* menuItems[] = {
			"Resume",
			"Settings",
			"Return to Title"
		};

		const int itemW = 280;
		const int itemH = 36;
		const int itemGap = 50;
		const int itemStartX = SCREEN_WIDTH / 2 - itemW / 2;
		const int itemStartY = boxY + 80;

		for (int i = 0; i < PAUSE_MENU_COUNT; ++i)
		{
			const bool selected = (i == m_pauseMenuIndex);
			const bool hovered = (i == m_pauseHoveredMenuIndex);

			int textColor;
			if (selected && hovered) textColor = GetColor(255, 200, 40);
			else if (hovered) textColor = GetColor(255, 160, 60);
			else if (selected) textColor = GetColor(100, 180, 255);
			else textColor = GetColor(180, 190, 200);

			const int x = itemStartX;
			const int y = itemStartY + i * itemGap;

			if (hovered || selected)
			{
				DrawBox(x - 8, y - 4, x + itemW + 8, y + itemH + 4, GetColor(45, 60, 85), TRUE);
				DrawBox(x - 8, y - 4, x + itemW + 8, y + itemH + 4, GetColor(80, 120, 200), FALSE);
			}

			DrawFormatString(x + 20, y + 8, textColor, "%s%s", selected ? "> " : "  ", menuItems[i]);
		}
	}
}
