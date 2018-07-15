#include "Game.hpp"
#include "Engine/Input/InputSystem.hpp"
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
	
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_T))
	{
		if(m_numberOfIterations == 1)
		{
			m_numberOfIterations = 50000;
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_Y))
	{
		if (m_numberOfIterations == 1)
		{
			m_numberOfIterations = 40000;
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_U))
	{
		if (m_numberOfIterations == 1)
		{
			m_numberOfIterations = 30000;
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_I))
	{
		if (m_numberOfIterations == 1)
		{
			m_numberOfIterations = 20000;
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_O))
	{
		if (m_numberOfIterations == 1)
		{
			m_numberOfIterations = 10000;
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_P))
	{
		if (m_numberOfIterations == 1)
		{
			m_numberOfIterations = 2500;
		}
	}
		
	for (int index = 0; index < m_numberOfIterations; index++)
	{
		m_map->Update(deltaTime);
	}
	m_numberOfIterations = 1;
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
