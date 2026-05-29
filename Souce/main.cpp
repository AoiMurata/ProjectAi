// =============================================================================
// main.cpp
// ゲームのエントリーポイント（WinMain）およびメインループ処理
//
// 【Visual Studio セットアップ手順（概要）】
// 1. 空の Windows デスクトップ アプリケーション プロジェクトを作成します。
// 2. このフォルダ内にあるすべての .cpp / .h ファイルをプロジェクトに追加します。
// 3. プロジェクトプロパティで ＤＸライブラリ のインクルードパスおよびライブラリパスを設定します。
// 4. リンカー入力に DxLib.lib (x64 または Win32 に一致するもの) を追加します。
// 5. サブシステムを「Windows (/SUBSYSTEM:WINDOWS)」に設定します。
// =============================================================================

#include "DxLib.h"
#include "Constants.h"
#include "SceneManager.h"
#include "PlayerSettings.h"

// Windowsアプリケーションのエントリーポイント
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	// ＤＸライブラリの初期化処理
	DxLib_Init();

	// マウスカーソルをグローバルに表示状態に設定
	SetMouseDispFlag(TRUE);

	// -------------------------------------------------------------------------
	// ウィンドウおよび描画環境の設定
	// -------------------------------------------------------------------------
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);           // 日本語文字列の文字化けを防ぐためにUTF-8を設定
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);          // 解像度と色深度を設定（1280x720, 32bit）
	SetBackgroundColor(0, 0, 0);                           // デフォルト背景色を黒に設定
	SetMainWindowText("Refrigerator Battle - Danmaku Dodge"); // ウィンドウのタイトルバー文字列を設定

	// 画面描画の垂直同期待ち（VSync）を有効にし、FPSをモニタのリフレッシュレートに同期させ画面のティアリングを防止
	SetWaitVSyncFlag(TRUE);

	// プレイヤーカラーカスタマイズ用設定の初期化
	PlayerSettings::Init();

	// シーン管理マネージャーを生成し、タイトル画面から開始するよう初期設定
	SceneManager sceneManager;
	sceneManager.Initialize(SceneType::Title);

	// -------------------------------------------------------------------------
	// メインループ（ゲーム全体の継続処理）
	// -------------------------------------------------------------------------
	// Windowsのメッセージ処理にエラーがない間ループを繰り返す（閉じるボタンなどでの停止に対応）
	while (ProcessMessage() == 0)
	{
		// 現在のアクティブなシーンのロジックを更新
		sceneManager.Update();
		// シーン側からアプリ終了要求があった場合はメインループをブレイクしてゲームを閉じる
		if (sceneManager.ShouldQuitApp())
		{
			break;
		}

		// 画面に描画されている前フレームの画像を消去（黒で塗り潰し）
		ClearDrawScreen();
		
		// 現在アクティブなシーンのゲーム画面を描画
		sceneManager.Draw();

		// ダブルバッファリングにおける裏画面と表画面のフリップ（同期表示）処理を実行
		ScreenFlip();
	}

	// ＤＸライブラリの終了処理（使用したメモリやシステムリソースの解放）
	DxLib_End();
	return 0;
}
