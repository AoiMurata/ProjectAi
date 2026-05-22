// =============================================================================
// ResultScene.cpp
// =============================================================================
#include "ResultScene.h"
#include "GameSession.h"
#include "Constants.h"
#include "DxLib.h"

void ResultScene::OnEnter()
{
	m_resultText = "GAME OVER";
	m_resultColor = GetColor(255, 100, 100);
}

SceneType ResultScene::Update()
{
	if (CheckHitKey(KEY_INPUT_SPACE) == 1)
	{
		return SceneType::Title;
	}

	return SceneType::None;
}

void ResultScene::Draw()
{
	SetBackgroundColor(30, 30, 50);

	DrawFormatString(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 - 120,
		m_resultColor, "%s", m_resultText);

	DrawFormatString(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50,
		GetColor(255, 220, 120), "Rounds cleared: %d",
		GameSession::GetRoundsCleared());

	DrawFormatString(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 - 10,
		GetColor(220, 220, 220), "Final Score: %d", GameSession::GetScore());

	DrawFormatString(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 + 30,
		GetColor(120, 255, 160), "High Score: %d", GameSession::GetHighScore());

	DrawFormatString(SCREEN_WIDTH / 2 - 280, SCREEN_HEIGHT / 2 + 80,
		GetColor(180, 180, 180),
		"You ran out of shots before defeating all enemies.");

	DrawFormatString(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 + 130,
		GetColor(180, 180, 180), "%s", m_guideText);
}
