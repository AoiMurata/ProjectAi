// =============================================================================
// main.cpp
// Game entry point (WinMain) and main loop
//
// [Visual Studio Setup Instructions (Overview)]
// 1. Create an empty Win32 project
// 2. Add all .cpp / .h files in this folder to the project
// 3. Set the include and library paths for DX Library in the project properties
// 4. Add DxLib.lib (matching x64 or Win32) to the linker input
// 5. Set SubSystem to "Windows" and entry point to WinMain
//
// [Commonly modified parts in this file]
// - Inside the game loop: Frame rate control, common inputs other than ESC exit
// =============================================================================

#include "DxLib.h"
#include "Constants.h"
#include "SceneManager.h"
#include "PlayerSettings.h"

// Entry point for Windows application
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	// Initialize DX Library
	DxLib_Init();

	// Enable mouse pointer display globally
	SetMouseDispFlag(TRUE);

	// -------------------------------------------------------------------------
	// Window & Drawing Settings
	// -------------------------------------------------------------------------
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	SetBackgroundColor(0, 0, 0);
	SetMainWindowText("Refrigerator Battle - Danmaku Dodge");


	// Wait for vertical synchronization when flipping screens (optional; stabilizes FPS)
	SetWaitVSyncFlag(TRUE);

	PlayerSettings::Init();

	SceneManager sceneManager;
	sceneManager.Initialize(SceneType::Title);

	// -------------------------------------------------------------------------
	// Main Loop
	// -------------------------------------------------------------------------
	while (ProcessMessage() == 0)
	{
		sceneManager.Update();
		if (sceneManager.ShouldQuitApp())
		{
			break;
		}

		ClearDrawScreen();
		sceneManager.Draw();

		// Swap back buffer and front buffer (double buffering)
		// SetWaitVSyncFlag(TRUE) prevents screen tearing by waiting for vertical sync
		ScreenFlip();
	}

	// Terminate DX Library
	DxLib_End();
	return 0;
}
