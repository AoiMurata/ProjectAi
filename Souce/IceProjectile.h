// =============================================================================
// IceProjectile.h
// =============================================================================
#pragma once

struct IceProjectile
{
	float x = 0.0f;
	float y = 0.0f;
	float vx = 0.0f;
	float vy = 0.0f;
	float radius = 0.0f;
	bool active = false;
	bool homing = false;
	bool isChain = false;

	void Spawn(float fromX, float fromY, float targetX, float targetY, bool useHoming);
	void Update(float targetX, float targetY, bool hasTarget, float homingTurnRate);
	void Draw() const;
};
