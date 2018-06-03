#include "Engine/Core//EngineCommon.hpp"
#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/FileUtil/File.h"
#include <stdarg.h>
#include <ctype.h>

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/28
*@purpose : Load shader from the shader input
*
*@param   : shader program , type of shader to load
*
*@return  : ShaderId
*/
//////////////////////////////////////////////////////////////
GLuint LoadShader(char* shaderProgram, GLenum type)
{
	GLuint shader_id = glCreateShader(type);
	_ASSERT(shader_id != NULL);

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(shaderProgram);
	glShaderSource(shader_id, 1, &shaderProgram, &shader_length);
	glCompileShader(shader_id);

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		LogShaderError(shader_id); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}
	//free(shaderProgram);

	return shader_id;
}

GLuint LoadShaderFromFile(char const *filename, GLenum type)
{
	char *src = (char*)FileReadToBuffer(filename);
	_ASSERT(src != nullptr);

	// Create a shader
	GLuint shader_id = glCreateShader(type);
	_ASSERT(shader_id != NULL);
	
	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(src);
	glShaderSource(shader_id, 1, &src, &shader_length);
	glCompileShader(shader_id);

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		LogShaderError(shader_id); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}
	free(src);

	return shader_id;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/31
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void * LoadShaderFromFile(char const *filename)
{
	return (char*)FileReadToBuffer(filename);
}

void LogShaderError(GLuint shader_id)
{
	// figure out how large the buffer needs to be
	GLint length;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

	// make a buffer, and copy the log to it. 
	char *buffer = new char[length + 1];
	glGetShaderInfoLog(shader_id, length, &length, buffer);

	// Print it out (may want to do some additional formatting)
	buffer[length] = NULL;
	//DebuggerPrintf(L"My output string.");
	//Logf("class", buffer);
	//DEBUGBREAK();
	PrintFailureMessage(__FILE__, __LINE__,false);
	char msgbuf[1000];
	sprintf_s(msgbuf, "ERROR : %s ::\n", buffer);
	OutputDebugStringA(msgbuf);
	// free up the memory we used. 
	delete buffer;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/20
*@purpose : Checks for gl errors
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool GLCheckError(char const *file, int line)
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		char msgbuf[1000];
		sprintf_s(msgbuf, "GL ERROR [0x%04x] at [%s(%i)]", error, file, line);
		OutputDebugStringA(msgbuf);
		return true;
	}
	return false;
}

GLuint CreateAndLinkProgram(GLint vs, GLint fs)
{
	// credate the program handle - how you will reference
	// this program within OpenGL, like a texture handle
	GLuint program_id = glCreateProgram();
	_ASSERT(program_id != 0);

	// Attach the shaders you want to use
	glAttachShader(program_id, vs);
	glAttachShader(program_id, fs);

	// Link the program (create the GPU program)
	glLinkProgram(program_id);

	// Check for link errors - usually a result
	// of incompatibility between stages.
	GLint link_status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

	if (link_status == GL_FALSE) 
	{
		LogProgramError(program_id);
		glDeleteProgram(program_id);
		program_id = 0;
	}

	// no longer need the shaders, you can detach them if you want
	// (not necessary)
	glDetachShader(program_id, vs);
	glDetachShader(program_id, fs);

	return program_id;
}

void LogProgramError(GLuint program_id)
{
	// get the buffer length
	GLint length;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

	// copy the log into a new buffer
	char *buffer = new char[length + 1];
	glGetProgramInfoLog(program_id, length, &length, buffer);

	// print it to the output pane
	buffer[length] = NULL;
	char msgbuf[1000];
	sprintf_s(msgbuf, "GL PROGRAM ERROR %s", buffer);
	OutputDebugStringA(msgbuf);
	//DEBUGBREAK();

	// cleanup
	delete buffer;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/28
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void PrintFailureMessage(char *file, int line,bool breakmode)
{
	char msgbuf[1000];
	sprintf_s(msgbuf, "FILE : %s :: Line Num %d\n",file,line);
	OutputDebugStringA(msgbuf);
	if(breakmode)
	{
		DebugBreak();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/28
*@purpose : PPrints the given error messaage
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void PrintFailureMessage(char *file, int line, char* message, bool breakmode)
{
	char msgbuf[1000];
	sprintf_s(msgbuf, "FILE : %s :: Line Num %d\n%s\n", file, line,message);
	OutputDebugStringA(msgbuf);
	if (breakmode)
	{
		DebugBreak();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Prints message in console
*@param   : Message
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PrintMessage(char *message)
{
	if(true)
	{
		return;
	}
	char msgbuf[1000];
	sprintf_s(msgbuf, "=======================\nTODO : %s\n====================", message);
	OutputDebugStringA(msgbuf);
}

