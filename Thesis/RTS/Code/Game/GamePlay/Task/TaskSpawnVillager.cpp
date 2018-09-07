#include "Game/GamePlay/Task//TaskSpawnVillager.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
TaskSpawnVillager::TaskSpawnVillager(Map* map,TownCenter *townCenter)
{
	m_entity	 = townCenter;
	m_map		 = map;
}

// DESTRUCTOR
TaskSpawnVillager::~TaskSpawnVillager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Do task spawn villager
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskSpawnVillager::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	int tileIndex = m_map->GetTileIndex(m_entity->GetPosition());
	IntVector2 tileCords = m_map->GetCordinates(tileIndex);

	IntVector2 tileCordsE  = IntVector2(tileCords.x + 1, tileCords.y + 0);
	IntVector2 tileCordsN  = IntVector2(tileCords.x + 0, tileCords.y + 1);
	IntVector2 tileCordsW  = IntVector2(tileCords.x - 1, tileCords.y + 0);
	IntVector2 tileCordsS  = IntVector2(tileCords.x + 0, tileCords.y - 1);

	IntVector2 tileCordsNE = IntVector2(tileCords.x + 1, tileCords.y + 1);
	IntVector2 tileCordsNW = IntVector2(tileCords.x - 1, tileCords.y + 1);
	IntVector2 tileCordsSW = IntVector2(tileCords.x - 1, tileCords.y - 1);
	IntVector2 tileCordsSE = IntVector2(tileCords.x + 1, tileCords.y - 1);


	
	if(m_map->IsValidCordinate(tileCordsE))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsE))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsE),m_entity->m_teamID);
			return true;
		}
	}
	if (m_map->IsValidCordinate(tileCordsN))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsN))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsN), m_entity->m_teamID);
			return true;
		}
	}
	if (m_map->IsValidCordinate(tileCordsW))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsW))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsW), m_entity->m_teamID);
			return true;
		}
	}
	if (m_map->IsValidCordinate(tileCordsS))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsS))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsS), m_entity->m_teamID);
			return true;
		}
	}
	if (m_map->IsValidCordinate(tileCordsNE))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsNE))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsNE), m_entity->m_teamID);
			return true;
		}
	}
	if (m_map->IsValidCordinate(tileCordsNW))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsNW))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsNW), m_entity->m_teamID);
			return true;
		}
	}
	if (m_map->IsValidCordinate(tileCordsSW))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsSW))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsSW), m_entity->m_teamID);
			return true;
		}
	}
	if (m_map->IsValidCordinate(tileCordsSE))
	{
		if (!m_map->HasAnyEntityInTile(tileCordsSE))
		{
			m_map->CreateCivilian(m_map->GetMapPosition(tileCordsSE), m_entity->m_teamID);
			return true;
		}
	}
	DebugDraw::GetInstance()->DebugRenderLogf(1, "CANNOT SPAWN VILLAGER NO SPACE NEARBY ");
	return true;
}
