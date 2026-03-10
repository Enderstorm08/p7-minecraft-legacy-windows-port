#include "stdafx.h"
#include "UI.h"
#include "UIScene_SettingsControlMenu.h"

UIScene_SettingsControlMenu::UIScene_SettingsControlMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

#ifdef _WIN64
	m_buttons[eControl_MouseCamera].init(L"Mouse & Camera", eControl_MouseCamera);
	m_buttons[eControl_Movement].init(L"Movement", eControl_Movement);
	m_buttons[eControl_Gameplay].init(L"Gameplay", eControl_Gameplay);
	m_buttons[eControl_HotbarInventory].init(L"Hotbar / Inventory", eControl_HotbarInventory);
	m_buttons[eControl_MenuUI].init(L"Menu / UI", eControl_MenuUI);
	m_buttons[eControl_MiscDefaults].init(L"Misc & Defaults", eControl_MiscDefaults);
#else
	WCHAR TempString[256];
	swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_SENSITIVITY_INGAME ),app.GetGameSettings(m_iPad,eGameSetting_Sensitivity_InGame));	
	m_sliderSensitivityInGame.init(TempString,eControl_SensitivityInGame,0,200,app.GetGameSettings(m_iPad,eGameSetting_Sensitivity_InGame));
	
	swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_SENSITIVITY_INMENU ),app.GetGameSettings(m_iPad,eGameSetting_Sensitivity_InMenu));	
	m_sliderSensitivityInMenu.init(TempString,eControl_SensitivityInMenu,0,200,app.GetGameSettings(m_iPad,eGameSetting_Sensitivity_InMenu));
#endif

	doHorizontalResizeCheck();

	if(app.GetLocalPlayerCount()>1)
	{
#if TO_BE_IMPLEMENTED
		app.AdjustSplitscreenScene(m_hObj,&m_OriginalPosition,m_iPad,false);
#endif
	}
}

UIScene_SettingsControlMenu::~UIScene_SettingsControlMenu()
{
}

wstring UIScene_SettingsControlMenu::getMoviePath()
{
#ifdef _WIN64
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"SettingsMenuSplit";
	}
	else
	{
		return L"SettingsMenu";
	}
#else
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"SettingsControlMenuSplit";
	}
	else
	{
		return L"SettingsControlMenu";
	}
#endif
}

void UIScene_SettingsControlMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_SettingsControlMenu::updateComponents()
{
	bool bNotInGame=(Minecraft::GetInstance()->level==NULL);
	if(bNotInGame)
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,true);
		m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
	}
	else
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,false);

		if( app.GetLocalPlayerCount() == 1 ) m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
		else m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);
	}
}

void UIScene_SettingsControlMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	ui.AnimateKeyPress(iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			navigateBack();
			handled = true;
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
		sendInputToMovie(key, repeat, pressed, released);
		break;
#ifdef _WIN64
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
#else
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_LEFT:
	case ACTION_MENU_RIGHT:
#endif
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

#ifdef _WIN64
void UIScene_SettingsControlMenu::handlePress(F64 controlId, F64 childId)
{
	ui.PlayUISFX(eSFX_Press);

	UIScene_SettingsKeyBindMenu::EControlsSection section = UIScene_SettingsKeyBindMenu::eSection_MouseCamera;
	switch ((int)controlId)
	{
	case eControl_MouseCamera:
		section = UIScene_SettingsKeyBindMenu::eSection_MouseCamera;
		break;
	case eControl_Movement:
		section = UIScene_SettingsKeyBindMenu::eSection_Movement;
		break;
	case eControl_Gameplay:
		section = UIScene_SettingsKeyBindMenu::eSection_Gameplay;
		break;
	case eControl_HotbarInventory:
		section = UIScene_SettingsKeyBindMenu::eSection_HotbarInventory;
		break;
	case eControl_MenuUI:
		section = UIScene_SettingsKeyBindMenu::eSection_MenuUI;
		break;
	case eControl_MiscDefaults:
		section = UIScene_SettingsKeyBindMenu::eSection_MiscDefaults;
		break;
	default:
		return;
	}

	ui.NavigateToScene(m_iPad, eUIScene_SettingsKeyBindMenu, (void *)(size_t)section);
}
#else
void UIScene_SettingsControlMenu::handleSliderMove(F64 sliderId, F64 currentValue)
{
	WCHAR TempString[256];
	int value = (int)currentValue;
	switch((int)sliderId)
	{
	case eControl_SensitivityInGame:
		m_sliderSensitivityInGame.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_Sensitivity_InGame,value);
		swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_SENSITIVITY_INGAME ),value);	
		m_sliderSensitivityInGame.setLabel(TempString);

		break;
	case eControl_SensitivityInMenu:
		m_sliderSensitivityInMenu.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_Sensitivity_InMenu,value);
		swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_SENSITIVITY_INMENU ),value);	
		m_sliderSensitivityInMenu.setLabel(TempString);

		break;
	}
}
#endif
