#include "Engine/Core/Windows.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define GAME_WINDOW_CLASS (TEXT( "Simple Window Class" ))

static Windows* gWindow = nullptr; 

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
// #SD1ToDo: We will move this function to a more appropriate place when we're ready
//
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
   bool run_default = true; 
   Windows *window = Windows::GetInstance(); 
   if (nullptr != window) 
   {
	  for (windows_message_handler_cb iter : window->m_message_handlers) 
	  {
		 run_default = iter( wmMessageCode, wParam, lParam ) && run_default;  
	  }
   }

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	if (run_default) 
	{
	  return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
    } 
	else 
	{
	  return 0; 
    }
}

static void RegisterWindowClass() 
{
   // Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = GAME_WINDOW_CLASS;
	RegisterClassEx( &windowClassDescription );
}

Windows::Windows( char const *app_name, float clientAspect )
{
   RegisterWindowClass(); 
   
	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags   = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED; // WS_POPUP
	//const DWORD windowStyleFlags   = WS_POPUP;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth =  (float)(desktopRect.right - desktopRect.left);
	float desktopHeight =  (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 1;// 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		//clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0;// 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0;// 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	m_dimensions = IntVector2(static_cast<int>(clientWidth),static_cast<int>(clientHeight));
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, app_name, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		GAME_WINDOW_CLASS,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		GetModuleHandle( NULL ),
		NULL );

	ShowWindow( hwnd, SW_SHOW );
	SetForegroundWindow( hwnd );
	SetFocus( hwnd );

   m_hwnd = (void*)hwnd; 
}

Windows::~Windows()
{
   ::DestroyWindow( (HWND)m_hwnd ); 

   ::UnregisterClass( GAME_WINDOW_CLASS, GetModuleHandle(NULL) ); 
   m_hwnd = nullptr; 
   gWindow = nullptr; 
}

void Windows::AddHandler( windows_message_handler_cb cb )
{
   m_message_handlers.push_back( cb ); 
}

 
//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Removes handler when not in use
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Windows::RemoveHandler(windows_message_handler_cb cb)
{
	for(int index = 0;index <m_message_handlers.size();index++)
	{
		if(m_message_handlers.at(index) == cb)
		{
			m_message_handlers.erase(m_message_handlers.begin() + index,m_message_handlers.begin()+index+1);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Windows::GetCurrentMemoryUsage()
{
/*
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;
*/

	return  100;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/08
*@purpose : Get windows Dimensions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IntVector2 Windows::GetDimensions()
{
	return m_dimensions;
}

Windows* Windows::CreateInstance( char const *title, float aspect ) 
{
   if (gWindow == nullptr) 
   {
	  gWindow = new Windows( title, aspect ); 
   }
   return gWindow; 
}

Windows* Windows::GetInstance()
{
   return gWindow; 
}
