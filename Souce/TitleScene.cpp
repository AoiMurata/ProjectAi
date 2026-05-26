// =============================================================================
// TitleScene.cpp
// =============================================================================
#include "TitleScene.h"
#include "InputManager.h"
#include "GameSession.h"
#include "PlayerSettings.h"
#include "Constants.h"
#include "DxLib.h"
#include <cmath>

namespace
{
	const char* MENU_ITEMS[] = {
		"Start Game",
		"Settings",
		"Quit Game",
		"Change Color"
	};

	const int MENU_START_X = SCREEN_WIDTH / 2 - 140;
	const int MENU_START_Y = SCREEN_HEIGHT / 2 + 20;
	const int MENU_ITEM_W = 280;
	const int MENU_ITEM_H = 36;
	const int MENU_ITEM_GAP = 44;

	int GetMouseX()
	{
		int x = 0;
		int y = 0;
		GetMousePoint(&x, &y);
		return x;
	}

	int GetMouseY()
	{
		int x = 0;
		int y = 0;
		GetMousePoint(&x, &y);
		return y;
	}
}

void TitleScene::OnEnter()
{
	m_mode = TitleScreenMode::PressEnter;
	m_menuIndex = 0;
	m_hoveredMenuIndex = -1;
	m_colorIndex = (int)PlayerSettings::GetSelectedPreset();
	m_hoveredColorIndex = -1;
	m_doorOpenFrame = 0;
	PlayerSettings::Init();
}

void TitleScene::GetMenuItemRect(int index, int& outX, int& outY, int& outW, int& outH) const
{
	outX = MENU_START_X;
	outY = MENU_START_Y + index * MENU_ITEM_GAP;
	outW = MENU_ITEM_W;
	outH = MENU_ITEM_H;
}

int TitleScene::GetMenuIndexAt(int mouseX, int mouseY) const
{
	for (int i = 0; i < MENU_COUNT; ++i)
	{
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		GetMenuItemRect(i, x, y, w, h);

		if (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h)
		{
			return i;
		}
	}
	return -1;
}

void TitleScene::DrawRefrigerator(float doorOpenRatio)
{
	if (doorOpenRatio < 0.0f) doorOpenRatio = 0.0f;
	if (doorOpenRatio > 1.0f) doorOpenRatio = 1.0f;

	const int bodyX = SCREEN_WIDTH / 2 - 220;
	const int bodyY = SCREEN_HEIGHT / 2 - 200;
	const int bodyW = 440;
	const int bodyH = 400;

	const int bodyColor = GetColor(220, 230, 240);
	const int edgeColor = GetColor(140, 150, 170);
	const int handleColor = GetColor(80, 90, 110);

	DrawBox(bodyX, bodyY, bodyX + bodyW, bodyY + bodyH, bodyColor, TRUE);
	DrawBox(bodyX, bodyY, bodyX + bodyW, bodyY + bodyH, edgeColor, FALSE);

	const int doorW = bodyW / 2 - 8;
	const int doorH = bodyH - 24;
	const int doorY = bodyY + 12;
	const int closedDoorX = bodyX + bodyW / 2 - doorW - 4;
	const int openOffset = (int)(doorW * 0.85f * doorOpenRatio);
	const int doorX = closedDoorX + openOffset;

	const int doorColor = GetColor(200, 215, 230);
	DrawBox(doorX, doorY, doorX + doorW, doorY + doorH, doorColor, TRUE);
	DrawBox(doorX, doorY, doorX + doorW, doorY + doorH, edgeColor, FALSE);

	if (doorOpenRatio < 0.95f)
	{
		DrawCircle(doorX + doorW - 18, doorY + doorH / 2, 8, handleColor, TRUE);
	}

	const int innerColor = GetColor(180, 210, 230);
	DrawBox(bodyX + 12, doorY + 12, bodyX + bodyW - 12, doorY + doorH - 12, innerColor, TRUE);

	if (doorOpenRatio > 0.3f)
	{
		const int shelfColor = GetColor(160, 180, 200);
		for (int i = 0; i < 3; ++i)
		{
			const int sy = doorY + 40 + i * 70;
			DrawBox(bodyX + 20, sy, bodyX + bodyW - 20, sy + 6, shelfColor, TRUE);
		}
	}
}

void TitleScene::DrawPressEnter()
{
	DrawRefrigerator(0.0f);

	const int textColor = GetColor(40, 60, 100);
	DrawFormatString(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 30,
		textColor, "Press to Enter");
	DrawFormatString(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 50,
		GetColor(100, 100, 120), "Refrigerator Battle");
}

SceneType TitleScene::UpdatePressEnter()
{
	if (InputManager::CheckDownKey(KEY_INPUT_RETURN) == 1 ||
		InputManager::CheckDownKey(KEY_INPUT_SPACE) == 1)
	{
		m_mode = TitleScreenMode::DoorOpening;
		m_doorOpenFrame = 0;
	}

	return SceneType::None;
}

SceneType TitleScene::UpdateDoorOpening()
{
	++m_doorOpenFrame;
	if (m_doorOpenFrame >= DOOR_OPEN_FRAMES)
	{
		m_mode = TitleScreenMode::MainMenu;
	}

	return SceneType::None;
}

SceneType TitleScene::ActivateMenuItem(int index)
{
	switch (index)
	{
	case 0:
		GameSession::StartNewRun();
		return SceneType::Main;
	case 1:
		m_mode = TitleScreenMode::Settings;
		break;
	case 2:
		return SceneType::QuitApp;
	case 3:
		m_mode = TitleScreenMode::ColorCustomize;
		m_colorIndex = (int)PlayerSettings::GetSelectedPreset();
		break;
	}
	return SceneType::None;
}

SceneType TitleScene::UpdateMainMenu()
{
	const int mouseX = GetMouseX();
	const int mouseY = GetMouseY();
	m_hoveredMenuIndex = GetMenuIndexAt(mouseX, mouseY);

	if (m_hoveredMenuIndex >= 0)
	{
		m_menuIndex = m_hoveredMenuIndex;
	}

	if (InputManager::CheckDownKey(KEY_INPUT_W) == 1)
	{
		m_menuIndex = (m_menuIndex + MENU_COUNT - 1) % MENU_COUNT;
	}
	if (InputManager::CheckDownKey(KEY_INPUT_S) == 1)
	{
		m_menuIndex = (m_menuIndex + 1) % MENU_COUNT;
	}
	if (InputManager::CheckDownKey(KEY_INPUT_Q) == 1)
	{
		m_mode = TitleScreenMode::ColorCustomize;
		m_colorIndex = (int)PlayerSettings::GetSelectedPreset();
		return SceneType::None;
	}

	if (InputManager::CheckDownMouse(MOUSE_INPUT_LEFT) == 1 && m_hoveredMenuIndex >= 0)
	{
		return ActivateMenuItem(m_hoveredMenuIndex);
	}

	if (InputManager::CheckDownKey(KEY_INPUT_RETURN) == 1)
	{
		return ActivateMenuItem(m_menuIndex);
	}

	return SceneType::None;
}

SceneType TitleScene::UpdateSettings()
{
	if (InputManager::CheckDownKey(KEY_INPUT_F1) == 1)
	{
		m_debugMode = !m_debugMode;
		m_debugSelectIndex = 0;
	}

	if (m_debugMode)
	{
		// Press F2 to immediately launch configured game
		if (InputManager::CheckDownKey(KEY_INPUT_F2) == 1)
		{
			GameSession::StartDebugRun();
			m_debugMode = false;
			m_mode = TitleScreenMode::MainMenu;
			return SceneType::Main;
		}

		// A/D to select parameter
		if (InputManager::CheckDownKey(KEY_INPUT_A) == 1)
		{
			m_debugSelectIndex = (m_debugSelectIndex + 6 - 1) % 6;
		}
		if (InputManager::CheckDownKey(KEY_INPUT_D) == 1)
		{
			m_debugSelectIndex = (m_debugSelectIndex + 1) % 6;
		}

		// W/S to increase/decrease
		int diff = 0;
		if (InputManager::CheckDownKey(KEY_INPUT_W) == 1) diff = 1;
		if (InputManager::CheckDownKey(KEY_INPUT_S) == 1) diff = -1;

		if (diff != 0)
		{
			if (m_debugSelectIndex == 0)
			{
				int val = GameSession::GetSkillLevel(SkillType::FireRate);
				GameSession::SetSkillLevel(SkillType::FireRate, val + diff);
			}
			else if (m_debugSelectIndex == 1)
			{
				int val = GameSession::GetSkillLevel(SkillType::ChargeBoost);
				GameSession::SetSkillLevel(SkillType::ChargeBoost, val + diff);
			}
			else if (m_debugSelectIndex == 2)
			{
				int val = GameSession::GetSkillLevel(SkillType::Damage);
				GameSession::SetSkillLevel(SkillType::Damage, val + diff);
			}
			else if (m_debugSelectIndex == 3)
			{
				int val = GameSession::GetSpecialSkillLevel(SpecialSkillType::MultiShot);
				GameSession::SetSpecialSkillLevel(SpecialSkillType::MultiShot, val + diff);
			}
			else if (m_debugSelectIndex == 4)
			{
				int val = GameSession::GetSpecialSkillLevel(SpecialSkillType::Homing);
				GameSession::SetSpecialSkillLevel(SpecialSkillType::Homing, val + diff);
			}
			else if (m_debugSelectIndex == 5)
			{
				int val = GameSession::GetRound();
				GameSession::SetRound(val + diff);
			}
		}
	}

	if (InputManager::CheckDownKey(KEY_INPUT_ESCAPE) == 1 ||
		InputManager::CheckDownKey(KEY_INPUT_B) == 1 ||
		(!m_debugMode && InputManager::CheckDownKey(KEY_INPUT_RETURN) == 1))
	{
		m_mode = TitleScreenMode::MainMenu;
		m_debugMode = false;
	}
	return SceneType::None;
}

SceneType TitleScene::UpdateColorCustomize()
{
	const int presetCount = PlayerSettings::GetPresetCount();
	const int cols = 4;
	const int startX = SCREEN_WIDTH / 2 - 280;
	const int startY = 250;
	const int cellW = 130;
	const int cellH = 50;

	const int mouseX = GetMouseX();
	const int mouseY = GetMouseY();
	m_hoveredColorIndex = -1;

	for (int i = 0; i < presetCount; ++i)
	{
		const int col = i % cols;
		const int row = i / cols;
		const int x = startX + col * cellW;
		const int y = startY + row * cellH;

		if (mouseX >= x && mouseX <= x + cellW - 10 &&
			mouseY >= y && mouseY <= y + cellH - 10)
		{
			m_hoveredColorIndex = i;
			m_colorIndex = i;
		}
	}

	if (InputManager::CheckDownKey(KEY_INPUT_W) == 1)
	{
		m_colorIndex = (m_colorIndex - cols + presetCount) % presetCount;
	}
	if (InputManager::CheckDownKey(KEY_INPUT_S) == 1)
	{
		m_colorIndex = (m_colorIndex + cols) % presetCount;
	}
	if (InputManager::CheckDownKey(KEY_INPUT_A) == 1)
	{
		m_colorIndex = (m_colorIndex - 1 + presetCount) % presetCount;
	}
	if (InputManager::CheckDownKey(KEY_INPUT_D) == 1)
	{
		m_colorIndex = (m_colorIndex + 1) % presetCount;
	}

	if (InputManager::CheckDownMouse(MOUSE_INPUT_LEFT) == 1)
	{
		if (m_hoveredColorIndex >= 0)
		{
			PlayerSettings::ApplyPreset((ColorPreset)m_hoveredColorIndex);
		}
	}

	if (InputManager::CheckDownKey(KEY_INPUT_RETURN) == 1 ||
		InputManager::CheckDownKey(KEY_INPUT_ESCAPE) == 1 ||
		InputManager::CheckDownKey(KEY_INPUT_B) == 1 ||
		InputManager::CheckDownKey(KEY_INPUT_Q) == 1)
	{
		PlayerSettings::ApplyPreset((ColorPreset)m_colorIndex);
		m_mode = TitleScreenMode::MainMenu;
	}

	return SceneType::None;
}

SceneType TitleScene::Update()
{
	switch (m_mode)
	{
	case TitleScreenMode::PressEnter:
		return UpdatePressEnter();
	case TitleScreenMode::DoorOpening:
		return UpdateDoorOpening();
	case TitleScreenMode::Settings:
		return UpdateSettings();
	case TitleScreenMode::ColorCustomize:
		return UpdateColorCustomize();
	default:
		return UpdateMainMenu();
	}
}

void TitleScene::DrawMainMenu()
{
	const float doorRatio = (m_mode == TitleScreenMode::DoorOpening)
		? (float)m_doorOpenFrame / (float)DOOR_OPEN_FRAMES
		: 1.0f;

	DrawRefrigerator(doorRatio);

	DrawFormatString(SCREEN_WIDTH / 2 - 160, MENU_START_Y - 50,
		GetColor(40, 60, 100), "Select an option");

	for (int i = 0; i < MENU_COUNT; ++i)
	{
		const bool selected = (i == m_menuIndex);
		const bool hovered = (i == m_hoveredMenuIndex);

		int textColor;
		if (selected && hovered)
		{
			textColor = GetColor(255, 200, 40);
		}
		else if (hovered)
		{
			textColor = GetColor(255, 160, 60);
		}
		else if (selected)
		{
			textColor = GetColor(40, 100, 220);
		}
		else
		{
			textColor = GetColor(70, 80, 100);
		}

		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		GetMenuItemRect(i, x, y, w, h);

		if (hovered || selected)
		{
			DrawBox(x - 8, y - 4, x + w + 8, y + h + 4,
				GetColor(255, 248, 210), TRUE);
		}

		DrawFormatString(x, y, textColor, "%s%s",
			selected ? "> " : "  ", MENU_ITEMS[i]);
	}

	DrawFormatString(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 80,
		GetColor(100, 100, 120), "W/S: Move  |  Enter: Select  |  Q: Color");
	DrawFormatString(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT - 50,
		GetColor(120, 255, 160), "High Score: %d", GameSession::GetHighScore());

	PlayerSettings::DrawPreview(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 90, 22);
}

void GetColorSkillText(ColorPreset preset, const char*& outName, const char*& outDesc, const char*& outCd)
{
	switch (preset)
	{
	case ColorPreset::Blue:
		outName = "Frost Touch";
		outDesc = "E-key: Attacks slow hit enemies for 3s (Active for 1s).";
		outCd = "Cooldown: 5s";
		break;
	case ColorPreset::Green:
		outName = "Bullet Save";
		outDesc = "E-key: Attacks consume no stored shots for 2s.";
		outCd = "Cooldown: 10s";
		break;
	case ColorPreset::Yellow:
		outName = "Chain Shot";
		outDesc = "E-key: Shot bounces to other close enemies for 2s.";
		outCd = "Cooldown: 10s";
		break;
	case ColorPreset::Pink:
		outName = "Shared Pain";
		outDesc = "E-key: Attacks share damage to all enemies for 1s.";
		outCd = "Cooldown: 13s";
		break;
	case ColorPreset::Orange:
		outName = "Acid Burn";
		outDesc = "E-key: Attacks deal continuous damage for 3s (Active for 2s).";
		outCd = "Cooldown: 8s";
		break;
	case ColorPreset::Black:
		outName = "Black Hole";
		outDesc = "E-key: Spawn gravity singularity pulling enemies for 3s.";
		outCd = "Cooldown: 20s";
		break;
	case ColorPreset::White:
		outName = "Overdrive";
		outDesc = "E-key: Player speed is multiplied by 1.4x for 10s.";
		outCd = "Cooldown: 15s";
		break;
	case ColorPreset::Brown:
		outName = "Self-Explosion";
		outDesc = "E-key: Instantly blast nearby enemies for 5x damage.";
		outCd = "Cooldown: 12s";
		break;
	default:
		outName = "";
		outDesc = "";
		outCd = "";
		break;
	}
}

void TitleScene::DrawSettings()
{
	DrawRefrigerator(1.0f);

	if (m_debugMode)
	{
		DrawFormatString(SCREEN_WIDTH / 2 - 120, 95, GetColor(255, 60, 60), "DEBUG MODE ACTIVE");
		DrawFormatString(SCREEN_WIDTH / 2 - 190, 125, GetColor(80, 90, 110), "A/D: Select skill | W/S: Edit value");

		const char* paramNames[] = {
			"Fire Rate Level",
			"Charge Boost Level",
			"Damage Level",
			"Multi Shot Level",
			"Homing Level",
			"Round Number"
		};

		int paramValues[] = {
			GameSession::GetSkillLevel(SkillType::FireRate),
			GameSession::GetSkillLevel(SkillType::ChargeBoost),
			GameSession::GetSkillLevel(SkillType::Damage),
			GameSession::GetSpecialSkillLevel(SpecialSkillType::MultiShot),
			GameSession::GetSpecialSkillLevel(SpecialSkillType::Homing),
			GameSession::GetRound()
		};

		for (int i = 0; i < 6; ++i)
		{
			const bool selected = (i == m_debugSelectIndex);
			int color = selected ? GetColor(255, 60, 60) : GetColor(60, 80, 100);
			int bgCol = selected ? GetColor(255, 235, 235) : GetColor(240, 245, 250);

			int x = SCREEN_WIDTH / 2 - 160;
			int y = 160 + i * 40;
			DrawBox(x - 10, y - 4, x + 330, y + 26, bgCol, TRUE);
			DrawBox(x - 10, y - 4, x + 330, y + 26, color, FALSE);

			DrawFormatString(x + 10, y, color, "%s %s: %d", selected ? "->" : "  ", paramNames[i], paramValues[i]);
		}

		static int debugPulseFrame = 0;
		++debugPulseFrame;
		const float dp = sinf((float)debugPulseFrame * 0.1f) * 0.5f + 0.5f;
		const int f2Color = GetColor((int)(220 + dp * 35), (int)(180 + dp * 50), (int)(60 + dp * 120));
		DrawFormatString(SCREEN_WIDTH / 2 - 165, SCREEN_HEIGHT - 100, f2Color, "Press F2 to START battle with these stats!");
	}
	else
	{
		DrawFormatString(SCREEN_WIDTH / 2 - 80, 160, GetColor(40, 60, 100), "SETTINGS");
		DrawFormatString(SCREEN_WIDTH / 2 - 200, 230, GetColor(80, 90, 110),
			"(Sound and other options coming soon)");
		
		DrawFormatString(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT - 60,
			GetColor(100, 100, 120), "Enter / ESC: Back to menu");
	}
}

void TitleScene::DrawColorCustomize()
{
	DrawRefrigerator(1.0f);

	DrawFormatString(SCREEN_WIDTH / 2 - 140, 120, GetColor(40, 60, 100), "CHANGE COLOR");
	DrawFormatString(SCREEN_WIDTH / 2 - 220, 160, GetColor(80, 90, 110),
		"Pick a color (enemy colors are not allowed)");

	const int presetCount = PlayerSettings::GetPresetCount();
	const int cols = 4;
	const int startX = SCREEN_WIDTH / 2 - 280;
	const int startY = 230; // Shifted up to fit skill descriptions nicely
	const int cellW = 130;
	const int cellH = 50;

	for (int i = 0; i < presetCount; ++i)
	{
		const int col = i % cols;
		const int row = i / cols;
		const int x = startX + col * cellW;
		const int y = startY + row * cellH;
		const ColorPreset preset = (ColorPreset)i;

		const bool selected = (i == m_colorIndex);
		const bool hovered = (i == m_hoveredColorIndex);

		int labelColor;
		if (selected && hovered)
		{
			labelColor = GetColor(255, 200, 40);
		}
		else if (hovered)
		{
			labelColor = GetColor(255, 160, 60);
		}
		else if (selected)
		{
			labelColor = GetColor(40, 100, 220);
		}
		else
		{
			labelColor = GetColor(70, 80, 100);
		}

		DrawCircle(x + 18, y + 18, 14, PlayerSettings::GetPresetBodyColor(preset), TRUE);
		DrawCircle(x + 18, y + 18, 14, GetColor(60, 60, 60), FALSE);
		DrawFormatString(x + 40, y + 10, labelColor, "%s",
			PlayerSettings::GetPresetName(preset));
	}

	PlayerSettings::DrawPreview(SCREEN_WIDTH / 2 - 230, 410, 26);
	DrawFormatString(SCREEN_WIDTH / 2 - 250, 445, GetColor(80, 90, 110), "Preview");

	const char* sName = "";
	const char* sDesc = "";
	const char* sCd = "";
	GetColorSkillText((ColorPreset)m_colorIndex, sName, sDesc, sCd);

	// Draw beautiful description box on the right of preview
	int descX = SCREEN_WIDTH / 2 - 130;
	int descY = 380;
	int descW = 410;
	int descH = 100;
	int borderCol = PlayerSettings::GetPresetBodyColor((ColorPreset)m_colorIndex);
	DrawBox(descX, descY, descX + descW, descY + descH, GetColor(245, 250, 255), TRUE);
	DrawBox(descX, descY, descX + descW, descY + descH, borderCol, FALSE);

	DrawFormatString(descX + 15, descY + 12, GetColor(40, 100, 220), "Skill [E]: %s", sName);
	DrawFormatString(descX + 15, descY + 40, GetColor(80, 90, 110), "%s", sDesc);
	DrawFormatString(descX + 15, descY + 68, GetColor(200, 60, 60), "%s", sCd);

	DrawFormatString(SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT - 60,
		GetColor(100, 100, 120), "W/S/A/D or Mouse  |  Enter/Q/ESC: Back");
}

void TitleScene::Draw()
{
	SetBackgroundColor(180, 210, 235);

	switch (m_mode)
	{
	case TitleScreenMode::PressEnter:
		DrawPressEnter();
		break;
	case TitleScreenMode::DoorOpening:
	{
		const float ratio = (float)m_doorOpenFrame / (float)DOOR_OPEN_FRAMES;
		DrawRefrigerator(ratio);
		DrawFormatString(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 180,
			GetColor(40, 60, 100), "Opening...");
		break;
	}
	case TitleScreenMode::Settings:
		DrawSettings();
		break;
	case TitleScreenMode::ColorCustomize:
		DrawColorCustomize();
		break;
	default:
		DrawMainMenu();
		break;
	}
}
