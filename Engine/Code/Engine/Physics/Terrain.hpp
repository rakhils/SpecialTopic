#pragma once
#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Physics/Raycast3D.hpp"
/*\class  : Terrain		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/11/2018 8:17:54 PM
* \contact: srsrakhil@gmail.com
*/
class Mesh;
class Renderable;
class Terrain
{

public:
	//Member_Variables
	AABB2	m_extents;
	Vector2 m_cellSize;
	Vector2 m_dimensions;
	MeshBuilder m_meshBuilder;
	float m_minHeight;
	float m_maxHeight;
	Vector2 m_chunkDimensions;
	std::vector<Renderable*> m_renderables;
	std::vector<float> m_heights;
	//Static_Member_Variables
	void		LoadFromImage(Image *image, AABB2 const &extents, float min_height, float max_height, Vector2 chunk_counts);
	void		CreateTerrainChunk(Vector2 startPosition, Vector2 chunkStart);
	float		GetHeight(Vector2 position);
	float		GetHeightAtDiscreteCordinate(IntVector2 cords);
	float		GetHeightAtDiscreteCordinate(int index);
	Vector3		GetPositionAtDiscreteCordinate(IntVector2 cords);
	Vector3		GetNormalAtDiscreteCordinate(IntVector2 cords);
	Vector3		GetNormalAtXZ(Vector2 cords);
	Vector3		GetTangentAtXZ(Vector2 cords);
	bool		GetQuadFromWorldPosition(Vector2 position,Vector3* positions);
	RaycastHit	Raycast(GameObject *object,Ray &ray);
	//Methods

	Terrain();
	~Terrain();

	
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};