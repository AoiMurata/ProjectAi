// =============================================================================
// SceneManager.h
// シーンの作成、切り替え、毎フレームの更新と描画を管理するクラス
// 【main.cpp内での使用例】
//   SceneManager sceneManager;
//   sceneManager.Initialize(SceneType::Title);  // タイトル画面からスタート
//   ゲームループ内で sceneManager.Update(); と sceneManager.Draw(); を呼び出す
// =============================================================================
#pragma once

#include "SceneBase.h"
#include <memory>

// 各種シーン全体の流れをコントロールするマネージャークラス
class SceneManager
{
public:
	// コンストラクタ
	SceneManager();
	// デストラクタ
	~SceneManager();

	// 開始（最初の）シーンを設定してマネージャーを初期化する
	void Initialize(SceneType firstScene);

	// 現在のシーンのロジックを更新し、必要に応じて遷移処理を実行する
	void Update();

	// 現在のシーンを描画する
	void Draw();

	// アプリケーション終了フラグが立っているかを確認する
	bool ShouldQuitApp() const;

	// 次フレームでのシーン切り替えをリクエストする（Updateの戻り値により自動的に呼び出されます）
	void RequestChangeScene(SceneType nextScene);

private:
	// 指定されたシーンタイプのインスタンス（具象クラス）を動的に生成する
	std::unique_ptr<SceneBase> CreateScene(SceneType type);

	// シーンを切り替える内部ヘルパー処理
	void ChangeScene(SceneType nextScene);

	// 現在プレイ中のアクティブなシーンオブジェクト
	std::unique_ptr<SceneBase> m_currentScene;

	// 次のフレームで切り替える遷移先のシーン（Noneの場合は遷移リクエストなし）
	SceneType m_nextScene;
	
	bool m_quitApp = false; // アプリ終了を指示するフラグ
};
