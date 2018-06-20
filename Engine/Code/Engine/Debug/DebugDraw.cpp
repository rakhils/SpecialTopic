#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtil.hpp"

bool					 DebugDraw::s_debugRender		= true;
DebugDraw*				 DebugDraw::s_defaultDraw		= nullptr;
std::vector<DebugDraw *> DebugDraw::s_debugDraws;

// CONSTRUCTOR
DebugDraw::DebugDraw()
{
	m_logTextFontSize = Windows::GetInstance()->GetDimensions().y / 100.f;
}

// DESTRUCTOR
DebugDraw::~DebugDraw()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : Updates all debug render objects
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::Update()
{
	for (size_t debugRenderIndex = 0; debugRenderIndex < s_debugDraws.size(); debugRenderIndex++)
	{
		s_debugDraws.at(debugRenderIndex)->DebugRenderUpdate();
	}
	if(s_defaultDraw != nullptr)
	{
		s_defaultDraw->DebugRenderUpdate();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : Renders all debug draw objects
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::Render()
{
	for (size_t debugRenderIndex = 0; debugRenderIndex < s_debugDraws.size(); debugRenderIndex++)
	{
		s_debugDraws.at(debugRenderIndex)->DebugRenderElements();
	}
	if (s_defaultDraw != nullptr)
	{
		s_defaultDraw->DebugRenderElements();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : Returns the fill mode equivalent of debug render modes
*
*@param   : debug render fill mode
*
*@return  : render fill mode
*/
//////////////////////////////////////////////////////////////
FillMode DebugDraw::GetFillMode(DebugRenderFillType type)
{
	switch (type)
	{
	case DEBUG_RENDER_FILL:
		return FILL_MODE_FILL;
		break;
	case DEBUG_RENDER_WIRE:
		return FILL_MODE_WIRE;
		break;
	case DEBUG_RENDER_FILL_WIRE:
		return FILL_MODE_FILL_WIRE;
		break;
	default:
		break;
	}
	return FILL_MODE_WIRE;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Debug start up
*
*@param   : Render object
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderStartup(Renderer *renderer)
{
	m_renderer = renderer;
	m_camera   = Camera::GetCurrentCamera();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Clears all render objects and delete the debug draw object
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderShutdown()
{
	DebugRenderClear();
	for(int index = 0;index < s_debugDraws.size();index++)
	{
		if(s_debugDraws.at(index) == this)
		{
			delete s_debugDraws.at(index);
			index--;
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Updates all the debug render draw elements
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderUpdate()
{
	float deltaTime = static_cast<float>(Clock::g_theMasterClock->GetMasterDeltaTime());
	for (size_t index = 0; index < m_drawElements.size(); index++)
	{
		DebugDrawElement debugDrawElement       = m_drawElements.at(index);
		m_drawElements.at(index).m_elapsedTime  += deltaTime;
		float elapsedTime						= m_drawElements.at(index).m_elapsedTime;
		float elapsedTimeFraction				= elapsedTime/m_drawElements.at(index).m_renderOptions.m_lifeTime;
		
		Rgba startColor							= debugDrawElement.m_renderOptions.m_startColor;
		Rgba endColor							= debugDrawElement.m_renderOptions.m_endColor;
		m_drawElements.at(index).m_color		= Interpolate(startColor, endColor,elapsedTimeFraction);
		// DELETING EXPIRED ONES
		if(debugDrawElement.m_elapsedTime		> debugDrawElement.m_renderOptions.m_lifeTime )
		{
			delete m_drawElements.at(index).m_mesh;
			m_drawElements.at(index).m_mesh = nullptr;
			m_drawElements.erase(m_drawElements.begin() + index, m_drawElements.begin() + index + 1);
			index--;
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Draws all elements/texts/logs
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderElements()
{
	if(!s_debugRender)
	{
		return;
	}
	Matrix44 identity;
	m_logTextYValue = Windows::GetInstance()->GetDimensions().y - 10*m_logTextFontSize;
	
	for (size_t index = 0; index < m_drawElements.size(); index++)
	{
		switch (m_drawElements.at(index).m_type)
		{
		case ELEMENT:
		{
			Camera::SetCurrentCamera(Camera::GetGamePlayCamera());
			SetDepth(m_drawElements.at(index).m_renderOptions.m_mode);
			DebugRenderElements(m_drawElements.at(index), identity);
			break;
		}
		case ELEMENT_TAG:
		{
			Camera::SetCurrentCamera(Camera::GetGamePlayCamera());
			SetDepth(m_drawElements.at(index).m_renderOptions.m_mode);
			Vector3 cameraRotation = Camera::GetGamePlayCamera()->m_transform.GetWorldRotation();
			Matrix44 tagMatrix = Matrix44::MatrixFromEuler(cameraRotation);
			DebugRenderElements(m_drawElements.at(index), tagMatrix);
			break;
		}
		case ELEMENT_UI:
		{
			Camera::SetCurrentCamera(Camera::GetUICamera());
			SetDepth(DEBUG_RENDER_IGNORE_DEPTH);
			DebugRenderElements(m_drawElements.at(index), identity);
			ResetDepth();
			break;
		}
		case TEXT:
		{
			Camera::SetCurrentCamera(Camera::GetGamePlayCamera());
			SetDepth(m_drawElements.at(index).m_renderOptions.m_mode);
			DebugRenderTextElements(m_drawElements.at(index));
			ResetDepth();
			break;
		}
		case TEXT_TAG:
		{
			Camera::SetCurrentCamera(Camera::GetGamePlayCamera());
			SetDepth(m_drawElements.at(index).m_renderOptions.m_mode);
			DebugRenderTextTagElements(m_drawElements.at(index));
			break;
		}
		case TEXT_UI:
		{
			Camera::SetCurrentCamera(Camera::GetUICamera());
			SetDepth(DEBUG_RENDER_IGNORE_DEPTH);
			DebugRenderText2DElements(m_drawElements.at(index));
			break;
		}
		case LOG:
		{
			Camera::SetCurrentCamera(Camera::GetUICamera());
			DebugRenderLogElements(m_drawElements.at(index));
			break;
		}
		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Draws all elements
*@param   : element , model matrix
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderElements(DebugDrawElement element,Matrix44 model)
{
	Material *material  = Material::AquireResource("Data\\Materials\\debug.mat");
	Texture::SetCurrentTexture(element.m_texture);
	m_renderer->BindMaterial(material);

	m_renderer->SetUniform("MODELCOLOR", element.m_color.GetAsFloats());
	m_renderer->DrawMesh(element.m_mesh, model);
	ResetDepth();

	if (element.m_renderOptions.m_mode == DEBUG_RENDER_XRAY)
	{
		SetDepth(DEBUG_RENDER_IGNORE_DEPTH);
		Rgba color = element.m_color;
		color.a = static_cast<char>(100);
		m_renderer->SetUniform("MODELCOLOR", color.GetAsFloats());
		m_renderer->DrawMesh(element.m_mesh, model);
	}
	delete material;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Log element
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLogElements(DebugDrawElement element)
{
	Material *material			 = Material::AquireResource("Data\\Materials\\text.mat");
	Texture::SetCurrentTexture(Texture::GetDefaultTexture());
	GL_CHECK_ERROR();
	m_renderer->BindMaterial(material);
	Vector3 rigthDirection(1,  0, 0);
	Vector3 upDirection   (0,  1, 0);
	Vector3 position      (-1 + element.m_fontSize * 2, m_logTextYValue, 0);
	GL_CHECK_ERROR();
	SetDepth(DEBUG_RENDER_IGNORE_DEPTH);
	m_renderer->SetUniform("MODELCOLOR", element.m_renderOptions.m_startColor.GetAsFloats());
	m_renderer->DrawTextOn3DPoint(position, rigthDirection, upDirection, element.m_text, element.m_fontSize, element.m_renderOptions.m_startColor);
	GL_CHECK_ERROR();
	ResetDepth();

	m_logTextYValue -= m_logTextFontSize * 2;
	delete material;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : tex2D element
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderText2DElements(DebugDrawElement element)
{
	Material *material		     = Material::AquireResource("Data\\Materials\\text.mat");
	Texture::SetCurrentTexture(Texture::GetDefaultTexture());
	m_renderer->BindMaterial(material);
	SetDepth(element.m_renderOptions.m_mode);
	Transform textTransform;
	textTransform.RotateLocalByEuler(element.m_transform.GetWorldRotation());

	Vector3 rigthDirection = textTransform.GetLocalMatrix().GetIAxis();
	Vector3 upDirection    = textTransform.GetLocalMatrix().GetJAxis();
	m_renderer->DrawTextOn3DPoint(element.m_transform.GetWorldPosition(), rigthDirection, upDirection, element.m_text, element.m_fontSize, element.m_color);
	delete material;
	ResetDepth();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Text element
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderTextElements(DebugDrawElement element)
{
	Material *material		= Material::AquireResource("Data\\Materials\\text.mat");
	m_renderer->BindMaterial(material);
	Matrix44 rotationMatrix = Matrix44::MatrixFromEuler(element.m_transform.GetLocalEulerAngles());
	Vector3 rigthDirection  = rotationMatrix.GetIAxis();
	Vector3 upDirection     = rotationMatrix.GetJAxis();
	SetDepth(element.m_renderOptions.m_mode);
	m_renderer->DrawTextOn3DPoint(element.m_transform.GetWorldPosition(), rigthDirection, upDirection, element.m_text, element.m_fontSize, element.m_color);
	ResetDepth();
	if (element.m_renderOptions.m_mode == DEBUG_RENDER_XRAY)
	{
		SetDepth(DEBUG_RENDER_IGNORE_DEPTH);
		Rgba color = element.m_color;
		color.a = static_cast<char>(100);
		m_renderer->SetUniform("MODELCOLOR", color.GetAsFloats());
		m_renderer->DrawTextOn3DPoint(element.m_transform.GetWorldPosition(), rigthDirection, upDirection, element.m_text, element.m_fontSize, color);
	}
	delete material;
	ResetDepth();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderTextTagElements(DebugDrawElement element)
{
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	m_renderer->BindMaterial(material);
	Camera::SetCurrentCamera(m_camera);
	Vector3 rigthDirection = m_camera->m_model.GetIAxis();
	Vector3 upDirection    = m_camera->m_model.GetJAxis();
	rigthDirection         = rigthDirection.GetNormalized();
	upDirection			   = upDirection.GetNormalized();
	SetDepth(element.m_renderOptions.m_mode);
	Vector3 position = element.m_transform.GetLocalPosition() - rigthDirection * (element.m_text.size()*element.m_fontSize);
	m_renderer->DrawTextOn3DPoint(position, rigthDirection, upDirection, element.m_text, element.m_fontSize, element.m_color);
	ResetDepth();
	if (element.m_renderOptions.m_mode == DEBUG_RENDER_XRAY)
	{
		SetDepth(DEBUG_RENDER_IGNORE_DEPTH);
		element.m_color.a = static_cast<char>(100);
		m_renderer->SetUniform("MODELCOLOR", element.m_color.GetAsFloats());
		position = position - rigthDirection * (element.m_text.size()*element.m_fontSize);
		m_renderer->DrawTextOn3DPoint(position, rigthDirection, upDirection, element.m_text, element.m_fontSize, element.m_color);
	}
	delete material;
	ResetDepth();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Clears all draws
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderClear()
{
	m_drawElements.clear();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Sets the camera for debug draw
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::SetDebugRender3DCamera(Camera *camera)
{
	m_camera = camera;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/01
*@purpose : Sets depth to the shader
*
*@param   : Rendermode
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::SetDepth(DebugRenderMode mode)
{
	Shader *debugShader = Shader::GetCurrentShader();
	switch (mode)
	{
	case DEBUG_RENDER_IGNORE_DEPTH:
		debugShader->DisableDepth();
		break;
	case DEBUG_RENDER_USE_DEPTH:
		debugShader->SetDepth(COMPARE_LESS, true);
		break;
	case DEBUG_RENDER_HIDDEN:
		debugShader->SetDepth(COMPARE_ALWAYS, false);
		break;
	case DEBUG_RENDER_XRAY:
		debugShader->SetDepth(COMPARE_LESS, true);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/01
*@purpose : Resets depth on render
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::ResetDepth()
{
	m_renderer->EnableDepth(COMPARE_GREATER, false);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Draws debug render points
*
*@param   : Position,color
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderPoint(Vector3 position, Rgba const &color /*= Rgba::WHITE*/)
{
	Mesh *point = MeshBuilder::Create3DPoint<Vertex_3DPCU>(position, color);
	DebugRenderOptions options;
	DebugDrawElement debugDrawElement(point, 0.0f, options);
	debugDrawElement.SetTexture(Texture::GetDefaultTexture());
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : draws a point with given render options
*
*@param   : point's position and render option
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderPoint(Vector3 position, DebugRenderOptions const &options)
{
	Mesh *mesh = MeshBuilder::Create3DPoint<Vertex_3DPCU>(position, options.m_startColor);
	DebugDrawElement debugDrawElement(mesh, 0.0f, options);
	DebugRenderBasis(position, Vector3(0, 0, 0), options.m_lifeTime);
	debugDrawElement.SetTexture(Texture::GetDefaultTexture());
	debugDrawElement.m_type = ELEMENT;
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Draws a point in world with position start color end color
*
*@param   : position,life,start and end color
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderPoint(Vector3 position, float lifetime, Rgba const &start_color, Rgba const &end_color)
{
	DebugRenderOptions options;
	options.m_startColor = start_color;
	options.m_endColor = end_color;
	options.m_lifeTime = lifetime;
	Mesh *point = MeshBuilder::Create3DPoint<Vertex_3DPCU>(position, options.m_startColor);
	DebugDrawElement debugDrawElement(point, 0.0f, options);
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(Texture::GetDefaultTexture());
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Renders a line with positions given
*
*@param   : Start and end position,color,lifetime,depth mode
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLine(Vector3 p0, Vector3 p1, Rgba const &color /*= Rgba::WHITE*/, float time /*= 0.0f*/, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH*/)
{
	Mesh *line = MeshBuilder::Create3DLine<Vertex_3DPCU>(p0, p1, color, color);
	DebugRenderOptions options;
	options.m_mode = mode;
	options.m_lifeTime = time;
	options.m_startColor = color;
	options.m_endColor = color;
	DebugDrawElement debugDrawElement(line, 0, options);
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(Texture::GetDefaultTexture());
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : Renders cross  like this ->  -|-
*
*@param   : position,length of each line,render options
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderCross(Vector3 p0, float length, Rgba color, DebugRenderOptions const &options)
{
	Vector3 leftPos   = p0 - Vector3(1, 0, 0)*length;
	Vector3 rightPos  = p0 + Vector3(1, 0, 0)*length;
	Vector3 topPos    = p0 + Vector3(0, 1, 0)*length;
	Vector3 bottomPos = p0 - Vector3(0, 1, 0)*length;

	DebugRenderLine(leftPos, rightPos, color, options.m_lifeTime,options.m_mode);
	DebugRenderLine(topPos, bottomPos, color, options.m_lifeTime,options.m_mode);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : Draws line in UI Space
*
*@param   : Start and End position ,color, render options
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLine2D(Vector3 p0, Vector3 p1, Rgba color, DebugRenderOptions const &options)
{
	Mesh *line = MeshBuilder::Create3DLine<Vertex_3DPCU>(p0, p1, color, color);
	DebugDrawElement debugDrawElement(line, 0, options);
	debugDrawElement.m_type = ELEMENT_UI;
	debugDrawElement.SetTexture(Texture::GetDefaultTexture());
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Draws debug lines
*
*@param   : position 1 color one position 2 color 2 renderoptions;
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLineSegment(Vector3 p0, Rgba p0_color, Vector3 p1, Rgba p1_color, DebugRenderOptions const &options)
{
	Mesh *line = MeshBuilder::Create3DLine<Vertex_3DPCU>(p0, p1, p0_color, p1_color);
	DebugDrawElement debugDrawElement(line, 0.0f, options);
	m_drawElements.push_back(debugDrawElement);
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(Texture::GetDefaultTexture());
	//Matrix44 model;
	//m_renderer->DrawMesh(line,model);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : Draws quad in 2d space
*
*@param   : position , bounds, angle texture,color fill type and options regarding color change lifetime
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderQuad2D(Vector3 origin, AABB2 bounds, float zAngle, Texture *texture, Rgba color, DebugRenderFillType fill_type, DebugRenderOptions const &options)
{
	Vector3 rightVector( CosDegrees(zAngle), SinDegrees(zAngle), 0);
	Vector3 upVector   (-SinDegrees(zAngle), CosDegrees(zAngle), 0);
	Vector2 dimensions = bounds.GetDimensions();
	Mesh *quad = MeshBuilder::CreateQuad<Vertex_3DPCUN>(origin, rightVector, upVector, dimensions, color, GetFillMode(fill_type));
	DebugDrawElement debugDrawElement(quad, 0.0f, options);
	if (texture == nullptr)
	{
		texture = Texture::GetDefaultTexture();
	}
	debugDrawElement.m_type = ELEMENT_UI;
	debugDrawElement.SetTexture(texture);
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Draw quad in world space wit given right and up vector
*@param   : Position,bounds(dimension) rigth,up, texture color and file type
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderQuad(Vector3 origin, AABB2 bounds, Vector3 rightVector, Vector3 upVector,Texture *texture,Rgba color,DebugRenderFillType fill_type, DebugRenderOptions const &options)
{
	Vector2 dimensions = bounds.GetDimensions();
	Mesh *quad = MeshBuilder::CreateQuad<Vertex_3DPCUN>(origin, rightVector, upVector, dimensions, color,GetFillMode(fill_type));
	DebugDrawElement debugDrawElement(quad, 0.0f, options);
	if (texture == nullptr)
	{
		texture = Texture::GetDefaultTexture();
	}
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(texture);
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Draws a spheres with given full mode and render options
*
*@param   : Position , radius, slices(horizontal) wedges (vertical) cross section count,texture,color,fill type,render options
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderSphere(Vector3 position,float radius,int slices,int wedges,Texture *texture,Rgba color,DebugRenderFillType fill_type,DebugRenderOptions const &options)
{
	Mesh *sphere = MeshBuilder::CreateUVSpehere<Vertex_3DPCUN>(position, radius, wedges, slices, color, GetFillMode(fill_type));
	DebugDrawElement debugDrawElement(sphere, 0.0f, options);
	debugDrawElement.m_renderFillType = fill_type;
	if (texture == nullptr)
	{
		texture = Texture::GetDefaultTexture();
	}
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(texture);
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/31
*@purpose : Draws cube 
*@param   : Position,size,texture color,file type,render options
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderCube(Vector3 position, Vector3 size, Texture *texture, Rgba color, DebugRenderFillType fill_type, DebugRenderOptions const &options)
{
	Mesh *cube = MeshBuilder::CreateCube<Vertex_3DPCUN>(position, size, color,GetFillMode(fill_type));
	DebugDrawElement debugDrawElement(cube, 0.0f, options);
	debugDrawElement.m_renderFillType = fill_type;
	if (texture == nullptr)
	{
		texture = Texture::GetDefaultTexture();
	}
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(texture);
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Renders plane in 3d space
*@param   : position,dimension, rotation euler angles texture ,color,fill type,render options.
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderPlane(Vector3 position, Vector2 dimensions, Vector3 rotation,Texture *texture,Rgba color,DebugRenderFillType fill_type, DebugRenderOptions const &options)
{
	Transform transform;
	transform.SetLocalRotationEuler(rotation);
	Matrix44 modelMatrix = transform.GetLocalMatrix();

	Vector3 rigthVector = modelMatrix.GetIAxis();
	Vector3 upVector    = modelMatrix.GetJAxis();

	Mesh *plane = MeshBuilder::Create3DPlane<Vertex_3DPCUN>(position, rigthVector, upVector, dimensions, color,GetFillMode(fill_type));
	DebugDrawElement debugDrawElement(plane, 0.0f, options);
	if (texture == nullptr)
	{
		texture = Texture::GetDefaultTexture();
	}
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(texture);
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/20
*@purpose : Renders a cone in space
*@param   : position,forward directions,height of cone,color,filltype,renderoptions
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderCone(Vector3 position, Vector3 direction, float length, Rgba color, DebugRenderFillType fill_type, DebugRenderOptions const &options)
{
	UNUSED(color);
	Matrix44 rotationMatrix    = Matrix44::MakeRotationMatrixFromDirection(direction);
	Matrix44 translationMatrix = Matrix44::MakeTranslation3D(position);
	Matrix44 scaleMatrix	   = Matrix44::MakeScale3D(length);

	Matrix44 modelMatrix;
	modelMatrix.SetIdentity();

	modelMatrix.MultiplyAndSet(translationMatrix);
	modelMatrix.MultiplyAndSet(rotationMatrix);
	modelMatrix.MultiplyAndSet(scaleMatrix);


	Matrix44 coneVertexRotationMatrix;
	coneVertexRotationMatrix = coneVertexRotationMatrix.MakeRotationDegreesInY(45.f);

	Vector3 coneDirection(1, 0, 0);

	Vector3 first  = coneVertexRotationMatrix.Multiply(coneDirection);
	coneVertexRotationMatrix.SetIdentity();
	coneVertexRotationMatrix = coneVertexRotationMatrix.MakeRotationDegreesInY(-45);
	Vector3 second = coneVertexRotationMatrix.Multiply(coneDirection);
	coneVertexRotationMatrix.SetIdentity();
	coneVertexRotationMatrix = coneVertexRotationMatrix.MakeRotationDegreesInZ(45);

	Vector3 third  = coneVertexRotationMatrix.Multiply(coneDirection);
	coneVertexRotationMatrix.SetIdentity();
	coneVertexRotationMatrix = coneVertexRotationMatrix.MakeRotationDegreesInZ(-45);
	Vector3 fourth = coneVertexRotationMatrix.Multiply(coneDirection);


	first = modelMatrix.Multiply(first);
	second = modelMatrix.Multiply(second);
	third = modelMatrix.Multiply(third);
	fourth = modelMatrix.Multiply(fourth);

	Vector3 startPosition[4] = { position,position,position,position };
	Vector3 endPosition[4]   = { position + first,position + second,position + third,position + fourth };

	Mesh *mesh = MeshBuilder::CreateLineSet<Vertex_3DPCU>(startPosition, endPosition, 4, Rgba::WHITE);


	DebugDrawElement debugDrawElement(mesh, 0.0f, options);
	debugDrawElement.m_renderFillType = fill_type;
	debugDrawElement.m_type = ELEMENT;
	debugDrawElement.SetTexture(Texture::GetDefaultTexture());
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/01
*@purpose : Draws plane which always views camera
*@param   : position , dimensions,texture color,render options
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderGlyph(Vector3 position, Vector2 dimensions, Texture *texture, Rgba color, DebugRenderOptions options)
{
	Vector3 rigthVector = m_camera->GetCameraRightVector();
	Vector3 upVector = m_camera->GetCameraUpVector();

	Mesh *plane = MeshBuilder::Create3DPlane<Vertex_3DPCUN>(position, rigthVector, upVector, dimensions, color, FILL_MODE_FILL);
	DebugDrawElement debugDrawElement(plane, 0.0f, options);
	if (texture == nullptr)
	{
		texture = Texture::GetDefaultTexture();
	}
	debugDrawElement.m_type = ELEMENT_TAG;
	debugDrawElement.SetTexture(texture);
	m_drawElements.push_back(debugDrawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Renders a wireframe sphere (not solid)
*@param   : position,radius,render options,color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderWireSphere(Vector3 position, float radius,DebugRenderOptions const &options, Rgba const &color /*= Rgba::WHITE*/)
{
	DebugRenderSphere(position, radius, 25, 25,nullptr,color, DEBUG_RENDER_WIRE, options);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/31
*@purpose : Draws wire frame cube
*@param   : position,size in xyz, render options ,color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderWireAABB3(Vector3 position, Vector3 size, DebugRenderOptions const &options, Rgba const &color /*= Rgba::WHITE*/)
{
	DebugRenderCube(position, size, nullptr, color,DEBUG_RENDER_WIRE, options);
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Debug render draws text in 3d position
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderText(std::string text, Vector3 position, Vector3 rotation, Vector2 alignment, float lifeTime, float size, Rgba color)
{
	DebugRenderOptions options;
	options.m_lifeTime   = lifeTime;
	options.m_startColor = color;
	options.m_endColor   = color;
	DebugRenderText(position, rotation, text, alignment, size, options);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/01
*@purpose : Draws text
*@param   : position,rotation,test,alignment of text within,size,render options
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderText(Vector3 position, Vector3 rotation, std::string const &text, Vector2 alignment, float fontSize, DebugRenderOptions const &options)
{
	DebugDrawElement drawTextElement;
	drawTextElement.m_renderOptions   = options;
	drawTextElement.m_text			  = text;
	drawTextElement.m_alignment		  = alignment;
	drawTextElement.m_fontSize		  = fontSize;
	drawTextElement.m_type			  = TEXT;
	drawTextElement.m_transform.SetLocalRotationEuler(rotation);
	drawTextElement.m_transform.SetLocalPosition(position);

	m_drawElements.push_back(drawTextElement);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Draws text
*@param   : position alignment,size,color,format
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderText(Vector3 position, Vector2 alignment, float size, Rgba color, char const *format, ...)
{
	DebugRenderOptions options;
	options.m_lifeTime = 1;
	options.m_startColor = color;
	options.m_endColor   = color;
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);
	DebugRenderText(position, Vector3(0, 0, 0), std::string(buffer), alignment, size, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Draws text
*@param   : position alignment,size,color,format
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderText(Vector3 position, Vector3 rotation, Vector2 alignment, float size, Rgba color, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);
	DebugRenderOptions options;
	options.m_lifeTime   = 1;
	options.m_startColor = color;
	options.m_endColor   = color;
	DebugRenderText(position, rotation, std::string(buffer), alignment, size, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Draws text
*@param   : Position,format strings
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderText(Vector3 position, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);
	DebugRenderOptions options;
	options.m_lifeTime   = 1;
	options.m_startColor = Rgba::WHITE;
	options.m_endColor   = Rgba::WHITE;
	DebugRenderText(position, Vector3::ZERO, std::string(buffer), Vector2::ONE, 1, options);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/02
*@purpose : Renders text in 2D (UI SPACE)
*@param   : position rotation,text,alignment,fontsize,color,render options
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderText2D(Vector3 position,Vector3 rotation, std::string const &text, Vector2 alignment, float fontSize,Rgba color, DebugRenderOptions const &options)
{
	DebugDrawElement drawTextElement;
	drawTextElement.m_text						 = text;
	drawTextElement.m_color						 = color;
	drawTextElement.m_renderOptions.m_startColor = options.m_startColor;
	drawTextElement.m_renderOptions.m_endColor   = options.m_endColor;
	drawTextElement.m_renderOptions.m_lifeTime   = options.m_lifeTime;
	drawTextElement.m_fontSize					 = fontSize;
	drawTextElement.m_transform.SetLocalPosition(position);
	drawTextElement.m_transform.SetLocalRotationEuler(rotation);
	drawTextElement.m_type = TEXT_UI;
	m_drawElements.push_back(drawTextElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/01
*@purpose : Draws text always oriented towards camera
*@param   : Position,text,size,color,render options
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderTag(Vector3 position, std::string const &text, float fontsize, Rgba color, DebugRenderOptions const &options)
{
	DebugDrawElement drawTextTag;
	drawTextTag.m_transform.SetLocalPosition(position);
	drawTextTag.m_renderOptions.m_startColor = options.m_startColor;
	drawTextTag.m_renderOptions.m_endColor = options.m_endColor;
	drawTextTag.m_renderOptions.m_lifeTime = options.m_lifeTime;
	drawTextTag.m_text = text;
	drawTextTag.m_type = TEXT_TAG;
	drawTextTag.m_color = color;
	drawTextTag.m_fontSize = fontsize;
	m_drawElements.push_back(drawTextTag);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Renders grid in position with given rotation for time 
*@param   : Position,rotation,time,scale
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderBasis(Vector3 position, Vector3 rotation, float time /*= 0.0f*/, float scale /*= 1.0f*/)
{
	DebugRenderOptions options;
	options.m_lifeTime = time;
	options.m_mode	   = DEBUG_RENDER_USE_DEPTH;
	Mesh *mesh		   = MeshBuilder::CreateBasis<Vertex_3DPCUN>(position, rotation, scale);
	DebugDrawElement drawElement(mesh, 0.f, options);
	drawElement.m_renderOptions.m_lifeTime = time;
	drawElement.SetTexture(Texture::GetDefaultTexture());
	drawElement.m_type = ELEMENT;
	m_drawElements.push_back(drawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Renders 2d Grid in 3D space
*@param   : position , dimensions (x,y) rigth,up,color,lifetime
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderGrid(Vector3 position, Vector2 dimensions,Vector3 rightDirection,Vector3 upDirection, Rgba color, float time)
{
	DebugRenderOptions options;
	options.m_lifeTime = time;
	options.m_mode     = DEBUG_RENDER_USE_DEPTH;
	Mesh *mesh         = MeshBuilder::Create2DGrid<Vertex_3DPCUN>(position, dimensions, rightDirection, upDirection, color);
	DebugDrawElement drawElement(mesh, 0.f, options);
	drawElement.m_renderOptions.m_lifeTime = time;
	drawElement.SetTexture(Texture::GetDefaultTexture());
	drawElement.m_type = ELEMENT;
	m_drawElements.push_back(drawElement);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Renders logs
*@param   : text,lifetime,start and end color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLogf(std::string text,float lifetime, Rgba const &start_color, Rgba const &end_color)
{
	DebugDrawElement drawLogElement;
	drawLogElement.m_renderOptions.m_startColor = start_color;
	drawLogElement.m_renderOptions.m_endColor = end_color;
	drawLogElement.m_renderOptions.m_lifeTime = lifetime;
	drawLogElement.m_elapsedTime = 0;
	drawLogElement.m_text = text;
	drawLogElement.m_fontSize = m_logTextFontSize;
	drawLogElement.m_type = LOG;
	m_drawElements.push_back(drawLogElement);
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Draw debug logs at top left corner of 
*@param   : lifetime start end color,formated string
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLogf(float lifetime, Rgba const &start_color, Rgba const &end_color, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	DebugRenderLogf(std::string(buffer),lifetime, start_color, end_color);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : renders logs in top left of screen
*@param   : lifetime,color,formated string
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLogf(float lifetime, Rgba const &color, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	DebugRenderLogf(std::string(buffer),lifetime, color, color);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : renders logs in top left part of screen
*@param   : color and formated string
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLogf(Rgba const &color, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	DebugRenderLogf(std::string(buffer),0.0f, color, color);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : Renders log in top left part of screen
*@param   : lifetime, formated strings
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLogf(float lifetime, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	DebugRenderLogf(std::string(buffer),lifetime, Rgba::WHITE, Rgba::WHITE);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/28
*@purpose : renders logs in top left part of screen
*@param   : formated string
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderLogf(char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	DebugRenderLogf(std::string(buffer),0.01f, Rgba::WHITE, Rgba::WHITE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : Displays matrix values as logs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugDraw::DebugRenderMatrix(Matrix44 value)
{
	DebugRenderLogf("MATRIX ", "");
	DebugRenderLogf("%f , %f , %f , %f", value.GetIAxis4());
	DebugRenderLogf("%f , %f , %f , %f", value.GetJAxis4());
	DebugRenderLogf("%f , %f , %f , %f", value.GetKAxis4());
	DebugRenderLogf("%f , %f , %f , %f", value.GetTAxis4());

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Creates new instance and push to debug draw array
*@param   : NIL
*@return  : new DebugDraw pointer
*//////////////////////////////////////////////////////////////
DebugDraw * DebugDraw::GetNewInstance()
{
	DebugDraw * debugDraw = new DebugDraw();
	s_debugDraws.push_back(debugDraw);
	debugDraw->m_handle =  static_cast<int>(s_debugDraws.size()) - 1;
	return debugDraw;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/19
*@purpose : Gets static instance
*@param   : NIL
*@return  : Debug draw pointer
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DebugDraw * DebugDraw::GetInstance()
{
	if(s_defaultDraw == nullptr)
	{
		s_defaultDraw = new DebugDraw();
	}
	s_defaultDraw->SetDebugRender3DCamera(Camera::GetCurrentCamera());
	return s_defaultDraw;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Clears all items in debug draw object
*@param   : Debug draw object
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::ClearDebugDraws(DebugDraw *debugDraws)
{
	for(size_t index = 0;index < s_debugDraws.size();index++)
	{
		if(s_debugDraws.at(index) == debugDraws)
		{
			delete s_debugDraws.at(index);
			s_debugDraws.erase(s_debugDraws.begin() + index, s_debugDraws.begin() + index + 1);
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/01
*@purpose : Clears all debug draw by handle
*@param   : Handle values
*@return  : True if could find object with handle
*//////////////////////////////////////////////////////////////
bool DebugDraw::ClearDebugDraws(int handle)
{
	if(handle < s_debugDraws.size())
	{
		s_debugDraws.erase(s_debugDraws.begin() + handle, s_debugDraws.begin() + handle + 1);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Clears all debug draws
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void DebugDraw::ClearAllDebugRenderDraws()
{
	for (size_t index = 0; index < s_debugDraws.size(); index++)
	{
		s_debugDraws.at(index)->m_drawElements.clear();
	}
}

