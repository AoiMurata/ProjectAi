// =============================================================================
// PlayerSettings.cpp
// プレイヤーのカラーカスタム、プリセット切り替え、禁止色検出などの実装ファイル
// =============================================================================
#include "PlayerSettings.h"
#include "DxLib.h"

namespace
{
	// プリセット用構造体
	struct PresetData
	{
		const char* name; // プリセット名
		int r;            // 赤の初期値
		int g;            // 緑の初期値
		int b;            // 青の初期値
	};

	// 各プリセットの色データ（DxLibの描画色に対応）
	const PresetData PRESETS[] = {
		{ "Blue",   60,  140, 255 },
		{ "Green",  80,  200, 100 },
		{ "Yellow", 255, 230,  60 },
		{ "Pink",   255, 140, 200 },
		{ "Orange", 255, 140,  60 },
		{ "Black",   40,   40,  40 },
		{ "White",  240,  240, 240 },
		{ "Brown",  120,   80,  50 },
	};

	// プレイヤー設定色の静的状態変数（初期値は青 preset）
	int s_r = 60;
	int s_g = 140;
	int s_b = 255;
	ColorPreset s_selected = ColorPreset::Blue;

	// 2つの色のユークリッド類似度（色相差）を算出する簡易ヘルパー関数
	int ColorDistance(int r1, int g1, int b1, int r2, int g2, int b2)
	{
		return abs(r1 - r2) + abs(g1 - g2) + abs(b1 - b2);
	}
}

namespace PlayerSettings
{
	// 初期設定（ゲーム起動時に呼び出される）
	void Init()
	{
		ApplyPreset(ColorPreset::Blue);
	}

	// プレイヤーのカスタム色を設定する（禁止色でなければ適用）
	void SetColor(int r, int g, int b)
	{
		if (IsForbiddenColor(r, g, b))
		{
			return; // 禁止色なら適用せずに無視する
		}
		s_r = r;
		s_g = g;
		s_b = b;
	}

	// 指定プリセットの配色を読み込んで適用する
	void ApplyPreset(ColorPreset preset)
	{
		const int index = (int)preset;
		if (index < 0 || index >= (int)ColorPreset::Count)
		{
			return;
		}

		const PresetData& p = PRESETS[index];
		s_selected = preset;
		s_r = p.r;
		s_g = p.g;
		s_b = p.b;
	}

	// 敵キャラクター（一般敵・中ボス・大ボス・メディックなど）と同じようなカラーであるかを調べる
	bool IsForbiddenColor(int r, int g, int b)
	{
		// 敵の固有色（一般敵赤、ボスピンク、大ボス赤など）
		const int forbidden[][3] = {
			{ 220,  80,  80 }, // 一般敵の赤
			{ 255, 140, 140 }, // 一般敵の逃走時の薄赤
			{ 200, 100, 255 }, // 中ボスの薄紫
			{ 255,  60,  60 }, // 大ボスの真っ赤
			{ 120,  20,  20 }, // 敵の濃い赤アウトライン
		};

		// 判定対象の色が、敵の禁止色リストといずれか1つでも近い色差（閾値90未満）にあれば禁止色とする
		for (const auto& c : forbidden)
		{
			if (ColorDistance(r, g, b, c[0], c[1], c[2]) < 90)
			{
				return true;
			}
		}
		return false;
	}

	// 現在のプレイヤーのカスタム色が禁止色リストに該当するかどうか
	bool IsCurrentColorForbidden()
	{
		return IsForbiddenColor(s_r, s_g, s_b);
	}

	// プリセットの総数を返す
	int GetPresetCount()
	{
		return (int)ColorPreset::Count;
	}

	// 現在選択されているプリセットを返す
	ColorPreset GetSelectedPreset()
	{
		return s_selected;
	}

	// 指定されたプリセットの名前を文字列で返す
	const char* GetPresetName(ColorPreset preset)
	{
		const int index = (int)preset;
		if (index < 0 || index >= (int)ColorPreset::Count)
		{
			return "";
		}
		return PRESETS[index].name;
	}

	// 指定されたプリセットの描画カラー（DxLibカラー）を返す
	int GetPresetBodyColor(ColorPreset preset)
	{
		const int index = (int)preset;
		if (index < 0 || index >= (int)ColorPreset::Count)
		{
			return GetColor(255, 255, 255);
		}
		const PresetData& p = PRESETS[index];
		return GetColor(p.r, p.g, p.b);
	}

	// 各色のゲッター
	int GetR() { return s_r; }
	int GetG() { return s_g; }
	int GetB() { return s_b; }

	// 現在のプレイヤーのカラーハンドルを返す
	int GetBodyColor()
	{
		return GetColor(s_r, s_g, s_b);
	}

	// 現在のプレイヤー色のアウトライン色（輝度を3分の1に落としたダークカラー）を返す
	int GetOutlineColor()
	{
		return GetColor(s_r / 3, s_g / 3, s_b / 3);
	}

	// 球体のカラープレビューを描画する
	void DrawPreview(int x, int y, int radius)
	{
		// プレイヤーカスタマイズプレビュー用の塗り潰し円
		DrawCircle(x, y, radius, GetBodyColor(), TRUE);
		// 少しダークな輪郭円を描画して立体感を出す
		DrawCircle(x, y, radius, GetOutlineColor(), FALSE);
	}
}
