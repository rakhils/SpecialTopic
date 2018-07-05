#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/Time.hpp"

struct KeyButtonState
{
	char inputChar;
	bool m_wasJustPressed;
	bool m_wasJustReleased;
	bool m_isKeyDown;
};

enum MousMode
{
	MOUSEMODE_RELATIVE,
	MOUSEMODE_ABSOLUTE
};
struct Mouse
{
	Vector2  m_positionLastFrame;
	Vector2  m_positionThisFrame;
	MousMode m_mouseMode = MOUSEMODE_RELATIVE;

};

struct MouseButtonState
{
	bool m_isLButtonDown;
	bool m_wasLButtonJustPressed;
	bool m_wasLButtonJustReleased;

	bool m_wasLeftDoubleClicked;

	bool m_isRButtonDown;
	bool m_wasRButtonJustPressed;
	bool m_wasRButtonJustReleased;
	
	int m_mouseXPosition;
	int m_mouseYPosition;
};
class InputSystem
{
public:
	Mouse m_mouse;
	InputSystem();
	~InputSystem();
	void BeginFrame();
	void EndFrame();
	void UpdateControllers();
	void onKeyPressed(unsigned char keyCode );
	void onKeyReleased(unsigned char keyCode );
	bool isKeyPressed(unsigned char keyCode ) const;
	bool isKeyReleased(unsigned char keyCode ) const;
	bool wasKeyJustPressed(unsigned char keyCode ) const;
	bool wasKeyJustReleased(unsigned char keyCode ) const;

	void OnLButtonClicked();
	void OnRButtonClicked();

	void OnLButtonReleased();
	void OnRButtonReleased();

	void UpdateMouseXYPosition(int x,int y);
	int  GetMouseXPosition();
	int  GetMouseYPosition();
	Vector2 getRangeMappedMousePosition();

	Vector2 GetMousePosition();
	Vector2 GetMouseClientPosition();
	Vector2 GetCenterOfClientWindow();
	void SetMouseScreenPosition(Vector2 desktop_pos);
	void SetMousePosition(Vector2 position);
	Vector2 GetMouseDelta();
	void ShowCursor(bool show);
	void MouseLockToScreen(bool lock);

	bool IsLButtonDown();
	bool IsRButtonDown();
	bool WasLButtonJustPressed() const;
	bool WasRButtonJustPressed() const;
	bool WasLButtonJustReleased() const;
	bool WasRButtonJustReleased() const;
	bool WasLeftDoubleClicked() const;

	XboxController&		GetController( int controllerID ) { return m_controllers[ controllerID ]; }

	static InputSystem*		GetInstance();
public:
	float m_lastLeftClickTime = 0.0f;

	static const int			NUM_KEYS					= 256; 
	static const int			NUM_CONTROLLERS				= 4;
	static const unsigned char	KEYBOARD_BACKSPACE			= 8;
	static const unsigned char	KEYBOARD_SPACE				= 32;
	static const unsigned char	KEYBOARD_ENTER				= 13;
	static const unsigned char	KEYBOARD_ESCAPE				= 27;
	static const unsigned char	KEYBOARD_UP_ARROW			= 38;
	static const unsigned char	KEYBOARD_LEFT_ARROW			= 37;
	static const unsigned char	KEYBOARD_DOWN_ARROW			= 40;
	static const unsigned char	KEYBOARD_RIGHT_ARROW		= 39;
	static const unsigned char	KEYBOARD_SHIFT				= 16;
	static const unsigned char	KEYBOARD_F1					= 112;
	static const unsigned char	KEYBOARD_F2					= 113;
	static const unsigned char	KEYBOARD_F3					= 114;
	static const unsigned char	KEYBOARD_F4					= 115;
	static const unsigned char	KEYBOARD_F5					= 116;
	static const unsigned char	KEYBOARD_F6					= 117;
	static const unsigned char	KEYBOARD_F7					= 118;
	static const unsigned char	KEYBOARD_F8					= 119;
	static const unsigned char	KEYBOARD_F9					= 120;
	static const unsigned char	KEYBOARD_F10				= 121;
	static const unsigned char	KEYBOARD_F11				= 122;
	static const unsigned char	KEYBOARD_F12				= 123;

	static const unsigned char	KEYBOARD_A					= 65;
	static const unsigned char	KEYBOARD_B					= 66;
	static const unsigned char	KEYBOARD_C					= 67;
	static const unsigned char	KEYBOARD_D					= 68;
	static const unsigned char	KEYBOARD_E					= 69;
	static const unsigned char	KEYBOARD_F					= 70;
	static const unsigned char	KEYBOARD_G					= 71;
	static const unsigned char	KEYBOARD_H					= 72;
	static const unsigned char	KEYBOARD_I					= 73;
	static const unsigned char	KEYBOARD_J					= 74;
	static const unsigned char	KEYBOARD_K					= 75;
	static const unsigned char	KEYBOARD_L				    = 76;
	static const unsigned char	KEYBOARD_M				    = 77;
	static const unsigned char	KEYBOARD_N					= 78;
	static const unsigned char	KEYBOARD_O					= 79;
	static const unsigned char	KEYBOARD_P					= 80;
	static const unsigned char	KEYBOARD_Q					= 81;
	static const unsigned char	KEYBOARD_R					= 82;
	static const unsigned char	KEYBOARD_S					= 83;
	static const unsigned char	KEYBOARD_T					= 84;
	static const unsigned char	KEYBOARD_U					= 85;
	static const unsigned char	KEYBOARD_V					= 86;
	static const unsigned char	KEYBOARD_W					= 87;
	static const unsigned char	KEYBOARD_X					= 88;
	static const unsigned char	KEYBOARD_Y					= 89;
	static const unsigned char	KEYBOARD_Z					= 90;

	static const unsigned char	KEYBOARD_0					= 48;
	static const unsigned char	KEYBOARD_1					= 49;
	static const unsigned char	KEYBOARD_2					= 50;
	static const unsigned char	KEYBOARD_3					= 51;
	static const unsigned char	KEYBOARD_4					= 52;
	static const unsigned char	KEYBOARD_5					= 53;
	static const unsigned char	KEYBOARD_6					= 54;
	static const unsigned char	KEYBOARD_7					= 55;
	static const unsigned char	KEYBOARD_8					= 56;
	static const unsigned char	KEYBOARD_9					= 57;

	static InputSystem*			s_inputSystem;
protected:
	void UpdateKeyboard();
	void UpdateMouse();
	//void UpdateControllers();

protected:
	KeyButtonState	m_keyStates[ NUM_KEYS ];
	XboxController	m_controllers[ NUM_CONTROLLERS ];

	MouseButtonState m_mouseStates;

};