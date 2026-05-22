// =============================================================================
// SceneBase.h
// Abstract base class for all scenes (Title, Main, Result)
// [To add a new scene] Inherit from this class when creating a new scene.
// =============================================================================
#pragma once

// Enum representing the type of scene
// Used by SceneManager to determine which scene to transition to next
enum class SceneType
{
	Title,     // Title screen
	Main,      // Main game screen
	SkillPick, // Pick a skill upgrade after clearing a round
	Result,    // Game over / final results
	QuitApp,   // Exit application
	None       // No transition (continue current scene)
};

// Common interface for all scenes (pure virtual functions)
class SceneBase
{
public:
	virtual ~SceneBase() = default;

	// Called once immediately after entering the scene (for initialization)
	virtual void OnEnter() {}

	// Called once immediately before exiting the scene (for cleanup)
	virtual void OnExit() {}

	// Update logic called every frame
	// Return value: The scene to transition to next. SceneType::None if no transition.
	virtual SceneType Update() = 0;

	// Drawing logic called every frame
	virtual void Draw() = 0;
};
