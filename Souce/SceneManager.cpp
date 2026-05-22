// =============================================================================
// SceneManager.cpp
// Implementation of scene transitions
// =============================================================================
#include "SceneManager.h"
#include "TitleScene.h"
#include "MainScene.h"
#include "SkillPickScene.h"
#include "ResultScene.h"

SceneManager::SceneManager()
	: m_nextScene(SceneType::None)
{
}

SceneManager::~SceneManager() = default;

void SceneManager::Initialize(SceneType firstScene)
{
	m_currentScene = CreateScene(firstScene);
	if (m_currentScene)
	{
		m_currentScene->OnEnter();
	}
	m_nextScene = SceneType::None;
}

void SceneManager::Update()
{
	if (!m_currentScene)
	{
		return;
	}

	// Update the current scene. The return value indicates the requested next scene.
	SceneType requested = m_currentScene->Update();
	if (requested == SceneType::QuitApp)
	{
		m_quitApp = true;
	}
	else if (requested != SceneType::None)
	{
		RequestChangeScene(requested);
	}

	// If there is a transition request, switch the scene at the end of this frame's update.
	if (m_nextScene != SceneType::None)
	{
		ChangeScene(m_nextScene);
		m_nextScene = SceneType::None;
	}
}

void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->Draw();
	}
}

bool SceneManager::ShouldQuitApp() const
{
	return m_quitApp;
}

void SceneManager::RequestChangeScene(SceneType nextScene)
{
	m_nextScene = nextScene;
}

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

void SceneManager::ChangeScene(SceneType nextScene)
{
	// Exit the old scene
	if (m_currentScene)
	{
		m_currentScene->OnExit();
	}

	// Create and enter the new scene
	m_currentScene = CreateScene(nextScene);
	if (m_currentScene)
	{
		m_currentScene->OnEnter();
	}
}
