// GAME RELATED
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Tiles/Tiles.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"

// ENGINE RELATED
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"

// Constructor
Map::Map()
{
	Initialize();
}

Map::~Map()
{
	// Destructor
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Initialize the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::Initialize()
{
	InitCamera();
	InitFlowField();
	InitPath();

	m_entities.reserve(25);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Initialize camera system (creates frame buffer set color targets)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	Vector2 resoulution  = Windows::GetInstance()->GetDimensions().GetAsVector2();
	m_camera   = new OrthographicCamera();
	m_camera->SetDefaultFramebuffer(new FrameBuffer());
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(resoulution/2.f);
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Initialize flow fields for the flow field demo
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitFlowField()
{
	m_flowField.reserve(g_maxWidth * g_maxHeight);

	MeshBuilder flowFieldBuilder;
	flowFieldBuilder.Begin(PRIMITIVE_LINES, false); // Rendering flow field direction as lines
	for(int indexY = 0;indexY < g_maxHeight;indexY++)
	{
		for (int indexX = 0; indexX < g_maxWidth; indexX++)
		{
			float randomAngle = GetRandomFloatInRange(-g_randomAngle, g_randomAngle); // random angle for forward direction
			Vector2 direction(CosDegrees(randomAngle), SinDegrees(randomAngle));
			m_flowField.push_back(direction);
			Vector2 position(indexX * g_unitDistance, indexY * static_cast<float>(g_unitDistance));

			flowFieldBuilder.SetColor(Rgba::WHITE);
			flowFieldBuilder.PushVertex(position);

			flowFieldBuilder.SetColor(Rgba::WHITE);
			flowFieldBuilder.PushVertex(position + direction*static_cast<float>(g_unitDistance));
		}
	}
	flowFieldBuilder.End();
	m_flowFieldMesh = flowFieldBuilder.CreateMesh();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Initialize path for path following demo
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitPath()
{
	float pathRadius = g_unitDistance * 2; // Random path for the project
	m_paths.reserve(9);
	m_paths.emplace_back(Path(g_demoPath1, g_demoPath2, pathRadius));
	m_paths.emplace_back(Path(g_demoPath2, g_demoPath3, pathRadius));
	m_paths.emplace_back(Path(g_demoPath3, g_demoPath4, pathRadius));
	m_paths.emplace_back(Path(g_demoPath4, g_demoPath5, pathRadius));
	m_paths.emplace_back(Path(g_demoPath5, g_demoPath6, pathRadius));
	m_paths.emplace_back(Path(g_demoPath6, g_demoPath7, pathRadius));
	m_paths.emplace_back(Path(g_demoPath7, g_demoPath8, pathRadius));
	m_paths.emplace_back(Path(g_demoPath8, g_demoPath9, pathRadius));
	m_paths.emplace_back(Path(g_demoPath9, g_demoPath10, pathRadius));

	MeshBuilder pathBuilder;
	pathBuilder.Begin(PRIMITIVE_LINES, false); // Rendering paths as lines
	for(int index = 0;index < m_paths.size();index++)
	{
		pathBuilder.SetColor(Rgba::WHITE);
		pathBuilder.PushVertex(m_paths.at(index).m_start);
		pathBuilder.SetColor(Rgba::WHITE);
		pathBuilder.PushVertex(m_paths.at(index).m_end);
	}
	pathBuilder.End();
	m_pathMesh = pathBuilder.CreateMesh();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/10
*@purpose : Initialize all entities needed for the demo
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitEntities()
{
	CreatePlayerEntity(g_playerStart, 0.f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Creates the main player entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreatePlayerEntity(Vector2 &position, float angle)
{
	Entity *entity      = new Entity(position);
	entity->m_angle		= angle;
	m_entities.emplace_back(entity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Destroys the last entity and clean up memory from entity list
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::DestroyLastEntity()
{
	Entity *entity = m_entities.at(m_entities.size() - 1);
	m_entities.erase(m_entities.begin() + m_entities.size() - 2, m_entities.begin() + m_entities.size() - 1);
	delete entity;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Destroys and cleans up memory from the entity list
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::DestroyAllEntity()
{
	for(int index = 0;index < m_entities.size();index++)
	{
		Entity *entity = m_entities.at(index);
		m_entities.erase(m_entities.begin() + index, m_entities.begin() + index + 1);
		index--;
		delete entity;
	}
	if(m_pathMesh != nullptr)
	{
		delete m_pathMesh;
		m_pathMesh = nullptr;
	}
	if(m_flowFieldMesh != nullptr)
	{
		delete m_flowFieldMesh;
		m_flowFieldMesh = nullptr;
	}
	delete m_camera;
	m_camera = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Retrieves the closest index from a position
*@			Found using Point Vs Line Math Lib
*@param   : Position in map
*@return  : Closest path index
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetClosestPathIndex(Vector2 &position)
{
	int returnIndex = 0;
	float minDistance = g_maxValue;
	for(int index = 0;index < m_paths.size();index++)
	{
		float distance = GetShortestDistanceBetweenLineAndPoint(m_paths.at(index).m_start, m_paths.at(index).m_end, position);
		Vector2 AB = m_paths.at(index).m_end - m_paths.at(index).m_start;
		Vector2 BA = m_paths.at(index).m_start - m_paths.at(index).m_end;

		AB = AB.GetNormalized();
		BA = BA.GetNormalized();

		Vector2 pointA = position - m_paths.at(index).m_start;
		Vector2 pointB = position - m_paths.at(index).m_end;

		pointA = pointA.GetNormalized();
		pointB = pointB.GetNormalized();

		if(DotProduct(AB,pointA) > 0 && DotProduct(BA,pointB) > 0)
		{
			if (distance < minDistance)
			{
				minDistance = distance;
				returnIndex = index;
			}
		}
	}
	return returnIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Clears all debug entities in the debug list
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ClearDebugEntities()
{
	m_debugLines.clear();
	m_debugCircles.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Process all inputs (Mouse and Keyboard)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	if(InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_SPACE))
	{
		CreatePlayerEntity(mousePosition,0);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_V))
	{
		m_flowFieldVisible = m_flowFieldVisible == true ? false : true;
	}

	ProcessInputForNobes();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Main Update loop of the Map
*@param   : Delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	UpdateCamera(deltaTime);
	ClearDebugEntities();

	UpdateEntity(deltaTime);
	switch (m_mapType)
	{
	case MAP_TYPE_FLOW_FIELD_BEHAVIOR:
		FollowFlowField();
		break;
	case MAP_TYPE_PATH_FOLLOWING:
		FollowPath();
		break;
	case  MAP_TYPE_SEEK_ARRIVE_BEHAVIOR:
		SeekBehavior();
		ArriveBehavior();
		break;
		break;
	default:
		break;
	}
	AlignBehavior();
	SeperateBehavior();
	CohesionBehavior();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Updates camera currently used orthographic camera
*@param   : Delta Time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera(float deltaTime)
{
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Updates all entities boundary
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateEntity(float deltaTime)
{
	for(int index = 0;index < m_entities.size();index++)
	{
		m_entities.at(index)->Update(deltaTime);

		// Check entities inside boundary
		if (m_entities.at(index)->m_position.x > (Windows::GetInstance()->GetDimensions().x - g_unitDistance))
		{
			m_entities.at(index)->m_position.x = -static_cast<float>(g_unitDistance);
		}
		if (m_entities.at(index)->m_position.y > (Windows::GetInstance()->GetDimensions().y - g_unitDistance))
		{
			m_entities.at(index)->m_position.y = -g_unitDistance;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/02
*@purpose : Process inputs for steering behavioral knobs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ProcessInputForNobes()
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	if (InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		m_start = m_start ? false : true;
	}

	if(InputSystem::GetInstance()->IsLButtonDown())
	{
		if(g_cohesionNegativeBounds.IsPointInside(mousePosition))
		{
			m_cohesiveForce -= g_deltaChangeInForce;
			m_cohesiveForce = ClampFloat(m_cohesiveForce, 0, 5);
		}
		if (g_cohesionPositiveBounds.IsPointInside(mousePosition))
		{
			m_cohesiveForce += g_deltaChangeInForce;
			m_cohesiveForce = ClampFloat(m_cohesiveForce, 0, 5);
		}
		if (g_seperationNegativeBounds.IsPointInside(mousePosition))
		{
			m_seperationForce -= g_deltaChangeInForce;
			m_seperationForce = ClampFloat(m_seperationForce, 0, 5);
		}
		if (g_seperationPositiveBounds.IsPointInside(mousePosition))
		{
			m_seperationForce += g_deltaChangeInForce;
			m_seperationForce = ClampFloat(m_seperationForce, 0, 5);
		}
		if (g_alignmentNegativeBounds.IsPointInside(mousePosition))
		{
			m_alignmentForce -= g_deltaChangeInForce;
			m_alignmentForce = ClampFloat(m_alignmentForce, 0, 5);
		}
		if (g_alignmentPositiveBounds.IsPointInside(mousePosition))
		{
			m_alignmentForce += g_deltaChangeInForce;
			m_alignmentForce = ClampFloat(m_alignmentForce, 0, 5);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Applies Seek force
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SeekBehavior()
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
	for(int index = 0;index < m_entities.size();index++)
	{
		Vector2 desiredVelocity = mousePosition - m_entities.at(index)->m_position;
		desiredVelocity = desiredVelocity.GetNormalized();
		desiredVelocity = desiredVelocity * m_entities.at(index)->m_maxSpeed;

		Vector2 appliedForce = desiredVelocity - m_entities.at(index)->m_velocity;
		appliedForce = appliedForce.GetNormalized();
		appliedForce = appliedForce * m_entities.at(index)->m_maxForce;
		m_entities.at(index)->ApplyForce(appliedForce);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Applies arriving force
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ArriveBehavior()
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
	for (int index = 0; index < m_entities.size(); index++)
	{
		Vector2 desiredVelocity = mousePosition - m_entities.at(index)->m_position;
		float desiredMagnitude = desiredVelocity.GetLength();

		if(desiredMagnitude < g_maxArriveForce)
		{
			float appliedMagnitude = RangeMapFloat(desiredMagnitude, 0.f, g_maxArriveForce, 0.f, m_entities.at(index)->m_maxSpeed);
			desiredVelocity.Limit(appliedMagnitude);
		}
		else
		{
			desiredVelocity.Limit(m_entities.at(index)->m_maxSpeed);
		}

		Vector2 appliedForce = desiredVelocity - m_entities.at(index)->m_velocity;
		appliedForce = appliedForce.GetNormalized();
		appliedForce = appliedForce * m_entities.at(index)->m_maxForce;
		m_entities.at(index)->ApplyForce(appliedForce);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Set force for following flow field
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::FollowFlowField()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		Vector2 desiredVelocity;
		Vector2 position = m_entities.at(index)->m_position;
		int xIndex = static_cast<int>(position.x / g_unitDistance);
		int yIndex = static_cast<int>(position.y / g_unitDistance);

		int flowFieldIndex = yIndex * g_maxWidth + xIndex;
		if(flowFieldIndex < m_flowField.size())
		{
			desiredVelocity = m_flowField.at(flowFieldIndex);
		}
		else
		{
			desiredVelocity = Vector2::ONE_ZERO; // default direction
		}

		desiredVelocity = desiredVelocity * m_entities.at(index)->m_maxSpeed;

		Vector2 appliedForce = desiredVelocity - m_entities.at(index)->m_velocity;
		appliedForce = appliedForce.GetNormalized();
		appliedForce = appliedForce * m_entities.at(index)->m_maxForce;
		m_entities.at(index)->ApplyForce(appliedForce );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Set all entities to follow path
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::FollowPath()
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
	int pathIndex = GetClosestPathIndex(mousePosition);
	Vector2 closestPosition = m_paths.at(pathIndex).FindClosestPositionAlongPath(mousePosition);

	m_debugCircles.reserve(m_entities.size());
	m_debugCircles.emplace_back(DebugCircle(closestPosition,g_unitDistance/4.f,Rgba::GREEN));
	for(int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++)
	{
		pathIndex = GetClosestPathIndex(m_entities.at(entityIndex)->m_position);
		Vector2 position		  = m_entities.at(entityIndex)->m_position;

		closestPosition = m_paths.at(pathIndex).FindClosestPositionAlongPath(position);
		closestPosition = closestPosition + m_paths.at(pathIndex).GetDirection() * g_unitDistance * 5;

		m_debugCircles.emplace_back(DebugCircle(closestPosition,g_unitDistance/4.f,Rgba::RED));

		Vector2 desiredVelocity = closestPosition - m_entities.at(entityIndex)->m_position;
		desiredVelocity = desiredVelocity.GetNormalized();
		desiredVelocity = desiredVelocity * m_entities.at(entityIndex)->m_maxSpeed;

		Vector2 appliedForce = desiredVelocity - m_entities.at(entityIndex)->m_velocity;
		appliedForce = appliedForce.GetNormalized();
		appliedForce = appliedForce * m_entities.at(entityIndex)->m_maxForce;
		m_entities.at(entityIndex)->ApplyForce(appliedForce );		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/02
*@purpose : Apply alignment force to each entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::AlignBehavior()
{
	for(int index = 0;index < m_entities.size();index++)
	{
		Vector2 neighbourVelocity = Vector2::ZERO;
		int     neighbourCount = 0;
		for(int entityIndex = 0;entityIndex < m_entities.size();entityIndex++)
		{
			Vector2 distance = m_entities.at(index)->m_position - m_entities.at(entityIndex)->m_position;
			if(distance.GetLengthSquared() < g_unitDistance * g_unitDistance)
			{
				neighbourCount++;
				neighbourVelocity += m_entities.at(entityIndex)->m_velocity;
			}
		}

		Vector2 desiredVelocity = neighbourVelocity / static_cast<float>(neighbourCount);
		desiredVelocity = desiredVelocity.GetNormalized();
		desiredVelocity = desiredVelocity * m_entities.at(index)->m_maxSpeed;

		Vector2 appliedForce = desiredVelocity - m_entities.at(index)->m_velocity;
		appliedForce = appliedForce.GetNormalized();
		appliedForce = appliedForce * m_entities.at(index)->m_maxForce;
		m_entities.at(index)->ApplyForce(appliedForce * m_alignmentForce);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/02
*@purpose : Apply separation force to each entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SeperateBehavior()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		Vector2 neighbourVelocity = Vector2::ZERO;
		int     neighbourCount = 0;
		for (int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++)
		{
			Vector2 distance = m_entities.at(index)->m_position - m_entities.at(entityIndex)->m_position;
			if (distance.GetLengthSquared() < g_unitDistance * g_unitDistance)
			{
				neighbourCount++;
				Vector2 distanceFromNeighbour = ( m_entities.at(index)->m_position - m_entities.at(entityIndex)->m_position);
				distanceFromNeighbour = distanceFromNeighbour.GetNormalized();
				
				neighbourVelocity += distanceFromNeighbour;
			}
		}
		if(neighbourCount <= 1)
		{
			continue;
		}
		Vector2 desiredVelocity = neighbourVelocity / static_cast<float>(neighbourCount);
		desiredVelocity = desiredVelocity.GetNormalized();
		desiredVelocity = desiredVelocity * m_entities.at(index)->m_maxSpeed;

		Vector2 appliedForce = desiredVelocity - m_entities.at(index)->m_velocity;
		appliedForce = appliedForce.GetNormalized();
		appliedForce = appliedForce * m_entities.at(index)->m_maxForce;
		m_entities.at(index)->ApplyForce(appliedForce * m_seperationForce);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/02
*@purpose : Apply cohesion force to each entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CohesionBehavior()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		Vector2 neighbourVelocity = Vector2::ZERO;
		int     neighbourCount = 0;
		for (int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++)
		{
			Vector2 distance = m_entities.at(index)->m_position - m_entities.at(entityIndex)->m_position;
			if (distance.GetLengthSquared() < g_unitDistance * g_unitDistance)
			{
				neighbourCount++;
				Vector2 distanceFromNeighbour = m_entities.at(entityIndex)->m_position - m_entities.at(index)->m_position;
				neighbourVelocity += distanceFromNeighbour;
			}
		}
		if (neighbourCount <= 1) // no neighbours
		{
			continue;
		}
		Vector2 desiredVelocity = neighbourVelocity / static_cast<float>(neighbourCount);
		desiredVelocity = desiredVelocity.GetNormalized();
		desiredVelocity = desiredVelocity * m_entities.at(index)->m_maxSpeed;

		Vector2 appliedForce = desiredVelocity - m_entities.at(index)->m_velocity;
		appliedForce = appliedForce.GetNormalized();
		appliedForce = appliedForce * m_entities.at(index)->m_maxForce;
		m_entities.at(index)->ApplyForce(appliedForce * m_cohesiveForce);
	}
}

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();

	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
	Material *defaultMaterial = Material::AquireResource(g_defaultMaterialPath);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawSolidCircle(mousePosition, g_unitDistance / 4.f, Rgba::RED);
	delete defaultMaterial;

	switch (m_mapType)
	{
	case MAP_TYPE_FLOW_FIELD_BEHAVIOR:
		RenderFlowField();
		break;
	case MAP_TYPE_PATH_FOLLOWING:
		RenderPaths();
		break;
	default:
		break;
	}
	RenderEntities();
	RenderDebugPoints();
	RenderSteeringKnobsInUI();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Renders flow field in the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderFlowField()
{
	Material *defaultMaterial = Material::AquireResource(g_defaultMaterialPath);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawMesh(m_flowFieldMesh,Matrix44::GetIdentity());
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Renders path in the map 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderPaths()
{
	Material *defaultMaterial = Material::AquireResource(g_defaultMaterialPath);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for(int index = 0;index < m_paths.size();index++)
	{
		Path currentPath = m_paths.at(index);
		Vector2 pathDirection = currentPath.GetDirection();
		float   angle		  = Atan2Degrees(pathDirection.y,pathDirection.x);
		float   normalAngle2D = angle + 90;
		Vector2 normalDirection(CosDegrees(normalAngle2D), SinDegrees(normalAngle2D));

		Vector2 leftTop     = currentPath.m_start + normalDirection * currentPath.m_radius;
		Vector2 leftBottom  = currentPath.m_start - normalDirection * currentPath.m_radius;

		Vector2 rightTop    = currentPath.m_end   + normalDirection * currentPath.m_radius;
		Vector2 rightBottom = currentPath.m_end   - normalDirection * currentPath.m_radius;

		Renderer::GetInstance()->DrawSolidRectangle(leftBottom, rightBottom, rightTop, leftTop,Rgba::FADED_BLUE);
		Renderer::GetInstance()->DrawLine(m_paths.at(index).m_start, m_paths.at(index).m_end);
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : Renders debug entities in map (circles and lines)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderDebugPoints()
{
	Material *defaultMaterial = Material::AquireResource(g_defaultMaterialPath);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	for(int index = 0;index < m_debugCircles.size();index++)
	{
		Renderer::GetInstance()->DrawSolidCircle(m_debugCircles.at(index).m_position, m_debugCircles.at(index).m_radius, m_debugCircles.at(index).m_color);
	}
	for (int index = 0; index < m_debugLines.size(); index++)
	{
		DebugLines line = m_debugLines.at(index);
		Renderer::GetInstance()->DrawLine(line.m_position1, line.m_position2, line.m_color);
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/02
*@purpose : Render all Steering knobs in UI
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderSteeringKnobsInUI()
{
	// Steering knob buttons
	Material *defaultMaterial = Material::AquireResource(g_defaultMaterialPath);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawAABB(g_cohesionNegativeBounds,   Rgba::RED);
	Renderer::GetInstance()->DrawAABB(g_cohesionPositiveBounds,   Rgba::GREEN);
									  
	Renderer::GetInstance()->DrawAABB(g_seperationNegativeBounds, Rgba::RED);
	Renderer::GetInstance()->DrawAABB(g_seperationPositiveBounds, Rgba::GREEN);

	Renderer::GetInstance()->DrawAABB(g_alignmentNegativeBounds,  Rgba::RED);
	Renderer::GetInstance()->DrawAABB(g_alignmentPositiveBounds,  Rgba::GREEN);
	delete defaultMaterial;

	// Steering values
	Material *textMaterial = Material::AquireResource(g_textMaterialPath);
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOn3DPoint(g_cohesionValueBounds.GetCenter(), 
		Vector2::ONE_ZERO, Vector2::ZERO_ONE, ToString(m_cohesiveForce,2),   g_fontSize, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(g_seperationValueBounds.GetCenter(), 
		Vector2::ONE_ZERO, Vector2::ZERO_ONE, ToString(m_seperationForce,2), g_fontSize, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(g_alignmentValueBounds.GetCenter(),  
		Vector2::ONE_ZERO, Vector2::ZERO_ONE, ToString(m_alignmentForce,2),  g_fontSize, Rgba::WHITE);

	// Constants
	Renderer::GetInstance()->DrawTextOn3DPoint(g_cohesionStringValueBounds.GetCenter(), 
		Vector2::ONE_ZERO, Vector2::ZERO_ONE, COHESION,   g_fontSize, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(g_seperationStringValueBounds.GetCenter(), 
		Vector2::ONE_ZERO, Vector2::ZERO_ONE, SEPARATION, g_fontSize, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(g_alignmentStringValueBounds.GetCenter(),
		Vector2::ONE_ZERO, Vector2::ZERO_ONE, ALIGNMENT,  g_fontSize, Rgba::WHITE);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/10
*@purpose : Renders all entities in the game
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderEntities()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		m_entities.at(index)->Render();
	}
}
