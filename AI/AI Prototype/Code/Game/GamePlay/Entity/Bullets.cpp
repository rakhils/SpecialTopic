#include "Game/GamePlay/Entity/Bullets.hpp"

#include "Engine/Net/NetObject.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Net/NetObjectSystem.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"

#include "Game/GameCommon.hpp"


// CONSTRUCTOR
Bullets::Bullets(Vector2 position,float angle,float health)
{
	m_position	 = position;
	m_angle		 = angle;
	m_health	 = health;
	m_bulletMesh = MeshBuilder::Create3DPlane<Vertex_3DPCU>(Vector3::ZERO, Vector3::RIGHT, Vector3::UP, Vector2(g_bulletWidth, g_bulletHeight), Rgba::WHITE, FILL_MODE_FILL);
	m_spawnTime  = static_cast<float>(GetCurrentTimeSeconds());
}

// DESTRUCTOR
Bullets::~Bullets()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullets::Update(float deltaTime)
{
	if(m_markAsDead)
	{
		return;
	}

	m_position += deltaTime * m_velocity * Vector2(CosDegrees(m_angle), SinDegrees(m_angle));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullets::Render()
{
	if(m_markAsDead)
	{
		return;
	}

	Material *defaultMaterial = Material::AquireResource("default");
	Texture *texture = Texture::CreateOrGetTexture("Data//Images//bullet.png");
	defaultMaterial->m_textures.at(0) = texture;
	Texture::SetCurrentTexture(texture);
	g_theRenderer->BindMaterial(defaultMaterial);

	Matrix44 modelMatrix;
	modelMatrix.Translate2D(m_position);
	modelMatrix.RotateDegrees2D(m_angle);

	g_theRenderer->DrawMesh(m_bulletMesh, modelMatrix);
	delete defaultMaterial;
}
