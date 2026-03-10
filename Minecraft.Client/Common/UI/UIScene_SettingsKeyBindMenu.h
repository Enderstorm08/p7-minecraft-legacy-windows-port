#pragma once

#include "UIScene.h"

#ifdef _WIN64

class UIScene_SettingsKeyBindMenu : public UIScene
{
public:
	enum EControlsSection
	{
		eSection_MouseCamera = 0,
		eSection_Movement,
		eSection_Gameplay,
		eSection_HotbarInventory,
		eSection_MenuUI,
		eSection_MiscDefaults,
		eSection_All,
	};

private:
	static const int BUTTON_COUNT = 6;

	enum ERowType
	{
		eRowType_SectionHeader = 0,
		eRowType_Action,
		eRowType_SensitivityInGame,
		eRowType_SensitivityInMenu,
		eRowType_Fov,
		eRowType_Gamma,
		eRowType_ViewBobbing,
		eRowType_SmoothCamera,
		eRowType_InvertLook,
		eRowType_SprintMode,
		eRowType_ResetDefaults,
	};

	struct RowEntry
	{
		ERowType type;
		int value;
		const wchar_t* name;
	};

	UIControl_Button m_buttons[BUTTON_COUNT];
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_buttons[0], "Button1")
		UI_MAP_ELEMENT( m_buttons[1], "Button2")
		UI_MAP_ELEMENT( m_buttons[2], "Button3")
		UI_MAP_ELEMENT( m_buttons[3], "Button4")
		UI_MAP_ELEMENT( m_buttons[4], "Button5")
		UI_MAP_ELEMENT( m_buttons[5], "Button6")
	UI_END_MAP_ELEMENTS_AND_NAMES()

	bool m_bWaitingForKey;
	int m_iRebindAction;   // EGameAction being rebound
	int m_iRebindDelay;    // Frame delay to avoid capturing the activation key
	int m_iSelectedRow;
	int m_iTopRow;
	bool m_bNeedsRefresh;
	EControlsSection m_eSection;

	static RowEntry s_rows[];

	int GetRowCount() const;
	const RowEntry* GetRow(int rowIndex) const;
	bool IsInteractiveRow(const RowEntry& row) const;
	bool IsWheelAdjustableRow(const RowEntry& row) const;
	bool RowBelongsToSection(const RowEntry& row) const;
	int FindNextSelectableRow(int startRow, int direction) const;
	void SetSelectedRow(int rowIndex);
	bool AdjustSettingRow(const RowEntry& row, int delta);
	void ResetControlsToDefaults();
	void UpdateVisibleRows();
	void RefreshVisibleRowWindow();
	void SaveSettingsNow();
	wstring GetInlineSliderBar(int value, int maxValue = 200) const;
	wstring GetRowLabel(const RowEntry& row) const;

public:
	UIScene_SettingsKeyBindMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_SettingsKeyBindMenu();

	virtual EUIScene getSceneType() { return eUIScene_SettingsKeyBindMenu; }

	virtual void updateTooltips();
	virtual void updateComponents();
	virtual void tick();
	virtual void handleFocusChange(F64 controlId, F64 childId);
	bool ApplyMouseWheelToVisibleSlot(int slot, int wheelDelta);
	bool ApplyMousePressToVisibleSlot(int slot);
	void SyncMouseHoverToVisibleSlot(int slot);

protected:
	virtual wstring getMoviePath();

public:
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

protected:
	void handlePress(F64 controlId, F64 childId);
};

#endif
