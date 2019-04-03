#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Math/MathUtil.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
class Tiles;
enum MAP_TYPE
{
	SEEK_ARRIVE_BEHAVIOR,
	ALIGNMENT_SEPERATION_COHESION_BEHAVIOR,
	FLOW_FIELD_BEHAVIOR,
	PATH_FOLLOWING,

};
struct DebugLines
{
	Vector2 m_position1;
	Vector2 m_position2;
	Rgba    m_color;
	DebugLines(Vector2 position1,Vector2 position2,Rgba color)
	{
		m_position1 = position1;
		m_position2 = position2;
		m_color     = color;
	}
};
struct DebugCircle
{
	Vector2 m_position;
	float   m_radius;
	Rgba    m_color;
	DebugCircle(Vector2 position,float radius,Rgba color)
	{
		m_position = position;
		m_radius   = radius;
		m_color    = color;
	}
};
struct Path
{
	Vector2 m_start;
	Vector2 m_end;
	float   m_radius;
	Path(Vector2 startPosition,Vector2 endPositoin,float radius)
	{
		m_start  = startPosition;
		m_end	 = endPositoin;
		m_radius = radius;
	}

	bool IsPositionInRange(Vector2 position)
	{
		float distance = GetShortestDistanceBetweenLineAndPoint(m_start, m_end, position);
		if(distance < m_radius)
		{
			return true;
		}
		return false;
	}

	Vector2 FindClosestPositionAlongPath(Vector2 position)
	{
		Vector2 line = m_end - m_start;
		line = line.GetNormalized();

		Vector2 lineToPosition = position - m_start;

		float projectedDistance = DotProduct(lineToPosition, line);

		return m_start + line * projectedDistance;
	}

	Vector2 GetDirection()
	{
		Vector2 direction = m_end - m_start;
		direction = direction.GetNormalized();
		return direction;
	}
};
class Map
{
public:
	bool							m_init		   = false;
	Camera *						m_camera       = nullptr;
	MAP_TYPE						m_mapType;
	std::vector<Tiles*>				m_tiles;
	std::vector<Entity*>			m_entities;
	std::vector<Vector2>			m_flowField;
	std::vector<Path>				m_paths;

	float							m_cohesiveForce   = 1.f;
	float							m_alignmentForce  = 1.f;
	float							m_seperationForce = 1.f;

	// DEBUG
	std::vector<DebugCircle>		m_debugCircles;
	std::vector<DebugLines>			m_debugLines;

	Mesh *							m_flowFieldMesh = nullptr;
	Mesh *							m_pathMesh		= nullptr;
	bool							m_flowFieldVisible = false;

	AABB2							m_cohesionNegativeBounds;
	AABB2							m_cohesionPositiveBounds;
	AABB2							m_cohesionValueBounds;
	AABB2							m_cohesionStringValueBounds;

	AABB2							m_seperationNegativeBounds;
	AABB2							m_seperationPositiveBounds;
	AABB2							m_seperationValueBounds;
	AABB2							m_seperationStringValueBounds;

	AABB2							m_alignmentNegativeBounds;
	AABB2							m_alignmentPositiveBounds;
	AABB2							m_alignmentValueBounds;
	AABB2							m_alignmentStringValueBounds;


	
	Map();
	~Map();

	void							Initialize();
	void							InitCamera();
	void							InitTiles();
	void							InitFlowField();
	void							InitPath();
	void							InitNobes();

	void							CreateEntity(Vector2 position,float angle);
	void							DestroyLastEntity();
	void							DestroyAllEntity();
	int								GetClosestPathIndex(Vector2 position);
	void							ClearDebugEntities();

	void							ProcessInputs(float deltaTime);

	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);
	void							UpdateEntity(float deltaTime);
	void							ProcessInputForNobes();

	void							SeekBehavior();
	void							ArriveBehavior();
	void							FollowFlowField();
	void							FollowPath();
	void							AlignBehavior();
	void							SeperateBehavior();
	void							CohesionBehavior();

	void							Render();
	void							RenderEntity();
	void							RenderFlowField();
	void							RenderPaths();
	void							RenderDebugPoints();
	void							RenderNobes();

};