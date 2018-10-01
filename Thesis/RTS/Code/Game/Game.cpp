#include "Game.hpp"
#include "Engine/Net/Socket.hpp"


Game *Game::s_game = nullptr;
int Game::s_gameCounter = 0;
Game::Game()
{
}

void Game::Render()
{
	m_map->Render();
}

void Game::Update(float deltaTime)
{		
	int updateCount = 1;
	if(g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
	{
		isQuitTriggered = true;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_T))
	{
		g_enableNeuralNet = g_enableNeuralNet ? false : true;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_A))
	{
		updateCount = 20000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_S))
	{
		updateCount = 40000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_D))
	{
		updateCount = 60000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_F))
	{
		updateCount = 80000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_G))
	{
		updateCount = 100000;
	}
	if(m_map == nullptr)
	{
		m_map = new Map();
	}
	for(int index = 0;index < updateCount;index++)
	{
		s_gameCounter++;
		m_map->Update(deltaTime);
	}
	updateCount = 1;
}

Game* Game::GetInstance()
{
	if (s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}