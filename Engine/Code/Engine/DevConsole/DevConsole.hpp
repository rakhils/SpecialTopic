#pragma once
//#include <windows.h>
#include <stdarg.h>
#include "Engine/Core/Windows.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/DevConsole/Command.hpp"
/*\class  : DevConsole		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   : Console to view commands in window class		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 2/2/2018 10:26:23 PM
* \contact: srsrakhil@gmail.com
*/

struct OutputBox 
{
	std::string m_string;
	Rgba rgba;
};

class DevConsole
{

public:
	static DevConsole *s_devConsole;
	static DevConsole* GetInstance();
	
	//Member_Variables

	bool							m_isActive             = false;
	bool							m_wasJustActive		   = false;
	int								m_cursorBlinkCount     = 0;
	int								m_fontSize			   = 0;
	int								m_commandHistoryIndex  = 0;
	int								m_lastOutputIndex      = 0;
	int								m_commandIndex		   = 0;
	float							m_fontScreenRatio      = 1.0f/50.0f;
	AABB2							m_screenBounds         = AABB2(0, 0, 0, 0);
	AABB2							m_outputBox		       = AABB2(0, 0, 0, 0);
	AABB2							m_inputBox			   = AABB2(0, 0, 0, 0);
	AABB2							m_scrollBar		       = AABB2(0,0,0,0);
	std::string						m_inputString          = "";
	bool							m_shiftPressed         = false;
	bool							m_controlPressed	   = false;
	float							m_scrollBarMins	       = 0.0f;
	int								m_startPoint		   = -1;
	int								m_endPoint			   = -1;
	bool							m_predictionOn		   = false;
	int								m_predictionIndex      = 0;
	std::vector<CommandDefinition>  m_predictions;
	std::vector<OutputBox*>		    m_outputString;
	bool							m_isDevConsoleOuputEnabled = true;
private:
	DevConsole();
	~DevConsole();
public:
	void Open();
	void Close();
	bool IsDevConsoleOpen();
	bool WasDevConsoleJustOpened();
	bool IsInputAlreadyCleared();
	bool IsTextSelected();
	bool IsPredictionOn();
	
	void Update(float deltaTime);
	void UpdateDimensions(Vector2 mins,Vector2 maxs);
	void Render();
	void RenderOutputBox(Renderer *renderer);
	void RenderCursor(Renderer *renderer);
	void RenderBoundaries(Renderer *renderer);
	void RenderSelections(Renderer *renderer);
	void RenderScrollBar(Renderer *renderer);
	void RenderPredictionBox(Renderer *renderer);
	void RenderRCSInfo(Renderer *renderer);

	void UpdateInputText(char ch);
	void UpdateInputWithPrediction();
	void PushToOutputText();
	void PushToOutputText(Rgba rgb);
	void PushToOutputText(std::string);
	void PushToOutputText(std::string,Rgba rgb);
	void UpdateInputWithEnterKey();
	void UpdateInputTextWithLeftKey();
	void UpdateInputTextWithRightKey();
	void UpdateInputTextWithUpKey();
	void UpdateInputTextWithDownKey();
	void UpdateInputTextWithDeleteKey();
	void UpdateInputTextWithEscapeKey();
	void UpdateInputTextWithShiftKey();

	void InsertIntoCommandHistory(std::string command);

	void ConsolePrintf(Rgba &color, char const *format, ...);
	void ConsolePrintf(char const *format, ...);
	void ClearInputScreen();
	void ClearOutputScreen();
	void ClearTextSelection();
	void ClearPrediction();
	void ResetHistoryIndex();
	void ScrollHistoryCommandsUp();
	void ScrollHistoryCommandsDown();

	void ScrollOutputDown();
	void ScrollOutputUp();

	void SetShiftPressed(bool value);
	void SetControlPressed(bool value);
	//Static_Methods
	static void StartUp();
protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};