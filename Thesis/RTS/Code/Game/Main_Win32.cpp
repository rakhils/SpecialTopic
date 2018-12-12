#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>	
#include <cassert>
#include <crtdbg.h>
#include <stdio.h>
#include "Engine/EngineSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Time/Clock.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"

//#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#define UNUSED(x) (void)(x);
float MAX_DELTA_VALUE = 0.01616f;

constexpr float CLIENT_ASPECT = 1.0f;
const char* APP_NAME = "Space Shooter3D";
App *g_theApp;
bool DevConsoleMessageHandler(unsigned int wmMessageCode, size_t wParam, size_t lParam);
bool AppMessageHandler(unsigned int wmMessageCode, size_t wParam, size_t lParam)
{

	unsigned char keyCode = (unsigned char)wParam;
	switch (wmMessageCode)
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		
		// TO UPDATE AND RENDER EVEN IF NOT IN FOCUS
	case WM_NCACTIVATE:
		return 0;
	case WM_CLOSE:
		//case WM_DESTROY:
	case WM_QUIT:
	{
		g_isQuitting = true;
		return false;
		// "Consumes" this message (tells Windows "okay, we handled it")}
		//Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	}
	break;
	case WM_CHAR:
	{
		unsigned char asKey = (unsigned char)wParam;
		if (asKey == '`') // TILDE
		{
			Windows::GetInstance()->RemoveHandler(AppMessageHandler);
			Windows::GetInstance()->AddHandler(DevConsoleMessageHandler);
			DevConsole::GetInstance()->Open();
			DevConsole::GetInstance()->ClearPrediction();
			IntVector2 dimensions = Windows::GetInstance()->GetDimensions();
			DevConsole::GetInstance()->UpdateDimensions(Vector2(0, 0), Vector2(static_cast<float>(dimensions.x), static_cast<float>(dimensions.y)));
			return false;
		}
	}
	break;
	case WM_KEYDOWN:
	{
		unsigned char asKey = (unsigned char)wParam;
		g_theInput->onKeyPressed(keyCode);
		// If ESC, Quit the app
		if (asKey == VK_ESCAPE)
		{
			//g_isQuitting = true;
			//return false;
		}
		if (asKey == VK_F10)
		{
			g_theGameClock->SetScale(0);
		}

		break;
	}// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		g_theInput->onKeyReleased(keyCode);
		break;
	}
	case WM_LBUTTONDOWN:
	{

		int iPosX = LOWORD(lParam);
		int iPosY = HIWORD(lParam);
		g_theInput->OnLButtonClicked();
		g_theInput->UpdateMouseXYPosition(iPosX, iPosY);
		break;
	}
	case WM_LBUTTONUP:
		g_theInput->OnLButtonReleased();
		break;

	case WM_RBUTTONDOWN:
	{
		int iPosX = LOWORD(lParam);
		int iPosY = HIWORD(lParam);
		g_theInput->OnRButtonClicked();
		g_theInput->UpdateMouseXYPosition(iPosX, iPosY);
		break;
	}
	case WM_RBUTTONUP:
		g_theInput->OnRButtonReleased();
		break;
	}
	return true;
}

bool DevConsoleMessageHandler(unsigned int wmMessageCode, size_t wParam, size_t lParam)
{
	UNUSED(lParam);
	switch (wmMessageCode)
	{
	case WM_NCACTIVATE:
		return 0;
	case WM_CHAR:
	{
		unsigned char asKey = (unsigned char)wParam;

		if (asKey == VK_ESCAPE)
		{
			if (DevConsole::GetInstance()->IsPredictionOn())
			{
				DevConsole::GetInstance()->ClearPrediction();
				return true;
			}
			DevConsole::GetInstance()->UpdateInputTextWithEscapeKey();
			DevConsole::GetInstance()->ResetHistoryIndex();
			if (DevConsole::GetInstance()->IsTextSelected())
			{
				DevConsole::GetInstance()->ClearTextSelection();
				return true;
			}
			if (DevConsole::GetInstance()->IsInputAlreadyCleared())
			{
				Windows::GetInstance()->AddHandler(AppMessageHandler);
				Windows::GetInstance()->RemoveHandler(DevConsoleMessageHandler);
				DevConsole::GetInstance()->Close();
			}
			DevConsole::GetInstance()->ClearInputScreen();
			return true;
		}
		if (asKey == InputSystem::KEYBOARD_ENTER)
		{
			DevConsole::GetInstance()->ResetHistoryIndex();
			if (DevConsole::GetInstance()->IsPredictionOn())
			{
				DevConsole::GetInstance()->UpdateInputWithEnterKey();
				return true;
			}
			if (!DevConsole::GetInstance()->IsInputAlreadyCleared())
			{
				DevConsole::GetInstance()->UpdateInputWithEnterKey();
			}
		}
		else
		{
			DevConsole::GetInstance()->UpdateInputText(asKey);
		}
	}
	break;
	case WM_MOUSEWHEEL:
	{
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta < 0)
		{
			DevConsole::GetInstance()->ScrollOutputUp();
		}
		if (zDelta > 0)
		{
			DevConsole::GetInstance()->ScrollOutputDown();
		}
	}
	break;
	case WM_KEYUP:
	{
		unsigned char asKey = (unsigned char)wParam;
		if (asKey == VK_SHIFT)
		{
			DevConsole::GetInstance()->SetShiftPressed(false);
		}
		if (asKey == VK_CONTROL)
		{
			DevConsole::GetInstance()->SetControlPressed(false);
		}
	}
	break;
	case WM_KEYDOWN:
	{
		unsigned char asKey = (unsigned char)wParam;
		if (asKey == VK_LEFT)
		{
			DevConsole::GetInstance()->UpdateInputTextWithLeftKey();
		}
		if (asKey == VK_RIGHT)
		{
			DevConsole::GetInstance()->UpdateInputTextWithRightKey();
		}
		if (asKey == VK_UP)
		{
			DevConsole::GetInstance()->UpdateInputTextWithUpKey();
		}
		if (asKey == VK_DOWN)
		{
			DevConsole::GetInstance()->UpdateInputTextWithDownKey();
		}
		if (asKey == VK_DELETE)
		{
			DevConsole::GetInstance()->UpdateInputTextWithDeleteKey();
		}
		if (asKey == VK_SHIFT)
		{
			DevConsole::GetInstance()->SetShiftPressed(true);
		}
		if (asKey == VK_CONTROL)
		{
			DevConsole::GetInstance()->SetControlPressed(true);
		}
	}
	break;
	}
	return true;
}

void CreateOpenGLWindow(HINSTANCE applicationInstanceHandle, float clientAspect)
{
	UNUSED(applicationInstanceHandle);
	Windows *window = Windows::CreateInstance(APP_NAME, clientAspect);
	window->AddHandler(DevConsoleMessageHandler);
	window->AddHandler(AppMessageHandler);
}

//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle)
{
	g_theApp = new App();
	CreateOpenGLWindow(applicationInstanceHandle, CLIENT_ASPECT);
	EngineSystem::StartUp();
	InitVariables();
	g_theRenderer = Renderer::GetInstance();
	CommandRegister("quit", QuitApp, "QUITS APPLICATION");
	CommandRegister("net_unreliable_test", UnreliableTest, "TEST UNRELIABLE");
	CommandRegister("net_reliable_test", ReliableTest, "TEST UNRELIABLE");

	//CommandRegister("udp_star", UDPTestStart, "STARTS TEST UDP ");
	//CommandRegister("udp_stop", UDPTestStop, "STOPS TEST UDP");
	//CommandRegister("udp_send", UDPTestSend, "UDP SEND TEST");
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	delete g_theApp;
	g_theApp = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{

	UNUSED(commandLineString);
	Initialize(applicationInstanceHandle);
	while (!g_theApp->IsReadyToQuit())
	{
		if (g_isQuitting)
		{
			break;
		}
		g_theApp->RunFrame();
	}
	Shutdown();
	return 0;
}


