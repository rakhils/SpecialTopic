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
	//InitSampleNN();
	int updateCount = 1;
	if(g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
	{
		isQuitTriggered = true;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_T))
	{
		g_enableNeuralNet = g_enableNeuralNet ? false : true;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_Q))
	{
		updateCount = 2500;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_W))
	{
		updateCount = 5000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_E))
	{
		updateCount = 7500;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_R))
	{
		updateCount = 10000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_A))
	{
		updateCount = 20000;
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
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_H))
	{
		updateCount = 1000000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_J))
	{
		updateCount = 10000000;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_K))
	{
		updateCount = 100000000;
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
	neuralNet.m_inputs->m_neurons.at(0).m_value = 0.05f;
	neuralNet.m_inputs->m_neurons.at(1).m_value = 0.1f;
	neuralNet.m_inputs->m_bias.m_value = 1;

	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(0) = 0.15f;
	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(1) = 0.25f;

	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(0) = 0.20f;
	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(1) = 0.30f;

	neuralNet.m_inputs->m_bias.m_weights.at(0) = 0.35f;
	neuralNet.m_inputs->m_bias.m_weights.at(1) = 0.35f;

	///////////////////////////////////////////////////////////////

	/*neuralNet.m_hiddenLayers->m_neurons.at(0).m_value = 0.40;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_value = 0.1;*/
	neuralNet.m_hiddenLayers->m_bias.m_value = 1;

	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(0) = 0.40f;
	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(1) = 0.50f;

	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(0) = 0.45f;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(1) = 0.55f;

	neuralNet.m_hiddenLayers->m_bias.m_weights.at(0) = 0.60f;
	neuralNet.m_hiddenLayers->m_bias.m_weights.at(1) = 0.60f;

	neuralNet.FeedForwardNN();
	
	std::vector<float> inputs;
	inputs.push_back(.05f);
	inputs.push_back(.1f);

	std::vector<float> m_targetOuputs;
	m_targetOuputs.push_back(.5f);
	m_targetOuputs.push_back(.5f);
	for(int index = 0;index < 2000;index ++)
	{
		inputs.at(0) = GetRandomFloatZeroToOne();
		inputs.at(1) = GetRandomFloatZeroToOne();
		neuralNet.FeedForward(inputs);
		neuralNet.BackPropogateOuputToHiddenLayer(m_targetOuputs);
		neuralNet.BackPropogateHiddenToInputLayer(m_targetOuputs);
	}
	neuralNet.FeedForward(inputs);
	neuralNet;
}
