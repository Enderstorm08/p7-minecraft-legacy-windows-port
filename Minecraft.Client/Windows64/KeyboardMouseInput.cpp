#include "stdafx.h"
#include "KeyboardMouseInput.h"
#include "KeyBindings.h"

KeyboardMouseInput g_KeyboardMouseInput;

KeyboardMouseInput::KeyboardMouseInput()
{
	m_hWnd = NULL;
	m_enabled = true;
	m_initialized = false;
	m_hasInputThisFrame = false;

	memset(m_keyState, 0, sizeof(m_keyState));
	memset(m_keyStatePrev, 0, sizeof(m_keyStatePrev));
	memset(m_keyJustPressed, 0, sizeof(m_keyJustPressed));
	memset(m_keyJustReleased, 0, sizeof(m_keyJustReleased));

	memset(m_mouseButtons, 0, sizeof(m_mouseButtons));
	memset(m_mouseButtonsPrev, 0, sizeof(m_mouseButtonsPrev));
	memset(m_mouseJustPressed, 0, sizeof(m_mouseJustPressed));
	memset(m_mouseJustReleased, 0, sizeof(m_mouseJustReleased));
	m_mouseX = 0;
	m_mouseY = 0;
	m_mousePrevX = 0;
	m_mousePrevY = 0;
	m_mouseDeltaX = 0.0f;
	m_mouseDeltaY = 0.0f;
	m_smoothedMouseDeltaX = 0.0f;
	m_smoothedMouseDeltaY = 0.0f;
	m_mouseWheelDelta = 0;

	m_mouseCaptured = false;
	m_windowCenterX = 0;
	m_windowCenterY = 0;
	m_windowWidth = 1920;
	m_windowHeight = 1080;

	m_mouseSensitivity = 1.0f;
	m_charInput.clear();
	m_screenKeyEvents.clear();
	m_lastInputDevice = eInputDevice_Controller;
}

KeyboardMouseInput::~KeyboardMouseInput()
{
	if (m_mouseCaptured)
	{
		SetMouseCaptured(false);
	}
}

void KeyboardMouseInput::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;
	m_initialized = true;
	ResetMouseDeltas();

	RECT rect;
	GetClientRect(m_hWnd, &rect);
	m_windowWidth = rect.right - rect.left;
	m_windowHeight = rect.bottom - rect.top;
	m_windowCenterX = m_windowWidth / 2;
	m_windowCenterY = m_windowHeight / 2;
}

void KeyboardMouseInput::Tick()
{
	if (!m_initialized || !m_enabled)
	{
		return;
	}

	m_hasInputThisFrame = false;

	RECT rect;
	if (GetClientRect(m_hWnd, &rect))
	{
		m_windowWidth = rect.right - rect.left;
		m_windowHeight = rect.bottom - rect.top;
		m_windowCenterX = m_windowWidth / 2;
		m_windowCenterY = m_windowHeight / 2;
	}

	m_mousePrevX = m_mouseX;
	m_mousePrevY = m_mouseY;

	POINT cursorPos;
	if (GetCursorPos(&cursorPos))
	{
		ScreenToClient(m_hWnd, &cursorPos);
		m_mouseX = cursorPos.x;
		m_mouseY = cursorPos.y;
	}

	if (m_mouseCaptured)
	{
		float rawDeltaX = (float)(m_mouseX - m_windowCenterX);
		float rawDeltaY = (float)(m_mouseY - m_windowCenterY);
		m_mouseDeltaX = rawDeltaX;
		m_mouseDeltaY = rawDeltaY;

		if (rawDeltaX != 0.0f || rawDeltaY != 0.0f)
		{
			CenterMouse();
			m_hasInputThisFrame = true;
			m_lastInputDevice = eInputDevice_Keyboard;
		}
	}
	else
	{
		float rawDeltaX = (float)(m_mouseX - m_mousePrevX);
		float rawDeltaY = (float)(m_mouseY - m_mousePrevY);
		if (abs((int)rawDeltaX) <= MENU_MOUSE_MOVE_THRESHOLD)
		{
			rawDeltaX = 0.0f;
		}
		if (abs((int)rawDeltaY) <= MENU_MOUSE_MOVE_THRESHOLD)
		{
			rawDeltaY = 0.0f;
		}

		rawDeltaX *= m_mouseSensitivity;
		rawDeltaY *= m_mouseSensitivity;
		ApplyMouseDeltaSmoothing(rawDeltaX, rawDeltaY, 0.60f, 96.0f);

		if (m_mouseDeltaX != 0.0f || m_mouseDeltaY != 0.0f)
		{
			m_hasInputThisFrame = true;
			m_lastInputDevice = eInputDevice_Keyboard;
		}
	}
}

bool KeyboardMouseInput::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_initialized || !m_enabled)
	{
		return false;
	}

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		UpdateKeyStateFromMessage((int)wParam, true);
		if (ShouldQueueScreenKey((int)wParam))
		{
			m_screenKeyEvents.push_back((int)wParam);
		}
		return true;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		UpdateKeyStateFromMessage((int)wParam, false);
		return true;

	case WM_LBUTTONDOWN:
		UpdateMouseButtonStateFromMessage(MOUSE_BUTTON_LEFT, true);
		return true;

	case WM_LBUTTONUP:
		UpdateMouseButtonStateFromMessage(MOUSE_BUTTON_LEFT, false);
		return true;

	case WM_RBUTTONDOWN:
		UpdateMouseButtonStateFromMessage(MOUSE_BUTTON_RIGHT, true);
		return true;

	case WM_RBUTTONUP:
		UpdateMouseButtonStateFromMessage(MOUSE_BUTTON_RIGHT, false);
		return true;

	case WM_MBUTTONDOWN:
		UpdateMouseButtonStateFromMessage(MOUSE_BUTTON_MIDDLE, true);
		return true;

	case WM_MBUTTONUP:
		UpdateMouseButtonStateFromMessage(MOUSE_BUTTON_MIDDLE, false);
		return true;

	case WM_XBUTTONDOWN:
		{
			int xButton = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MOUSE_BUTTON_4 : MOUSE_BUTTON_5;
			UpdateMouseButtonStateFromMessage(xButton, true);
		}
		return true;

	case WM_XBUTTONUP:
		{
			int xButton = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MOUSE_BUTTON_4 : MOUSE_BUTTON_5;
			UpdateMouseButtonStateFromMessage(xButton, false);
		}
		return true;

	case WM_MOUSEWHEEL:
		m_mouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		m_hasInputThisFrame = true;
		m_lastInputDevice = eInputDevice_Keyboard;
		return true;

	case WM_MOUSEMOVE:
		m_mouseX = GET_X_LPARAM(lParam);
		m_mouseY = GET_Y_LPARAM(lParam);
		m_hasInputThisFrame = true;
		m_lastInputDevice = eInputDevice_Keyboard;
		return true;

	case WM_CHAR:
		if (wParam >= 32 || wParam == VK_RETURN || wParam == VK_BACK || wParam == VK_TAB)
		{
			m_charInput.push_back((wchar_t)wParam);
			m_hasInputThisFrame = true;
			m_lastInputDevice = eInputDevice_Keyboard;
		}
		return true;

	case WM_KILLFOCUS:
		// Window lost focus - release all keys and mouse capture
		memset(m_keyState, 0, sizeof(m_keyState));
		memset(m_mouseButtons, 0, sizeof(m_mouseButtons));
		memset(m_keyJustPressed, 0, sizeof(m_keyJustPressed));
		memset(m_keyJustReleased, 0, sizeof(m_keyJustReleased));
		memset(m_mouseJustPressed, 0, sizeof(m_mouseJustPressed));
		memset(m_mouseJustReleased, 0, sizeof(m_mouseJustReleased));
		m_charInput.clear();
		m_screenKeyEvents.clear();
		ResetMouseDeltas();
		if (m_mouseCaptured)
		{
			SetMouseCaptured(false);
		}
		return true;

	case WM_SETFOCUS:
		{
			RECT rect;
			GetClientRect(m_hWnd, &rect);
			m_windowCenterX = (rect.right - rect.left) / 2;
			m_windowCenterY = (rect.bottom - rect.top) / 2;
			ResetMouseDeltas();
		}
		return true;
	}

	return false;
}

void KeyboardMouseInput::UpdateKeyStateFromMessage(int vkCode, bool isDown)
{
	if (vkCode < 0 || vkCode >= MAX_KEYS)
	{
		return;
	}

	if (isDown)
	{
		if (!m_keyState[vkCode])
		{
			m_keyJustPressed[vkCode] = true;
		}
	}
	else if (m_keyState[vkCode])
	{
		m_keyJustReleased[vkCode] = true;
	}

	m_keyState[vkCode] = isDown;
	if (isDown)
	{
		m_hasInputThisFrame = true;
		m_lastInputDevice = eInputDevice_Keyboard;
	}
}

void KeyboardMouseInput::UpdateMouseButtonStateFromMessage(int button, bool isDown)
{
	if (button < 0 || button >= MAX_MOUSE_BUTTONS)
	{
		return;
	}

	if (isDown)
	{
		if (!m_mouseButtons[button])
		{
			m_mouseJustPressed[button] = true;
		}
	}
	else if (m_mouseButtons[button])
	{
		m_mouseJustReleased[button] = true;
	}

	m_mouseButtons[button] = isDown;
	if (isDown)
	{
		m_hasInputThisFrame = true;
		m_lastInputDevice = eInputDevice_Keyboard;
	}
}

bool KeyboardMouseInput::IsKeyDown(int vkCode) const
{
	if (!m_enabled || vkCode < 0 || vkCode >= MAX_KEYS)
	{
		return false;
	}
	return m_keyState[vkCode];
}

bool KeyboardMouseInput::IsKeyPressed(int vkCode) const
{
	if (!m_enabled || vkCode < 0 || vkCode >= MAX_KEYS)
	{
		return false;
	}
	return m_keyJustPressed[vkCode];
}

bool KeyboardMouseInput::IsKeyReleased(int vkCode) const
{
	if (!m_enabled || vkCode < 0 || vkCode >= MAX_KEYS)
	{
		return false;
	}
	return m_keyJustReleased[vkCode];
}

bool KeyboardMouseInput::IsMouseButtonDown(int button) const
{
	if (!m_enabled || button < 0 || button >= MAX_MOUSE_BUTTONS)
	{
		return false;
	}
	return m_mouseButtons[button];
}

bool KeyboardMouseInput::IsMouseButtonPressed(int button) const
{
	if (!m_enabled || button < 0 || button >= MAX_MOUSE_BUTTONS)
	{
		return false;
	}
	return m_mouseJustPressed[button];
}

bool KeyboardMouseInput::IsMouseButtonReleased(int button) const
{
	if (!m_enabled || button < 0 || button >= MAX_MOUSE_BUTTONS)
	{
		return false;
	}
	return m_mouseJustReleased[button];
}

bool KeyboardMouseInput::PopCharacter(wchar_t &ch)
{
	if (!m_enabled || m_charInput.empty())
	{
		return false;
	}

	ch = m_charInput[0];
	m_charInput.erase(0, 1);
	return true;
}

bool KeyboardMouseInput::PopScreenKey(int &vkCode)
{
	if (!m_enabled || m_screenKeyEvents.empty())
	{
		return false;
	}

	vkCode = m_screenKeyEvents.front();
	m_screenKeyEvents.pop_front();
	return true;
}

void KeyboardMouseInput::SetMouseCaptured(bool captured)
{
	if (captured == m_mouseCaptured)
	{
		return;
	}

	m_mouseCaptured = captured;
	ResetMouseDeltas();

	if (captured)
	{
		ShowCursor(FALSE);
		
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		POINT topLeft = { rect.left, rect.top };
		POINT bottomRight = { rect.right, rect.bottom };
		ClientToScreen(m_hWnd, &topLeft);
		ClientToScreen(m_hWnd, &bottomRight);
		rect.left = topLeft.x;
		rect.top = topLeft.y;
		rect.right = bottomRight.x;
		rect.bottom = bottomRight.y;
		ClipCursor(&rect);

		CenterMouse();
	}
	else
	{
		ShowCursor(TRUE);
		ClipCursor(NULL);
	}
}

void KeyboardMouseInput::CenterMouse()
{
	POINT center = { m_windowCenterX, m_windowCenterY };
	ClientToScreen(m_hWnd, &center);
	SetCursorPos(center.x, center.y);
	m_mouseX = m_windowCenterX;
	m_mouseY = m_windowCenterY;
}

float KeyboardMouseInput::GetMovementX() const
{
	if (!m_enabled)
	{
		return 0.0f;
	}

	float x = 0.0f;
	KeyBind bindLeft = g_KeyBindings.GetBinding(eGameAction_MoveLeft);
	KeyBind bindRight = g_KeyBindings.GetBinding(eGameAction_MoveRight);
	if (!bindLeft.isMouse && IsKeyDown(bindLeft.vkCode))
	{
		x -= 1.0f;
	}
	if (!bindRight.isMouse && IsKeyDown(bindRight.vkCode))
	{
		x += 1.0f;
	}
	return x;
}

float KeyboardMouseInput::GetMovementY() const
{
	if (!m_enabled)
	{
		return 0.0f;
	}

	float y = 0.0f;
	KeyBind bindFwd = g_KeyBindings.GetBinding(eGameAction_MoveForward);
	KeyBind bindBack = g_KeyBindings.GetBinding(eGameAction_MoveBack);
	if (!bindFwd.isMouse && IsKeyDown(bindFwd.vkCode))
	{
		y += 1.0f;
	}
	if (!bindBack.isMouse && IsKeyDown(bindBack.vkCode))
	{
		y -= 1.0f;
	}
	return y;
}

void KeyboardMouseInput::ResetFrameState()
{
	m_hasInputThisFrame = false;
	m_mouseWheelDelta = 0;
}

void KeyboardMouseInput::ResetMouseDeltas()
{
	m_mouseDeltaX = 0.0f;
	m_mouseDeltaY = 0.0f;
	m_smoothedMouseDeltaX = 0.0f;
	m_smoothedMouseDeltaY = 0.0f;
}

bool KeyboardMouseInput::ShouldQueueScreenKey(int vkCode) const
{
	switch (vkCode)
	{
	case VK_ESCAPE:
	case VK_RETURN:
	case VK_BACK:
	case VK_TAB:
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_DELETE:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:
	case VK_NEXT:
		return true;
	default:
		return false;
	}
}

void KeyboardMouseInput::ApplyMouseDeltaSmoothing(float rawDeltaX, float rawDeltaY, float smoothing, float maxDelta)
{
	if (rawDeltaX > maxDelta)
	{
		rawDeltaX = maxDelta;
	}
	else if (rawDeltaX < -maxDelta)
	{
		rawDeltaX = -maxDelta;
	}

	if (rawDeltaY > maxDelta)
	{
		rawDeltaY = maxDelta;
	}
	else if (rawDeltaY < -maxDelta)
	{
		rawDeltaY = -maxDelta;
	}

	m_smoothedMouseDeltaX += (rawDeltaX - m_smoothedMouseDeltaX) * smoothing;
	m_smoothedMouseDeltaY += (rawDeltaY - m_smoothedMouseDeltaY) * smoothing;

	if (fabsf(rawDeltaX) < 0.01f && fabsf(m_smoothedMouseDeltaX) < 0.01f)
	{
		m_smoothedMouseDeltaX = 0.0f;
	}
	if (fabsf(rawDeltaY) < 0.01f && fabsf(m_smoothedMouseDeltaY) < 0.01f)
	{
		m_smoothedMouseDeltaY = 0.0f;
	}

	m_mouseDeltaX = m_smoothedMouseDeltaX;
	m_mouseDeltaY = m_smoothedMouseDeltaY;
}

void KeyboardMouseInput::EndFrame()
{
	memset(m_keyJustPressed, 0, sizeof(m_keyJustPressed));
	memset(m_keyJustReleased, 0, sizeof(m_keyJustReleased));
	memset(m_mouseJustPressed, 0, sizeof(m_mouseJustPressed));
	memset(m_mouseJustReleased, 0, sizeof(m_mouseJustReleased));
	m_mouseWheelDelta = 0;
	m_charInput.clear();
	m_screenKeyEvents.clear();
}
