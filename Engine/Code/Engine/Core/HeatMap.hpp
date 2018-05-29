#pragma once
#include <vector>

#include "Engine/Math/IntVector2.hpp"

class HeatMap
{
public:
	std::vector<float> m_heatPerGridCell;
	IntVector2 m_dimensions;

	HeatMap();

	void  SetHeat(int cellCoords,float heatValue);
	void  AddHeat(int cellCoords,float heatValue);
	float GetHeat(int cellCoords);
	void  PopulateWithMaxValue();
};