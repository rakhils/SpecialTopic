#pragma once
#include "Game.hpp"

class App
{
public:

	float MAX_DELTA_VALUE = 0.01616f;
	bool isQuitTriggered = false;
	Game *g_theGame;

	App::App();
	App::~App();

	void RunFrame();
	void Update(float deltaTime);
	void Render();
	bool IsReadyToQuit();
};
