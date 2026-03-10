#include "stdafx.h"
#include <string>
#include <unordered_map>
#include "..\..\Minecraft.h"
#include "..\..\MultiplayerLocalPlayer.h"
#include "Tutorial.h"
#include "TutorialConstraints.h"
#include "InfoTask.h"
#include "..\..\..\Minecraft.World\Material.h"
#ifdef _WIN64
#include "..\..\Windows64\KeyboardMouseInput.h"
#endif

InfoTask::InfoTask(Tutorial *tutorial, int descriptionId, int promptId /*= -1*/, bool requiresUserInput /*= false*/,
	int iMapping /*= 0*/, ETelemetryChallenges telemetryEvent /*= eTelemetryTutorial_NoEvent*/)
	: TutorialTask( tutorial, descriptionId, false, NULL, true, false, false )
{
	if(requiresUserInput == true)
	{
		constraints.push_back( new InputConstraint( iMapping ) );
	}
	completedMappings[iMapping]=false;

	m_promptId = promptId;
	tutorial->addMessage( m_promptId );

	m_eTelemetryEvent = telemetryEvent;
}

bool InfoTask::isCompleted()
{
	if( bIsCompleted )
		return true;

	if( tutorial->m_hintDisplayed )
		return false;

	if( !bHasBeenActivated || !m_bShownForMinimumTime )
		return false;

	bool bAllComplete = true;
	
	Minecraft *pMinecraft = Minecraft::GetInstance();

	// If the player is under water then allow all keypresses so they can jump out
	if( pMinecraft->localplayers[tutorial->getPad()]->isUnderLiquid(Material::water) ) return false;

	if(ui.GetMenuDisplayed(tutorial->getPad()))
	{
		// If a menu is displayed, then we use the handleUIInput to complete the task
		bAllComplete = true;
		for(AUTO_VAR(it, completedMappings.begin()); it != completedMappings.end(); ++it)
		{
			bool current = (*it).second;
			if(!current)
			{
				bAllComplete = false;
				break;
			}
		}
	}
	else
	{
		int iCurrent=0;

		for(AUTO_VAR(it, completedMappings.begin()); it != completedMappings.end(); ++it)
		{
			bool current = (*it).second;
			if(!current)
			{
				bool inputDetected = InputManager.GetValue(pMinecraft->player->GetXboxPad(), (*it).first) > 0;
#ifdef _WIN64
				if (!inputDetected && g_KeyboardMouseInput.IsEnabled())
				{
					int mapping = (*it).first;
					switch (mapping)
					{
					case ACTION_MENU_A:
					case ACTION_MENU_OK:
						inputDetected = g_KeyboardMouseInput.IsKeyPressed(KB_KEY_ENTER);
						break;
					case ACTION_MENU_B:
					case ACTION_MENU_CANCEL:
						inputDetected = g_KeyboardMouseInput.IsKeyPressed(KB_KEY_ESCAPE);
						break;
					case MINECRAFT_ACTION_JUMP:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_SPACE);
						break;
					case MINECRAFT_ACTION_FORWARD:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_W);
						break;
					case MINECRAFT_ACTION_BACKWARD:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_S);
						break;
					case MINECRAFT_ACTION_LEFT:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_A);
						break;
					case MINECRAFT_ACTION_RIGHT:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_D);
						break;
					case MINECRAFT_ACTION_USE:
						inputDetected = g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
						break;
					case MINECRAFT_ACTION_ACTION:
						inputDetected = g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_LEFT);
						break;
					case MINECRAFT_ACTION_INVENTORY:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_E);
						break;
					case MINECRAFT_ACTION_CRAFTING:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_C);
						break;
					case MINECRAFT_ACTION_DROP:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_Q);
						break;
					case MINECRAFT_ACTION_SNEAK_TOGGLE:
						inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_SHIFT);
						break;
					case MINECRAFT_ACTION_LOOK_LEFT:
					case MINECRAFT_ACTION_LOOK_RIGHT:
						inputDetected = g_KeyboardMouseInput.GetMouseDeltaX() != 0.0f;
						break;
					case MINECRAFT_ACTION_LOOK_UP:
					case MINECRAFT_ACTION_LOOK_DOWN:
						inputDetected = g_KeyboardMouseInput.GetMouseDeltaY() != 0.0f;
						break;
					}
				}
#endif
				if (inputDetected)
				{
					(*it).second = true;
					bAllComplete=true;
				}
				else
				{
					bAllComplete = false;
				}
			}
			iCurrent++;
		}
	}

	if(bAllComplete==true)
	{
		sendTelemetry();
		enableConstraints(false, true);
	}
	bIsCompleted = bAllComplete;
	return bAllComplete;
}

int InfoTask::getPromptId()
{
	if( m_bShownForMinimumTime )
		return m_promptId;
	else
		return -1;
}

void InfoTask::setAsCurrentTask(bool active /*= true*/)
{
	enableConstraints( active );
	TutorialTask::setAsCurrentTask(active);
}

void InfoTask::handleUIInput(int iAction)
{
	if(bHasBeenActivated)
	{
		for(AUTO_VAR(it, completedMappings.begin()); it != completedMappings.end(); ++it)
		{
			if( iAction == (*it).first )
			{
				(*it).second = true;
			}
		}
	}
}


void InfoTask::sendTelemetry()
{
	Minecraft *pMinecraft = Minecraft::GetInstance();

	if( m_eTelemetryEvent != eTelemetryChallenges_Unknown )
	{
		bool firstPlay = true;
		// We only store first play for some of the events
		switch(m_eTelemetryEvent)
		{
		case eTelemetryTutorial_Complete:
			firstPlay = !tutorial->getCompleted( eTutorial_Telemetry_Complete );
			tutorial->setCompleted( eTutorial_Telemetry_Complete );
			break;
		};
		TelemetryManager->RecordEnemyKilledOrOvercome(pMinecraft->player->GetXboxPad(), 0, 0, 0, 0, 0, 0, m_eTelemetryEvent);
	}
}
