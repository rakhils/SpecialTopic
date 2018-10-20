#pragma once
#include <unordered_map>

#include <windows.h>		
#include <cassert>
#include <crtdbg.h>
#include <algorithm>
#include <time.h>

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"

#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/wglext.h"

enum eCompare
{
	COMPARE_NEVER,         // GL_NEVER
	COMPARE_LESS,          // GL_LESS
	COMPARE_LEQUAL,        // GL_LEQUAL
	COMPARE_GREATER,       // GL_GREATER
	COMPARE_GEQUAL,        // GL_GEQUAL
	COMPARE_EQUAL,         // GL_EQUAL
	COMPARE_NOT_EQUAL,     // GL_NOTEQUAL
	COMPARE_ALWAYS         // GL_ALWAYS
};
enum DrawPrimitiveType
{
	PRIMITIVE_POINTS,		// in OpenGL, for example, this becomes GL_POINTS
	PRIMITIVE_LINES,		// in OpenGL, for example, this becomes GL_LINES
	PRIMITIVE_TRIANGES,		// in OpenGL, for example, this becomes GL_TRIANGLES
	PRIMITIVE_QUADS,		// in OpenGL, for example, this becomes GL_QUADS
	NUM_PRIMITIVE_TYPES
};
enum eTextureCubeSide
{
	TEXCUBE_RIGHT,
	TEXCUBE_LEFT,
	TEXCUBE_TOP, 
	TEXCUBE_BOTTOM,
	TEXCUBE_FRONT,
	TEXCUBE_BACK
};
enum TextDrawMode
{
	TEXT_DRAW_SHRINK_TO_FIT,
	TEXT_DRAW_WORD_WRAP1,
	TEXT_DRAW_WORD_WRAP,
	TEXT_DRAW_OVERRUN
};
enum eRenderDataType
{
	RDT_FLOAT,           // GL_FLOAT
	RDT_UNSIGNED_BYTE    // GL_UNSIGNED_BYTE
};

enum eBlendOperation
{
	COMPARE_ADD,
	COMPARE_SUBTRACT,
	COMPARE_REVERSE_SUBTRACT,
	COMPARE_MIN,
	COMPARE_MAX
};

enum eBlendFactor
{
	BLEND_ONE,
	BLEND_ZERO,
	BLEND_SRC_ALPHA,
	BLEND_INV_SRC_ALPHA 
};
enum eCullMode
{
	CULLMODE_BACK,          // GL_BACK     glEnable(GL_CULL_FACE); glCullFace(GL_BACK); 
	CULLMODE_FRONT,         // GL_FRONT    glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); 
	CULLMODE_NONE          // GL_NONE     glDisable(GL_CULL_FACE)
};

enum eFillMode
{
	FILLMODE_SOLID,         // GL_FILL     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL )
	FILLMODE_WIRE          // GL_LINE     glPolygonMode( GL_FRONT_AND_BACK, GL_LINE )
};

enum eWindOrder
{
	WIND_COUNTER_CLOCKWISE,  // GL_CCW      glFrontFace( GL_CCW ); 
	WIND_CLOCKWISE         // GL_CW       glFrontFace( GL_CW ); 
};
struct RenderState
{
	// Raster State Control
	eCullMode		 m_cull_mode;			// CULL_BACK
	eFillMode		 m_fill_mode;			// FILL_SOLID
	eWindOrder		 m_front_face;			// WIND_COUNTER_CLOCKWISE

											// Depth State Control
	eCompare	     m_depth_compare;		// COMPARE_LESS
	bool			 m_depth_write;			// true

											// Blend
	eBlendOperation  m_color_blend_op;		// COMPARE_ADD
	eBlendFactor     m_color_src_factor;	// BLEND_ONE
	eBlendFactor     m_color_dst_factor;	// BLEND_ZERO

	eBlendOperation  m_alpha_blend_op;		// COMPARE_ADD
	eBlendFactor     m_alpha_src_factor;	// BLEND_ONE
	eBlendFactor     m_alpha_dst_factor;	// BLEND_ONE

};
struct FogFactor
{
	float nearPlane = 0;
	float farPlane = 0;
};

class BitmapFont;
class ShaderProgram;
class RenderBuffer;
class Sprites;
class TextureCube;
class Shader;
class Mesh;
class Vertex;
class Material;
class OrbitCamera;
class PerspectiveCamera;
class Camera;
class SpriteSheet;
class UniformBuffer;

extern HMODULE gGLLibrary;
extern HWND    gGLwnd;			   // window our context is attached to; 
extern HDC     gHDC;			   // our device context
extern HGLRC   gGLContext;         // our rendering context; 

class Renderer
{

public:
	FrameBuffer *m_effectFrameBuffer       = nullptr;
	FrameBuffer *m_copyFrameBuffer         = nullptr; 
	RenderBuffer *temp_render_buffer	   = nullptr;
	RenderBuffer *m_immediateBufferVBO     = nullptr;
	RenderBuffer *m_immediateBufferIBO     = nullptr;

	GLuint m_defaultVAO = NULL;

	Renderer();
	~Renderer();

	static Renderer *s_renderer;

	static std::string vertexShader;
	static std::string fragmentShader; 
	static UniformBuffer*		m_FogUBuffer;
	static std::string invalidVertexShader;
	static std::string invalidFragmentShader;

	void			BeginFrame();
	void			EndFrame();
	void			ClearScreen();

	// TEXTURE OPENGL FUNCTIONS
	void			BindTexture(GLuint textureId, Texture *texture);
	void			BindTexture(Texture *texture);
	void			SetColorBlending(eBlendOperation op, eBlendFactor src, eBlendFactor dst);
	void			SetAlphaBlending(eBlendOperation op, eBlendFactor src, eBlendFactor dst);
	void			SetCompleteBlending(eBlendOperation opColor, eBlendFactor srcColor, eBlendFactor dstColor, eBlendOperation opAlpha, eBlendFactor srcAlpha, eBlendFactor dstAlpha);
	void			SetCullMode(eCullMode cullMode);
	void			SetWindingOrder(eWindOrder windOrder);
	void			SetFillMode(eFillMode fillMode);
	
	void			EnableDepth(eCompare compare, bool should_write);
	void			DisableDepth();
	void			ClearDepth(float depth = 1.0f);

	// FUNCTIONS FOR CHANGING AXIS
	void			PushMatrix();
	void			PopMatrix();
	void			Scale(float scaleX,float scaleY,float scaleZ);
	void			ScaleUniform( float uniformScale );
	void			Translate( const Vector2& translation );
	void			Rotate(float degrees);

	Vector3			GetCurrentCameraRigthVector();
	Vector3			GetCurrentCameraForwardVector();
	Vector3			GetCurrentCameraUpVector();

	// FUNCTION TO DRAW LINES,LINE ARRAYS AND POINTS
	void			DrawLine(Vector3 *point1, Vector3 *point2);
	void			DrawLine(Vector3 point1, Vector3 point2, Rgba rgba);
	void			DrawLine(Vector3 point1, Vector3 point2);
	void			DrawFeatheredLine(Vector3 point1, Vector3 point2, Rgba startColor, Rgba finalColor);
	void			DrawLine(float x1, float y1, float x2, float y2);
	
	// FUNCTION TO DRAW CIRCLE
	void			DrawDottedCircle(float centreX,float centreY,float radius);
	void			DrawCircle(Disc2 disc, Rgba color);
	void			DrawCircle(float centreX,float centreY,float nradius);
	void			DrawCircle(Vector2 centre,float nradius);
	void			DrawCircle(float centreX,float centreY,float nradius,Rgba color,const float MAX_NUM_VERTEX);
	
	// FUNCTION TO DRAW RECTANGLE
	void			DrawRectangle(AABB2 aabb2);
	void			DrawRectangle(float centreX,float centreY,float legnth,float bredth,float angle);
	void			DrawRectangle(float x1,float y1,float x2,float y2);
	void			DrawSolidRectangle(float x1, float y1, float x2, float y2);

	void			DrawCube(Vector3 centre,float length,Rgba color);
	void			DrawCuboid(Vector3 centre,Vector3 dimensions,Rgba color);
	void			DrawTexturedCube(Vector3 centre,Vector3 size,const Texture *texture,AABB2 texCordsTop,AABB2 texCordsSide,AABB2 texCordsFront);

	// FUNCTION TO DRAW - AABB
	void			DrawTexturedAABB( const AABB2& bounds, const Texture *texture,const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint );
	void			DrawTexturedAABBOnXY(const AABB2& bounds,Vector3 cameraRigth,Vector3 cameraUp, float zValue, const Texture *texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint);
	void			DrawTexturedAABBOn3D( Vector3 position, Vector3 rigthDirection,Vector3 upDirection, Vector2 dimension, const Texture *texture,const Vector2& mins, const Vector2& maxs,const Rgba& color);

	void			DrawAABB( const AABB2& bounds, const Rgba& color );
	void			DrawAABBXY(Vector3 position,Vector2 dimensions, Vector3 rigthDirection, Vector3 upDirection, const Rgba& color);
	void			DrawAABBXZ( const AABB2& bounds, float Y,const Rgba& color );
	void			DrawAABBYZ( const AABB2& bounds, float X,const Rgba& color );
	void			DrawAABB( const AABB2& bounds, const Rgba& color1,const Rgba& color2 );

	// FUNCTIONS TO DRAW TEXT
	void			DrawTextOnPoint(char value[], int start, int length, Vector2 pos, float height);
	void			DrawTextOnPoint(char value[], int start, int length, Vector2 pos, float height, Rgba rgba);
	void			DrawTextOnPoint(char value[], int start, int length, Vector3 pos, float height, Rgba rgba);
	void			DrawTextOnPoint(std::string str, int start, int length, Vector2 pos, float height, Rgba rgba);
	void			DrawTextOnPoint(std::string str, Vector2 pos, float height, Rgba rgba);
	void			DrawTextOn3DPoint(Vector3 position, Vector3 rigthDirection, Vector3 upDirection, std::string stringValue, float heigth, Rgba color);
	void			DrawTextOnPointWithCR(std::vector<std::string>, Vector2 pos, float height, Rgba rgba);
	void			DrawTextOnPointWithCR(std::vector<std::string>, Vector2 pos, float height, Rgba rgba, int startIndex);
	void			DrawInt(int value, Vector2 pos,int height);

	// FUNCTIONS TO CREATE TEXT INSIDE A BOX
	void			DrawText2D( const Vector2& drawMins,const std::string& asciiText,float cellHeight,Rgba tint,float aspectScale = 1.f, const BitmapFont* font = nullptr );
	void			DrawTextInBox2D(AABB2 aabb2,std::string text,float aspectRation,float height,Vector2 alignment,TextDrawMode drawMode);

	void			SetCamera(Camera *camera);
	void			SetUniform(std::string name, Vector3 value);
	void			SetUniform(std::string name, Vector4 value);
	void			SetUniform(std::string name, int count,Vector3 *value);
	void			SetUniform(std::string name, int count,Vector4 *value);
	void			SetUniform(std::string name, float value);
	void			SetUniformBlockBinding(std::string name);
	void			BindUniformBuffer(size_t slotValue, UniformBuffer *buffer, size_t size, const void *data);

	bool			RenderStartup();
	static HGLRC	CreateOldRenderContext(HDC);
	void			BindNewWGLFunctions();
	void			BindGLFunctions();
	static HGLRC	CreateRealRenderContext( HDC hdc, int major, int minor );
	void			PostStartup();
	void			DrawMeshImmediate(DrawPrimitiveType drawPrimitive, unsigned int const vertex_count, Vertex_3DPCU const *buffer);
	void			DrawMeshImmediateWithIndices(DrawPrimitiveType drawPrimitive, unsigned int const vertex_count, Vertex_3DPCU const *buffer,int *indices,int icount);
	void			DrawMesh(Mesh *mesh,Matrix44 model);
	void			BindMeshToProgram(ShaderProgram *program, Mesh *mesh);
	void			BindRendererUniforms(Matrix44 model);
	void			BindRenderState(RenderState state);
	void			BindSampler(GLuint textureId,Sampler *sampler);
	void			BindShader(Shader *shader);
	void			BindMaterial(Material *material);
	void			BindMaterialProperty(Material *material);
	void			BindCubeMapTexture(TextureCube* cubeMap, int bindIndex);

	void			SetShader(Shader* tshader);
	void			UseSampler(Sampler *sampler);

	void			ApplyEffect(ShaderProgram *shaderProgram);
	void			FinishEffects();
	void			CopyTexture2D(Texture *from, Texture *to);

	Texture*		CreateRenderTarget(int width,int height,eTextureFormat fmt);
	Texture*		CreateDepthStencilTarget( int width, int height );
	bool			CopyFrameBuffer( FrameBuffer *dst, FrameBuffer *src );

	void			TakeScreenShotAndSave();
	//////////////////////////////////////////////////////////////////////////
	//Static functions
	static Renderer*		GetInstance();

	static GLenum			ToGLCompare(eCompare compare);
	static GLenum			ToGLBlendOperation(eBlendOperation blendOperation);
	static GLenum			ToGLFactor(eBlendFactor blendFactor);
	static GLenum			ToCullMode(eCullMode cullMode);
	static GLenum			ToWindingOrder(eWindOrder windOrder);
	static GLenum			ToFillMode(eFillMode fillMode);

	//////////////////////////////////////////////////////////////////////////
	static std::string     GetBlendOperationAsString(eBlendOperation operation);
	static eBlendOperation GetBlendOperationFromString(std::string blendOp);

	static std::string     GetBlendFactorAsString(eBlendFactor factor);
	static eBlendFactor    GetBlendFactorFromString(std::string blendFactor);

	static std::string     GetCullModeAsString(eCullMode cullMode);
	static eCullMode       GetCullModeFromString(std::string cullMode);

	static std::string     GetFillModeAsString(eFillMode fillMode);
	static eFillMode       GetFillModeFromString(std::string fillMode);

	static std::string     GetWindOrderAsString(eWindOrder windOrder);
	static eWindOrder      GetWindOrderFromString(std::string windOrder);

	static std::string     GetDepthCompareAsString(eCompare depthCompare);
	static eCompare        GetDepthCompareFromString(std::string deptCompare);

	static GLenum          ToGLCubeSide(eTextureCubeSide textureCubeSide);
	//////////////////////////////////////////////////////////////////////////
};