// FUCK IGGY FUCK FUCK FUCK FUCK FUCK

#include "stdafx.h"
#include "UIController.h"
#include "UI.h"
#include "UIScene.h"
#include "..\..\..\Minecraft.World\StringHelpers.h"
#include "..\..\LocalPlayer.h"
#include "..\..\DLCTexturePack.h"
#include "..\..\TexturePackRepository.h"
#include "..\..\Minecraft.h"
#include "..\..\..\Minecraft.World\net.minecraft.world.entity.boss.enderdragon.h"
#include "..\..\EnderDragonRenderer.h"
#include "..\..\MultiPlayerLocalPlayer.h"
#include "UIFontData.h"

#ifdef _WIN64
#include "..\..\Windows64\KeyboardMouseInput.h"
#include "..\..\Windows64\KeyBindings.h"
#include "UIControl_ButtonList.h"
#include "UIControl_Button.h"
#include "UIControl_CheckBox.h"
#include "UIControl_DynamicLabel.h"
#include "UIControl_HTMLLabel.h"
#include "UIControl_TexturePackList.h"
#include "UIControl_LeaderboardList.h"
#include "UIControl_Slider.h"
#include "UIControl_TextInput.h"
#include "UIScene_SettingsKeyBindMenu.h"
#endif
#ifdef __PSVITA__
#include <message_dialog.h>
#endif

// 4J Stu - Enable this to override the Iggy Allocator
//#define ENABLE_IGGY_ALLOCATOR
//#define EXCLUDE_IGGY_ALLOCATIONS_FROM_HEAP_INSPECTOR

//#define ENABLE_IGGY_EXPLORER
#ifdef ENABLE_IGGY_EXPLORER
#include "Windows64\Iggy\include\iggyexpruntime.h"
#endif

//#define ENABLE_IGGY_PERFMON
#ifdef ENABLE_IGGY_PERFMON

#define PM_ORIGIN_X 24
#define PM_ORIGIN_Y 34

#ifdef __ORBIS__
#include "Orbis\Iggy\include\iggyperfmon.h"
#include "Orbis\Iggy\include\iggyperfmon_orbis.h"
#elif defined _DURANGO
#include "Durango\Iggy\include\iggyperfmon.h"
#elif defined __PS3__
#include "PS3\Iggy\include\iggyperfmon.h"
#include "PS3\Iggy\include\iggyperfmon_ps3.h"
#elif defined __PSVITA__
#include "PSVita\Iggy\include\iggyperfmon.h"
#include "PSVita\Iggy\include\iggyperfmon_psp2.h"
#elif defined __WINDOWS64
#include "Windows64\Iggy\include\iggyperfmon.h"
#endif

#endif

CRITICAL_SECTION UIController::ms_reloadSkinCS;
bool UIController::ms_bReloadSkinCSInitialised = false;

DWORD UIController::m_dwTrialTimerLimitSecs=DYNAMIC_CONFIG_DEFAULT_TRIAL_TIME;

#ifdef _WIN64
static const char *GetWindowsInputDeviceName()
{
	return "KeyboardMouse";
}
#endif

#ifdef _WIN64
static bool IsMouseBlockedScene(EUIScene sceneType)
{
	switch (sceneType)
	{
	case eUIScene_InventoryMenu:
	case eUIScene_CreativeMenu:
	case eUIScene_Crafting2x2Menu:
	case eUIScene_Crafting3x3Menu:
	case eUIScene_FurnaceMenu:
	case eUIScene_BrewingStandMenu:
	case eUIScene_EnchantingMenu:
	case eUIScene_ContainerMenu:
	case eUIScene_LargeContainerMenu:
	case eUIScene_AnvilMenu:
	case eUIScene_DispenserMenu:
	case eUIScene_TradingMenu:
		return true;
	default:
		return false;
	}
}

static bool IsMouseActivatableControlType(UIControl::eUIControlType controlType)
{
	switch (controlType)
	{
	case UIControl::eButtonList:
	case UIControl::eSaveList:
	case UIControl::ePlayerList:
	case UIControl::eDLCList:
	case UIControl::eTexturePackList:
	case UIControl::eLeaderboardList:
	case UIControl::eButton:
	case UIControl::eCheckBox:
	case UIControl::eTextInput:
	case UIControl::ePlayerSkinPreview:
	case UIControl::eEnchantmentButton:
	case UIControl::eSlider:
	case UIControl::eTouchControl:
		return true;
	default:
		return false;
	}
}
#endif

static void RADLINK WarningCallback(void *user_callback_data, Iggy *player, IggyResult code, const char *message)
{
	//enum IggyResult{    IGGY_RESULT_SUCCESS = 0,    IGGY_RESULT_Warning_None = 0,
	//   IGGY_RESULT_Warning_Misc = 100,    IGGY_RESULT_Warning_GDraw = 101,
	//   IGGY_RESULT_Warning_ProgramFlow = 102,
	//   IGGY_RESULT_Warning_Actionscript = 103,
	//   IGGY_RESULT_Warning_Graphics = 104,    IGGY_RESULT_Warning_Font = 105,
	//   IGGY_RESULT_Warning_Timeline = 106,    IGGY_RESULT_Warning_Library = 107,
	//   IGGY_RESULT_Warning_CannotSustainFrameRate = 201,
	//   IGGY_RESULT_Warning_ThrewException = 202,
	//   IGGY_RESULT_Error_Threshhold = 400,    IGGY_RESULT_Error_Misc = 400,
	//   IGGY_RESULT_Error_GDraw = 401,    IGGY_RESULT_Error_ProgramFlow = 402,
	//   IGGY_RESULT_Error_Actionscript = 403,    IGGY_RESULT_Error_Graphics = 404,
	//   IGGY_RESULT_Error_Font = 405,    IGGY_RESULT_Error_Create = 406,
	//   IGGY_RESULT_Error_Library = 407,    IGGY_RESULT_Error_ValuePath = 408,
	//   IGGY_RESULT_Error_Audio = 409,    IGGY_RESULT_Error_Internal = 499,
	//   IGGY_RESULT_Error_InvalidIggy = 501,
	//   IGGY_RESULT_Error_InvalidArgument = 502,
	//   IGGY_RESULT_Error_InvalidEntity = 503,
	//   IGGY_RESULT_Error_UndefinedEntity = 504,
	//   IGGY_RESULT_Error_OutOfMemory = 1001,};

	switch(code)
	{
	case IGGY_RESULT_Warning_CannotSustainFrameRate:
		// Ignore warning
		break;
	default:
		/* Normally, we'd want to issue this warning to some kind of
		logging system or error reporting system, but since this is a
		tutorial app, we just use Win32's default error stream.  Since
		ActionScript 3 exceptions are routed through this warning
		callback, it's definitely a good idea to make sure these
		warnings get printed somewhere that's easy for you to read and
		use for debugging, otherwise debugging errors in the
		ActionScript 3 code in your Flash content will be very
		difficult! */
		app.DebugPrintf(app.USER_SR, message);
		app.DebugPrintf(app.USER_SR, "\n");
		break;
	};
}


/* Flash provides a way for ActionScript 3 code to print debug output
using a function called "trace".  It's very useful for debugging
Flash programs, so ideally, when using Iggy, we'd like to see any
trace output alongside our own debugging output.  To facilitate
this, Iggy allows us to install a callback that will be called
any time ActionScript code calls trace. */
static void RADLINK TraceCallback(void *user_callback_data, Iggy *player, char const *utf8_string, S32 length_in_bytes)
{
	app.DebugPrintf(app.USER_UI, (char *)utf8_string);
}

#ifdef ENABLE_IGGY_PERFMON
static void *RADLINK perf_malloc(void *handle, U32 size)
{
   return malloc(size);
}

static void RADLINK perf_free(void *handle, void *ptr)
{
   return free(ptr);
}
#endif

#ifdef EXCLUDE_IGGY_ALLOCATIONS_FROM_HEAP_INSPECTOR
extern "C" void *__real_malloc(size_t t);
extern "C" void __real_free(void *t);
#endif

__int64 UIController::iggyAllocCount = 0;
static unordered_map<void *,size_t> allocations;
static void * RADLINK AllocateFunction ( void * alloc_callback_user_data , size_t size_requested , size_t * size_returned )
{
	UIController *controller = (UIController *)alloc_callback_user_data;
	EnterCriticalSection(&controller->m_Allocatorlock);
#ifdef EXCLUDE_IGGY_ALLOCATIONS_FROM_HEAP_INSPECTOR
	void *alloc = __real_malloc(size_requested);
#else
	void *alloc = malloc(size_requested);
#endif
	*size_returned = size_requested;
	UIController::iggyAllocCount += size_requested;
	allocations[alloc] = size_requested;
	app.DebugPrintf(app.USER_SR, "Allocating %d, new total: %d\n", size_requested, UIController::iggyAllocCount);
	LeaveCriticalSection(&controller->m_Allocatorlock);
	return alloc;
}

static void RADLINK DeallocateFunction ( void * alloc_callback_user_data , void * ptr )
{
	UIController *controller = (UIController *)alloc_callback_user_data;
	EnterCriticalSection(&controller->m_Allocatorlock);
	size_t size = allocations[ptr];
	UIController::iggyAllocCount -= size;
	allocations.erase(ptr);
	app.DebugPrintf(app.USER_SR, "Freeing %d, new total %d\n", size, UIController::iggyAllocCount);
#ifdef EXCLUDE_IGGY_ALLOCATIONS_FROM_HEAP_INSPECTOR
	__real_free(ptr);
#else
	free(ptr);
#endif
	LeaveCriticalSection(&controller->m_Allocatorlock);
}

UIController::UIController()
{
	m_uiDebugConsole = NULL;
	m_reloadSkinThread = NULL;
	m_navigateToHomeOnReload = false;
	m_mcTTFFont= NULL;
	m_moj7 = NULL;
	m_moj11 = NULL;

#ifdef ENABLE_IGGY_ALLOCATOR
	InitializeCriticalSection(&m_Allocatorlock);
#endif

	// 4J Stu - This is a bit of a hack until we change the Minecraft initialisation to store the proper screen size for other platforms
#if defined _WINDOWS64 || defined _DURANGO || defined __ORBIS__
	m_fScreenWidth = 1920.0f;
	m_fScreenHeight = 1080.0f;
	m_bScreenWidthSetup = true;
#else
	m_fScreenWidth = 1280.0f;
	m_fScreenHeight = 720.0f;
	m_bScreenWidthSetup = false;
#endif

	for(unsigned int i = 0; i < eLibrary_Count; ++i)
	{
		m_iggyLibraries[i] = IGGY_INVALID_LIBRARY;
	}

	for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		m_bMenuDisplayed[i] = false;
		m_iCountDown[i]=0;
		m_bMenuToBeClosed[i]=false;

		for(unsigned int key = 0; key <= ACTION_MAX_MENU; ++key)
		{
			m_actionRepeatTimer[i][key] = 0;
		}
	}

	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		m_bCloseAllScenes[i] = false;
	}

	m_iPressStartQuadrantsMask = 0;

	m_currentRenderViewport = C4JRender::VIEWPORT_TYPE_FULLSCREEN;
	m_bCustomRenderPosition = false;
	m_winUserIndex = 0;
	m_accumulatedTicks = 0;
	m_inputDispatchDepth = 0;
	m_uiBreadcrumbHead = 0;
	m_uiBreadcrumbCount = 0;

	InitializeCriticalSection(&m_navigationLock);
	InitializeCriticalSection(&m_registeredCallbackScenesCS);
	//m_bSysUIShowing=false;
	m_bSystemUIShowing=false;
#ifdef __PSVITA__
	m_bTouchscreenPressed=false;
	m_ActiveUIElement = NULL;
	m_HighlightedUIElement = NULL;
#endif
#ifdef _WIN64
	m_lastMouseX = -1;
	m_lastMouseY = -1;
	m_bMouseOverClickable = false;
	m_pMouseHoverScene = NULL;
	m_iMouseHoverControlId = -1;
	m_pMouseFocusScene = NULL;
	m_iMouseFocusControlId = -1;
	m_pMouseActiveScene = NULL;
	m_iMouseActiveControlId = -1;
	m_iMousePressStartX = 0;
	m_iMousePressStartY = 0;
	m_bMouseDragActive = false;
#endif

	if(!ms_bReloadSkinCSInitialised)
	{
		// MGH - added to prevent crash loading Iggy movies while the skins were being reloaded
		InitializeCriticalSection(&ms_reloadSkinCS);
		ms_bReloadSkinCSInitialised = true;
	}
}

void UIController::PushDebugBreadcrumbV(const char *szFormat, va_list args)
{
	char buffer[512];
	vsprintf_s(buffer, sizeof(buffer), szFormat, args);

	m_uiBreadcrumbs[m_uiBreadcrumbHead] = buffer;
	m_uiBreadcrumbHead = (m_uiBreadcrumbHead + 1) % UI_BREADCRUMB_COUNT;
	if(m_uiBreadcrumbCount < UI_BREADCRUMB_COUNT)
	{
		++m_uiBreadcrumbCount;
	}

	app.DebugPrintf(app.USER_UI, "[UI] %s\n", buffer);
}

void UIController::PushDebugBreadcrumb(const char *szFormat, ...)
{
	va_list args;
	va_start(args, szFormat);
	PushDebugBreadcrumbV(szFormat, args);
	va_end(args);
}

void UIController::DispatchInputToGroups(unsigned int iPad, unsigned int key, bool repeat, bool pressed, bool released)
{
	if(!(repeat || pressed || released))
	{
		return;
	}

#ifdef _WIN64
	PushDebugBreadcrumb("input pad=%u key=%u pressed=%d repeat=%d released=%d device=%s",
		iPad, key, pressed ? 1 : 0, repeat ? 1 : 0, released ? 1 : 0, GetWindowsInputDeviceName());
#else
	PushDebugBreadcrumb("input pad=%u key=%u pressed=%d repeat=%d released=%d",
		iPad, key, pressed ? 1 : 0, repeat ? 1 : 0, released ? 1 : 0);
#endif

	bool handled = false;
	++m_inputDispatchDepth;
	m_groups[(int)eUIGroup_Fullscreen]->handleInput(iPad, key, repeat, pressed, released, handled);
	if(!handled)
	{
		m_groups[(iPad+1)]->handleInput(iPad, key, repeat, pressed, released, handled);
	}
	--m_inputDispatchDepth;

	if(m_inputDispatchDepth == 0)
	{
		FlushPendingNavigation();
	}
}

void UIController::FlushPendingNavigation()
{
	while(!m_pendingNavigation.empty())
	{
		vector<PendingNavigationRequest> requests = m_pendingNavigation;
		m_pendingNavigation.clear();

		for(size_t i = 0; i < requests.size(); ++i)
		{
			PendingNavigationRequest &request = requests[i];
			switch(request.type)
			{
			case PendingNavigationRequest::eNavigateTo:
				NavigateToSceneImmediate(request.iPad, request.scene, request.initData, request.layer, request.group);
				break;
			case PendingNavigationRequest::eNavigateBack:
				NavigateBackImmediate(request.iPad, request.forceUsePad, request.scene, request.layer);
				break;
			case PendingNavigationRequest::eCloseScenes:
				CloseUIScenesImmediate(request.iPad, request.forceIPad);
				break;
			default:
				break;
			}
		}
	}
}

void UIController::SetSysUIShowing(bool bVal)
{
	if(bVal) app.DebugPrintf("System UI showing\n");
	else app.DebugPrintf("System UI stopped showing\n");
	m_bSystemUIShowing=bVal;
}

void UIController::SetSystemUIShowing(LPVOID lpParam,bool bVal)
{
	UIController *pClass=(UIController *)lpParam;
	pClass->SetSysUIShowing(bVal);
}

// SETUP
void UIController::preInit(S32 width, S32 height)
{
	m_fScreenWidth = width;
	m_fScreenHeight = height;
	m_bScreenWidthSetup = true;

#ifdef ENABLE_IGGY_ALLOCATOR
	IggyAllocator allocator;
	allocator.user_callback_data = this;
	allocator.mem_alloc = &AllocateFunction;
	allocator.mem_free = &DeallocateFunction;
	IggyInit(&allocator);
#else
	IggyInit(0);
#endif

	IggySetWarningCallback(WarningCallback, 0);
	IggySetTraceCallbackUTF8(TraceCallback, 0);

	setFontCachingCalculationBuffer(-1);
}

void UIController::postInit()
{
	// set up a custom rendering callback
	IggySetCustomDrawCallback(&UIController::CustomDrawCallback, this);
	IggySetAS3ExternalFunctionCallbackUTF16 ( &UIController::ExternalFunctionCallback, this );
	IggySetTextureSubstitutionCallbacks ( &UIController::TextureSubstitutionCreateCallback , &UIController::TextureSubstitutionDestroyCallback, this );

	SetupFont();
	// 
	loadSkins();

	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		m_groups[i] = new UIGroup((EUIGroup)i,i-1);
	}


#ifdef ENABLE_IGGY_EXPLORER
	iggy_explorer = IggyExpCreate("127.0.0.1", 9190, malloc(IGGYEXP_MIN_STORAGE), IGGYEXP_MIN_STORAGE);
	if ( iggy_explorer == NULL )
	{
		// not normally an error, just an error for this demo!
		app.DebugPrintf( "Couldn't connect to Iggy Explorer, did you run it first?" );
	}
	else
	{
		IggyUseExplorer( m_groups[1]->getHUD()->getMovie(), iggy_explorer);
	}
#endif

#ifdef ENABLE_IGGY_PERFMON
	m_iggyPerfmonEnabled = false;
	iggy_perfmon = IggyPerfmonCreate(perf_malloc, perf_free, NULL);
	IggyInstallPerfmon(iggy_perfmon);
#endif

	NavigateToScene(0, eUIScene_Intro);
}

void UIController::SetupFont()
{
	bool bBitmapFont=false;

	if(m_mcTTFFont!=NULL)
	{
		delete m_mcTTFFont;
	}

	switch(XGetLanguage())
	{
#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
	case XC_LANGUAGE_JAPANESE:
		m_mcTTFFont = new UITTFFont("Common/Media/font/JPN/DF-DotDotGothic16.ttf", 0x203B); // JPN
		break;
	case XC_LANGUAGE_SCHINESE: //TODO
	case XC_LANGUAGE_TCHINESE:
		m_mcTTFFont = new UITTFFont("Common/Media/font/CHT/DFTT_R5.TTC", 0x203B); // CHT
		break;
	case XC_LANGUAGE_KOREAN:
		m_mcTTFFont = new UITTFFont("Common/Media/font/KOR/candadite2.ttf", 0x203B); // KOR
		break;
		// 4J-JEV, Cyrillic characters have been added to this font now, (4/July/14)
		//case XC_LANGUAGE_RUSSIAN:
		//case XC_LANGUAGE_GREEK:
#else
	case XC_LANGUAGE_JAPANESE:
		m_mcTTFFont = new UITTFFont("Common/Media/font/JPN/DFGMaruGothic-Md.ttf", 0x2022); // JPN
		break;
	case XC_LANGUAGE_SCHINESE: //TODO
	case XC_LANGUAGE_TCHINESE:
		m_mcTTFFont = new UITTFFont("Common/Media/font/CHT/DFHeiMedium-B5.ttf", 0x2022); // CHT
		break;
	case XC_LANGUAGE_KOREAN:
		m_mcTTFFont = new UITTFFont("Common/Media/font/KOR/BOKMSD.ttf", 0x2022); // KOR
		break;
#endif
	default:
		bBitmapFont=true;
		// m_mcTTFFont = new UITTFFont("Common/Media/font/Mojangles.ttf", 0x2022); // 4J-JEV: Shouldn't be using this.
		break;
	}

	if(bBitmapFont)
	{
		// these may have been set up by a previous language being chosen
		if(m_moj7==NULL)
		{
			m_moj7 = new UIBitmapFont(SFontData::Mojangles_7);
			m_moj7->registerFont();
		}
		if(m_moj11==NULL)
		{
			m_moj11 = new UIBitmapFont(SFontData::Mojangles_11);
			m_moj11->registerFont();
		}
	}
	else
	{
		app.DebugPrintf("IggyFontSetIndirectUTF8\n");
		IggyFontSetIndirectUTF8( "Mojangles7", -1, IGGY_FONTFLAG_all, "Mojangles_TTF",-1 ,IGGY_FONTFLAG_none );
		IggyFontSetIndirectUTF8( "Mojangles11", -1, IGGY_FONTFLAG_all, "Mojangles_TTF",-1 ,IGGY_FONTFLAG_none );
	}
}

// TICKING
void UIController::tick()
{
	if(m_navigateToHomeOnReload && !ui.IsReloadingSkin())
	{
		ui.CleanUpSkinReload();
		m_navigateToHomeOnReload = false;
		ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_MainMenu);
	}

	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		if(m_bCloseAllScenes[i])
		{
			m_groups[i]->closeAllScenes();
			m_groups[i]->getTooltips()->SetTooltips(-1);
			m_bCloseAllScenes[i] = false;
		}
	}

	if(m_accumulatedTicks == 0) tickInput();
	m_accumulatedTicks = 0;
	FlushPendingNavigation();

	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		m_groups[i]->tick();

		// TODO: May wish to skip ticking other groups here
	}

	// Fix for HUD ticks so that they all tick before this reference is cleared
	EnderDragonRenderer::bossInstance = nullptr;

	// Clear out the cached movie file data
	__int64 currentTime = System::currentTimeMillis();
	for(AUTO_VAR(it, m_cachedMovieData.begin()); it != m_cachedMovieData.end();)
	{
		if(it->second.m_expiry < currentTime)
		{
			delete [] it->second.m_ba.data;
			it = m_cachedMovieData.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void UIController::loadSkins()
{
	wstring platformSkinPath = L"";

#ifdef __PS3__
	platformSkinPath = L"skinPS3.swf";
#elif defined __PSVITA__
	platformSkinPath = L"skinVita.swf";
#elif defined _WINDOWS64
	if(m_fScreenHeight==1080.0f)
	{
		platformSkinPath = L"skinHDWin.swf";
	}
	else
	{
		platformSkinPath = L"skinWin.swf";	
	}
#elif defined _DURANGO
	if(m_fScreenHeight==1080.0f)
	{
		platformSkinPath = L"skinHDDurango.swf";	
	}
	else
	{
		platformSkinPath = L"skinDurango.swf";	
	}
#elif defined __ORBIS__
	if(m_fScreenHeight==1080.0f)
	{
		platformSkinPath = L"skinHDOrbis.swf";	
	}
	else
	{
		platformSkinPath = L"skinOrbis.swf";	
	}

#endif
	// Every platform has one of these, so nothing shared
	if(m_fScreenHeight==1080.0f)
	{
		m_iggyLibraries[eLibrary_Platform] = loadSkin(platformSkinPath, L"platformskinHD.swf");
	}
	else
	{
		m_iggyLibraries[eLibrary_Platform] = loadSkin(platformSkinPath, L"platformskin.swf");
	}

#if defined(__PS3__) || defined(__PSVITA__)
	m_iggyLibraries[eLibrary_GraphicsDefault] = loadSkin(L"skinGraphics.swf", L"skinGraphics.swf");
	m_iggyLibraries[eLibrary_GraphicsHUD] = loadSkin(L"skinGraphicsHud.swf", L"skinGraphicsHud.swf");
	m_iggyLibraries[eLibrary_GraphicsInGame] = loadSkin(L"skinGraphicsInGame.swf", L"skinGraphicsInGame.swf");
	m_iggyLibraries[eLibrary_GraphicsTooltips] = loadSkin(L"skinGraphicsTooltips.swf", L"skinGraphicsTooltips.swf");
	m_iggyLibraries[eLibrary_GraphicsLabels] = loadSkin(L"skinGraphicsLabels.swf", L"skinGraphicsLabels.swf");
	m_iggyLibraries[eLibrary_Labels] = loadSkin(L"skinLabels.swf", L"skinLabels.swf");
	m_iggyLibraries[eLibrary_InGame] = loadSkin(L"skinInGame.swf", L"skinInGame.swf");
	m_iggyLibraries[eLibrary_HUD] = loadSkin(L"skinHud.swf", L"skinHud.swf");
	m_iggyLibraries[eLibrary_Tooltips] = loadSkin(L"skinTooltips.swf", L"skinTooltips.swf");
	m_iggyLibraries[eLibrary_Default] = loadSkin(L"skin.swf", L"skin.swf");
#endif

#if ( defined(_WINDOWS64) || defined(_DURANGO) || defined(__ORBIS__) )

#if defined(_WINDOWS64)
	// 4J Stu - Load the 720/480 skins so that we have something to fallback on during development
#ifndef _FINAL_BUILD
	m_iggyLibraries[eLibraryFallback_GraphicsDefault] = loadSkin(L"skinGraphics.swf", L"skinGraphics.swf");
	m_iggyLibraries[eLibraryFallback_GraphicsHUD] = loadSkin(L"skinGraphicsHud.swf", L"skinGraphicsHud.swf");
	m_iggyLibraries[eLibraryFallback_GraphicsInGame] = loadSkin(L"skinGraphicsInGame.swf", L"skinGraphicsInGame.swf");
	m_iggyLibraries[eLibraryFallback_GraphicsTooltips] = loadSkin(L"skinGraphicsTooltips.swf", L"skinGraphicsTooltips.swf");
	m_iggyLibraries[eLibraryFallback_GraphicsLabels] = loadSkin(L"skinGraphicsLabels.swf", L"skinGraphicsLabels.swf");
	m_iggyLibraries[eLibraryFallback_Labels] = loadSkin(L"skinLabels.swf", L"skinLabels.swf");
	m_iggyLibraries[eLibraryFallback_InGame] = loadSkin(L"skinInGame.swf", L"skinInGame.swf");
	m_iggyLibraries[eLibraryFallback_HUD] = loadSkin(L"skinHud.swf", L"skinHud.swf");
	m_iggyLibraries[eLibraryFallback_Tooltips] = loadSkin(L"skinTooltips.swf", L"skinTooltips.swf");
	m_iggyLibraries[eLibraryFallback_Default] = loadSkin(L"skin.swf", L"skin.swf");
#endif
#endif

	m_iggyLibraries[eLibrary_GraphicsDefault] = loadSkin(L"skinHDGraphics.swf", L"skinHDGraphics.swf");
	m_iggyLibraries[eLibrary_GraphicsHUD] = loadSkin(L"skinHDGraphicsHud.swf", L"skinHDGraphicsHud.swf");
	m_iggyLibraries[eLibrary_GraphicsInGame] = loadSkin(L"skinHDGraphicsInGame.swf", L"skinHDGraphicsInGame.swf");
	m_iggyLibraries[eLibrary_GraphicsTooltips] = loadSkin(L"skinHDGraphicsTooltips.swf", L"skinHDGraphicsTooltips.swf");
	m_iggyLibraries[eLibrary_GraphicsLabels] = loadSkin(L"skinHDGraphicsLabels.swf", L"skinHDGraphicsLabels.swf");
	m_iggyLibraries[eLibrary_Labels] = loadSkin(L"skinHDLabels.swf", L"skinHDLabels.swf");
	m_iggyLibraries[eLibrary_InGame] = loadSkin(L"skinHDInGame.swf", L"skinHDInGame.swf");
	m_iggyLibraries[eLibrary_HUD] = loadSkin(L"skinHDHud.swf", L"skinHDHud.swf");
	m_iggyLibraries[eLibrary_Tooltips] = loadSkin(L"skinHDTooltips.swf", L"skinHDTooltips.swf");
	m_iggyLibraries[eLibrary_Default] = loadSkin(L"skinHD.swf", L"skinHD.swf");
#endif // HD platforms
}

IggyLibrary UIController::loadSkin(const wstring &skinPath, const wstring &skinName)
{
	IggyLibrary lib = IGGY_INVALID_LIBRARY;
	// 4J Stu - We need to load the platformskin before the normal skin, as the normal skin requires some elements from the platform skin
	if(!skinPath.empty() && app.hasArchiveFile(skinPath))
	{
		byteArray baFile = app.getArchiveFile(skinPath);
		lib = IggyLibraryCreateFromMemoryUTF16( (IggyUTF16 *)skinName.c_str() , (void *)baFile.data, baFile.length, NULL );

		delete[] baFile.data;
#ifdef _DEBUG
		IggyMemoryUseInfo memoryInfo;
		rrbool res;
		int iteration = 0;
		__int64 totalStatic = 0;
		while(res = IggyDebugGetMemoryUseInfo ( NULL ,
			lib ,
			"" ,
			0 ,
			iteration ,
			&memoryInfo ))
		{
			totalStatic += memoryInfo.static_allocation_bytes;
			app.DebugPrintf(app.USER_SR, "%ls - %.*s, static: %dB, dynamic: %dB\n", skinPath.c_str(), memoryInfo.subcategory_stringlen, memoryInfo.subcategory, memoryInfo.static_allocation_bytes, memoryInfo.dynamic_allocation_bytes);
			++iteration;
		}

		app.DebugPrintf(app.USER_SR, "%ls - Total static: %dB (%dKB)\n", skinPath.c_str(), totalStatic, totalStatic/1024);
#endif
	}
	return lib;
}

void UIController::ReloadSkin()
{
	// Destroy all scene swf
	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		//m_bCloseAllScenes[i] = true;
		m_groups[i]->DestroyAll();
	}

	// Unload the current libraries
	// Some libraries reference others, so we destroy in reverse order
	for(int i = eLibrary_Count - 1; i >= 0; --i)
	{
		if(m_iggyLibraries[i] != IGGY_INVALID_LIBRARY) IggyLibraryDestroy(m_iggyLibraries[i]);
		m_iggyLibraries[i] = IGGY_INVALID_LIBRARY;
	}

#ifdef _WINDOWS64
	// 4J Stu - Don't load on a thread on windows. I haven't investigated this in detail, so a quick fix
	reloadSkinThreadProc(this);
#else
	// Navigate to the timer scene so that we can display something while the loading is happening
	ui.NavigateToScene(0,eUIScene_Timer,(void *)1,eUILayer_Tooltips,eUIGroup_Fullscreen);

	m_reloadSkinThread = new C4JThread(reloadSkinThreadProc, (void*)this, "Reload skin thread");
	m_reloadSkinThread->SetProcessor(CPU_CORE_UI_SCENE);
	//m_reloadSkinThread->Run();

	//// Load new skin
	//loadSkins();

	//// Reload all scene swf
	//for(int i = eUIGroup_Player1; i <= eUIGroup_Player4; ++i)
	//{
	//	m_groups[i]->ReloadAll();
	//}

	//// Always reload the fullscreen group
	//m_groups[eUIGroup_Fullscreen]->ReloadAll();
#endif
}

void UIController::StartReloadSkinThread()
{
	if(m_reloadSkinThread) m_reloadSkinThread->Run();
}

int UIController::reloadSkinThreadProc(void* lpParam)
{
	EnterCriticalSection(&ms_reloadSkinCS);		// MGH - added to prevent crash loading Iggy movies while the skins were being reloaded
	UIController *controller = (UIController *)lpParam;
	// Load new skin
	controller->loadSkins();

	// Reload all scene swf
	for(int i = eUIGroup_Player1; i < eUIGroup_COUNT; ++i)
	{
		controller->m_groups[i]->ReloadAll();
	}

	// Always reload the fullscreen group
	controller->m_groups[eUIGroup_Fullscreen]->ReloadAll();

	// 4J Stu - Don't do this on windows, as we never navigated forwards to start with
#ifndef _WINDOW64
	controller->NavigateBack(0, false, eUIScene_COUNT, eUILayer_Tooltips);
#endif
	LeaveCriticalSection(&ms_reloadSkinCS);

	return 0;
}

bool UIController::IsReloadingSkin()
{
	return m_reloadSkinThread && (!m_reloadSkinThread->hasStarted() || m_reloadSkinThread->isRunning());
}

bool UIController::IsExpectingOrReloadingSkin()
{
	return Minecraft::GetInstance()->skins->getSelected()->isLoadingData() || Minecraft::GetInstance()->skins->needsUIUpdate() || IsReloadingSkin();
}

void UIController::CleanUpSkinReload()
{
	delete m_reloadSkinThread;
	m_reloadSkinThread = NULL;

	if(!Minecraft::GetInstance()->skins->isUsingDefaultSkin())
	{
		if(!Minecraft::GetInstance()->skins->getSelected()->hasAudio())
		{
#ifdef _DURANGO			
			DWORD result = StorageManager.UnmountInstalledDLC(L"TPACK");
#else
			DWORD result = StorageManager.UnmountInstalledDLC("TPACK");
#endif
		}
	}

	for(AUTO_VAR(it,m_queuedMessageBoxData.begin()); it != m_queuedMessageBoxData.end(); ++it)
	{
		QueuedMessageBoxData *queuedData = *it;
		ui.NavigateToScene(queuedData->iPad, eUIScene_MessageBox, &queuedData->info, queuedData->layer, eUIGroup_Fullscreen);
		delete queuedData->info.uiOptionA;
		delete queuedData;
	}
	m_queuedMessageBoxData.clear();
}

byteArray UIController::getMovieData(const wstring &filename)
{
	// Cache everything we load in the current tick
	__int64 targetTime = System::currentTimeMillis() + (1000LL * 60);
	AUTO_VAR(it,m_cachedMovieData.find(filename));
	if(it == m_cachedMovieData.end() )
	{
		byteArray baFile = app.getArchiveFile(filename);
		CachedMovieData cmd;
		cmd.m_ba = baFile;
		cmd.m_expiry = targetTime;
		m_cachedMovieData[filename] = cmd;
		return baFile;
	}
	else
	{
		it->second.m_expiry = targetTime;
		return it->second.m_ba;
	}
}

// INPUT
void UIController::tickInput()
{
	// If system/commerce UI up, don't handle input
	//if(!m_bSysUIShowing && !m_bSystemUIShowing)
	if(!m_bSystemUIShowing)
	{
#ifdef ENABLE_IGGY_PERFMON
		if (m_iggyPerfmonEnabled)
		{
			if(InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(), ACTION_MENU_STICK_PRESS)) m_iggyPerfmonEnabled = !m_iggyPerfmonEnabled;
		}
		else
#endif
		{
			handleInput();
			++m_accumulatedTicks;
		}
	}
}

void UIController::handleInput()
{
#ifdef _WIN64
	// Handle mouse hover for menu button lists
	HandleMouseHover();
#endif

	// For each user, loop over each key type and send messages based on the state
	for(unsigned int iPad = 0; iPad < XUSER_MAX_COUNT; ++iPad)
	{
#ifdef _DURANGO
		// 4J-JEV: Added exception for primary play who migh've uttered speech commands.
		if(iPad != ProfileManager.GetPrimaryPad() 
			&& (!InputManager.IsPadConnected(iPad) || !InputManager.IsPadLocked(iPad)) ) continue;
#endif
		for(unsigned int key = 0; key <= ACTION_MAX_MENU; ++key)
		{
			handleKeyPress(iPad, key);
		}

#ifdef __PSVITA__
		//CD - Vita requires key press 40 - select [MINECRAFT_ACTION_GAME_INFO]
		handleKeyPress(iPad, MINECRAFT_ACTION_GAME_INFO);
#endif
	}

#ifdef _WIN64
	if(m_inputDispatchDepth == 0)
	{
		FlushPendingMouseActions();
	}
#endif

#ifdef _DURANGO
	if(!app.GetGameStarted())
	{
		bool repeat = false;
		int firstUnfocussedUnhandledPad = -1;

		// For durango, check for unmapped controllers
		for(unsigned int iPad = XUSER_MAX_COUNT; iPad < (XUSER_MAX_COUNT + InputManager.MAX_GAMEPADS); ++iPad)
		{
			if(InputManager.IsPadLocked(iPad) || !InputManager.IsPadConnected(iPad) ) continue;

			for(unsigned int key = 0; key <= ACTION_MAX_MENU; ++key)
			{

				bool pressed = InputManager.ButtonPressed(iPad,key); // Toggle
				bool released = InputManager.ButtonReleased(iPad,key); // Toggle

				if(pressed || released)
				{
					bool handled = false;

					// Send the key to the fullscreen group first
					m_groups[(int)eUIGroup_Fullscreen]->handleInput(iPad, key, repeat, pressed, released, handled);

					if(firstUnfocussedUnhandledPad < 0 && !m_groups[(int)eUIGroup_Fullscreen]->HasFocus(iPad))
					{
						firstUnfocussedUnhandledPad = iPad;
					}
				}
			}
		}

		if(ProfileManager.GetLockedProfile() >= 0 && !InputManager.IsPadLocked( ProfileManager.GetLockedProfile() ) && firstUnfocussedUnhandledPad >= 0)
		{
			ProfileManager.RequestSignInUI(false, false, false, false, true, NULL, NULL, firstUnfocussedUnhandledPad );
		}
	}
#endif
}

void UIController::handleKeyPress(unsigned int iPad, unsigned int key)
{

	bool down = false;
	bool pressed = false; // Toggle
	bool released = false; // Toggle
	bool repeat = false;

#ifdef __PSVITA__
	if(key==ACTION_MENU_OK)
	{
		bool bTouchScreenInput=false;

		// check the touchscreen

		// 4J-PB - use the touchscreen for quickselect
		SceTouchData* pTouchData = InputManager.GetTouchPadData(iPad,false);

		if((m_bTouchscreenPressed==false) && pTouchData->reportNum==1)
		{
			// no active touch? clear active and highlighted touch UI elements
			m_ActiveUIElement = NULL;
			m_HighlightedUIElement = NULL; 

			// fullscreen first
			UIScene *pScene=m_groups[(int)eUIGroup_Fullscreen]->getCurrentScene();
			// also check tooltip scene if we're not touching anything in the main scene
			UIScene *pToolTips=m_groups[(int)eUIGroup_Fullscreen]->getTooltips();
			if(pScene)
			{
				// scene touch check
				if(TouchBoxHit(pScene,pTouchData->report[0].x,pTouchData->report[0].y))
				{
					down=pressed=m_bTouchscreenPressed=true;
					bTouchScreenInput=true;
				}
				// tooltip touch check
				else if(TouchBoxHit(pToolTips,pTouchData->report[0].x,pTouchData->report[0].y))
				{
					down=pressed=m_bTouchscreenPressed=true;
					bTouchScreenInput=true;
				}
			}
			else
			{
				pScene=m_groups[(EUIGroup)(iPad+1)]->getCurrentScene();
				pToolTips=m_groups[(int)iPad+1]->getTooltips();
				if(pScene)
				{
					// scene touch check
					if(TouchBoxHit(pScene,pTouchData->report[0].x,pTouchData->report[0].y))
					{
						down=pressed=m_bTouchscreenPressed=true;
						bTouchScreenInput=true;
					}
					// tooltip touch check (if scene exists but not component has been touched)
					else if(TouchBoxHit(pToolTips,pTouchData->report[0].x,pTouchData->report[0].y))
					{
						down=pressed=m_bTouchscreenPressed=true;
						bTouchScreenInput=true;
					}
				}
				else if(pToolTips)
				{
					// tooltip touch check (if scene does not exist)
					if(TouchBoxHit(pToolTips,pTouchData->report[0].x,pTouchData->report[0].y))
					{
						down=pressed=m_bTouchscreenPressed=true;
						bTouchScreenInput=true;
					}
				}
			}
		}	
		else if(m_bTouchscreenPressed && pTouchData->reportNum==1)
		{
			// fullscreen first
			UIScene *pScene=m_groups[(int)eUIGroup_Fullscreen]->getCurrentScene();
			// also check tooltip scene if we're not touching anything in the main scene
			UIScene *pToolTips=m_groups[(int)eUIGroup_Fullscreen]->getTooltips();
			if(pScene)
			{
				// scene touch check
				if(TouchBoxHit(pScene,pTouchData->report[0].x,pTouchData->report[0].y))
				{
					down=true;
					bTouchScreenInput=true;
				}
				// tooltip touch check (if scene exists but not component has been touched)
				else if(TouchBoxHit(pToolTips,pTouchData->report[0].x,pTouchData->report[0].y))
				{
					down=true;
					bTouchScreenInput=true;
				}
			}
			else
			{
				pScene=m_groups[(EUIGroup)(iPad+1)]->getCurrentScene();
				pToolTips=m_groups[(int)iPad+1]->getTooltips();
				if(pScene)
				{
					// scene touch check
					if(TouchBoxHit(pScene,pTouchData->report[0].x,pTouchData->report[0].y))
					{
						down=true;
						bTouchScreenInput=true;
					}
					// tooltip touch check (if scene exists but not component has been touched)
					else if(TouchBoxHit(pToolTips,pTouchData->report[0].x,pTouchData->report[0].y))
					{
						down=true;
						bTouchScreenInput=true;
					}
				}
				else if(pToolTips)
				{
					// tooltip touch check (if scene does not exist)
					if(TouchBoxHit(pToolTips,pTouchData->report[0].x,pTouchData->report[0].y))
					{
						down=true;
						bTouchScreenInput=true;
					}
				}
			}
		}
		else if(m_bTouchscreenPressed && pTouchData->reportNum==0)
		{
			// released
			bTouchScreenInput=true;
			m_bTouchscreenPressed=false;
			released=true;
		}

		if(pressed)
		{
			// Start repeat timer
			m_actionRepeatTimer[iPad][key] = GetTickCount() + UI_REPEAT_KEY_DELAY_MS;
		}
		else if (released)
		{
			// Stop repeat timer
			m_actionRepeatTimer[iPad][key] = 0;
		}
		else if (down)
		{
			// Check is enough time has elapsed to be a repeat key
			DWORD currentTime = GetTickCount();
			if(m_actionRepeatTimer[iPad][key] > 0 && currentTime > m_actionRepeatTimer[iPad][key])
			{
				repeat = true;
				pressed = true;
				m_actionRepeatTimer[iPad][key] = currentTime + UI_REPEAT_KEY_REPEAT_RATE_MS;
			}
		}

		// handle touch input
		HandleTouchInput(iPad, key, pressed, repeat, released);

		// ignore any other presses if the touchscreen has been used
		if(bTouchScreenInput) return;
	}
#endif

	down = InputManager.ButtonDown(iPad,key);
	pressed = InputManager.ButtonPressed(iPad,key); // Toggle
	released = InputManager.ButtonReleased(iPad,key); // Toggle

#ifdef _WIN64
	if (iPad == 0 && g_KeyboardMouseInput.IsEnabled())
	{
		bool kbDown = false;
		bool kbPressed = false;
		bool kbReleased = false;
		bool mouseCaptured = g_KeyboardMouseInput.IsMouseCaptured();

		auto isBindDown = [](EGameAction action) -> bool {
			KeyBind b = g_KeyBindings.GetBinding(action);
			if (b.isMouse) return g_KeyboardMouseInput.IsMouseButtonDown(b.vkCode);
			return b.vkCode != 0 && g_KeyboardMouseInput.IsKeyDown(b.vkCode);
		};
		auto isBindPressed = [](EGameAction action) -> bool {
			KeyBind b = g_KeyBindings.GetBinding(action);
			if (b.isMouse) return g_KeyboardMouseInput.IsMouseButtonPressed(b.vkCode);
			return b.vkCode != 0 && g_KeyboardMouseInput.IsKeyPressed(b.vkCode);
		};
		auto isBindReleased = [](EGameAction action) -> bool {
			KeyBind b = g_KeyBindings.GetBinding(action);
			if (b.isMouse) return g_KeyboardMouseInput.IsMouseButtonReleased(b.vkCode);
			return b.vkCode != 0 && g_KeyboardMouseInput.IsKeyReleased(b.vkCode);
		};

		switch (key)
		{
		case ACTION_MENU_UP:
			if (!mouseCaptured)
			{
				kbDown = isBindDown(eGameAction_MoveForward) || isBindDown(eGameAction_MenuUp);
				kbPressed = isBindPressed(eGameAction_MoveForward) || isBindPressed(eGameAction_MenuUp);
				kbReleased = isBindReleased(eGameAction_MoveForward) || isBindReleased(eGameAction_MenuUp);
			}
			break;
		case ACTION_MENU_DOWN:
			if (!mouseCaptured)
			{
				kbDown = isBindDown(eGameAction_MoveBack) || isBindDown(eGameAction_MenuDown);
				kbPressed = isBindPressed(eGameAction_MoveBack) || isBindPressed(eGameAction_MenuDown);
				kbReleased = isBindReleased(eGameAction_MoveBack) || isBindReleased(eGameAction_MenuDown);
			}
			break;
		case ACTION_MENU_LEFT:
			if (!mouseCaptured)
			{
				kbDown = isBindDown(eGameAction_MoveLeft) || isBindDown(eGameAction_MenuLeft);
				kbPressed = isBindPressed(eGameAction_MoveLeft) || isBindPressed(eGameAction_MenuLeft);
				kbReleased = isBindReleased(eGameAction_MoveLeft) || isBindReleased(eGameAction_MenuLeft);
			}
			break;
		case ACTION_MENU_RIGHT:
			if (!mouseCaptured)
			{
				kbDown = isBindDown(eGameAction_MoveRight) || isBindDown(eGameAction_MenuRight);
				kbPressed = isBindPressed(eGameAction_MoveRight) || isBindPressed(eGameAction_MenuRight);
				kbReleased = isBindReleased(eGameAction_MoveRight) || isBindReleased(eGameAction_MenuRight);
			}
			break;
		case ACTION_MENU_A:
		case ACTION_MENU_OK:
			{
				kbDown = isBindDown(eGameAction_MenuOK) || g_KeyboardMouseInput.IsKeyDown(KB_KEY_SPACE);
				kbPressed = isBindPressed(eGameAction_MenuOK) || g_KeyboardMouseInput.IsKeyPressed(KB_KEY_SPACE);
				kbReleased = isBindReleased(eGameAction_MenuOK) || g_KeyboardMouseInput.IsKeyReleased(KB_KEY_SPACE);
			}
			break;
		case ACTION_MENU_B:
		case ACTION_MENU_CANCEL:
			{
				kbDown = isBindDown(eGameAction_MenuCancel) || g_KeyboardMouseInput.IsKeyDown(KB_KEY_BACKSPACE);
				kbPressed = isBindPressed(eGameAction_MenuCancel) || g_KeyboardMouseInput.IsKeyPressed(KB_KEY_BACKSPACE);
				kbReleased = isBindReleased(eGameAction_MenuCancel) || g_KeyboardMouseInput.IsKeyReleased(KB_KEY_BACKSPACE);
			}
			break;
		case ACTION_MENU_X:
			if (!mouseCaptured)
			{
				kbDown = isBindDown(eGameAction_MenuAlt);
				kbPressed = isBindPressed(eGameAction_MenuAlt);
				kbReleased = isBindReleased(eGameAction_MenuAlt);
			}
			break;
		case ACTION_MENU_Y:
			if (!mouseCaptured)
			{
				kbDown = isBindDown(eGameAction_MenuQuickMove);
				kbPressed = isBindPressed(eGameAction_MenuQuickMove);
				kbReleased = isBindReleased(eGameAction_MenuQuickMove);
			}
			break;
		case ACTION_MENU_LEFT_SCROLL:
			{
				KeyBind tabL = g_KeyBindings.GetBinding(eGameAction_MenuTabLeft);
				if (!tabL.isMouse && tabL.vkCode != 0 && g_KeyboardMouseInput.IsKeyPressed(tabL.vkCode))
					kbPressed = true;
			}
			break;
		case ACTION_MENU_RIGHT_SCROLL:
			{
				KeyBind tabR = g_KeyBindings.GetBinding(eGameAction_MenuTabRight);
				if (!tabR.isMouse && tabR.vkCode != 0 && g_KeyboardMouseInput.IsKeyPressed(tabR.vkCode))
					kbPressed = true;
			}
			break;
		case ACTION_MENU_PAGEUP:
			{
				KeyBind pgUp = g_KeyBindings.GetBinding(eGameAction_MenuPageUp);
				if (!pgUp.isMouse && pgUp.vkCode != 0 && g_KeyboardMouseInput.IsKeyPressed(pgUp.vkCode))
					kbPressed = true;
			}
			break;
		case ACTION_MENU_PAGEDOWN:
			{
				KeyBind pgDn = g_KeyBindings.GetBinding(eGameAction_MenuPageDown);
				if (!pgDn.isMouse && pgDn.vkCode != 0 && g_KeyboardMouseInput.IsKeyPressed(pgDn.vkCode))
					kbPressed = true;
			}
			break;
		case ACTION_MENU_PAUSEMENU:
			{
				KeyBind pauseBind = g_KeyBindings.GetBinding(eGameAction_MenuPause);
				if (!pauseBind.isMouse && pauseBind.vkCode != 0)
				{
					kbDown = g_KeyboardMouseInput.IsKeyDown(pauseBind.vkCode);
					kbPressed = g_KeyboardMouseInput.IsKeyPressed(pauseBind.vkCode);
					kbReleased = g_KeyboardMouseInput.IsKeyReleased(pauseBind.vkCode);
				}
			}
			break;
		case ACTION_MENU_STICK_PRESS:
			if (g_KeyboardMouseInput.IsKeyPressed(KB_KEY_F))
				kbPressed = true;
			break;
		case ACTION_MENU_OTHER_STICK_PRESS:
			if (g_KeyboardMouseInput.IsKeyPressed(KB_KEY_R))
				kbPressed = true;
			break;
		case ACTION_MENU_OTHER_STICK_UP:
			if (!mouseCaptured && g_KeyboardMouseInput.IsKeyPressed(KB_KEY_PAGEUP))
				kbPressed = true;
			break;
		case ACTION_MENU_OTHER_STICK_DOWN:
			if (!mouseCaptured && g_KeyboardMouseInput.IsKeyPressed(KB_KEY_PAGEDOWN))
				kbPressed = true;
			break;
		case ACTION_MENU_OTHER_STICK_LEFT:
			if (!mouseCaptured && g_KeyboardMouseInput.IsKeyPressed(KB_KEY_HOME))
				kbPressed = true;
			break;
		case ACTION_MENU_OTHER_STICK_RIGHT:
			if (!mouseCaptured && g_KeyboardMouseInput.IsKeyPressed(KB_KEY_END))
				kbPressed = true;
			break;
		}

		down = down || kbDown;
		pressed = pressed || kbPressed;
		released = released || kbReleased;
	}
#endif

	if(pressed) app.DebugPrintf("Pressed %d\n",key);
	if(released) app.DebugPrintf("Released %d\n",key);
	// Repeat handling
	if(pressed)
	{
		// Start repeat timer
		m_actionRepeatTimer[iPad][key] = GetTickCount() + UI_REPEAT_KEY_DELAY_MS;
	}
	else if (released)
	{
		// Stop repeat timer
		m_actionRepeatTimer[iPad][key] = 0;
	}
	else if (down)
	{
		// Check is enough time has elapsed to be a repeat key
		DWORD currentTime = GetTickCount();
		if(m_actionRepeatTimer[iPad][key] > 0 && currentTime > m_actionRepeatTimer[iPad][key])
		{
			repeat = true;
			pressed = true;
			m_actionRepeatTimer[iPad][key] = currentTime + UI_REPEAT_KEY_REPEAT_RATE_MS;
		}
	}

#ifndef _CONTENT_PACKAGE

#ifdef ENABLE_IGGY_PERFMON
	if ( pressed && !repeat && key == ACTION_MENU_STICK_PRESS)
	{
		m_iggyPerfmonEnabled = !m_iggyPerfmonEnabled;
	}
#endif

	// 4J Stu - Removed this function
#if 0
#ifdef __PS3__
	//if (	pressed &&
	//	!repeat &&
	//	//app.GetGameSettingsDebugMask(ProfileManager.GetPrimaryPad())&(1L<<eDebugSetting_ToggleFont) &&
	//	key == ACTION_MENU_STICK_PRESS)
	//{
	//	static bool whichFont = true;
	//	if (whichFont)
	//	{
	//		IggyFontSetIndirectUTF8( "Mojangles_7", -1, IGGY_FONTFLAG_all, "Mojangles_TTF",-1 ,IGGY_FONTFLAG_none );
	//		IggyFontSetIndirectUTF8( "Mojangles_11", -1, IGGY_FONTFLAG_all, "Mojangles_TTF",-1 ,IGGY_FONTFLAG_none );
	//		whichFont = false;
	//	}
	//	else
	//	{
	//		IggyFontSetIndirectUTF8( "Mojangles_7", -1, IGGY_FONTFLAG_all, "Mojangles_7",-1 ,IGGY_FONTFLAG_none );
	//		IggyFontSetIndirectUTF8( "Mojangles_11", -1, IGGY_FONTFLAG_all, "Mojangles_11",-1 ,IGGY_FONTFLAG_none );
	//		whichFont = true;
	//	}
	//}
	//else
	if (	pressed &&
		!repeat &&
		//!(app.GetGameSettingsDebugMask(ProfileManager.GetPrimaryPad())&(1L<<eDebugSetting_ToggleFont)) &&
		key == ACTION_MENU_STICK_PRESS)
	{
		__int64 totalStatic = 0;
		__int64 totalDynamic = 0;
		app.DebugPrintf(app.USER_SR, "********************************\n");
		app.DebugPrintf(app.USER_SR, "BEGIN TOTAL SWF MEMORY USAGE\n\n");
		for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
		{
			m_groups[i]->PrintTotalMemoryUsage(totalStatic, totalDynamic);
		}
		for(unsigned int i = 0; i < eLibrary_Count; ++i)
		{
			__int64 libraryStatic = 0;
			__int64 libraryDynamic = 0;

			if(m_iggyLibraries[i] != IGGY_INVALID_LIBRARY)
			{

				IggyMemoryUseInfo memoryInfo;
				rrbool res;
				int iteration = 0;
				while(res = IggyDebugGetMemoryUseInfo ( NULL ,
					m_iggyLibraries[i] ,
					"" ,
					0 ,
					iteration ,
					&memoryInfo ))
				{
					libraryStatic += memoryInfo.static_allocation_bytes;
					libraryDynamic += memoryInfo.dynamic_allocation_bytes;
					totalStatic += memoryInfo.static_allocation_bytes;
					totalDynamic += memoryInfo.dynamic_allocation_bytes;
					++iteration;
				}
			}

			app.DebugPrintf(app.USER_SR, "Library static: %dB , Library dynamic: %d, ID: %d\n", libraryStatic, libraryDynamic, i);
		}
		app.DebugPrintf(app.USER_SR, "Total static: %d , Total dynamic: %d\n", totalStatic, totalDynamic);
		app.DebugPrintf(app.USER_SR, "\n\nEND TOTAL SWF MEMORY USAGE\n");
		app.DebugPrintf(app.USER_SR, "********************************\n\n");
	} 
	else 
#endif
#endif
#endif
		//#endif
		DispatchInputToGroups(iPad, key, repeat, pressed, released);
}

rrbool RADLINK UIController::ExternalFunctionCallback( void * user_callback_data , Iggy * player , IggyExternalFunctionCallUTF16 * call)
{
	UIScene *scene = (UIScene *)IggyPlayerGetUserdata(player);

	if(scene != NULL)
	{
		scene->externalCallback(call);
	}

	return true;
}

// RENDERING
void UIController::renderScenes()
{
	PIXBeginNamedEvent(0, "Rendering Iggy scenes");
	// Only render player scenes if the game is started
	if(app.GetGameStarted() && !m_groups[eUIGroup_Fullscreen]->hidesLowerScenes())
	{
		for(int i = eUIGroup_Player1; i < eUIGroup_COUNT; ++i)
		{
			PIXBeginNamedEvent(0, "Rendering layer %d scenes", i);
			m_groups[i]->render();
			PIXEndNamedEvent();
		}
	}

	// Always render the fullscreen group
	PIXBeginNamedEvent(0, "Rendering fullscreen scenes");
	m_groups[eUIGroup_Fullscreen]->render();
	PIXEndNamedEvent();

	PIXEndNamedEvent();

#ifdef ENABLE_IGGY_PERFMON
	if (m_iggyPerfmonEnabled)
	{
		IggyPerfmonPad pm_pad;

		pm_pad.bits = 0;
		pm_pad.field.dpad_up           = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_UP);
		pm_pad.field.dpad_down         = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_DOWN);
		pm_pad.field.dpad_left         = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_LEFT);
		pm_pad.field.dpad_right        = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_RIGHT);
		pm_pad.field.button_up         = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_Y);
		pm_pad.field.button_down       = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_A);
		pm_pad.field.button_left       = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_X);
		pm_pad.field.button_right      = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_B);
		pm_pad.field.shoulder_left_hi  = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_LEFT_SCROLL);
		pm_pad.field.shoulder_right_hi = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_RIGHT_SCROLL);
		pm_pad.field.trigger_left_low  = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_PAGEUP);
		pm_pad.field.trigger_right_low = InputManager.ButtonPressed(ProfileManager.GetPrimaryPad(),ACTION_MENU_PAGEDOWN);
		//IggyPerfmonPadFromXInputStatePointer(pm_pad, &xi_pad);

		//gdraw_D3D_SetTileOrigin( fb,
		//	zb,
		//	PM_ORIGIN_X,
		//	PM_ORIGIN_Y );
		IggyPerfmonTickAndDraw(iggy_perfmon, gdraw_funcs, &pm_pad,
			PM_ORIGIN_X, PM_ORIGIN_Y, getScreenWidth(), getScreenHeight());     // perfmon draw area in window coords
	}
#endif
}

void UIController::getRenderDimensions(C4JRender::eViewportType viewport, S32 &width, S32 &height)
{
	switch( viewport )
	{
	case C4JRender::VIEWPORT_TYPE_FULLSCREEN:
		width = (S32)(getScreenWidth());
		height = (S32)(getScreenHeight());
		break;
	case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
	case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
		width = (S32)(getScreenWidth() / 2);
		height = (S32)(getScreenHeight() / 2);
		break;
	case C4JRender::VIEWPORT_TYPE_SPLIT_LEFT:
	case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
		width = (S32)(getScreenWidth() / 2);
		height = (S32)(getScreenHeight() / 2);
		break;
	case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT:
	case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
	case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_LEFT:
	case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT:
		width = (S32)(getScreenWidth() / 2);
		height = (S32)(getScreenHeight() / 2);
		break;
	}
}

void UIController::setupRenderPosition(C4JRender::eViewportType viewport)
{
	if(m_bCustomRenderPosition || m_currentRenderViewport != viewport)
	{
		m_currentRenderViewport = viewport;
		m_bCustomRenderPosition = false;
		S32 xPos = 0;
		S32 yPos = 0;
		switch( viewport )
		{
		case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
			xPos = (S32)(getScreenWidth() / 4);
			break;
		case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
			xPos = (S32)(getScreenWidth() / 4);
			yPos = (S32)(getScreenHeight() / 2);
			break;
		case C4JRender::VIEWPORT_TYPE_SPLIT_LEFT:
			yPos = (S32)(getScreenHeight() / 4);
			break;
		case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
			xPos = (S32)(getScreenWidth() / 2);
			yPos = (S32)(getScreenHeight() / 4);
			break;
		case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT:
			break;
		case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
			xPos = (S32)(getScreenWidth() / 2);
			break;
		case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_LEFT:
			yPos = (S32)(getScreenHeight() / 2);
			break;
		case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT:
			xPos = (S32)(getScreenWidth() / 2);
			yPos = (S32)(getScreenHeight() / 2);
			break;
		}
		m_tileOriginX = xPos;
		m_tileOriginY = yPos;
		setTileOrigin(xPos, yPos);
	}
}

void UIController::setupRenderPosition(S32 xOrigin, S32 yOrigin)
{
	m_bCustomRenderPosition = true;
	m_tileOriginX = xOrigin;
	m_tileOriginY = yOrigin;
	setTileOrigin(xOrigin, yOrigin);
}

void UIController::setupCustomDrawGameState()
{
	// Rest the clear rect
	m_customRenderingClearRect.left = LONG_MAX;
	m_customRenderingClearRect.right = LONG_MIN;
	m_customRenderingClearRect.top = LONG_MAX;
	m_customRenderingClearRect.bottom = LONG_MIN;

#if defined _WINDOWS64 || _DURANGO
	PIXBeginNamedEvent(0,"StartFrame");
	RenderManager.StartFrame();
	PIXEndNamedEvent();
	gdraw_D3D11_setViewport_4J();
#elif defined __PS3__
	RenderManager.StartFrame();
#elif defined __PSVITA__
	RenderManager.StartFrame();
#elif defined __ORBIS__
	RenderManager.StartFrame(false);
	// Set up a viewport for the render that matches Iggy's own viewport, apart form using an opengl-style z-range (Iggy uses a DX-style range on PS4), so
	// that the renderer orthographic projection will work
	gdraw_orbis_setViewport_4J();
#endif
	RenderManager.Set_matrixDirty();

	// 4J Stu - We don't need to clear this here as iggy hasn't written anything to the depth buffer.
	// We DO however clear after we render which is why we still setup the rectangle here
	//RenderManager.Clear(GL_DEPTH_BUFFER_BIT, &m_customRenderingClearRect);
	//glClear(GL_DEPTH_BUFFER_BIT);

	PIXBeginNamedEvent(0,"Final setup");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_fScreenWidth, m_fScreenHeight, 0, 1000, 3000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_ALPHA_TEST);			
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);
	PIXEndNamedEvent();
}

void UIController::setupCustomDrawMatrices(UIScene *scene, CustomDrawData *customDrawRegion)
{
	Minecraft *pMinecraft=Minecraft::GetInstance();

	// Clear just the region required for this control.
	float sceneWidth = (float)scene->getRenderWidth();
	float sceneHeight = (float)scene->getRenderHeight();

	LONG left, right, top, bottom;
#ifdef __PS3__
	if(!RenderManager.IsHiDef() && !RenderManager.IsWidescreen())
	{
		// 4J Stu - Our SD target on PS3 is double width
		left = m_tileOriginX + (sceneWidth + customDrawRegion->mat[(0*4)+3]*sceneWidth);
		right = left + ( (sceneWidth * customDrawRegion->mat[0]) ) * customDrawRegion->x1;
	}
	else
#endif
	{
		left = m_tileOriginX + (sceneWidth + customDrawRegion->mat[(0*4)+3]*sceneWidth)/2;
		right = left + ( (sceneWidth * customDrawRegion->mat[0])/2 ) * customDrawRegion->x1;
	}

	top = m_tileOriginY + (sceneHeight - customDrawRegion->mat[(1*4)+3]*sceneHeight)/2;
	bottom = top + (sceneHeight * -customDrawRegion->mat[(1*4) + 1])/2 * customDrawRegion->y1;

	m_customRenderingClearRect.left = min(m_customRenderingClearRect.left, left);
	m_customRenderingClearRect.right = max(m_customRenderingClearRect.right, right);;
	m_customRenderingClearRect.top = min(m_customRenderingClearRect.top, top);
	m_customRenderingClearRect.bottom = max(m_customRenderingClearRect.bottom, bottom);

	if(!m_bScreenWidthSetup)
	{
		Minecraft *pMinecraft=Minecraft::GetInstance();
		if(pMinecraft != NULL)
		{
			m_fScreenWidth=(float)pMinecraft->width_phys;
			m_fScreenHeight=(float)pMinecraft->height_phys;
			m_bScreenWidthSetup = true;
		}
	}

	glLoadIdentity();
	glTranslatef(0, 0, -2000);
	// Iggy translations are based on a double-size target, with the origin in the centre
	glTranslatef((m_fScreenWidth + customDrawRegion->mat[(0*4)+3]*m_fScreenWidth)/2,(m_fScreenHeight - customDrawRegion->mat[(1*4)+3]*m_fScreenHeight)/2,0);
	// Iggy scales are based on a double-size target
	glScalef( (m_fScreenWidth * customDrawRegion->mat[0])/2,(m_fScreenHeight * -customDrawRegion->mat[(1*4) + 1])/2,1.0f);
}

void UIController::setupCustomDrawGameStateAndMatrices(UIScene *scene, CustomDrawData *customDrawRegion)
{
	setupCustomDrawGameState();
	setupCustomDrawMatrices(scene, customDrawRegion);
}

void UIController::endCustomDrawGameState()
{
#ifdef __ORBIS__
	// TO BE IMPLEMENTED
	RenderManager.Clear(GL_DEPTH_BUFFER_BIT);
#else
	RenderManager.Clear(GL_DEPTH_BUFFER_BIT, &m_customRenderingClearRect);
#endif
	//glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(false);
	glDisable(GL_ALPHA_TEST);
}

void UIController::endCustomDrawMatrices()
{
}

void UIController::endCustomDrawGameStateAndMatrices()
{
	endCustomDrawMatrices();
	endCustomDrawGameState();
}

void RADLINK UIController::CustomDrawCallback(void *user_callback_data, Iggy *player, IggyCustomDrawCallbackRegion *region)
{
	UIScene *scene = (UIScene *)IggyPlayerGetUserdata(player);

	if(scene != NULL)
	{
		scene->customDraw(region);
	}
}

//Description
//Callback to create a user-defined texture to replace SWF-defined textures.
//Parameters
//width - Input value: optional number of pixels wide specified from AS3, or -1 if not defined. Output value: the number of pixels wide to pretend to Iggy that the bitmap is. SWF and AS3 scales bitmaps based on their pixel dimensions, so you can use this to substitute a texture that is higher or lower resolution that ActionScript thinks it is.  
//height - Input value: optional number of pixels high specified from AS3, or -1 if not defined. Output value: the number of pixels high to pretend to Iggy that the bitmap is. SWF and AS3 scales bitmaps based on their pixel dimensions, so you can use this to substitute a texture that is higher or lower resolution that ActionScript thinks it is.  
//destroy_callback_data - Optional additional output value you can set; the value will be passed along to the corresponding Iggy_TextureSubstitutionDestroyCallback (e.g. you can store the pointer to your own internal structure here).  
//return - A platform-independent wrapped texture handle provided by GDraw, or NULL (NULL with throw an ActionScript 3 ArgumentError that the Flash developer can catch) Use by calling IggySetTextureSubstitutionCallbacks.  
//
//Discussion
//
//If your texture includes an alpha channel, you must use a premultiplied alpha (where the R,G, and B channels have been multiplied by the alpha value); all Iggy shaders assume premultiplied alpha (and it looks better anyway).
GDrawTexture * RADLINK UIController::TextureSubstitutionCreateCallback ( void * user_callback_data , IggyUTF16 * texture_name , S32 * width , S32 * height , void * * destroy_callback_data )
{
	UIController *uiController = (UIController *)user_callback_data;
	AUTO_VAR(it,uiController->m_substitutionTextures.find((wchar_t *)texture_name));

	if(it != uiController->m_substitutionTextures.end())
	{
		app.DebugPrintf("Found substitution texture %ls, with %d bytes\n", (wchar_t *)texture_name,it->second.length);

		BufferedImage image(it->second.data, it->second.length);
		if( image.getData() != NULL )
		{
			image.preMultiplyAlpha();
			Textures *t = Minecraft::GetInstance()->textures;
			int id = t->getTexture(&image,C4JRender::TEXTURE_FORMAT_RxGyBzAw,false);

			// 4J Stu - All our flash controls that allow replacing textures use a special 64x64 symbol
			// Force this size here so that our images don't get scaled wildly
	#if (defined __ORBIS__ || defined _DURANGO )
			*width = 96;
			*height = 96;
	#else
			*width = 64;
			*height = 64;

	#endif
			*destroy_callback_data = (void *)id;

			app.DebugPrintf("Found substitution texture %ls (%d) - %dx%d\n", (wchar_t *)texture_name, id, image.getWidth(), image.getHeight());
			return ui.getSubstitutionTexture(id);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		app.DebugPrintf("Could not find substitution texture %ls\n", (wchar_t *)texture_name);
		return NULL;
	}
}

//Description
//Callback received from Iggy when it stops using a user-defined texture.
void RADLINK UIController::TextureSubstitutionDestroyCallback ( void * user_callback_data , void * destroy_callback_data , GDrawTexture * handle )
{
	// Orbis complains about casting a pointer to an int
	LONGLONG llVal=(LONGLONG)destroy_callback_data;
	int id=(int)llVal;
	app.DebugPrintf("Destroying iggy texture %d\n", id);

	ui.destroySubstitutionTexture(user_callback_data, handle);

	Textures *t = Minecraft::GetInstance()->textures;
	t->releaseTexture( id );
}

void UIController::registerSubstitutionTexture(const wstring &textureName, PBYTE pbData, DWORD dwLength)
{
	// Remove it if it already exists
	unregisterSubstitutionTexture(textureName,false);

	m_substitutionTextures[textureName] = byteArray(pbData, dwLength);
}

void UIController::unregisterSubstitutionTexture(const wstring &textureName, bool deleteData)
{
	AUTO_VAR(it,m_substitutionTextures.find(textureName));

	if(it != m_substitutionTextures.end())
	{
		if(deleteData) delete [] it->second.data;
		m_substitutionTextures.erase(it);
	}
}

// NAVIGATION
bool UIController::NavigateToSceneImmediate(int iPad, EUIScene scene, void *initData, EUILayer layer, EUIGroup group)
{
	// if you're trying to navigate to the inventory,the crafting, pause or game info or any of the trigger scenes and there's already a menu up (because you were pressing a few buttons at the same time) then ignore the navigate
	if(GetMenuDisplayed(iPad))
	{
		switch(scene)
		{
		case eUIScene_PauseMenu:
		case eUIScene_Crafting2x2Menu:
		case eUIScene_Crafting3x3Menu:
		case eUIScene_FurnaceMenu:
		case eUIScene_ContainerMenu:
		case eUIScene_LargeContainerMenu:
		case eUIScene_InventoryMenu:
		case eUIScene_CreativeMenu:
		case eUIScene_DispenserMenu:
		case eUIScene_SignEntryMenu:
		case eUIScene_InGameInfoMenu:
		case eUIScene_EnchantingMenu:
		case eUIScene_BrewingStandMenu:
		case eUIScene_AnvilMenu:
		case eUIScene_TradingMenu:
			app.DebugPrintf("IGNORING NAVIGATE - we're trying to navigate to a user selected scene when there's already a scene up: pad:%d, scene:%d\n", iPad, scene);
			return false;
			break;
		}
	}

	switch(scene)
	{
	case eUIScene_FullscreenProgress:
		{
			// 4J Stu - The fullscreen progress scene should not interfere with any other scene stack, so should be placed in it's own group/layer
			layer = eUILayer_Fullscreen;
			group = eUIGroup_Fullscreen;
		}
		break;
	case eUIScene_ConnectingProgress:
		{
			// The connecting progress scene shouldn't interfere with other scenes
			layer = eUILayer_Fullscreen;
		}
		break;
	case eUIScene_EndPoem:
		{
			// The end poem scene shouldn't interfere with other scenes, but will be underneath the autosave progress
			group = eUIGroup_Fullscreen;
			layer = eUILayer_Scene;
		}
		break;
	};
	int menuDisplayedPad = XUSER_INDEX_ANY;
	if(group == eUIGroup_PAD)
	{
		if( app.GetGameStarted() )
		{
			// If the game isn't running treat as user 0, otherwise map index directly from pad
			if( ( iPad != 255 ) && ( iPad >= 0 ) )
			{
				menuDisplayedPad = iPad;
				group = (EUIGroup)(iPad+1);
			}
			else group = eUIGroup_Fullscreen;
		}
		else
		{
			layer = eUILayer_Fullscreen;
			group = eUIGroup_Fullscreen;
		}
	}

	PerformanceTimer timer;

	EnterCriticalSection(&m_navigationLock);
	SetMenuDisplayed(menuDisplayedPad,true);
	bool success = m_groups[(int)group]->NavigateToScene(iPad, scene, initData, layer);
	if(success && group == eUIGroup_Fullscreen) setFullscreenMenuDisplayed(true);
	LeaveCriticalSection(&m_navigationLock);

#ifdef _WIN64
	ResetMouseHoverState(true);
#endif
	PushDebugBreadcrumb("nav to apply pad=%d scene=%d layer=%d group=%d success=%d", iPad, scene, layer, group, success ? 1 : 0);

	timer.PrintElapsedTime(L"Navigate to scene");

	return success;
	//return true;
}

bool UIController::NavigateToScene(int iPad, EUIScene scene, void *initData, EUILayer layer, EUIGroup group)
{
	if(m_inputDispatchDepth > 0)
	{
		PendingNavigationRequest request;
		request.type = PendingNavigationRequest::eNavigateTo;
		request.iPad = iPad;
		request.scene = scene;
		request.initData = initData;
		request.layer = layer;
		request.group = group;
		request.forceUsePad = false;
		request.forceIPad = false;
		m_pendingNavigation.push_back(request);
		PushDebugBreadcrumb("nav to queued pad=%d scene=%d layer=%d group=%d", iPad, scene, layer, group);
		return true;
	}

	return NavigateToSceneImmediate(iPad, scene, initData, layer, group);
}

bool UIController::NavigateBackImmediate(int iPad, bool forceUsePad, EUIScene eScene, EUILayer eLayer)
{
	bool navComplete = false;
	if( app.GetGameStarted() )
	{
		navComplete = m_groups[(int)eUIGroup_Fullscreen]->NavigateBack(iPad, eScene, eLayer);

		if(!navComplete && ( iPad != 255 ) && ( iPad >= 0 ) )
		{
			EUIGroup group = (EUIGroup)(iPad+1);
			navComplete = m_groups[(int)group]->NavigateBack(iPad, eScene, eLayer);
			if(!m_groups[(int)group]->GetMenuDisplayed())SetMenuDisplayed(iPad,false);
		}
		// 4J-PB - autosave in fullscreen doesn't clear the menuDisplayed flag
		else
		{
			if(!m_groups[(int)eUIGroup_Fullscreen]->GetMenuDisplayed())
			{
				setFullscreenMenuDisplayed(false);
				for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
				{
					SetMenuDisplayed(i,m_groups[i+1]->GetMenuDisplayed());
				}
			}
		}
	}
	else
	{
		navComplete = m_groups[(int)eUIGroup_Fullscreen]->NavigateBack(iPad, eScene, eLayer);
		if(!m_groups[(int)eUIGroup_Fullscreen]->GetMenuDisplayed()) SetMenuDisplayed(XUSER_INDEX_ANY,false);
	}
	if(!navComplete)
	{
		app.DebugPrintf("UIController::NavigateBack failed for pad %d, target scene %d, layer %d\n", iPad, eScene, eLayer);
	}
#ifdef _WIN64
	ResetMouseHoverState(true);
#endif
	PushDebugBreadcrumb("nav back apply pad=%d scene=%d layer=%d success=%d", iPad, eScene, eLayer, navComplete ? 1 : 0);
	return navComplete;
}

bool UIController::NavigateBack(int iPad, bool forceUsePad, EUIScene eScene, EUILayer eLayer)
{
	if(m_inputDispatchDepth > 0)
	{
		PendingNavigationRequest request;
		request.type = PendingNavigationRequest::eNavigateBack;
		request.iPad = iPad;
		request.scene = eScene;
		request.initData = NULL;
		request.layer = eLayer;
		request.group = eUIGroup_PAD;
		request.forceUsePad = forceUsePad;
		request.forceIPad = false;
		m_pendingNavigation.push_back(request);
		PushDebugBreadcrumb("nav back queued pad=%d scene=%d layer=%d", iPad, eScene, eLayer);
		return true;
	}

	return NavigateBackImmediate(iPad, forceUsePad, eScene, eLayer);
}

void UIController::NavigateToHomeMenu()
{
	ui.CloseAllPlayersScenes();
	
	// Alert the app the we no longer want to be informed of ethernet connections
	app.SetLiveLinkRequired( false );

	Minecraft *pMinecraft = Minecraft::GetInstance();

	// 4J-PB - just about to switched to the default texture pack , so clean up anything texture pack related here

	// unload any texture pack audio
	// if there is audio in use, clear out the audio, and unmount the pack
	TexturePack *pTexPack=Minecraft::GetInstance()->skins->getSelected();


	DLCTexturePack *pDLCTexPack=NULL;
	if(pTexPack->hasAudio())
	{
		// get the dlc texture pack, and store it
		pDLCTexPack=(DLCTexturePack *)pTexPack;
	}

	// change to the default texture pack
	pMinecraft->skins->selectTexturePackById(TexturePackRepository::DEFAULT_TEXTURE_PACK_ID);


	if(pTexPack->hasAudio())
	{
		// need to stop the streaming audio - by playing streaming audio from the default texture pack now
		// reset the streaming sounds back to the normal ones
		pMinecraft->soundEngine->SetStreamingSounds(eStream_Overworld_Calm1,eStream_Overworld_piano3,
			eStream_Nether1,eStream_Nether4,
			eStream_end_dragon,eStream_end_end,
			eStream_CD_1);
		pMinecraft->soundEngine->playStreaming(L"", 0, 0, 0, 1, 1);

		// 		if(pDLCTexPack->m_pStreamedWaveBank!=NULL)
		// 		{
		// 			pDLCTexPack->m_pStreamedWaveBank->Destroy();
		// 		}
		// 		if(pDLCTexPack->m_pSoundBank!=NULL)
		// 		{
		// 			pDLCTexPack->m_pSoundBank->Destroy();
		// 		}
#ifdef _XBOX_ONE
		DWORD result = StorageManager.UnmountInstalledDLC(L"TPACK");
#else
		DWORD result = StorageManager.UnmountInstalledDLC("TPACK");
#endif

		app.DebugPrintf("Unmount result is %d\n",result);
	}

	g_NetworkManager.ForceFriendsSessionRefresh();

	if(pMinecraft->skins->needsUIUpdate())
	{
		m_navigateToHomeOnReload = true;
	}
	else
	{
		ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_MainMenu);
	}
}

UIScene *UIController::GetTopScene(int iPad, EUILayer layer, EUIGroup group)
{
	if(group == eUIGroup_PAD)
	{
		if( app.GetGameStarted() )
		{
			// If the game isn't running treat as user 0, otherwise map index directly from pad
			if( ( iPad != 255 ) && ( iPad >= 0 ) )
			{
				group = (EUIGroup)(iPad+1);
			}
			else group = eUIGroup_Fullscreen;
		}
		else
		{
			layer = eUILayer_Fullscreen;
			group = eUIGroup_Fullscreen;
		}
	}
	return m_groups[(int)group]->GetTopScene(layer);
}

size_t UIController::RegisterForCallbackId(UIScene *scene)
{
	EnterCriticalSection(&m_registeredCallbackScenesCS);
	size_t newId = GetTickCount();
	newId &= 0xFFFFFF; // Chop off the top byte, we don't need any more accuracy than that
	newId |= (scene->getSceneType() << 24); // Add in the scene's type to help keep this unique
	m_registeredCallbackScenes[newId] = scene;
	LeaveCriticalSection(&m_registeredCallbackScenesCS);
	return newId;
}

void UIController::UnregisterCallbackId(size_t id)
{
	EnterCriticalSection(&m_registeredCallbackScenesCS);
	AUTO_VAR(it, m_registeredCallbackScenes.find(id) );
	if(it != m_registeredCallbackScenes.end() )
	{
		m_registeredCallbackScenes.erase(it);
	}
	LeaveCriticalSection(&m_registeredCallbackScenesCS);
}

UIScene *UIController::GetSceneFromCallbackId(size_t id)
{
	UIScene *scene = NULL;
	AUTO_VAR(it, m_registeredCallbackScenes.find(id) );
	if(it != m_registeredCallbackScenes.end() )
	{
		scene = it->second;
	}
	return scene;
}

void UIController::EnterCallbackIdCriticalSection()
{
	EnterCriticalSection(&m_registeredCallbackScenesCS);
}

void UIController::LeaveCallbackIdCriticalSection()
{
	LeaveCriticalSection(&m_registeredCallbackScenesCS);
}

void UIController::CloseAllPlayersScenes()
{
	m_groups[(int)eUIGroup_Fullscreen]->getTooltips()->SetTooltips(-1);
	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		//m_bCloseAllScenes[i] = true;
		m_groups[i]->closeAllScenes();
		m_groups[i]->getTooltips()->SetTooltips(-1);
	}

	if (!m_groups[eUIGroup_Fullscreen]->GetMenuDisplayed()) {
		for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			SetMenuDisplayed(i,false);
		}
	}
	setFullscreenMenuDisplayed(false);
}

void UIController::CloseUIScenesImmediate(int iPad, bool forceIPad)
{
	EUIGroup group;
	if( app.GetGameStarted() || forceIPad )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}

	m_groups[(int)group]->closeAllScenes();
	m_groups[(int)group]->getTooltips()->SetTooltips(-1);
	
	// This should cause the popup to dissappear
	TutorialPopupInfo popupInfo;
	if(m_groups[(int)group]->getTutorialPopup()) m_groups[(int)group]->getTutorialPopup()->SetTutorialDescription(&popupInfo);

	if(group==eUIGroup_Fullscreen) setFullscreenMenuDisplayed(false);

	SetMenuDisplayed((group == eUIGroup_Fullscreen ? XUSER_INDEX_ANY : iPad), m_groups[(int)group]->GetMenuDisplayed());
#ifdef _WIN64
	ResetMouseHoverState(true);
#endif
	PushDebugBreadcrumb("close scenes apply pad=%d force=%d group=%d", iPad, forceIPad ? 1 : 0, group);
}

void UIController::CloseUIScenes(int iPad, bool forceIPad)
{
	if(m_inputDispatchDepth > 0)
	{
		PendingNavigationRequest request;
		request.type = PendingNavigationRequest::eCloseScenes;
		request.iPad = iPad;
		request.scene = eUIScene_COUNT;
		request.initData = NULL;
		request.layer = eUILayer_COUNT;
		request.group = eUIGroup_PAD;
		request.forceUsePad = false;
		request.forceIPad = forceIPad;
		m_pendingNavigation.push_back(request);
		PushDebugBreadcrumb("close scenes queued pad=%d force=%d", iPad, forceIPad ? 1 : 0);
		return;
	}

	CloseUIScenesImmediate(iPad, forceIPad);
}

void UIController::setFullscreenMenuDisplayed(bool displayed)
{
	// Show/hide the tooltips for the fullscreen group
	m_groups[(int)eUIGroup_Fullscreen]->showComponent(ProfileManager.GetPrimaryPad(),eUIComponent_Tooltips,eUILayer_Tooltips,displayed);

	// Show/hide tooltips for the other layers
	for(unsigned int i = (eUIGroup_Fullscreen+1); i < eUIGroup_COUNT; ++i)
	{
		m_groups[i]->showComponent(i,eUIComponent_Tooltips,eUILayer_Tooltips,!displayed);
	}
}

bool UIController::IsPauseMenuDisplayed(int iPad)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	return m_groups[(int)group]->IsPauseMenuDisplayed();
}

bool UIController::IsContainerMenuDisplayed(int iPad)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	return m_groups[(int)group]->IsContainerMenuDisplayed();
}

bool UIController::IsIgnorePlayerJoinMenuDisplayed(int iPad)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	return m_groups[(int)group]->IsIgnorePlayerJoinMenuDisplayed();
}

bool UIController::IsIgnoreAutosaveMenuDisplayed(int iPad)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	return m_groups[(int)eUIGroup_Fullscreen]->IsIgnoreAutosaveMenuDisplayed() || (group != eUIGroup_Fullscreen && m_groups[(int)group]->IsIgnoreAutosaveMenuDisplayed());
}

void UIController::SetIgnoreAutosaveMenuDisplayed(int iPad, bool displayed)
{
	app.DebugPrintf(app.USER_SR, "UIController::SetIgnoreAutosaveMenuDisplayed is not implemented\n");
}

bool UIController::IsSceneInStack(int iPad, EUIScene eScene)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	return m_groups[(int)group]->IsSceneInStack(eScene);
}

bool UIController::GetMenuDisplayed(int iPad)
{
	return m_bMenuDisplayed[iPad];
}

void UIController::SetMenuDisplayed(int iPad,bool bVal)
{
	if(bVal)
	{
		if(iPad==XUSER_INDEX_ANY)
		{
			for(int i=0;i<XUSER_MAX_COUNT;i++)
			{
				InputManager.SetMenuDisplayed(i,true);
				m_bMenuDisplayed[i]=true;
				// 4J Stu - Fix for #11018 - Functional: When the controller is unplugged during active gameplay and plugged back in at the resulting pause menu, it will demonstrate dual-functionality.
				m_bMenuToBeClosed[i]=false;
			}
		}
		else
		{
			InputManager.SetMenuDisplayed(iPad,true);
			m_bMenuDisplayed[iPad]=true;
			// 4J Stu - Fix for #11018 - Functional: When the controller is unplugged during active gameplay and plugged back in at the resulting pause menu, it will demonstrate dual-functionality.
			m_bMenuToBeClosed[iPad]=false;
		}
	}
	else
	{
		if(iPad==XUSER_INDEX_ANY)
		{
			for(int i=0;i<XUSER_MAX_COUNT;i++)
			{
				m_bMenuToBeClosed[i]=true;
				m_iCountDown[i]=10;
			}
		}
		else
		{
			m_bMenuToBeClosed[iPad]=true;
			m_iCountDown[iPad]=10;

#ifdef _DURANGO
			// 4J-JEV: When in-game, allow player to toggle the 'Pause' and 'IngameInfo' menus via Kinnect.
			if (Minecraft::GetInstance()->running) 
				InputManager.SetEnabledGtcButtons(_360_GTC_MENU | _360_GTC_PAUSE | _360_GTC_VIEW);
#endif
		}
	}
}

void UIController::CheckMenuDisplayed()
{
	for(int iPad=0;iPad<XUSER_MAX_COUNT;iPad++)
	{
		if(m_bMenuToBeClosed[iPad])
		{
			if(m_iCountDown[iPad]!=0)
			{
				m_iCountDown[iPad]--;
			}
			else
			{
				m_bMenuToBeClosed[iPad]=false;
				m_bMenuDisplayed[iPad]=false;
				InputManager.SetMenuDisplayed(iPad,false);
			}

		}
	}
}

void UIController::SetTooltipText( unsigned int iPad, unsigned int tooltip, int iTextID )
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTooltips()) m_groups[(int)group]->getTooltips()->SetTooltipText(tooltip, iTextID);
}

void UIController::SetEnableTooltips( unsigned int iPad, BOOL bVal )
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTooltips()) m_groups[(int)group]->getTooltips()->SetEnableTooltips(bVal);
}

void UIController::ShowTooltip( unsigned int iPad, unsigned int tooltip, bool show )
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTooltips()) m_groups[(int)group]->getTooltips()->ShowTooltip(tooltip,show);
}

void UIController::SetTooltips( unsigned int iPad, int iA, int iB, int iX, int iY, int iLT, int iRT, int iLB, int iRB, int iLS, bool forceUpdate)
{
	EUIGroup group;

	// 4J-PB - strip out any that are not applicable on the platform
#ifndef _XBOX
	if(iX==IDS_TOOLTIPS_SELECTDEVICE) iX=-1;
	if(iX==IDS_TOOLTIPS_CHANGEDEVICE) iX=-1;

#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
	if(iY==IDS_TOOLTIPS_VIEW_GAMERCARD) iY=-1;
	if(iY==IDS_TOOLTIPS_VIEW_GAMERPROFILE) iY=-1;

#endif
#endif

	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTooltips()) m_groups[(int)group]->getTooltips()->SetTooltips(iA, iB, iX, iY, iLT, iRT, iLB, iRB, iLS, forceUpdate);
}

void UIController::EnableTooltip( unsigned int iPad, unsigned int tooltip, bool enable )
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTooltips()) m_groups[(int)group]->getTooltips()->EnableTooltip(tooltip,enable);
}

void UIController::RefreshTooltips(unsigned int iPad)
{
	app.DebugPrintf(app.USER_SR, "UIController::RefreshTooltips is not implemented\n");
}

void UIController::AnimateKeyPress(int iPad, int iAction, bool bRepeat, bool bPressed, bool bReleased)
{
	EUIGroup group;
	if(bPressed==false)
	{
		// only animating button press
		return;
	}
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	bool handled = false;
	if(m_groups[(int)group]->getTooltips()) m_groups[(int)group]->getTooltips()->handleInput(iPad, iAction, bRepeat, bPressed, bReleased, handled);
}

void UIController::OverrideSFX(int iPad, int iAction,bool bVal)
{
	EUIGroup group;

	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	bool handled = false;
	if(m_groups[(int)group]->getTooltips()) m_groups[(int)group]->getTooltips()->overrideSFX(iPad, iAction,bVal);
}

void UIController::PlayUISFX(ESoundEffect eSound)
{
	Minecraft::GetInstance()->soundEngine->playUI(eSound,1.0f,1.0f);
}

void UIController::DisplayGamertag(unsigned int iPad, bool show)
{
	// The host decides whether these are on or off
	if( app.GetGameSettings(ProfileManager.GetPrimaryPad(),eGameSetting_DisplaySplitscreenGamertags) == 0)
	{
		show = false;
	}
	EUIGroup group = (EUIGroup)(iPad+1);
	if(m_groups[(int)group]->getHUD()) m_groups[(int)group]->getHUD()->ShowDisplayName(show);

	// Update TutorialPopup in Splitscreen if no container is displayed (to make sure the Popup does not overlap with the Gamertag!)
	if(app.GetLocalPlayerCount() > 1 && m_groups[(int)group]->getTutorialPopup() && !m_groups[(int)group]->IsContainerMenuDisplayed())
	{
		m_groups[(int)group]->getTutorialPopup()->UpdateTutorialPopup();
	}
}

void UIController::SetSelectedItem(unsigned int iPad, const wstring &name)
{
	EUIGroup group;

	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	bool handled = false;
	if(m_groups[(int)group]->getHUD()) m_groups[(int)group]->getHUD()->SetSelectedLabel(name);
}

void UIController::UpdateSelectedItemPos(unsigned int iPad)
{
	app.DebugPrintf(app.USER_SR, "UIController::UpdateSelectedItemPos not implemented\n");
}

void UIController::HandleDLCMountingComplete()
{
	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		app.DebugPrintf("UIController::HandleDLCMountingComplete - m_groups[%d]\n",i);
		m_groups[i]->HandleDLCMountingComplete();
	}
}

void UIController::HandleDLCInstalled(int iPad)
{
	//app.DebugPrintf(app.USER_SR, "UIController::HandleDLCInstalled not implemented\n");
	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		m_groups[i]->HandleDLCInstalled();
	}
}


#ifdef _XBOX_ONE
void UIController::HandleDLCLicenseChange()
{
	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		app.DebugPrintf("UIController::HandleDLCLicenseChange - m_groups[%d]\n",i);
		m_groups[i]->HandleDLCLicenseChange();
	}
}
#endif

void UIController::HandleTMSDLCFileRetrieved(int iPad)
{
	app.DebugPrintf(app.USER_SR, "UIController::HandleTMSDLCFileRetrieved not implemented\n");
}

void UIController::HandleTMSBanFileRetrieved(int iPad)
{
	app.DebugPrintf(app.USER_SR, "UIController::HandleTMSBanFileRetrieved not implemented\n");
}

void UIController::HandleInventoryUpdated(int iPad)
{
	app.DebugPrintf(app.USER_SR, "UIController::HandleInventoryUpdated not implemented\n");
}

void UIController::HandleGameTick()
{
	tickInput();

	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		if(m_groups[i]->getHUD()) m_groups[i]->getHUD()->handleGameTick();
	}
}

void UIController::SetTutorial(int iPad, Tutorial *tutorial)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTutorialPopup()) m_groups[(int)group]->getTutorialPopup()->SetTutorial(tutorial);
}

void UIController::SetTutorialDescription(int iPad, TutorialPopupInfo *info)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}

	if(m_groups[(int)group]->getTutorialPopup())
	{
		// tutorial popup needs to know if a container menu is being displayed
		m_groups[(int)group]->getTutorialPopup()->SetContainerMenuVisible(m_groups[(int)group]->IsContainerMenuDisplayed());
		m_groups[(int)group]->getTutorialPopup()->SetTutorialDescription(info);
	}
}

#ifndef _XBOX
void UIController::RemoveInteractSceneReference(int iPad, UIScene *scene)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTutorialPopup()) m_groups[(int)group]->getTutorialPopup()->RemoveInteractSceneReference(scene);
}
#endif

void UIController::SetTutorialVisible(int iPad, bool visible)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	if(m_groups[(int)group]->getTutorialPopup()) m_groups[(int)group]->getTutorialPopup()->SetVisible(visible);
}

bool UIController::IsTutorialVisible(int iPad)
{
	EUIGroup group;
	if( app.GetGameStarted() )
	{
		// If the game isn't running treat as user 0, otherwise map index directly from pad
		if( ( iPad != 255 ) && ( iPad >= 0 ) ) group = (EUIGroup)(iPad+1);
		else group = eUIGroup_Fullscreen;
	}
	else
	{
		group = eUIGroup_Fullscreen;
	}
	bool visible = false;
	if(m_groups[(int)group]->getTutorialPopup()) visible = m_groups[(int)group]->getTutorialPopup()->IsVisible();
	return visible;
}

void UIController::UpdatePlayerBasePositions()
{
	Minecraft *pMinecraft = Minecraft::GetInstance();

	for( BYTE idx = 0; idx < XUSER_MAX_COUNT; ++idx)
	{
		if(pMinecraft->localplayers[idx] != NULL)
		{
			if(pMinecraft->localplayers[idx]->m_iScreenSection==C4JRender::VIEWPORT_TYPE_FULLSCREEN)
			{
				DisplayGamertag(idx,false);
			}
			else
			{
				DisplayGamertag(idx,true);
			}
			m_groups[idx+1]->SetViewportType((C4JRender::eViewportType)pMinecraft->localplayers[idx]->m_iScreenSection);
		}
		else
		{
			// 4J Stu - This is a legacy thing from our XUI implementation that we don't need
			// Changing the viewport to fullscreen for users that no longer exist is SLOW
			// This should probably be on all platforms, but I don't have time to test them all just now!
#ifndef __ORBIS__
			m_groups[idx+1]->SetViewportType(C4JRender::VIEWPORT_TYPE_FULLSCREEN);
#endif
			DisplayGamertag(idx,false);
		}
	}
}

void UIController::SetEmptyQuadrantLogo(int iSection)
{
	// 4J Stu - We shouldn't need to implement this
}

void UIController::HideAllGameUIElements()
{
	// 4J Stu - We might not need to implement this
	app.DebugPrintf(app.USER_SR, "UIController::HideAllGameUIElements not implemented\n");
}

void UIController::ShowOtherPlayersBaseScene(unsigned int iPad, bool show)
{
	// 4J Stu - We shouldn't need to implement this
}

void UIController::ShowTrialTimer(bool show)
{
	if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->showTrialTimer(show);
}

void UIController::SetTrialTimerLimitSecs(unsigned int uiSeconds)
{
	UIController::m_dwTrialTimerLimitSecs = uiSeconds;
}

void UIController::UpdateTrialTimer(unsigned int iPad)
{
	WCHAR wcTime[20]; 

	DWORD dwTimeTicks=(DWORD)app.getTrialTimer();

	if(dwTimeTicks>m_dwTrialTimerLimitSecs)
	{
		dwTimeTicks=m_dwTrialTimerLimitSecs;
	}

	dwTimeTicks=m_dwTrialTimerLimitSecs-dwTimeTicks;

#ifndef _CONTENT_PACKAGE
	if(true)
#else
	// display the time - only if there's less than 3 minutes
	if(dwTimeTicks<180)
#endif
	{
		int iMins=dwTimeTicks/60;
		int iSeconds=dwTimeTicks%60;
		swprintf( wcTime, 20, L"%d:%02d",iMins,iSeconds);
		if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->setTrialTimer(wcTime);
	}
	else
	{
		if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->setTrialTimer(L"");
	}

	// are we out of time?
	if((dwTimeTicks==0))
	{
		// Trial over
		// bring up the pause menu to stop the trial over message box being called again?
		if(!ui.GetMenuDisplayed( iPad ) )
		{
			ui.NavigateToScene(iPad, eUIScene_PauseMenu, NULL, eUILayer_Scene);

			app.SetAction(iPad,eAppAction_TrialOver);
		}
	}
}

void UIController::ReduceTrialTimerValue()
{
	DWORD dwTimeTicks=(int)app.getTrialTimer();

	if(dwTimeTicks>m_dwTrialTimerLimitSecs)
	{
		dwTimeTicks=m_dwTrialTimerLimitSecs;
	}

	m_dwTrialTimerLimitSecs-=dwTimeTicks;
}

void UIController::ShowAutosaveCountdownTimer(bool show)
{
	if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->showTrialTimer(show);
}

void UIController::UpdateAutosaveCountdownTimer(unsigned int uiSeconds)
{
#if !(defined(_XBOX_ONE) || defined(__ORBIS__))
	WCHAR wcAutosaveCountdown[100]; 
	swprintf( wcAutosaveCountdown, 100, app.GetString(IDS_AUTOSAVE_COUNTDOWN),uiSeconds);
	if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->setTrialTimer(wcAutosaveCountdown);
#endif
}

void UIController::ShowSavingMessage(unsigned int iPad, C4JStorage::ESavingMessage eVal)
{
	bool show = false;
	switch(eVal)
	{
	case C4JStorage::ESavingMessage_None:
		show = false;
		break;
	case C4JStorage::ESavingMessage_Short:
	case C4JStorage::ESavingMessage_Long:
		show = true;
		break;
	}
	if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->showSaveIcon(show);
}

void UIController::ShowPlayerDisplayname(bool show)
{
	if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->showPlayerDisplayName(show);
}

void UIController::SetWinUserIndex(unsigned int iPad)
{
	m_winUserIndex = iPad;
}

unsigned int UIController::GetWinUserIndex()
{
	return m_winUserIndex;
}

void UIController::ShowUIDebugConsole(bool show)
{
#ifndef _CONTENT_PACKAGE

	if(show)
	{
		m_uiDebugConsole = (UIComponent_DebugUIConsole *)m_groups[eUIGroup_Fullscreen]->addComponent(0, eUIComponent_DebugUIConsole, eUILayer_Debug);
	}
	else
	{
		m_groups[eUIGroup_Fullscreen]->removeComponent(eUIComponent_DebugUIConsole, eUILayer_Debug);
		m_uiDebugConsole = NULL;
	}
#endif
}

void UIController::ShowUIDebugMarketingGuide(bool show)
{
#ifndef _CONTENT_PACKAGE

	if(show)
	{
		m_uiDebugMarketingGuide = (UIComponent_DebugUIMarketingGuide *)m_groups[eUIGroup_Fullscreen]->addComponent(0, eUIComponent_DebugUIMarketingGuide, eUILayer_Debug);
	}
	else
	{
		m_groups[eUIGroup_Fullscreen]->removeComponent(eUIComponent_DebugUIMarketingGuide, eUILayer_Debug);
		m_uiDebugMarketingGuide = NULL;
	}
#endif
}

void UIController::logDebugString(const string &text)
{
	if(m_uiDebugConsole) m_uiDebugConsole->addText(text);
}

bool UIController::PressStartPlaying(unsigned int iPad)
{
	return m_iPressStartQuadrantsMask&(1<<iPad)?true:false;
}

void UIController::ShowPressStart(unsigned int iPad)
{
	m_iPressStartQuadrantsMask|=1<<iPad;
	if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->showPressStart(iPad, true);
}

void UIController::HidePressStart()
{
	ClearPressStart();
	if(m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()) m_groups[(int)eUIGroup_Fullscreen]->getPressStartToPlay()->showPressStart(0, false);
}

void UIController::ClearPressStart()
{
	m_iPressStartQuadrantsMask = 0;
}

// 4J Stu - For the different StringTable classes. Should really fix the libraries.
#ifndef __PS3__
C4JStorage::EMessageResult UIController::RequestMessageBox(UINT uiTitle, UINT uiText, UINT *uiOptionA,UINT uiOptionC, DWORD dwPad,
														   int( *Func)(LPVOID,int,const C4JStorage::EMessageResult),LPVOID lpParam, C4JStringTable *pStringTable, WCHAR *pwchFormatString,DWORD dwFocusButton, bool bIsError)
#else
C4JStorage::EMessageResult UIController::RequestMessageBox(UINT uiTitle, UINT uiText, UINT *uiOptionA,UINT uiOptionC, DWORD dwPad,
														   int( *Func)(LPVOID,int,const C4JStorage::EMessageResult),LPVOID lpParam, StringTable *pStringTable, WCHAR *pwchFormatString,DWORD dwFocusButton, bool bIsError)
#endif
{
	MessageBoxInfo param;
	param.uiTitle = uiTitle;
	param.uiText = uiText;
	param.uiOptionA = uiOptionA;
	param.uiOptionC = uiOptionC;
	param.dwPad = dwPad;
	param.Func = Func;\
	param.lpParam = lpParam;
	param.pwchFormatString = pwchFormatString;
	param.dwFocusButton = dwFocusButton;

	EUILayer layer = bIsError?eUILayer_Error:eUILayer_Alert;

	bool completed = false;
	if(ui.IsReloadingSkin())
	{
		// Queue this message box
		QueuedMessageBoxData *queuedData = new QueuedMessageBoxData();
		queuedData->info = param;
		queuedData->info.uiOptionA = new UINT[param.uiOptionC];
		memcpy(queuedData->info.uiOptionA, param.uiOptionA, param.uiOptionC * sizeof(UINT));
		queuedData->iPad = dwPad;
		queuedData->layer = eUILayer_Error; // Ensures that these don't get wiped out by a CloseAllScenes call
		m_queuedMessageBoxData.push_back(queuedData);
	}
	else
	{
		completed = ui.NavigateToScene(dwPad, eUIScene_MessageBox, &param, layer, eUIGroup_Fullscreen);
	}

	if( completed )
	{
		// This may happen if we had to queue the message box, or there was already a message box displaying and so the NavigateToScene returned false;
		return C4JStorage::EMessage_Pending;
	}
	else
	{
		return C4JStorage::EMessage_Busy;
	}
}

C4JStorage::EMessageResult UIController::RequestUGCMessageBox(UINT title/* = -1 */, UINT message/* = -1 */, int iPad/* = -1*/, int( *Func)(LPVOID,int,const C4JStorage::EMessageResult)/* = NULL*/, LPVOID lpParam/* = NULL*/)
{
	// Default title / messages
	if (title == -1)
	{
		title = IDS_FAILED_TO_CREATE_GAME_TITLE;
	}

	if (message == -1)
	{
		message = IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_CREATE;
	}

	// Default pad to primary player
	if (iPad == -1) iPad = ProfileManager.GetPrimaryPad();

#ifdef __ORBIS__
	// Show the vague UGC system message in addition to our message
	ProfileManager.DisplaySystemMessage( SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_UGC_RESTRICTION, iPad );
	return C4JStorage::EMessage_ResultAccept; 
#elif defined(__PSVITA__)
	ProfileManager.ShowSystemMessage( SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_CHAT_RESTRICTION, iPad );
	UINT uiIDA[1];
	uiIDA[0]=IDS_CONFIRM_OK;
	return ui.RequestMessageBox( title, IDS_CHAT_RESTRICTION_UGC, uiIDA, 1, iPad, Func, lpParam, app.GetStringTable(), NULL, 0, false);
#else
	UINT uiIDA[1];
	uiIDA[0]=IDS_CONFIRM_OK;
	return ui.RequestMessageBox( title, message, uiIDA, 1, iPad, Func, lpParam, app.GetStringTable(), NULL, 0, false);
#endif
}

C4JStorage::EMessageResult UIController::RequestContentRestrictedMessageBox(UINT title/* = -1 */, UINT message/* = -1 */, int iPad/* = -1*/, int( *Func)(LPVOID,int,const C4JStorage::EMessageResult)/* = NULL*/, LPVOID lpParam/* = NULL*/)
{
	// Default title / messages
	if (title == -1)
	{
		title = IDS_FAILED_TO_CREATE_GAME_TITLE;
	}

	if (message == -1)
	{
#if defined(_XBOX_ONE) || defined(_WINDOWS64)
		// IDS_CONTENT_RESTRICTION doesn't exist on XB1
		message = IDS_NO_USER_CREATED_CONTENT_PRIVILEGE_CREATE;
#else
		message = IDS_CONTENT_RESTRICTION;
#endif
	}

	// Default pad to primary player
	if (iPad == -1) iPad = ProfileManager.GetPrimaryPad();

#ifdef __ORBIS__
	// Show the vague UGC system message in addition to our message
	ProfileManager.DisplaySystemMessage( SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_UGC_RESTRICTION, iPad );
	return C4JStorage::EMessage_ResultAccept; 
#elif defined(__PSVITA__)
	ProfileManager.ShowSystemMessage( SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_AGE_RESTRICTION, iPad );
	return C4JStorage::EMessage_ResultAccept;
#else
	UINT uiIDA[1];
	uiIDA[0]=IDS_CONFIRM_OK;
	return ui.RequestMessageBox( title, message, uiIDA, 1, iPad, Func, lpParam, app.GetStringTable(), NULL, 0, false);
#endif
}

void UIController::setFontCachingCalculationBuffer(int length)
{
	/* 4J-JEV: As described in an email from Sean.
	If your `optional_temp_buffer` is NULL, Iggy will allocate the temp
	buffer on the stack during Iggy draw calls. The size of the buffer it
	will allocate is 16 bytes times `max_chars` in 32-bit, and 24 bytes
	times `max_chars` in 64-bit. If the stack of the thread making the
	draw call is not large enough, Iggy will crash or otherwise behave
	incorrectly.
	*/
#if defined __ORBIS__ || defined _DURANGO || defined _WIN64
	static const int CHAR_SIZE = 24;
#else
	static const int CHAR_SIZE = 16;
#endif

	if (m_tempBuffer != NULL) delete [] m_tempBuffer;
	if (length<0)
	{
		if (m_defaultBuffer == NULL) m_defaultBuffer = new char[CHAR_SIZE*5000];
		IggySetFontCachingCalculationBuffer(5000, m_defaultBuffer, CHAR_SIZE*5000);
	}
	else
	{
		m_tempBuffer = new char[CHAR_SIZE*length];
		IggySetFontCachingCalculationBuffer(length, m_tempBuffer, CHAR_SIZE*length);
	}
}

// Returns the first scene of given type if it exists, NULL otherwise 
UIScene *UIController::FindScene(EUIScene sceneType)
{
	UIScene *pScene = NULL;

	for (int i = 0; i < eUIGroup_COUNT; i++)
	{
		pScene = m_groups[i]->FindScene(sceneType);
#ifdef __PS3__
		if (pScene != NULL) return pScene;
#else
		if (pScene != nullptr) return pScene;
#endif
	}

	return pScene;
}

#ifdef _WIN64
bool UIController::IsMouseOverClickableControl()
{
	return m_bMouseOverClickable;
}

void UIController::ResetMouseFocusState()
{
	m_pMouseFocusScene = NULL;
	m_iMouseFocusControlId = -1;
}

void UIController::ResetMouseActiveState()
{
	m_pMouseActiveScene = NULL;
	m_iMouseActiveControlId = -1;
	m_bMouseDragActive = false;
}

void UIController::ResetMouseHoverState(bool resetLastMousePos)
{
	m_bMouseOverClickable = false;
	m_pMouseHoverScene = NULL;
	m_iMouseHoverControlId = -1;
	if (resetLastMousePos)
	{
		m_lastMouseX = -1;
		m_lastMouseY = -1;
	}
}

void UIController::CommitMouseFocus(UIScene *pScene, UIControl *pControl)
{
	if (pScene == NULL || pControl == NULL)
	{
		return;
	}

	if (m_pMouseFocusScene == pScene && m_iMouseFocusControlId == pControl->getId())
	{
		return;
	}

	m_pMouseFocusScene = pScene;
	m_iMouseFocusControlId = pControl->getId();
	PushDebugBreadcrumb("mouse focus scene=%d control=%d", (int)pScene->getSceneType(), m_iMouseFocusControlId);
	pScene->SetFocusToElement(m_iMouseFocusControlId);
}

UIScene *UIController::FindTopMouseScene()
{
	for (int groupIdx = eUIGroup_Fullscreen; groupIdx < eUIGroup_COUNT; ++groupIdx)
	{
		UIGroup* pGroup = m_groups[groupIdx];
		if (!pGroup || !pGroup->GetMenuDisplayed())
		{
			continue;
		}
		for (int layerIdx = eUILayer_COUNT - 1; layerIdx >= 0; --layerIdx)
		{
			UIScene* pScene = pGroup->GetTopScene((EUILayer)layerIdx);
			if (pScene && pScene->hasMovie() && pScene->stealsFocus())
			{
				return pScene;
			}
		}
	}

	return NULL;
}

UIControl *UIController::FindSceneControlById(UIScene *pScene, int controlId)
{
	if (pScene == NULL || controlId < 0)
	{
		return NULL;
	}

	vector<UIControl*> *pControls = pScene->GetControls();
	if (pControls == NULL)
	{
		return NULL;
	}

	for (size_t i = 0; i < pControls->size(); ++i)
	{
		UIControl *pControl = (*pControls)[i];
		if (pControl != NULL && pControl->getId() == controlId)
		{
			return pControl;
		}
	}

	return NULL;
}

bool UIController::ResolveMouseControlBounds(UIScene *pScene, UIControl *pControl, S32 &x1, S32 &y1, S32 &x2, S32 &y2)
{
	if (pScene == NULL || pControl == NULL || !pControl->getVisible())
	{
		return false;
	}

	S32 ctrlX = 0;
	S32 ctrlY = 0;
	pControl->getAbsolutePos(ctrlX, ctrlY);

	S32 ctrlW = pControl->getWidth();
	S32 ctrlH = pControl->getHeight();
	UIControl::eUIControlType ctrlType = pControl->getControlType();

	if (ctrlType == UIControl::eSlider)
	{
		ctrlW = ((UIControl_Slider *)pControl)->GetRealWidth();
	}
	else if (ctrlType == UIControl::eTexturePackList)
	{
		ctrlH = ((UIControl_TexturePackList *)pControl)->GetRealHeight();
	}
	else if (ctrlType == UIControl::eDynamicLabel)
	{
		UIControl_DynamicLabel *pDynamicLabel = (UIControl_DynamicLabel *)pControl;
		ctrlW = pDynamicLabel->GetRealWidth();
		ctrlH = pDynamicLabel->GetRealHeight();
	}
	else if (ctrlType == UIControl::eHTMLLabel)
	{
		UIControl_HTMLLabel *pHtmlLabel = (UIControl_HTMLLabel *)pControl;
		ctrlW = pHtmlLabel->GetRealWidth();
		ctrlH = pHtmlLabel->GetRealHeight();
	}

	if (ctrlW <= 0 || ctrlH <= 0)
	{
		return false;
	}

	x1 = ctrlX;
	y1 = ctrlY;
	x2 = ctrlX + ctrlW;
	y2 = ctrlY + ctrlH;
	return true;
}

void UIController::HandleMouseControlHover(UIScene *pScene, UIControl *pControl, S32 uiMouseX, S32 uiMouseY, bool leftDown, bool commitFocus)
{
	if (pScene == NULL || pControl == NULL)
	{
		return;
	}

	S32 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	if (!ResolveMouseControlBounds(pScene, pControl, x1, y1, x2, y2))
	{
		return;
	}

	S32 relX = uiMouseX - x1;
	S32 relY = uiMouseY - y1;
	if (pScene->getSceneType() == eUIScene_SettingsKeyBindMenu &&
		pControl->getControlType() == UIControl::eButton)
	{
		((UIScene_SettingsKeyBindMenu *)pScene)->SyncMouseHoverToVisibleSlot(pControl->getId());
	}

	switch (pControl->getControlType())
	{
	case UIControl::eButtonList:
	case UIControl::eSaveList:
	case UIControl::ePlayerList:
	case UIControl::eDLCList:
		if (commitFocus)
		{
			CommitMouseFocus(pScene, pControl);
		}
		((UIControl_ButtonList *)pControl)->SetTouchFocus(uiMouseX, uiMouseY, leftDown);
		break;
	case UIControl::eTexturePackList:
		if (commitFocus)
		{
			CommitMouseFocus(pScene, pControl);
		}
		((UIControl_TexturePackList *)pControl)->SetTouchFocus(relX, relY, leftDown);
		break;
	case UIControl::eLeaderboardList:
		if (commitFocus)
		{
			CommitMouseFocus(pScene, pControl);
		}
		((UIControl_LeaderboardList *)pControl)->SetTouchFocus(uiMouseX, uiMouseY, leftDown);
		break;
	case UIControl::eSlider:
		if (commitFocus || leftDown)
		{
			CommitMouseFocus(pScene, pControl);
		}
		if (leftDown)
		{
			float relativePos = (float)relX / (float)(x2 - x1);
			if (relativePos < 0.0f) relativePos = 0.0f;
			if (relativePos > 1.0f) relativePos = 1.0f;
			((UIControl_Slider *)pControl)->SetSliderTouchPos(relativePos);
		}
		break;
	case UIControl::eButton:
	case UIControl::eCheckBox:
	case UIControl::eTextInput:
	case UIControl::ePlayerSkinPreview:
	case UIControl::eEnchantmentButton:
	case UIControl::eTouchControl:
		if (commitFocus)
		{
			CommitMouseFocus(pScene, pControl);
		}
		break;
	default:
		break;
	}
}

void UIController::HandleMouseControlDrag(UIScene *pScene, UIControl *pControl, S32 uiMouseX, S32 uiMouseY, bool leftDown)
{
	if (pScene == NULL || pControl == NULL)
	{
		return;
	}

	switch (pControl->getControlType())
	{
	case UIControl::eDynamicLabel:
		((UIControl_DynamicLabel *)pControl)->TouchScroll(uiMouseY, leftDown);
		break;
	case UIControl::eHTMLLabel:
		((UIControl_HTMLLabel *)pControl)->TouchScroll(uiMouseY, leftDown);
		break;
	case UIControl::eTouchControl:
		pScene->handleTouchInput(0, uiMouseX, uiMouseY, pControl->getId(), false, leftDown, false);
		break;
	default:
		HandleMouseControlHover(pScene, pControl, uiMouseX, uiMouseY, leftDown, false);
		break;
	}
}

bool UIController::HandleMouseControlRelease(UIScene *pScene, UIControl *pControl, UIControl *pHoveredControl, S32 uiMouseX, S32 uiMouseY)
{
	if (pScene == NULL || pControl == NULL)
	{
		return false;
	}

	S32 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	ResolveMouseControlBounds(pScene, pControl, x1, y1, x2, y2);
	S32 relX = uiMouseX - x1;
	S32 relY = uiMouseY - y1;
	bool activate = (pHoveredControl != NULL && pHoveredControl == pControl);

	switch (pControl->getControlType())
	{
	case UIControl::eButton:
	case UIControl::eTextInput:
	case UIControl::ePlayerSkinPreview:
	case UIControl::eEnchantmentButton:
		if (activate)
		{
			QueueMouseActivate(pScene, pControl, 0.0);
			return true;
		}
		break;
	case UIControl::eCheckBox:
		if (activate)
		{
			UIControl_CheckBox *pCheckBox = (UIControl_CheckBox *)pControl;
			CommitMouseFocus(pScene, pControl);
			pCheckBox->TouchSetCheckbox(!pCheckBox->IsChecked());
		}
		break;
	case UIControl::eButtonList:
	case UIControl::eSaveList:
	case UIControl::ePlayerList:
	case UIControl::eDLCList:
		if (activate && ((UIControl_ButtonList *)pControl)->CanTouchTrigger(uiMouseX, uiMouseY))
		{
			QueueMouseActivate(pScene, pControl, (F64)((UIControl_ButtonList *)pControl)->getCurrentSelection());
			return true;
		}
		break;
	case UIControl::eTexturePackList:
		if (activate && ((UIControl_TexturePackList *)pControl)->CanTouchTrigger(relX, relY))
		{
			QueueMouseActivate(pScene, pControl, (F64)pScene->getControlChildFocus());
			return true;
		}
		break;
	case UIControl::eDynamicLabel:
		((UIControl_DynamicLabel *)pControl)->TouchScroll(uiMouseY, false);
		break;
	case UIControl::eHTMLLabel:
		((UIControl_HTMLLabel *)pControl)->TouchScroll(uiMouseY, false);
		break;
	case UIControl::eTouchControl:
		if (activate)
		{
			pScene->handleTouchInput(0, uiMouseX, uiMouseY, pControl->getId(), false, false, true);
		}
		break;
	default:
		break;
	}

	return false;
}

void UIController::QueueMouseActivate(UIScene *pScene, UIControl *pControl, F64 childId)
{
	if (pScene == NULL || pControl == NULL)
	{
		return;
	}

	PendingMouseAction action;
	action.type = PendingMouseAction::eActivateControl;
	action.callbackId = RegisterForCallbackId(pScene);
	action.controlId = (F64)pControl->getId();
	action.childId = childId;
	action.key = 0;
	m_pendingMouseActions.push_back(action);
	PushDebugBreadcrumb("mouse queue activate scene=%d control=%d child=%d",
		(int)pScene->getSceneType(), pControl->getId(), (int)childId);
}

void UIController::QueueMouseCancel(UIScene *pScene)
{
	if (pScene == NULL)
	{
		return;
	}

	PendingMouseAction action;
	action.type = PendingMouseAction::eSceneCancel;
	action.callbackId = RegisterForCallbackId(pScene);
	action.controlId = 0.0;
	action.childId = 0.0;
	action.key = ACTION_MENU_CANCEL;
	m_pendingMouseActions.push_back(action);
	PushDebugBreadcrumb("mouse queue cancel scene=%d", (int)pScene->getSceneType());
}

void UIController::QueueMouseDispatchInput(UIScene *pScene, unsigned int key)
{
	PendingMouseAction action;
	action.type = PendingMouseAction::eDispatchInput;
	action.callbackId = 0;
	action.controlId = 0.0;
	action.childId = 0.0;
	action.key = key;
	m_pendingMouseActions.push_back(action);
	PushDebugBreadcrumb("mouse queue input key=%u scene=%d", key, pScene != NULL ? (int)pScene->getSceneType() : -1);
}

void UIController::FlushPendingMouseActions()
{
	while(!m_pendingMouseActions.empty())
	{
		vector<PendingMouseAction> actions = m_pendingMouseActions;
		m_pendingMouseActions.clear();

		for(size_t i = 0; i < actions.size(); ++i)
		{
			PendingMouseAction &action = actions[i];
			UIScene *pScene = (action.callbackId != 0) ? GetSceneFromCallbackId(action.callbackId) : NULL;
			if(action.callbackId != 0)
			{
				UnregisterCallbackId(action.callbackId);
			}

			switch(action.type)
			{
			case PendingMouseAction::eActivateControl:
				if (pScene != NULL)
				{
					PushDebugBreadcrumb("mouse apply activate scene=%d control=%d child=%d",
						(int)pScene->getSceneType(), (int)action.controlId, (int)action.childId);
					pScene->TriggerControlPress(action.controlId, action.childId);
				}
				break;
			case PendingMouseAction::eSceneCancel:
				if (pScene != NULL)
				{
					PushDebugBreadcrumb("mouse apply cancel scene=%d", (int)pScene->getSceneType());
					pScene->TriggerDirectMenuAction(ACTION_MENU_CANCEL);
				}
				break;
			case PendingMouseAction::eDispatchInput:
				DispatchMouseSyntheticInput(0, action.key, true, true);
				break;
			default:
				break;
			}
		}
	}
}

void UIController::DispatchMouseSyntheticInput(unsigned int iPad, unsigned int key, bool pressed, bool released)
{
	PushDebugBreadcrumb("mouse synthetic pad=%u key=%u pressed=%d released=%d",
		iPad, key, pressed ? 1 : 0, released ? 1 : 0);
	DispatchInputToGroups(iPad, key, false, pressed, released);
}

void UIController::HandleMouseHover()
{
	if (!g_KeyboardMouseInput.IsEnabled())
	{
		ResetMouseHoverState(true);
		ResetMouseFocusState();
		ResetMouseActiveState();
		return;
	}

	UIScene *pTopScene = FindTopMouseScene();
	if (pTopScene == NULL || pTopScene->getMovie() == NULL || IsMouseBlockedScene(pTopScene->getSceneType()))
	{
		ResetMouseHoverState(false);
		ResetMouseFocusState();
		ResetMouseActiveState();
		return;
	}

	if (g_KeyboardMouseInput.IsMouseCaptured())
	{
		g_KeyboardMouseInput.SetMouseCaptured(false);
	}

	int mouseX = g_KeyboardMouseInput.GetMouseX();
	int mouseY = g_KeyboardMouseInput.GetMouseY();
	int mouseDeltaX = (m_lastMouseX >= 0) ? abs(mouseX - m_lastMouseX) : 0;
	int mouseDeltaY = (m_lastMouseY >= 0) ? abs(mouseY - m_lastMouseY) : 0;
	bool leftPressed = g_KeyboardMouseInput.IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	bool leftReleased = g_KeyboardMouseInput.IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
	bool leftDown = g_KeyboardMouseInput.IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	bool rightReleased = g_KeyboardMouseInput.IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
	bool mouse4Released = g_KeyboardMouseInput.IsMouseButtonReleased(MOUSE_BUTTON_4);
	bool mouse5Released = g_KeyboardMouseInput.IsMouseButtonReleased(MOUSE_BUTTON_5);
	int wheelDelta = g_KeyboardMouseInput.GetMouseWheelDelta();
	bool sceneChanged = (pTopScene != m_pMouseHoverScene);
	bool mouseMovedEnough = sceneChanged || mouseDeltaX >= 2 || mouseDeltaY >= 2;

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;

	if (sceneChanged && m_pMouseActiveScene != pTopScene)
	{
		ResetMouseActiveState();
	}
	if (sceneChanged && m_pMouseFocusScene != pTopScene)
	{
		ResetMouseFocusState();
	}

	S32 movieW = 0, movieH = 0;
	pTopScene->getMovieDimensions(movieW, movieH);
	if (movieW <= 0 || movieH <= 0 ||
		g_KeyboardMouseInput.GetWindowWidth() <= 0 || g_KeyboardMouseInput.GetWindowHeight() <= 0)
	{
		ResetMouseHoverState(false);
		ResetMouseFocusState();
		ResetMouseActiveState();
		return;
	}

	float scaleX = (float)movieW / (float)g_KeyboardMouseInput.GetWindowWidth();
	float scaleY = (float)movieH / (float)g_KeyboardMouseInput.GetWindowHeight();
	S32 uiMouseX = (S32)((float)mouseX * scaleX);
	S32 uiMouseY = (S32)((float)mouseY * scaleY);

	UIControl *pHoveredControl = NULL;
	bool hoveredClickable = false;
	vector<UIControl*> *pControls = pTopScene->GetControls();
	if (pControls != NULL)
	{
		for (size_t i = pControls->size(); i-- > 0;)
		{
			UIControl *pControl = (*pControls)[i];
			S32 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
			if (!ResolveMouseControlBounds(pTopScene, pControl, x1, y1, x2, y2))
			{
				continue;
			}

			UIControl::eUIControlType ctrlType = pControl->getControlType();
			if (!IsMouseActivatableControlType(ctrlType) &&
				ctrlType != UIControl::eDynamicLabel &&
				ctrlType != UIControl::eHTMLLabel)
			{
				continue;
			}

			if (uiMouseX >= x1 && uiMouseX <= x2 && uiMouseY >= y1 && uiMouseY <= y2)
			{
				pHoveredControl = pControl;
				hoveredClickable = IsMouseActivatableControlType(ctrlType);
				break;
			}
		}
	}

	if (pHoveredControl != NULL && (mouseMovedEnough || leftPressed))
	{
		HandleMouseControlHover(pTopScene, pHoveredControl, uiMouseX, uiMouseY, leftDown, true);
	}

	if (leftPressed)
	{
		m_pMouseActiveScene = pTopScene;
		m_iMouseActiveControlId = (pHoveredControl != NULL) ? pHoveredControl->getId() : -1;
		m_iMousePressStartX = uiMouseX;
		m_iMousePressStartY = uiMouseY;
		m_bMouseDragActive = false;
		PushDebugBreadcrumb("mouse down scene=%d control=%d x=%d y=%d",
			(int)pTopScene->getSceneType(), m_iMouseActiveControlId, uiMouseX, uiMouseY);
		if (pHoveredControl != NULL)
		{
			CommitMouseFocus(pTopScene, pHoveredControl);
			if (pHoveredControl->getControlType() == UIControl::eSlider)
			{
				m_bMouseDragActive = true;
				HandleMouseControlDrag(pTopScene, pHoveredControl, uiMouseX, uiMouseY, true);
			}
		}
	}
	else if (leftDown && m_pMouseActiveScene == pTopScene && m_iMouseActiveControlId >= 0)
	{
		UIControl *pActiveControl = FindSceneControlById(pTopScene, m_iMouseActiveControlId);
		if (pActiveControl != NULL)
		{
			if (!m_bMouseDragActive)
			{
				int dragDeltaX = abs(uiMouseX - m_iMousePressStartX);
				int dragDeltaY = abs(uiMouseY - m_iMousePressStartY);
				m_bMouseDragActive = (dragDeltaX >= 4 || dragDeltaY >= 4);
				if (m_bMouseDragActive)
				{
					PushDebugBreadcrumb("mouse drag start scene=%d control=%d",
						(int)pTopScene->getSceneType(), pActiveControl->getId());
				}
			}

			if (m_bMouseDragActive || pActiveControl->getControlType() == UIControl::eSlider)
			{
				HandleMouseControlDrag(pTopScene, pActiveControl, uiMouseX, uiMouseY, true);
			}
		}
	}

	if (leftReleased)
	{
		UIControl *pActiveControl = (m_pMouseActiveScene == pTopScene) ? FindSceneControlById(pTopScene, m_iMouseActiveControlId) : NULL;
		PushDebugBreadcrumb("mouse up scene=%d control=%d x=%d y=%d",
			(int)pTopScene->getSceneType(), pActiveControl != NULL ? pActiveControl->getId() : -1, uiMouseX, uiMouseY);
		bool dispatched = HandleMouseControlRelease(pTopScene, pActiveControl, pHoveredControl, uiMouseX, uiMouseY);
		if (m_bMouseDragActive)
		{
			PushDebugBreadcrumb("mouse drag end scene=%d control=%d",
				(int)pTopScene->getSceneType(), pActiveControl != NULL ? pActiveControl->getId() : -1);
		}
		ResetMouseActiveState();
		if (dispatched)
		{
			m_bMouseOverClickable = hoveredClickable;
			m_pMouseHoverScene = pTopScene;
			m_iMouseHoverControlId = (pHoveredControl != NULL) ? pHoveredControl->getId() : -1;
		}
	}

	if (rightReleased || mouse5Released)
	{
		QueueMouseCancel(pTopScene);
		ResetMouseHoverState(true);
		ResetMouseFocusState();
		ResetMouseActiveState();
		return;
	}

	if (mouse4Released)
	{
		QueueMouseDispatchInput(pTopScene, ACTION_MENU_Y);
		ResetMouseHoverState(true);
		ResetMouseActiveState();
		return;
	}

	if (wheelDelta != 0)
	{
		bool consumed = false;
		UIControl *pWheelControl = pHoveredControl;
		if (pWheelControl == NULL)
		{
			pWheelControl = FindSceneControlById(pTopScene, pTopScene->getControlFocus());
		}

		if (pWheelControl != NULL)
		{
			if (pTopScene->getSceneType() == eUIScene_SettingsKeyBindMenu &&
				pWheelControl->getControlType() == UIControl::eButton)
			{
				consumed = ((UIScene_SettingsKeyBindMenu *)pTopScene)->ApplyMouseWheelToVisibleSlot(
					pWheelControl->getId(), wheelDelta);
			}

			switch (pWheelControl->getControlType())
			{
			case UIControl::eButtonList:
			case UIControl::eSaveList:
			case UIControl::ePlayerList:
			case UIControl::eDLCList:
				if (consumed)
				{
					break;
				}
				{
					UIControl_ButtonList *pButtonList = (UIControl_ButtonList *)pWheelControl;
					int currentSel = pButtonList->getCurrentSelection();
					int newSel = currentSel;
					if (wheelDelta > 0 && currentSel > 0)
					{
						newSel = currentSel - 1;
					}
					else if (wheelDelta < 0 && currentSel < pButtonList->getItemCount() - 1)
					{
						newSel = currentSel + 1;
					}

					if (newSel != currentSel)
					{
						CommitMouseFocus(pTopScene, pWheelControl);
						pButtonList->setCurrentSelection(newSel);
						pButtonList->updateChildFocus(newSel);
					}

					consumed = true;
				}
				break;
			case UIControl::eSlider:
				if (consumed)
				{
					break;
				}
				HandleMouseControlDrag(pTopScene, pWheelControl, uiMouseX, uiMouseY, true);
				consumed = true;
				break;
			default:
				break;
			}
		}

		if (!consumed)
		{
			if (pTopScene->getSceneType() == eUIScene_SettingsKeyBindMenu)
			{
				unsigned int key = (wheelDelta > 0) ? ACTION_MENU_LEFT_SCROLL : ACTION_MENU_RIGHT_SCROLL;
				for (int step = 0; step < abs(wheelDelta); ++step)
				{
					QueueMouseDispatchInput(pTopScene, key);
				}
				ResetMouseHoverState(true);
				ResetMouseActiveState();
				return;
			}
		}
	}

	m_bMouseOverClickable = hoveredClickable;
	m_pMouseHoverScene = pTopScene;
	m_iMouseHoverControlId = (pHoveredControl != NULL) ? pHoveredControl->getId() : -1;
}
#endif

#ifdef __PSVITA__

void UIController::TouchBoxAdd(UIControl *pControl,UIScene *pUIScene)
{
	EUIGroup eUIGroup=pUIScene->GetParentLayerGroup();
	EUILayer eUILayer=pUIScene->GetParentLayer()->m_iLayer;
	EUIScene eUIscene=pUIScene->getSceneType();

	TouchBoxAdd(pControl,eUIGroup,eUILayer,eUIscene, pUIScene->GetMainPanel());
}

void UIController::TouchBoxAdd(UIControl *pControl,EUIGroup eUIGroup,EUILayer eUILayer,EUIScene eUIscene, UIControl *pMainPanelControl)
{
	UIELEMENT *puiElement = new UIELEMENT;
	puiElement->pControl = pControl;

	S32 iControlWidth = pControl->getWidth();
	S32 iControlHeight = pControl->getHeight();
	S32 iMainPanelOffsetX = 0;
	S32 iMainPanelOffsetY= 0;

	// 4J-TomK add main panel offset if controls do not live in the root scene
	if(pMainPanelControl)
	{
		iMainPanelOffsetX = pMainPanelControl->getXPos();
		iMainPanelOffsetY = pMainPanelControl->getYPos();
	}

	// 4J-TomK override control width / height where needed
	if(puiElement->pControl->getControlType() == UIControl::eSlider)
	{
		// Sliders are never scaled but masked, so we have to get the real width from AS
		UIControl_Slider *pSlider = (UIControl_Slider *)puiElement->pControl;
		iControlWidth = pSlider->GetRealWidth();
	}
	else if(puiElement->pControl->getControlType() == UIControl::eTexturePackList)
	{
		// The origin of the TexturePackList is NOT in the top left corner but where the slot area starts. therefore we need the height of the slot area itself.
		UIControl_TexturePackList *pTexturePackList = (UIControl_TexturePackList *)puiElement->pControl;
		iControlHeight = pTexturePackList->GetRealHeight();
	}
	else if(puiElement->pControl->getControlType() == UIControl::eDynamicLabel)
	{
		// The height and width of this control changes per how to play page
		UIControl_DynamicLabel *pDynamicLabel = (UIControl_DynamicLabel *)puiElement->pControl;
		iControlWidth = pDynamicLabel->GetRealWidth();
		iControlHeight = pDynamicLabel->GetRealHeight();
	}
	else if(puiElement->pControl->getControlType() == UIControl::eHTMLLabel)
	{
		// The height and width of this control changes per how to play page
		UIControl_HTMLLabel *pHtmlLabel = (UIControl_HTMLLabel *)puiElement->pControl;
		iControlWidth = pHtmlLabel->GetRealWidth();
		iControlHeight = pHtmlLabel->GetRealHeight();
	}

	puiElement->x1=(S32)((float)pControl->getXPos() + (float)iMainPanelOffsetX);
	puiElement->y1=(S32)((float)pControl->getYPos() + (float)iMainPanelOffsetY);
	puiElement->x2=(S32)(((float)pControl->getXPos() + (float)iControlWidth + (float)iMainPanelOffsetX));
	puiElement->y2=(S32)(((float)pControl->getYPos() + (float)iControlHeight + (float)iMainPanelOffsetY));

	if(puiElement->pControl->getControlType() == UIControl::eNoControl)
	{
		app.DebugPrintf("NO CONTROL!");
	}

	if(puiElement->x1 == puiElement->x2 || puiElement->y1 == puiElement->y2)
	{
		app.DebugPrintf("NOT adding touchbox %d,%d,%d,%d\n",puiElement->x1,puiElement->y1,puiElement->x2,puiElement->y2);
	}
	else
	{
		app.DebugPrintf("Adding touchbox %d,%d,%d,%d\n",puiElement->x1,puiElement->y1,puiElement->x2,puiElement->y2);
		m_TouchBoxes[eUIGroup][eUILayer][eUIscene].push_back(puiElement);
	}
}

void UIController::TouchBoxRebuild(UIScene *pUIScene)
{
	EUIGroup eUIGroup=pUIScene->GetParentLayerGroup();
	EUILayer eUILayer=pUIScene->GetParentLayer()->m_iLayer;
	EUIScene eUIscene=pUIScene->getSceneType();

	// if we delete an element, it's possible that the scene has re-arranged all the elements, so we need to rebuild the boxes
	ui.TouchBoxesClear(pUIScene);

	// rebuild boxes
	AUTO_VAR(itEnd, pUIScene->GetControls()->end());
	for (AUTO_VAR(it, pUIScene->GetControls()->begin()); it != itEnd; it++)
	{
		UIControl *control=(UIControl *)*it;

		if(control->getControlType() == UIControl::eButton ||
			control->getControlType() == UIControl::eSlider ||
			control->getControlType() == UIControl::eCheckBox ||
			control->getControlType() == UIControl::eTexturePackList ||
			control->getControlType() == UIControl::eButtonList ||
			control->getControlType() == UIControl::eTextInput ||
			control->getControlType() == UIControl::eDynamicLabel ||
			control->getControlType() == UIControl::eHTMLLabel ||
			control->getControlType() == UIControl::eLeaderboardList ||
			control->getControlType() == UIControl::eTouchControl)
		{
			// 4J-TomK update the control (it might have been moved by flash / AS)
			control->UpdateControl();

			ui.TouchBoxAdd(control,eUIGroup,eUILayer,eUIscene, pUIScene->GetMainPanel());
		}
	}
}

void UIController::TouchBoxesClear(UIScene *pUIScene)
{
	EUIGroup eUIGroup=pUIScene->GetParentLayerGroup();
	EUILayer eUILayer=pUIScene->GetParentLayer()->m_iLayer;
	EUIScene eUIscene=pUIScene->getSceneType();

	AUTO_VAR(itEnd, m_TouchBoxes[eUIGroup][eUILayer][eUIscene].end());
	for (AUTO_VAR(it, m_TouchBoxes[eUIGroup][eUILayer][eUIscene].begin()); it != itEnd; it++)
	{
		UIELEMENT *element=(UIELEMENT *)*it;
		delete element;		
	}
	m_TouchBoxes[eUIGroup][eUILayer][eUIscene].clear();
}

bool UIController::TouchBoxHit(UIScene *pUIScene,S32 x, S32 y)
{
	EUIGroup eUIGroup=pUIScene->GetParentLayerGroup();
	EUILayer eUILayer=pUIScene->GetParentLayer()->m_iLayer;
	EUIScene eUIscene=pUIScene->getSceneType();

	// 4J-TomK let's do the transformation from touch resolution to screen resolution here, so our touchbox values always are in screen resolution!
	x *= (m_fScreenWidth/1920.0f);
	y *= (m_fScreenHeight/1080.0f);

	if(m_TouchBoxes[eUIGroup][eUILayer][eUIscene].size()>0)
	{
		AUTO_VAR(itEnd, m_TouchBoxes[eUIGroup][eUILayer][eUIscene].end());
		for (AUTO_VAR(it, m_TouchBoxes[eUIGroup][eUILayer][eUIscene].begin()); it != itEnd; it++)
		{
			UIELEMENT *element=(UIELEMENT *)*it;
			if(element->pControl->getHidden() == false && element->pControl->getVisible()) // ignore removed controls
			{
				if((x>=element->x1) &&(x<=element->x2) && (y>=element->y1) && (y<=element->y2))
				{
					if(!m_bTouchscreenPressed)
					{
						app.DebugPrintf("SET m_ActiveUIElement (Layer: %i) at x = %i y = %i\n", (int)eUILayer, (int)x, (int)y);
						m_ActiveUIElement = element;
					}
					// remember the currently highlighted element
					m_HighlightedUIElement = element;

					return true;
				}
			}
		}			
	}

	//app.DebugPrintf("MISS at x = %i y = %i\n", (int)x, (int)y);
	m_HighlightedUIElement = NULL;
	return false;
}

//
// Handle Touch Input
//
void UIController::HandleTouchInput(unsigned int iPad, unsigned int key, bool bPressed, bool bRepeat, bool bReleased)
{
	// no input? no handling!
	if(!bPressed && !bRepeat && !bReleased)
	{
		// override for instand repeat without delay!
		if(m_bTouchscreenPressed && m_ActiveUIElement && (
			m_ActiveUIElement->pControl->getControlType() == UIControl::eSlider ||
			m_ActiveUIElement->pControl->getControlType() == UIControl::eButtonList ||
			m_ActiveUIElement->pControl->getControlType() == UIControl::eTexturePackList ||
			m_ActiveUIElement->pControl->getControlType() == UIControl::eDynamicLabel ||
			m_ActiveUIElement->pControl->getControlType() == UIControl::eHTMLLabel ||
			m_ActiveUIElement->pControl->getControlType() == UIControl::eLeaderboardList ||
			m_ActiveUIElement->pControl->getControlType() == UIControl::eTouchControl))
			bRepeat = true;	// the above controls need to be controllable without having the finger over them
		else
			return;
	}

	SceTouchData* pTouchData = InputManager.GetTouchPadData(iPad,false);
	S32 x = pTouchData->report[0].x * (m_fScreenWidth/1920.0f);
	S32 y = pTouchData->report[0].y * (m_fScreenHeight/1080.0f);

	if(bPressed && !bRepeat && !bReleased)	// PRESSED HANDLING
	{
		app.DebugPrintf("touch input pressed\n");
		switch(m_ActiveUIElement->pControl->getControlType())
		{
		case UIControl::eButton:
			// set focus
			UIControl_Button *pButton=(UIControl_Button *)m_ActiveUIElement->pControl;
			pButton->getParentScene()->SetFocusToElement(m_ActiveUIElement->pControl->getId());
			// override bPressed to false. we only want the button to trigger on touch release!
			bPressed = false;
			break;
		case UIControl::eSlider:
			// set focus
			UIControl_Slider *pSlider=(UIControl_Slider *)m_ActiveUIElement->pControl;
			pSlider->getParentScene()->SetFocusToElement(m_ActiveUIElement->pControl->getId());
			break;
		case UIControl::eCheckBox:
			// set focus
			UIControl_CheckBox *pCheckbox=(UIControl_CheckBox *)m_ActiveUIElement->pControl;
			pCheckbox->getParentScene()->SetFocusToElement(m_ActiveUIElement->pControl->getId());
			// override bPressed. we only want the checkbox to trigger on touch release!
			bPressed = false;
			break;
		case UIControl::eButtonList:
			// set focus to list
			UIControl_ButtonList *pButtonList=(UIControl_ButtonList *)m_ActiveUIElement->pControl;
			//pButtonList->getParentScene()->SetFocusToElement(m_ActiveUIElement->pControl->getId());
			// tell list where we tapped it so it can set focus to the correct button
			pButtonList->SetTouchFocus((float)x, (float)y, false);
			// override bPressed. we only want the ButtonList to trigger on touch release!
			bPressed = false;
			break;
		case UIControl::eTexturePackList:
			// set focus to list
			UIControl_TexturePackList *pTexturePackList=(UIControl_TexturePackList *)m_ActiveUIElement->pControl;
			pTexturePackList->getParentScene()->SetFocusToElement(m_ActiveUIElement->pControl->getId());
			// tell list where we tapped it so it can set focus to the correct texture pack
			pTexturePackList->SetTouchFocus((float)x - (float)m_ActiveUIElement->x1, (float)y - (float)m_ActiveUIElement->y1, false);
			// override bPressed. we only want the TexturePack List to trigger on touch release!
			bPressed = false;
			break;
		case UIControl::eTextInput:
			// set focus
			UIControl_TextInput *pTextInput=(UIControl_TextInput *)m_ActiveUIElement->pControl;
			pTextInput->getParentScene()->SetFocusToElement(m_ActiveUIElement->pControl->getId());
			// override bPressed to false. we only want the textinput to trigger on touch release!
			bPressed = false;
			break;
		case UIControl::eDynamicLabel:
			// handle dynamic label scrolling
			UIControl_DynamicLabel *pDynamicLabel=(UIControl_DynamicLabel *)m_ActiveUIElement->pControl;
			pDynamicLabel->TouchScroll(y, true);
			// override bPressed to false
			bPressed = false;
			break;
		case UIControl::eHTMLLabel:
			// handle dynamic label scrolling
			UIControl_HTMLLabel *pHtmlLabel=(UIControl_HTMLLabel *)m_ActiveUIElement->pControl;
			pHtmlLabel->TouchScroll(y, true);
			// override bPressed to false
			bPressed = false;
			break;
		case UIControl::eLeaderboardList:
			// set focus to list
			UIControl_LeaderboardList *pLeaderboardList=(UIControl_LeaderboardList *)m_ActiveUIElement->pControl;
			// tell list where we tapped it so it can set focus to the correct button
			pLeaderboardList->SetTouchFocus((float)x, (float)y, false);
			// override bPressed. we only want the ButtonList to trigger on touch release!
			bPressed = false;
			break;
		case UIControl::eTouchControl:
			// pass on touch input to relevant parent scene so we can handle it there!
			m_ActiveUIElement->pControl->getParentScene()->handleTouchInput(iPad, x, y, m_ActiveUIElement->pControl->getId(), bPressed, bRepeat, bReleased);
			// override bPressed to false
			bPressed = false;
			break;
		default:
			app.DebugPrintf("PRESSED - UNHANDLED UI ELEMENT\n");
			break;
		}
	}
	else if(bRepeat)	// REPEAT HANDLING
	{
		switch(m_ActiveUIElement->pControl->getControlType())
		{
		case UIControl::eButton:
			/* no action */
			break;
		case UIControl::eSlider:
			// handle slider movement
			UIControl_Slider *pSlider=(UIControl_Slider *)m_ActiveUIElement->pControl;
			float fNewSliderPos = ((float)x - (float)m_ActiveUIElement->x1) / (float)pSlider->GetRealWidth();
			pSlider->SetSliderTouchPos(fNewSliderPos);
			break;
		case UIControl::eCheckBox:
			/* no action */
			bRepeat = false;
			bPressed = false;
			break;
		case UIControl::eButtonList:
			// handle button list scrolling
			UIControl_ButtonList *pButtonList=(UIControl_ButtonList *)m_ActiveUIElement->pControl;
			pButtonList->SetTouchFocus((float)x, (float)y, true);
			break;
		case UIControl::eTexturePackList:
			// handle texturepack list scrolling
			UIControl_TexturePackList *pTexturePackList=(UIControl_TexturePackList *)m_ActiveUIElement->pControl;
			pTexturePackList->SetTouchFocus((float)x - (float)m_ActiveUIElement->x1, (float)y - (float)m_ActiveUIElement->y1, true);
			break;
		case UIControl::eTextInput:
			/* no action */
			bRepeat = false;
			bPressed = false;
			break;
		case UIControl::eDynamicLabel:
			// handle dynamic label scrolling
			UIControl_DynamicLabel *pDynamicLabel=(UIControl_DynamicLabel *)m_ActiveUIElement->pControl;
			pDynamicLabel->TouchScroll(y, true);
			// override bPressed & bRepeat to false
			bPressed = false;
			bRepeat = false;
			break;
		case UIControl::eHTMLLabel:
			// handle dynamic label scrolling
			UIControl_HTMLLabel *pHtmlLabel=(UIControl_HTMLLabel *)m_ActiveUIElement->pControl;
			pHtmlLabel->TouchScroll(y, true);
			// override bPressed & bRepeat to false
			bPressed = false;
			bRepeat = false;
			break;
		case UIControl::eLeaderboardList:
			// handle button list scrolling
			UIControl_LeaderboardList *pLeaderboardList=(UIControl_LeaderboardList *)m_ActiveUIElement->pControl;
			pLeaderboardList->SetTouchFocus((float)x, (float)y, true);
			break;
		case UIControl::eTouchControl:
			// override bPressed to false
			bPressed = false;
			// pass on touch input to relevant parent scene so we can handle it there!
			m_ActiveUIElement->pControl->getParentScene()->handleTouchInput(iPad, x, y, m_ActiveUIElement->pControl->getId(), bPressed, bRepeat, bReleased);
			// override bRepeat to false
			bRepeat = false;
			break;
		default:
			app.DebugPrintf("REPEAT - UNHANDLED UI ELEMENT\n");
			break;
		}
	}
	if(bReleased)	// RELEASED HANDLING
	{
		app.DebugPrintf("touch input released\n");
		switch(m_ActiveUIElement->pControl->getControlType())
		{
		case UIControl::eButton:
			// trigger button on release (ONLY if the finger is still on it!)
			if(m_HighlightedUIElement && m_ActiveUIElement->pControl == m_HighlightedUIElement->pControl)
				bPressed = true;
			break;
		case UIControl::eSlider:
			/* no action */
			break;
		case UIControl::eCheckBox:
			// trigger checkbox on release (ONLY if the finger is still on it!)
			if(m_HighlightedUIElement && m_ActiveUIElement->pControl == m_HighlightedUIElement->pControl)
			{
				UIControl_CheckBox *pCheckbox=(UIControl_CheckBox *)m_ActiveUIElement->pControl;
				pCheckbox->TouchSetCheckbox(!pCheckbox->IsChecked());
			}
			bReleased = false;
			break;
		case UIControl::eButtonList:
			// trigger buttonlist on release (ONLY if the finger is still on it!)
			if(m_HighlightedUIElement && m_ActiveUIElement->pControl == m_HighlightedUIElement->pControl)
			{
				UIControl_ButtonList *pButtonList=(UIControl_ButtonList *)m_ActiveUIElement->pControl;
				if(pButtonList->CanTouchTrigger(x,y))
					bPressed = true;
			}
			break;
		case UIControl::eTexturePackList:
			// trigger texturepack list on release (ONLY if the finger is still on it!)
			if(m_HighlightedUIElement && m_ActiveUIElement->pControl == m_HighlightedUIElement->pControl)
			{
				UIControl_TexturePackList *pTexturePackList=(UIControl_TexturePackList *)m_ActiveUIElement->pControl;
				if(pTexturePackList->CanTouchTrigger((float)x - (float)m_ActiveUIElement->x1, (float)y - (float)m_ActiveUIElement->y1))
					bPressed = true;
			}
			break;
		case UIControl::eTextInput:
			// trigger TextInput on release (ONLY if the finger is still on it!)
			if(m_HighlightedUIElement && m_ActiveUIElement->pControl == m_HighlightedUIElement->pControl)
				bPressed = true;
			break;
		case UIControl::eDynamicLabel:
			// handle dynamic label scrolling
			UIControl_DynamicLabel *pDynamicLabel=(UIControl_DynamicLabel *)m_ActiveUIElement->pControl;
			pDynamicLabel->TouchScroll(y, false);
			break;
		case UIControl::eHTMLLabel:
			// handle dynamic label scrolling
			UIControl_HTMLLabel *pHtmlLabel=(UIControl_HTMLLabel *)m_ActiveUIElement->pControl;
			pHtmlLabel->TouchScroll(y, false);
			break;
		case UIControl::eLeaderboardList:
			/* no action */
			break;
		case UIControl::eTouchControl:
			// trigger only if touch is released over the same component!
			if(m_HighlightedUIElement && m_ActiveUIElement->pControl == m_HighlightedUIElement->pControl)
			{
				// pass on touch input to relevant parent scene so we can handle it there!
				m_ActiveUIElement->pControl->getParentScene()->handleTouchInput(iPad, x, y, m_ActiveUIElement->pControl->getId(), bPressed, bRepeat, bReleased);
			}
			// override bReleased to false
			bReleased = false;
			break;
		default:
			app.DebugPrintf("RELEASED - UNHANDLED UI ELEMENT\n");
			break;
		}
	}

	// only proceed if there's input to be handled
	if(bPressed || bRepeat || bReleased)
	{
		SendTouchInput(iPad, key, bPressed, bRepeat, bReleased);
	}
}

void UIController::SendTouchInput(unsigned int iPad, unsigned int key, bool bPressed, bool bRepeat, bool bReleased)
{
	DispatchInputToGroups(iPad, key, bRepeat, bPressed, bReleased);
}

#endif // __PSVITA__

void UIController::WriteWindowsDebugSnapshot(FILE *file)
{
	if(file == NULL)
	{
		return;
	}

	fprintf(file, "\nUI Debug Snapshot\n");
#ifdef _WIN64
	fprintf(file, "LastInputDevice: %s\n", GetWindowsInputDeviceName());
	fprintf(file, "MouseHoverScene: %d\n", m_pMouseHoverScene != NULL ? (int)m_pMouseHoverScene->getSceneType() : -1);
	fprintf(file, "MouseHoverControl: %d\n", m_iMouseHoverControlId);
	fprintf(file, "MouseFocusScene: %d\n", m_pMouseFocusScene != NULL ? (int)m_pMouseFocusScene->getSceneType() : -1);
	fprintf(file, "MouseFocusControl: %d\n", m_iMouseFocusControlId);
	fprintf(file, "MouseActiveScene: %d\n", m_pMouseActiveScene != NULL ? (int)m_pMouseActiveScene->getSceneType() : -1);
	fprintf(file, "MouseActiveControl: %d\n", m_iMouseActiveControlId);
	fprintf(file, "MouseDragActive: %d\n", m_bMouseDragActive ? 1 : 0);
	fprintf(file, "MouseClickable: %d\n", m_bMouseOverClickable ? 1 : 0);
#else
	fprintf(file, "LastInputDevice: Controller\n");
#endif
	fprintf(file, "PendingNavigationCount: %u\n", (unsigned int)m_pendingNavigation.size());
	fprintf(file, "InputDispatchDepth: %d\n", m_inputDispatchDepth);

	for(unsigned int i = 0; i < eUIGroup_COUNT; ++i)
	{
		UIScene *currentScene = NULL;
#if defined(__PSVITA__) || defined(_WIN64)
		currentScene = m_groups[i]->getCurrentScene();
#endif
		fprintf(file, "FocusedScene[%u]: %d control=%d child=%d\n",
			i,
			currentScene != NULL ? (int)currentScene->getSceneType() : -1,
			currentScene != NULL ? currentScene->getControlFocus() : -1,
			currentScene != NULL ? currentScene->getControlChildFocus() : -1);
		m_groups[i]->WriteSceneStackDebug(file);
	}

	fprintf(file, "Breadcrumbs (%d):\n", m_uiBreadcrumbCount);
	int start = (m_uiBreadcrumbHead - m_uiBreadcrumbCount + UI_BREADCRUMB_COUNT) % UI_BREADCRUMB_COUNT;
	for(int i = 0; i < m_uiBreadcrumbCount; ++i)
	{
		int index = (start + i) % UI_BREADCRUMB_COUNT;
		fprintf(file, "  %02d: %s\n", i, m_uiBreadcrumbs[index].c_str());
	}
}
