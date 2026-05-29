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
	bool isAllyShot = false;
	int lifetime = 0;

	void Spawn(float fromX, float fromY, float targetX, float targetY, bool useHoming, bool allyShot = false);
	void Update(float targetX, float targetY, bool hasTarget, float homingTurnRate);
	void Draw() const;
};
