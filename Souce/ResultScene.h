// =============================================================================
// ResultScene.h
// ゲームオーバー時の最終成績を表示するリザルト画面シーンの定義ヘッダーファイル
// =============================================================================
#pragma once

#include "SceneBase.h"

// リザルト画面を管理するクラス
class ResultScene : public SceneBase
{
public:
	// シーン開始時の初期化（ゲーム結果判定と色の設定など）
	void OnEnter() override;
	
	// リザルト画面の入力更新（スペースキー押下でタイトル画面へ遷移）
	SceneType Update() override;
	
	// 最終リザルトテキストやスコア、ハイスコア等の描画処理
	void Draw() override;

private:
	const char* m_resultText = "Result";            // 表示結果テキスト
	const char* m_guideText  = "Press SPACE for Title"; // 操作ガイドテキスト
	int m_resultColor = 0;                         // 結果テキストの描画色
};
