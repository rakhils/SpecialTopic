#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Math/MathUtil.hpp"

//#include "Game/GamePlay/Entity/Entity.hpp"
class Tiles;
enum MAP_TYPE
{
	MAP_TYPE_SEEK_ARRIVE_BEHAVIOR,
	MAP_TYPE_ALIGNMENT_SEPERATION_COHESION_BEHAVIOR,
	MAP_TYPE_FLOW_FIELD_BEHAVIOR,
	MAP_TYPE_PATH_FOLLOWING,
	MAP_TYPE_UNKNOWN
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

	bool IsPositionInRange(Vector2 position) const
	{
		float distance = GetShortestDistanceBetweenLineAndPoint(m_start, m_end, position);
		if(distance < m_radius)
		{
			return true;
		}
		return false;
	}

	Vector2 FindClosestPositionAlongPath(Vector2 position) const
	{
		Vector2 line = m_end - m_start;
		line = line.GetNormalized();

		Vector2 lineToPosition = position - m_start;

		float projectedDistance = DotProduct(lineToPosition, line);

		return m_start + line * projectedDistance;
	}

	Vector2 GetDirection() const
	{
		Vector2 direction = m_end - m_start;
		direction = direction.GetNormalized();
		return direction;
	}
};
class Map
{
public:
	bool							m_init							= false;
	bool							m_start							= false;
	Camera *						m_camera						= nullptr;
	MAP_TYPE						m_mapType						= MAP_TYPE_UNKNOWN;

	std::vector<Entity*>			m_entities;
	std::vector<Vector2>			m_flowField;
	std::vector<Path>				m_paths;

	// DEBUG
	std::vector<DebugCircle>		m_debugCircles;
	std::vector<DebugLines>			m_debugLines;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float							m_cohesiveForce					= 1.f;
	float							m_alignmentForce				= 1.f;
	float							m_seperationForce				= 1.f;
	bool							m_flowFieldVisible				= false;

	Mesh *							m_flowFieldMesh					= nullptr;
	Mesh *							m_pathMesh						= nullptr;

	Map();
	~Map();

	// Map Initialization
	void							Initialize();
	void							InitCamera();
	void							InitFlowField();
	void							InitPath();
	void							InitEntities();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void							CreatePlayerEntity(Vector2 &position,float angle);
	void							DestroyLastEntity();
	void							DestroyAllEntity();
	int								GetClosestPathIndex(Vector2 &position);
	void							ClearDebugEntities();

	void							ProcessInputs(float deltaTime);
	// Update cycle
	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);
	void							UpdateEntity(float deltaTime);
	void							ProcessInputForNobes();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Behaviors
	void							SeekBehavior();
	void							ArriveBehavior();
	void							FollowFlowField();
	void							FollowPath();
	void							AlignBehavior();
	void							SeperateBehavior();
	void							CohesionBehavior();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Rendering
	void							Render();
	void							RenderEntities();
	void							RenderFlowField();
	void							RenderPaths();
	void							RenderDebugPoints();
	void							RenderSteeringKnobsInUI();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};