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

Map::Map()
{
	Initialize();
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
	InitBlocks();
	InitObstacles();
	GenerateMarchingSquareMesh();
	//CreateRamerDouglasPeuckerMesh();
	//GenerateDelaunayTriangulation();
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
/*DATE    : 2019/03/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitBlocks()
{
	for(int tileIndex = 0;tileIndex < g_maxWidth * g_maxHeight;tileIndex++)
	{
		m_tiles.push_back(new Tiles());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitObstacles()
{
	Texture *texture = Texture::CreateOrGetTexture("Data\\Images\\Map.png",true,false);

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
	

	std::vector<ControlNode> controlNodes;

	for (int indexY = 0; indexY < g_maxHeight; indexY++)
	{
		for (int indexX = 0; indexX < g_maxWidth; indexX++)
		{
			int index = indexY * (g_maxWidth) + indexX;
			bool isActive = false;
			if(m_tiles.at(index)->m_type == BLOCK)
			{
				isActive = true;
			}
			
			Vector2 position(indexX * g_unitDistance + g_unitDistance /2.f,indexY * g_unitDistance + g_unitDistance/2.f);
			ControlNode controlNode(position, isActive, g_unitDistance); 
			controlNodes.push_back(controlNode);
		}
	}

	for (int indexY = 0; indexY < g_maxHeight -1; indexY++)
	{
		for (int indexX = 0; indexX < g_maxWidth -1; indexX++)
		{
			int index = indexY * (g_maxWidth) + indexX;

			int index1 = indexY			* (g_maxWidth)+  indexX;
			int index2 = indexY			* (g_maxWidth)+ (indexX + 1);
			int index3 = (indexY + 1)	* (g_maxWidth)+  indexX;
			int index4 = (indexY + 1)	* (g_maxWidth)+ (indexX + 1);

			m_tiles.at(index)->SetNodes(&controlNodes.at(index1), &(controlNodes.at(index2)),
										&controlNodes.at(index3), &(controlNodes.at(index4)));
		}
	}


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::GenerateMarchingSquareMesh()
{
	m_marchingSquareMeshBuilder.Begin(PRIMITIVE_TRIANGES, false);
	for (int indexY = 0; indexY < g_maxHeight; indexY++)
	{
		for (int indexX = 0; indexX < g_maxWidth; indexX++)
		{
			int index = indexY * g_maxWidth + indexX;
			TriangulateCells(m_tiles.at(index));
		}
	}
	m_marchingSquareMeshBuilder.End();
	m_mesh = m_marchingSquareMeshBuilder.CreateMesh();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RunRamerDouglasPeuckerAlgorithm(std::vector<Vector2> pointList, int startPosition, int endPosition,int epsilonValue,std::vector<Vector2> &outPointList)
{
	float maxDistance = 0.f;
	int   savedIndex = 0;
	for (int index = startPosition + 1; index < endPosition - 1; index++)
	{
		float distance = GetShortestDistanceBetweenLineAndPoint(pointList.at(startPosition), pointList.at(endPosition), pointList.at(index));
		if(distance > maxDistance)
		{
			maxDistance = distance;
			savedIndex = index;
		}
	}
	if(maxDistance > epsilonValue)
	{
		std::vector<Vector2> recResults1;
		std::vector<Vector2> recResults2;

		RunRamerDouglasPeuckerAlgorithm(pointList,startPosition + 1,savedIndex, epsilonValue, recResults1);
		RunRamerDouglasPeuckerAlgorithm(pointList,savedIndex,endPosition -1, epsilonValue,  recResults2);

		outPointList.assign(recResults1.begin(), recResults1.end()-1);
		outPointList.insert(outPointList.end(), recResults2.begin(), recResults2.end());
	}
	else
	{
		outPointList.clear();
		outPointList.push_back(pointList.at(startPosition));
		outPointList.push_back(pointList.at(endPosition));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateRamerDouglasPeuckerMesh()
{
	std::vector<Vector2> outPoints;
	for(int index = 0;index < m_marchingSquareMeshBuilder.m_vertices.size();index++)
	{
		m_pointList.push_back(m_marchingSquareMeshBuilder.m_vertices.at(index).m_position.GetXY());
	}
	RunRamerDouglasPeuckerAlgorithm(m_pointList, 0, m_pointList.size()-1, 1.f, outPoints);

	MeshBuilder builder;
	builder.Begin(PRIMITIVE_POINTS, false);
	for (int index = 0; index < outPoints.size();index++)
	{
		builder.SetColor(Rgba::GREEN);
		builder.PushVertex(Vector3(outPoints.at(index)));
	}
	builder.End();
	m_meshFinalizedPointList = builder.CreateMesh();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::GenerateDelaunayTriangulation()
{
	SortPointListByXY();
	PushInitialTriangles();

	int a = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SortPointListByXY()
{
	for (int indexI = 0; indexI < m_pointList.size(); indexI++)
	{
		for (int indexJ = 0; indexJ < m_pointList.size(); indexJ++)
		{
			Vector2 pointOne = m_pointList.at(indexI);
			Vector2 pointTwo = m_pointList.at(indexJ);

			if (pointOne.x < pointTwo.x)
			{
				m_pointList.at(indexJ) = pointOne;
				m_pointList.at(indexI) = pointTwo;
				continue;
			}
			if (pointOne.x == pointTwo.x)
			{
				if (pointOne.y < pointTwo.y)
				{
					m_pointList.at(indexJ) = pointOne;
					m_pointList.at(indexI) = pointTwo;
					continue;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::PushInitialTriangles()
{
	Triangle triangle;
	triangle.position1 = m_pointList.at(0);
	triangle.position2 = m_pointList.at(1);
	triangle.position3 = m_pointList.at(2);

	m_triangles.push_back(triangle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::PushAllTriangleFromPointList()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::TriangulateCells(Tiles *tile)
{
	Rgba color = Rgba::WHITE;
	switch (tile->m_configurationNumber)
	{
	case 0:
		break;
	case 1:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		break;
	case 2:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		break;
	case 4:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		break;
	case 8:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		break;
		// 2 point cases
	case 3:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		break;
	case 6:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		break;

	case 9:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		break;

	case 12:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		break;

		// 4 triangles 

	case 10:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		break;

	case 5:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		break;

	case 7:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		break;

	case 11:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreTop()));
		break;

	case 13:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		break;

	case 14:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetCentreBottom()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		break;

	case 15:
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomLeft()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));

		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetBottomRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopRight()));
		m_marchingSquareMeshBuilder.SetColor(color);
		m_marchingSquareMeshBuilder.PushVertex(Vector3(tile->GetTopLeft()));
		break;
	}
	
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
}

void Map::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	UpdateCamera(deltaTime);
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

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	
	RenderGrids();
	RenderTiles();
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	//Renderer::GetInstance()->DrawMesh(m_mesh,Matrix44::GetIdentity());

	Renderer::GetInstance()->DrawMesh(m_mesh, Matrix44::GetIdentity());
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/22
*@purpose : render grids
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderGrids()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);


	for (float x = 0; x < static_cast<float>(g_maxWidth*g_unitDistance); x += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(x, 0.f, 0.f), Vector3(x, static_cast<float>(g_maxHeight*g_unitDistance), 0),Rgba::BLUE);
	}
	for (float y = 0; y < static_cast<float>(g_maxHeight* g_unitDistance); y += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(0.f, y, 0.f), Vector3(g_maxWidth* g_unitDistance, y, 0.f),Rgba::BLUE);
	}

	g_theRenderer->DrawLine(Vector3(0.f, 0.f, 0.f), Vector3(0.f,static_cast<float>(g_maxHeight*g_unitDistance), 0));
	g_theRenderer->DrawLine(Vector3(0.f, 0.f, 0.f), Vector3(static_cast<float>(g_maxWidth*g_unitDistance), 0.f, 0));

	g_theRenderer->DrawLine(Vector3(0.f, static_cast<float>(g_maxHeight*g_unitDistance), 0),Vector3(static_cast<float>(g_maxWidth*g_unitDistance),static_cast<float>(g_maxHeight)*g_unitDistance,0.f));
	g_theRenderer->DrawLine(Vector3(static_cast<float>(g_maxWidth*g_unitDistance),static_cast<float>(g_maxHeight)*g_unitDistance,0.f), Vector3(static_cast<float>(g_maxWidth)*g_unitDistance, 0.f,0.f));


	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderTiles()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for(int tileIndex = 0;tileIndex < m_tiles.size();tileIndex++)
	{
		if(m_tiles.at(tileIndex)->m_type == BLOCK)
		{
			g_theRenderer->DrawAABB(m_tiles.at(tileIndex)->m_aabb2, Rgba::BLUE);
		}
	}
	delete defaultMaterial;
}
