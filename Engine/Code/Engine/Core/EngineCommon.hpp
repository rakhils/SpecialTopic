#pragma once
#include "Blackboard.hpp"
#include <stdio.h>
#include <cassert>
#include <iostream>
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/wglext.h"



#define ASSERT(expr)\
    if (!(expr))\
PrintFailureMessage(__FILE__, __LINE__,false)

#define ASSERT_BREAK(expr)\
    if (!(expr))\
	{\
		DebugBreak();\
		PrintFailureMessage(__FILE__, __LINE__,false);\
	}

#define DebugBreak()\
		__debugbreak()

#define UNUSED(x) (void)(x);
#define GL_CHECK_ERROR() GLCheckError(__FILE__, __LINE__)
#define TODO(x) PrintMessage(x)

extern Blackboard g_gameConfigBlackboard;
GLuint LoadShader(char *shaderProgram, GLenum type);
GLuint LoadShaderFromFile(char const *filename, GLenum type);
GLuint CreateAndLinkProgram(GLint vs, GLint fs);
void  *LoadShaderFromFile(char const *filename);
void   LogShaderError(GLuint shader_id);
bool   GLCheckError(char const *file, int line);
bool   DoFileExist(std::string filepath);
void   LogProgramError(GLuint program_id);
void   PrintFailureMessage(char *file, int line,bool breakmode);
void   PrintFailureMessage(char *file, int line,char* message,bool breakmode);
void   PrintMessage(char *message);