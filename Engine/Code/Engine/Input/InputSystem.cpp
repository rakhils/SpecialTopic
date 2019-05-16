#include "InputSystem.hpp"
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <math.h>

#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Renderer/Camera.hpp"
//const unsigned char	InputSystem::KEYBOARD_ESCAPE	= VK_ESCAPE;
//const unsigned char	InputSystem::KEYBOARD_F1		= VK_F1;

int val = 0;
InputSystem * InputSystem::s_inputSystem = nullptr;

InputSystem::InputSystem()
{
	for (int keyCode = 0; keyCode < InputSystem::NUM_KEYS; ++keyCode)
	{
		m_keyStates[keyCode].m_wasJustPressed = false;
		m_keyStates[keyCode].m_wasJustReleased = false;
		m_keyStates[keyCode].m_isKeyDown = false;
	}

	m_mouseStates.m_isLButtonDown = false;
	m_mouseStates.m_isRButtonDown = false;

	m_mouseStates.m_wasLeftDoubleClicked = false;

	m_mouseStates.m_wasLButtonJustPressed = false;
	m_mouseStates.m_wasLButtonJustReleased = false;

	m_mouseStates.m_wasRButtonJustPressed = false;
	m_mouseStates.m_wasLButtonJustPressed = false;

	m_mouseStates.m_mouseXPosition = -1;
	m_mouseStates.m_mouseYPosition = -1;
}

InputSystem::~InputSystem()
{

}

void RunMessagePump() // NOTE: standalone function in InputSystem.cpp (not an InputSystem method)
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}
//-----------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
	UpdateControllers();
	UpdateKeyboard();
	UpdateMouse();
	RunMessagePump(); // Ask Windows to call our registered WinProc function with WM_KEYDOWN notifications, etc.
}

//-----------------------------------------------------------------------------------------------
void InputSystem::EndFrame()
{
}

void InputSystem::UpdateControllers()
{
	for (int i = 0; i < 4; i++)
	{
		XboxController xController = m_controllers[i];
		if (m_controllers[i].isConnected())
		{
			m_controllers[i].clearKeyStates();
			m_controllers[i].updateContoller();
		}
	}
}

//-----------------------------------------------------------------------------------------------
void InputSystem::UpdateKeyboard()
{
	// Clear all just-changed flags, in preparation for the next round of WM_KEYDOWN, etc. messages
	for (int keyCode = 0; keyCode < InputSystem::NUM_KEYS; ++keyCode)
	{
		m_keyStates[keyCode].m_wasJustPressed = false;
		m_keyStates[keyCode].m_wasJustReleased = false;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Update mouse events
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void InputSystem::UpdateMouse()
{
	m_mouseStates.m_wasLButtonJustPressed = false;
	m_mouseStates.m_wasRButtonJustPressed = false;
	m_mouseStates.m_wasLeftDoubleClicked  = false;

	m_mouseStates.m_wasLButtonJustReleased = false;
	m_mouseStates.m_wasRButtonJustReleased = false;

	m_mouse.m_positionLastFrame = m_mouse.m_positionThisFrame;
	m_mouse.m_positionThisFrame = GetMouseClientPosition();
	//ShowCursor(true);
	if (m_mouse.m_mouseMode == MOUSEMODE_RELATIVE) 
	{
		m_mouse.m_positionLastFrame = GetCenterOfClientWindow();
		SetMousePosition(m_mouse.m_positionLastFrame);
		
	}
	//MouseLockToScreen(true);
	if (m_mouse.m_mouseMode == MOUSEMODE_ABSOLUTE)
	{
		//ShowCursor(true);
		//MouseLockToScreen(false);
	}
}

void InputSystem::onKeyPressed(unsigned char keyCode)
{
	if (!m_keyStates[keyCode].m_isKeyDown)
	{
		m_keyStates[keyCode].m_wasJustPressed = true;
	}
	else
	{
		m_keyStates[keyCode].m_wasJustPressed = false;
	}
	m_keyStates[keyCode].m_isKeyDown = true;
}

void InputSystem::onKeyReleased(unsigned char keyCode)
{
	if (m_keyStates[keyCode].m_isKeyDown)
	{
		m_keyStates[keyCode].m_wasJustReleased = true;
	}
	else
	{
		m_keyStates[keyCode].m_wasJustReleased = false;
	}
	m_keyStates[keyCode].m_isKeyDown = false;
}

bool InputSystem::isKeyPressed(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_isKeyDown;
}

bool InputSystem::isKeyReleased(unsigned char keyCode) const
{
	return  !m_keyStates[keyCode].m_isKeyDown;
}

bool InputSystem::wasKeyJustPressed(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_wasJustPressed;
}

bool InputSystem::wasKeyJustReleased(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_wasJustReleased;
}


//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Update mouse button states on click
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::OnLButtonClicked()
{
	if(!m_mouseStates.m_isLButtonDown)
	{
		m_mouseStates.m_wasLButtonJustPressed = true;
	}
	else
	{
		m_mouseStates.m_wasLButtonJustPressed = false;
	}
	m_mouseStates.m_isLButtonDown = true;
	if(m_lastLeftClickTime + 0.5f > static_cast<float>(GetCurrentTimeSeconds()))
	{
		m_mouseStates.m_wasLeftDoubleClicked = true;
	}
	m_lastLeftClickTime = static_cast<float>(GetCurrentTimeSeconds());

}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Update Mousekeystate on mouse RButton click
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::OnRButtonClicked()
{
	if(!m_mouseStates.m_isRButtonDown)
	{
		m_mouseStates.m_wasRButtonJustPressed = true;
	}
	else
	{
		m_mouseStates.m_wasRButtonJustPressed = false;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Update Mousekeystates on LButton release
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::OnLButtonReleased()
{
	m_mouseStates.m_wasLButtonJustReleased = true;
	m_mouseStates.m_isLButtonDown = false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Update MouseKeystates on Rbutton release
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::OnRButtonReleased()
{
	m_mouseStates.m_isRButtonDown = false;
	m_mouseStates.m_wasRButtonJustReleased = true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Update the current Mouse XY Position to MouseKeyStates
*
*@param   : X Cord of Mouse, Y Cord of Mouse
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::UpdateMouseXYPosition(int x, int y)
{
	m_mouseStates.m_mouseXPosition = static_cast<int>(RangeMapInt(x,0,1728,0,980));
	m_mouseStates.m_mouseYPosition = 972 - y;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Get Current Cursor X Position
*
*@param   : NIL
*
*@return  : X Cords of cursor
*/
//////////////////////////////////////////////////////////////
int InputSystem::GetMouseXPosition()
{
	return m_mouseStates.m_mouseXPosition;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Get Mouse Cursor's Y Coords
*
*@param   : NIL
*
*@return  : Cursor Y Cords
*/
//////////////////////////////////////////////////////////////
int InputSystem::GetMouseYPosition()
{
	return m_mouseStates.m_mouseYPosition;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Get Mouse cordinate position mapped to screen position
*
*@param   : NIL
*
*@return  : Screen cordinate of corresponding cursor position
*/
//////////////////////////////////////////////////////////////
Vector2 InputSystem::getRangeMappedMousePosition()
{
	//int width  = 1728;
	//int height = 972;
	return Vector2::ZERO;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 InputSystem::GetMousePosition()
{
	POINT desktop_pos;
	::GetCursorPos(&desktop_pos);

	HWND hwnd = GetActiveWindow();

	::ScreenToClient(hwnd, &desktop_pos);
	POINT client_pos = desktop_pos;

	POINT offset;
	offset.x = 0;
	offset.y = 0;
	::ClientToScreen(hwnd, &offset);

	client_pos.x += offset.x;
	client_pos.y += offset.y;

	return Vector2( static_cast<float>(client_pos.x),  static_cast<float>(client_pos.y));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 InputSystem::GetMouseClientPosition()
{
	POINT desktop_pos;
	::GetCursorPos(&desktop_pos);

	HWND hwnd = (HWND)Windows::GetInstance()->m_hwnd;

	::ScreenToClient(hwnd, &desktop_pos);
	POINT client_pos = desktop_pos;

	return Vector2( static_cast<float>(client_pos.x),  static_cast<float>(client_pos.y));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 InputSystem::GetCenterOfClientWindow()
{
	HWND hwnd = (HWND)Windows::GetInstance()->m_hwnd;
	// Get your windows HWND

	RECT client_rect; // window class RECDT
	::GetClientRect(hwnd, &client_rect);

	return Vector2((client_rect.right + client_rect.left) / 2.f, (client_rect.top + client_rect.bottom) / 2.f);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::SetMouseScreenPosition(Vector2 desktop_pos)
{
	::SetCursorPos( static_cast<int>(desktop_pos.x),static_cast<int>(desktop_pos.y) ); 
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::SetMousePosition(Vector2 position)
{
	HWND hwnd = (HWND)Windows::GetInstance()->m_hwnd;

	RECT client_rect; // window class RECDT
	::GetClientRect(hwnd, &client_rect);

	POINT offset;
	offset.x = 0;//static_cast<int>(position.x);
	offset.y = 0;//static_cast<int>(position.y);
	::ClientToScreen(hwnd, &offset);

	client_rect.left += offset.x;
	client_rect.top += offset.y;

	Vector2 desktopPosition(position.x + client_rect.left, position.y + client_rect.top);
	SetMouseScreenPosition(desktopPosition);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 InputSystem::GetMouseDelta()
{
	return m_mouse.m_positionThisFrame - m_mouse.m_positionLastFrame;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::ShowCursor(bool show)
{
	::ShowCursor(show);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void InputSystem::MouseLockToScreen(bool lock)
{
	if (!lock) {
		::ClipCursor(nullptr); // this unlock the mouse
	}
	else {
		HWND hwnd = GetActiveWindow(); // Get your windows HWND

		RECT client_rect; // window class RECDT
		::GetClientRect(hwnd, &client_rect);

		POINT offset;
		offset.x = 0;
		offset.y = 0;
		::ClientToScreen(hwnd, &offset);

		client_rect.left += offset.x;
		client_rect.right += offset.x;
		client_rect.top += offset.y;
		client_rect.bottom += offset.y;

		::ClipCursor(&client_rect);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Check if Left Mouse button is down
*
*@param   : NIL
*
*@return  : Returns Yes if MouseKeyStates's LButton is down else false
*/
//////////////////////////////////////////////////////////////
bool InputSystem::IsLButtonDown()
{
	return m_mouseStates.m_isLButtonDown;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Check if MouseState's Rbutton is down
*
*@param   : NIL
*
*@return  : Returns 
*/
//////////////////////////////////////////////////////////////
bool InputSystem::IsRButtonDown()
{
	return m_mouseStates.m_isRButtonDown;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Check if Mouse Left is Just clicked
*
*@param   : NIL
*
*@return  : Returns true if LButton is Just clicked else false
*/
//////////////////////////////////////////////////////////////
bool InputSystem::WasLButtonJustPressed() const
{
	return m_mouseStates.m_wasLButtonJustPressed;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Check is MouseKeyState's RButton Just pressed
*
*@param   : NIL
*
*@return  : returns true if RButtongJust pressed is true else false
*/
//////////////////////////////////////////////////////////////
bool InputSystem::WasRButtonJustPressed() const
{
	return m_mouseStates.m_wasRButtonJustPressed;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Check if MouseKeyState LButton Just released
*
*@param   : NIL
*
*@return  : returns true if MouseKeyState LButton Just realsed else false
*/
//////////////////////////////////////////////////////////////
bool InputSystem::WasLButtonJustReleased() const
{
	return m_mouseStates.m_wasLButtonJustReleased;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/19
*@purpose : Check if MouseKeyState RButton Just released value
*
*@param   : NIL
*
*@return  : Returns true if RButton Just Released
*/
//////////////////////////////////////////////////////////////
bool InputSystem::WasRButtonJustReleased() const
{
	return m_mouseStates.m_wasRButtonJustReleased;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/22
*@purpose : Check for Left Double click
*
*@param   : NIL
*
*@return  : mouse state double click value
*/
//////////////////////////////////////////////////////////////
bool InputSystem::WasLeftDoubleClicked() const
{
	return m_mouseStates.m_wasLeftDoubleClicked;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Gets the static instance
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
InputSystem* InputSystem::GetInstance()
{
	if(s_inputSystem == nullptr)
	{
		s_inputSystem = new InputSystem();
	}
	return s_inputSystem;
}

//XboxController&		GetController( int controllerID ) { return m_controllers[ controllerID ]; }

