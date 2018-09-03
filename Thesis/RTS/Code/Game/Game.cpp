#include "Game.hpp"
Game *Game::s_game = nullptr;
Game::Game()
{
	
}

void Game::Render()
{
	m_map->Render();
}

void Game::Update(float deltaTime)
{		
	if(g_theInput->wasKeyJustPressed(27))
	{
		isQuitTriggered = true;
	}
	if(m_map == nullptr)
	{
		m_map = new Map();
	}
	m_map->Update(deltaTime);
}

Game* Game::GetInstance()
{
	if (s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}