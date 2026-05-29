// =============================================================================
// InputManager.cpp
// キーボード・マウスの各入力エッジ検知（トリガー・リリース）処理の実装ファイル
// =============================================================================
#include "InputManager.h"
#include "DxLib.h"

// 静的メンバ変数の初期化
int InputManager::mDownBuffer[256] = { 0 };
int InputManager::mUpBuffer[256] = { 0 };
int InputManager::mMouseDownBuffer[3] = { 0 };
int InputManager::mMouseUpBuffer[3] = { 0 };

// コンストラクタ
InputManager::InputManager()
{
}

// デストラクタ
InputManager::~InputManager()
{
}

// キーが押された瞬間を検知する
int InputManager::CheckDownKey(int KeyCode)
{
	int result = 0;

	// 指定されたキーの現在の入力を取得 (1: 押されている, 0: 離されている)
	int keyState = CheckHitKey(KeyCode);

	// 前フレームで押されておらず、現フレームで押されている場合のみトリガー検知とする
	if (mDownBuffer[KeyCode] == 0 && keyState == 1)
	{
		result = 1;
	}

	// 現在の入力を前フレーム用としてバッファに記憶
	mDownBuffer[KeyCode] = keyState;

	return result;
}

// キーが離された瞬間を検知する
int InputManager::CheckUpKey(int KeyCode)
{
	int result = 0;

	// 指定されたキーの現在の入力を取得
	int keyState = CheckHitKey(KeyCode);

	// 前フレームで押されており、現フレームで離されている場合のみリリース検知とする
	if (mUpBuffer[KeyCode] == 1 && keyState == 0)
	{
		result = 1;
	}

	// 現在の入力をバッファに記憶
	mUpBuffer[KeyCode] = keyState;

	return result;
}

// キーが押されているかどうかの状態生値を取得する
int InputManager::CheckPressKey(int KeyCode)
{
	return CheckHitKey(KeyCode);
}

// マウスボタンが押された瞬間を検知する
int InputManager::CheckDownMouse(int MouseCode)
{
	int result = 0;
	// 指定されたボタンの現在の押下状態を取得
	int mouseState = CheckPressMouse(MouseCode);

	// 前フレームで押されておらず、現フレームで押されている場合のみトリガーとする
	if (mMouseDownBuffer[MouseCode] == 0 && mouseState == 1)
	{
		result = 1;
	}
	// 現在の状態をバッファに記憶
	mMouseDownBuffer[MouseCode] = mouseState;
	return result;
}

// マウスボタンが離された瞬間を検知する
int InputManager::CheckUpMouse(int MouseCode)
{
	int result = 0;
	// 指定されたボタンの現在の押下状態を取得
	int mouseState = CheckPressMouse(MouseCode);

	// 前フレームで押されており、現フレームで離されている場合のみリリースとする
	if (mMouseUpBuffer[MouseCode] == 1 && mouseState == 0)
	{
		result = 1;
	}
	// 現在の状態をバッファに記憶
	mMouseUpBuffer[MouseCode] = mouseState;
	return result;
}

// マウスボタンが現在押されているかを判定する
int InputManager::CheckPressMouse(int MouseCode)
{
	// ＤＸライブラリのGetMouseInput関数の出力から指定ビットと論理積をとる
	return GetMouseInput() & MouseCode;
}