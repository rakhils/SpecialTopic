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
	InitSampleNN();
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitSampleNN()
{
	NeuralNetwork neuralNet(2, 2, 2);
	neuralNet.m_inputs->m_neurons.at(0).m_value = 0.05;
	neuralNet.m_inputs->m_neurons.at(1).m_value = 0.1;
	neuralNet.m_inputs->m_bias.m_value = 1;

	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(0) = 0.15;
	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(1) = 0.25;

	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(0) = 0.20;
	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(1) = 0.30;

	neuralNet.m_inputs->m_bias.m_weights.at(0) = 0.35;
	neuralNet.m_inputs->m_bias.m_weights.at(1) = 0.35;

	///////////////////////////////////////////////////////////////

	/*neuralNet.m_hiddenLayers->m_neurons.at(0).m_value = 0.40;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_value = 0.1;*/
	neuralNet.m_hiddenLayers->m_bias.m_value = 1;

	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(0) = 0.40;
	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(1) = 0.50;

	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(0) = 0.45;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(1) = 0.55;

	neuralNet.m_hiddenLayers->m_bias.m_weights.at(0) = 0.60;
	neuralNet.m_hiddenLayers->m_bias.m_weights.at(1) = 0.60;

	neuralNet.FeedForwardNN();
	
	std::vector<float> m_targetOuputs;
	m_targetOuputs.push_back(.01);
	m_targetOuputs.push_back(.99);
	neuralNet.BackPropogateOuputToHiddenLayer(m_targetOuputs);
	neuralNet.BackPropogateHiddenToInputLayer(m_targetOuputs);


}
