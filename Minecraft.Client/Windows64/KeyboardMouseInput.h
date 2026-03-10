// TODO: leaving this to future me to expand on

// future me here; fuck you

#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <deque>

#define KB_KEY_A          0x41
#define KB_KEY_B          0x42
#define KB_KEY_C          0x43
#define KB_KEY_D          0x44
#define KB_KEY_E          0x45
#define KB_KEY_F          0x46
#define KB_KEY_G          0x47
#define KB_KEY_H          0x48
#define KB_KEY_I          0x49
#define KB_KEY_J          0x4A
#define KB_KEY_K          0x4B
#define KB_KEY_L          0x4C
#define KB_KEY_M          0x4D
#define KB_KEY_N          0x4E
#define KB_KEY_O          0x4F
#define KB_KEY_P          0x50
#define KB_KEY_Q          0x51
#define KB_KEY_R          0x52
#define KB_KEY_S          0x53
#define KB_KEY_T          0x54
#define KB_KEY_U          0x55
#define KB_KEY_V          0x56
#define KB_KEY_W          0x57
#define KB_KEY_X          0x58
#define KB_KEY_Y          0x59
#define KB_KEY_Z          0x5A
#define KB_KEY_0          0x30
#define KB_KEY_1          0x31
#define KB_KEY_2          0x32
#define KB_KEY_3          0x33
#define KB_KEY_4          0x34
#define KB_KEY_5          0x35
#define KB_KEY_6          0x36
#define KB_KEY_7          0x37
#define KB_KEY_8          0x38
#define KB_KEY_9          0x39
#define KB_KEY_F1         VK_F1
#define KB_KEY_F2         VK_F2
#define KB_KEY_F3         VK_F3
#define KB_KEY_F4         VK_F4
#define KB_KEY_F5         VK_F5
#define KB_KEY_F6         VK_F6
#define KB_KEY_F7         VK_F7
#define KB_KEY_F8         VK_F8
#define KB_KEY_F9         VK_F9
#define KB_KEY_F10        VK_F10
#define KB_KEY_F11        VK_F11
#define KB_KEY_F12        VK_F12
#define KB_KEY_SPACE      VK_SPACE
#define KB_KEY_SHIFT      VK_SHIFT
#define KB_KEY_CTRL       VK_CONTROL
#define KB_KEY_ALT        VK_MENU
#define KB_KEY_ESCAPE     VK_ESCAPE
#define KB_KEY_ENTER      VK_RETURN
#define KB_KEY_TAB        VK_TAB
#define KB_KEY_BACKSPACE  VK_BACK
#define KB_KEY_DELETE     VK_DELETE
#define KB_KEY_INSERT     VK_INSERT
#define KB_KEY_HOME       VK_HOME
#define KB_KEY_END        VK_END
#define KB_KEY_PAGEUP     VK_PRIOR
#define KB_KEY_PAGEDOWN   VK_NEXT
#define KB_KEY_UP         VK_UP
#define KB_KEY_DOWN       VK_DOWN
#define KB_KEY_LEFT       VK_LEFT
#define KB_KEY_RIGHT      VK_RIGHT
#define KB_KEY_CAPSLOCK   VK_CAPITAL
#define KB_KEY_TILDE      VK_OEM_3
#define KB_KEY_MINUS      VK_OEM_MINUS
#define KB_KEY_EQUALS     VK_OEM_PLUS
#define KB_KEY_LBRACKET   VK_OEM_4
#define KB_KEY_RBRACKET   VK_OEM_6
#define KB_KEY_SEMICOLON  VK_OEM_1
#define KB_KEY_QUOTE      VK_OEM_7
#define KB_KEY_COMMA      VK_OEM_COMMA
#define KB_KEY_PERIOD     VK_OEM_PERIOD
#define KB_KEY_SLASH      VK_OEM_2
#define KB_KEY_BACKSLASH  VK_OEM_5

#define MOUSE_BUTTON_LEFT   0
#define MOUSE_BUTTON_RIGHT  1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_4      3
#define MOUSE_BUTTON_5      4
#define MAX_MOUSE_BUTTONS   5

class KeyboardMouseInput
{
public:
	KeyboardMouseInput();
	~KeyboardMouseInput();

	void Initialize(HWND hWnd);
	void Tick();

	bool ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

	bool IsKeyDown(int vkCode) const;
	bool IsKeyPressed(int vkCode) const;
	bool IsKeyReleased(int vkCode) const;

	bool IsMouseButtonDown(int button) const;
	bool IsMouseButtonPressed(int button) const;
	bool IsMouseButtonReleased(int button) const;
	bool PopCharacter(wchar_t &ch);
	bool PopScreenKey(int &vkCode);

	int GetMouseX() const { return m_mouseX; }
	int GetMouseY() const { return m_mouseY; }

	int GetWindowWidth() const { return m_windowWidth; }
	int GetWindowHeight() const { return m_windowHeight; }

	float GetMouseDeltaX() const { return m_mouseDeltaX; }
	float GetMouseDeltaY() const { return m_mouseDeltaY; }

	int GetMouseWheelDelta() const { return m_mouseWheelDelta; }

	void SetMouseCaptured(bool captured);
	bool IsMouseCaptured() const { return m_mouseCaptured; }

	void SetEnabled(bool enabled) { m_enabled = enabled; }
	bool IsEnabled() const { return m_enabled; }

	bool HasInputThisFrame() const { return m_hasInputThisFrame; }

	enum EInputDevice { eInputDevice_Controller, eInputDevice_Keyboard };
	EInputDevice GetLastInputDevice() const { return m_lastInputDevice; }
	void SetLastInputDevice(EInputDevice device) { m_lastInputDevice = device; }

	void EndFrame();
	float GetMovementX() const; // A/D keys -> -1/+1
	float GetMovementY() const; // W/S keys -> -1/+1

	void SetMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
	float GetMouseSensitivity() const { return m_mouseSensitivity; }

private:
	static const int MENU_MOUSE_MOVE_THRESHOLD = 1;
	void ResetFrameState();
	void ResetMouseDeltas();
	void ApplyMouseDeltaSmoothing(float rawDeltaX, float rawDeltaY, float smoothing, float maxDelta);
	bool ShouldQueueScreenKey(int vkCode) const;
	void UpdateKeyStateFromMessage(int vkCode, bool isDown);
	void UpdateMouseButtonStateFromMessage(int button, bool isDown);
	void CenterMouse();
	HWND m_hWnd;
	bool m_enabled;
	bool m_initialized;
	bool m_hasInputThisFrame;
	static const int MAX_KEYS = 256;
	bool m_keyState[MAX_KEYS];
	bool m_keyStatePrev[MAX_KEYS];
	bool m_keyJustPressed[MAX_KEYS];
	bool m_keyJustReleased[MAX_KEYS];
	bool m_mouseButtons[MAX_MOUSE_BUTTONS];
	bool m_mouseButtonsPrev[MAX_MOUSE_BUTTONS];
	bool m_mouseJustPressed[MAX_MOUSE_BUTTONS];
	bool m_mouseJustReleased[MAX_MOUSE_BUTTONS];
	int m_mouseX;
	int m_mouseY;
	int m_mousePrevX;
	int m_mousePrevY;
	float m_mouseDeltaX;
	float m_mouseDeltaY;
	float m_smoothedMouseDeltaX;
	float m_smoothedMouseDeltaY;
	int m_mouseWheelDelta;
	bool m_mouseCaptured;
	int m_windowCenterX;
	int m_windowCenterY;
	int m_windowWidth;
	int m_windowHeight;
	float m_mouseSensitivity;
	std::wstring m_charInput;
	std::deque<int> m_screenKeyEvents;
	EInputDevice m_lastInputDevice;
};
extern KeyboardMouseInput g_KeyboardMouseInput;
