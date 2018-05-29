#include "Engine/Math/CubicSpline2D.hpp"

CubicSpline2D::CubicSpline2D(const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray/*=nullptr */)
{
	AppendPoints(positionsArray,numPoints,velocitiesArray);
}

void CubicSpline2D::AppendPoint(const Vector2& position, const Vector2& velocity/*=Vector2::ZERO */)
{
	m_positions.push_back(position);
	m_velocities.push_back(velocity);
}

void CubicSpline2D::AppendPoints(const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray/*=nullptr */)
{
	for(int index = 0;index < numPoints;index++)
	{
		m_positions.push_back(positionsArray[index]);
		if(velocitiesArray != nullptr)
		{
			m_velocities.push_back(velocitiesArray[index]);
		}
		else
		{
			m_velocities.push_back(Vector2(0,0));
		}
	}
}

void CubicSpline2D::InsertPoint(int insertBeforeIndex, const Vector2& position, const Vector2& velocity/*=Vector2::ZERO */)
{
	m_positions.insert(m_positions.begin()+insertBeforeIndex,position);
	if(velocity != Vector2(0,0))
	{
		m_velocities.insert(m_velocities.begin() + insertBeforeIndex,velocity);
	}
	else
	{
		m_velocities.insert(m_velocities.begin() + insertBeforeIndex,Vector2(0,0));
	}
}

void CubicSpline2D::RemovePoint(int pointIndex)
{
	m_positions.erase(m_positions.begin()+pointIndex);
	m_velocities.erase(m_velocities.begin()+pointIndex);
}

void CubicSpline2D::RemoveAllPoints()
{
	m_positions.clear();
	m_velocities.clear();
}

void CubicSpline2D::SetPoint(int pointIndex, const Vector2& newPosition, const Vector2& newVelocity)
{
	m_positions.at(pointIndex) = newPosition;
	m_velocities.at(pointIndex) = newVelocity;
}

void CubicSpline2D::SetPosition(int pointIndex, const Vector2& newPosition)
{
	m_positions.at(pointIndex) = newPosition;
}

void CubicSpline2D::SetVelocity(int pointIndex, const Vector2& newVelocity)
{
	m_velocities.at(pointIndex) = newVelocity;
}

void CubicSpline2D::SetCardinalVelocities(float tension/*=0.f*/, const Vector2& startVelocity/*=Vector2::ZERO*/, const Vector2& endVelocity/*=Vector2::ZERO */)
{
	if(startVelocity != Vector2(0,0))
	m_velocities.at(0) = startVelocity;
	int index = 1;
	for(;index < m_velocities.size();index++)
	{
		if(index + 1 < m_velocities.size())
		{
			Vector2 velocity = (1.0f - tension)*(m_positions.at(index + 1) - m_positions.at(index - 1))/2.0f; 
			m_velocities.at(index) = velocity;
		}
	}
	if(endVelocity != Vector2(0,0))
	m_velocities.at(index - 1) = endVelocity;
}

const Vector2 CubicSpline2D::GetPosition(int pointIndex)
{
	return m_positions.at(pointIndex);
}

const Vector2 CubicSpline2D::GetVelocity(int pointIndex)
{
	return m_velocities.at(pointIndex);
}

int CubicSpline2D::GetPositions(std::vector<Vector2>& out_positions) const
{
	for(int index = 0;index<m_velocities.size();index++)
	{
		out_positions.push_back(m_positions.at(index));
	}
	return static_cast<int>(m_positions.size());
}

int CubicSpline2D::GetVelocities(std::vector<Vector2>& out_velocities) const
{
	out_velocities.clear();
	for(int index = 0;index<m_velocities.size();index++)
	{
		out_velocities.push_back(m_velocities.at(index));
	}
	return static_cast<int>(m_velocities.size());
}

Vector2 CubicSpline2D::EvaluateAtCumulativeParametric(float t) const
{
	int lowestInteger = static_cast<int>(t);
	if(static_cast<float>(lowestInteger) > t)
	{
		lowestInteger--;
	}
	t = t - static_cast<float>(lowestInteger);
	

	return EvaluateCubicHermite<Vector2>(m_positions.at(lowestInteger),m_velocities.at(lowestInteger),m_positions.at(lowestInteger + 1),m_velocities.at(lowestInteger+1),t);
}

Vector2 CubicSpline2D::EvaluateAtNormalizedParametric(float t) const
{
	int totalNumberOfCurves = static_cast<int>(m_positions.size() - 1);
	float rangeMappedValue = RangeMapFloat(t,0.0f,1.0f,0.0f,static_cast<float>(totalNumberOfCurves));
	t = rangeMappedValue;
	int lowestInteger = static_cast<int>(rangeMappedValue);
	if(static_cast<float>(lowestInteger) > rangeMappedValue)
	{
		lowestInteger--;
	}
	t = t - static_cast<float>(lowestInteger);
	

	return EvaluateCubicHermite<Vector2>(m_positions.at(lowestInteger),m_velocities.at(lowestInteger),m_positions.at(lowestInteger + 1),m_velocities.at(lowestInteger+1),t);
}
