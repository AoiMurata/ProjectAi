// =============================================================================
// SceneManager.h
// Class that manages scene creation, switching, updates, and drawing
// [Usage in main.cpp]
//   SceneManager sceneManager;
//   sceneManager.Initialize(SceneType::Title);  // Start with Title Scene
//   Call sceneManager.Update(); and sceneManager.Draw(); inside the loop
// =============================================================================
#pragma once

#include "SceneBase.h"
#include <memory>

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	
	void Initialize(SceneType firstScene);

	// Updates the current scene and performs transition if requested
	void Update();

	// Draws the current scene
	void Draw();

	bool ShouldQuitApp() const;

	// Requests a scene transition on the next frame (called via Update return value)
	void RequestChangeScene(SceneType nextScene);

private:
	// Instantiates the scene (creates the subclass based on Type)
	std::unique_ptr<SceneBase> CreateScene(SceneType type);

	// Internal helper to switch the scene
	void ChangeScene(SceneType nextScene);

	// Active scene currently being played
	std::unique_ptr<SceneBase> m_currentScene;

	// Scene to transition to on the next frame (None means no transition)
	SceneType m_nextScene;
	bool m_quitApp = false;
};
