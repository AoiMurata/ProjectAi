// =============================================================================
// Player.h
// プレイヤーキャラクターの定義を行うヘッダーファイル
// =============================================================================
#pragma once

// プレイヤーを表すクラス
class Player
{
public:
	// プレイヤーを初期位置でリセット（初期化）する
	void Reset(float x, float y);

	// プレイヤーの移動更新処理（移動可否、移動速度倍率を適用）
	void Update(bool canMove, float speedMultiplier = 1.0f);

	// プレイヤーの描画処理
	void Draw() const;

	// X座標を取得
	float GetX() const { return m_x; }
	// Y座標を取得
	float GetY() const { return m_y; }
	// 当たり判定の半径を取得
	float GetRadius() const { return m_radius; }

private:
	float m_x = 0.0f;       // プレイヤーの現在X座標
	float m_y = 0.0f;       // プレイヤーの現在Y座標
	float m_radius = 0.0f;  // プレイヤーの当たり判定の半径
};
