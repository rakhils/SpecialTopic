#include "Engine/Physics/Terrain.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collider/Collider.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Debug/DebugDraw.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Terrain::Terrain()
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : Loads terrain from height map
*@param   : Image file,World scale,Min and max heigth chunk counts for total terrain
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Terrain::LoadFromImage(Image *image, AABB2 const &extents, float min_height, float max_height, Vector2 chunk_counts)
{
	m_extents		  = extents;
	m_minHeight		  = min_height;
	m_maxHeight		  = max_height;
	m_chunkDimensions = chunk_counts;
	m_meshBuilder.Begin(PRIMITIVE_TRIANGES, true);
	
	for(int index = 0;index < image->texture->texels.size();index++)
	{
		Rgba color = image->texture->texels.at(index);
		float height = color.GetAsFloats().GetAverageValue();
		height = RangeMapFloat(height,0.f,1.f,m_minHeight, m_maxHeight);
		//height = 0;
		m_heights.push_back(height);
	}
	m_dimensions = image->GetDimensions().GetAsVector2();
	m_cellSize.x = m_dimensions.x / extents.GetDimensions().x;
	m_cellSize.y = m_dimensions.y / extents.GetDimensions().y;
	
	Vector2 chunkDimension(m_dimensions.x / chunk_counts.x, m_dimensions.y/chunk_counts.y);
	Vector3 startWorldPosition = Vector3::ZERO;
	for (float chunkIndexY = 0; chunkIndexY < m_dimensions.y;)
	{
		for(float chunkIndexX = 0;chunkIndexX < m_dimensions.x;)
		{
			Vector2 startChunkPosition(chunkIndexX, chunkIndexY);
			CreateTerrainChunk(startWorldPosition.GetXY(), startChunkPosition);
			startWorldPosition.x += m_cellSize.x*chunkDimension.x;
			chunkIndexX += chunkDimension.x;
		}
		chunkIndexY += chunkDimension.y;
		startWorldPosition.x = 0;
		startWorldPosition.y += m_cellSize.y*chunkDimension.y;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : Creates one chunk
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Terrain::CreateTerrainChunk(Vector2 startPosition, Vector2 chunkStart)
{
	Vector2 tempStartPosition = startPosition;
	Vector2 chunkDimension(m_dimensions.x / m_chunkDimensions.x, m_dimensions.y/m_chunkDimensions.y);
	for(float indexY = 0;indexY < chunkDimension.y;indexY++)
	{
		for(float indexX = 0;indexX < chunkDimension.x;indexX++)
		{
			float height = GetHeightAtDiscreteCordinate(Vector2(chunkStart.x + indexX, chunkStart.y + indexY).ToIntVector2());
			Vector3 position(startPosition.x, height, startPosition.y);
			Vector2 UV(indexX, indexY);
			Vector3 normal = GetNormalAtDiscreteCordinate(Vector2(chunkStart.x + indexX, chunkStart.y + indexY).ToIntVector2());
			m_meshBuilder.SetColor(Rgba::WHITE);
			m_meshBuilder.SetUV(UV);
			m_meshBuilder.SetNormal(normal);
			m_meshBuilder.PushVertex(position);

			startPosition.x += m_cellSize.x;
		}
		startPosition.x = tempStartPosition.x;
		startPosition.y += m_cellSize.y;
	}

	for (int z = 0; z < (static_cast<int>(chunkDimension.y) - 1); z++)
	{
		for (int x = 0; x < (static_cast<int>(chunkDimension.x) - 1); x++)
		{
			int ll = static_cast<int>(chunkDimension.y) * z + x;
			int lr = ll + 1;

			int ul = ll + static_cast<int>(chunkDimension.y);
			int ur = ul + 1;
			m_meshBuilder.AddQuadIndex(ll, lr, ul, ur);
		}
	}
	m_meshBuilder.End();
	Mesh* mesh = m_meshBuilder.CreateMesh<Vertex_3DPCUN>();
	m_meshBuilder.Reset();
	Renderable *renderable = new Renderable();
	renderable->m_name = "Terrain";
	renderable->SetMesh(mesh);
	renderable->SetMaterial(Material::AquireResource("Data\\Materials\\Terrain.mat"));
	//renderable->m_material->m_textures.at(0) = Texture::CreateOrGetTexture("Data\\Images\\terrain_grass.jpg");
	m_renderables.push_back(renderable);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : Gets continous height
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Terrain::GetHeight(Vector2 position)
{
	int index1 = static_cast<int>((static_cast<int>(position.y)		* m_dimensions.x + static_cast<int>(position.x)));
	int index2 = static_cast<int>((static_cast<int>(position.y)		* m_dimensions.x + static_cast<int>(position.x + 1)));
	int index3 = static_cast<int>((static_cast<int>(position.y + 1) * m_dimensions.x + static_cast<int>(position.x)));
	int index4 = static_cast<int>((static_cast<int>(position.y + 1) * m_dimensions.x + static_cast<int>(position.x + 1)));


	float height00	= GetHeightAtDiscreteCordinate(static_cast<int>(index1));
	float height01	= GetHeightAtDiscreteCordinate(static_cast<int>(index2));
	float height10	= GetHeightAtDiscreteCordinate(static_cast<int>(index3));
	float height11 = GetHeightAtDiscreteCordinate(static_cast<int>(index4));


	float dx	 = Interpolate(height00, height01, GetFraction(position.x));
	float dy	 = Interpolate(height10, height11, GetFraction(position.x));
	float height = Interpolate(dx, dy, GetFraction(position.y));
	return height;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Terrain::GetHeightAtDiscreteCordinate(IntVector2 cords)
{
	int idx = cords.y * static_cast<int>(m_dimensions.x) + cords.x;
	if(idx < m_heights.size())
	{
		return m_heights[idx];
	}
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Terrain::GetHeightAtDiscreteCordinate(int index)
{
	int Xpos = index % static_cast<int>(m_dimensions.x);
	int Ypos = index / static_cast<int>(m_dimensions.y);
	return GetHeightAtDiscreteCordinate(IntVector2(Xpos, Ypos));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Terrain::GetPositionAtDiscreteCordinate(IntVector2 cords)
{
	Vector2 worldXZ = m_extents.mins;
	Vector2 xz = worldXZ + cords.GetAsVector2() * m_cellSize;

	float height = GetHeightAtDiscreteCordinate(cords);

	return Vector3(xz.x, height, xz.y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Terrain::GetNormalAtDiscreteCordinate(IntVector2 cords)
{
	Vector3 du			= GetPositionAtDiscreteCordinate(cords + IntVector2(1, 0)) - GetPositionAtDiscreteCordinate(cords - IntVector2(1, 0));
	Vector3 dv			= GetPositionAtDiscreteCordinate(cords + IntVector2(0, 1)) - GetPositionAtDiscreteCordinate(cords - IntVector2(0, 1));
	Vector3 tangent		= du.GetNormalized();
	Vector3 bitangent	= dv.GetNormalized();
	Vector3 normal		= CrossProduct(bitangent, tangent);
	return normal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Terrain::GetNormalAtXZ(Vector2 cords)
{
	Vector3 normalAt00 = GetNormalAtDiscreteCordinate(cords.ToIntVector2());
	Vector3 normalAt01 = GetNormalAtDiscreteCordinate(IntVector2(static_cast<int>(cords.x + 1), static_cast<int>(cords.y	)));
	Vector3 normalAt10 = GetNormalAtDiscreteCordinate(IntVector2(static_cast<int>(cords.x	 ), static_cast<int>(cords.y + 1)));
	Vector3 normalAt11 = GetNormalAtDiscreteCordinate(IntVector2(static_cast<int>(cords.x + 1), static_cast<int>(cords.y + 1)));

	Vector3 normalXBottom = Interpolate(normalAt00, normalAt01, GetFraction(cords.x));
	Vector3 normalXTop	  = Interpolate(normalAt10, normalAt11, GetFraction(cords.x));
	Vector3 normal		  = Interpolate(normalXBottom, normalXTop, GetFraction(cords.y));
	return normal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Gets tangent at xz cords
*@param   : XZ cords
*@return  : tangent
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Terrain::GetTangentAtXZ(Vector2 cords)
{
	Vector3 du = GetPositionAtDiscreteCordinate(cords.ToIntVector2() + IntVector2(1, 0)) - GetPositionAtDiscreteCordinate(cords.ToIntVector2() - IntVector2(1, 0));
	Vector3 dv = GetPositionAtDiscreteCordinate(cords.ToIntVector2() + IntVector2(0, 1)) - GetPositionAtDiscreteCordinate(cords.ToIntVector2() - IntVector2(0, 1));
	Vector3 tangent = du.GetNormalized();
	return tangent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/19
*@purpose : Get the index of the quad from world position
*@param   : World Pos
*@return  : Index
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Terrain::GetQuadFromWorldPosition(Vector2 position,Vector3* positions)
{
	UNUSED(positions);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/12
*@purpose : Raycast in the terrain and gets the raycast hit result
*@param   : Ray constain start,direction,max length
*@return  : Hit result
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RaycastHit Terrain::Raycast(GameObject *gameobject,Ray &ray)
{
	Vector3		currentPosition = ray.m_start;
	RaycastHit  result;
	for(float rayDistance = 0;(rayDistance + ray.m_stepSize)< ray.m_maxDistance;rayDistance += ray.m_stepSize)
	{
		Vector3 nextPosition = ray.Evaluate(rayDistance + ray.m_stepSize);
		if(gameobject != nullptr)
		{
			Collider *collider = Raycast3D::GetColliderAtPosition(gameobject, nextPosition);
			if (collider != nullptr)
			{
				result.m_hit = true;
				result.m_position = ray.Evaluate(rayDistance);
				result.m_collider = collider;
				DebugDraw::GetInstance()->DebugRenderLogf(Rgba::RED,"GAMEOBJECT HIT NAME %s", collider->m_gameObject->m_name.c_str());
				return result;

			}
		}
		
		float height = GetHeight(nextPosition.GetXZ());
		if(height > nextPosition.y)
		{
			float stepSize = ray.m_stepSize / 2.f;
			while (stepSize > (ray.m_stepSize / 16))
			{
				nextPosition = ray.Evaluate(rayDistance + stepSize);
				height		 = GetHeight(nextPosition.GetXZ());
				if(height > nextPosition.y)
				{
					stepSize    = stepSize / 2.f;
					rayDistance = rayDistance + stepSize;
				}
				else
				{
					rayDistance = rayDistance + stepSize;
					stepSize    = stepSize / 2.f;
				}
			}
			result.m_hit = true;
			result.m_position = ray.Evaluate(rayDistance);
			return result;
		}
		currentPosition = nextPosition;
	}
	result.m_hit = false;
	return result;
}

// DESTRUCTOR
Terrain::~Terrain()
{

}
