#include "stdafx.h"
#include "UI.h"
#include "UIScene_SettingsKeyBindMenu.h"
#include "..\..\Options.h"

#ifdef _WIN64
#include "..\App_enums.h"
#include "..\..\Windows64\KeyBindings.h"
#include "..\..\Windows64\KeyboardMouseInput.h"

UIScene_SettingsKeyBindMenu::RowEntry UIScene_SettingsKeyBindMenu::s_rows[] = {
	{ eRowType_SectionHeader, 0, L"Mouse" },
	{ eRowType_SensitivityInGame, 0, L"Look Sensitivity" },
	{ eRowType_SensitivityInMenu, 0, L"Menu Sensitivity" },
	{ eRowType_InvertLook, 0, L"Invert Look" },
	{ eRowType_SectionHeader, 0, L"Camera" },
	{ eRowType_Fov, 0, L"FOV" },
	{ eRowType_Gamma, 0, L"Gamma" },
	{ eRowType_ViewBobbing, 0, L"View Bobbing" },
	{ eRowType_SmoothCamera, 0, L"Smooth Camera" },

	{ eRowType_SectionHeader, 0, L"Movement" },
	{ eRowType_Action, eGameAction_MoveForward, L"Forward" },
	{ eRowType_Action, eGameAction_MoveBack, L"Back" },
	{ eRowType_Action, eGameAction_MoveLeft, L"Left" },
	{ eRowType_Action, eGameAction_MoveRight, L"Right" },
	{ eRowType_Action, eGameAction_Jump, L"Jump" },
	{ eRowType_Action, eGameAction_Sneak, L"Sneak" },
	{ eRowType_Action, eGameAction_Sprint, L"Sprint" },
	{ eRowType_SprintMode, 0, L"Sprint Mode" },

	{ eRowType_SectionHeader, 0, L"Gameplay" },
	{ eRowType_Action, eGameAction_Attack, L"Attack" },
	{ eRowType_Action, eGameAction_UseItem, L"Use Item" },
	{ eRowType_Action, eGameAction_PickBlock, L"Pick Block" },
	{ eRowType_Action, eGameAction_Inventory, L"Inventory" },
	{ eRowType_Action, eGameAction_Crafting, L"Crafting" },
	{ eRowType_Action, eGameAction_Drop, L"Drop" },
	{ eRowType_Action, eGameAction_Chat, L"Chat" },
	{ eRowType_Action, eGameAction_Command, L"Command" },
	{ eRowType_Action, eGameAction_PauseMenu, L"Pause" },

	{ eRowType_SectionHeader, 0, L"Hotbar" },
	{ eRowType_Action, eGameAction_Hotbar1, L"Slot 1" },
	{ eRowType_Action, eGameAction_Hotbar2, L"Slot 2" },
	{ eRowType_Action, eGameAction_Hotbar3, L"Slot 3" },
	{ eRowType_Action, eGameAction_Hotbar4, L"Slot 4" },
	{ eRowType_Action, eGameAction_Hotbar5, L"Slot 5" },
	{ eRowType_Action, eGameAction_Hotbar6, L"Slot 6" },
	{ eRowType_Action, eGameAction_Hotbar7, L"Slot 7" },
	{ eRowType_Action, eGameAction_Hotbar8, L"Slot 8" },
	{ eRowType_Action, eGameAction_Hotbar9, L"Slot 9" },
	{ eRowType_Action, eGameAction_HotbarScrollUp, L"Scroll Up" },
	{ eRowType_Action, eGameAction_HotbarScrollDown, L"Scroll Down" },

	{ eRowType_SectionHeader, 0, L"Menu / UI" },
	{ eRowType_Action, eGameAction_MenuUp, L"Menu Up" },
	{ eRowType_Action, eGameAction_MenuDown, L"Menu Down" },
	{ eRowType_Action, eGameAction_MenuLeft, L"Menu Left" },
	{ eRowType_Action, eGameAction_MenuRight, L"Menu Right" },
	{ eRowType_Action, eGameAction_MenuOK, L"Confirm" },
	{ eRowType_Action, eGameAction_MenuCancel, L"Cancel" },
	{ eRowType_Action, eGameAction_MenuAlt, L"Alternate" },
	{ eRowType_Action, eGameAction_MenuQuickMove, L"Quick Move" },
	{ eRowType_Action, eGameAction_MenuTabLeft, L"Tab Left" },
	{ eRowType_Action, eGameAction_MenuTabRight, L"Tab Right" },
	{ eRowType_Action, eGameAction_MenuPageUp, L"Page Up" },
	{ eRowType_Action, eGameAction_MenuPageDown, L"Page Down" },
	{ eRowType_Action, eGameAction_MenuDelete, L"Delete" },
	{ eRowType_Action, eGameAction_MenuPause, L"Menu Pause" },

	{ eRowType_SectionHeader, 0, L"Toggles / Misc" },
	{ eRowType_Action, eGameAction_ToggleThirdPerson, L"Third Person" },
	{ eRowType_Action, eGameAction_ToggleDebug, L"Debug" },
	{ eRowType_Action, eGameAction_TogglePlayerList, L"Player List" },
	{ eRowType_Action, eGameAction_ToggleHUD, L"HUD" },
	{ eRowType_Action, eGameAction_ToggleFullscreen, L"Fullscreen" },
	{ eRowType_Action, eGameAction_Screenshot, L"Screenshot" },
	{ eRowType_Action, eGameAction_ToggleFly, L"Fly" },
	{ eRowType_Action, eGameAction_ToggleFog, L"Fog" },
	{ eRowType_Action, eGameAction_SmoothCamera, L"Toggle Smooth Camera" },
	{ eRowType_Action, eGameAction_ChangeSkin, L"Change Skin" },
	{ eRowType_Action, eGameAction_SpawnCreeper, L"Spawn Creeper" },
	{ eRowType_Action, eGameAction_ZoomIn, L"Zoom In" },
	{ eRowType_Action, eGameAction_ZoomOut, L"Zoom Out" },

	{ eRowType_SectionHeader, 0, L"Defaults" },
	{ eRowType_ResetDefaults, 0, L"Reset Controls to Defaults" },
};

UIScene_SettingsKeyBindMenu::UIScene_SettingsKeyBindMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	initialiseMovie();

	m_eSection = eSection_All;
	if (initData != NULL)
	{
		int sectionValue = (int)(size_t)initData;
		if (sectionValue >= 0 && sectionValue < eSection_All)
		{
			m_eSection = (EControlsSection)sectionValue;
		}
	}

	m_bWaitingForKey = false;
	m_iRebindAction = -1;
	m_iRebindDelay = 0;
	m_iTopRow = 0;
	m_bNeedsRefresh = false;
	m_iSelectedRow = FindNextSelectableRow(0, 1);
	if (m_iSelectedRow < 0)
	{
		m_iSelectedRow = 0;
	}

	UpdateVisibleRows();
	SetFocusToElement(m_iSelectedRow - m_iTopRow);

	doHorizontalResizeCheck();
}

UIScene_SettingsKeyBindMenu::~UIScene_SettingsKeyBindMenu()
{
}

wstring UIScene_SettingsKeyBindMenu::getMoviePath()
{
	if (app.GetLocalPlayerCount() > 1)
	{
		return L"SettingsMenuSplit";
	}

	return L"SettingsMenu";
}

int UIScene_SettingsKeyBindMenu::GetRowCount() const
{
	int count = 0;
	for (int i = 0; i < (int)_countof(s_rows); ++i)
	{
		if (RowBelongsToSection(s_rows[i]))
		{
			++count;
		}
	}
	return count;
}

const UIScene_SettingsKeyBindMenu::RowEntry* UIScene_SettingsKeyBindMenu::GetRow(int rowIndex) const
{
	if (rowIndex < 0)
	{
		return NULL;
	}

	int visibleIndex = 0;
	for (int i = 0; i < (int)_countof(s_rows); ++i)
	{
		if (!RowBelongsToSection(s_rows[i]))
		{
			continue;
		}

		if (visibleIndex == rowIndex)
		{
			return &s_rows[i];
		}

		++visibleIndex;
	}

	return NULL;
}

bool UIScene_SettingsKeyBindMenu::IsInteractiveRow(const RowEntry& row) const
{
	return row.type == eRowType_Action ||
		row.type == eRowType_SensitivityInGame ||
		row.type == eRowType_SensitivityInMenu ||
		row.type == eRowType_Fov ||
		row.type == eRowType_Gamma ||
		row.type == eRowType_ViewBobbing ||
		row.type == eRowType_SmoothCamera ||
		row.type == eRowType_InvertLook ||
		row.type == eRowType_SprintMode ||
		row.type == eRowType_ResetDefaults;
}

bool UIScene_SettingsKeyBindMenu::IsWheelAdjustableRow(const RowEntry& row) const
{
	return row.type == eRowType_SensitivityInGame ||
		row.type == eRowType_SensitivityInMenu ||
		row.type == eRowType_Fov ||
		row.type == eRowType_Gamma;
}

bool UIScene_SettingsKeyBindMenu::RowBelongsToSection(const RowEntry& row) const
{
	if (m_eSection == eSection_All)
	{
		return true;
	}

	switch (m_eSection)
	{
	case eSection_MouseCamera:
		return row.type == eRowType_SensitivityInGame ||
			row.type == eRowType_SensitivityInMenu ||
			row.type == eRowType_Fov ||
			row.type == eRowType_Gamma ||
			row.type == eRowType_ViewBobbing ||
			row.type == eRowType_SmoothCamera ||
			row.type == eRowType_InvertLook;
	case eSection_Movement:
		return row.type == eRowType_SprintMode ||
			(row.type == eRowType_Action &&
			(row.value == eGameAction_MoveForward ||
			 row.value == eGameAction_MoveBack ||
			 row.value == eGameAction_MoveLeft ||
			 row.value == eGameAction_MoveRight ||
			 row.value == eGameAction_Jump ||
			 row.value == eGameAction_Sneak ||
			 row.value == eGameAction_Sprint));
	case eSection_Gameplay:
		return row.type == eRowType_Action &&
			(row.value == eGameAction_Attack ||
			 row.value == eGameAction_UseItem ||
			 row.value == eGameAction_PickBlock ||
			 row.value == eGameAction_Inventory ||
			 row.value == eGameAction_Crafting ||
			 row.value == eGameAction_Drop ||
			 row.value == eGameAction_Chat ||
			 row.value == eGameAction_Command ||
			 row.value == eGameAction_PauseMenu);
	case eSection_HotbarInventory:
		return row.type == eRowType_Action &&
			(row.value == eGameAction_Hotbar1 ||
			 row.value == eGameAction_Hotbar2 ||
			 row.value == eGameAction_Hotbar3 ||
			 row.value == eGameAction_Hotbar4 ||
			 row.value == eGameAction_Hotbar5 ||
			 row.value == eGameAction_Hotbar6 ||
			 row.value == eGameAction_Hotbar7 ||
			 row.value == eGameAction_Hotbar8 ||
			 row.value == eGameAction_Hotbar9 ||
			 row.value == eGameAction_HotbarScrollUp ||
			 row.value == eGameAction_HotbarScrollDown ||
			 row.value == eGameAction_MenuQuickMove);
	case eSection_MenuUI:
		return row.type == eRowType_Action &&
			(row.value == eGameAction_MenuUp ||
			 row.value == eGameAction_MenuDown ||
			 row.value == eGameAction_MenuLeft ||
			 row.value == eGameAction_MenuRight ||
			 row.value == eGameAction_MenuOK ||
			 row.value == eGameAction_MenuCancel ||
			 row.value == eGameAction_MenuAlt ||
			 row.value == eGameAction_MenuTabLeft ||
			 row.value == eGameAction_MenuTabRight ||
			 row.value == eGameAction_MenuPageUp ||
			 row.value == eGameAction_MenuPageDown ||
			 row.value == eGameAction_MenuDelete ||
			 row.value == eGameAction_MenuPause);
	case eSection_MiscDefaults:
		return (row.type == eRowType_Action &&
			(row.value == eGameAction_ToggleThirdPerson ||
			 row.value == eGameAction_ToggleDebug ||
			 row.value == eGameAction_TogglePlayerList ||
			 row.value == eGameAction_ToggleHUD ||
			 row.value == eGameAction_ToggleFullscreen ||
			 row.value == eGameAction_Screenshot ||
			 row.value == eGameAction_ToggleFly ||
			 row.value == eGameAction_ToggleFog ||
			 row.value == eGameAction_SmoothCamera ||
			 row.value == eGameAction_ChangeSkin ||
			 row.value == eGameAction_SpawnCreeper ||
			 row.value == eGameAction_ZoomIn ||
			 row.value == eGameAction_ZoomOut)) ||
			row.type == eRowType_ResetDefaults;
	default:
		return true;
	}
}

int UIScene_SettingsKeyBindMenu::FindNextSelectableRow(int startRow, int direction) const
{
	if (direction == 0)
	{
		return -1;
	}

	int rowIndex = startRow;
	while (rowIndex >= 0 && rowIndex < GetRowCount())
	{
		const RowEntry* row = GetRow(rowIndex);
		if (row != NULL && IsInteractiveRow(*row))
		{
			return rowIndex;
		}

		rowIndex += direction;
	}

	return -1;
}

void UIScene_SettingsKeyBindMenu::SetSelectedRow(int rowIndex)
{
	const int rowCount = GetRowCount();
	if (rowCount <= 0)
	{
		return;
	}

	if (rowIndex < 0)
	{
		rowIndex = 0;
	}
	else if (rowIndex >= rowCount)
	{
		rowIndex = rowCount - 1;
	}

	const RowEntry* row = GetRow(rowIndex);
	if (row == NULL || !IsInteractiveRow(*row))
	{
		return;
	}

	m_iSelectedRow = rowIndex;

	if (m_iSelectedRow < m_iTopRow)
	{
		m_iTopRow = m_iSelectedRow;
	}
	else if (m_iSelectedRow >= m_iTopRow + BUTTON_COUNT)
	{
		m_iTopRow = m_iSelectedRow - BUTTON_COUNT + 1;
	}

	const int maxTopRow = max(0, rowCount - BUTTON_COUNT);
	if (m_iTopRow > maxTopRow)
	{
		m_iTopRow = maxTopRow;
	}

	UpdateVisibleRows();
	SetFocusToElement(m_iSelectedRow - m_iTopRow);
}

void UIScene_SettingsKeyBindMenu::RefreshVisibleRowWindow()
{
	UpdateVisibleRows();

	const int focusedSlot = m_iSelectedRow - m_iTopRow;
	if (focusedSlot >= 0 && focusedSlot < BUTTON_COUNT)
	{
		SetFocusToElement(focusedSlot);
	}
}

void UIScene_SettingsKeyBindMenu::SaveSettingsNow()
{
	Minecraft *pMinecraft = Minecraft::GetInstance();
	if (pMinecraft != NULL && pMinecraft->options != NULL)
	{
		pMinecraft->options->save();
	}

	app.CheckGameSettingsChanged(true, m_iPad);
}

bool UIScene_SettingsKeyBindMenu::AdjustSettingRow(const RowEntry& row, int delta)
{
	switch (row.type)
	{
	case eRowType_SensitivityInGame:
	case eRowType_SensitivityInMenu:
		{
			eGameSetting gameSetting = (row.type == eRowType_SensitivityInGame) ? eGameSetting_Sensitivity_InGame : eGameSetting_Sensitivity_InMenu;
			int currentValue = app.GetGameSettings(m_iPad, gameSetting);
			int newValue = currentValue + delta;
			if (newValue < 0)
			{
				newValue = 0;
			}
			else if (newValue > 200)
			{
				newValue = 200;
			}

			if (newValue == currentValue)
			{
				return true;
			}

			app.SetGameSettings(m_iPad, gameSetting, newValue);
			RefreshVisibleRowWindow();
			SaveSettingsNow();
			return true;
		}

	case eRowType_Fov:
		{
			Minecraft* pMinecraft = Minecraft::GetInstance();
			int currentPercent = (int)(pMinecraft->options->fov * 100.0f + 0.5f);
			int newPercent = currentPercent + delta;
			if (newPercent < 0)
			{
				newPercent = 0;
			}
			else if (newPercent > 100)
			{
				newPercent = 100;
			}

			if (newPercent == currentPercent)
			{
				return true;
			}

			pMinecraft->options->set(Options::Option::FOV, ((float)newPercent) / 100.0f);
			RefreshVisibleRowWindow();
			SaveSettingsNow();
			return true;
		}

	case eRowType_Gamma:
		{
			int currentValue = app.GetGameSettings(m_iPad, eGameSetting_Gamma);
			int newValue = currentValue + delta;
			if (newValue < 0)
			{
				newValue = 0;
			}
			else if (newValue > 100)
			{
				newValue = 100;
			}

			if (newValue == currentValue)
			{
				return true;
			}

			app.SetGameSettings(m_iPad, eGameSetting_Gamma, (unsigned char)newValue);
			RefreshVisibleRowWindow();
			SaveSettingsNow();
			return true;
		}

	case eRowType_ViewBobbing:
		{
			int currentValue = app.GetGameSettings(m_iPad, eGameSetting_ViewBob);
			int newValue = (delta > 0) ? 1 : 0;
			if (newValue == currentValue)
			{
				return true;
			}

			app.SetGameSettings(m_iPad, eGameSetting_ViewBob, (unsigned char)newValue);
			RefreshVisibleRowWindow();
			SaveSettingsNow();
			return true;
		}

	case eRowType_SmoothCamera:
		{
			Minecraft* pMinecraft = Minecraft::GetInstance();
			bool newValue = (delta > 0);
			if (pMinecraft->options->smoothCamera == newValue)
			{
				return true;
			}

			pMinecraft->options->smoothCamera = newValue;
			RefreshVisibleRowWindow();
			SaveSettingsNow();
			return true;
		}

	case eRowType_InvertLook:
		{
			int currentValue = app.GetGameSettings(m_iPad, eGameSetting_ControlInvertLook);
			int newValue = (delta > 0) ? 1 : 0;
			if (newValue == currentValue)
			{
				return true;
			}

			app.SetGameSettings(m_iPad, eGameSetting_ControlInvertLook, (unsigned char)newValue);
			RefreshVisibleRowWindow();
			SaveSettingsNow();
			return true;
		}

	case eRowType_SprintMode:
		{
			int currentValue = app.GetGameSettings(m_iPad, eGameSetting_SprintMode);
			int newValue = (delta > 0) ? 1 : 0;
			if (newValue == currentValue)
			{
				return true;
			}

			app.SetGameSettings(m_iPad, eGameSetting_SprintMode, (unsigned char)newValue);
			RefreshVisibleRowWindow();
			SaveSettingsNow();
			return true;
		}

	default:
		return false;
	}
}

void UIScene_SettingsKeyBindMenu::ResetControlsToDefaults()
{
	g_KeyBindings.SetDefaults();
	g_KeyBindings.Save("keybinds.dat");
	app.SetGameSettings(m_iPad, eGameSetting_Sensitivity_InGame, 100);
	app.SetGameSettings(m_iPad, eGameSetting_Sensitivity_InMenu, 100);
	app.SetGameSettings(m_iPad, eGameSetting_Gamma, 50);
	app.SetGameSettings(m_iPad, eGameSetting_ViewBob, 1);
	app.SetGameSettings(m_iPad, eGameSetting_ControlInvertLook, 0);
	app.SetGameSettings(m_iPad, eGameSetting_SprintMode, 0);
	Minecraft::GetInstance()->options->set(Options::Option::FOV, 0.0f);
	Minecraft::GetInstance()->options->smoothCamera = false;
	SaveSettingsNow();
}

wstring UIScene_SettingsKeyBindMenu::GetRowLabel(const RowEntry& row) const
{
	WCHAR tempString[256];

	switch (row.type)
	{
	case eRowType_SectionHeader:
		swprintf(tempString, 256, L"[%ls]", row.name);
		return tempString;

	case eRowType_Action:
		{
			wstring label = L"  ";
			label += row.name;
			label += L": [";
			label += g_KeyBindings.GetActionKeyName((EGameAction)row.value);
			label += L"]";
			return label;
		}

	case eRowType_SensitivityInGame:
		swprintf(tempString, 256, L"  %ls %ls %d%%", row.name, GetInlineSliderBar(app.GetGameSettings(m_iPad, eGameSetting_Sensitivity_InGame)).c_str(), app.GetGameSettings(m_iPad, eGameSetting_Sensitivity_InGame));
		return tempString;

	case eRowType_SensitivityInMenu:
		swprintf(tempString, 256, L"  %ls %ls %d%%", row.name, GetInlineSliderBar(app.GetGameSettings(m_iPad, eGameSetting_Sensitivity_InMenu)).c_str(), app.GetGameSettings(m_iPad, eGameSetting_Sensitivity_InMenu));
		return tempString;

	case eRowType_Fov:
		{
			int fovPercent = (int)(Minecraft::GetInstance()->options->fov * 100.0f + 0.5f);
			int fovValue = 70 + ((fovPercent * 40 + 50) / 100);
			swprintf(tempString, 256, L"  %ls %ls %d", row.name, GetInlineSliderBar(fovPercent, 100).c_str(), fovValue);
			return tempString;
		}

	case eRowType_Gamma:
		swprintf(tempString, 256, L"  %ls %ls %d%%", row.name, GetInlineSliderBar(app.GetGameSettings(m_iPad, eGameSetting_Gamma), 100).c_str(), app.GetGameSettings(m_iPad, eGameSetting_Gamma));
		return tempString;

	case eRowType_ViewBobbing:
		swprintf(tempString, 256, L"  %ls: %ls", row.name, app.GetGameSettings(m_iPad, eGameSetting_ViewBob) ? L"On" : L"Off");
		return tempString;

	case eRowType_SmoothCamera:
		swprintf(tempString, 256, L"  %ls: %ls", row.name, Minecraft::GetInstance()->options->smoothCamera ? L"On" : L"Off");
		return tempString;

	case eRowType_InvertLook:
		swprintf(tempString, 256, L"  %ls: %ls", row.name, app.GetGameSettings(m_iPad, eGameSetting_ControlInvertLook) ? L"On" : L"Off");
		return tempString;

	case eRowType_SprintMode:
		swprintf(tempString, 256, L"  %ls: %ls", row.name, app.GetGameSettings(m_iPad, eGameSetting_SprintMode) ? L"Toggle" : L"Hold");
		return tempString;

	case eRowType_ResetDefaults:
		return row.name;

	default:
		return L"";
	}
}

wstring UIScene_SettingsKeyBindMenu::GetInlineSliderBar(int value, int maxValue) const
{
	const int segmentCount = 10;
	int clampedValue = value;
	if (clampedValue < 0)
	{
		clampedValue = 0;
	}
	else if (clampedValue > maxValue)
	{
		clampedValue = maxValue;
	}

	int filledSegments = (clampedValue * segmentCount + (maxValue / 2)) / maxValue;
	if (filledSegments < 0)
	{
		filledSegments = 0;
	}
	else if (filledSegments > segmentCount)
	{
		filledSegments = segmentCount;
	}

	wstring bar = L"[";
	for (int i = 0; i < segmentCount; ++i)
	{
		bar += (i < filledSegments) ? L'=' : L'-';
	}
	bar += L"]";
	return bar;
}

void UIScene_SettingsKeyBindMenu::UpdateVisibleRows()
{
	const int rowCount = GetRowCount();
	for (int slot = 0; slot < BUTTON_COUNT; ++slot)
	{
		const int rowIndex = m_iTopRow + slot;
		const RowEntry* row = GetRow(rowIndex);
		if (row != NULL && rowIndex >= 0 && rowIndex < rowCount)
		{
			m_buttons[slot].init(GetRowLabel(*row), slot);
			m_buttons[slot].setEnable(IsInteractiveRow(*row));
		}
		else
		{
			m_buttons[slot].init(L"", slot);
			m_buttons[slot].setEnable(false);
		}
	}
}

bool UIScene_SettingsKeyBindMenu::ApplyMouseWheelToVisibleSlot(int slot, int wheelDelta)
{
	if (slot < 0 || slot >= BUTTON_COUNT || wheelDelta == 0)
	{
		return false;
	}

	const int rowIndex = m_iTopRow + slot;
	const RowEntry* row = GetRow(rowIndex);
	if (row == NULL || !IsWheelAdjustableRow(*row))
	{
		return false;
	}

	SetSelectedRow(rowIndex);

	const int stepDelta = (wheelDelta > 0) ? 5 : -5;
	for (int step = 0; step < abs(wheelDelta); ++step)
	{
		AdjustSettingRow(*row, stepDelta);
	}

	ui.PlayUISFX(eSFX_Press);
	return true;
}

bool UIScene_SettingsKeyBindMenu::ApplyMousePressToVisibleSlot(int slot)
{
	if (slot < 0 || slot >= BUTTON_COUNT)
	{
		return false;
	}

	const int rowIndex = m_iTopRow + slot;
	const RowEntry* row = GetRow(rowIndex);
	if (row == NULL || !IsInteractiveRow(*row))
	{
		return false;
	}

	SetSelectedRow(rowIndex);
	handlePress((F64)slot, 0.0);
	return true;
}

void UIScene_SettingsKeyBindMenu::SyncMouseHoverToVisibleSlot(int slot)
{
	if (slot < 0 || slot >= BUTTON_COUNT)
	{
		return;
	}

	const int rowIndex = m_iTopRow + slot;
	const RowEntry* row = GetRow(rowIndex);
	if (row == NULL || !IsInteractiveRow(*row))
	{
		return;
	}

	if (m_iSelectedRow != rowIndex)
	{
		SetSelectedRow(rowIndex);
	}
}

void UIScene_SettingsKeyBindMenu::updateTooltips()
{
	if (m_bWaitingForKey)
	{
		ui.SetTooltips(m_iPad, -1, IDS_TOOLTIPS_BACK);
		return;
	}

	ui.SetTooltips(m_iPad, IDS_TOOLTIPS_SELECT, IDS_TOOLTIPS_BACK);
}

void UIScene_SettingsKeyBindMenu::updateComponents()
{
	bool bNotInGame = (Minecraft::GetInstance()->level == NULL);
	if (bNotInGame)
	{
		m_parentLayer->showComponent(m_iPad, eUIComponent_Panorama, true);
		m_parentLayer->showComponent(m_iPad, eUIComponent_Logo, true);
	}
	else
	{
		m_parentLayer->showComponent(m_iPad, eUIComponent_Panorama, false);
		if (app.GetLocalPlayerCount() == 1)
		{
			m_parentLayer->showComponent(m_iPad, eUIComponent_Logo, true);
		}
		else
		{
			m_parentLayer->showComponent(m_iPad, eUIComponent_Logo, false);
		}
	}
}

void UIScene_SettingsKeyBindMenu::tick()
{
	UIScene::tick();

	if (m_bNeedsRefresh)
	{
		m_bNeedsRefresh = false;
		RefreshVisibleRowWindow();
	}

	if (!m_bWaitingForKey)
	{
		return;
	}

	if (m_iRebindDelay > 0)
	{
		--m_iRebindDelay;
		return;
	}

	if (g_KeyboardMouseInput.IsKeyPressed(VK_ESCAPE))
	{
		m_bWaitingForKey = false;
		m_iRebindAction = -1;
		RefreshVisibleRowWindow();
		return;
	}

	for (int vk = 0x08; vk < 0xFF; ++vk)
	{
		if (vk == VK_ESCAPE || !g_KeyboardMouseInput.IsKeyPressed(vk))
		{
			continue;
		}

		if (m_iRebindAction >= 0)
		{
			g_KeyBindings.SetBinding((EGameAction)m_iRebindAction, KeyBind(vk, false));
			g_KeyBindings.Save("keybinds.dat");
			SaveSettingsNow();
		}

		m_bWaitingForKey = false;
		m_iRebindAction = -1;
		RefreshVisibleRowWindow();
		return;
	}

	for (int mb = 0; mb < MAX_MOUSE_BUTTONS; ++mb)
	{
		if (!g_KeyboardMouseInput.IsMouseButtonPressed(mb))
		{
			continue;
		}

		if (m_iRebindAction >= 0)
		{
			g_KeyBindings.SetBinding((EGameAction)m_iRebindAction, KeyBind(mb, true));
			g_KeyBindings.Save("keybinds.dat");
			SaveSettingsNow();
		}

		m_bWaitingForKey = false;
		m_iRebindAction = -1;
		RefreshVisibleRowWindow();
		return;
	}
}

void UIScene_SettingsKeyBindMenu::handleFocusChange(F64 controlId, F64 childId)
{
	int slot = (int)controlId;
	int rowIndex = m_iTopRow + slot;
	const RowEntry* row = GetRow(rowIndex);

	if (row == NULL)
	{
		return;
	}

	if (!IsInteractiveRow(*row))
	{
		SetFocusToElement(m_iSelectedRow - m_iTopRow);
		return;
	}

	m_iSelectedRow = rowIndex;
}

void UIScene_SettingsKeyBindMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	if (m_bWaitingForKey)
	{
		if (key == ACTION_MENU_CANCEL && pressed)
		{
			m_bWaitingForKey = false;
			m_iRebindAction = -1;
			RefreshVisibleRowWindow();
		}

		handled = true;
		return;
	}

	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch (key)
	{
	case ACTION_MENU_CANCEL:
		if (pressed)
		{
			app.CheckGameSettingsChanged(true, iPad);
			navigateBack();
			handled = true;
		}
		break;

	case ACTION_MENU_UP:
		if (pressed || repeat)
		{
			int nextRow = FindNextSelectableRow(m_iSelectedRow - 1, -1);
			if (nextRow >= 0)
			{
				SetSelectedRow(nextRow);
			}
			handled = true;
		}
		break;

	case ACTION_MENU_DOWN:
		if (pressed || repeat)
		{
			int nextRow = FindNextSelectableRow(m_iSelectedRow + 1, 1);
			if (nextRow >= 0)
			{
				SetSelectedRow(nextRow);
			}
			handled = true;
		}
		break;

	case ACTION_MENU_LEFT_SCROLL:
		if (pressed || repeat)
		{
			int nextRow = FindNextSelectableRow(m_iSelectedRow - 1, -1);
			if (nextRow >= 0)
			{
				SetSelectedRow(nextRow);
			}
			handled = true;
		}
		break;

	case ACTION_MENU_RIGHT_SCROLL:
		if (pressed || repeat)
		{
			int nextRow = FindNextSelectableRow(m_iSelectedRow + 1, 1);
			if (nextRow >= 0)
			{
				SetSelectedRow(nextRow);
			}
			handled = true;
		}
		break;

	case ACTION_MENU_LEFT:
	case ACTION_MENU_RIGHT:
		if (pressed || repeat)
		{
			const RowEntry* row = GetRow(m_iSelectedRow);
			if (row != NULL && AdjustSettingRow(*row, (key == ACTION_MENU_RIGHT) ? 5 : -5))
			{
				ui.PlayUISFX(eSFX_Press);
				handled = true;
				break;
			}
		}
		break;

	case ACTION_MENU_OK:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_SettingsKeyBindMenu::handlePress(F64 controlId, F64 childId)
{
	int slot = (int)controlId;
	int rowIndex = m_iTopRow + slot;
	const RowEntry* row = GetRow(rowIndex);
	if (row == NULL)
	{
		return;
	}

	if (!IsInteractiveRow(*row))
	{
		return;
	}

	ui.PlayUISFX(eSFX_Press);
	SetSelectedRow(rowIndex);

	switch (row->type)
	{
	case eRowType_SensitivityInGame:
	case eRowType_SensitivityInMenu:
	case eRowType_Fov:
	case eRowType_Gamma:
		AdjustSettingRow(*row, 5);
		break;

	case eRowType_ViewBobbing:
		AdjustSettingRow(*row, app.GetGameSettings(m_iPad, eGameSetting_ViewBob) ? -1 : 1);
		break;

	case eRowType_SmoothCamera:
		AdjustSettingRow(*row, Minecraft::GetInstance()->options->smoothCamera ? -1 : 1);
		break;

	case eRowType_InvertLook:
		AdjustSettingRow(*row, app.GetGameSettings(m_iPad, eGameSetting_ControlInvertLook) ? -1 : 1);
		break;

	case eRowType_SprintMode:
		AdjustSettingRow(*row, app.GetGameSettings(m_iPad, eGameSetting_SprintMode) ? -1 : 1);
		break;

	case eRowType_ResetDefaults:
		ResetControlsToDefaults();
		m_bNeedsRefresh = true;
		break;

	case eRowType_Action:
		m_bWaitingForKey = true;
		m_iRebindAction = row->value;
		m_iRebindDelay = 3;
		m_buttons[slot].setLabel(L"Press any key...", true, true);
		break;

	default:
		break;
	}
}

#endif
