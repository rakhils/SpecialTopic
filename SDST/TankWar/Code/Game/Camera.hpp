#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtil.hpp"

class Camera
{
public:
	Vector2 WorldPosition;
	float m_orientationDegrees = 0;
	float m_numTilesInViewVertically = 0;
	float m_numTilesInViewHorizontally = 0;
	float m_cameraRatio = 16.0f/9.0f;
	float m_tileWidth = 2;
	Vector2 m_positionVector;
	Vector2 m_minPosition;
	Vector2 m_maxPosition;

	float currentTime;
	float screenShakeTime;


	Camera();
	//~Camera();
	void resetCamera();
	void setOrtographicPosition(Vector2 positionVector,Vector2 minsPosition,Vector2 maxPosition);
	void setOrtographicPosition(Vector2 minPosition,Vector2 maxPosition);
	void setDefaultOrthoPosition();
	
	void screenShake(float time);
};