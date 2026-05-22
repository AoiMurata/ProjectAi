// =============================================================================
// InputManager.h
// Key/mouse edge detection helpers
// =============================================================================
#pragma once

class InputManager
{
public:
	// コンストラクタ
	InputManager();
	// デストラクタ
	~InputManager();

	// 指定されたキーが押された瞬間だけ 1 を返す関数
	static int CheckDownKey(int KeyCode);

	// 指定されたキーが離された瞬間だけ 1 を返す関数
	static int CheckUpKey(int KeyCode);

	// 指定されたキーを押し続けている間１を返す関数
	static int CheckPressKey(int KeyCode);

	// 指定されたマウスボタンが押された瞬間だけ 1 を返す関数
	static int CheckDownMouse(int MouseCode);

	// 指定されたマウスボタンが話された瞬間だけ 1 を返す関数
	static int CheckUpMouse(int MouseCode);

	// 指定されたマウスボタンを押し続けている間１を返す関数
	static int CheckPressMouse(int MouseCode);

private:
	static int mDownBuffer[256];	// CheckDownKey用のキーバッファ
	static int mUpBuffer[256];		// CheckUpKey用のキーバッファ
	static int mMouseDownBuffer[3];	// CheckDownMouse用のマウスバッファ
	static int mMouseUpBuffer[3];	// CheckUpMouse用のマウスバッファ
};
