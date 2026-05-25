// =============================================================================
// Player.h
// =============================================================================
#pragma once

class Player
{
public:
	void Reset(float x, float y);
	void Update(bool canMove, float speedMultiplier = 1.0f);
	void Draw() const;

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetRadius() const { return m_radius; }

private:
	float m_x = 0.0f;
	float m_y = 0.0f;
	float m_radius = 0.0f;
};
