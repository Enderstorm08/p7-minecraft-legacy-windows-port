#include "stdafx.h"
#include "Minecraft.h"
#include "GameMode.h"
#include "..\Minecraft.World\net.minecraft.world.entity.player.h"
#include "..\Minecraft.World\net.minecraft.world.level.h"
#include "..\Minecraft.World\net.minecraft.world.level.storage.h"
#include "Input.h"
#include "..\Minecraft.Client\LocalPlayer.h"
#include "Options.h"
#include "Common\UI\UI.h"
#ifdef _WINDOWS64
#include "Windows64\KeyboardMouseInput.h"
#include "Windows64\KeyBindings.h"
#endif

#ifdef _WINDOWS64
static bool IsKeyboardBindingActive(EGameAction action)
{
	KeyBind bind = g_KeyBindings.GetBinding(action);
	if(bind.isMouse)
	{
		return g_KeyboardMouseInput.IsMouseButtonDown(bind.vkCode) || g_KeyboardMouseInput.IsMouseButtonPressed(bind.vkCode);
	}
	return g_KeyboardMouseInput.IsKeyDown(bind.vkCode) || g_KeyboardMouseInput.IsKeyPressed(bind.vkCode);
}

static bool HasImmediateKeyboardMouseInput()
{
	return g_KeyboardMouseInput.HasInputThisFrame() ||
		g_KeyboardMouseInput.GetMovementX() != 0.0f ||
		g_KeyboardMouseInput.GetMovementY() != 0.0f ||
		g_KeyboardMouseInput.GetMouseDeltaX() != 0.0f ||
		g_KeyboardMouseInput.GetMouseDeltaY() != 0.0f ||
		g_KeyboardMouseInput.GetMouseWheelDelta() != 0 ||
		IsKeyboardBindingActive(eGameAction_MoveForward) ||
		IsKeyboardBindingActive(eGameAction_MoveBack) ||
		IsKeyboardBindingActive(eGameAction_MoveLeft) ||
		IsKeyboardBindingActive(eGameAction_MoveRight) ||
		IsKeyboardBindingActive(eGameAction_Jump) ||
		IsKeyboardBindingActive(eGameAction_Sneak) ||
		IsKeyboardBindingActive(eGameAction_Attack) ||
		IsKeyboardBindingActive(eGameAction_UseItem) ||
		IsKeyboardBindingActive(eGameAction_Inventory) ||
		IsKeyboardBindingActive(eGameAction_PauseMenu);
}

static bool UseKeyboardMouseInput(int iPad)
{
	Minecraft *minecraft = Minecraft::GetInstance();
	return iPad == 0 &&
		!ui.GetMenuDisplayed(iPad) &&
		(minecraft == NULL || minecraft->screen == NULL) &&
		(g_KeyboardMouseInput.GetLastInputDevice() == KeyboardMouseInput::eInputDevice_Keyboard || HasImmediateKeyboardMouseInput());
}
#endif

Input::Input()
{
	xa = 0;
	ya = 0;
	wasJumping = false;
	jumping = false;
	sneaking = false;

	lReset = false;
    rReset = false;
}

void Input::tick(LocalPlayer *player)
{
	// 4J Stu -  Assume that we only need one input class, even though the java has subclasses for keyboard/controller
	// This function is based on the ControllerInput class in the Java, and will probably need changed
	//OutputDebugString("INPUT: Beginning input tick\n");

	Minecraft *pMinecraft=Minecraft::GetInstance();
	int iPad=player->GetXboxPad();

	// 4J-PB minecraft movement seems to be the wrong way round, so invert x!
	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LEFT) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_RIGHT) )
#ifdef _WINDOWS64
	{
		if(UseKeyboardMouseInput(iPad))
			xa = -g_KeyboardMouseInput.GetMovementX();
		else
			xa = -InputManager.GetJoypadStick_LX(iPad);
	}
#else
		xa = -InputManager.GetJoypadStick_LX(iPad);
#endif
	else
		xa = 0.0f;
	
	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_FORWARD) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_BACKWARD) )
#ifdef _WINDOWS64
	{
		if(UseKeyboardMouseInput(iPad))
			ya = g_KeyboardMouseInput.GetMovementY();
		else
			ya = InputManager.GetJoypadStick_LY(iPad);
	}
#else
		ya = InputManager.GetJoypadStick_LY(iPad);
#endif
	else
		ya = 0.0f;

#ifndef _CONTENT_PACKAGE
	if (app.GetFreezePlayers())
	{
		xa = ya = 0.0f;
		player->abilities.flying = true;
	}
#endif
	
    if (!lReset)
    {
        if (xa*xa+ya*ya==0.0f)
        {
            lReset = true;
        }
        xa = ya = 0.0f;
    }

	// 4J - in flying mode, don't actually toggle sneaking
	if(!player->abilities.flying)
	{
		if((player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_SNEAK_TOGGLE)) && pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_SNEAK_TOGGLE))
		{
			sneaking=!sneaking;
		}
	}

	if(sneaking)
	{
		xa*=0.3f;
		ya*=0.3f;
	}

	float turnSpeed = 50.0f;

	float tx = 0.0f;
	float ty = 0.0f;
	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_LEFT) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_RIGHT) )
#ifdef _WINDOWS64
	{
		if(UseKeyboardMouseInput(iPad))
			tx = g_KeyboardMouseInput.GetMouseDeltaX() * (((float)app.GetGameSettings(iPad,eGameSetting_Sensitivity_InGame))/100.0f);
		else
			tx = InputManager.GetJoypadStick_RX(iPad)*(((float)app.GetGameSettings(iPad,eGameSetting_Sensitivity_InGame))/100.0f);
	}
#else
		tx = InputManager.GetJoypadStick_RX(iPad)*(((float)app.GetGameSettings(iPad,eGameSetting_Sensitivity_InGame))/100.0f); // apply sensitivity to look
#endif
	if( pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_UP) || pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_LOOK_DOWN) )
#ifdef _WINDOWS64
	{
		if(UseKeyboardMouseInput(iPad))
			ty = g_KeyboardMouseInput.GetMouseDeltaY() * (((float)app.GetGameSettings(iPad,eGameSetting_Sensitivity_InGame))/100.0f);
		else
			ty = InputManager.GetJoypadStick_RY(iPad)*(((float)app.GetGameSettings(iPad,eGameSetting_Sensitivity_InGame))/100.0f);
	}
#else
		ty = InputManager.GetJoypadStick_RY(iPad)*(((float)app.GetGameSettings(iPad,eGameSetting_Sensitivity_InGame))/100.0f); // apply sensitivity to look
#endif
	
#ifndef _CONTENT_PACKAGE
	if (app.GetFreezePlayers())	tx = ty = 0.0f;
#endif

	// 4J: WESTY : Invert look Y if required.
	if ( app.GetGameSettings(iPad,eGameSetting_ControlInvertLook) )
	{
		ty = -ty;
	}

    if (!rReset)
    {
        if (tx*tx+ty*ty==0.0f)
        {
            rReset = true;
        }
        tx = ty = 0.0f;
    }
#ifdef _WINDOWS64
	if(UseKeyboardMouseInput(iPad))
	{
		player->interpolateTurn(tx * turnSpeed, ty * turnSpeed);
	}
	else
	{
		player->interpolateTurn(tx * abs(tx) * turnSpeed, ty * abs(ty) * turnSpeed);
	}
#else
	player->interpolateTurn(tx * abs(tx) * turnSpeed, ty * abs(ty) * turnSpeed);
#endif
        
    //jumping = controller.isButtonPressed(0);

	
	unsigned int jump = InputManager.GetValue(iPad, MINECRAFT_ACTION_JUMP);
#ifdef _WINDOWS64
	if(UseKeyboardMouseInput(iPad))
	{
		KeyBind jumpBind = g_KeyBindings.GetBinding(eGameAction_Jump);
		jump = (!jumpBind.isMouse && g_KeyboardMouseInput.IsKeyDown(jumpBind.vkCode)) ? 1 : 0;
	}
#endif
	if( jump > 0 && pMinecraft->localgameModes[iPad]->isInputAllowed(MINECRAFT_ACTION_JUMP) )
		jumping = true;
	else
 		jumping = false;

#ifndef _CONTENT_PACKAGE
	if (app.GetFreezePlayers())	jumping = false;
#endif

	//OutputDebugString("INPUT: End input tick\n");
}
