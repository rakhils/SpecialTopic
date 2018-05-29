#include "Camera.hpp"

Camera::Camera()
{
	m_numTilesInViewVertically = 7;
	m_numTilesInViewHorizontally = m_cameraRatio*m_numTilesInViewVertically;
}

void Camera::resetCamera()
{
	m_numTilesInViewVertically = 7;
	m_numTilesInViewHorizontally = m_cameraRatio*m_numTilesInViewVertically;
}

void Camera::setOrtographicPosition(Vector2 positionVector,Vector2 minsPosition,Vector2 maxPosition)
{
	this->m_positionVector = positionVector;

	float minX = minsPosition.x + m_numTilesInViewHorizontally;
	float minY = minsPosition.y + m_numTilesInViewVertically;

	float maxX = maxPosition.x - m_numTilesInViewHorizontally;
	float maxY = maxPosition.y - m_numTilesInViewVertically;

	positionVector.x = ClampFloat(positionVector.x,minX,maxX);
	positionVector.y = ClampFloat(positionVector.y,minY,maxY);
	m_positionVector = positionVector;
	Vector2 minPosition1(positionVector.x - (m_numTilesInViewHorizontally) , positionVector.y - (m_numTilesInViewVertically));
	Vector2 maxPosition1(positionVector.x + (m_numTilesInViewHorizontally) , positionVector.y + (m_numTilesInViewVertically));
	m_minPosition = minPosition1;
	m_maxPosition = maxPosition1;
	g_theRenderer->SetOrtho(minPosition1,maxPosition1);

}

void Camera::setOrtographicPosition(Vector2 minPosition,Vector2 maxPosition)
{
	m_positionVector = (minPosition+maxPosition)/2;
	g_theRenderer->SetOrtho(minPosition,maxPosition);
}

void Camera::setDefaultOrthoPosition()
{
	g_theRenderer->SetOrtho(m_minPosition,m_maxPosition);
}

void Camera::screenShake(float time)
{
	currentTime = static_cast<float>(GetCurrentTimeSeconds());
	screenShakeTime = time;
}


