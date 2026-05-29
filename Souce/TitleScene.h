// =============================================================================
// TitleScene.h
// 冷蔵庫の開閉演出を伴うタイトル画面および設定・カラーカスタム画面の定義ヘッダーファイル
// =============================================================================
#pragma once

#include "SceneBase.h"

// タイトル画面内での表示・入力処理モード
enum class TitleScreenMode
{
	PressEnter,     // 「ENTERキーを押してください」の初期待機状態
	DoorOpening,    // 冷蔵庫の扉が開くアニメーション中
	MainMenu,       // メインメニュー（ゲーム開始、設定、終了など）
	Settings,       // 設定画面（デバッグラウンド選択など）
	ColorCustomize  // プレイヤーカラーのカスタマイズ画面
};

// タイトル画面シーンを制御するクラス
class TitleScene : public SceneBase
{
public:
	// シーン開始時の初期化
	void OnEnter() override;
	// タイトル画面全体の毎フレーム状態の更新処理
	SceneType Update() override;
	// タイトル画面全体の毎フレーム描画処理
	void Draw() override;

private:
	// 各モード毎の個別ロジック更新
	SceneType UpdatePressEnter();
	SceneType UpdateDoorOpening();
	SceneType UpdateMainMenu();
	SceneType UpdateSettings();
	SceneType UpdateColorCustomize();
	
	// 選択されたメインメニューを実行する処理
	SceneType ActivateMenuItem(int index);

	// 冷蔵庫の立体的なドア開閉ビジュアルを描画する
	void DrawRefrigerator(float doorOpenRatio);
	
	// 各モード毎の個別グラフィック描画
	void DrawPressEnter();
	void DrawMainMenu();
	void DrawSettings();
	void DrawColorCustomize();

	// マウス座標からホバーしているメニュー項目インデックスを取得
	int GetMenuIndexAt(int mouseX, int mouseY) const;
	// メニュー項目の矩形範囲（座標・幅・高さ）を計算して取得
	void GetMenuItemRect(int index, int& outX, int& outY, int& outW, int& outH) const;

	static constexpr int MENU_COUNT = 4;        // メニューの項目総数
	static constexpr int DOOR_OPEN_FRAMES = 45; // ドア開閉演出にかかるトータルフレーム数

	TitleScreenMode m_mode = TitleScreenMode::PressEnter; // 現在のタイトル画面モード
	int m_menuIndex = 0;             // キーボード操作用の現在選択中のメニュー項目インデックス
	int m_hoveredMenuIndex = -1;     // マウス操作用の現在ホバー中のメニュー項目インデックス
	int m_colorIndex = 0;            // キーボード操作用の現在選択中のプレイヤーカラーインデックス
	int m_hoveredColorIndex = -1;    // マウス操作用の現在ホバー中のプレイヤーカラーインデックス
	int m_doorOpenFrame = 0;         // ドア開閉演出の経過フレームカウンタ

	bool m_debugMode = false;        // デバッグのステージ選択がアクティブかどうかのフラグ
	int m_debugSelectIndex = 0;      // デバッグ選択時のターゲットラウンド数
};
