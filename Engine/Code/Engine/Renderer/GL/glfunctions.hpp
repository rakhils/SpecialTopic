#pragma once
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/wglext.h"

/*\class  : glfunctions
*		   
* \group  : <GroupName>
*		   
* \brief  : All GL Related functions are functions declared in global scope
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 1/24/2018 4:41:05 PM
*
* \contact: srsrakhil@gmail.com
*/


extern PFNGLCLEARPROC					 glClear;
extern PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNGLDELETEBUFFERSPROC			 glDeleteBuffers;
extern PFNGLGENBUFFERSPROC				 glGenBuffers;
extern PFNGLBINDBUFFERPROC				 glBindBuffer;
extern PFNGLBUFFERDATAPROC				 glBufferData;
extern PFNGLDELETESHADERPROC			 glDeleteShader;       
extern PFNGLSHADERSOURCEPROC			 glShaderSource;
extern PFNGLCOMPILESHADERARBPROC		 glCompileShader;
extern PFNGLGETSHADERIVPROC				 glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC		 glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC			 glCreateProgram;
extern PFNGLATTACHSHADERPROC			 glAttachShader;
extern PFNGLLINKPROGRAMARBPROC			 glLinkProgram;
extern PFNGLGETPROGRAMIVPROC			 glGetProgramiv;
extern PFNGLDELETEPROGRAMPROC			 glDeleteProgram;
extern PFNGLDETACHSHADERPROC			 glDetachShader;
extern PFNGLGETPROGRAMINFOLOGPROC		 glGetProgramInfoLog;
extern PFNGLGETATTRIBLOCATIONPROC		 glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLUSEPROGRAMPROC				 glUseProgram;
extern PFNGLDRAWARRAYSPROC				 glDrawArrays;
extern PFNGLGENVERTEXARRAYSPROC			 glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
extern PFNGLCREATESHADERPROC		     glCreateShader;
extern PFNGLGETUNIFORMLOCATIONPROC 		 glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC	     glUniformMatrix4fv;
extern PFNGLGENSAMPLERSPROC				 glGenSamplers;
extern PFNGLSAMPLERPARAMETERIPROC		 glSamplerParameteri;
extern PFNGLDELETESAMPLERSPROC			 glDeleteSamplers;
extern PFNGLBINDSAMPLERPROC				 glBindSampler;
extern PFNGLACTIVETEXTUREPROC			 glActiveTexture;
extern PFNGLBINDTEXTUREPROC				 glBindTexture;
extern PFNGLGENTEXTURESPROC				 glGenTextures;
extern PFNGLTEXPARAMETERIPROC		     glTexParameteri;
extern PFNGLPIXELSTOREIPROC				 glPixelStorei;
extern PFNGLTEXIMAGE2DPROC				 glTexImage2D;
extern PFNGLENABLEPROC					 glEnable;
extern PFNGLDISABLEPROC			   	     glDisable;
extern PFNGLBLENDFUNCPROC			     glBlendFunc;
extern PFNGLCULLFACEPROC				 glCullFace;
extern PFNGLDEPTHFUNCPROC			     glDepthFunc;
extern PFNGLDEPTHMASKPROC			     glDepthMask;
extern PFNGLCLEARDEPTHFPROC				 glClearDepthf;
extern PFNGLGENFRAMEBUFFERSPROC			 glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC		 glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC		     glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC	     glFramebufferTexture;
extern PFNGLDRAWBUFFERSPROC			     glDrawBuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus;
extern PFNGLBLITFRAMEBUFFERPROC			 glBlitFramebuffer;
extern PFNGLGETERRORPROC				 glGetError;
extern PFNGLREADBUFFERPROC               glReadBuffer;
extern PFNGLREADPIXELSPROC               glReadPixels;
extern PFNGLPOLYGONMODEPROC				 glPolygonMode;

extern PFNGLSCALEXOESPROC   			 glScalexOES;
extern PFNGLUNIFORM1FPROC				 glUniform1f;
extern PFNGLUNIFORM1FVPROC				 glUniform1fv;
extern PFNGLUNIFORM3FVPROC				 glUniform3fv;
extern PFNGLUNIFORM4FPROC				 glUniform4f;
extern PFNGLUNIFORM4FVPROC				 glUniform4fv;
extern PFNGLDRAWELEMENTSPROC			 glDrawElements;
extern PFNGLFRONTFACEPROC				 glFrontFace;
extern PFNGLBLENDEQUATIONSEPARATEPROC	 glBlendEquationSeparate;
extern PFNGLBLENDFUNCSEPARATEPROC		 glBlendFuncSeparate;
extern PFNGLBINDBUFFERBASEPROC		     glBindBufferBase;
extern PFNGLGETUNIFORMBLOCKINDEXPROC	 glGetUniformBlockIndex;
extern PFNGLUNIFORMBLOCKBINDINGPROC		 glUniformBlockBinding;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC  glGetActiveUniformBlockiv;
extern PFNGLGETACTIVEUNIFORMSIVPROC		 glGetActiveUniformsiv;
extern PFNGLGETACTIVEUNIFORMPROC		 glGetActiveUniform;
extern PFNGLDELETETEXTURESPROC			 glDeleteTextures;
extern PFNGLTEXSTORAGE2DPROC			 glTexStorage2D;
extern PFNGLTEXSUBIMAGE2DPROC			 glTexSubImage2D;
extern PFNGLCLEARCOLORPROC				 glClearColor;

//extern PFNGLPUSHMA

template <typename T>
bool wglGetTypedProcAddress(T *out, char const *name)
{
	// Grab the function from the currently bound render contect
	// most opengl 2.0+ features will be found here
	*out = (T)wglGetProcAddress(name);

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T)GetProcAddress(gGLLibrary, name);
	}

	return (*out != nullptr);
}

#define GL_BIND_FUNCTION(f)         wglGetTypedProcAddress( &f, #f )

class glfunctions
{

public:
	//Member_Variables
	
	//Static_Member_Variables
	//Methods

	glfunctions();
	~glfunctions();
	
	//Static_Methods
	static void BindGLFunctions();
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