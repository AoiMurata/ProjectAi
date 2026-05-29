// =============================================================================
// ResultScene.cpp
// リザルト画面での成績表示およびキー入力によるタイトルシーン遷移処理の実装ファイル
// =============================================================================
#include "ResultScene.h"
#include "GameSession.h"
#include "Constants.h"
#include "DxLib.h"

// シーン開始時の初期化
void ResultScene::OnEnter()
{
	m_resultText = "GAME OVER";
	m_resultColor = GetColor(255, 100, 100); // ゲームオーバー用赤色を設定
}

// 毎フレームのロジック更新処理
SceneType ResultScene::Update()
{
	// スペースキーが押されたらタイトル画面に遷移する
	if (CheckHitKey(KEY_INPUT_SPACE) == 1)
	{
		return SceneType::Title;
	}

	return SceneType::None;
}

// リザルト表示を描画する
void ResultScene::Draw()
{
	// 背景色を設定（少し暗めの紺色）
	SetBackgroundColor(30, 30, 50);

	// ゲームオーバーのメイン表示
	DrawFormatString(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 - 120,
		m_resultColor, "%s", m_resultText);

	// クリアラウンド数の表示
	DrawFormatString(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50,
		GetColor(255, 220, 120), "Rounds cleared: %d",
		GameSession::GetRoundsCleared());

	// 今回の最終スコアの表示
	DrawFormatString(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 - 10,
		GetColor(220, 220, 220), "Final Score: %d", GameSession::GetScore());

	// ハイスコアの表示
	DrawFormatString(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 + 30,
		GetColor(120, 255, 160), "High Score: %d", GameSession::GetHighScore());

	// ゲームオーバーの説明ガイド
	DrawFormatString(SCREEN_WIDTH / 2 - 280, SCREEN_HEIGHT / 2 + 80,
		GetColor(180, 180, 180),
		"You ran out of shots before defeating all enemies.");

	// タイトルに戻るためのスペースキー操作案内表示
	DrawFormatString(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 + 130,
		GetColor(180, 180, 180), "%s", m_guideText);
}
