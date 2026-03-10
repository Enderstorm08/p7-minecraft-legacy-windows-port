#include "stdafx.h"
#include "KeyBindings.h"
#include "KeyboardMouseInput.h"
#include <fstream>

KeyBindings g_KeyBindings;

KeyBindings::KeyBindings()
{
	SetDefaults();
}

void KeyBindings::SetDefaults()
{
	m_bindings[eGameAction_MoveForward]       = KeyBind(0x57);
	m_bindings[eGameAction_MoveBack]          = KeyBind(0x53);
	m_bindings[eGameAction_MoveLeft]          = KeyBind(0x41);
	m_bindings[eGameAction_MoveRight]         = KeyBind(0x44);
	m_bindings[eGameAction_Jump]              = KeyBind(VK_SPACE);
	m_bindings[eGameAction_Sneak]             = KeyBind(VK_SHIFT);
	m_bindings[eGameAction_Sprint]            = KeyBind(VK_CONTROL);
	m_bindings[eGameAction_Attack]            = KeyBind(MOUSE_BUTTON_LEFT, true);
	m_bindings[eGameAction_UseItem]           = KeyBind(MOUSE_BUTTON_RIGHT, true);
	m_bindings[eGameAction_PickBlock]         = KeyBind(MOUSE_BUTTON_MIDDLE, true);

	m_bindings[eGameAction_Inventory]         = KeyBind(0x45);
	m_bindings[eGameAction_Crafting]          = KeyBind(0x43);
	m_bindings[eGameAction_Drop]              = KeyBind(0x51);
	m_bindings[eGameAction_Chat]              = KeyBind(0x54);
	m_bindings[eGameAction_Command]           = KeyBind(VK_OEM_2);
	m_bindings[eGameAction_PauseMenu]         = KeyBind(VK_ESCAPE);

	m_bindings[eGameAction_Hotbar1]           = KeyBind(0x31);
	m_bindings[eGameAction_Hotbar2]           = KeyBind(0x32);
	m_bindings[eGameAction_Hotbar3]           = KeyBind(0x33);
	m_bindings[eGameAction_Hotbar4]           = KeyBind(0x34);
	m_bindings[eGameAction_Hotbar5]           = KeyBind(0x35);
	m_bindings[eGameAction_Hotbar6]           = KeyBind(0x36);
	m_bindings[eGameAction_Hotbar7]           = KeyBind(0x37);
	m_bindings[eGameAction_Hotbar8]           = KeyBind(0x38);
	m_bindings[eGameAction_Hotbar9]           = KeyBind(0x39);

	m_bindings[eGameAction_HotbarScrollUp]    = KeyBind(0);
	m_bindings[eGameAction_HotbarScrollDown]  = KeyBind(0);

	m_bindings[eGameAction_ToggleThirdPerson] = KeyBind(VK_F5);
	m_bindings[eGameAction_ToggleDebug]       = KeyBind(VK_F3);
	m_bindings[eGameAction_TogglePlayerList]  = KeyBind(VK_TAB);
	m_bindings[eGameAction_ToggleHUD]         = KeyBind(VK_F1);
	m_bindings[eGameAction_ToggleFullscreen]  = KeyBind(VK_F11);
	m_bindings[eGameAction_Screenshot]        = KeyBind(VK_F2);
	m_bindings[eGameAction_ToggleFly]         = KeyBind(VK_F10);
	m_bindings[eGameAction_ToggleFog]         = KeyBind(VK_F6);
	m_bindings[eGameAction_SmoothCamera]      = KeyBind(VK_F8);
	m_bindings[eGameAction_ChangeSkin]        = KeyBind(VK_F9);
	m_bindings[eGameAction_SpawnCreeper]      = KeyBind(VK_F7);
	m_bindings[eGameAction_ZoomIn]            = KeyBind(VK_OEM_PLUS);
	m_bindings[eGameAction_ZoomOut]           = KeyBind(VK_OEM_MINUS);

	m_bindings[eGameAction_MenuUp]            = KeyBind(VK_UP);
	m_bindings[eGameAction_MenuDown]          = KeyBind(VK_DOWN);
	m_bindings[eGameAction_MenuLeft]          = KeyBind(VK_LEFT);
	m_bindings[eGameAction_MenuRight]         = KeyBind(VK_RIGHT);
	m_bindings[eGameAction_MenuOK]            = KeyBind(VK_RETURN);
	m_bindings[eGameAction_MenuCancel]        = KeyBind(VK_ESCAPE);
	m_bindings[eGameAction_MenuAlt]           = KeyBind(MOUSE_BUTTON_MIDDLE, true);
	m_bindings[eGameAction_MenuQuickMove]     = KeyBind(VK_TAB);
	m_bindings[eGameAction_MenuTabLeft]       = KeyBind(VK_OEM_4);
	m_bindings[eGameAction_MenuTabRight]      = KeyBind(VK_OEM_6);
	m_bindings[eGameAction_MenuPause]         = KeyBind(VK_ESCAPE);
	m_bindings[eGameAction_MenuPageUp]        = KeyBind(VK_PRIOR);
	m_bindings[eGameAction_MenuPageDown]      = KeyBind(VK_NEXT);
	m_bindings[eGameAction_MenuDelete]        = KeyBind(VK_DELETE);
}

KeyBind KeyBindings::GetBinding(EGameAction action) const
{
	if (action >= 0 && action < eGameAction_Count)
		return m_bindings[action];
	return KeyBind();
}

void KeyBindings::SetBinding(EGameAction action, const KeyBind& bind)
{
	if (action >= 0 && action < eGameAction_Count)
		m_bindings[action] = bind;
}

const wchar_t* KeyBindings::GetKeyName(const KeyBind& bind)
{
	if (bind.isMouse)
	{
		switch (bind.vkCode)
		{
		case MOUSE_BUTTON_LEFT:   return L"LMB";
		case MOUSE_BUTTON_RIGHT:  return L"RMB";
		case MOUSE_BUTTON_MIDDLE: return L"MMB";
		case MOUSE_BUTTON_4:      return L"Mouse4";
		case MOUSE_BUTTON_5:      return L"Mouse5";
		default:                  return L"Mouse";
		}
	}
	switch (bind.vkCode)
	{
	case VK_RETURN:  return L"Enter";
	case VK_ESCAPE:  return L"Esc";
	case VK_SPACE:   return L"Space";
	case VK_TAB:     return L"Tab";
	case VK_SHIFT:   return L"Shift";
	case VK_CONTROL: return L"Ctrl";
	case VK_MENU:    return L"Alt";
	case VK_UP:      return L"Up";
	case VK_DOWN:    return L"Down";
	case VK_LEFT:    return L"Left";
	case VK_RIGHT:   return L"Right";
	case VK_BACK:    return L"Backspace";
	case VK_DELETE:  return L"Delete";
	case VK_INSERT:  return L"Insert";
	case VK_HOME:    return L"Home";
	case VK_END:     return L"End";
	case VK_PRIOR:   return L"PgUp";
	case VK_NEXT:    return L"PgDn";
	case VK_F1:      return L"F1";
	case VK_F2:      return L"F2";
	case VK_F3:      return L"F3";
	case VK_F4:      return L"F4";
	case VK_F5:      return L"F5";
	case VK_F6:      return L"F6";
	case VK_F7:      return L"F7";
	case VK_F8:      return L"F8";
	case VK_F9:      return L"F9";
	case VK_F10:     return L"F10";
	case VK_F11:     return L"F11";
	case VK_F12:     return L"F12";
	case 0x30: return L"0";
	case 0x31: return L"1";
	case 0x32: return L"2";
	case 0x33: return L"3";
	case 0x34: return L"4";
	case 0x35: return L"5";
	case 0x36: return L"6";
	case 0x37: return L"7";
	case 0x38: return L"8";
	case 0x39: return L"9";
	case 0x41: return L"A";
	case 0x42: return L"B";
	case 0x43: return L"C";
	case 0x44: return L"D";
	case 0x45: return L"E";
	case 0x46: return L"F";
	case 0x47: return L"G";
	case 0x48: return L"H";
	case 0x49: return L"I";
	case 0x4A: return L"J";
	case 0x4B: return L"K";
	case 0x4C: return L"L";
	case 0x4D: return L"M";
	case 0x4E: return L"N";
	case 0x4F: return L"O";
	case 0x50: return L"P";
	case 0x51: return L"Q";
	case 0x52: return L"R";
	case 0x53: return L"S";
	case 0x54: return L"T";
	case 0x55: return L"U";
	case 0x56: return L"V";
	case 0x57: return L"W";
	case 0x58: return L"X";
	case 0x59: return L"Y";
	case 0x5A: return L"Z";
	case VK_OEM_1:     return L";";
	case VK_OEM_PLUS:  return L"=";
	case VK_OEM_COMMA: return L",";
	case VK_OEM_MINUS: return L"-";
	case VK_OEM_PERIOD:return L".";
	case VK_OEM_2:     return L"/";
	case VK_OEM_3:     return L"`";
	case VK_OEM_4:     return L"[";
	case VK_OEM_5:     return L"\\";
	case VK_OEM_6:     return L"]";
	case VK_OEM_7:     return L"'";
	default:           return L"?";
	}
}

const wchar_t* KeyBindings::GetActionKeyName(EGameAction action) const
{
	return GetKeyName(GetBinding(action));
}

std::wstring KeyBindings::GetTooltipLabel(EGameAction action) const
{
	std::wstring label = L"[";
	label += GetActionKeyName(action);
	label += L"]";
	return label;
}

void KeyBindings::Save(const char* filename)
{
	std::ofstream file(filename, std::ios::binary);
	if (file.is_open())
	{
		for (int i = 0; i < eGameAction_Count; i++)
		{
			file.write((const char*)&m_bindings[i], sizeof(KeyBind));
		}
	}
}

void KeyBindings::Load(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (file.is_open())
	{
		for (int i = 0; i < eGameAction_Count; i++)
		{
			file.read((char*)&m_bindings[i], sizeof(KeyBind));
		}
	}
}
