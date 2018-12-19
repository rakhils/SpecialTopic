#include "Game.hpp"
#include "Engine/Input/InputSystem.hpp"
Game *Game::s_game = nullptr;
Game::Game()
{
	TileDefinition::LoadDefintions("Data\\Definitions\\Tiles.xml");
	MapDefinition::LoadDefintions("Data\\Definitions\\MapDefinitions.xml");
	EntityDefinition::LoadDefinition("Data\\Definitions\\Actors.xml");
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

Game* Game::GetInstance()
{
	if(s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}
