#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Core/Windows.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Math/MathUtil.hpp"

Map::Map()
{
	Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Init the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::Initialize()
{
	InitCamera();
	m_xOffset   = 0.f;
	m_yOffset   = 2 * g_radius;
	m_maxWidth  = g_mapMaxWidth;
	m_maxHeight = g_mapMaxHeight;

	CreateTownCenter(Vector2(20, 340),1);
	CreateTownCenter(Vector2(800, 600), 2);
	CreateCivilian(Vector2(850, 600), 2);
	CreateCivilian(Vector2::ONE * 500, 1);
/*
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);
	CreateCivilian(Vector2::ONE * 500, 1);*/

	CreateResources(Vector2(200,300),  RESOURCE_FOOD);
	CreateResources(Vector2(200, 500), RESOURCE_WOOD);
	CreateResources(Vector2(300, 500), RESOURCE_STONE);


	CreateResources(Vector2(900, 800),  RESOURCE_FOOD);
	CreateResources(Vector2(900, 500),  RESOURCE_WOOD);
	CreateResources(Vector2(1100, 500), RESOURCE_STONE);
	//CreateClassAWarrior(Vector2(500, 300), 1);
	CreateArmySpawner(Vector2(500, 300), 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Inits camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_camera   = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Create a civilian at position and with team id
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateCivilian(Vector2 position, int teamID)
{
	Civilian *civilian = new Civilian(this,position,teamID);
	m_civilians.push_back(civilian);
	m_movableEntities.push_back(civilian);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateArmySpawner(Vector2 position, int teamID)
{
	ArmySpawner * armySpawner = new ArmySpawner(this,position,teamID);
	m_armySpawners.push_back(armySpawner);
	m_standAloneEntities.push_back(armySpawner);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a class Warrior index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateClassAWarrior(Vector2 position, int teamID)
{
	ClassAWarrior *classAWarrior = new ClassAWarrior(this,position,teamID);
	m_classAWarriors.push_back(classAWarrior);
	m_movableEntities.push_back(classAWarrior);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates classB Warriors
*@param   : Position and teamID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateClassBWarrior(Vector2 position, int teamID)
{
	ClassBWarrior *classBWarrior = new ClassBWarrior(this,position,teamID);
	m_classBWarriors.push_back(classBWarrior);
	m_movableEntities.push_back(classBWarrior);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a House
*@param   : Position and teamID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateHouse(Vector2 position, int teamID)
{
	House *house = new House(this,position,teamID);
	m_houses.push_back(house);
	m_standAloneEntities.push_back(house);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates town center
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateTownCenter(Vector2 position, int teamID)
{
	TownCenter *townCenter = new TownCenter(this,position,teamID);
	m_townCenters.push_back(townCenter);
	m_standAloneEntities.push_back(townCenter);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a resource item in map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateResources(Vector2 position,EntityType type)
{
	Resource *resource = new Resource(this,position,type);
	m_resources.push_back(resource);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Creates explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateExplosions(Vector2 position)
{
	Explosion *explosion = new Explosion(position);
	m_explosions.push_back(explosion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Cheks if a position is inside map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsPositionInsideMap(Vector2 position)
{
	IntVector2 cords = GetCordinates(position);
	if(cords.x == -1 || cords.y ==-1)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Check if 2 position are neighbours
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsNeighbours(IntVector2 position1, IntVector2 position2)
{
	return IsNeighbours(position1, position2, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsNeighbours(IntVector2 position1, IntVector2 position2, int distance)
{
	IntVector2 displacement = position1 - position2;
	int neighbourDistance = distance;
	if (position1 == position2)
	{
		return false;
	}
	if (displacement.x <= neighbourDistance && displacement.x >= -neighbourDistance && displacement.y <= neighbourDistance && displacement.y >= -neighbourDistance)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Gets a free neighbour tile
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetFreeNeighbourTile(Vector2 position)
{
	return GetFreeNeighbourTile(position, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetFreeNeighbourTile(Vector2 position, int distance)
{
	int tileIndex = GetTileIndex(position);
	IntVector2 tileCords = GetCordinates(tileIndex);

	IntVector2 tileCordsE = IntVector2(tileCords.x + distance, tileCords.y + 0);
	IntVector2 tileCordsN = IntVector2(tileCords.x + 0, tileCords.y + distance);
	IntVector2 tileCordsW = IntVector2(tileCords.x - distance, tileCords.y + 0);
	IntVector2 tileCordsS = IntVector2(tileCords.x + 0, tileCords.y - distance);

	IntVector2 tileCordsNE = IntVector2(tileCords.x + distance, tileCords.y + distance);
	IntVector2 tileCordsNW = IntVector2(tileCords.x - distance, tileCords.y + distance);
	IntVector2 tileCordsSW = IntVector2(tileCords.x - distance, tileCords.y - distance);
	IntVector2 tileCordsSE = IntVector2(tileCords.x + distance, tileCords.y - distance);

	if (IsValidCordinate(tileCordsE))
	{
		if (!HasAnyEntityInTile(tileCordsE))
		{
			return tileCordsE;
		}
	}
	if (IsValidCordinate(tileCordsN))
	{
		if (!HasAnyEntityInTile(tileCordsN))
		{
			return tileCordsN;
		}
	}
	if (IsValidCordinate(tileCordsW))
	{
		if (!HasAnyEntityInTile(tileCordsW))
		{
			return tileCordsW;
		}
	}
	if (IsValidCordinate(tileCordsS))
	{
		if (!HasAnyEntityInTile(tileCordsS))
		{
			return tileCordsS;
		}
	}
	if (IsValidCordinate(tileCordsNE))
	{
		if (!HasAnyEntityInTile(tileCordsNE))
		{
			return tileCordsNE;
		}
	}
	if (IsValidCordinate(tileCordsNW))
	{
		if (!HasAnyEntityInTile(tileCordsNW))
		{
			return tileCordsNW;
		}
	}
	if (IsValidCordinate(tileCordsSW))
	{
		if (!HasAnyEntityInTile(tileCordsSW))
		{
			return tileCordsSW;
		}
	}
	if (IsValidCordinate(tileCordsSE))
	{
		if (!HasAnyEntityInTile(tileCordsSE))
		{
			return tileCordsSE;
		}
	}
	return IntVector2(-1, -1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Retrieves tile position from map position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetTilePosition(Vector2 position)
{
	int x = static_cast<int>(position.x / g_radius);
	int y = static_cast<int>(position.y / g_radius);
	return IntVector2(x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetTilePosition(int tilePosition)
{
	UNUSED(tilePosition);
	return IntVector2::ONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetTileIndex(Vector2 mapPosition)
{
	//mapPosition.y = Windows::GetInstance()->GetDimensions().y - mapPosition.y;
	//DebugDraw::GetInstance()->DebugRenderLogf("MousePOs %f ", mapPosition.y);
	int x = static_cast<int>((mapPosition.x - m_xOffset) / g_unitDistance);
	int y = static_cast<int>((mapPosition.y - m_yOffset) / g_unitDistance);
	if(x > m_maxWidth || y > m_maxHeight || x < 0 || y < 0)
	{
		return -1;
	}
	return GetTileIndex(IntVector2(x, y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetTileIndex(IntVector2 position)
{
	return (position.x) + (position.y) * m_maxWidth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Check for all entities in a grid
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasAnyEntityInTile(int index)
{
	for(int armySpawnerIndex = 0;armySpawnerIndex < m_armySpawners.size();armySpawnerIndex++)
	{
		Vector2 position = m_armySpawners.at(armySpawnerIndex)->GetPosition();
		int armySpawnerTileIndex = GetTileIndex(position);
		if(armySpawnerTileIndex == index)
		{
			return true;
		}
	}
	for (int civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
	{
		Vector2 position = m_civilians.at(civilianIndex)->GetPosition();
		int civilianTileIndex = GetTileIndex(position);
		if (civilianTileIndex == index)
		{
			return true;
		}
	}
	for (int classAWarriorIndex = 0; classAWarriorIndex < m_classAWarriors.size(); classAWarriorIndex++)
	{
		Vector2 position = m_classAWarriors.at(classAWarriorIndex)->GetPosition();
		int classAWarriorPositionIndex = GetTileIndex(position);
		if (classAWarriorPositionIndex == index)
		{
			return true;
		}
	}
	for (int classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		Vector2 position = m_classBWarriors.at(classBWarriorIndex)->GetPosition();
		int classBWarriorPositionIndex = GetTileIndex(position);
		if (classBWarriorPositionIndex == index)
		{
			return true;
		}
	}
	for (int houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		Vector2 position = m_houses.at(houseIndex)->GetPosition();
		int housePositionIndex = GetTileIndex(position);
		if (housePositionIndex == index)
		{
			return true;
		}
	}
	for (int resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		Vector2 position = m_resources.at(resourceIndex)->GetPosition();
		int resourceTileIndex = GetTileIndex(position);
		if (resourceTileIndex == index)
		{
			return true;
		}
	}
	for (int towncenterIndex = 0; towncenterIndex < m_townCenters.size(); towncenterIndex++)
	{
		Vector2 position = m_townCenters.at(towncenterIndex)->GetPosition();
		int townCenterPositionIndex = GetTileIndex(position);
		if (townCenterPositionIndex == index)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Check for all entities in a grid
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasAnyEntityInTile(IntVector2 Cords)
{
	int index = GetTileIndex(Cords);
	if(index == -1)
	{
		return false;
	}
	return HasAnyEntityInTile(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Check for all entities in a grid
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasAnyEntityInTile(Vector2 mapPosition)
{
	int index = GetTileIndex(mapPosition);
	if(index == -1)
	{
		return false;
	}
	return HasAnyEntityInTile(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::GetEntityFromPosition(int index)
{
	if(index < 0 && index > m_maxHeight*m_maxWidth)
	{
		return nullptr;
	}

	for (int armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
	{
		Vector2 position = m_armySpawners.at(armySpawnerIndex)->GetPosition();
		int armySpawnerTileIndex = GetTileIndex(position);
		if (armySpawnerTileIndex == index)
		{
			return m_armySpawners.at(armySpawnerIndex);
		}
	}
	for (int civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
	{
		Vector2 position = m_civilians.at(civilianIndex)->GetPosition();
		int civilianTileIndex = GetTileIndex(position);
		if (civilianTileIndex == index)
		{
			return m_civilians.at(civilianIndex);
		}
	}
	for (int classAWarriorIndex = 0; classAWarriorIndex < m_classAWarriors.size(); classAWarriorIndex++)
	{
		Vector2 position = m_classAWarriors.at(classAWarriorIndex)->GetPosition();
		int classAWarriorPositionIndex = GetTileIndex(position);
		if (classAWarriorPositionIndex == index)
		{
			return m_classAWarriors.at(classAWarriorIndex);
		}
	}
	for (int classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		Vector2 position = m_classBWarriors.at(classBWarriorIndex)->GetPosition();
		int classBWarriorPositionIndex = GetTileIndex(position);
		if (classBWarriorPositionIndex == index)
		{
			return m_classBWarriors.at(classBWarriorIndex);
		}
	}
	for (int houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		Vector2 position = m_houses.at(houseIndex)->GetPosition();
		int housePositionIndex = GetTileIndex(position);
		if (housePositionIndex == index)
		{
			return m_houses.at(houseIndex);
		}
	}
	for (int resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		Vector2 position = m_resources.at(resourceIndex)->GetPosition();
		int resourceTileIndex = GetTileIndex(position);
		if (resourceTileIndex == index)
		{
			return m_resources.at(resourceIndex);
		}
	}
	for (int towncenterIndex = 0; towncenterIndex < m_townCenters.size(); towncenterIndex++)
	{
		Vector2 position = m_townCenters.at(towncenterIndex)->GetPosition();
		int townCenterPositionIndex = GetTileIndex(position);
		if (townCenterPositionIndex == index)
		{
			return m_townCenters.at(towncenterIndex);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::GetEntityFromPosition(IntVector2 cords)
{
	int tileIndex = GetTileIndex(cords);
	return GetEntityFromPosition(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::GetEntityFromPosition(Vector2 position)
{
	int tileIndex = GetTileIndex(position);
	return GetEntityFromPosition(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Is a valid map cordinate
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsValidCordinate(IntVector2 cords)
{
	if(cords.x >= 0 && cords.x < m_maxWidth && cords.y >=0 && cords.y < m_maxHeight)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Attacks on tile
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::AttackOnPosition(int tileIndex,float damagePoint)
{
	Entity *entity = GetEntityFromPosition(tileIndex);
	entity->TakeDamage(damagePoint);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Attacks on cords
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::AttackOnPosition(IntVector2 cords, float damagePoint)
{
	int tileIndex = GetTileIndex(cords);
	return AttackOnPosition(tileIndex,damagePoint);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Attacks on position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::AttackOnPosition(Vector2 position, float damagePoint)
{
	int tileIndex = GetTileIndex(position);
	return AttackOnPosition(tileIndex, damagePoint);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : Check if 2 entities are enemies
*@param   : 2 entities
*@return  : true if teamId not matched
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsEnemies(Entity *entityOne, Entity *entityTwo)
{
	if(entityOne->m_teamID != entityTwo->m_teamID)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Destroys entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::DestroyEntity(Entity *entity)
{
	switch (entity->m_type)
	{
	case CIVILIAN:
		for (int civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
		{
			if (m_civilians.at(civilianIndex) == entity)
			{
				delete m_civilians.at(civilianIndex);
				m_civilians.erase(m_civilians.begin() + civilianIndex, m_civilians.begin() + civilianIndex + 1);
				return;
			}
		}
		break;
	case WARRIOR_SHORT_RANGE:
		for (int warriorShortRangeIndex = 0; warriorShortRangeIndex < m_classAWarriors.size(); warriorShortRangeIndex++)
		{
			if (m_classAWarriors.at(warriorShortRangeIndex) == entity)
			{
				delete m_classAWarriors.at(warriorShortRangeIndex);
				m_classAWarriors.erase(m_classAWarriors.begin() + warriorShortRangeIndex, m_classAWarriors.begin() + warriorShortRangeIndex + 1);
				return;
			}
		}
		break;
	case WARRIOR_LONG_RANGE:
		for (int warriorLongRangeIndex = 0; warriorLongRangeIndex < m_classBWarriors.size(); warriorLongRangeIndex++)
		{
			if (m_classBWarriors.at(warriorLongRangeIndex) == entity)
			{
				delete m_classBWarriors.at(warriorLongRangeIndex);
				m_classBWarriors.erase(m_classBWarriors.begin() + warriorLongRangeIndex, m_classBWarriors.begin() + warriorLongRangeIndex + 1);
				return;
			}
		}
		break;
	case HOUSE:
		for (int houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
		{
			if (m_houses.at(houseIndex) == entity)
			{
				delete m_houses.at(houseIndex);
				m_houses.erase(m_houses.begin() + houseIndex, m_houses.begin() + houseIndex + 1);
				return;
			}
		}
		break;
	case TOWN_CENTER:
		for (int townCenterIndex = 0; townCenterIndex < m_houses.size(); townCenterIndex++)
		{
			if (m_houses.at(townCenterIndex) == entity)
			{
				delete m_houses.at(townCenterIndex);
				m_houses.erase(m_houses.begin() + townCenterIndex, m_houses.begin() + townCenterIndex + 1);
				return;
			}
		}
		break;
	case RESOURCE_FOOD:
		for (int resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
		{
			if (m_resources.at(resourceIndex) == entity)
			{
				delete m_resources.at(resourceIndex);
				m_resources.erase(m_resources.begin() + resourceIndex, m_resources.begin() + resourceIndex + 1);
				return;
			}
		}
		break;
	case ARMY_SPAWNER:
		for (int armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
		{
			if (m_armySpawners.at(armySpawnerIndex) == entity)
			{
				delete m_armySpawners.at(armySpawnerIndex);
				m_armySpawners.erase(m_armySpawners.begin() + armySpawnerIndex, m_armySpawners.begin() + armySpawnerIndex + 1);
				return;
			}
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Changes to map position from tile index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Map::GetMapPosition(int tileIndex)
{
	float x = static_cast<float>(tileIndex % m_maxWidth);
	float y = static_cast<float>(tileIndex / m_maxWidth);
	
	return Vector2(x * g_unitDistance + m_xOffset + g_radius, y * g_unitDistance + m_yOffset + g_radius);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Map position from cordinate position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Map::GetMapPosition(IntVector2 tilecords)
{
	if(!IsValidCordinate(tilecords))
	{
		return Vector2::ONE*-1;
	}
	int tileIndex = GetTileIndex(tilecords);
	return GetMapPosition(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetCordinates(int tileIndex)
{
	int x = tileIndex % m_maxWidth;
	int y = tileIndex / m_maxWidth;
	return IntVector2(x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Gets position as cords
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetCordinates(Vector2 mapPosition)
{
	int tileIndex = GetTileIndex(mapPosition);
	return GetCordinates(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
	m_currentTileIndex = GetTileIndex(mousePosition);

	if(InputSystem::GetInstance()->WasRButtonJustPressed())
	{
		g_currentSelectedEntity = nullptr;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_1))
	{
		m_townCenters.at(0)->m_resourceStat.m_food++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_2))
	{
		m_townCenters.at(0)->m_resourceStat.m_stone++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_3))
	{
		m_townCenters.at(0)->m_resourceStat.m_wood++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_4))
	{
		m_townCenters.at(1)->m_resourceStat.m_food++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_5))
	{
		m_townCenters.at(1)->m_resourceStat.m_stone++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_6))
	{
		m_townCenters.at(1)->m_resourceStat.m_wood++;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndUpdateOnWinCondition()
{
	for(size_t townCenterIndex = 0;townCenterIndex < m_townCenters.size();townCenterIndex++)
	{
		if(m_townCenters.at(townCenterIndex)->m_health <= 0)
		{
			m_gameFinished = true;
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : If two entities overlap make the distant
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndClearEntityOverlap()
{
	for (size_t entityOneIndex = 0; entityOneIndex < m_movableEntities.size(); entityOneIndex++)
	{
		for (size_t entityTwoIndex = entityOneIndex + 1; entityTwoIndex < m_movableEntities.size(); entityTwoIndex++)
		{
			if(m_movableEntities.at(entityOneIndex)->m_taskQueue.size() == 0 && m_movableEntities.at(entityTwoIndex)->m_taskQueue.size() == 0)
			{
				if (m_movableEntities.at(entityOneIndex)->GetTileIndex() == m_movableEntities.at(entityTwoIndex)->GetTileIndex())
				{
					IntVector2 freeCords = GetFreeNeighbourTile(m_movableEntities.at(entityOneIndex)->GetPosition());

					if (freeCords == IntVector2(-1, -1))
					{
						freeCords = GetCordinates(m_movableEntities.at(entityOneIndex)->GetPosition());
						freeCords = freeCords + IntVector2(GetRandomIntInRange(-1,2), GetRandomIntInRange(-1,2));
					}
					m_movableEntities.at(entityOneIndex)->SetPosition(GetMapPosition(freeCords));
				}
			}
		}
	}
}

void Map::Update(float deltaTime)
{
	CheckAndUpdateOnWinCondition();
	CheckAndClearEntityOverlap();
	if(m_gameFinished)
	{
		return;
	}
	ProcessInputs(deltaTime);

	UpdateCamera(deltaTime);
	UpdateResources(deltaTime);
	UpdateCivilans(deltaTime);
	UpdateArmySpawners(deltaTime);
	UpdateClassAWarriors(deltaTime);
	UpdateClassBWarriors(deltaTime);
	UpdateHouses(deltaTime);
	UpdateTownCenters(deltaTime);
	UpdateExplosions(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Updates camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera(float deltaTime)
{
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCivilans(float deltaTime)
{
	for (size_t civiliansIndex = 0; civiliansIndex < m_civilians.size(); civiliansIndex++)
	{
		m_civilians.at(civiliansIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateArmySpawners(float deltaTime)
{
	for (size_t armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
	{
		m_armySpawners.at(armySpawnerIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates classA Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateClassAWarriors(float deltaTime)
{
	for (size_t classAWarriorIndex = 0; classAWarriorIndex < m_classAWarriors.size(); classAWarriorIndex++)
	{
		m_classAWarriors.at(classAWarriorIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates classB Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateClassBWarriors(float deltaTime)
{
	for (size_t classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		m_classBWarriors.at(classBWarriorIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates all houses
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateHouses(float deltaTime)
{
	for (size_t houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		m_houses.at(houseIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates all town centers
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateTownCenters(float deltaTime)
{
	for (size_t townCenterIndex = 0; townCenterIndex < m_townCenters.size(); townCenterIndex++)
	{
		m_townCenters.at(townCenterIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates resources
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateResources(float deltaTime)
{
	for (size_t resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		m_resources.at(resourceIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Updates explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateExplosions(float deltaTime)
{
	for (int explosionIndex = 0; explosionIndex < m_explosions.size(); explosionIndex++)
	{
		Explosion *explosion = m_explosions.at(explosionIndex);
		explosion->Update(deltaTime);
		if(explosion->m_completed)
		{
			delete explosion;
			m_explosions.erase(m_explosions.begin() + explosionIndex, m_explosions.begin() + explosionIndex + 1);
			explosionIndex--;
		}
	}
}

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();

	RenderCivilians();
	RenderClassAWarriors();
	RenderClassBWarriors();
	RenderArmySpawners();
	RenderHouses();
	RenderTownCenters();
	RenderResources();
	RenderGrids();
	RenderExplosions();
	RenderHUDGameStat();
	RenderHUDUnitStat();

	RenderMousePosition();

	if (m_gameFinished)
	{
		RenderWinState();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render Civilians
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderCivilians()
{
	for (size_t civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
	{
		m_civilians.at(civilianIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderArmySpawners()
{
	for (size_t armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
	{
		m_armySpawners.at(armySpawnerIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Renders classA Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderClassAWarriors()
{
	for (size_t classAWarriorsIndex = 0; classAWarriorsIndex < m_classAWarriors.size(); classAWarriorsIndex++)
	{
		m_classAWarriors.at(classAWarriorsIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Renders classB Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderClassBWarriors()
{
	for (size_t classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		m_classBWarriors.at(classBWarriorIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render houses
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderHouses()
{
	for (size_t houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		m_houses.at(houseIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Renders towncenters
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderTownCenters()
{
	for (size_t townCenterIndex = 0; townCenterIndex < m_townCenters.size(); townCenterIndex++)
	{
		m_townCenters.at(townCenterIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render resources
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderResources()
{
	for (size_t resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		m_resources.at(resourceIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/22
*@purpose : render grids
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderGrids()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for(float x = 0; x < (static_cast<float>(m_maxWidth) + 1)*g_unitDistance ;x += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(x, 0.f, 0.f), Vector3(x, (static_cast<float>(m_maxHeight) + 2)*g_unitDistance, 0));
	}
	for (float y = 0; y < (static_cast<float>(m_maxHeight) + 2)* g_unitDistance; y += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(0.f, y, 0.f), Vector3((m_maxWidth + 1.f)* g_unitDistance,y,0.f));
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderExplosions()
{
	for(int explosionIndex = 0;explosionIndex < m_explosions.size();explosionIndex++)
	{
		m_explosions.at(explosionIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderHUDGameStat()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawAABB(g_resourceStatHUD, Rgba::BLUE);

	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	TownCenter *populationATownCenter = m_townCenters.at(0);
	TownCenter *populationBTownCenter = nullptr;
	
	if(m_townCenters.size() > 1)
	{
		populationBTownCenter = m_townCenters.at(1);
	}

	g_theRenderer->DrawTextOn3DPoint(g_resourceFoodPositionPopulationA, Vector3::RIGHT, Vector3::UP , "FOOD  : " + ToString(populationATownCenter->m_resourceStat.m_food),  g_fontSize, Rgba::GREEN);																																   
	g_theRenderer->DrawTextOn3DPoint(g_resourceStonePositionPopulationA, Vector3::RIGHT, Vector3::UP, "STONE : " + ToString(populationATownCenter->m_resourceStat.m_stone), g_fontSize, Rgba::GREEN);																																   
	g_theRenderer->DrawTextOn3DPoint(g_resourceWoodPositionPopulationA, Vector3::RIGHT, Vector3::UP , "WOOD  : " + ToString(populationATownCenter->m_resourceStat.m_wood),  g_fontSize, Rgba::GREEN);

	g_theRenderer->DrawTextOn3DPoint(g_unitsCountPositionPopulationA, Vector3::RIGHT, Vector3::UP ,       "UNITS         : " + ToString(populationATownCenter->m_resourceStat.m_units),  g_fontSize, Rgba::GREEN);

	g_theRenderer->DrawTextOn3DPoint(g_unitsKilledCountPositionPopulationA, Vector3::RIGHT, Vector3::UP , "UNITS KILLED  : " + ToString(populationATownCenter->m_resourceStat.m_unitsKilled),  g_fontSize, Rgba::GREEN);

	g_theRenderer->DrawTextOn3DPoint(g_buildingCountPositionPopulationA, Vector3::RIGHT, Vector3::UP ,    "BUILDINGS     : " + ToString(populationATownCenter->m_resourceStat.m_buildings),  g_fontSize, Rgba::GREEN);

	if(populationBTownCenter != nullptr)
	{
		g_theRenderer->DrawTextOn3DPoint(g_resourceFoodPositionPopulationB, Vector3::RIGHT, Vector3::UP, "FOOD  : " + ToString(populationBTownCenter->m_resourceStat.m_food), g_fontSize, Rgba::RED);
		g_theRenderer->DrawTextOn3DPoint(g_resourceStonePositionPopulationB, Vector3::RIGHT, Vector3::UP, "STONE : " + ToString(populationBTownCenter->m_resourceStat.m_stone), g_fontSize, Rgba::RED);
		g_theRenderer->DrawTextOn3DPoint(g_resourceWoodPositionPopulationB, Vector3::RIGHT, Vector3::UP, "WOOD  : " + ToString(populationBTownCenter->m_resourceStat.m_wood), g_fontSize, Rgba::RED);
		g_theRenderer->DrawTextOn3DPoint(g_unitsCountPositionPopulationB, Vector3::RIGHT, Vector3::UP,       "UNITS         : " + ToString(populationBTownCenter->m_resourceStat.m_units), g_fontSize, Rgba::RED);

		g_theRenderer->DrawTextOn3DPoint(g_unitsKilledCountPositionPopulationB, Vector3::RIGHT, Vector3::UP, "UNITS KILLED  : " + ToString(populationBTownCenter->m_resourceStat.m_unitsKilled), g_fontSize, Rgba::RED);

		g_theRenderer->DrawTextOn3DPoint(g_buildingCountPositionPopulationB, Vector3::RIGHT, Vector3::UP,    "BUILDINGS     : " + ToString(populationBTownCenter->m_resourceStat.m_buildings), g_fontSize, Rgba::RED);

	}

	delete textMaterial;
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Render HUD
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderHUDUnitStat()
{
	if(g_currentSelectedEntity == nullptr)
	{
		return;
	}
	Material *defaultMaterial = Material::AquireResource("default");
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawAABB(g_unitStatHUD, Rgba::BLUE);

	
	switch (g_currentSelectedEntity->m_type)
	{
	case TOWN_CENTER:
		g_theRenderer->DrawAABB(g_unitStatHUDFirstButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDFirstButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize/2.f,0), Vector3::RIGHT, Vector3::UP, "CREATE CIVILIAN ",g_fontSize, Rgba::YELLOW);


	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDHealthInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "HEALTH : " + ToString(g_currentSelectedEntity->m_health), g_fontSize, Rgba::YELLOW);
		break;
	case CIVILIAN:
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDHealthInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "HEALTH : " + ToString(g_currentSelectedEntity->m_health), g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDFirstButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDFirstButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "BUILD ARMY UNIT ", g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDSecondButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDSecondButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "BUILD HOUSE ", g_fontSize, Rgba::YELLOW);
		break;
	case HOUSE:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		break;
	case ARMY_SPAWNER:
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDHealthInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "HEALTH : "+ToString(g_currentSelectedEntity->m_health), g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDFirstButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDFirstButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "CREATE SHORT ARMY ", g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDSecondButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDSecondButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "CREATE LONG ARMY ", g_fontSize, Rgba::YELLOW);
		
		break;
	default:
		break;
	}

	delete textMaterial;
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Renders the current mouse position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderMousePosition()
{
	Vector2 mapPosition = GetMapPosition(m_currentTileIndex);
	//DebugDraw::GetInstance()->DebugRenderLogf("CurrentTile %d ", m_currentTileIndex);
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawAABB(AABB2(mapPosition,g_radius,g_radius), Rgba::FADED_BLUE);
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : Renders win condition
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderWinState()
{
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	if(m_townCenters.at(0)->m_health <= 0)
	{
		g_theRenderer->DrawTextOn3DPoint(Windows::GetInstance()->GetDimensions().GetAsVector2()/2.f, Vector3::RIGHT, Vector3::UP, "TEAM 2 WINS ", g_fontSize, Rgba::YELLOW);
		return;
	}
	g_theRenderer->DrawTextOn3DPoint(Windows::GetInstance()->GetDimensions().GetAsVector2()/2.f, Vector3::RIGHT, Vector3::UP, "TEAM 1 WINS ", g_fontSize, Rgba::YELLOW);
}

