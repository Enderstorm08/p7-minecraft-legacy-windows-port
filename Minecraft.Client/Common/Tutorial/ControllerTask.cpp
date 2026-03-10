#include "stdafx.h"
#include <string>
#include <unordered_map>
#include "..\..\Minecraft.h"
#include "..\..\MultiplayerLocalPlayer.h"
#include "Tutorial.h"
#include "TutorialConstraints.h"
#include "ControllerTask.h"
#ifdef _WIN64
#include "..\..\Windows64\KeyboardMouseInput.h"
#endif

ControllerTask::ControllerTask(Tutorial *tutorial, int descriptionId, bool enablePreCompletion, bool showMinimumTime,
								int mappings[], unsigned int mappingsLength, int iCompletionMaskA[], int iCompletionMaskACount, int iSouthpawMappings[], unsigned int uiSouthpawMappingsCount)
	: TutorialTask( tutorial, descriptionId, enablePreCompletion, NULL, showMinimumTime )
{
	for(unsigned int i = 0; i < mappingsLength; ++i)
	{
		constraints.push_back( new InputConstraint( mappings[i] ) );
		completedMappings[mappings[i]] = false;
	}
	if(uiSouthpawMappingsCount > 0 ) m_bHasSouthpaw = true;
	for(unsigned int i = 0; i < uiSouthpawMappingsCount; ++i)
	{
		southpawCompletedMappings[iSouthpawMappings[i]] = false;
	}

	m_iCompletionMaskA= new int [iCompletionMaskACount];
	for(int i=0;i<iCompletionMaskACount;i++)
	{
		m_iCompletionMaskA[i]=iCompletionMaskA[i];
	}
	m_iCompletionMaskACount=iCompletionMaskACount;
	m_uiCompletionMask=0;

	// If we don't want to be able to complete it early..then assume we want the constraints active
	//if( !enablePreCompletion )
	//	enableConstraints( true );
}

ControllerTask::~ControllerTask()
{
	delete[] m_iCompletionMaskA;
}

bool ControllerTask::isCompleted()
{
	if( bIsCompleted )
		return true;

	bool bAllComplete = true;
	
	Minecraft *pMinecraft = Minecraft::GetInstance();

	int iCurrent=0;

	if(m_bHasSouthpaw && app.GetGameSettings(pMinecraft->player->GetXboxPad(),eGameSetting_ControlSouthPaw))
	{
		for(AUTO_VAR(it, southpawCompletedMappings.begin()); it != southpawCompletedMappings.end(); ++it)
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
					case MINECRAFT_ACTION_JUMP: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_SPACE); break;
					case MINECRAFT_ACTION_FORWARD: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_W); break;
					case MINECRAFT_ACTION_BACKWARD: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_S); break;
					case MINECRAFT_ACTION_LEFT: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_A); break;
					case MINECRAFT_ACTION_RIGHT: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_D); break;
					case MINECRAFT_ACTION_USE: inputDetected = g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_RIGHT); break;
					case MINECRAFT_ACTION_ACTION: inputDetected = g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_LEFT); break;
					case MINECRAFT_ACTION_INVENTORY: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_E); break;
					case MINECRAFT_ACTION_CRAFTING: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_C); break;
					case MINECRAFT_ACTION_DROP: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_Q); break;
					case MINECRAFT_ACTION_SNEAK_TOGGLE: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_SHIFT); break;
					case MINECRAFT_ACTION_LOOK_LEFT:
					case MINECRAFT_ACTION_LOOK_RIGHT: inputDetected = g_KeyboardMouseInput.GetMouseDeltaX() != 0.0f; break;
					case MINECRAFT_ACTION_LOOK_UP:
					case MINECRAFT_ACTION_LOOK_DOWN: inputDetected = g_KeyboardMouseInput.GetMouseDeltaY() != 0.0f; break;
					}
				}
#endif
				if (inputDetected)
				{
					(*it).second = true;
					m_uiCompletionMask|=1<<iCurrent;
				}
				else
				{
					bAllComplete = false;
				}
			}
			iCurrent++;
		}
	}
	else
	{
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
					case MINECRAFT_ACTION_JUMP: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_SPACE); break;
					case MINECRAFT_ACTION_FORWARD: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_W); break;
					case MINECRAFT_ACTION_BACKWARD: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_S); break;
					case MINECRAFT_ACTION_LEFT: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_A); break;
					case MINECRAFT_ACTION_RIGHT: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_D); break;
					case MINECRAFT_ACTION_USE: inputDetected = g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_RIGHT); break;
					case MINECRAFT_ACTION_ACTION: inputDetected = g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_LEFT); break;
					case MINECRAFT_ACTION_INVENTORY: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_E); break;
					case MINECRAFT_ACTION_CRAFTING: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_C); break;
					case MINECRAFT_ACTION_DROP: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_Q); break;
					case MINECRAFT_ACTION_SNEAK_TOGGLE: inputDetected = g_KeyboardMouseInput.IsKeyDown(KB_KEY_SHIFT); break;
					case MINECRAFT_ACTION_LOOK_LEFT:
					case MINECRAFT_ACTION_LOOK_RIGHT: inputDetected = g_KeyboardMouseInput.GetMouseDeltaX() != 0.0f; break;
					case MINECRAFT_ACTION_LOOK_UP:
					case MINECRAFT_ACTION_LOOK_DOWN: inputDetected = g_KeyboardMouseInput.GetMouseDeltaY() != 0.0f; break;
					}
				}
#endif
				if (inputDetected)
				{
					(*it).second = true;
					m_uiCompletionMask|=1<<iCurrent;
				}
				else
				{
					bAllComplete = false;
				}
			}
			iCurrent++;
		}
	}

	// If this has a list of completion masks then check if there is a matching one to mark the task as complete
	if(m_iCompletionMaskA && CompletionMaskIsValid())
	{
		bIsCompleted = true;
	}
	else
	{
		bIsCompleted = bAllComplete;
	}

	return bIsCompleted;
}

bool ControllerTask::CompletionMaskIsValid()
{
	for(int i=0;i<m_iCompletionMaskACount;i++)
	{
		if(m_uiCompletionMask==m_iCompletionMaskA[i]) return true;
	}

	return false;
}
void ControllerTask::setAsCurrentTask(bool active /*= true*/)
{
	TutorialTask::setAsCurrentTask(active);
	enableConstraints(!active);
}