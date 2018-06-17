#include "Map.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Physics/Terrain.hpp"

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
void Map::LoadFromImage(Scene*scene,Image *image, AABB2 const &extents, float min_height, float max_height, Vector2 chunk_counts)
{
	m_terrain = new Terrain();
	m_terrain->LoadFromImage(image, extents, min_height, max_height, chunk_counts);
	for (size_t renderableIndex = 0; renderableIndex < m_terrain->m_renderables.size(); renderableIndex++)
	{
		scene->AddRenderable(m_terrain->m_renderables.at(renderableIndex));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::GetHeigthAtIndex(int index)
{
	return m_terrain->GetHeightAtDiscreteCordinate(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : Calculates height
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::GetHeight(Vector2 position)
{
	return m_terrain->GetHeight(position);
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


