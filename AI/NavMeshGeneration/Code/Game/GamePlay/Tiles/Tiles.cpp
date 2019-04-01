#include "Tiles.hpp"

Tiles::Tiles()
{
}

void Tiles::Render()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tiles::SetNodes(ControlNode *bottomLeft, ControlNode *bottomRight, ControlNode *topLeft, ControlNode *topRight)
{
	m_bottomLeft  = bottomLeft;
	m_bottomRight = bottomRight;
	m_topLeft     = topLeft;
	m_topRight	  = topRight;

	m_centreLeft   = bottomLeft->m_above;
	m_centreRight  = bottomRight->m_above;
	m_centreBottom = bottomLeft->m_right;
	m_centreTop    = topLeft->m_right;

	if(topLeft->m_active)
	{
		m_configurationNumber += 8;
	}
	if (topRight->m_active)
	{
		m_configurationNumber += 4;
	}
	if (bottomRight->m_active)
	{
		m_configurationNumber += 2;
	}
	if (bottomLeft->m_active)
	{
		m_configurationNumber += 1;
	}
}

Vector2 Tiles::GetTopLeft()
{
	return m_aabb2.GetCenter() + Vector2(-g_unitDistance / 2.f, g_unitDistance / 2.f)+ Vector2(g_unitDistance/2,g_unitDistance/2);
}

Vector2 Tiles::GetTopRight()
{
	return m_aabb2.GetCenter() + Vector2(g_unitDistance / 2.f, g_unitDistance / 2.f)+ Vector2(g_unitDistance/2,g_unitDistance/2);
}

Vector2 Tiles::GetBottomLeft()
{
	return m_aabb2.GetCenter() + Vector2(-g_unitDistance / 2.f, -g_unitDistance / 2.f)  + Vector2(g_unitDistance/2,g_unitDistance/2);
}

Vector2 Tiles::GetBottomRight()
{
	return m_aabb2.GetCenter() + Vector2(g_unitDistance / 2.f, -g_unitDistance / 2.f)+ Vector2(g_unitDistance/2,g_unitDistance/2);
}

Vector2 Tiles::GetCentreLeft()
{
	return m_aabb2.GetCenter() + Vector2(-g_unitDistance / 2.f, 0)+ Vector2(g_unitDistance/2,g_unitDistance/2);
}

Vector2 Tiles::GetCentreRight()
{
	return m_aabb2.GetCenter() + Vector2(g_unitDistance / 2.f, 0.f)+ Vector2(g_unitDistance/2,g_unitDistance/2);
}

Vector2 Tiles::GetCentreTop()
{
	return m_aabb2.GetCenter() + Vector2(0.f, g_unitDistance / 2.f)+ Vector2(g_unitDistance/2,g_unitDistance/2);
}

Vector2 Tiles::GetCentreBottom()
{
	return m_aabb2.GetCenter() + Vector2(0.f, -g_unitDistance / 2.f)+ Vector2(g_unitDistance/2,g_unitDistance/2);
}

