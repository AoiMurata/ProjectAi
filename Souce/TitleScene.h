// =============================================================================
// TitleScene.h
// =============================================================================
#pragma once

#include "SceneBase.h"

enum class TitleScreenMode
{
	PressEnter,
	DoorOpening,
	MainMenu,
	Settings,
	ColorCustomize
};

class TitleScene : public SceneBase
{
public:
	void OnEnter() override;
	SceneType Update() override;
	void Draw() override;

private:
	SceneType UpdatePressEnter();
	SceneType UpdateDoorOpening();
	SceneType UpdateMainMenu();
	SceneType UpdateSettings();
	SceneType UpdateColorCustomize();
	SceneType ActivateMenuItem(int index);

	void DrawRefrigerator(float doorOpenRatio);
	void DrawPressEnter();
	void DrawMainMenu();
	void DrawSettings();
	void DrawColorCustomize();

	int GetMenuIndexAt(int mouseX, int mouseY) const;
	void GetMenuItemRect(int index, int& outX, int& outY, int& outW, int& outH) const;

	static constexpr int MENU_COUNT = 4;
	static constexpr int DOOR_OPEN_FRAMES = 45;

	TitleScreenMode m_mode = TitleScreenMode::PressEnter;
	int m_menuIndex = 0;
	int m_hoveredMenuIndex = -1;
	int m_colorIndex = 0;
	int m_hoveredColorIndex = -1;
	int m_doorOpenFrame = 0;

	bool m_debugMode = false;
	int m_debugSelectIndex = 0;
};
