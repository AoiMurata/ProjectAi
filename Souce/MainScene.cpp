// =============================================================================
// MainScene.cpp
// =============================================================================
#include "MainScene.h"
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

	m_activeEnemyCount = 0;

	auto addEnemy = [&](float x, float y)
	{
		if (m_activeEnemyCount < MAX_ENEMIES)
		{
			++m_activeEnemyCount;
		}
	};

	if (spawnType == RoundSpawnType::Boss)
	{
		m_enemies[0].ResetBoss(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.28f,
			GameSession::GetBossHp());
		addEnemy(0, 0);

		const float spots[][2] = {
			{ SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.55f },
			{ SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.55f },
			{ SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.75f },
			{ SCREEN_WIDTH * 0.65f, SCREEN_HEIGHT * 0.75f },
		};
		for (int i = 0; i < 4; ++i)
		{
			m_enemies[i + 1].ResetNormal(spots[i][0], spots[i][1], hpMin, hpMax);
			addEnemy(spots[i][0], spots[i][1]);
		}
		m_activeEnemyCount = 5;
	}
	else if (spawnType == RoundSpawnType::MidBoss)
	{
		m_enemies[0].ResetMidBoss(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.30f,
			GameSession::GetMidBossHp());
		addEnemy(0, 0);

		const float spots[][2] = {
			{ SCREEN_WIDTH * 0.22f, SCREEN_HEIGHT * 0.55f },
			{ SCREEN_WIDTH * 0.78f, SCREEN_HEIGHT * 0.55f },
			{ SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.72f },
			{ SCREEN_WIDTH * 0.65f, SCREEN_HEIGHT * 0.72f },
			{ SCREEN_WIDTH * 0.50f, SCREEN_HEIGHT * 0.50f },
		};
		for (int i = 0; i < 5; ++i)
		{
			m_enemies[i + 1].ResetNormal(spots[i][0], spots[i][1], hpMin, hpMax);
		}
		m_activeEnemyCount = 6;
	}
	else
	{
		const float spots[][2] = {
			{ SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.25f },
			{ SCREEN_WIDTH * 0.50f, SCREEN_HEIGHT * 0.20f },
			{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f },
			{ SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.40f },
			{ SCREEN_WIDTH * 0.65f, SCREEN_HEIGHT * 0.40f },
			{ SCREEN_WIDTH * 0.50f, SCREEN_HEIGHT * 0.50f },
		};
		for (int i = 0; i < MAX_ENEMIES; ++i)
		{
			m_enemies[i].ResetNormal(spots[i][0], spots[i][1], hpMin, hpMax);
		}
		m_activeEnemyCount = MAX_ENEMIES;
	}
}

void MainScene::UpdateEnemies()
{
	const float px = m_player.GetX();
	const float py = m_player.GetY();

	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		if (m_enemies[i].IsAlive())
		{
			m_enemies[i].Update(px, py, m_enemyAttacks, MAX_ENEMY_ATTACKS);
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
	for (int i = 0; i < perClick && m_shotBudget < MAX_CHARGE_SHOTS; ++i)
	{
		++m_shotBudget;
	}
}

SceneType MainScene::UpdateChargePhase()
{
	m_player.Update(true);

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

	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		const Enemy& enemy = m_enemies[i];
		if (!enemy.IsAlive())
		{
			continue;
		}

		const float dx = enemy.GetX() - fromX;
		const float dy = enemy.GetY() - fromY;
		const float distSq = dx * dx + dy * dy;

		if (nearest == nullptr || distSq < bestDistSq)
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
	if (m_shotsFired >= m_shotBudget || m_autoFireCooldown > 0)
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
		++m_shotsFired;
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
				enemy.TakeDamage(damage);
				enemy.OnHitByIce(m_player.GetX(), m_player.GetY());
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
		if (shot.active)
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
	if (m_stunFrames > 0)
	{
		--m_stunFrames;
	}
	else if (m_autoFireCooldown > 0)
	{
		--m_autoFireCooldown;
	}

	m_player.Update(m_stunFrames <= 0);
	UpdateEnemies();
	UpdateEnemyAttacks();
	TryAutoFire();
	UpdateIceProjectiles();
	CheckIceEnemyCollisions();
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
	if (m_phase == MainPhase::Charge)
	{
		return UpdateChargePhase();
	}
	return UpdateBattlePhase();
}

void MainScene::Draw()
{
	SetBackgroundColor(40, 50, 70);

	m_player.Draw();

	for (int i = 0; i < m_activeEnemyCount; ++i)
	{
		m_enemies[i].Draw();
	}

	if (m_phase == MainPhase::Battle)
	{
		for (const auto& attack : m_enemyAttacks)
		{
			attack.Draw();
		}
		for (const auto& shot : m_iceShots)
		{
			shot.Draw();
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

	if (m_stunFrames > 0)
	{
		DrawFormatString(40, 120, GetColor(255, 200, 80),
			"STUNNED! (%d)", (m_stunFrames + TARGET_FPS - 1) / TARGET_FPS);
	}

	if (m_phase == MainPhase::Charge)
	{
		const int framesLeft = CHARGE_TIME_FRAMES - m_chargeFrame;
		const int secondsLeft = (framesLeft + TARGET_FPS - 1) / TARGET_FPS;
		const int timerColor = (secondsLeft <= 3)
			? GetColor(255, 120, 120) : GetColor(255, 220, 120);

		DrawFormatString(40, 40, textColor, "Charge time! Click to store ice balls");
		DrawFormatString(40, 80, timerColor, "Time left: %d sec", secondsLeft);
		DrawFormatString(40, 120, GetColor(200, 240, 255),
			"Charged: %d / %d  (+%d per click)",
			m_shotBudget, MAX_CHARGE_SHOTS, GameSession::GetChargePerClick());
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
}
