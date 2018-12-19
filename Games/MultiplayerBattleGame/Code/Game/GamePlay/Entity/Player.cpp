#include "Game/GamePlay/Entity/Player.hpp"

#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetObject.hpp"
#include "Engine/Net/NetObjectSystem.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetObjectConnectionView.hpp"
#include "Engine/Net/NetObjectView.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Mesh/Mesh.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

Player::Player(Vector2 position) : Entity(position,PLAYER)
{
	m_entityMesh = MeshBuilder::Create3DPlane<Vertex_3DPCU>(Vector3::ZERO, Vector3::RIGHT, Vector3::UP, Vector2(g_playerWidth, g_playerHeight),Rgba::WHITE,FILL_MODE_FILL);

}

// DESTRUCTOR
Player::~Player()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : Calculates bullet spawn position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Player::GetBulletSpawnPosition()
{
	return (m_position + Vector2(CosDegrees(m_angle), SinDegrees(m_angle)) * g_playerWidth * 1.5f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::SetPosition(Vector2 position)
{
	m_position = position;
	m_localEntitySnapshot->m_position.x = position.x;
	m_localEntitySnapshot->m_position.y = position.y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::SetDirection(float angle)
{
	m_angle = angle;
	m_localEntitySnapshot->m_angle = angle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::ProcessInputs(float deltaTime)
{
	if (NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		return;
	}
	if (!IsCurrentPlayer())
	{
		return;
	}
	bool keyUpdate = false;
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_LEFT_ARROW))
	{
		m_localEntitySnapshot->m_angle += deltaTime * m_multiplier;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_RIGHT_ARROW))
	{
		m_localEntitySnapshot->m_angle -= deltaTime * m_multiplier;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_UP_ARROW))
	{
		Vector2 direction(CosDegrees(m_angle), SinDegrees(m_angle));
		Vector2 predictedPosition = m_localEntitySnapshot->m_position + deltaTime * direction * m_multiplier;
		if(!(Game::GetInstance()->IsPositionOutOfBounds(predictedPosition)))
		{
			m_localEntitySnapshot->m_position += deltaTime * direction * m_multiplier;
			keyUpdate = true;
		}
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_DOWN_ARROW))
	{
		Vector2 direction(CosDegrees(m_angle), SinDegrees(m_angle));
		Vector2 predictedPosition = m_localEntitySnapshot->m_position - deltaTime * direction * m_multiplier;
		if (!(Game::GetInstance()->IsPositionOutOfBounds(predictedPosition)))
		{
			m_localEntitySnapshot->m_position -= deltaTime * direction * m_multiplier;
			keyUpdate = true;
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_SPACE))
	{
		SendCreateBulletMsg();
	}
	if(keyUpdate)
	{
		SendPlayerSnapshot();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	ApplySnapshot((EntitySnapShot_t*)m_netObject->GetLatestReceivedSnapshot());
	//DebugDraw::GetInstance()->DebugRenderLogf(deltaTime,"angle %f " , (m_angle));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::Render()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Texture *texture = Texture::CreateOrGetTexture("Data//Images//fighter.png");
	defaultMaterial->m_textures.at(0) = texture;
	Texture::SetCurrentTexture(texture);
	g_theRenderer->BindMaterial(defaultMaterial); 

	Matrix44 modelMatrix;
	modelMatrix.Translate2D(m_position);
	modelMatrix.RotateDegrees2D(m_angle);

	g_theRenderer->DrawMesh(m_entityMesh,modelMatrix);
	delete defaultMaterial;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::SendPlayerSnapshot()
{
	NetMessage *msg = NetMessage::CreateObjectUpdateMsg(NETOBJ_PLAYER,m_index);
	msg->WriteBytes(4, (void*)&m_localEntitySnapshot->m_position.x);
	msg->WriteBytes(4, (void*)&m_localEntitySnapshot->m_position.y);
	msg->WriteBytes(4, (void*)&m_localEntitySnapshot->m_angle);
	msg->WriteBytes(4, (void*)&m_localEntitySnapshot->m_health);

	size_t msgSize = msg->m_bufferSize - 2;
	msg->m_currentWritePosition = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	msg->m_address = &NetSession::GetInstance()->m_hostConnection->m_address;

	NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[m_index]->UpdateAgeInAllNetObjectViews();
	NetObjectView *netObjectView = NetSession::GetInstance()->m_hostConnection->m_netObjectConnectionView->GetNetObjectView(m_index);
	/*if(netObjectView->m_netMsg != nullptr)
	{
		delete netObjectView->m_netMsg;
		netObjectView->m_netMsg = nullptr;
	}*/
	netObjectView->m_netMsg = msg;
	NetSession::GetInstance()->m_hostConnection->SortAndPushIntoNetObjectViews(netObjectView);
	std::string bit = msg->GetBitString();

	//NetSession::GetInstance()->m_hostConnection->Append(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::SendCreateBulletMsg()
{
	uint8_t networkID = 0;
	NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_BULLET,networkID,NetSession::GetInstance()->m_hostConnection->m_index);
	//DevConsole::GetInstance()->PushToOutputText("CREATE BULLET REL ID " + ToString(msg->m_reliableID), Rgba::GREEN);

	Vector2 bulletSpawnPosition = GetBulletSpawnPosition();
	float bulletHealth = 1;

	msg->WriteBytes(4, (void*)&bulletSpawnPosition.x);
	msg->WriteBytes(4, (void*)&bulletSpawnPosition.y);
	msg->WriteBytes(4, (void*)&m_angle);
	msg->WriteBytes(1, (void*)&m_index);

	size_t msgSize = msg->m_bufferSize - 2;
	msg->m_currentWritePosition = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	msg->SetAddress(&NetSession::GetInstance()->m_hostConnection->m_address);

	NetSession::GetInstance()->m_hostConnection->Append(msg);

}
