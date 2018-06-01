#pragma once

class Entity
{
public:
	
	Entity();
	Entity(float x,float y);
	void Update(float deltaTime);
	void Render();
};