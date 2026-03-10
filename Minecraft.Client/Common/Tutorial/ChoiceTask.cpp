#include "stdafx.h"
#include <string>
#include <unordered_map>
#include "..\..\Minecraft.h"
#include "..\..\MultiplayerLocalPlayer.h"
#include "Tutorial.h"
#include "TutorialConstraints.h"
#include "ChoiceTask.h"
#include "..\..\..\Minecraft.World\Material.h"
#ifdef _WIN64
#include "..\..\Windows64\KeyboardMouseInput.h"

static bool CheckKeyboardInput(int mapping)
{
	if (!g_KeyboardMouseInput.IsEnabled()) return false;
	switch (mapping)
	{
	case ACTION_MENU_A:
	case ACTION_MENU_OK: return g_KeyboardMouseInput.IsKeyPressed(KB_KEY_ENTER);
	case ACTION_MENU_B:
	case ACTION_MENU_CANCEL: return g_KeyboardMouseInput.IsKeyPressed(KB_KEY_ESCAPE);
	case MINECRAFT_ACTION_JUMP: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_SPACE);
	case MINECRAFT_ACTION_FORWARD: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_W);
	case MINECRAFT_ACTION_BACKWARD: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_S);
	case MINECRAFT_ACTION_LEFT: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_A);
	case MINECRAFT_ACTION_RIGHT: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_D);
	case MINECRAFT_ACTION_USE: return g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	case MINECRAFT_ACTION_ACTION: return g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	case MINECRAFT_ACTION_INVENTORY: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_E);
	case MINECRAFT_ACTION_CRAFTING: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_C);
	case MINECRAFT_ACTION_DROP: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_Q);
	case MINECRAFT_ACTION_SNEAK_TOGGLE: return g_KeyboardMouseInput.IsKeyDown(KB_KEY_SHIFT);
	case MINECRAFT_ACTION_LOOK_LEFT:
	case MINECRAFT_ACTION_LOOK_RIGHT: return g_KeyboardMouseInput.GetMouseDeltaX() != 0.0f;
	case MINECRAFT_ACTION_LOOK_UP:
	case MINECRAFT_ACTION_LOOK_DOWN: return g_KeyboardMouseInput.GetMouseDeltaY() != 0.0f;
	}
	return false;
}
#endif

ChoiceTask::ChoiceTask(Tutorial *tutorial, int descriptionId, int promptId /*= -1*/, bool requiresUserInput /*= false*/,
	int iConfirmMapping /*= 0*/, int iCancelMapping /*= 0*/,
	eTutorial_CompletionAction cancelAction /*= e_Tutorial_Completion_None*/, ETelemetryChallenges telemetryEvent /*= eTelemetryTutorial_NoEvent*/)
	: TutorialTask( tutorial, descriptionId, false, NULL, true, false, false )
{
	if(requiresUserInput == true)
	{
		constraints.push_back( new InputConstraint( iConfirmMapping ) );
		constraints.push_back( new InputConstraint( iCancelMapping ) );
	}
	m_iConfirmMapping = iConfirmMapping;
	m_iCancelMapping = iCancelMapping;
	m_bConfirmMappingComplete = false;
	m_bCancelMappingComplete = false;

	m_cancelAction = cancelAction;

	m_promptId = promptId;
	tutorial->addMessage( m_promptId );

	m_eTelemetryEvent = telemetryEvent;
}

bool ChoiceTask::isCompleted()
{
	Minecraft *pMinecraft = Minecraft::GetInstance();

	if( m_bConfirmMappingComplete || m_bCancelMappingComplete )
	{
		sendTelemetry();
		enableConstraints(false, true);
		return true;
	}	
	
	if(ui.GetMenuDisplayed(tutorial->getPad()))
	{
		// If a menu is displayed, then we use the handleUIInput to complete the task
	}
	else
	{
		// If the player is under water then allow all keypresses so they can jump out
		if( pMinecraft->localplayers[tutorial->getPad()]->isUnderLiquid(Material::water) ) return false;

		bool confirmInput = InputManager.GetValue(pMinecraft->player->GetXboxPad(), m_iConfirmMapping) > 0;
		bool cancelInput = InputManager.GetValue(pMinecraft->player->GetXboxPad(), m_iCancelMapping) > 0;
#ifdef _WIN64
		if (!confirmInput) confirmInput = CheckKeyboardInput(m_iConfirmMapping);
		if (!cancelInput) cancelInput = CheckKeyboardInput(m_iCancelMapping);
#endif
		if(!m_bConfirmMappingComplete && confirmInput)
		{
			m_bConfirmMappingComplete = true;
		}
		if(!m_bCancelMappingComplete && cancelInput)
		{
			m_bCancelMappingComplete = true;
		}
	}

	if(m_bConfirmMappingComplete || m_bCancelMappingComplete)
	{
		sendTelemetry();
		enableConstraints(false, true);
	}
	return m_bConfirmMappingComplete || m_bCancelMappingComplete;
}

eTutorial_CompletionAction ChoiceTask::getCompletionAction()
{
	if(m_bCancelMappingComplete)
	{
		return m_cancelAction;
	}
	else
	{
		return e_Tutorial_Completion_None;
	}
}

int ChoiceTask::getPromptId()
{
	if( m_bShownForMinimumTime )
		return m_promptId;
	else
		return -1;
}

void ChoiceTask::setAsCurrentTask(bool active /*= true*/)
{
	enableConstraints( active );
	TutorialTask::setAsCurrentTask(active);
}

void ChoiceTask::handleUIInput(int iAction)
{
	if(bHasBeenActivated && m_bShownForMinimumTime)
	{
		if( iAction == m_iConfirmMapping )
		{
			m_bConfirmMappingComplete = true;
		}
		else if(iAction == m_iCancelMapping )
		{
			m_bCancelMappingComplete = true;
		}
	}
}

void ChoiceTask::sendTelemetry()
{
	Minecraft *pMinecraft = Minecraft::GetInstance();

	if( m_eTelemetryEvent != eTelemetryChallenges_Unknown )
	{
		bool firstPlay = true;
		// We only store first play for some of the events
		switch(m_eTelemetryEvent)
		{
		case eTelemetryTutorial_TrialStart:
			firstPlay = !tutorial->getCompleted( eTutorial_Telemetry_TrialStart );
			tutorial->setCompleted( eTutorial_Telemetry_TrialStart );
			break;
		case eTelemetryTutorial_Halfway:
			firstPlay = !tutorial->getCompleted( eTutorial_Telemetry_Halfway );
			tutorial->setCompleted( eTutorial_Telemetry_Halfway );
			break;
		};

		TelemetryManager->RecordEnemyKilledOrOvercome(pMinecraft->player->GetXboxPad(), 0, 0, 0, 0, 0, 0, m_eTelemetryEvent);
	}
}
