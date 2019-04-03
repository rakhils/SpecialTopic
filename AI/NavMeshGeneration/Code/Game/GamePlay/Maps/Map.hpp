#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
class Tiles;
struct Triangle
{
	Vector2 position1;
	Vector2 position2;
	Vector2 position3;
};
class Map
{
public:
	bool							m_init		   = false;
	bool							m_gameFinished = false;
	Camera *						m_camera       = nullptr;
	std::vector<Tiles*>				m_tiles;
	MeshBuilder 					m_marchingSquareMeshBuilder;
	Mesh *							m_mesh = nullptr;
	Mesh *							m_meshFinalizedPointList = nullptr;
	std::vector<Vector2>			m_pointList;
	std::vector<Vector2>			m_finalizedPointList;
	std::vector<Triangle>			m_triangles;
	Map();
	~Map();

	void							Initialize();
	void							InitCamera();
	void							InitBlocks();
	void							InitObstacles();

	void							TriangulateCells(Tiles *tile);
	void							GenerateMarchingSquareMesh();
	void							RunRamerDouglasPeuckerAlgorithm(std::vector<Vector2> pointList,int startPosition,int endPosition,int epsilon,std::vector<Vector2> &outPointList);
	void							CreateRamerDouglasPeuckerMesh();
	void							GenerateDelaunayTriangulation();
	void							SortPointListByXY();
	void							PushInitialTriangles();
	void							PushAllTriangleFromPointList();


	void							ProcessInputs(float deltaTime);

	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);

	void							Render();
	void							RenderGrids();
	void							RenderTiles();

};