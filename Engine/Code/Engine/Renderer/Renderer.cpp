#include "Engine\Renderer\Renderer.hpp"

#include <stdio.h>
#include <string>
#include "Engine\ThirdParty\stb\stb_image.h"
#include "Engine\ThirdParty\stb\stb_image_write.h"

#include "Engine\Math\MathUtil.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Renderer\BitmapFont.hpp"
#include "Engine\Renderer\GL\glfunctions.hpp"
#include "Engine\Renderer\Sprites.hpp"
#include "Engine\Renderer\ShaderProgram.hpp"
#include "Engine\Renderer\RenderBuffer.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\Image.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Core\Windows.hpp"
#include "Engine\Mesh\Mesh.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/ShaderDefinitions.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Materials/MaterialProperty.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Camera/OrbitCamera.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
//#include "Engine\Renderer\Layouts\VertexLayout.hpp"
//#pragma comment( lib, "opengl32" )
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HMODULE gGLLibrary  = NULL;
HWND gGLwnd			= NULL;    // window our context is attached to; 
HDC gHDC			= NULL;    // our device context
HGLRC gGLContext	= NULL;    // our rendering context; 

Renderer* Renderer::s_renderer = nullptr;
UniformBuffer*		Renderer::m_FogUBuffer = nullptr;

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::BeginFrame()
{
	ClearScreen();
	ClearDepth(1.0f);
	EnableDepth(COMPARE_LESS, true);
}

void Renderer::EndFrame()
{
	HWND hWnd = GetActiveWindow();
	HDC hDC   = GetDC( hWnd );
	CopyFrameBuffer(nullptr,Camera::GetCurrentCamera()->m_defaultFrameBuffer);
	SwapBuffers( hDC );
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Bind texture 
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindTexture(GLuint textureSlot, Texture *texture)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, texture->m_textureID);
}

void Renderer::BindTexture(Texture *texture)
{
	BindTexture(0U, texture);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Sets alpha , colour blending
*@param   : Blendoperation color,source and destination color,blend op for alpha and src and destination alpha
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetCompleteBlending(eBlendOperation opColor, eBlendFactor srcColor, eBlendFactor dstColor, eBlendOperation opAlpha, eBlendFactor srcAlpha, eBlendFactor dstAlpha)
{
	glBlendEquationSeparate(ToGLBlendOperation(opColor), ToGLBlendOperation(opAlpha));

	glBlendFuncSeparate(ToGLFactor(srcColor), ToGLFactor(dstColor), ToGLFactor(srcAlpha), ToGLFactor(dstAlpha));

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Sets the cull mode in renderer
*@param   : cull mode enum
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetCullMode(eCullMode cullMode)
{
	switch (cullMode)
	{
	case CULLMODE_BACK:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		break;
	case CULLMODE_FRONT:
		glEnable(GL_CULL_FACE); 
		glCullFace(GL_FRONT);
		break;
	case CULLMODE_NONE:
		glDisable(GL_CULL_FACE);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Set renders winding order
*@param   : wind order enum
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetWindingOrder(eWindOrder windOrder)
{
	switch (windOrder)
	{
	case WIND_CLOCKWISE:
		glFrontFace(GL_CW);
		break;
	case WIND_COUNTER_CLOCKWISE:
		glFrontFace(GL_CCW);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Sets renders fill mode
*@param   : File mode
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetFillMode(eFillMode fillMode)
{
	switch (fillMode)
	{
	case FILLMODE_SOLID:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case FILLMODE_WIRE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	default:
		break;
	}
}

void Renderer::ClearScreen()
{
	glClear( GL_COLOR_BUFFER_BIT );
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/27
*@purpose : Returns instance of renderer
*@param   : NIL
*@return  : Renderer pointer
*//////////////////////////////////////////////////////////////
Renderer* Renderer::GetInstance()
{
	if(s_renderer == nullptr)
	{
		s_renderer = new Renderer();
	}
	return s_renderer;
}

void Renderer::PushMatrix()
{
	//glPushMatrix();
}

void Renderer::PopMatrix()
{
	//glPopMatrix();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2017/12/20
*@purpose : Scale non uniform
*@param   : ScaleX component ,ScaleY component, ScaleZ component
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::Scale(float scaleX, float scaleY,float scaleZ)
{
	UNUSED(scaleX);
	UNUSED(scaleY);
	UNUSED(scaleZ);
	//glScalexOES(scaleX, scaleY, scaleZ);
	//glScalef(scaleX,scaleY,0);
}

void Renderer::ScaleUniform( float uniformScale )
{
	UNUSED(uniformScale);
	//glScalef(uniformScale,uniformScale,0);
}

void Renderer::Translate( const Vector2& translation )
{
	UNUSED(translation);
	//glTranslatef(translation.x,translation.y,0);
}

void Renderer::Rotate( float degrees )
{
	UNUSED(degrees);
	//glRotatef(degrees,0,0,1);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/17
*@purpose : Get right vector from current camera
*@param   : NIL
*@return  : rigth vector
*//////////////////////////////////////////////////////////////
Vector3 Renderer::GetCurrentCameraRigthVector()
{
	return Camera::GetCurrentCamera()->GetCameraRightVector();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/17
*@purpose : Returns camera vector
*@param   : NIL
*@return  : forward vector
*//////////////////////////////////////////////////////////////
Vector3 Renderer::GetCurrentCameraForwardVector()
{
	return Camera::GetCurrentCamera()->GetCameraForwardVector();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : current camera up vector
*@param   : NIL
*@return  : camera Up vector
*//////////////////////////////////////////////////////////////
Vector3 Renderer::GetCurrentCameraUpVector()
{
	return Camera::GetCurrentCamera()->GetCameraUpVector();
}

void Renderer::DrawLine(Vector3 *point1,Vector3 *point2)
{
	DrawLine(*point1, *point2, Rgba::WHITE);
}

void Renderer::DrawLine(Vector3 point1,Vector3 point2)
{
	DrawLine(point1, point2, Rgba::WHITE);
}

void Renderer::DrawLine(float x1,float y1,float x2,float y2)
{
	DrawLine(Vector3(x1, y1, 0), Vector3(x2, y2, 0), Rgba::WHITE);
}

void Renderer::DrawLine(Vector3 point1, Vector3 point2, Rgba rgba)
{
	Vertex_3DPCU line[2];
	line[0].setPoint(point1.x,point1.y,point1.z);
	line[1].setPoint(point2.x,point2.y,point2.z);

	line[0].m_rgba = rgba;
	line[1].m_rgba = rgba;

	DrawMeshImmediate(PRIMITIVE_LINES,2,line);
}

void Renderer::DrawFeatheredLine(Vector3 point1, Vector3 point2, Rgba startColor, Rgba finalColor)
{
	Vertex_3DPCU line[2];
	line[0].setPoint(point1);
	line[0].setRGBA(startColor);
	line[1].setPoint(point2);
	line[1].setRGBA(finalColor);
	DrawMeshImmediate(PRIMITIVE_LINES,2,line);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/11
*@purpose : GL Comparison operation 
*@param   : Compare operation enum
*@return  : NIL
*//////////////////////////////////////////////////////////////
GLenum depthComparison[8] = { GL_NEVER,GL_LESS,GL_EQUAL,GL_LEQUAL,GL_GREATER,GL_NOTEQUAL,GL_GEQUAL,GL_ALWAYS };
GLenum Renderer::ToGLCompare(eCompare compare)
{
	GLenum GLCompare = depthComparison[compare];
	return GLCompare;
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Convert to GL Blend operation enum
*@param   : renderer Blend enum
*@return  : opengl blend enum
*//////////////////////////////////////////////////////////////
GLenum blendOperation[5] = { GL_FUNC_ADD,GL_FUNC_SUBTRACT,GL_FUNC_REVERSE_SUBTRACT,GL_MIN,GL_MAX };
GLenum Renderer::ToGLBlendOperation(eBlendOperation blendOp)
{
	return blendOperation[blendOp];
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Convert to GL blend factor
*@param   : renderer blend enum
*@return  : GL blend enum
*//////////////////////////////////////////////////////////////
GLenum blendFactors[4] = { GL_ZERO,GL_ONE,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA };
GLenum Renderer::ToGLFactor(eBlendFactor blendFactor)
{
	return blendFactors[blendFactor];
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08 
*@purpose : Convert to GL cull mode
*@param   : Renderer cull mode
*@return  : GL cull mode
*//////////////////////////////////////////////////////////////
GLenum cullModes[3] = { GL_BACK,GL_FRONT,GL_NONE };
GLenum Renderer::ToCullMode(eCullMode cullMode)
{
	return cullModes[cullMode];
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Convert to GL winding order  
*@param   : Renderer Wind enum
*@return  : GL wind enum
*//////////////////////////////////////////////////////////////
GLenum windingOrders[2] = { GL_CW,GL_CCW };
GLenum Renderer::ToWindingOrder(eWindOrder windOrder)
{
	return windingOrders[windOrder];
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Convert to gl fill mode
*@param   : Renderer fill mode
*@return  : GL Fill mode
*//////////////////////////////////////////////////////////////
GLenum fillModes[2] = { GL_FILL,GL_LINE };
GLenum Renderer::ToFillMode(eFillMode fillMode)
{
	return fillModes[fillMode];
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/11
*@purpose : Enables depth buffer
*@param   : Check the comparison enum,should overwrite the buffer
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::EnableDepth(eCompare compare, bool should_write)
{
	// enable/disable the test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(ToGLCompare(compare));
	glDepthMask(should_write ? GL_TRUE : GL_FALSE);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/11
*@purpose : Disable depth buffer
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DisableDepth()
{
	// You can glDisable( GL_DEPTH_TEST ) as well, 
	// but that prevents depth clearing from working, 
	// so I prefer to just use these settings which is 
	// effectively not using the depth buffer.
	EnableDepth(COMPARE_ALWAYS, false);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/11
*@purpose : Clears depth
*@param   : depth value
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::ClearDepth(float depth /*= 1.0f*/)
{
	glDepthMask(GL_TRUE);
	glClearDepthf(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawDottedCircle(float centreX,float centreY,float nradius)
{
	const int MAX_NUM_VERTEX = 40;
	Vertex_3DPCU line[MAX_NUM_VERTEX];
	float nangle = 360.0f/(float)MAX_NUM_VERTEX/2.0f;
	for(int i = 0,j = 0;i <= MAX_NUM_VERTEX/2.f; i++)
	{
		float startX =  centreX + (nradius)*CosDegrees(i*nangle);
		float startY =  centreY + (nradius)*SinDegrees(i*nangle);

		float endX =  centreX + (nradius)*CosDegrees((i+1)*nangle);
		float endY =  centreY + (nradius)*SinDegrees((i+1)*nangle);
		if(i%2==0)
		{
			line[j++].setPoint(startX,startY);
			line[j++].setPoint(endX,endY);
		}
	}
	DrawMeshImmediate(PRIMITIVE_LINES,2,line);
}

void Renderer::DrawCircle(float centreX,float centreY,float radius)
{
	DrawCircle(centreX,centreY,radius,Rgba::WHITE,10);
}

void Renderer::DrawCircle(float centreX,float centreY,float nradius,Rgba color,float numofSize)
{
	UNUSED(numofSize);
	UNUSED(color);
	const int MAX_NUM_VERTEX = 31;
	Vertex_3DPCU line[MAX_NUM_VERTEX];
	float nangle = 360.0f/(MAX_NUM_VERTEX -1);
	for(int i = 0;i < (MAX_NUM_VERTEX - 1); i++)
	{
		float startX =  centreX + (nradius)*CosDegrees(i*nangle);
		float startY =  centreY + (nradius)*SinDegrees(i*nangle);

		float endX   =  centreX + (nradius)*CosDegrees((i+1)*nangle);
		float endY   =  centreY + (nradius)*SinDegrees((i+1)*nangle);
		line[i].setRGBA(color);
		line[i].setPoint(startX,startY);
		line[i + 1].setRGBA(color);
		line[i + 1].setPoint(endX,endY);
	}
	DrawMeshImmediate(PRIMITIVE_LINES,static_cast<int>(MAX_NUM_VERTEX),line);
}

void Renderer::DrawCircle(Vector2 centre, float nradius)
{
	DrawCircle(centre.x,centre.y,nradius);
}

void Renderer::DrawCircle(Disc2 disc, Rgba color)
{
	DrawCircle(disc.center.x,disc.center.y,disc.radius,color,20);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Draws rectangle in 2d space
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawRectangle(AABB2 aabb2)
{
	DrawRectangle(aabb2.mins.x,aabb2.mins.y,aabb2.maxs.x,aabb2.maxs.y);
}
// Draws rectangle rotated by an angle
void Renderer::DrawRectangle(float centreX,float centreY,float length,float bredth,float angle)
{
	float RX = centreX + length/2*CosDegrees(angle);
	float RY = centreY + length/2*SinDegrees(angle);

	float RXTX = RX + bredth/2*CosDegrees(90+angle);
	float RYTY = RY + bredth/2*SinDegrees(90+angle);

	float RXBX = RX + bredth/2*CosDegrees(270+angle);
	float RYBY = RY + bredth/2*SinDegrees(270+angle);

	float LX = centreX + length/2*CosDegrees(180+angle);
	float LY = centreY + length/2*SinDegrees(180+angle);

	float LXTX = LX + bredth/2*CosDegrees(90+angle);
	float LYTY = LY + bredth/2*SinDegrees(90+angle);
		  
	float LXBX = LX + bredth/2*CosDegrees(270+angle);
	float LYBY = LY + bredth/2*SinDegrees(270+angle);
	const int MAX_NUM_VERTEX = 5;

	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];
	rectangle[0].setPoint(LXTX,LYTY);
	rectangle[1].setPoint(RXTX,RYTY);
	rectangle[2].setPoint(RXBX,RYBY);
	rectangle[3].setPoint(LXBX,LYBY);
	rectangle[4].setPoint(LXTX,LYTY);
	DrawMeshImmediate(PRIMITIVE_LINES,2,rectangle);
}

void Renderer::DrawRectangle(float x1, float y1, float x2, float y2)
{
	float LXBX = x1;
	float LYBY = y1;

	float LXTX = x1;
	float LYTY = y2;

	float RXTX = x2;
	float RYTY = y2;

	float RXBX = x2;
	float RYBY = y1;

	const int MAX_NUM_VERTEX = 8;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];
	rectangle[0].setPoint(LXBX,LYBY);
	rectangle[0].setUV(Vector2(0,0));
	rectangle[0].setRGBA(Rgba::WHITE);
	rectangle[1].setPoint(RXBX,RYBY);
	rectangle[1].setUV(Vector2(0, 0));
	rectangle[1].setRGBA(Rgba::WHITE);

	rectangle[2].setPoint(RXBX,RYBY);
	rectangle[2].setUV(Vector2(0, 0));
	rectangle[2].setRGBA(Rgba::WHITE);
	rectangle[3].setPoint(RXTX,RYTY);
	rectangle[3].setUV(Vector2(0, 0));
	rectangle[3].setRGBA(Rgba::WHITE);

	rectangle[4].setPoint(RXTX,RYTY);
	rectangle[4].setUV(Vector2(0, 0));
	rectangle[4].setRGBA(Rgba::WHITE);
	rectangle[5].setPoint(LXTX,LYTY);
	rectangle[5].setUV(Vector2(0, 0));
	rectangle[5].setRGBA(Rgba::WHITE);

	rectangle[6].setPoint(LXTX,LYTY);
	rectangle[6].setUV(Vector2(0, 0));
	rectangle[6].setRGBA(Rgba::WHITE);
	rectangle[7].setPoint(LXBX,LYBY);
	rectangle[7].setUV(Vector2(0, 0));
	rectangle[7].setRGBA(Rgba::WHITE);
	DrawMeshImmediate(PRIMITIVE_LINES,MAX_NUM_VERTEX,rectangle);
}

void Renderer::DrawSolidRectangle(float x1, float y1, float x2, float y2)
{
	DrawRectangle(x1,y1,x2,y2);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Draw Cube in 3D space
*@param   : Center of cube and half length of one side
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawCube(Vector3 centre, float length,Rgba color)
{
	Vertex_3DPCU square[6][6];
	
	for(int index = 0;index <6;index++)
	{
		float xDiff = 0;
		float yDiff = 0;
		float zDiff = 0;
		if(index <2)
		{
			if(index == 0)
			{
				zDiff = length;
			}
			else
			{
				zDiff = -length;
			}
		}
		if (index > 1 && index < 4)
		{
			if (index == 2)
			{
				yDiff = length;
			}
			else
			{
				yDiff = -length;
			}
		}
		if (index > 3 && index < 6)
		{
			if (index == 4)
			{
				xDiff = length;
			}
			else
			{
				xDiff = -length;
			}
		}
		Vector2 texCoordsAtMins(0, 0);
		Vector2 texCoordsAtMaxs(1, 1);
		
		Vector3 currentFaceCentre = Vector3(centre.x - xDiff,centre.y - yDiff,centre.z - zDiff);

		square[index][0].setUV(Vector2(0,0));
		square[index][0].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y - zDiff - xDiff,currentFaceCentre.z + xDiff + yDiff);
		square[index][0].setRGBA(color);

		square[index][1].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
		square[index][1].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y - zDiff - xDiff,currentFaceCentre.z - xDiff + yDiff);
		square[index][1].setRGBA(color);

		square[index][2].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
		square[index][2].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y + zDiff + xDiff,currentFaceCentre.z + xDiff - yDiff);
		square[index][2].setRGBA(color);

		
		square[index][3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
		square[index][3].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y + zDiff + xDiff,currentFaceCentre.z + xDiff - yDiff);
		square[index][3].setRGBA(color);

		square[index][4].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
		square[index][4].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y - zDiff - xDiff,currentFaceCentre.z - xDiff + yDiff);
		square[index][4].setRGBA(color);

		square[index][5].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
		square[index][5].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y + zDiff + xDiff,currentFaceCentre.z - xDiff -yDiff);
		square[index][5].setRGBA(color);
		
		DrawMeshImmediate(PRIMITIVE_TRIANGES,6,square[index]);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/21
*@purpose : Draw cuboid  (TODO merge cube and cuboid)
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawCuboid(Vector3 centre, Vector3 dimensions, Rgba color)
{
	TODO("Cuboid TO be merged with Cube");
	Vertex_3DPCU square[6][6];

	for (int index = 0; index < 6; index++)
	{
		float xDiff = 0;
		float yDiff = 0;
		float zDiff = 0;
		if (index < 2)
		{
			if (index == 0)
			{
				zDiff = dimensions.z;
			}
			else
			{
				zDiff = -dimensions.z;
			}
		}
		if (index > 1 && index < 4)
		{
			if (index == 2)
			{
				yDiff = dimensions.y;
			}
			else
			{
				yDiff = -dimensions.y;
			}
		}
		if (index > 3 && index < 6)
		{
			if (index == 4)
			{
				xDiff = dimensions.x;
			}
			else
			{
				xDiff = -dimensions.x;
			}
		}
		Vector2 texCoordsAtMins(0, 0);
		Vector2 texCoordsAtMaxs(1, 1);

		Vector3 currentFaceCentre = Vector3(centre.x - xDiff, centre.y - yDiff, centre.z - zDiff);

		square[index][0].setUV(Vector2(0, 0));
		square[index][0].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y - zDiff - xDiff, currentFaceCentre.z + xDiff + yDiff);
		square[index][0].setRGBA(color);

		square[index][1].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
		square[index][1].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y - zDiff - xDiff, currentFaceCentre.z - xDiff + yDiff);
		square[index][1].setRGBA(color);

		square[index][2].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
		square[index][2].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y + zDiff + xDiff, currentFaceCentre.z + xDiff - yDiff);
		square[index][2].setRGBA(color);


		square[index][3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
		square[index][3].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y + zDiff + xDiff, currentFaceCentre.z + xDiff - yDiff);
		square[index][3].setRGBA(color);

		square[index][4].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
		square[index][4].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y - zDiff - xDiff, currentFaceCentre.z - xDiff + yDiff);
		square[index][4].setRGBA(color);

		square[index][5].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
		square[index][5].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y + zDiff + xDiff, currentFaceCentre.z - xDiff - yDiff);
		square[index][5].setRGBA(color);

		DrawMeshImmediate(PRIMITIVE_TRIANGES, 6, square[index]);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/12
*@purpose : Draws textured cube in world space
*@param   : Position,size in xyz,texture potiner,topAABB ,side,front uvs
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawTexturedCube(Vector3 centre, Vector3 size, const Texture *texture, AABB2 texCordsTop, AABB2 texCordsSide, AABB2 texCordsFront)
{
	Vertex_3DPCU square[6][6];
	for (int index = 0; index < 6; index++)
	{
		Vector2 texCoordsAtMins(0,0);
		Vector2 texCoordsAtMaxs(1,1);
		float xDiff = 0;
		float yDiff = 0;
		float zDiff = 0;
		if (index < 2)
		{
			texCoordsAtMins = texCordsFront.mins;
			texCoordsAtMaxs = texCordsFront.maxs;
			if (index == 0)
			{
				zDiff = size.z;
			}
			else
			{
				zDiff = -size.z;
			}
		}
		if (index > 1 && index < 4)
		{
			texCoordsAtMins = texCordsTop.mins;
			texCoordsAtMaxs = texCordsTop.maxs;
			if (index == 2)
			{
				yDiff = size.y;
			}
			else
			{
				yDiff = -size.y;
			}
		}
		if (index > 3 && index < 6)
		{
			texCoordsAtMins = texCordsSide.mins;
			texCoordsAtMaxs = texCordsSide.maxs;
			if (index == 4)
			{
				xDiff = size.x;
			}
			else
			{
				xDiff = -size.x;
			}
		}

		Vector3 currentFaceCentre = Vector3(centre.x - xDiff, centre.y - yDiff, centre.z - zDiff);

		square[index][0].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
		square[index][0].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y - zDiff - xDiff, currentFaceCentre.z + xDiff + yDiff);
		square[index][0].setRGBA(Rgba::WHITE);

		square[index][1].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
		square[index][1].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y - zDiff - xDiff, currentFaceCentre.z - xDiff + yDiff);
		square[index][1].setRGBA(Rgba::WHITE);

		square[index][2].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
		square[index][2].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y + zDiff + xDiff, currentFaceCentre.z + xDiff - yDiff);
		square[index][2].setRGBA(Rgba::WHITE);


		square[index][3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
		square[index][3].setPoint(currentFaceCentre.x - zDiff - yDiff, currentFaceCentre.y + zDiff + xDiff, currentFaceCentre.z + xDiff - yDiff);
		square[index][3].setRGBA(Rgba::WHITE);

		square[index][4].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
		square[index][4].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y - zDiff - xDiff, currentFaceCentre.z - xDiff + yDiff);
		square[index][4].setRGBA(Rgba::WHITE);

		square[index][5].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
		square[index][5].setPoint(currentFaceCentre.x + zDiff + yDiff, currentFaceCentre.y + zDiff + xDiff, currentFaceCentre.z - xDiff - yDiff);
		square[index][5].setRGBA(Rgba::WHITE);
		Texture::SetCurrentTexture((Texture*)texture);
		DrawMeshImmediate(PRIMITIVE_TRIANGES, 6, square[index]);
	}
}

void Renderer::DrawTexturedAABB(const AABB2& bounds,const Texture *texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& color)
{
	UNUSED(texture);
	const int MAX_NUM_VERTEX = 6;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];

	rectangle[0].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	rectangle[0].setPoint(bounds.mins.x, bounds.mins.y);
	rectangle[0].setRGBA(color);

	rectangle[1].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[1].setPoint(bounds.maxs.x, bounds.mins.y);
	rectangle[1].setRGBA(color);

	rectangle[2].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[2].setPoint(bounds.mins.x, bounds.maxs.y);
	rectangle[2].setRGBA(color);

	rectangle[3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[3].setPoint(bounds.maxs.x, bounds.mins.y);
	rectangle[3].setRGBA(color);

	rectangle[4].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
	rectangle[4].setPoint(bounds.maxs.x, bounds.maxs.y);
	rectangle[4].setRGBA(color);

	rectangle[5].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[5].setPoint(bounds.mins.x, bounds.maxs.y);
	rectangle[5].setRGBA(color);
	
	DrawMeshImmediate(PRIMITIVE_TRIANGES, MAX_NUM_VERTEX, rectangle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/15
*@purpose : Draws textured aabb on XZ plane 
*@param   : AABB bounds, Y heigth,Texture, texcordsmins, textcords max,rgb color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawTexturedAABBOnXY(const AABB2& bounds, Vector3 rigthDirection,Vector3 upDirection, float zValue, const Texture *texture, const Vector2& mins, const Vector2& maxs, const Rgba& color)
{
	UNUSED(texture);
	Vector3 position(bounds.GetCenter().x, bounds.GetCenter().y, zValue);
	Vector2 dimension = bounds.GetDimensions();
	const int MAX_NUM_VERTEX = 6;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];

	rectangle[0].setUV(Vector2(mins.x, mins.y));
	rectangle[0].setPoint(position - rigthDirection * dimension.x - upDirection * dimension.y);
	rectangle[0].setRGBA(color);

	rectangle[1].setUV(Vector2(maxs.x, mins.y));
	rectangle[1].setPoint(position + rigthDirection * dimension.x - upDirection * dimension.y);
	rectangle[1].setRGBA(color);

	rectangle[2].setUV(Vector2(mins.x, maxs.y));
	rectangle[2].setPoint(position - rigthDirection * dimension.x + upDirection * dimension.y);
	rectangle[2].setRGBA(color);

	rectangle[3].setUV(Vector2(maxs.x, mins.y));
	rectangle[3].setPoint(position + rigthDirection * dimension.x - upDirection * dimension.y);
	rectangle[3].setRGBA(color);

	rectangle[4].setUV(Vector2(maxs.x, maxs.y));
	rectangle[4].setPoint(position + rigthDirection * dimension.x + upDirection * dimension.y);
	rectangle[4].setRGBA(color);

	rectangle[5].setUV(Vector2(mins.x, maxs.y));
	rectangle[5].setPoint(position - rigthDirection * dimension.x + upDirection * dimension.y);
	rectangle[5].setRGBA(color);

	DrawMeshImmediate(PRIMITIVE_TRIANGES, MAX_NUM_VERTEX, rectangle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Draw texture on XZ plane
*@param   : AABB Bounds, Texture mins and max UVs,color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawTexturedAABBOn3D( Vector3 position, Vector3 rigthDirection,Vector3 upDirection, Vector2 dimension, const Texture *texture,const Vector2& mins, const Vector2& maxs,const Rgba& color)
{
	UNUSED(texture);
	const int MAX_NUM_VERTEX = 6;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];

	rectangle[0].setUV(Vector2(mins.x, mins.y));
	rectangle[0].setPoint(position - rigthDirection*dimension.x - upDirection*dimension.y);
	rectangle[0].setRGBA(color);

	rectangle[1].setUV(Vector2(maxs.x, mins.y));
	rectangle[1].setPoint(position + rigthDirection*dimension.x - upDirection*dimension.y);
	rectangle[1].setRGBA(color);

	rectangle[2].setUV(Vector2(mins.x, maxs.y));
	rectangle[2].setPoint(position - rigthDirection*dimension.x + upDirection*dimension.y);
	rectangle[2].setRGBA(color);

	rectangle[3].setUV(Vector2(maxs.x, mins.y));
	rectangle[3].setPoint(position + rigthDirection*dimension.x - upDirection*dimension.y);
	rectangle[3].setRGBA(color);

	rectangle[4].setUV(Vector2(maxs.x, maxs.y));
	rectangle[4].setPoint(position + rigthDirection*dimension.x + upDirection*dimension.y);
	rectangle[4].setRGBA(color);

	rectangle[5].setUV(Vector2(mins.x, maxs.y));
	rectangle[5].setPoint(position - rigthDirection*dimension.x + upDirection*dimension.y);
	rectangle[5].setRGBA(color);

	DrawMeshImmediate(PRIMITIVE_TRIANGES, MAX_NUM_VERTEX, rectangle);
}

void Renderer::DrawAABB(const AABB2& bounds, const Rgba& color)
{
	const int MAX_NUM_VERTEX = 6;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];
	Vector2 texCoordsAtMins(0,0);
	Vector2 texCoordsAtMaxs(1,1);

	rectangle[0].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	rectangle[0].setPoint(bounds.mins.x,bounds.mins.y);
	rectangle[0].setRGBA(color);

	rectangle[1].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[1].setPoint(bounds.maxs.x, bounds.mins.y);
	rectangle[1].setRGBA(color);

	rectangle[2].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[2].setPoint(bounds.mins.x,bounds.maxs.y);
	rectangle[2].setRGBA(color);

	rectangle[3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[3].setPoint(bounds.maxs.x, bounds.mins.y);
	rectangle[3].setRGBA(color);

	rectangle[4].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
	rectangle[4].setPoint(bounds.maxs.x, bounds.maxs.y);
	rectangle[4].setRGBA(color);

	rectangle[5].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[5].setPoint(bounds.mins.x, bounds.maxs.y);
	rectangle[5].setRGBA(color);
	DrawMeshImmediate(PRIMITIVE_TRIANGES,MAX_NUM_VERTEX,rectangle);
}

void Renderer::DrawAABB(const AABB2& bounds, const Rgba& color1,const Rgba& color2)
{
	const int MAX_NUM_VERTEX = 4;
	Vertex_3DPCU rectangle[4];
	Vector2 texCoordsAtMins(0,1);
	Vector2 texCoordsAtMaxs(1,0);

	rectangle[0].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	rectangle[0].setPoint(bounds.mins.x,bounds.mins.y);
	rectangle[0].setRGBA(color1);
	rectangle[1].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[1].setPoint(bounds.mins.x,bounds.maxs.y);
	rectangle[1].setRGBA(color1);
	rectangle[2].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
	rectangle[2].setPoint(bounds.maxs.x,bounds.maxs.y);
	rectangle[2].setRGBA(color2);
	rectangle[3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[3].setPoint(bounds.maxs.x,bounds.mins.y);
	rectangle[3].setRGBA(color2);
	DrawMeshImmediate(PRIMITIVE_QUADS,MAX_NUM_VERTEX,rectangle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : DrawAABB in XY plane
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawAABBXY(Vector3 position,Vector2 dimension, Vector3 rigthDirection,Vector3 upDirection,const Rgba& color)
{
	const int MAX_NUM_VERTEX = 6;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];
	Vector2 mins(0, 0);
	Vector2 maxs(1, 1);

	rectangle[0].setUV(Vector2(mins.x, mins.y));
	rectangle[0].setPoint(position - rigthDirection * dimension.x - upDirection * dimension.y);
	rectangle[0].setRGBA(color);

	rectangle[1].setUV(Vector2(maxs.x, mins.y));
	rectangle[1].setPoint(position + rigthDirection * dimension.x - upDirection * dimension.y);
	rectangle[1].setRGBA(color);

	rectangle[2].setUV(Vector2(mins.x, maxs.y));
	rectangle[2].setPoint(position - rigthDirection * dimension.x + upDirection * dimension.y);
	rectangle[2].setRGBA(color);

	rectangle[3].setUV(Vector2(maxs.x, mins.y));
	rectangle[3].setPoint(position + rigthDirection * dimension.x - upDirection * dimension.y);
	rectangle[3].setRGBA(color);

	rectangle[4].setUV(Vector2(maxs.x, maxs.y));
	rectangle[4].setPoint(position + rigthDirection * dimension.x + upDirection * dimension.y);
	rectangle[4].setRGBA(color);

	rectangle[5].setUV(Vector2(mins.x, maxs.y));
	rectangle[5].setPoint(position - rigthDirection * dimension.x + upDirection * dimension.y);
	rectangle[5].setRGBA(color);

	DrawMeshImmediate(PRIMITIVE_TRIANGES, MAX_NUM_VERTEX, rectangle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Draw AABB in XZ plane
*@param   : Params in XZ and color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawAABBXZ(const AABB2& bounds,float y, const Rgba& color)
{
	const int MAX_NUM_VERTEX = 6;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];
	Vector2 texCoordsAtMins(0, 0);
	Vector2 texCoordsAtMaxs(1, 1);

	rectangle[0].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	rectangle[0].setPoint(bounds.mins.x,y,bounds.mins.y);
	rectangle[0].setRGBA(color);

	rectangle[1].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[1].setPoint(bounds.maxs.x,y, bounds.mins.y);
	rectangle[1].setRGBA(color);

	rectangle[2].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[2].setPoint(bounds.mins.x,y, bounds.maxs.y);
	rectangle[2].setRGBA(color);

	rectangle[3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[3].setPoint(bounds.maxs.x,y, bounds.mins.y);
	rectangle[3].setRGBA(color);

	rectangle[4].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
	rectangle[4].setPoint(bounds.maxs.x,y, bounds.maxs.y);
	rectangle[4].setRGBA(color);

	rectangle[5].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[5].setPoint(bounds.mins.x,y, bounds.maxs.y);
	rectangle[5].setRGBA(color);
	DrawMeshImmediate(PRIMITIVE_TRIANGES, MAX_NUM_VERTEX, rectangle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Draw AABB in YZ plane
*@param   : AABB in YZ ,X of aabb value and color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawAABBYZ(const AABB2& bounds, float X, const Rgba& color)
{
	const int MAX_NUM_VERTEX = 6;
	Vertex_3DPCU rectangle[MAX_NUM_VERTEX];
	Vector2 texCoordsAtMins(0, 0);
	Vector2 texCoordsAtMaxs(1, 1);

	rectangle[0].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	rectangle[0].setPoint(X,bounds.mins.x, bounds.mins.y);
	rectangle[0].setRGBA(color);

	rectangle[1].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[1].setPoint(X,bounds.maxs.x, bounds.mins.y);
	rectangle[1].setRGBA(color);

	rectangle[2].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[2].setPoint(X,bounds.mins.x, bounds.maxs.y);
	rectangle[2].setRGBA(color);

	rectangle[3].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	rectangle[3].setPoint(X,bounds.maxs.x, bounds.mins.y);
	rectangle[3].setRGBA(color);

	rectangle[4].setUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
	rectangle[4].setPoint(X,bounds.maxs.x, bounds.maxs.y);
	rectangle[4].setRGBA(color);

	rectangle[5].setUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	rectangle[5].setPoint(X,bounds.mins.x, bounds.maxs.y);
	rectangle[5].setRGBA(color);
	DrawMeshImmediate(PRIMITIVE_TRIANGES, MAX_NUM_VERTEX, rectangle);
}

void Renderer::DrawTextOnPoint(char value[],int start,int length, Vector3 position,float height,Rgba rgba)
{
	DrawTextOnPoint(value,start,length, Vector2(position.x,position.y),height,rgba);
}

void Renderer::DrawTextOnPoint(char value[],int start,int length, Vector2 position,float height,Rgba rgba)
{
	DrawTextOn3DPoint(Vector3(position.x, position.y, 0), Vector3::RIGHT, Vector3::UP, std::string(value), height, rgba);
	if(true)
	{
		return;
	}
	Material *sampleMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	BitmapFont *bitmapFont   = BitmapFont::CreateOrGetBitmapFont("Data//Font//font2.png");
	IntVector2 spriteCords   = bitmapFont->m_spriteSheet->GetSpriteLayout();
	Texture::SetCurrentTexture((Texture*)bitmapFont->m_spriteSheet->getTexture());
	sampleMaterial->m_textures.at(0) = (Texture*)bitmapFont->m_spriteSheet->getTexture();
	BindMaterial(sampleMaterial);
	/*BindShader(Shader::GetDefaultShader());
	BindSampler(0U, Sampler::GetDefaultSampler());
	BindTexture(Texture::GetCurrentTexture());*/
	for(int index = start;index<length;index++)
	{
		char ch			= value[index];
		position.x		+= height*2;
		int spriteIndex = (int)ch;

		//spriteIndex = (15 - (spriteIndex / 16))*16 + (spriteIndex % 16);

		spriteIndex = (spriteCords.y - 1 - (spriteIndex / spriteCords.y))*spriteCords.y + (spriteIndex % spriteCords.x);
		AABB2 aabb2 = bitmapFont->m_spriteSheet->GetTexCoordsForSpriteIndex(spriteIndex);
		Texture::SetCurrentTexture((Texture*)bitmapFont->m_spriteSheet->getTexture());

		Vector2 minPosition = aabb2.mins;
		Vector2 maxPosition = aabb2.maxs;
		AABB2 textPosition(position,static_cast<float>(height),static_cast<float>(height));
		
		DrawTexturedAABB(textPosition,Texture::GetCurrentTexture(),minPosition,maxPosition,rgba);
	}
	delete sampleMaterial;
}

void Renderer::DrawTextOnPoint(char value[],int start,int length, Vector2 pos,float height)
{
	DrawTextOnPoint(value,start,length,pos,height,Rgba::WHITE);
}

void Renderer::DrawTextOnPoint(std::string str, int start, int length, Vector2 pos, float height, Rgba rgba)
{
	char *charArray = &str[0];
	DrawTextOnPoint(charArray,start,length,pos,height,rgba);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Draws text on 3d point along the direction
*@param   : Position , direction along,string , color
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawTextOn3DPoint(Vector3 position, Vector3 rigthDirection,Vector3 upDirection, std::string stringValue, float heigth, Rgba color)
{
	Vector2 position2D;
	for (int index = 0; index < stringValue.length(); index++)
	{
		char ch = stringValue.at(index);
		int spriteIndex = (int)ch;

		//spriteIndex = (15 - (spriteIndex / 16))*16 + (spriteIndex % 16);

		BitmapFont *bitmapFont = BitmapFont::CreateOrGetBitmapFont("Data\\Font\\font2.png");
		IntVector2 spriteCords = bitmapFont->m_spriteSheet->GetSpriteLayout();
		spriteIndex = (spriteCords.y - 1 - (spriteIndex / spriteCords.y))*spriteCords.y + (spriteIndex % spriteCords.x);
		AABB2 aabb2 = bitmapFont->m_spriteSheet->GetTexCoordsForSpriteIndex(spriteIndex);
		Texture::SetCurrentTexture((Texture*)bitmapFont->m_spriteSheet->getTexture());

		Vector2 minPosition = aabb2.mins;
		Vector2 maxPosition = aabb2.maxs;
		DrawTexturedAABBOn3D(position, rigthDirection, upDirection, Vector2(heigth, heigth), Texture::GetCurrentTexture(), minPosition, maxPosition, color);
		position += rigthDirection * (heigth * 2);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Draws text on point with CR(Carriage return -> \n or enter)
*@param   : String , starting character index, length of the string , position, heigth of font, color rgba
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Renderer::DrawTextOnPointWithCR(std::vector<std::string> stringArray,Vector2 pos, float height, Rgba rgba)
{
	DrawTextOnPointWithCR(stringArray,pos,height,rgba,0);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Draws text on point with CR(Carriage return -> \n or enter)
*@param   : String , starting character index, length of the string , position, heigth of font, color rgba
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawTextOnPointWithCR(std::vector<std::string> stringArray,Vector2 pos, float height, Rgba rgba,int startIndex)
{
	float positionY = pos.y + static_cast<float>(stringArray.size()) * height*2;
	positionY -= (startIndex * height*2);
	for (int index = 0; index < stringArray.size() - startIndex; index++)
	{
		Vector2 startPosition(pos.x, positionY);
		if (stringArray.at(index) == "Invalid Command")
		{
			DrawTextOnPoint(stringArray.at(index), 0, static_cast<int>(stringArray.at(index).length()), startPosition, height, Rgba::RED);
			positionY -= height*2;
			continue;
		}
		DrawTextOnPoint(stringArray.at(index), 0, static_cast<int>(stringArray.at(index).length()), startPosition, height, rgba);
		positionY -= height*2;
	}
}

void Renderer::DrawInt(int value, Vector2 pos,int height)
{
	std::string strValue ;
	int j = 0;
	while(value>0)
	{
		char val = '0' + value%10;
		strValue += val;
		value = value/10;
		j++;
	}
	strValue[j] = '\0';
	std::reverse(strValue.begin(),strValue.end());
	DrawTextOnPoint(strValue,0,static_cast<int>(strValue.length()),pos,static_cast<float>(height),Rgba::WHITE);
}

void Renderer::DrawTextInBox2D(AABB2 aabb2, std::string text, float aspectRatio, float height, Vector2 alignment, TextDrawMode drawMode)
{
	float length = aspectRatio*height*text.size();
	float boxHeight = aabb2.maxs.y - aabb2.mins.y;
	float boxWidth  = aabb2.maxs.x - aabb2.mins.x;
	float size = static_cast<float>(text.size());
	float lengthOfEach  = length/text.size();
	Vector2 startPosition(0,0);
	startPosition = Vector2(aabb2.mins.x + lengthOfEach/2,aabb2.GetCenter().y);
	float offset = 0;
	float offsetDown = 0;

	AABB2 actualAABB2(aabb2.mins.x,aabb2.mins.y,aabb2.maxs.x,aabb2.maxs.y);
	
	BitmapFont *bitMapFont = BitmapFont::CreateOrGetBitmapFont("font2");
	switch(drawMode)	
	{
		case TEXT_DRAW_SHRINK_TO_FIT:
			if(height > boxHeight)
			{
				float scaleFactor = height/boxHeight;
				length = length/scaleFactor;
				height = height/scaleFactor;
			}
			if(length > boxWidth)
			{
				float scaleFactor = length/boxWidth;
				length = length/scaleFactor;
				height = height/scaleFactor;
			}
			lengthOfEach = length/text.size();
			

			if(alignment.x == 0 && alignment.y == 0)
			{
				offsetDown = actualAABB2.maxs.y - actualAABB2.mins.y - height/2;
			}


			if(alignment.x == .5f && alignment.y == 0.5)
			{
				if(length < boxWidth)
				{
					float midPoint = boxWidth/2;
					offset = midPoint-length/2.0f;
				}

				if(height < boxHeight)
				{
					float midPoint = boxHeight/2;
					offsetDown = midPoint-height/2.0f;
				}
			}

			if(alignment.x == 1 && alignment.y == 1)
			{
				if(length < boxWidth)
				{
					offset = boxWidth-length;
				}
				if(height < boxHeight)
				{
					offsetDown = height/2;//actualAABB2.maxs.y - actualAABB2.mins.y;
				}
			}

			startPosition = Vector2(aabb2.mins.x + lengthOfEach/2 + offset,aabb2.mins.y+offsetDown);
			DrawText2D(startPosition,text,height,Rgba(255,0,0,255),aspectRatio,bitMapFont);
			break;
		case TEXT_DRAW_WORD_WRAP: // area wise shrink
			{

				float widthAllowedToDraw = actualAABB2.maxs.x - actualAABB2.mins.x;
				float heightAllowedToDraw = actualAABB2.maxs.y - actualAABB2.mins.y;
				
				//float actualTotalNumOnRigth = widthAllowedToDraw/height;
				//float actualTotalNumOnDown  = heightAllowedToDraw/height;
				
				float totalCharAllowedToDraw = widthAllowedToDraw*heightAllowedToDraw; // calculating the area available to draw
				float value = totalCharAllowedToDraw/size;// total area divided by number of characters
				height = sqrt(value);// heigth * widht both are same taking square root
				
				float numOfCharAllowedToDrawRight = widthAllowedToDraw/height;

				if(numOfCharAllowedToDrawRight >= size)
				{
					numOfCharAllowedToDrawRight = size;
				}
				
				int index = static_cast<int>(numOfCharAllowedToDrawRight);
				int minValue = 0;
				float startHeigth = actualAABB2.maxs.y;// - height/2;
				for(; minValue < size; index += static_cast<int>(numOfCharAllowedToDrawRight))
				{
					std::string str = text.substr(minValue,static_cast<int>(numOfCharAllowedToDrawRight));
					startPosition = Vector2(actualAABB2.mins.x + height/2,startHeigth);
				
					DrawText2D(startPosition,str,height,Rgba(255,0,0,255),aspectRatio,bitMapFont);
					minValue = index;
					startHeigth -= height;
				}
			}
			break;
		case TEXT_DRAW_WORD_WRAP1:
		{
			/*int numOfStepDown = 1;
			for(int index = 0;index < size;index++)
			{
				if(text[index] == '\n')
				{
					numOfStepDown++;
				}
			}
			float newHeigth = height*numOfStepDown;

			if(newHeigth > boxHeight)
			{
				float scaleFactor = height/boxHeight;
				length = length/scaleFactor;
				height = height/scaleFactor;
			}
			

			lengthOfEach = length/text.size();
			int lastIndex = 0;
			float startHeigth = actualAABB2.maxs.y;// - height/2;
			startPosition = Vector2(aabb2.mins.x + height/2,startHeigth);
			for(int index = 0;index < size;index++)
			{
				if(text[index] == '\n')
				{
					startPosition.y = startHeigth;
					DrawText2D(startPosition,text.substr(lastIndex,index-lastIndex),height,Rgba(255,0,0,255),aspectRatio,bitMapFont);
					lastIndex = index;
					startHeigth -= height;
				}
			}
			//startHeigth -= height;
			startPosition.y = startHeigth;
			DrawText2D(startPosition,text.substr(lastIndex,size),height,Rgba(255,0,0,255),aspectRatio,bitMapFont);
			
			break;*/
		}
		break;
		case TEXT_DRAW_OVERRUN:
			//if(alignment.x == 0 && alignment.y == 0)
			{
				offsetDown = actualAABB2.maxs.y - actualAABB2.mins.y - height/2;
			}


			if(alignment.x == .5f && alignment.y == 0.5)
			{
				//if(length < boxWidth)
				{
					float midPoint = boxWidth/2;
					offset = midPoint-length/2.0f;
				}

				//if(height < boxHeight)
				{
					float midPoint = boxHeight/2;
					offsetDown = midPoint-height/2.0f;
				}
			}

			if(alignment.x == 1 && alignment.y == 1)
			{
				//if(length < boxWidth)
				{
					offset = boxWidth-length;
				}
				//if(height < boxHeight)
				{
					offsetDown = height/2;//actualAABB2.maxs.y - actualAABB2.mins.y;
				}
			}

			startPosition = Vector2(aabb2.mins.x + lengthOfEach/2 + offset,aabb2.mins.y+offsetDown);
			DrawText2D(startPosition,text,height,Rgba(255,0,0,255),aspectRatio,bitMapFont);
			break;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/06
*@purpose : Sets uniform to the current shader 
*@param   : Uniform name ,value
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetUniform(std::string name,Vector3 value)
{
	GLint bindindx = glGetUniformLocation(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle(), name.c_str());
	if (bindindx >= 0)
	{
		glUniform3fv(bindindx,1,(GLfloat*)&value);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/07
*@purpose : Sets unifrom for the current shader program
*@param   : uniform name and value
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetUniform(std::string name, float value)
{
	GLint uniformfloat = glGetUniformLocation(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle(), name.c_str());
	if (uniformfloat >= 0)
	{
		glUniform1fv(uniformfloat,1, &value);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/09
*@purpose : Sets unifrom for the current shader program
*@param   : uniform name and value
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetUniform(std::string name, Vector4 value)
{
	GLint uniformfloat = glGetUniformLocation(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle(), name.c_str());
	if (uniformfloat >= 0)
	{
		glUniform4f(uniformfloat, (GLfloat)value.x,(GLfloat)value.y,(GLfloat)value.z,(GLfloat)value.w);
		GL_CHECK_ERROR();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/16
*@purpose : Sets uniform for the current shader program
*@param   : uniform name and value
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetUniform(std::string name, int count, Vector3 *value)
{
	GLint uniformfloat = glGetUniformLocation(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle(), name.c_str());
	if (uniformfloat >= 0)
	{
		glUniform3fv(uniformfloat,count,(GLfloat*)value);
		GL_CHECK_ERROR();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/16
*@purpose : Sets unifrom for the current shader program
*@param   : uniform name and value
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetUniform(std::string name, int count, Vector4 *value)
{
	GLint uniformfloat = glGetUniformLocation(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle(), name.c_str());
	if (uniformfloat >= 0)
	{
		glUniform4fv(uniformfloat, count, (GLfloat*)value);
		GL_CHECK_ERROR();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/17
*@purpose : Binds uniform block to current shader program
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetUniformBlockBinding(std::string name)
{
	int block_index = glGetUniformBlockIndex(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle(), name.c_str());
	if(block_index >= 0)
	{
		glUniformBlockBinding(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle(), block_index, (GLuint)1);
	}
	GL_CHECK_ERROR();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/17
*@purpose : Binds uniform buffer shader program
*@param   : Slot to bu=ind,uniform buffer,total size,data
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindUniformBuffer(size_t slotValue, UniformBuffer *buffer, size_t size, const void *data)
{
	buffer->SetGPUBuffer((GLuint)size, data);
	glBindBufferBase(GL_UNIFORM_BUFFER,(GLuint)slotValue, buffer->m_handle);
	GL_CHECK_ERROR();
}

void Renderer::DrawText2D(const Vector2& drawMins, const std::string& asciiText, float cellHeight, Rgba tint, float aspectScale /*= 1.f*/, const BitmapFont* font /*= nullptr */)
{
	UNUSED(font);
	aspectScale = 1.0f;
	DrawTextOnPoint(asciiText,(int)0,static_cast<int>(asciiText.length()),drawMins,(cellHeight),tint);
}

int g_openGlPrimitiveTypes[ NUM_PRIMITIVE_TYPES ] =
{
	GL_POINTS,			// called PRIMITIVE_POINTS		in our engine
	GL_LINES,			// called PRIMITIVE_LINES		in our engine
	GL_TRIANGLES,		// called PRIMITIVE_TRIANGES	in our engine
	GL_QUADS			// called PRIMITIVE_QUADS		in our engine
};

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/24
*@purpose : Renderer startup
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
bool Renderer::RenderStartup()
{
	// load and get a handle to the opengl dll (dynamic link library)
	gGLLibrary = ::LoadLibraryA("opengl32.dll");
	HWND hwnd = GetActiveWindow();

	// Get the Device Context (DC) - how windows handles the interace to rendering devices
	// This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call. 
	HDC hdc = ::GetDC(hwnd);
	// use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
	// This should be very simiilar to SD1
	HGLRC temp_context = CreateOldRenderContext(hdc);
	
	::wglMakeCurrent(hdc, temp_context);
	BindNewWGLFunctions();  // find the functions we'll need to create the real context; 

							// create the real context, using opengl version 4.2
	HGLRC real_context = CreateRealRenderContext(hdc, 4, 2);
	
	// Set and cleanup
	::wglMakeCurrent(hdc, real_context);
	::wglDeleteContext(temp_context);
	// Bind all our OpenGL functions we'll be using.
	BindGLFunctions();

	// set the globals
	gGLwnd = hwnd;
	gHDC = hdc;
	gGLContext = real_context;
	temp_render_buffer = new RenderBuffer();
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/24
*@purpose : NIL
*@param   : Handle device context
*@return  : NIL
*//////////////////////////////////////////////////////////////
HGLRC Renderer::CreateOldRenderContext(HDC hdc)
{
	// Setup the output to be able to render how we want
	// (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
	// and is double buffered
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
	pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
	pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN

									 // Find a pixel format that matches our search criteria above. 
	int pixel_format = ::ChoosePixelFormat(hdc, &pfd);
	if (pixel_format == NULL) {
		return NULL;
	}

	// Set our HDC to have this output. 
	if (!::SetPixelFormat(hdc, pixel_format, &pfd)) {
		return NULL;
	}

	// Create the context for the HDC
	HGLRC context = wglCreateContext(hdc);
	if (context == NULL) {
		return NULL;
	}
	// return the context; 
	return context;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/26
*@purpose : Binds all new gl functions
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindNewWGLFunctions()
{
	GL_BIND_FUNCTION(wglGetExtensionsStringARB);
	GL_BIND_FUNCTION(wglChoosePixelFormatARB);
	GL_BIND_FUNCTION(wglCreateContextAttribsARB);
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/24
*@purpose : Binding gl functions
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindGLFunctions()
{
	glfunctions::BindGLFunctions();
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/24
*@purpose : modern context
*@param   : device context
*@return  : NIL
*//////////////////////////////////////////////////////////////
HGLRC Renderer::CreateRealRenderContext(HDC hdc, int major, int minor)
{
	// So similar to creating the temp one - we want to define 
	// the style of surface we want to draw to.  But now, to support
	// extensions, it takes key_value pairs
	int const format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,    // The rc will be used to draw to a window
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,    // ...can be drawn to by GL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,     // ...is double buffered
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // ...uses a RGBA texture
		WGL_COLOR_BITS_ARB, 24,             // 24 bits for color (8 bits per channel)
											// WGL_DEPTH_BITS_ARB, 24,          // if you wanted depth a default depth buffer...
											// WGL_STENCIL_BITS_ARB, 8,         // ...you could set these to get a 24/8 Depth/Stencil.
											NULL, NULL,                         // Tell it we're done.
	};

	// Given the above criteria, we're going to search for formats
	// our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
	size_t const MAX_PIXEL_FORMATS = 128;
	int formats[MAX_PIXEL_FORMATS];
	int pixel_format = 0;
	UINT format_count = 0;
	
	BOOL succeeded = wglChoosePixelFormatARB(hdc,format_attribs,nullptr,MAX_PIXEL_FORMATS,formats,(UINT*)&format_count);

	if (!succeeded) {
		return NULL;
	}

	// Loop through returned formats, till we find one that works
	for (unsigned int i = 0; i < format_count; ++i) 
	{
		pixel_format = formats[i];
		succeeded = SetPixelFormat(hdc, pixel_format, NULL); // same as the temp context; 
		if (succeeded) {
			break;
		}
		else 
		{
			DWORD error = GetLastError();
			char msgbuf[1000];
			sprintf_s(msgbuf, "FILE : %s :: Line Num %d\n%u", __FILE__, __LINE__,error);
			OutputDebugStringA(msgbuf);
			//Logf("Failed to set the format: %u", error);
		}
	}

	if (!succeeded) {
		return NULL;
	}

	// Okay, HDC is setup to the rihgt format, now create our GL context

	// First, options for creating a debug context (potentially slower, but 
	// driver may report more useful errors). 
	int context_flags = 0;
#if defined(_DEBUG)
	context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

	// describe the context
	int const attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
		WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
		0, 0
	};

	// Try to create context
	HGLRC context = wglCreateContextAttribsARB(hdc, NULL, attribs);
	if (context == NULL) {
		return NULL;
	}

	return context;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/25
*@purpose : Shutdown Rendering
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void GLShutdown()
{
	wglMakeCurrent(gHDC, NULL);

	::wglDeleteContext(gGLContext);
	::ReleaseDC(gGLwnd, gHDC);

	gGLContext = NULL;
	gHDC = NULL;
	gGLwnd = NULL;

	::FreeLibrary(gGLLibrary);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/25
*@purpose : Post startup
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::PostStartup()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	int window_width  = Windows::GetInstance()->GetDimensions().x;
	int window_height = Windows::GetInstance()->GetDimensions().y;
	ShaderDefinitions::LoadDefintions("Data//Definitions//Shaders.xml", this);
	Light::Initialize();
	glGenVertexArrays(1, &m_defaultVAO);
	glBindVertexArray(m_defaultVAO);
	
	Image  * onePixelImage  = new Image(Rgba::WHITE);
	Texture* defaultTexture = new Texture(*onePixelImage);
	Texture* shadowDepth	= new Texture(*onePixelImage);
	Texture::SetDefaultTexture(defaultTexture);
	Texture::SetCurrentTexture(defaultTexture);
	Texture::SetDefaultShadowDepthTexture(shadowDepth);
	Texture * defaultColorTarget = CreateRenderTarget(window_width, window_height, TEXTURE_FORMAT_RGBA8);
	Texture * defaultDepthTarget = CreateRenderTarget(window_width, window_height, TEXTURE_FORMAT_D24S8);
	Texture::SetDefaultColorTargetTexture(defaultColorTarget);
	Texture::SetDefaultDepthTargetTexture(defaultDepthTarget);

	Sampler *defaultSampler = new Sampler();
	Sampler *effectSampler  = new Sampler();
	defaultSampler->Create();
	effectSampler->Create();
	Sampler::SetDefaultSampler(defaultSampler);
	Sampler::SetCurrentSampler(defaultSampler);
	Sampler::SetEffectSampler(effectSampler);

	ShaderProgram *defaultShaderProgram = ShaderProgram::CreateOrGetShaderProgram("Data\\Shaders\\PASSTHROUGH");
	ShaderProgram::SetDefaultShaderProgram(defaultShaderProgram);
	ShaderProgram::SetCurrentShaderProgram(defaultShaderProgram);

	Shader *defaultShader = new Shader(this);
	defaultShader->SetProgram(ShaderProgram::GetDefaultShaderProgram());
	Shader::SetDefaultShader(defaultShader);
	Shader::SetCurrentShader(defaultShader);

	m_immediateBufferIBO = new RenderBuffer();
	m_immediateBufferVBO = new RenderBuffer();

	m_effectFrameBuffer  = new FrameBuffer();
	m_copyFrameBuffer    = new FrameBuffer();

	Camera::InitCamera();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/25
*@purpose : DrawMesh
*@param   : Draw type,vertex count, vertex data
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawMeshImmediate(DrawPrimitiveType drawPrimitive, unsigned int const vertex_count, Vertex_3DPCU const *buffer)
{
	Mesh *mesh = new Mesh();
	mesh->SetVertices<Vertex_3DPCU>(static_cast<int>(vertex_count), buffer);
	mesh->m_drawInstruction.m_elemCount = vertex_count;
	mesh->m_drawInstruction.m_primitiveType = drawPrimitive;
	mesh->m_drawInstruction.m_startIndex = 0;
	mesh->m_drawInstruction.m_usingIndices = false;
	Matrix44 model;
	DrawMesh(mesh,model);
	delete mesh;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/12
*@purpose : DrawMesh with index buffer
*@param   : Primitive type of draw,vertex count,vertex buffer,indexbuffer,indexcount
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawMeshImmediateWithIndices(DrawPrimitiveType drawPrimitive, unsigned int const vertex_count, Vertex_3DPCU const *vertices, int *indices, int icount)
{
	m_immediateBufferVBO->CopyToGPU(vertex_count*sizeof(Vertex_3DPCU),vertices);
	m_immediateBufferIBO->CopyToGPU(icount*sizeof(int),indices);			    

	glBindBuffer(GL_ARRAY_BUFFER, m_immediateBufferVBO->GetHandle());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_immediateBufferIBO->GetHandle());

	GLuint program_handle = Shader::GetCurrentShader()->GetShaderProgram()->GetHandle();
	glUseProgram(program_handle);
	// Describe the buffer - first, figure out where the shader is expecting
	GLint projection = glGetUniformLocation(program_handle, "PROJECTION");
	if (projection >= 0)
	{
		glUniformMatrix4fv(projection, 1, GL_FALSE, (GLfloat*)&Camera::GetCurrentCamera()->m_projection);
	}

	GLint view = glGetUniformLocation(program_handle, "VIEW");
	if (view >= 0)
	{
		glUniformMatrix4fv(view, 1, GL_FALSE, (GLfloat*)&Camera::GetCurrentCamera()->m_view);
	}

	GLint model = glGetUniformLocation(program_handle, "MODEL");
	if (model >= 0)
	{
		Matrix44 identity;
		glUniformMatrix4fv(model, 1, GL_FALSE, (GLfloat*)&identity);
	}

	GLint totalTime = glGetUniformLocation(program_handle, "TOTAL_TIME");
	if (totalTime >= 0)
	{
		glUniform1f(totalTime,static_cast<float>(Clock::g_theMasterClock->frame.m_seconds));
	}

	GLint pos_bind   = glGetAttribLocation(program_handle, "POSITION");
	GLint uv_bind    = glGetAttribLocation(program_handle, "UV");
	GLint color_bind = glGetAttribLocation(program_handle, "COLOR");

	GLuint textureIndex = 0U;
	glBindBuffer(GL_ARRAY_BUFFER, temp_render_buffer->GetHandle());

	glBindSampler(textureIndex, Sampler::GetCurrentSampler()->GetHandle());
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, Texture::GetCurrentTexture()->m_textureID);

	if (color_bind >= 0)
	{
		glEnableVertexAttribArray(color_bind);
		glVertexAttribPointer(color_bind, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex_3DPCU), (GLvoid*)offsetof(Vertex_3DPCU, m_rgba));
	}

	if (uv_bind >= 0)
	{
		glEnableVertexAttribArray(uv_bind);
		glVertexAttribPointer(uv_bind, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_3DPCU), (GLvoid*)offsetof(Vertex_3DPCU, m_uvs));
	}

	if (pos_bind >= 0)
	{
		glEnableVertexAttribArray(pos_bind);

		glVertexAttribPointer(pos_bind, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_3DPCU), (GLvoid*)offsetof(Vertex_3DPCU, m_position));
	}
	glBindFramebuffer(GL_FRAMEBUFFER, Camera::GetCurrentCamera()->GetFrameBufferHandle());
	glDrawElements(g_openGlPrimitiveTypes[drawPrimitive],icount,GL_UNSIGNED_INT,0);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/26
*@purpose : Draws the raw mesh
*@param   : Mesh pointer and model matrix
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::DrawMesh(Mesh *mesh,Matrix44 model)
{
	GL_CHECK_ERROR();
	BindShader(Shader::GetCurrentShader());
	GL_CHECK_ERROR();
	BindMeshToProgram(Shader::GetCurrentShader()->GetShaderProgram(), mesh);
	GL_CHECK_ERROR();

	BindRendererUniforms(model);
	GL_CHECK_ERROR();

	BindRenderState(Shader::GetCurrentShader()->m_state);
	GL_CHECK_ERROR();

	// Now that it is described and bound, draw using our program
	glBindFramebuffer(GL_FRAMEBUFFER, Camera::GetCurrentCamera()->GetFrameBufferHandle());
	GL_CHECK_ERROR();

	if(mesh->m_drawInstruction.m_usingIndices)
	{
		glDrawElements(g_openGlPrimitiveTypes[mesh->m_drawInstruction.m_primitiveType], mesh->m_drawInstruction.m_elemCount, GL_UNSIGNED_INT, 0);
	}	
	else
	{
		glDrawArrays(g_openGlPrimitiveTypes[mesh->m_drawInstruction.m_primitiveType], 0, mesh->m_drawInstruction.m_elemCount);
	}
	GL_CHECK_ERROR();

}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/03
*@purpose : Binds the current mesh to program
*@param   : Shader program , Mesh pointer
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindMeshToProgram(ShaderProgram *program, Mesh *mesh)
{
	GL_CHECK_ERROR();
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo.GetHandle());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo.GetHandle());
	GL_CHECK_ERROR();

	int vertex_stride = mesh->GetVertexStride();
	// but now, lets describe the mesh to the program, generally, 
	// so first, how many things do we need to know about?  There are
	// multiple, so this means we'll need a container of some sorts (m_attributes),
	// and this container contains a description of each member
	// (vertex_attribute_t)
	GLuint programHandle  = program->GetHandle();
	int    attributeCount =  mesh->m_layout->GetAttributeCount();

	for (int attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex) 
	{
		VertexAttribute attribute = mesh->m_layout->GetAttribute(attributeIndex);

		// a program needs a name;
		int bind = glGetAttribLocation(programHandle, attribute.m_name.c_str());

		// this attribute exists in this shader, cool, bind it
		if (bind >= 0)
		{
			glEnableVertexAttribArray(bind);
			// be sure mesh and program are bound at this point
			// as this links them together
			//glVertexAttribPointer(uv_bind, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_3DPCU), (GLvoid*)offsetof(Vertex_3DPCU, m_uvs));
			// 
			glVertexAttribPointer(bind,attribute.m_elementCount,(attribute.m_GLType),attribute.m_isNormalized,vertex_stride,(GLvoid*)attribute.m_memberOffset);  

		}
		GL_CHECK_ERROR();

	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/03
*@purpose : Binds uniforms to shader program
*@param   : Mesh pointer
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindRendererUniforms(Matrix44 model)
{
	GLuint program_handle = Shader::GetCurrentShader()->GetShaderProgram()->GetHandle();
	GLint projection = glGetUniformLocation(program_handle, "PROJECTION");
	if (projection >= 0)
	{
		glUniformMatrix4fv(projection, 1, GL_FALSE, (GLfloat*)&Camera::GetCurrentCamera()->m_projection);
	}

	GLint view = glGetUniformLocation(program_handle, "VIEW");
	if (view >= 0)
	{
		glUniformMatrix4fv(view, 1, GL_FALSE, (GLfloat*)&Camera::GetCurrentCamera()->m_view);
	}

	GLint modeluniform = glGetUniformLocation(program_handle, "MODEL");
	if (modeluniform >= 0)
	{
		glUniformMatrix4fv(modeluniform, 1, GL_FALSE, (GLfloat*)&model);
	}

	GLint totalTime = glGetUniformLocation(program_handle, "TIME");
	if (totalTime >= 0)
	{
		glUniform1f(totalTime, static_cast<float>(Clock::g_theMasterClock->total.m_seconds));
	}

	GLint camPos = glGetUniformLocation(program_handle, "CAMERA_POSITION");
	if (camPos >= 0)
	{
		Vector3 position = Camera::GetCurrentCamera()->m_transform.GetWorldPosition();
		glUniform3fv(camPos,1, (GLfloat*)&(position));
	}
	/*FogFactor fog;
	fog.nearPlane = Camera::GetCurrentCamera()->m_model.GetKVector().z + Camera::GetCurrentCamera()->GetCameraForwardVector().z*30;
	fog.farPlane  = fog.nearPlane + 30;

	if(m_FogUBuffer == nullptr)
	{
		m_FogUBuffer = new UniformBuffer();
	}
	BindUniformBuffer(7, m_FogUBuffer, sizeof(FogFactor), &fog);*/

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : Binds render stats with current renderer
*@param   : Render state
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindRenderState(RenderState state)
{
	SetCompleteBlending(state.m_color_blend_op, state.m_color_src_factor, state.m_color_dst_factor, state.m_alpha_blend_op, state.m_alpha_src_factor, state.m_alpha_dst_factor);

	EnableDepth(state.m_depth_compare, state.m_depth_write);

	SetFillMode(state.m_fill_mode);

	SetCullMode(state.m_cull_mode);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : Binds the sampler
*@param   : texture unit index,sampler 
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindSampler(GLuint Id,Sampler *sampler)
{
	glBindSampler(Id, sampler->GetHandle());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/04
*@purpose : Binds shader program current renderer
*@param   : Shader pointer
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindShader(Shader *shader)
{
	GLuint program_handle = shader->GetShaderProgram()->GetHandle();
	glUseProgram(program_handle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : Binds material with current draw call
*@param   : Material object
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindMaterial(Material *material)
{
	BindShader(material->GetShader());
	GL_CHECK_ERROR();

	BindMaterialProperty(material);
	GL_CHECK_ERROR();

	Shader::SetCurrentShader(material->GetShader());
	GL_CHECK_ERROR();

	//m_currentShaderProgram = m_currentShader->GetShaderProgram();
	//material->BindAll(m_currentShader->m_shaderProgram->GetHandle());
	for(int index = 0;index < material->m_textures.size();index++)
	{
		Texture *texture = material->m_textures.at(index);
		BindSampler(texture->m_slot, material->m_samplers.at(index));
		GL_CHECK_ERROR();

		BindTexture(texture->m_slot, material->m_textures.at(index));
		GL_CHECK_ERROR();

	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/06
*@purpose : Bind all material properties with shader
*@param   : Material pointer
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindMaterialProperty(Material *material)
{
	std::map<std::string, MaterialProperty*>::iterator it = material->m_properties.begin();
	for(;it != material->m_properties.end();it++)
	{
		it->second->Bind(material->m_shader->GetShaderProgram()->program_handle);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/10
*@purpose : Binds cube map
*@param   : texture cubemap, bind index
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::BindCubeMapTexture(TextureCube* cubeMap, int bindIndex)
{
	BindSampler(bindIndex, Sampler::GetDefaultSampler());
	glActiveTexture(GL_TEXTURE0 + bindIndex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->get_handle());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/09
*@purpose : Sets the current shader
*@param   : Shader
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::SetShader(Shader* tshader)
{
	Shader::SetCurrentShader(tshader);
	glUseProgram(Shader::GetCurrentShader()->GetShaderProgram()->GetHandle());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/23
*@purpose : Sets the sampler as current sampler
*@param   : Sampler object pointer
*@return  : NIL
*/////////////////////////////////////////////////////////////
void Renderer::UseSampler(Sampler *sampler)
{
	Sampler::SetCurrentSampler(sampler);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/21
*@purpose : Apply full screen affects
*@param   : Shader program
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::ApplyEffect(ShaderProgram *shaderProgram)
{
	UNUSED(shaderProgram);
	if(Texture::GetEffectCurrentTexture() == nullptr)
	{
		Texture::SetEffectCurrentTexture(Texture::GetDefaultColorTargetTexture());
		if(Texture::GetEffectScratchTexture() == nullptr)
		{
			// some buffer same size		
			Texture * effectCurretTexture = Texture::GetEffectCurrentTexture();
			Texture::SetEffectScratchTexture(CreateRenderTarget(effectCurretTexture->m_dimensions.x, effectCurretTexture->m_dimensions.y, TEXTURE_FORMAT_RGBA8));
		}
	}
	Camera::GetEffectCamera()->SetColorTarget(Texture::GetEffectScratchTexture());
	Camera::SetCurrentCamera(Camera::GetEffectCamera());

	TODO("Pass shader instead of shader program to apply effect");
	//UseShaderProgram(shaderProgram);
	Texture::SetCurrentTexture(Texture::GetEffectCurrentTexture());
	Sampler::SetCurrentSampler(Sampler::GetEffectSampler());
	AABB2 drawAABB(Vector2(-1, -1), Vector2(1, 1));
	DrawTexturedAABB(drawAABB, Texture::GetCurrentTexture(), Vector2(0, 0), Vector2(1, 1), Rgba::WHITE);
	//std::swap(Texture::GetEffectCurrentTexture(), Texture::GetEffectScratchTexture());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/21
*@purpose : Finish the current applied effect
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::FinishEffects()
{
	if(Texture::GetEffectCurrentTexture() == nullptr)
	{
		return;
	}
	if (Texture::GetEffectCurrentTexture() != Texture::GetDefaultColorTargetTexture())
	{
		CopyTexture2D(Texture::GetEffectCurrentTexture(), Texture::GetDefaultColorTargetTexture());
		//m_defaultColorTarget = m_effectCurrentTarget;
		Texture::SetEffectCurrentTexture(Texture::GetEffectCurrentTexture());
	}
	Texture::SetEffectCurrentTexture(nullptr);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/23
*@purpose : Copy texture
*@param   : From and to texture pointer
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Renderer::CopyTexture2D(Texture *from, Texture *to)
{
	m_effectFrameBuffer->m_color_target = from;
	m_copyFrameBuffer->m_color_target = to;

	CopyFrameBuffer(m_copyFrameBuffer, m_effectFrameBuffer);
}

Texture* Renderer::CreateRenderTarget(int width, int height,eTextureFormat fmt)
{
	Texture *texture = new Texture();
	texture->CreateRenderTarget(width, height, fmt);
	return texture;
}

//------------------------------------------------------------------------
Texture* Renderer::CreateDepthStencilTarget(int width, int height)
{
	return CreateRenderTarget(width, height, TEXTURE_FORMAT_D24S8);
}

bool Renderer::CopyFrameBuffer(FrameBuffer *dst, FrameBuffer *src)
{
	// we need at least the src.
	if (src == nullptr) {
		return false;
	}

	// Get the handles - NULL refers to the "default" or back buffer FBO
	GLuint src_fbo = src->GetHandle();
	GLuint dst_fbo = NULL;
	if (dst != nullptr) {
		dst_fbo = dst->GetHandle();
	}

	// can't copy onto ourselves
	if (dst_fbo == src_fbo) {
		return false;
	}

	// the GL_READ_FRAMEBUFFER is where we copy from
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src_fbo);

	// what are we copying to?
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_fbo);

	// blit it over - get teh size
	// (we'll assume dst matches for now - but to be safe,
	// you should get dst_width and dst_height using either
	// dst or the window depending if dst was nullptr or not
	int width  = src->GetDimensions().x;
	int height = src->GetDimensions().y;

	// Copy it over
	glBlitFramebuffer(0, 0, // src start pixel
		width, height,        // src size
		0, 0,                 // dst start pixel
		width, height,        // dst size
		GL_COLOR_BUFFER_BIT,  // what are we copying (just colour)
		GL_NEAREST);         // resize filtering rule (in case src/dst don't match)

							  // Make sure it succeeded
	GLCheckError(__FILE__,__LINE__);

	// cleanup after ourselves
	glBindFramebuffer(GL_READ_FRAMEBUFFER, NULL);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);
	TODO("GLSucceeded() Yet to make");
	return true;//GLSucceeded();
}


void Renderer::TakeScreenShotAndSave()
{
	int width = Camera::GetCurrentCamera()->m_defaultFrameBuffer->GetDimensions().x;
	int height = Camera::GetCurrentCamera()->m_defaultFrameBuffer->GetDimensions().y;

	std::vector<GLubyte> pixels;
	pixels.reserve(width * height * 4);
	glReadBuffer(GL_FRONT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	stbi_flip_vertically_on_write(true);
	
	time_t rawtime;
	char buffer[80];
	time(&rawtime);

	struct tm info;
	localtime_s(&info,&rawtime);
	strftime(buffer,50,"%Y%m%d_%H%M%S", &info);

	std::string filename = "ScreenShots//ScreenShots";
	filename.append(buffer);// +".png");
	filename.append(".png");

	std::wstring folder( L"ScreenShots");
	
	if (CreateDirectory(folder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), 0);
		stbi_write_png("ScreenShots//screenshot.png", width, height, 4, pixels.data(), 0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Renderer::TakeScreenShotAndSave(std::string directoryLocation)
{
	int width = Camera::GetCurrentCamera()->m_defaultFrameBuffer->GetDimensions().x;
	int height = Camera::GetCurrentCamera()->m_defaultFrameBuffer->GetDimensions().y;

	std::vector<GLubyte> pixels;
	pixels.reserve(width * height * 4);
	glReadBuffer(GL_FRONT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	stbi_flip_vertically_on_write(true);

	time_t rawtime;
	char buffer[80];
	time(&rawtime);

	struct tm info;
	localtime_s(&info, &rawtime);
	strftime(buffer, 50, "%Y %m %d  %H'%M'%S", &info);

	std::string filename = directoryLocation;
	filename.append(buffer);// +".png");
	filename.append(".png");

	std::wstring folder(L"ScreenShots");
	stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), 0);	
}

std::string Renderer::GetBlendOperationAsString(eBlendOperation operation)
{
	switch (operation)
	{
	case COMPARE_ADD:
		return "compare_add";
		break;
	case COMPARE_SUBTRACT:
		return "compare_subtract";
		break;
	case COMPARE_REVERSE_SUBTRACT:
		return "compare_reverse_subtract";
		break;
	case COMPARE_MIN:
		return "compare_min";
		break;
	case COMPARE_MAX:
		return "compare_max";
		break;
	default:
		break;
	}
	return "";
}

eBlendOperation Renderer::GetBlendOperationFromString(std::string blendOp)
{
	if(blendOp == "compare_add")
	{
		return COMPARE_ADD;
	}
	if(blendOp == "compare_subtract")
	{
		return COMPARE_SUBTRACT;
	}
	if(blendOp == "compare_reverse_subtract")
	{
		return COMPARE_REVERSE_SUBTRACT;
	}
	if(blendOp == "compare_min")
	{
		return COMPARE_MIN;
	}
	if(blendOp == "compare_max")
	{
		return COMPARE_MAX;
	}
	return COMPARE_ADD;
}

std::string Renderer::GetBlendFactorAsString(eBlendFactor factor)
{
	switch (factor)
	{
	case BLEND_ONE:
		return "blend_one";
		break;
	case BLEND_ZERO:
		return "blend_zero";
		break;
	case BLEND_SRC_ALPHA:
		return "blend_src_alpha";
		break;
	case BLEND_INV_SRC_ALPHA:
		return "blend_inv_src_alpha";
		break;
	default:
		break;
	}
	return "";
}

eBlendFactor Renderer::GetBlendFactorFromString(std::string blendFactor)
{
	if(blendFactor == "blend_one")
	{
		return BLEND_ZERO;
	}
	if(blendFactor == "blend_zero")
	{
		return BLEND_ZERO;
	}
	if(blendFactor == "blend_src_alpha")
	{
		return BLEND_SRC_ALPHA;
	}
	if(blendFactor == "blend_inv_src_alpha")
	{
		return BLEND_INV_SRC_ALPHA;
	}
	return BLEND_ZERO;
}

std::string Renderer::GetCullModeAsString(eCullMode cullMode)
{
	switch (cullMode)
	{
	case CULLMODE_BACK:
		return "cullmode_back";
		break;
	case CULLMODE_FRONT:
		return "cullmode_front";
		break;
	case CULLMODE_NONE:
		return "cullmode_none";
		break;
	default:
		break;
	}
	return "";
}

eCullMode Renderer::GetCullModeFromString(std::string cullMode)
{
	if (cullMode == "cullmode_back")
	{
		return CULLMODE_BACK;
	}
	if (cullMode == "cullmode_front")
	{
		return CULLMODE_FRONT;
	}
	if(cullMode == "cullmode_none")
	{
		return CULLMODE_NONE;
	}
	return CULLMODE_BACK;
}

std::string Renderer::GetFillModeAsString(eFillMode fillMode)
{
	switch (fillMode)
	{
	case FILLMODE_SOLID:
		return "fillmode_solid";
		break;
	case FILLMODE_WIRE:
		return "fillmode_wire";
		break;
	default:
		break;
	}
	return "";
}

eFillMode Renderer::GetFillModeFromString(std::string fillMode)
{
	if(fillMode == "fillmode_solid")
	{
		return FILLMODE_SOLID;
	}
	if(fillMode == "fillmode_wire")
	{
		return FILLMODE_WIRE;
	}
	return FILLMODE_SOLID;
}

std::string Renderer::GetWindOrderAsString(eWindOrder windOrder)
{
	switch (windOrder)
	{
	case WIND_CLOCKWISE:
		return "wind_clockwise";
		break;
	case WIND_COUNTER_CLOCKWISE:
		return "wind_counter_clockwise";
		break;
	default:
		break;
	}
	return "";
}

eWindOrder Renderer::GetWindOrderFromString(std::string windOrder)
{
	if(windOrder == "wind_clockwise")
	{
		return WIND_CLOCKWISE;
	}
	if(windOrder == "wind_counter_clockwise")
	{
		return WIND_COUNTER_CLOCKWISE;
	}
	return WIND_COUNTER_CLOCKWISE;
}

std::string Renderer::GetDepthCompareAsString(eCompare depthCompare)
{
	switch (depthCompare)
	{
	case COMPARE_NEVER:
		return "compare_never";
		break;
	case COMPARE_LESS:
		return "compare_less";
		break;
	case COMPARE_LEQUAL:
		return "compare_lequal";
		break;
	case COMPARE_GREATER:
		return "compare_greater";
		break;
	case COMPARE_GEQUAL:
		return "compare_gequal";
		break;
	case COMPARE_EQUAL:
		return "compare_equal";
		break;
	case COMPARE_NOT_EQUAL:
		return "compare_not_equal";
		break;
	case COMPARE_ALWAYS:
		return "compare_always";
		break;
	default:
		break;
	}
	return "";
}

eCompare Renderer::GetDepthCompareFromString(std::string deptCompare)
{
	if(deptCompare == "compare_never")
	{
		return COMPARE_NEVER;
	}
	if(deptCompare =="compare_less")
	{
		return COMPARE_LESS;
	}
	if(deptCompare =="compare_lequal")
	{
		return COMPARE_LEQUAL;
	}
	if(deptCompare =="compare_greater")
	{
		return COMPARE_GREATER;
	}
	if(deptCompare =="compare_gequal")
	{
		return COMPARE_GEQUAL;
	}
	if(deptCompare =="compare_equal")
	{
		return COMPARE_EQUAL;
	}
	if (deptCompare == "compare_not_equal")
	{
		return COMPARE_NOT_EQUAL;
	}
	if(deptCompare =="compare_always")
	{
		return COMPARE_ALWAYS;
	}
	return COMPARE_ALWAYS;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
GLenum Renderer::ToGLCubeSide(eTextureCubeSide textureCubeSide)
{
	switch (textureCubeSide)
	{
	case TEXCUBE_FRONT:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		break;
	case TEXCUBE_BACK:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		break;
	case TEXCUBE_TOP:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		break;
	case TEXCUBE_BOTTOM:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		break;
	case TEXCUBE_LEFT:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		break;
	case TEXCUBE_RIGHT:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		break;
	default:
		break;
	}
	return NULL;
}

