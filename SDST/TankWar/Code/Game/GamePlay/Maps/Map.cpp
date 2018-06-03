#include "Map.hpp"

#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Debug/DebugDraw.hpp"

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
		
		if(isRepeatForUV || isFirstLoop)
		{
			meshBuilder.SetUV(Vector2(0, 0));
		}
		else
		{
			meshBuilder.SetUV(Vector2(1, 0));
		}
		meshBuilder.SetColor(Rgba::WHITE);
		meshBuilder.SetNormal(Vector3::UP);
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
		meshBuilder.SetNormal(Vector3::UP);
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
		meshBuilder.SetNormal(Vector3::UP);
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
		meshBuilder.SetNormal(Vector3::UP);
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
/*DATE    : 2018/06/02
*@purpose : Calculates height
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::GetHeight(Vector2 position)
{
	DebugDraw::GetInstance()->DebugRenderLogf("GET HEIGHT POS X = %f ,Y = %f ", position.x, position.y);
	size_t index = static_cast<size_t>((static_cast<int>(position.y) * m_extents.GetDimensions().x + static_cast<int>(position.x)));
	DebugDraw::GetInstance()->DebugRenderLogf("GET HEIGHT INDEX = %f", index);

	if(index >= 0 && index < m_image->texture->pixels.size())
	{
		Rgba *color1 = m_image->texture->pixels.at(index);
		Vector4 values = color1->GetAsFloats();
		DebugDraw::GetInstance()->DebugRenderLogf("GET HEIGHT VALUE AVG = %f", values.GetAverageValue());
		DebugDraw::GetInstance()->DebugRenderLogf("GET HEIGHT HEIGTH RANGMAP = %f", RangeMapFloat(values.GetAverageValue(), 0, 1, m_min_height, m_max_height));

		return RangeMapFloat(values.GetAverageValue(), 0, 1, m_min_height, m_max_height);
	}
	return 0.f;
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


