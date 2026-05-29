// =============================================================================
// SceneManager.cpp
// シーンの遷移、更新、描画管理処理の実装ファイル
// =============================================================================
#include "SceneManager.h"
#include "TitleScene.h"
#include "MainScene.h"
#include "SkillPickScene.h"
#include "ResultScene.h"

// コンストラクタ（メンバ変数を初期化）
SceneManager::SceneManager()
	: m_nextScene(SceneType::None)
{
}

// デストラクタ
SceneManager::~SceneManager() = default;

// マネージャーの初期シーン設定と初期化
void SceneManager::Initialize(SceneType firstScene)
{
	m_currentScene = CreateScene(firstScene);
	if (m_currentScene)
	{
		m_currentScene->OnEnter(); // 初期シーンの開始処理を呼び出す
	}
	m_nextScene = SceneType::None;
}

// ロジック更新処理
void SceneManager::Update()
{
	if (!m_currentScene)
	{
		return;
	}

	// 現在のアクティブなシーンのUpdateを呼び出し、戻り値から次の遷移先シーンをチェック
	SceneType requested = m_currentScene->Update();
	if (requested == SceneType::QuitApp)
	{
		m_quitApp = true; // 終了がリクエストされた場合はフラグを立てる
	}
	else if (requested != SceneType::None)
	{
		RequestChangeScene(requested); // 有効な遷移先があればリクエストを設定
	}

	// シーンの切り替えリクエストがある場合、このフレーム更新の最後に切り替えを実行する
	if (m_nextScene != SceneType::None)
	{
		ChangeScene(m_nextScene);
		m_nextScene = SceneType::None;
	}
}

// 描画処理
void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->Draw(); // 現在アクティブなシーンの描画関数を呼び出す
	}
}

// 終了フラグのゲッター
bool SceneManager::ShouldQuitApp() const
{
	return m_quitApp;
}

// シーン切り替えリクエストを設定する
void SceneManager::RequestChangeScene(SceneType nextScene)
{
	m_nextScene = nextScene;
}

// シーンの切り替えリクエストに応じて、該当シーンのインスタンスを生成する
std::unique_ptr<SceneBase> SceneManager::CreateScene(SceneType type)
{
	switch (type)
	{
	case SceneType::Title:
		return std::make_unique<TitleScene>();
	case SceneType::Main:
		return std::make_unique<MainScene>();
	case SceneType::SkillPick:
		return std::make_unique<SkillPickScene>();
	case SceneType::Result:
		return std::make_unique<ResultScene>();
	default:
		return nullptr;
	}
}

// 実際のシーンオブジェクト切り替えおよび遷移時関数のコール処理
void SceneManager::ChangeScene(SceneType nextScene)
{
	// 古いシーンのクリーンアップ（終了）関数を呼び出す
	if (m_currentScene)
	{
		m_currentScene->OnExit();
	}

	// 新しいシーンのインスタンスを作成し、その開始関数を呼び出す
	m_currentScene = CreateScene(nextScene);
	if (m_currentScene)
	{
		m_currentScene->OnEnter();
	}
}
