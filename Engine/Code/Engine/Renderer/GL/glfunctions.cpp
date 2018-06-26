#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/Renderer/Renderer.hpp"

PFNGLCLEARPROC					  glClear					  = nullptr;
PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB   = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB	  = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB  = nullptr;
PFNGLBINDBUFFERPROC				  glBindBuffer		          = nullptr;
PFNGLBUFFERDATAPROC				  glBufferData		          = nullptr;
PFNGLDELETEBUFFERSPROC			  glDeleteBuffers	          = nullptr;
PFNGLGENBUFFERSPROC				  glGenBuffers		          = nullptr;
PFNGLSHADERSOURCEPROC			  glShaderSource	          = nullptr;
PFNGLCOMPILESHADERARBPROC		  glCompileShader	          = nullptr;
PFNGLGETSHADERIVPROC			  glGetShaderiv		          = nullptr;
PFNGLGETSHADERINFOLOGPROC		  glGetShaderInfoLog          = nullptr;
PFNGLCREATEPROGRAMPROC			  glCreateProgram             = nullptr;
PFNGLATTACHSHADERPROC			  glAttachShader	          = nullptr;
PFNGLLINKPROGRAMARBPROC			  glLinkProgram		          = nullptr;
PFNGLGETPROGRAMIVPROC			  glGetProgramiv	          = nullptr;
PFNGLDELETEPROGRAMPROC			  glDeleteProgram	          = nullptr;
PFNGLDETACHSHADERPROC			  glDetachShader	          = nullptr;
PFNGLGETPROGRAMINFOLOGPROC		  glGetProgramInfoLog         = nullptr;
PFNGLGETATTRIBLOCATIONPROC		  glGetAttribLocation		  = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray   = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer 	  = nullptr;
PFNGLUSEPROGRAMPROC				  glUseProgram 				  = nullptr;
PFNGLDRAWARRAYSPROC				  glDrawArrays  			  = nullptr;
PFNGLDELETESHADERPROC			  glDeleteShader			  = nullptr;
PFNGLGENVERTEXARRAYSPROC		  glGenVertexArrays			  = nullptr;
PFNGLBINDVERTEXARRAYPROC          glBindVertexArray			  = nullptr;
PFNGLCREATESHADERPROC			  glCreateShader			  = nullptr;
PFNGLGETUNIFORMLOCATIONPROC 	  glGetUniformLocation        = nullptr;
PFNGLUNIFORMMATRIX4FVPROC	      glUniformMatrix4fv		  = nullptr;
PFNGLGENSAMPLERSPROC		      glGenSamplers			      = nullptr;
PFNGLSAMPLERPARAMETERIPROC		  glSamplerParameteri		  = nullptr;
PFNGLSAMPLERPARAMETERIPROC		  glSamplerParameterf		  = nullptr;
PFNGLDELETESAMPLERSPROC			  glDeleteSamplers			  = nullptr;
PFNGLBINDSAMPLERPROC			  glBindSampler				  = nullptr;
PFNGLACTIVETEXTUREPROC			  glActiveTexture			  = nullptr;
PFNGLBINDTEXTUREPROC			  glBindTexture				  = nullptr;
PFNGLGENERATEMIPMAPPROC			  glGenerateMipmap			  = nullptr;
PFNGLGENTEXTURESPROC			  glGenTextures				  = nullptr;
PFNGLTEXPARAMETERIPROC		      glTexParameteri			  = nullptr;
PFNGLPIXELSTOREIPROC		      glPixelStorei				  = nullptr;
PFNGLTEXIMAGE2DPROC				  glTexImage2D				  = nullptr;
PFNGLENABLEPROC			   		  glEnable					  = nullptr;
PFNGLDISABLEPROC			   	  glDisable					  = nullptr;
PFNGLCULLFACEPROC				  glCullFace				  = nullptr;
PFNGLBLENDFUNCPROC			      glBlendFunc				  = nullptr;
PFNGLDEPTHFUNCPROC			      glDepthFunc				  = nullptr;
PFNGLDEPTHMASKPROC			      glDepthMask				  = nullptr;
PFNGLCLEARDEPTHFPROC			  glClearDepthf				  = nullptr;
PFNGLGENFRAMEBUFFERSPROC		  glGenFramebuffers			  = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC		  glDeleteFramebuffers        = nullptr;
PFNGLBINDFRAMEBUFFERPROC		  glBindFramebuffer           = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC	      glFramebufferTexture	      = nullptr;
PFNGLDRAWBUFFERSPROC		      glDrawBuffers               = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus    = nullptr;
PFNGLBLITFRAMEBUFFERPROC		  glBlitFramebuffer			  = nullptr;
PFNGLGETERRORPROC				  glGetError				  = nullptr;
PFNGLREADBUFFERPROC               glReadBuffer				  = nullptr;
PFNGLREADPIXELSPROC               glReadPixels				  = nullptr;
PFNGLUNIFORM1FPROC				  glUniform1f				  = nullptr;
PFNGLUNIFORM1FVPROC				  glUniform1fv				  = nullptr;
PFNGLUNIFORM3FVPROC				  glUniform3fv				  = nullptr;
PFNGLUNIFORM4FPROC				  glUniform4f				  = nullptr;
PFNGLUNIFORM4FVPROC				  glUniform4fv				  = nullptr;
PFNGLSCALEXOESPROC   			  glScalexOES				  = nullptr;
PFNGLDRAWELEMENTSPROC			  glDrawElements			  = nullptr;
PFNGLPOLYGONMODEPROC			  glPolygonMode				  = nullptr;
PFNGLFRONTFACEPROC				  glFrontFace				  = nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC	  glBlendEquationSeparate	  = nullptr;
PFNGLBLENDFUNCSEPARATEPROC		  glBlendFuncSeparate		  = nullptr;
PFNGLBINDBUFFERBASEPROC		      glBindBufferBase			  = nullptr;
PFNGLGETUNIFORMBLOCKINDEXPROC	  glGetUniformBlockIndex	  = nullptr;
PFNGLUNIFORMBLOCKBINDINGPROC	  glUniformBlockBinding		  = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC  glGetActiveUniformBlockiv   = nullptr;
PFNGLGETACTIVEUNIFORMSIVPROC	  glGetActiveUniformsiv       = nullptr;
PFNGLGETACTIVEUNIFORMPROC		  glGetActiveUniform		  = nullptr;
PFNGLDELETETEXTURESPROC			  glDeleteTextures			  = nullptr;
PFNGLTEXSTORAGE2DPROC			  glTexStorage2D			  = nullptr;
PFNGLTEXSUBIMAGE2DPROC			  glTexSubImage2D			  = nullptr;
PFNGLCLEARCOLORPROC				  glClearColor				  = nullptr;
PFNGLVIEWPORTPROC				  glViewport				  = nullptr;

glfunctions::glfunctions()
{

}

// DESTRUCTOR
glfunctions::~glfunctions()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Binds all gl new functions
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void glfunctions::BindGLFunctions()
{
	GL_BIND_FUNCTION(glClear);
	GL_BIND_FUNCTION(glCreateShader);
	GL_BIND_FUNCTION(glDeleteBuffers);
	GL_BIND_FUNCTION(glGenBuffers);
	GL_BIND_FUNCTION(glBindBuffer);
	GL_BIND_FUNCTION(glBufferData);
	GL_BIND_FUNCTION(glDeleteShader);
	GL_BIND_FUNCTION(glShaderSource);
	GL_BIND_FUNCTION(glCompileShader);
	GL_BIND_FUNCTION(glGetShaderiv);
	GL_BIND_FUNCTION(glGetShaderInfoLog);
	GL_BIND_FUNCTION(glCreateProgram);
	GL_BIND_FUNCTION(glAttachShader);
	GL_BIND_FUNCTION(glLinkProgram);
	GL_BIND_FUNCTION(glGetProgramiv);
	GL_BIND_FUNCTION(glDeleteProgram);
	GL_BIND_FUNCTION(glDetachShader);
	GL_BIND_FUNCTION(glGetProgramInfoLog);
	GL_BIND_FUNCTION(glGenVertexArrays);
	GL_BIND_FUNCTION(glBindVertexArray);
	GL_BIND_FUNCTION(glGetAttribLocation);
	GL_BIND_FUNCTION(glVertexAttribPointer);
	GL_BIND_FUNCTION(glEnableVertexAttribArray);
	GL_BIND_FUNCTION(glDrawArrays);
	GL_BIND_FUNCTION(glUseProgram);
	GL_BIND_FUNCTION(glGetUniformLocation);
	GL_BIND_FUNCTION(glUniformMatrix4fv);
	GL_BIND_FUNCTION(glGenSamplers);
	GL_BIND_FUNCTION(glSamplerParameteri);
	GL_BIND_FUNCTION(glSamplerParameterf);
	GL_BIND_FUNCTION(glDeleteSamplers);
	GL_BIND_FUNCTION(glBindSampler);
	GL_BIND_FUNCTION(glBindTexture);
	GL_BIND_FUNCTION(glActiveTexture);
	GL_BIND_FUNCTION(glGenTextures);
	GL_BIND_FUNCTION(glTexParameteri);
	GL_BIND_FUNCTION(glPixelStorei);
	GL_BIND_FUNCTION(glTexImage2D);
	GL_BIND_FUNCTION(glEnable);
	GL_BIND_FUNCTION(glDisable);
	GL_BIND_FUNCTION(glBlendFunc);
	GL_BIND_FUNCTION(glCullFace);
	GL_BIND_FUNCTION(glDepthFunc);
	GL_BIND_FUNCTION(glDepthMask);
	GL_BIND_FUNCTION(glClearDepthf);
	GL_BIND_FUNCTION(glGenFramebuffers);
	GL_BIND_FUNCTION(glDeleteFramebuffers);
	GL_BIND_FUNCTION(glBindFramebuffer);
	GL_BIND_FUNCTION(glFramebufferTexture);
	GL_BIND_FUNCTION(glDrawBuffers);
	GL_BIND_FUNCTION(glCheckFramebufferStatus);
	GL_BIND_FUNCTION(glBlitFramebuffer);
	GL_BIND_FUNCTION(glGetError);
	GL_BIND_FUNCTION(glReadBuffer);
	GL_BIND_FUNCTION(glReadPixels);
	GL_BIND_FUNCTION(glUniform1f);
	GL_BIND_FUNCTION(glUniform1fv);
	GL_BIND_FUNCTION(glScalexOES);
	GL_BIND_FUNCTION(glDrawElements);
	GL_BIND_FUNCTION(glPolygonMode);
	GL_BIND_FUNCTION(glUniform3fv);
	GL_BIND_FUNCTION(glUniform4f);
	GL_BIND_FUNCTION(glUniform4fv);
	GL_BIND_FUNCTION(glFrontFace);
	GL_BIND_FUNCTION(glBlendEquationSeparate);
	GL_BIND_FUNCTION(glBlendFuncSeparate);
	GL_BIND_FUNCTION(glBindBufferBase);
	GL_BIND_FUNCTION(glGetUniformBlockIndex);
	GL_BIND_FUNCTION(glUniformBlockBinding);
	GL_BIND_FUNCTION(glGetActiveUniformBlockiv);
	GL_BIND_FUNCTION(glGetActiveUniformsiv);
	GL_BIND_FUNCTION(glGetActiveUniform);
	GL_BIND_FUNCTION(glDeleteTextures);
	GL_BIND_FUNCTION(glTexStorage2D);
	GL_BIND_FUNCTION(glTexSubImage2D);
	GL_BIND_FUNCTION(glClearColor);
	GL_BIND_FUNCTION(glGenerateMipmap);
	GL_BIND_FUNCTION(glViewport);

}
