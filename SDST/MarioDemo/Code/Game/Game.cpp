#include "Game.hpp"
Game *Game::s_game = nullptr;
Game::Game()
{
	TileDefinition::LoadDefintions("Data\\Definitions\\Tiles.xml");
	MapDefinition::LoadDefintions("Data\\Definitions\\MapDefinitions.xml");
	EntityDefinition::LoadDefinition("Data//Definitions//Actors.xml");
	GenerateMap("Level1","island");
}
void Game::Update(float deltaTime)
{	
	for(int index = 0;index < m_numberOfIterations;index++)
	{
		m_map->Update(deltaTime);
	}
}

void Game::Render()
{
	m_map->Render();
}

void Game::GenerateMap(std::string name, std::string map_name)
{
	m_map = new Map(MapDefinition::getDefinition("level1"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Game* Game::GetInstance()
{
	if(s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}
