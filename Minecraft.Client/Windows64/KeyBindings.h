#pragma once

#include <Windows.h>
#include <string>
#include <map>

enum EGameAction
{
	eGameAction_MoveForward = 0,
	eGameAction_MoveBack,
	eGameAction_MoveLeft,
	eGameAction_MoveRight,
	eGameAction_Jump,
	eGameAction_Sneak,
	eGameAction_Sprint,
	eGameAction_Attack,
	eGameAction_UseItem,
	eGameAction_PickBlock,

	eGameAction_Inventory,
	eGameAction_Crafting,
	eGameAction_Drop,
	eGameAction_Chat,
	eGameAction_Command,
	eGameAction_PauseMenu,

	eGameAction_Hotbar1,
	eGameAction_Hotbar2,
	eGameAction_Hotbar3,
	eGameAction_Hotbar4,
	eGameAction_Hotbar5,
	eGameAction_Hotbar6,
	eGameAction_Hotbar7,
	eGameAction_Hotbar8,
	eGameAction_Hotbar9,

	eGameAction_HotbarScrollUp,
	eGameAction_HotbarScrollDown,

	eGameAction_ToggleThirdPerson,
	eGameAction_ToggleDebug,
	eGameAction_TogglePlayerList,
	eGameAction_ToggleHUD,
	eGameAction_ToggleFullscreen,
	eGameAction_Screenshot,
	eGameAction_ToggleFly,
	eGameAction_ToggleFog,
	eGameAction_SmoothCamera,
	eGameAction_ChangeSkin,
	eGameAction_SpawnCreeper,
	eGameAction_ZoomIn,
	eGameAction_ZoomOut,

	eGameAction_MenuUp,
	eGameAction_MenuDown,
	eGameAction_MenuLeft,
	eGameAction_MenuRight,
	eGameAction_MenuOK,
	eGameAction_MenuCancel,
	eGameAction_MenuAlt,
	eGameAction_MenuQuickMove,
	eGameAction_MenuTabLeft,
	eGameAction_MenuTabRight,
	eGameAction_MenuPause,
	eGameAction_MenuPageUp,
	eGameAction_MenuPageDown,
	eGameAction_MenuDelete,

	eGameAction_Count
};

// key binding entry
struct KeyBind
{
	int vkCode;        // Virtual key code or mouse button ID
	bool isMouse;      // true = mouse button, false = keyboard key
	
	KeyBind() : vkCode(0), isMouse(false) {}
	KeyBind(int vk, bool mouse = false) : vkCode(vk), isMouse(mouse) {}
};

class KeyBindings
{
public:
	KeyBindings();

	void SetDefaults();

	KeyBind GetBinding(EGameAction action) const;
	void SetBinding(EGameAction action, const KeyBind& bind);

	static const wchar_t* GetKeyName(const KeyBind& bind);
	const wchar_t* GetActionKeyName(EGameAction action) const;

	std::wstring GetTooltipLabel(EGameAction action) const;

	void Save(const char* filename);
	void Load(const char* filename);

private:
	KeyBind m_bindings[eGameAction_Count];
};

extern KeyBindings g_KeyBindings;
