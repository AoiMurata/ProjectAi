
/*

キー入力に関する補助関数
DXLibに存在しないので自作

*/

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


int InputManager::CheckDownKey(int KeyCode)
{
	// 戻り値用の変数を用意
	int result = 0;

	// 指定キーの現在の状態を取得
	int keyState = CheckHitKey(KeyCode);

	// 前回キーが押されておらず、かつ、現在キーが押されていたら「キーを押した瞬間」とする
	if(mDownBuffer[KeyCode] == 0 && keyState == 1)
	{
		result = 1;
	}

	// 現在のキー状態をバッファに格納
	mDownBuffer[KeyCode] = keyState;

	return result;
}

// 指定されたキーが離された瞬間だけ 1 を返す関数
int InputManager::CheckUpKey(int KeyCode)
{
	// 戻り値用の変数を用意
	int result = 0;

	// 指定キーの現在の状態を取得
	int keyState = CheckHitKey(KeyCode);

	// 前回キーが押されており、かつ、現在キーが押されていなかったら「キーを離した瞬間」とする
	if(mUpBuffer[KeyCode] == 1 && keyState == 0)
	{
		result = 1;
	}

	// 現在のキー状態をバッファに格納
	mUpBuffer[KeyCode] = keyState;

	return result;
}

// 指定されたキーを押し続けている間１を返す関数
int InputManager::CheckPressKey(int KeyCode)
{
	return CheckHitKey(KeyCode);
}

// 指定されたマウスボタンが押された瞬間だけ 1 を返す関数
int InputManager::CheckDownMouse(int MouseCode)
{
	// 戻り値用の変数を用意
	int result = 0;
	// 指定マウスボタンの現在の状態を取得
	int mouseState = CheckPressMouse(MouseCode);
	// 前回マウスボタンが押されておらず、かつ、現在マウスボタンが押されていたら「マウスボタンを押した瞬間」とする
	if(mMouseDownBuffer[MouseCode] == 0 && mouseState == 1)
	{
		result = 1;
	}
	// 現在のマウスボタン状態をバッファに格納
	mMouseDownBuffer[MouseCode] = mouseState;
	return result;
}

// 指定されたマウスボタンが離された瞬間だけ 1 を返す関数 
int InputManager::CheckUpMouse(int MouseCode)
{
	// 戻り値用の変数を用意
	int result = 0;
	// 指定マウスボタンの現在の状態を取得
	int mouseState = CheckPressMouse(MouseCode);
	// 前回マウスボタンが押されており、かつ、現在マウスボタンが押されていなかったら「マウスボタンを離した瞬間」とする
	if(mMouseUpBuffer[MouseCode] == 1 && mouseState == 0)
	{
		result = 1;
	}
	// 現在のマウスボタン状態をバッファに格納
	mMouseUpBuffer[MouseCode] = mouseState;
	return result;
}

// 指定されたマウスボタンを押し続けている間１を返す関数
int InputManager::CheckPressMouse(int MouseCode)
{
	return GetMouseInput() & MouseCode;
}