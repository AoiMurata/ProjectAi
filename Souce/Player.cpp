// =============================================================================
// Player.cpp
// プレイヤーキャラクターの移動処理および描画処理の実装ファイル
// =============================================================================
#include "Player.h"
#include "PlayerSettings.h"
#include "Constants.h"
#include "DxLib.h"
#include <cmath>

// プレイヤーの状態をリセット（初期化）する
void Player::Reset(float x, float y)
{
	m_x = x;                 // 初期X座標を設定
	m_y = y;                 // 初期Y座標を設定
	m_radius = PLAYER_RADIUS; // 定数からプレイヤーの半径を設定
}

// プレイヤーの移動処理を更新する
void Player::Update(bool canMove, float speedMultiplier)
{
	// スタン中など移動不可の場合は何もしない
	if (!canMove)
	{
		return;
	}

	float dx = 0.0f;
	float dy = 0.0f;

	// W, S, A, D キーで上下左右の移動入力を検知
	if (CheckHitKey(KEY_INPUT_W) == 1) dy -= 1.0f;
	if (CheckHitKey(KEY_INPUT_S) == 1) dy += 1.0f;
	if (CheckHitKey(KEY_INPUT_A) == 1) dx -= 1.0f;
	if (CheckHitKey(KEY_INPUT_D) == 1) dx += 1.0f;

	// 入力がある場合、斜め移動の移動速度を一定にするために正規化を行う
	if (dx != 0.0f || dy != 0.0f)
	{
		const float len = sqrtf(dx * dx + dy * dy);
		dx /= len;
		dy /= len;
		// 速度定数と速度倍率を適用して移動座標を更新
		m_x += dx * PLAYER_SPEED * speedMultiplier;
		m_y += dy * PLAYER_SPEED * speedMultiplier;
	}

	// プレイヤーが画面外に出ないように境界制限をかける（画面端のクリッピング）
	const float margin = m_radius;
	if (m_x < margin) m_x = margin;
	if (m_x > SCREEN_WIDTH - margin) m_x = SCREEN_WIDTH - margin;
	if (m_y < margin) m_y = margin;
	if (m_y > SCREEN_HEIGHT - margin) m_y = SCREEN_HEIGHT - margin;
}

// プレイヤーを描画する
void Player::Draw() const
{
	// カスタマイズ画面で設定された配色を使用して、本体の円を描画
	DrawCircle((int)m_x, (int)m_y, (int)m_radius, PlayerSettings::GetBodyColor(), TRUE);
	// プレイヤーの白い輪郭線を描画
	DrawCircle((int)m_x, (int)m_y, (int)m_radius, PlayerSettings::GetOutlineColor(), FALSE);
}
