#include "Map.hpp"

#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Math/MathUtil.hpp"

#include "Game/GameCommon.hpp"
Map::Map()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Loads map from image
*@param   : Image object, AABB2 for dimension, Min and max heights and chunkcount
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::LoadFromImage(Image image, AABB2 const &extents, float min_height, float max_height, Vector2 chunk_counts)
{
	m_extents    = extents;
	m_min_height = min_height;
	m_max_height = max_height;
	MeshBuilder meshBuilder;
	meshBuilder.Begin(PRIMITIVE_TRIANGES,true);
	int minsx = 0;// static_cast<int>(extents.GetDimensions().x) * -1;
	int maxsx = static_cast<int>(extents.GetDimensions().x);

	int minsz = 0;// static_cast<int>(extents.GetDimensions().y) * -1;
	int maxsz = static_cast<int>(extents.GetDimensions().y);
	bool isRepeatForUV = false;
	bool isFirstLoop   = true;
	int  totalCount    = 0;
	for (int pixelIndex = 0; (pixelIndex + 4) < image.texture->pixels.size(); pixelIndex += 4)
	{
		totalCount+=4;
		Rgba *color1 = image.texture->pixels.at(pixelIndex);
		Vector4 values1 = color1->GetAsFloats();
		int x1 = static_cast<int>(pixelIndex % maxsx);
		int z1 = static_cast<int>(pixelIndex / maxsz);
		Vector3 position1(static_cast<float>(x1), RangeMapFloat(values1.GetAverageValue(),0,1,min_height,max_height), static_cast<float>(z1));

		Rgba *color2 = image.texture->pixels.at(pixelIndex + 1);
		Vector4 values2 = color2->GetAsFloats();
		int x2 = static_cast<int>((pixelIndex + 1) % maxsx);
		int z2 = static_cast<int>((pixelIndex + 1) / maxsz);
		Vector3 position2(static_cast<float>(x2), RangeMapFloat(values2.GetAverageValue(),0,1,min_height,max_height), static_cast<float>(z2));

		Rgba *color3 = image.texture->pixels.at(pixelIndex + 2);
		Vector4 values3 = color3->GetAsFloats();
		int x3 = static_cast<int>((pixelIndex + 2) % maxsx);
		int z3 = static_cast<int>((pixelIndex + 2) / maxsz);
		Vector3 position3(static_cast<float>(x3), RangeMapFloat(values3.GetAverageValue(),0,1,min_height,max_height), static_cast<float>(z3));

		Rgba *color4 = image.texture->pixels.at(pixelIndex + 3);
		Vector4 values4 = color4->GetAsFloats();
		int x4 = static_cast<int>((pixelIndex + 3) % maxsx);
		int z4 = static_cast<int>((pixelIndex + 3) / maxsz);
		Vector3 position4(static_cast<float>(x4), RangeMapFloat(values4.GetAverageValue(),0,1,min_height,max_height), static_cast<float>(z4));
		
		Vector3 normal;
		if(pixelIndex + maxsx < image.texture->pixels.size())
		{
			Rgba *color5 = image.texture->pixels.at(pixelIndex + maxsx);
			Vector4 values5 = color5->GetAsFloats();
			int x5 = static_cast<int>((pixelIndex + maxsx) % maxsx);
			int z5 = static_cast<int>((pixelIndex + maxsx) / maxsz);
			Vector3 position5(static_cast<float>(x5), RangeMapFloat(values5.GetAverageValue(), 0, 1, min_height, max_height), static_cast<float>(z5));

			Vector3 u = position2 - position1;
			Vector3 v = position5 - position4;

			normal = CrossProduct(v.GetNormalized(), u.GetNormalized());
		}

		if(isRepeatForUV || isFirstLoop)
		{
			meshBuilder.SetUV(Vector2(0, 0));
		}
		else
		{
			meshBuilder.SetUV(Vector2(1, 0));
		}
		meshBuilder.SetColor(Rgba::WHITE);
		meshBuilder.SetNormal(normal);
		meshBuilder.PushVertex(position1);

		if (isRepeatForUV || isFirstLoop)
		{
			meshBuilder.SetUV(Vector2(0, 1));
		}
		else
		{
			meshBuilder.SetUV(Vector2(1, 1));
		}
		meshBuilder.SetColor(Rgba::WHITE);
		meshBuilder.SetNormal(normal);
		meshBuilder.PushVertex(position2);

		if (isRepeatForUV || isFirstLoop)
		{
			meshBuilder.SetUV(Vector2(0, 0));
		}
		else
		{
			meshBuilder.SetUV(Vector2(1, 0));
		}
		meshBuilder.SetColor(Rgba::WHITE);
		meshBuilder.SetNormal(normal);
		meshBuilder.PushVertex(position3);

		if (isRepeatForUV || isFirstLoop)
		{
			meshBuilder.SetUV(Vector2(0, 1));
		}
		else
		{
			meshBuilder.SetUV(Vector2(1, 1));
		}
		meshBuilder.SetColor(Rgba::WHITE);
		meshBuilder.SetNormal(normal);
		meshBuilder.PushVertex(position4);
		if(pixelIndex % maxsx == 0 && pixelIndex !=0)
		{
			if (isRepeatForUV == false)
			{
				if (isFirstLoop)
				{
					isFirstLoop = false;
					continue;
				}
				isRepeatForUV = true;
				pixelIndex -= maxsx;
			}
			else
			{
				isRepeatForUV = false;
			}
		}
	}
	maxsz = totalCount / maxsx;
	for (int z = minsz; z <= (maxsz - 1); z++)
	{
		for (int x = minsx; x < (maxsx - 1); x++)
		{
			int ll = maxsx * z + x;
			int lr = ll + 1;

			int ul = ll + maxsx;
			int ur = ul + 1;
			meshBuilder.AddQuadIndex(ll,lr,ul,ur);
		}
	}
	meshBuilder.End();
	m_terrain = meshBuilder.CreateMesh<Vertex_3DPCUN>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::GetHeigthAtIndex(int index)
{
	if (index >= 0 && index < m_image->texture->pixels.size())
	{
		Rgba *color1 = m_image->texture->pixels.at(index);
		Vector4 values = color1->GetAsFloats();
		return RangeMapFloat(values.GetAverageValue(), 0, 1, m_min_height, m_max_height);
	}
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : Calculates height
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::GetHeight(Vector2 position)
{
	// dx LERP in X with Fractx
	// dy LERP in Y with Fracty
	// LERP in dx fracty 
	// Uv = RangeMap(pos,extents,lowwerleft,extens toprigth,vec0 vec1);
	// veccUV = UV.grid.
	// cellfract = fract(celluv)
	// bl_index  = floor(cell_uv)
	//DebugDraw::GetInstance()->DebugRenderLogf("GET HEIGHT POS X = %f ,Y = %f ", position.x, position.y);
	//DebugDraw::GetInstance()->DebugRenderLogf("GET HEIGHT INDEX = %f", index);
	DebugDraw::GetInstance()->DebugRenderLogf("POSITION %f , %f", position.x, position.y);
	int index1 = static_cast<int>((static_cast<int>(position.y) * m_extents.GetDimensions().x + static_cast<int>(position.x)));
	int index2 = static_cast<int>((static_cast<int>(position.y) * m_extents.GetDimensions().x + static_cast<int>(position.x + 1)));
	int index3 = static_cast<int>((static_cast<int>(position.y + 1) * m_extents.GetDimensions().x + static_cast<int>(position.x)));

	float height1 = GetHeigthAtIndex(static_cast<int>(index1));
	float heightx1 = GetHeigthAtIndex(static_cast<int>(index2));
	float heighty1 = GetHeigthAtIndex(static_cast<int>(index3));

	float dx = Interpolate(height1, heightx1, GetFraction(position.x));
	float dy = Interpolate(height1, heighty1, GetFraction(position.y));
	float height = Interpolate(dx, dy, GetFraction(position.y));

	DebugDraw::GetInstance()->DebugRenderLogf("INDEX1 = %d INDEX2 = %d INDEX3 %d ", (index1),(index2),(index3));
	DebugDraw::GetInstance()->DebugRenderLogf("HEIGHT1 = %f HEIGHT2 = %f HEIGHT3 %f ", height1,heightx1,heighty1);
	DebugDraw::GetInstance()->DebugRenderLogf("DX = %f DY = %f HEIGHT = %f FRACTIOn %f", dx,dy,height,GetFraction(position.x));

	return height;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::AddToMapObjects(GameObject *gameObject)
{
	m_mapObjects.push_back(gameObject);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::RemoveFromMapObject(GameObject *gameObject)
{
	for(int index = 0;index < m_mapObjects.size();index++)
	{
		if(m_mapObjects.at(index) == gameObject)
		{
			m_mapObjects.erase(m_mapObjects.begin() + index);
			index--;
		}
	}
}

void Map::Update(float deltaTime)
{
	UNUSED(deltaTime);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/22
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::Render()
{
	Vector2 basePosition(50,50);
	DebugRenderOptions options;
	debugDraw->DebugRenderText2D(Vector3(0, 110, 0), Vector3(0, 0, 0), "MINI MAP", Vector2(0, 0), 5, Rgba::GREEN, options);
	debugDraw->DebugRenderLine2D(Vector3(0, 0, 0),   Vector3(0, 100, 0),Rgba::WHITE,options);
	debugDraw->DebugRenderLine2D(Vector3(0, 0, 0),   Vector3(100,0, 0), Rgba::WHITE, options);
	debugDraw->DebugRenderLine2D(Vector3(0, 100, 0), Vector3(100, 100, 0), Rgba::WHITE, options);
	debugDraw->DebugRenderLine2D(Vector3(100, 0, 0), Vector3(100, 100, 0), Rgba::WHITE, options);
	for (int mapObjIndex = 0; mapObjIndex < m_mapObjects.size(); mapObjIndex++)
	{
		Vector3 gameObjPosition = m_mapObjects.at(mapObjIndex)->m_transform.GetLocalPosition();
		if(m_mapObjects.at(mapObjIndex)->m_name == "player")
		{
			gameObjPosition = Camera::GetCurrentCamera()->m_transform.GetLocalPosition();
		}
		float xPos = RangeMapFloat(gameObjPosition.x, -100, 100, -50, 50);
		float yPos = RangeMapFloat(gameObjPosition.z, -100, 100, -50, 50);
		xPos = ClampFloat(xPos, -50, 50) + basePosition.x;
		yPos = ClampFloat(yPos, -50, 50) + basePosition.y;
		Rgba color;
		//Not sure of excat way to do mini map. Dont want to disturb engine code with games minimap
		if(m_mapObjects.at(mapObjIndex)->m_name == "asteroid")
		{
			color = Rgba::RED;
		}
		if(m_mapObjects.at(mapObjIndex)->m_name == "enemy")
		{
			color = Rgba::BLUE;
		}
		if(m_mapObjects.at(mapObjIndex)->m_name == "player")
		{
			color = Rgba::GREEN;
		}
		debugDraw->DebugRenderCross(Vector3(xPos, yPos, 0), 2, color, options);

	}
}


