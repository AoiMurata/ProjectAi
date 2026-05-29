// =============================================================================
// Ally.cpp
// =============================================================================
#include "Ally.h"
#include "GameSession.h"
#include "DxLib.h"
#include <cmath>

void Ally::Reset(float x, float y)
{
	m_x = x;
	m_y = y;
	m_vx = 0.0f;
	m_vy = 0.0f;
	m_radius = PLAYER_RADIUS;
	m_active = true;

	m_destX = x;
	m_destY = y;
	m_destTimer = 0;

	// 8つの選択肢から完全にランダムなカラープリセットを選択
	m_color = static_cast<ColorPreset>(GetRand(static_cast<int>(ColorPreset::Count) - 1));

	m_autoFireCooldown = 0;
	m_skillActive = false;
	m_skillActiveTimer = 0;
	m_skillCooldownTimer = 0;
	m_blackholeX = 0.0f;
	m_blackholeY = 0.0f;
	m_explosionX = 0.0f;
	m_explosionY = 0.0f;
	m_explosionDrawRadius = 0.0f;
	m_explosionDrawTimer = 0;
	m_drawExplosion = false;
}

void Ally::Update(float px, float py, const Enemy* enemies, int activeEnemyCount, IceProjectile* iceShots, int maxShots, const EnemyAttack* attacks, int maxAttacks)
{
	if (!m_active)
	{
		return;
	}

	// 1. 移動AI：気ままなパトロールと弾幕回避の操舵AI
	if (m_destTimer > 0)
	{
		--m_destTimer;
	}

	float dxToDest = m_destX - m_x;
	float dyToDest = m_destY - m_y;
	float distToDest = sqrtf(dxToDest * dxToDest + dyToDest * dyToDest);

	// 定期的に、または目標に到達したときに、最適な戦闘空間内の新しいランダムな場所を選択
	if (m_destTimer <= 0 || distToDest < 30.0f)
	{
		m_destX = SCREEN_WIDTH * 0.15f + GetRand((int)(SCREEN_WIDTH * 0.7f));
		m_destY = SCREEN_HEIGHT * 0.25f + GetRand((int)(SCREEN_HEIGHT * 0.55f));
		m_destTimer = 90 + GetRand(120); // 1.5秒から3.5秒
	}

	float moveX = 0.0f;
	float moveY = 0.0f;
	if (distToDest > 5.0f)
	{
		moveX = dxToDest / distToDest;
		moveY = dyToDest / distToDest;
	}

	// 弾幕回避ベクトル：近く（100px以内）に危険な敵の弾がある場合は離れるように操舵
	for (int i = 0; i < maxAttacks; ++i)
	{
		const auto& attack = attacks[i];
		if (attack.active)
		{
			float bDx = m_x - attack.x;
			float bDy = m_y - attack.y;
			float bDist = sqrtf(bDx * bDx + bDy * bDy);
			if (bDist < 100.0f && bDist > 0.001f)
			{
				// 回避の操舵力
				moveX += (bDx / bDist) * 2.0f;
				moveY += (bDy / bDist) * 2.0f;
			}
		}
	}

	// プレイヤーとの適切な距離を保つ（直接重なり合わないようにする、例：60px以内）
	float pDx = m_x - px;
	float pDy = m_y - py;
	float pDist = sqrtf(pDx * pDx + pDy * pDy);
	if (pDist < 60.0f && pDist > 0.001f)
	{
		moveX += (pDx / pDist) * 1.5f;
		moveY += (pDy / pDist) * 1.5f;
	}

	// 接触ダメージを避けるため、最も近い敵から安全な距離を保つ（120px以上を維持）
	const Enemy* nearestEnemy = FindNearestAliveEnemy(enemies, activeEnemyCount);
	if (nearestEnemy != nullptr)
	{
		float eDx = m_x - nearestEnemy->GetX();
		float eDy = m_y - nearestEnemy->GetY();
		float eDist = sqrtf(eDx * eDx + eDy * eDy);
		if (eDist < 120.0f && eDist > 0.001f)
		{
			moveX += (eDx / eDist) * 1.8f;
			moveY += (eDy / eDist) * 1.8f;
		}
	}

	// 自律移動の速度倍率を適用
	float speedMult = 1.0f;
	if (m_skillActive && m_color == ColorPreset::White)
	{
		speedMult = 1.4f; // オーバードライブ中は移動速度1.4倍
	}

	const float speed = PLAYER_SPEED * speedMult;
	float moveLen = sqrtf(moveX * moveX + moveY * moveY);
	if (moveLen > 0.001f)
	{
		moveX /= moveLen;
		moveY /= moveLen;
		m_x += moveX * speed;
		m_y += moveY * speed;
	}

	// 味方NPCを画面の境界内に収める
	const float margin = m_radius;
	if (m_x < margin) m_x = margin;
	if (m_x > SCREEN_WIDTH - margin) m_x = SCREEN_WIDTH - margin;
	if (m_y < margin) m_y = margin;
	if (m_y > SCREEN_HEIGHT - margin) m_y = SCREEN_HEIGHT - margin;

	// 2. 射撃ループ
	if (m_autoFireCooldown > 0)
	{
		--m_autoFireCooldown;
	}
	else
	{
		TryAutoFire(enemies, activeEnemyCount, iceShots, maxShots);
	}

	// 3. アクティブスキルのタイマー処理
	if (m_skillCooldownTimer > 0)
	{
		--m_skillCooldownTimer;
	}

	if (m_skillActive)
	{
		--m_skillActiveTimer;
		if (m_skillActiveTimer <= 0)
		{
			m_skillActive = false;
			int dur, cd;
			const char* name;
			const char* desc;
			GetActiveSkillDetails(m_color, dur, cd, name, desc);
			m_skillCooldownTimer = cd;
		}
	}

	// 爆発の演出タイマー
	if (m_drawExplosion)
	{
		m_explosionDrawRadius += 340.0f / 30.0f; // 30フレームで最大350pxまで拡大
		--m_explosionDrawTimer;
		if (m_explosionDrawTimer <= 0)
		{
			m_drawExplosion = false;
		}
	}

	// 4. スキルの自動発動
	// スキルが未発動、クールダウン中ではない、かつ生存している敵が存在する場合にスキルを誘発
	if (!m_skillActive && m_skillCooldownTimer <= 0)
	{
		bool anyAlive = false;
		for (int i = 0; i < activeEnemyCount; ++i)
		{
			if (enemies[i].IsAlive())
			{
				anyAlive = true;
				break;
			}
		}
		if (anyAlive)
		{
			TriggerSkill(enemies, activeEnemyCount);
		}
	}
}

void Ally::TryAutoFire(const Enemy* enemies, int activeEnemyCount, IceProjectile* iceShots, int maxShots)
{
	const Enemy* target = FindNearestAliveEnemy(enemies, activeEnemyCount);
	if (target == nullptr)
	{
		return;
	}

	// 無限弾薬のシミュレーション（残弾数チェックはなく、味方NPCは無限に射撃可能）
	// 共有ステータスを持つ味方NPCの発射体をターゲット座標にスポーン
	bool spawned = false;
	const float tx = target->GetX();
	const float ty = target->GetY();

	// 味方NPCは通常ショットを発射（コードの簡潔さと堅牢性を保つため、マルチショットや追尾はなし）
	for (int i = 0; i < maxShots; ++i)
	{
		if (!iceShots[i].active)
		{
			iceShots[i].Spawn(m_x, m_y, tx, ty, false, true); // allyShot = true
			spawned = true;
			break;
		}
	}

	if (spawned)
	{
		// プレイヤーの1つ目のステータス（連射速度：FireRate）を共有
		m_autoFireCooldown = GameSession::GetAutoFireInterval();
	}
}

void Ally::TriggerSkill(const Enemy* enemies, int activeEnemyCount)
{
	int dur, cd;
	const char* name;
	const char* desc;
	GetActiveSkillDetails(m_color, dur, cd, name, desc);

	if (m_color == ColorPreset::Brown)
	{
		// 味方NPCの現在座標で即座に自己爆発を発動
		m_explosionX = m_x;
		m_explosionY = m_y;
		m_drawExplosion = true;
		m_explosionDrawRadius = 10.0f;
		m_explosionDrawTimer = 30;

		const int expDamage = GameSession::GetIceDamage() * 5; // プレイヤーの3つ目のステータス（ダメージ：Damage）の5倍を共有
		const float expRadius = 350.0f;

		for (int i = 0; i < activeEnemyCount; ++i)
		{
			// 敵にダメージを適用するため、非constキャストを行う
			Enemy& enemy = const_cast<Enemy&>(enemies[i]);
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
		m_skillActiveTimer = 1; // 即座に終了してクールダウンのサイクルに入る
	}
	else if (m_color == ColorPreset::Black)
	{
		// シンギュラリティ：味方NPCの位置に、周囲の敵を引き寄せるブラックホールを生成
		m_blackholeX = m_x;
		m_blackholeY = m_y;
		m_skillActive = true;
		m_skillActiveTimer = dur;
	}
	else
	{
		m_skillActive = true;
		m_skillActiveTimer = dur;
	}
}

const Enemy* Ally::FindNearestAliveEnemy(const Enemy* enemies, int activeEnemyCount) const
{
	// 優先ターゲット1：タンク（盾持ち）
	const Enemy* nearestTank = nullptr;
	float minTankDistSq = 1e9f;

	for (int i = 0; i < activeEnemyCount; ++i)
	{
		const auto& enemy = enemies[i];
		if (enemy.IsAlive() && enemy.GetType() == EnemyType::Tank)
		{
			float dx = enemy.GetX() - m_x;
			float dy = enemy.GetY() - m_y;
			float distSq = dx * dx + dy * dy;
			if (distSq < minTankDistSq)
			{
				minTankDistSq = distSq;
				nearestTank = &enemy;
			}
		}
	}

	if (nearestTank != nullptr)
	{
		return nearestTank;
	}

	// 優先ターゲット2：生存している最も近い一般の敵
	const Enemy* nearestEnemy = nullptr;
	float minDistSq = 1e9f;

	for (int i = 0; i < activeEnemyCount; ++i)
	{
		const auto& enemy = enemies[i];
		if (enemy.IsAlive())
		{
			float dx = enemy.GetX() - m_x;
			float dy = enemy.GetY() - m_y;
			float distSq = dx * dx + dy * dy;
			if (distSq < minDistSq)
			{
				minDistSq = distSq;
				nearestEnemy = &enemy;
			}
		}
	}

	return nearestEnemy;
}

void Ally::Draw() const
{
	if (!m_active)
	{
		return;
	}

	// 1. ランダム化されたColorPresetを使用して本体の円を描画
	const int bodyColor = PlayerSettings::GetPresetBodyColor(m_color);
	const int outlineColor = GetColor(255, 255, 255); // 味方NPC（ALLY）用の目立つ白い輪郭線

	DrawCircle((int)m_x, (int)m_y, (int)m_radius, bodyColor, TRUE);
	DrawCircle((int)m_x, (int)m_y, (int)m_radius, outlineColor, FALSE);

	// 2. 味方NPCに未来的な「コ・パイロット（副操縦士）」風のビジュアルを与えるため、内部のディテールを描画
	DrawCircle((int)m_x, (int)m_y, (int)(m_radius * 0.4f), GetColor(255, 255, 255), TRUE);

	// 3. 味方NPCの頭上に高精細なビジュアルタグを描画
	const int textColor = GetColor(200, 255, 220);
	const int textShadowColor = GetColor(0, 0, 0);

	// ドロップシャドウ付きのテキスト
	DrawFormatString((int)m_x - 39, (int)m_y - 34, textShadowColor, "[ALLY: NPC]");
	DrawFormatString((int)m_x - 40, (int)m_y - 35, textColor, "[ALLY: NPC]");
}

void Ally::GetActiveSkillDetails(ColorPreset preset, int& outDuration, int& outCooldown, const char*& outName, const char*& outDesc) const
{
	switch (preset)
	{
	case ColorPreset::Blue:
		outDuration = 60;
		outCooldown = 300;
		outName = "Frost Touch";
		outDesc = "Slow on hit (3s)";
		break;
	case ColorPreset::Green:
		outDuration = 120;
		outCooldown = 600;
		outName = "Bullet Save";
		outDesc = "Inf ammo (2s)";
		break;
	case ColorPreset::Yellow:
		outDuration = 120;
		outCooldown = 600;
		outName = "Chain Shot";
		outDesc = "Chains to next";
		break;
	case ColorPreset::Pink:
		outDuration = 60;
		outCooldown = 780;
		outName = "Shared Pain";
		outDesc = "Splash dmg all";
		break;
	case ColorPreset::Orange:
		outDuration = 120;
		outCooldown = 480;
		outName = "Acid Burn";
		outDesc = "DOT on hit (3s)";
		break;
	case ColorPreset::Black:
		outDuration = 180;
		outCooldown = 1200;
		outName = "Black Hole";
		outDesc = "Pull enemies";
		break;
	case ColorPreset::White:
		outDuration = 600;
		outCooldown = 900;
		outName = "Overdrive";
		outDesc = "Speed x1.4";
		break;
	case ColorPreset::Brown:
		outDuration = 1;
		outCooldown = 720;
		outName = "Explosion";
		outDesc = "Blast x5 dmg";
		break;
	default:
		outDuration = 0;
		outCooldown = 0;
		outName = "";
		outDesc = "";
		break;
	}
}
