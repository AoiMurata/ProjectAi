// =============================================================================
// InputManager.h
// キーボードおよびマウスの入力検知（トリガー判定・リリース判定等）を支援するクラス
// =============================================================================
#pragma once

class InputManager
{
public:
	// コンストラクタ
	InputManager();
	// デストラクタ
	~InputManager();

	// 指定されたキーが「押された瞬間」であるかを判定する（トリガー判定）
	static int CheckDownKey(int KeyCode);

	// 指定されたキーが「離された瞬間」であるかを判定する（リリース判定）
	static int CheckUpKey(int KeyCode);

	// 指定されたキーが「押され続けているか」を判定する（状態の生値取得）
	static int CheckPressKey(int KeyCode);

	// 指定されたマウスボタンが「押された瞬間」であるかを判定する
	static int CheckDownMouse(int MouseCode);

	// 指定されたマウスボタンが「離された瞬間」であるかを判定する
	static int CheckUpMouse(int MouseCode);

	// 指定されたマウスボタンが「押され続けているか」を判定する
	static int CheckPressMouse(int MouseCode);

private:
	static int mDownBuffer[256];	// キーのトリガー判定状態を保持するバッファ
	static int mUpBuffer[256];		// キーのリリース判定状態を保持するバッファ
	static int mMouseDownBuffer[3];	// マウスボタンのトリガー判定状態を保持するバッファ
	static int mMouseUpBuffer[3];	// マウスボタンのリリース判定状態を保持するバッファ
};
