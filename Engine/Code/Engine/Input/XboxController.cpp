#include "XboxController.hpp"
#include <stdio.h>
#include <Windows.h>
#include "Engine\Math\MathUtil.hpp"
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" )

unsigned short		XboxController::VK_DPadLeft = 4;
unsigned short		XboxController::VK_DPadRight = 8;
unsigned short		XboxController::VK_DPadTop = 1;
unsigned short		XboxController::VK_DpadBottom = 2;
unsigned short		XboxController::VK_X = 16384;
unsigned short		XboxController::VK_Y = 32768;
unsigned short		XboxController::VK_A = 4096;
unsigned short		XboxController::VK_B = 8192;
unsigned short		XboxController::VK_LeftBumper = 256;
unsigned short		XboxController::VK_RigthBumper = 512;
unsigned short		XboxController::VK_Menu = 16;
unsigned short		XboxController::VK_View = 32;
unsigned short		XboxController::VK_LeftThumb = 64;
unsigned short		XboxController::VK_RigthThumb = 128;

XboxController::XboxController()
{
	for(int i = 0;i < 14;i++)
	{
		XInputState[i].m_wasJustPressed = false;
		XInputState[i].m_wasJustReleased = false;
		XInputState[i].m_isKeyDown = false;
	}
}

void XboxController::updateContoller()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerNumber, &xboxControllerState );
	if( errorStatus == ERROR_SUCCESS )
	{
		updateButton(xboxControllerState.Gamepad.wButtons , VK_DPadLeft,	INP_DPadLeft);		 
		updateButton(xboxControllerState.Gamepad.wButtons , VK_DPadRight,	INP_DPadRight);	
		updateButton(xboxControllerState.Gamepad.wButtons , VK_DPadTop,		INP_DPadTop);	
		updateButton(xboxControllerState.Gamepad.wButtons , VK_DpadBottom,	INP_DpadBottom);	
		updateButton(xboxControllerState.Gamepad.wButtons , VK_X,			INP_X);			
		updateButton(xboxControllerState.Gamepad.wButtons , VK_Y,			INP_Y);			
		updateButton(xboxControllerState.Gamepad.wButtons , VK_A,			INP_A);			
		updateButton(xboxControllerState.Gamepad.wButtons , VK_B,			INP_B);			
		updateButton(xboxControllerState.Gamepad.wButtons , VK_LeftBumper,	INP_LeftBumper);	
		updateButton(xboxControllerState.Gamepad.wButtons , VK_RigthBumper,	INP_RigthBumper);	
		updateButton(xboxControllerState.Gamepad.wButtons , VK_Menu,		INP_Menu);			
		updateButton(xboxControllerState.Gamepad.wButtons , VK_View,		INP_View);			
		updateButton(xboxControllerState.Gamepad.wButtons , VK_LeftThumb,   INP_LeftThumb);	
		updateButton(xboxControllerState.Gamepad.wButtons , VK_RigthThumb,  INP_RigthThumb);	
	}
	CalculateRightStick();
	CalculateLeftStick();
	updateRightTrigger();
}

void XboxController::updateButton(unsigned short button,unsigned short flags,int index)
{
	if(button == 0)
	{
		XInputState[index].m_isKeyDown = false;
	}
	if((button & flags) == flags)
	{
		if(XInputState[index].m_isKeyDown)
		{
			XInputState[index].m_wasJustPressed = false;
		}
		else
		{
			XInputState[index].m_wasJustPressed = true;
		}
		XInputState[index].m_isKeyDown = true;
	}
	else
	{
		XInputState[index].m_isKeyDown = false;
	}
}

XboxController::XboxController(int controllerNumber)
{
	controllerNumber = 0;
}

bool XboxController::isConnected()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerNumber, &xboxControllerState );
	if( errorStatus == ERROR_DEVICE_NOT_CONNECTED )
	{
		return false;
	}
	return true;
}

void XboxController::clearKeyStates()
{
	for(int i = 0;i < 14;i++)
	{
		XInputState[i].m_wasJustPressed = false;
		XInputState[i].m_wasJustReleased = false;
	}
	leftTrigger = false;
	rightTrigger = false;
}

bool XboxController::WasKeyJustPressed(unsigned char keycode) const
{
	return (XInputState[keycode].m_wasJustPressed);
}

void XboxController::CalculateRightStick()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	XInputGetState( m_controllerNumber, &xboxControllerState );

	float sX = xboxControllerState.Gamepad.sThumbRX;
	float sY = xboxControllerState.Gamepad.sThumbRY;
	RightJoyStick.RawX = sX;
	RightJoyStick.RawY = sY;
	float fX = RangeMapFloat(sX,-32768.0f,32768.0f,-1.f,1.f);
	float fY = RangeMapFloat(sY,-32768.0f,32768.0f,-1.f,1.f);
	float Theta = Atan2Degrees(fY,fX);
	RightJoyStick.degrees = Theta;
	float R = Vector2(fX, fY).GetLength();

	float correctedR = RangeMapFloat(R,RightJoyStick.innerDeadZone,RightJoyStick.outerDeadZone,0.0f,1.0f);
	
	if(correctedR > 1) correctedR = 1;
	if(correctedR < 0) correctedR = 0;
	RightJoyStick.Rx = correctedR*CosDegrees(Theta);
	RightJoyStick.Ry = correctedR*SinDegrees(Theta);
}

void XboxController::CalculateLeftStick()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	XInputGetState( m_controllerNumber, &xboxControllerState );

	float sX = xboxControllerState.Gamepad.sThumbLX;
	float sY = xboxControllerState.Gamepad.sThumbLY;
	LeftJoyStick.RawX = sX;
	LeftJoyStick.RawY = sY;
	float fX = RangeMapFloat(sX,-32768.0f,32768.0f,-1.f,1.f);
	float fY = RangeMapFloat(sY,-32768.0f,32768.0f,-1.f,1.f);
	float Theta = Atan2Degrees(fY,fX);
	LeftJoyStick.degrees = Theta;
	float R = Vector2(fX, fY).GetLength();

	float correctedR = RangeMapFloat(R,LeftJoyStick.innerDeadZone,LeftJoyStick.outerDeadZone,0.0f,1.0f);

	if(correctedR > 1) correctedR = 1;
	if(correctedR < 0) correctedR = 0;
	LeftJoyStick.Rx = correctedR*CosDegrees(Theta);
	LeftJoyStick.Ry = correctedR*SinDegrees(Theta);
}

Vector2 XboxController::getLeftStickVector()
{
	Vector2 movementVector(LeftJoyStick.Rx,LeftJoyStick.Ry);
	return movementVector;
}

Vector2 XboxController::getRigthStickVector()
{
	Vector2 movementVector(RightJoyStick.Rx,RightJoyStick.Ry);
	return movementVector;
}

void XboxController::updateRightTrigger()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	XInputGetState( m_controllerNumber, &xboxControllerState );
	short int value = xboxControllerState.Gamepad.bRightTrigger;
	if(value >= 128)
	{
		rightTrigger = true;
		return;
	}
	rightTrigger = false;
}

