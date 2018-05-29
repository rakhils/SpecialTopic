#include "Engine/Core/HeatMap.hpp"

HeatMap::HeatMap()
{

}

void HeatMap::SetHeat(int cellCoords,float value)
{
	if(cellCoords > m_dimensions.x * m_dimensions.y || cellCoords < 0)
	{
		return;
	}
	m_heatPerGridCell.at(cellCoords) = value;
}

void HeatMap::AddHeat(int cellCoords,float value)
{
	float heatValue = GetHeat(cellCoords);
	SetHeat(cellCoords,heatValue + value);
}

float HeatMap::GetHeat(int cellCoords)
{
	if(cellCoords < 0 || cellCoords > m_dimensions.x*m_dimensions.y)
	{
		return 99999.f;
	}
	return m_heatPerGridCell.at(cellCoords);
}

//////////////////////////////////////////////////////////////////////////
//@purpose : Populating all elements in matrix with max value = 9999(infinity)
void HeatMap::PopulateWithMaxValue()
{
	for(int index = 0;index < m_dimensions.x*m_dimensions.y;index++)
	{
		m_heatPerGridCell.push_back(9999.0f);// MAX VALUE
	}
}

