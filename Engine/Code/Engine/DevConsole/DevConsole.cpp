#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderDefinitions.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
#include "Engine/Input/InputSystem.hpp"
// CONSTRUCTOR

DevConsole* DevConsole::s_devConsole = nullptr;
DevConsole::DevConsole()
{
}

// DESTRUCTOR
DevConsole::~DevConsole()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Creates instance of DevConsole (singleton)
*
*@param   : NIL
*
*@return  : Dev console
*/
//////////////////////////////////////////////////////////////
DevConsole* DevConsole::GetInstance()
{
	if(s_devConsole == nullptr)
	{
		s_devConsole = new DevConsole();
	}
	return s_devConsole;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Opens dev console values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::Open()
{
	if(m_isActive == false)
	{
		m_wasJustActive = true;
	}
	m_isActive = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Closes dev console values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::Close()
{
	m_isActive = false;
	m_wasJustActive = false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Checks if dev console is active
*@param   : NIL
*@return  : active variable
*//////////////////////////////////////////////////////////////
bool DevConsole::IsDevConsoleOpen()
{
	return m_isActive;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Checks if the dev console was just opened this frame
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DevConsole::WasDevConsoleJustOpened()
{
	return m_wasJustActive;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/03
*@purpose : Update Screen Dimensions
*
*@param   : Mins and max location
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::UpdateDimensions(Vector2 mins, Vector2 maxs)
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	mins = Vector2(0, 0);
	maxs = Vector2(static_cast<float>(width), static_cast<float>(height));
	m_screenBounds = AABB2(mins,maxs);
	m_fontSize = static_cast<int>((maxs.y - mins.y) * m_fontScreenRatio);
	m_inputBox.maxs.x = maxs.x;
	m_inputBox.mins.x = mins.x +2 * static_cast<float>(m_fontSize);
	m_inputBox.mins.y = 2.0f*static_cast<float>(m_fontSize);
	m_inputBox.maxs.y = m_fontSize * 3.0f;
	

	m_outputBox.maxs.x = maxs.x;
	m_outputBox.mins.x = mins.x + 2 * m_fontSize;
	m_outputBox.mins.y = mins.y + (maxs.y - mins.y) * 4 * m_fontScreenRatio;
	m_outputBox.maxs.y = maxs.y;

	m_scrollBarMins = m_outputBox.mins.y;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Check if input string is null
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool DevConsole::IsInputAlreadyCleared()
{
	if(m_inputString == "")
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Check if a piece of text is slected
*
*@param   : NIL
*
*@return  : true if selected false if not
*/
//////////////////////////////////////////////////////k////////
bool DevConsole::IsTextSelected()
{
	if(m_startPoint >= 0 || m_endPoint >= 0)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Checks if prediction is currently up
*@param   : NIL
*@return  : prediction status
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DevConsole::IsPredictionOn()
{
	return m_predictionOn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::Update(float deltaTime)
{
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_M))
	{
		InputSystem::GetInstance()->ShowCursor(true);
		InputSystem::GetInstance()->MouseLockToScreen(false);
		if(InputSystem::GetInstance()->m_mouse.m_mouseMode == MOUSEMODE_RELATIVE)
		{
			InputSystem::GetInstance()->m_mouse.m_mouseMode = MOUSEMODE_ABSOLUTE;
		
		}
		else
		{
			InputSystem::GetInstance()->m_mouse.m_mouseMode = MOUSEMODE_RELATIVE;
		}

	}

	if(m_wasJustActive)
	{
		ProfilerManager::CreateReportFromPreviousFrames();
	}
	ProfilerManager::Update(deltaTime);
	m_wasJustActive = false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Render Console
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::Render()
{
	Renderer *renderer = Renderer::GetInstance();
	Camera::SetCurrentCamera(Camera::GetUICamera());
	//Renderer::GetInstance()->BeginFrame();
	GL_CHECK_ERROR();
	RenderBoundaries(renderer);
	
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	renderer->BindMaterial(material);
	renderer->DrawText2D(m_inputBox.mins, m_inputString, 10.0f, Rgba::WHITE, 1.0f, nullptr);
	delete material;

	RenderSelections(renderer);

	RenderOutputBox(renderer);

	RenderCursor(renderer);
	
	RenderScrollBar(renderer);
	if(m_predictionOn)
	{
		RenderPredictionBox(renderer);
	}
	//ProfilerManager::RenderProfiler();
}


//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Renderer output box
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::RenderOutputBox(Renderer *renderer)
{
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	renderer->BindMaterial(material);
	float positionY = m_outputBox.mins.y + static_cast<float>(m_outputString.size()) * m_fontSize;
	positionY -= (m_lastOutputIndex * m_fontSize);
	for (int index = 0; index < m_outputString.size() - m_lastOutputIndex; index++)
	{
		if(positionY > (m_outputBox.maxs.y - m_fontSize*2))
		{
			positionY -= m_fontSize;
			continue;
		}
		Vector2 startPosition(m_outputBox.mins.x, positionY);
		std::string str = m_outputString.at(index)->m_string;
		Rgba rgb = m_outputString.at(index)->rgba;
		renderer->DrawTextOnPoint(str, 0, static_cast<int>(str.length()), startPosition, static_cast<float>(m_fontSize)/2.0f, rgb);
		positionY -= m_fontSize;
	}
	delete material;
	//renderer->DrawTextOnPointWithCR(m_outputString, m_outputBox.mins, 10.0f, Rgba::WHITE, m_firstOutputIndex);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Draws cursor blink on the input text box
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::RenderCursor(Renderer *renderer)
{
	m_cursorBlinkCount++;
	if (m_cursorBlinkCount < 180)
	{
		return;
	}
	Material *material = Material::AquireResource("default");
	renderer->BindMaterial(material);
	Vector2 position;
	position.x = m_inputBox.mins.x + static_cast<float>(20*(m_commandIndex));
	position.y = m_inputBox.mins.y;
	AABB2 aabb2(position,static_cast<float>(m_fontSize)/6.0f,static_cast<float>(m_fontSize));
	renderer->DrawAABB(aabb2,Rgba::RED);
	if(m_cursorBlinkCount > 360)
	{
		m_cursorBlinkCount = 0;
	}
	delete material;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Render Dev Console boundaries with inputs and outputs
*
*@param   : Renderer to draw elements
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::RenderBoundaries(Renderer *renderer)
{
	Material *material = Material::AquireResource("default");
	renderer->BindMaterial(material);
	int widthOfBoundary = m_fontSize; 
	AABB2 aabbBottom(Vector2(0.0f,0.0f),Vector2(m_screenBounds.maxs.x,static_cast<float>(widthOfBoundary)));
	AABB2 aabbLeft  (Vector2(0.0f,0.0f),Vector2(static_cast<float>(widthOfBoundary),m_screenBounds.maxs.y));

	AABB2 aabbRight (Vector2(m_screenBounds.maxs.x - widthOfBoundary,0.0f),Vector2(m_screenBounds.maxs.x,m_screenBounds.maxs.y));
	AABB2 aabbTop   (Vector2(0.0f,m_screenBounds.maxs.y-widthOfBoundary),Vector2(m_screenBounds.maxs.x,m_screenBounds.maxs.y));

	AABB2 aabbSeperation(Vector2(0.0f,3.0f*static_cast<float>(m_fontSize)),Vector2(m_screenBounds.maxs.x,4.0f*static_cast<float>(m_fontSize)));
	AABB2 inputBox = AABB2(m_inputBox.mins.x - 2*m_fontSize,m_inputBox.mins.y - m_fontSize,m_inputBox.maxs.x,m_inputBox.maxs.y);
	AABB2 outputBox = AABB2(m_outputBox.mins.x - 2 * m_fontSize, m_outputBox.mins.y, m_outputBox.maxs.x, m_outputBox.maxs.y);
	renderer->DrawAABB(inputBox,Rgba::CONSOLE_BLACK);
	renderer->DrawAABB(outputBox,Rgba::CONSOLE_BLACK);
	renderer->DrawAABB(aabbBottom,Rgba::WHITE);
	renderer->DrawAABB(aabbLeft,Rgba::WHITE);
	renderer->DrawAABB(aabbRight,Rgba::WHITE);
	renderer->DrawAABB(aabbTop,Rgba::WHITE);
	renderer->DrawAABB(aabbSeperation, Rgba::WHITE);
	delete material;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Render Shift selected characters
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::RenderSelections(Renderer *renderer)
{
	Material *material = Material::AquireResource("default");
	renderer->BindMaterial(material);
	int lowerValue = m_startPoint;
	int higherValue = m_endPoint;
	if(m_startPoint > m_endPoint)
	{
		lowerValue = m_endPoint;
		higherValue = m_startPoint;
	}
	if(m_startPoint == m_endPoint)
	{
		return;
	}

	float startPointX = static_cast<float>(m_startPoint)*m_fontSize + m_fontSize;
	float endPointX = startPointX + (m_endPoint - m_startPoint)*m_fontSize;
	AABB2 selectionAABB(Vector2(startPointX + m_fontSize/2.0f,m_inputBox.mins.y + m_fontSize),Vector2(endPointX+m_fontSize/2.0f,m_inputBox.mins.y - m_fontSize));
	renderer->DrawAABB(selectionAABB,Rgba::CONSOLE_FADED_BLUE);
	delete material;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Render scroll bar on the right side
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::RenderScrollBar(Renderer *renderer)
{
	Material *material = Material::AquireResource("default");
	renderer->BindMaterial(material);
	float outputBoxHeigth = m_outputBox.maxs.y - m_outputBox.mins.y - 2*m_fontSize; // border
	int numberOfLineCanBeDrawn = static_cast<int>(outputBoxHeigth/static_cast<float>(m_fontSize)*1.0f);
	if(m_outputString.size() > numberOfLineCanBeDrawn)
	{
		float scrollBarHeigthRatio = outputBoxHeigth*numberOfLineCanBeDrawn/m_outputString.size();
		float scrollBarHeight = scrollBarHeigthRatio + m_scrollBarMins;
		//border - scroll width
		m_scrollBar = AABB2(Vector2(m_outputBox.maxs.x - 3*m_fontSize,m_scrollBarMins),Vector2(m_outputBox.maxs.x - 2*m_fontSize,scrollBarHeight));
		renderer->DrawAABB(m_scrollBar,Rgba::BLUE);
	}
	delete material;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Draws the prediction box to choose from
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::RenderPredictionBox(Renderer *renderer)
{
	float boxStartX = m_inputBox.mins.x + m_inputString.size()*m_fontSize;
	float boxStartY = m_inputBox.mins.y + 3*m_fontSize;
	float boxMaxX   = m_screenBounds.maxs.x - 2*m_fontSize;


	AABB2 predictionBox(Vector2(boxStartX - m_fontSize, boxStartY), Vector2(boxMaxX,boxStartY + m_fontSize*m_predictions.size()));
	AABB2 predictionSelectionBox(Vector2(boxStartX - m_fontSize, boxStartY + m_predictionIndex * m_fontSize), Vector2(boxMaxX, boxStartY + (m_predictionIndex + 1)*m_fontSize));
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	renderer->DrawAABB(predictionBox,Rgba::BLACK);
	renderer->DrawRectangle(predictionBox);

	renderer->DrawAABB(predictionSelectionBox, Rgba::BLUE);
	renderer->DrawRectangle(predictionSelectionBox);
	delete defaultMaterial;
	Vector2 startPosition(boxStartX, boxStartY + m_fontSize/2);


	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(material);

	for(int predictionIndex = 0;predictionIndex < m_predictions.size();predictionIndex++)
	{
		CommandDefinition cmdDef = m_predictions.at(predictionIndex);
		std::string commandName = cmdDef.m_name;
		Rgba randomColor = Rgba::GetRandomColor();
		renderer->DrawTextOnPoint(commandName, 0, static_cast<int>(commandName.length()), startPosition, static_cast<float>(m_fontSize)/2.0f, randomColor);
		std::string helpText = cmdDef.m_description;
		int helpTextLength = static_cast<int>(helpText.length());
		Vector2 helpTextStartPos(boxMaxX - m_fontSize - helpTextLength * m_fontSize, startPosition.y);
		renderer->DrawTextOnPoint(helpText, 0, static_cast<int>(helpText.length()), helpTextStartPos, static_cast<float>(m_fontSize) / 2.0f, randomColor);
		startPosition.y += m_fontSize;
	}
	delete material;

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/03
*@purpose : Update each char in input text
*
*@param   : character typed
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::UpdateInputText(char ch)
{
	if(ch == InputSystem::KEYBOARD_BACKSPACE)
	{
		m_shiftPressed = false;
		if(m_startPoint !=-1 && m_endPoint != -1)
		{
			m_inputString.erase(m_inputString.begin() + m_startPoint, m_inputString.begin() + m_endPoint);
			m_commandIndex= m_startPoint;
			m_startPoint = -1;
			m_endPoint = -1;
			return;
		}
		if(m_commandIndex > 0)
		{
			m_inputString.erase(m_inputString.begin()+m_commandIndex-1,m_inputString.begin()+m_commandIndex);
			m_commandIndex--;
		}
		m_startPoint = -1;
		m_endPoint = -1;
		UpdateInputWithPrediction();
		return;
	}
	if(IsTextSelected())
	{
		m_inputString.erase(m_startPoint,m_endPoint-m_startPoint);
		m_commandIndex = m_startPoint;
		ClearTextSelection();
	}
	m_inputString.insert(m_inputString.begin() + m_commandIndex,ch);
	UpdateInputWithPrediction();
	m_commandIndex++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Updates the current input with the prediction box
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::UpdateInputWithPrediction()
{
	ClearPrediction();
	//std::map<std::string, command_cb>::iterator it = commandRegistery.begin();
	//for(;it != commandRegistery.end();it++)
	std::vector<CommandDefinition> cmdDefs = RetrieveAllCommands();
	for(int index = 0;index < cmdDefs.size();index++)
	{
		CommandDefinition cmdDef = cmdDefs.at(index);
		if(StartsWith(cmdDef.m_name,m_inputString))
		{
			m_predictions.push_back(cmdDef);
		}
	}
	if(m_predictions.size() > 0)
	{
		m_predictionOn = true;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Pushes text to output string text
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::PushToOutputText()
{
	PushToOutputText(Rgba::WHITE);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Push Data to Ouput string
*
*@param   : String to output
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::PushToOutputText(std::string str)
{
	PushToOutputText(str,Rgba::WHITE);	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Pushes text to output string text
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::PushToOutputText(Rgba rgb)
{
	Command cmd(m_inputString.c_str());
	OutputBox *outputBox = new OutputBox();
	outputBox->rgba = rgb;
	outputBox->m_string = m_inputString+"\n";
	m_outputString.push_back(outputBox);
	CommandRun(m_inputString.c_str());
	InsertIntoCommandHistory(m_inputString);
	m_inputString.clear();
	m_commandIndex = 0;
	m_commandHistoryIndex = -1;
	m_lastOutputIndex = 0;
	m_scrollBarMins = m_outputBox.mins.y;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose :  Pushes text to output string text
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::PushToOutputText(std::string str, Rgba rgb)
{
	OutputBox *outputBox = new OutputBox();
	outputBox->m_string = str;
	outputBox->rgba = rgb;
	m_outputString.push_back(outputBox);
	m_endPoint = -1;
	m_startPoint = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Updates the input with enter key
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::UpdateInputWithEnterKey()
{
	if(m_predictionOn)
	{
		m_inputString = m_predictions.at(m_predictionIndex).m_name;
		m_commandIndex = static_cast<int>(m_inputString.size());
		ClearPrediction();
		return;
	}
	PushToOutputText();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Input string cursor moves left
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::UpdateInputTextWithLeftKey()
{
	if(m_shiftPressed)
	{
		if(m_startPoint == -1 && m_endPoint == -1)// FIrst shift press
		{
			if(m_commandIndex > 0)
			{
				m_startPoint = m_commandIndex - 1;
				if (m_endPoint == -1)
				{
					m_endPoint = m_commandIndex;
				}
				m_commandIndex--;
				return;
			}
		}
		if (m_commandIndex > 0)
		{
			if(m_commandIndex == m_startPoint)
			{
				m_startPoint--;
			}
			else if(m_commandIndex == m_endPoint)
			{
				m_endPoint--;
			}
			else
			{
				m_startPoint = m_commandIndex-1;
				m_endPoint = m_commandIndex;
			}
		}
	}
	if (m_commandIndex > 0)
	{
		m_commandIndex--;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Update input string cursor to right
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::UpdateInputTextWithRightKey()
{
	if (m_shiftPressed)
	{
		if (m_startPoint == -1 && m_endPoint == -1)// FIrst shift press
		{
			if (m_commandIndex < m_inputString.length())
			{
				m_endPoint = m_commandIndex + 1;
				if (m_startPoint == -1)
				{
					m_startPoint = m_commandIndex;
				}
				m_commandIndex++;
				return;
			}
		}
		if (m_commandIndex < m_inputString.length())
		{
			if (m_commandIndex == m_endPoint)
			{
				m_endPoint++;
			}
			else if (m_commandIndex == m_startPoint)
			{
				m_startPoint++;
			}
			else
			{
				m_startPoint = m_commandIndex + 1;
				m_endPoint = m_commandIndex;
			}
		}
	}
	if (m_commandIndex < m_inputString.length())
	{
		m_commandIndex++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::UpdateInputTextWithUpKey()
{
	if(m_predictionOn)
	{
		m_predictionIndex++;
		m_predictionIndex = ClampInt(m_predictionIndex, 0, static_cast<int>(m_predictions.size() - 1));
		return;
	}
	ScrollHistoryCommandsUp();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::UpdateInputTextWithDownKey()
{
	if (m_predictionOn)
	{
		m_predictionIndex--;
		m_predictionIndex = ClampInt(m_predictionIndex, 0, static_cast<int>(m_predictions.size() - 1));
		return;
	}
	ScrollHistoryCommandsDown();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Deletes character in the input string to the left
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::UpdateInputTextWithDeleteKey()
{
	if (m_startPoint != -1 && m_endPoint != -1)
	{
		m_inputString.erase(m_inputString.begin() + m_startPoint, m_inputString.begin() + m_endPoint);
		m_commandIndex = m_startPoint;
		m_startPoint = -1;
		m_endPoint = -1;
		return;
	}

	if(m_commandIndex < m_inputString.length())
	{
		m_inputString.erase(m_inputString.begin()+m_commandIndex,m_inputString.begin()+m_commandIndex+1);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Clears all input string 
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::UpdateInputTextWithEscapeKey()
{
	if(m_predictionOn)
	{
		ClearPrediction();
		return;
	}

	DevConsole::GetInstance()->ClearInputScreen();
	m_inputString.clear();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Insert command into command histor
*
*@param   : Command
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::InsertIntoCommandHistory(std::string command)
{
	for (int historyIndex = 0; historyIndex < commandHistory.size(); historyIndex++)
	{
		if(commandHistory.at(historyIndex) == command)
		{
			commandHistory.erase(commandHistory.begin()+historyIndex,commandHistory.begin()+historyIndex+1);
			commandHistory.push_back(command);
			return;
		}
	}
	commandHistory.push_back(command);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Prints to Output screen
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ConsolePrintf(Rgba &color, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	PushToOutputText(buffer,color);
	va_end(args);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Prints to ouput screen
*
*@param   : Format("%s") and n number of arguments
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ConsolePrintf(char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer,1000,format,args);
	PushToOutputText(buffer);
	va_end(args);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Clears all input string
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ClearInputScreen()
{
	m_inputString.clear();
	m_commandIndex = 0;
	m_startPoint = -1;
	m_endPoint = -1;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Clears output screen
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ClearOutputScreen()
{
	m_outputString.clear();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Clears all text selected in input string
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ClearTextSelection()
{
	m_startPoint = -1;
	m_endPoint = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Clear all predictions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::ClearPrediction()
{
	m_predictionIndex = 0;
	m_predictionOn = false;
	m_predictions.clear();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/07
*@purpose : Reset history index
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ResetHistoryIndex()
{
	m_commandHistoryIndex = -1;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ScrollHistoryCommandsUp()
{
	int totalHistoryItems = static_cast<int>(commandHistory.size());

	if(totalHistoryItems == 0)
	{
		return;
	}
	if(m_commandHistoryIndex == -1 || m_commandHistoryIndex == 0)
	{
		m_commandHistoryIndex = totalHistoryItems - 1;
	}
	else
	{
		m_commandHistoryIndex--;
	}
	m_inputString.clear();
	if(m_commandHistoryIndex >= 0 && m_commandHistoryIndex < totalHistoryItems)
	{
		m_inputString = commandHistory.at(m_commandHistoryIndex);
		
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ScrollHistoryCommandsDown()
{
	int totalHistoryItems = static_cast<int>(commandHistory.size());
	if (totalHistoryItems == 0)
	{
		return;
	}
	if (m_commandHistoryIndex == -1)
	{
		m_commandHistoryIndex = totalHistoryItems - 1;
	}
	else
	{
		m_commandHistoryIndex++;
	}
	if (m_commandHistoryIndex == totalHistoryItems)
	{
		m_commandHistoryIndex = 0;
	}
	m_inputString.clear();
	
	if (m_commandHistoryIndex >= 0 && m_commandHistoryIndex < totalHistoryItems)
	{
		m_inputString = commandHistory.at(m_commandHistoryIndex);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Scrolls Output text down
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ScrollOutputDown()
{
	float outputBoxHeigth = m_outputBox.maxs.y - m_outputBox.mins.y - 2 * m_fontSize; // border
	int numberOfLineCanBeDrawn = static_cast<int>(outputBoxHeigth / static_cast<float>(m_fontSize)*1.0f);
	if(numberOfLineCanBeDrawn > m_outputString.size())
	{
		return;
	}

	if(m_outputString.size() - m_lastOutputIndex < numberOfLineCanBeDrawn)
	{
		return;
	}

	int diffInLineNumber = static_cast<int>(m_outputString.size()) - numberOfLineCanBeDrawn;
	float diffInScrollHeigth = outputBoxHeigth - (m_scrollBar.maxs.y - m_scrollBar.mins.y);
	float increaseInScrollHeigth = diffInScrollHeigth/diffInLineNumber;

	if(m_lastOutputIndex < m_outputString.size())
	{
		m_lastOutputIndex++;
		m_scrollBarMins += increaseInScrollHeigth;
	}
	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Scrolls output text up
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::ScrollOutputUp()
{
	float outputBoxHeigth = m_outputBox.maxs.y - m_outputBox.mins.y - 2 * m_fontSize; // border
	int numberOfLineCanBeDrawn = static_cast<int>(outputBoxHeigth / static_cast<float>(m_fontSize)*1.0f);
	if (numberOfLineCanBeDrawn > m_outputString.size())
	{
		return;
	}


	int diffInLineNumber = static_cast<int>(m_outputString.size()) - numberOfLineCanBeDrawn;
	float diffInScrollHeigth = outputBoxHeigth - (m_scrollBar.maxs.y - m_scrollBar.mins.y);
	float increaseInScrollHeigth = diffInScrollHeigth / diffInLineNumber;

	if (m_lastOutputIndex > 0)
	{
		m_lastOutputIndex--;
		m_scrollBarMins -= increaseInScrollHeigth;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Sets the shift pressed
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::SetShiftPressed(bool value)
{
	if(m_shiftPressed == false && value == true)
	{
		m_endPoint = -1;
		m_startPoint = -1;
	}
	m_shiftPressed = value;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Sets control press 
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DevConsole::SetControlPressed(bool value)
{
	m_controlPressed = value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Starts up dev console
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DevConsole::StartUp()
{

}
