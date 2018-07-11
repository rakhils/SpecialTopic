#pragma once
#include <map>
#include <string>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
/*\class  : DebugDraw	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/27/2018 10:53:37 PM
* \contact: srsrakhil@gmail.com
*/
enum DebugElementType
{
	ELEMENT_UI,ELEMENT_TAG,ELEMENT,
	TEXT_UI   ,TEXT_TAG   ,TEXT,
	LOG,
};
enum DebugRenderMode
{
	DEBUG_RENDER_IGNORE_DEPTH,
	DEBUG_RENDER_USE_DEPTH,
	DEBUG_RENDER_HIDDEN,
	DEBUG_RENDER_XRAY
};

enum DebugRenderFillType
{
	DEBUG_RENDER_FILL,
	DEBUG_RENDER_WIRE,
	DEBUG_RENDER_FILL_WIRE
};
class Camera;
class Mesh;
class Texture;
class Renderer;
struct DebugRenderOptions
{
	DebugRenderOptions()
		: m_startColor(Rgba::WHITE)
		, m_endColor(Rgba::WHITE)
		, m_lifeTime(0.0f)
		, m_mode(DEBUG_RENDER_USE_DEPTH)
		, m_screenspace(false)
	{}

	Rgba  m_startColor;
	Rgba  m_endColor;
	float m_lifeTime;
	DebugRenderMode m_mode;

	bool m_screenspace;
};
struct DebugDrawElement
{
	Mesh*			    m_mesh					= nullptr;
	std::string         m_text;
	float				m_fontSize;
	Vector2			    m_alignment;
	float			    m_elapsedTime			= 0;
	Rgba			    m_color;
	Texture*			m_texture				= nullptr;
	Transform			m_transform;
	DebugRenderOptions  m_renderOptions;
	DebugRenderMode     m_renderMode;
	DebugRenderFillType m_renderFillType;
	DebugElementType    m_type;

	DebugDrawElement(){}
	
	DebugDrawElement(Mesh *mesh,float elapsedTime,DebugRenderOptions options)
	{
		m_mesh = mesh;
		m_elapsedTime = elapsedTime;
		m_renderOptions = options;
	}

	void SetTexture(Texture *texture)
	{
		m_texture = texture;
	}
};

class DebugDraw
{

public:
	//Member_Variables
	int								m_handle			= 0;
	Renderer*						m_renderer			= nullptr;
	Camera*							m_camera			= nullptr;
	float							m_logTextYValue		= 0;
	float							m_logTextFontSize	= 0;
	std::vector<DebugDrawElement>   m_drawElements;

	//Static_Member_Variables
	static DebugDraw*				s_defaultDraw;
	static bool						s_debugRender;
	static std::vector<DebugDraw *> s_debugDraws;

private:
	DebugDraw();
	~DebugDraw();
public:
	//Methods

	static void Update();
	static void Render();

	FillMode GetFillMode(DebugRenderFillType type);
	void	 DebugRenderStartup(Renderer *renderer);
	void	 DebugRenderShutdown();
	void	 DebugRenderUpdate();
	void	 DebugRenderClear();
	void	 SetDebugRender3DCamera(Camera *camera);
	void	 SetDepth(DebugRenderMode mode);
	void	 ResetDepth();

	//////////////////////////////////////////////////////////////////////////
	// Rendering of each types
	void	 DebugRenderElements();
	void	 DebugRenderElements(DebugDrawElement element,Matrix44 model);
	void	 DebugRenderLogElements(DebugDrawElement element);
	void	 DebugRenderTextElements(DebugDrawElement element);
	void	 DebugRenderText2DElements(DebugDrawElement element);
	void	 DebugRenderTextTagElements(DebugDrawElement element);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	//BASIC SHAPES
	void	 DebugRenderPoint(Vector3 position, DebugRenderOptions const &options);
	void	 DebugRenderPoint(Vector3 position, Rgba const &color = Rgba::WHITE);
	void	 DebugRenderPoint(Vector3 position, float lifetime, Rgba const &start_color, Rgba const &end_color);

	void	 DebugRenderLine(Vector3 p0, Vector3 p1, Rgba const &color = Rgba::WHITE, float time = 0.0f, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void	 DebugRenderCross(Vector3 p0, float length, Rgba color, DebugRenderOptions const &options);
	void	 DebugRenderLine2D(Vector3 p0, Vector3 p1, Rgba color, DebugRenderOptions const &options);
	void	 DebugRenderLineSegment(Vector3 p0, Rgba p0_color, Vector3 p1, Rgba p1_color, DebugRenderOptions const &options);
	void	 DebugRenderQuad2D(Vector3 origin, AABB2 bounds, float zAngle, Texture *texture, Rgba color, DebugRenderFillType fill_type, DebugRenderOptions const &options);
	void	 DebugRenderQuad(Vector3 origin,AABB2 bounds,Vector3 rVector,Vector3 upVector,Texture *texture,Rgba color,DebugRenderFillType fill_type,DebugRenderOptions const &options);
	void	 DebugRenderSphere(Vector3 position,float radius,int slices,int wedges,Texture *texture,Rgba color,DebugRenderFillType fill_type,DebugRenderOptions const &options);
	void	 DebugRenderCube(Vector3 position,Vector3 size,Texture *texture,Rgba color,DebugRenderFillType fill_type,DebugRenderOptions const &options);
	void	 DebugRenderPlane(Vector3 position, Vector2 dimensions, Vector3 rotation,Texture *texture,Rgba color,DebugRenderFillType fill_type,DebugRenderOptions const &options);
	void	 DebugRenderCone(Vector3 position,Vector3 direction,float length,Rgba color,DebugRenderFillType fill_type,DebugRenderOptions const &options);
	//////////////////////////////////////////////////////////////////////////

	void	 DebugRenderGlyph(Vector3 position, Vector2 dimentions,Texture *texture, Rgba color, DebugRenderOptions options);

	//////////////////////////////////////////////////////////////////////////
	// TEXT , TAG TEXT
	void	 DebugRenderText(Vector3 position, Vector3 rotation, std::string const &str, Vector2 alignment, float fontSize, DebugRenderOptions const &options);
	void	 DebugRenderText2D(Vector3 position, Vector3 rotation, std::string const &str, Vector2 alignment, float fontSize, Rgba color, DebugRenderOptions const &options);
	void	 DebugRenderTag(Vector3 position, std::string const &str,float fontsize,Rgba color,DebugRenderOptions const &options);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// WIRE FRAMES
	void	 DebugRenderWireSphere(Vector3 position, float radius, DebugRenderOptions const &options, Rgba const &color = Rgba::WHITE);
	void	 DebugRenderWireAABB3(Vector3 position, Vector3 size, DebugRenderOptions const &options, Rgba const &color = Rgba::WHITE);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Text rendering with different arguments
	void	 DebugRenderText(std::string, Vector3 position, Vector3 rotation, Vector2 alignment, float lifeTime,float size, Rgba color);
	void	 DebugRenderText(Vector3 position, Vector2 alignment, float size, Rgba color, char const *format, ...);
	void	 DebugRenderText(Vector3 position, Vector3 rotation, Vector2 alignment, float size, Rgba color, char const *format, ...);
	void	 DebugRenderText(Vector3 position, char const *format, ...);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Grid and Basis
	void	 DebugRenderBasis(Vector3 position, Vector3 rot, float time = 0.0f, float scale = 1.0f);
	void	 DebugRenderGrid(Vector3 position,Vector2 dimensions,Vector3 rightDirection,Vector3 upDirection,Rgba color,float time);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	// LOGF
	void	 DebuggerPrinf(char const *format,...);
	void	 DebugRenderLogf(std::string text, float lifetime, Rgba const &start_color, Rgba const &end_color);
	void	 DebugRenderLogf(float lifetime, Rgba const &start_color, Rgba const &end_color, char const *format, ...);
	void	 DebugRenderLogf(float lifetime, Rgba const &color, char const *format, ...);
	void	 DebugRenderLogf(Rgba const &color, char const *format, ...);
	void	 DebugRenderLogf(float lifetime, char const *format, ...);
	void	 DebugRenderLogf(char const *format, ...);
	void	 DebugRenderMatrix(Matrix44 value);
	//////////////////////////////////////////////////////////////////////////

	//Static_Methods

	static DebugDraw *  GetNewInstance();
	static DebugDraw *  GetInstance();
	static bool			ClearDebugDraws(int handle);
	static void			ClearDebugDraws(DebugDraw *debugDraws);
	static void			ClearAllDebugRenderDraws();
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