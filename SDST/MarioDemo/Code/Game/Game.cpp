#include "Game.hpp"

Game::Game()
{
	TileDefinition::LoadDefintions("Data\\Definitions\\Tiles.xml");
	MapDefinition::LoadDefintions("Data\\Definitions\\MapDefinitions.xml");
	EntityDefinition::LoadDefinition("Data//Definitions//Actors.xml");
	GenerateMap("Level1","island");
}
void Game::Update(float deltaTime)
{		
	m_map->Update(deltaTime);
}

void Game::Render()
{
	m_map->Render();
}


void Game::GenerateMap(std::string name, std::string map_name)
{
	m_map = new Map(MapDefinition::getDefinition("level1"));
}
