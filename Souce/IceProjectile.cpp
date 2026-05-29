// =============================================================================
// IceProjectile.cpp
// =============================================================================
#include "IceProjectile.h"
#include "Constants.h"
#include "DxLib.h"
#include <cmath>

void IceProjectile::Spawn(float fromX, float fromY, float targetX, float targetY, bool useHoming, bool allyShot)
{
	x = fromX;
	y = fromY;
	radius = ICE_RADIUS;
	active = true;
	homing = useHoming;
	isChain = false;
	isAllyShot = allyShot;
	lifetime = 0;

	float dx = targetX - fromX;
	float dy = targetY - fromY;
	const float len = sqrtf(dx * dx + dy * dy);
	if (len < 0.001f)
	{
		dx = 1.0f;
		dy = 0.0f;
	}
	else
	{
		dx /= len;
		dy /= len;
	}

	vx = dx * ICE_SPEED;
	vy = dy * ICE_SPEED;
}

void IceProjectile::Update(float targetX, float targetY, bool hasTarget, float homingTurnRate)
{
	if (!active)
	{
		return;
	}

	++lifetime;

	// パフォーマンス最適化とラグ防止のため、追尾弾は2秒後（120フレーム経過）にデスポーンする
	if (homing && lifetime >= 120)
	{
		active = false;
		return;
	}

	if (homing && hasTarget && homingTurnRate > 0.0f)
	{
		float dx = targetX - x;
		float dy = targetY - y;
		const float len = sqrtf(dx * dx + dy * dy);
		if (len > 0.001f)
		{
			dx /= len;
			dy /= len;
			vx += (dx * ICE_SPEED - vx) * homingTurnRate;
			vy += (dy * ICE_SPEED - vy) * homingTurnRate;
		}
	}

	x += vx;
	y += vy;

	const float margin = radius;
	if (x < -margin || x > SCREEN_WIDTH + margin ||
		y < -margin || y > SCREEN_HEIGHT + margin)
	{
		active = false;
	}
}

void IceProjectile::Draw() const
{
	if (!active)
	{
		return;
	}

	int iceColor = homing ? GetColor(140, 220, 255) : GetColor(180, 240, 255);
	int coreColor = GetColor(255, 255, 255);

	if (isAllyShot)
	{
		// 味方NPCの弾用に、特徴的なプレミアム・ライトエメラルドの色調を適用
		iceColor = GetColor(150, 240, 190);
		coreColor = GetColor(230, 255, 240);
	}

	DrawCircle((int)x, (int)y, (int)radius, iceColor, TRUE);
	DrawCircle((int)x, (int)y, (int)(radius * 0.45f), coreColor, TRUE);
}
