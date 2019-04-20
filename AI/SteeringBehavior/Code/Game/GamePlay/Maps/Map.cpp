#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GamePlay/Tiles/Tiles.hpp"

#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/StringUtils.hpp"

Map::Map()
{
	Initialize();
}

Map::~Map()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Init the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::Initialize()
{
	InitCamera();
	//InitTiles();
	InitFlowField();
	InitPath();
	InitNobes();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Inits camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_camera   = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTiles()
{
	for (int tileIndex = 0; tileIndex < g_maxWidth * g_maxHeight; tileIndex++)
	{
		m_tiles.push_back(new Tiles());
	}
	Texture *texture = Texture::CreateOrGetTexture("Data\\Images\\Map.png", true, false);

	for (int indexY = 0; indexY < g_maxHeight; indexY++)
	{
		for (int indexX = 0; indexX < g_maxWidth; indexX++)
		{
			int index = indexY * g_maxWidth + indexX;
			Rgba color = texture->getTexel(index);
			if (color != Rgba::WHITE)
			{
				m_tiles.at(index)->m_type = BLOCK;
			}

			int xCords = index % g_maxWidth;
			int yCords = index / g_maxWidth;

			m_tiles.at(index)->m_tileCoords = IntVector2(xCords*g_unitDistance, yCords * g_unitDistance);

			IntVector2 mins = m_tiles.at(index)->m_tileCoords - IntVector2(g_unitDistance / 2, g_unitDistance / 2) + IntVector2(g_unitDistance / 2, g_unitDistance / 2);
			IntVector2 maxs = m_tiles.at(index)->m_tileCoords + IntVector2(g_unitDistance / 2, g_unitDistance / 2) + IntVector2(g_unitDistance / 2, g_unitDistance / 2);

			m_tiles.at(index)->m_aabb2 = AABB2(mins.GetAsVector2(), maxs.GetAsVector2());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitFlowField()
{
	MeshBuilder flowFieldBuilder;
	flowFieldBuilder.Begin(PRIMITIVE_LINES, false);
	for(int indexY = 0;indexY < g_maxHeight;indexY++)
	{
		for (int indexX = 0; indexX < g_maxWidth; indexX++)
		{
			float randomAngle = GetRandomFloatInRange(-45, 45);
			Vector2 direction(CosDegrees(randomAngle), SinDegrees(randomAngle));
			m_flowField.push_back(direction);
			Vector2 position(indexX * g_unitDistance, indexY * g_unitDistance);

			flowFieldBuilder.SetColor(Rgba::WHITE);
			flowFieldBuilder.PushVertex(position);

			flowFieldBuilder.SetColor(Rgba::WHITE);
			flowFieldBuilder.PushVertex(position + direction*g_unitDistance);
		}
	}
	flowFieldBuilder.End();
	m_flowFieldMesh = flowFieldBuilder.CreateMesh();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitPath()
{
	int pathRadius = g_unitDistance * 2;
	Path path1(Vector2(0,  100),   Vector2(200, 200),  pathRadius);
	Path path2(Vector2(200, 200),  Vector2(400, 400),  pathRadius);
	Path path3(Vector2(400, 400),  Vector2(600, 600),  pathRadius);	
	Path path4(Vector2(600, 600),  Vector2(800, 600),  pathRadius);
	Path path5(Vector2(800, 600),  Vector2(900, 500),  pathRadius);
	Path path6(Vector2(900, 500), Vector2(1200, 300),  pathRadius);
	Path path7(Vector2(1200, 300), Vector2(1400, 200), pathRadius);
	Path path8(Vector2(1400, 200), Vector2(1600, 100), pathRadius);
	Path path9(Vector2(1600, 100), Vector2(1900, 100), pathRadius);



	m_paths.push_back(path1);
	m_paths.push_back(path2);
	m_paths.push_back(path3);
	m_paths.push_back(path4);
	m_paths.push_back(path5);
	m_paths.push_back(path6);
	m_paths.push_back(path7);
	m_paths.push_back(path8);
	m_paths.push_back(path9);


	MeshBuilder pathBuilder;
	pathBuilder.Begin(PRIMITIVE_LINES, false);
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
/*DATE    : 2019/04/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitNobes()
{
	m_cohesionNegativeBounds		= AABB2(Vector2(300, 950), 25, 25);
	m_cohesionPositiveBounds		= AABB2(Vector2(500, 950), 25, 25);
	m_cohesionValueBounds			= AABB2(Vector2(350, 950), 25, 25);
	m_cohesionStringValueBounds		= AABB2(Vector2(50, 950), 25, 25);

	m_seperationNegativeBounds		= AABB2(Vector2(300, 890), 25, 25);
	m_seperationPositiveBounds		= AABB2(Vector2(500, 890), 25, 25);
	m_seperationValueBounds			= AABB2(Vector2(350, 890), 25, 25);
	m_seperationStringValueBounds	= AABB2(Vector2(50, 890), 25, 25);


	m_alignmentNegativeBounds		= AABB2(Vector2(300, 830), 25, 25);
	m_alignmentPositiveBounds		= AABB2(Vector2(500, 830), 25, 25);
	m_alignmentValueBounds		    = AABB2(Vector2(350, 830), 25, 25);
	m_alignmentStringValueBounds    = AABB2(Vector2(50, 830), 25, 25);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitSample()
{
	CreateEntity(Vector2(100, 100), 0.f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateEntity(Vector2 position, float angle)
{
	Entity *entity = new Entity();
	entity->m_position = position;
	entity->m_angle = angle;
	m_entities.push_back(entity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
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
*@purpose : NIL
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetClosestPathIndex(Vector2 position)
{
	int returnIndex = 0;
	float minDistance = 1000;
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
*@purpose : NIL
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
*@purpose : Process all inputs
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
		CreateEntity(mousePosition,0);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_V))
	{
		m_flowFieldVisible = m_flowFieldVisible == true ? false : true;
	}

	ProcessInputForNobes();
}

void Map::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	UpdateCamera(deltaTime);
	ClearDebugEntities();
	//UpdateEntity(deltaTime);


	switch (m_mapType)
	{
	case FLOW_FIELD_BEHAVIOR:
		FollowFlowField();
		break;
	case PATH_FOLLOWING:
		FollowPath();
		break;
	case  SEEK_ARRIVE_BEHAVIOR:
		SeekBehavior();
		ArriveBehavior();
		break;
	case SAMPLE:
		UpdateSample(deltaTime);
		return;
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
*@purpose : Updates camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera(float deltaTime)
{
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateEntity(float deltaTime)
{
	for(int index = 0;index < m_entities.size();index++)
	{
		m_entities.at(index)->Update(deltaTime);
		if(m_entities.at(index)->m_position.x > 1800)
		{
			m_entities.at(index)->m_position.x = -50;
		}
		if (m_entities.at(index)->m_position.y > 1000)
		{
			m_entities.at(index)->m_position.y = -50;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateSample(float deltaTime)
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	Vector2 direction = mousePosition - m_entities.at(0)->m_position;
	direction = direction.GetNormalized();

	float directionAngle = Atan2Degrees(direction.y, direction.x);

	float angleDiff = m_entities.at(0)->m_angle - directionAngle;



	float rotationalDiff = angleDiff;
	if (angleDiff > 180)
	{
		rotationalDiff -= 360;
	}
	if (angleDiff < -180)
	{
		rotationalDiff += 360;
	}


	if (m_start)
	{
		if (rotationalDiff < 0)
		{
			m_entities.at(0)->m_angle += 0.5;
		}
		else
		{
			m_entities.at(0)->m_angle -= 0.5;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/02
*@purpose : NIL
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
		if(m_cohesionNegativeBounds.IsPointInside(mousePosition))
		{
			m_cohesiveForce -= 0.005;
			m_cohesiveForce = ClampFloat(m_cohesiveForce, 0, 5);
		}
		if (m_cohesionPositiveBounds.IsPointInside(mousePosition))
		{
			m_cohesiveForce += 0.005;
			m_cohesiveForce = ClampFloat(m_cohesiveForce, 0, 5);
		}
		if (m_seperationNegativeBounds.IsPointInside(mousePosition))
		{
			m_seperationForce -= 0.005;
			m_seperationForce = ClampFloat(m_seperationForce, 0, 5);
		}
		if (m_seperationPositiveBounds.IsPointInside(mousePosition))
		{
			m_seperationForce += 0.005;
			m_seperationForce = ClampFloat(m_seperationForce, 0, 5);
		}
		if (m_alignmentNegativeBounds.IsPointInside(mousePosition))
		{
			m_alignmentForce -= 0.005;
			m_alignmentForce = ClampFloat(m_alignmentForce, 0, 5);
		}
		if (m_alignmentPositiveBounds.IsPointInside(mousePosition))
		{
			m_alignmentForce += 0.005;
			m_alignmentForce = ClampFloat(m_alignmentForce, 0, 5);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
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
*@purpose : NIL
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

		if(desiredMagnitude < 100)
		{
			float appliedMagnitude = RangeMap(desiredMagnitude, 0, 100, 0, m_entities.at(index)->m_maxSpeed);
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::FollowFlowField()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		Vector2 desiredVelocity;
		Vector2 position = m_entities.at(index)->m_position;
		int xIndex = (position.x / g_unitDistance);
		int yIndex = (position.y / g_unitDistance);

		int flowFieldIndex = yIndex * g_maxWidth + xIndex;
		if(flowFieldIndex < m_flowField.size())
		{
			desiredVelocity = m_flowField.at(flowFieldIndex);
		}
		else
		{
			desiredVelocity = Vector2(1, 0);
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::FollowPath()
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
	int pathIndex = GetClosestPathIndex(mousePosition);
	Vector2 closestPosition = m_paths.at(pathIndex).FindClosestPositionAlongPath(mousePosition);
	m_debugCircles.push_back(DebugCircle(closestPosition,g_unitDistance/4.f,Rgba::GREEN));

	for(int entityIndex = 0;entityIndex < m_entities.size();entityIndex++)
	{
		int pathIndex = GetClosestPathIndex(m_entities.at(entityIndex)->m_position);
		Vector2 position		  = m_entities.at(entityIndex)->m_position;
		//Vector2 predictedPosition = position + m_entities.at(entityIndex)->m_velocity.GetNormalized() * 50;

		Vector2 closestPosition = m_paths.at(pathIndex).FindClosestPositionAlongPath(position);
		closestPosition = closestPosition + m_paths.at(pathIndex).GetDirection() * g_unitDistance * 5;

		m_debugCircles.push_back(DebugCircle(closestPosition,g_unitDistance/4.f,Rgba::RED));

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
*@purpose : Aligns with the group
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::AlignBehavior()
{
	for(int index = 0;index < m_entities.size();index++)
	{
		Vector2 neighbourVelocity(0.f,0.f);
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

		Vector2 desiredVelocity = neighbourVelocity / neighbourCount;
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
*@purpose : Seperate from neighbour
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SeperateBehavior()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		Vector2 neighbourVelocity(0.f, 0.f);
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
		Vector2 desiredVelocity = neighbourVelocity / neighbourCount;
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CohesionBehavior()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		Vector2 neighbourVelocity(0.f, 0.f);
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
		if (neighbourCount <= 1)
		{
			continue;
		}
		Vector2 desiredVelocity = neighbourVelocity / neighbourCount;
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
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawSolidCircle(mousePosition, g_unitDistance / 4.f, Rgba::RED);
	delete defaultMaterial;

	switch (m_mapType)
	{
	case SEEK_ARRIVE_BEHAVIOR:
		break;
	case ALIGNMENT_SEPERATION_COHESION_BEHAVIOR:
		break;
	case FLOW_FIELD_BEHAVIOR:
		RenderFlowField();
		break;
	case PATH_FOLLOWING:
		RenderPaths();
		break;
	default:
		break;
	}
	RenderEntity();
	RenderDebugPoints();
	RenderNobes();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderEntity()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		m_entities.at(index)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderFlowField()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawMesh(m_flowFieldMesh,Matrix44::GetIdentity());
	delete defaultMaterial;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderPaths()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for(int index = 0;index < m_paths.size();index++)
	{
		Path currentPath = m_paths.at(index);
		Vector2 pathDirection = currentPath.GetDirection();
		float   angle		  = Atan2Degrees(pathDirection.y,pathDirection.x);

		Vector2 normalDirection(CosDegrees(angle + 90), SinDegrees(angle + 90));


		Vector2 leftTop     = currentPath.m_start + normalDirection * currentPath.m_radius;
		Vector2 leftBottom  = currentPath.m_start - normalDirection * currentPath.m_radius;

		Vector2 rightTop    = currentPath.m_end + normalDirection * currentPath.m_radius;
		Vector2 rightBottom = currentPath.m_end - normalDirection * currentPath.m_radius;


		Renderer::GetInstance()->DrawSolidRectangle(leftBottom, rightBottom, rightTop, leftTop,Rgba::FADED_BLUE);
		Renderer::GetInstance()->DrawLine(m_paths.at(index).m_start, m_paths.at(index).m_end);
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderDebugPoints()
{
	Material *defaultMaterial = Material::AquireResource("default");
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderNobes()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawAABB(m_cohesionNegativeBounds, Rgba::RED);
	Renderer::GetInstance()->DrawAABB(m_cohesionPositiveBounds, Rgba::GREEN);

	Renderer::GetInstance()->DrawAABB(m_seperationNegativeBounds, Rgba::RED);
	Renderer::GetInstance()->DrawAABB(m_seperationPositiveBounds, Rgba::GREEN);

	Renderer::GetInstance()->DrawAABB(m_alignmentNegativeBounds, Rgba::RED);
	Renderer::GetInstance()->DrawAABB(m_alignmentPositiveBounds, Rgba::GREEN);
	delete defaultMaterial;


	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOn3DPoint(m_cohesionValueBounds.GetCenter(), Vector2(1, 0), Vector2(0, 1), ToString(m_cohesiveForce,2), 12, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(m_seperationValueBounds.GetCenter(), Vector2(1, 0), Vector2(0, 1), ToString(m_seperationForce,2), 12, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(m_alignmentValueBounds.GetCenter(), Vector2(1, 0), Vector2(0, 1), ToString(m_alignmentForce,2), 12, Rgba::WHITE);

	Renderer::GetInstance()->DrawTextOn3DPoint(m_cohesionStringValueBounds.GetCenter(), Vector2(1, 0), Vector2(0, 1),   "COHESIVE", 12, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(m_seperationStringValueBounds.GetCenter(), Vector2(1, 0), Vector2(0, 1), "SEPERATION", 12, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(m_alignmentStringValueBounds.GetCenter(), Vector2(1, 0), Vector2(0, 1),  "ALIGNMENT", 12, Rgba::WHITE);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderSample()
{
	RenderEntity();
}
