#pragma once

#include "UIScene.h"
#ifdef _WIN64
#include "UIScene_SettingsKeyBindMenu.h"
#endif

class UIScene_SettingsControlMenu : public UIScene
{
private:
#ifdef _WIN64
	enum EControlsHubButtons
	{
		eControl_MouseCamera = 0,
		eControl_Movement,
		eControl_Gameplay,
		eControl_HotbarInventory,
		eControl_MenuUI,
		eControl_MiscDefaults,
		eControl_Count,
	};

	UIControl_Button m_buttons[eControl_Count];
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_buttons[eControl_MouseCamera], "Button1")
		UI_MAP_ELEMENT( m_buttons[eControl_Movement], "Button2")
		UI_MAP_ELEMENT( m_buttons[eControl_Gameplay], "Button3")
		UI_MAP_ELEMENT( m_buttons[eControl_HotbarInventory], "Button4")
		UI_MAP_ELEMENT( m_buttons[eControl_MenuUI], "Button5")
		UI_MAP_ELEMENT( m_buttons[eControl_MiscDefaults], "Button6")
	UI_END_MAP_ELEMENTS_AND_NAMES()
#else
	enum EControls
	{
		eControl_SensitivityInGame,
		eControl_SensitivityInMenu
	};

	UIControl_Slider m_sliderSensitivityInGame, m_sliderSensitivityInMenu; // Sliders
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_sliderSensitivityInGame, "SensitivityInGame")
		UI_MAP_ELEMENT( m_sliderSensitivityInMenu, "SensitivityInMenu")
	UI_END_MAP_ELEMENTS_AND_NAMES()
#endif
public:
	UIScene_SettingsControlMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_SettingsControlMenu();

	virtual EUIScene getSceneType() { return eUIScene_SettingsControlMenu;}
	
	virtual void updateTooltips();
	virtual void updateComponents();

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
#ifdef _WIN64
protected:
	void handlePress(F64 controlId, F64 childId);
#else
	virtual void handleSliderMove(F64 sliderId, F64 currentValue);
#endif
};
