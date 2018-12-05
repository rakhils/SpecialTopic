#include "Game/GamePlay/Entity/Player.hpp"

#include "Engine/Net/NetObject.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
//#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Debug/DebugDraw.hpp"

#include "Game/GameCommon.hpp"
// CONSTRUCTOR
Player::Player()
{
	m_playerSnapshot = new PlayerSnapShot_t();
	m_position = Vector2(1050, 550);
	m_localPlayerSnapshot.m_position = m_position;
}

// DESTRUCTOR
Player::~Player()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::SetNetObject(NetObject *netObject)
{
	m_netObject = netObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::Update(float deltaTime)
{
	if (NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		m_playerSnapshot = (PlayerSnapShot_t*)m_netObject->GetLatestReceivedSnapshot();
		ApplySnapshot(m_playerSnapshot);
		return;
	}
	m_playerSnapshot = (PlayerSnapShot_t*)m_netObject->GetLatestReceivedSnapshot();
	ApplySnapshot(m_playerSnapshot);
	if(!(NetSession::GetInstance()->m_hostConnection->m_index == m_index))
	{
		return;
	}
	
	bool keyUpdate = false;

	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_LEFT_ARROW))
	{
		m_localPlayerSnapshot.m_angle += deltaTime * 150;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_RIGHT_ARROW))
	{
		m_localPlayerSnapshot.m_angle -= deltaTime * 150;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_UP_ARROW))
	{
		Vector2 direction(CosDegrees(m_angle), SinDegrees(m_angle));
		m_localPlayerSnapshot.m_position += deltaTime * direction * 150;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_DOWN_ARROW))
	{
		Vector2 direction(CosDegrees(m_angle), SinDegrees(m_angle));
		m_localPlayerSnapshot.m_position -= deltaTime * direction * 150;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_SPACE))
	{
		keyUpdate = true;
	}

	if(keyUpdate)
	{
		SendLocalSnapshot();
	}
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
	g_theRenderer->BindMaterial(defaultMaterial);
	Disc2 disc2D(m_position,50);
	g_theRenderer->DrawCircle(disc2D,m_color);
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void * Player::UpdateAndGetLocalSnapshot()
{
	UpdateLocalPlayerSnapshot();
	return m_playerSnapshot;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::UpdateLocalPlayerSnapshot()
{
	m_playerSnapshot->m_angle     = m_angle;
	m_playerSnapshot->m_position  = m_position;
	m_playerSnapshot->m_primary   = m_primary;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::SendLocalSnapshot()
{
	NetMessage *msg = NetMessage::CreateObjectUpdateMsg(NETOBJ_PLAYER,m_index);
	msg->WriteBytes(4, (void*)&m_localPlayerSnapshot.m_position.x);
	msg->WriteBytes(4, (void*)&m_localPlayerSnapshot.m_position.y);
	msg->WriteBytes(4, (void*)&m_localPlayerSnapshot.m_angle);
	msg->WriteBytes(4, (void*)&m_localPlayerSnapshot.m_primary);

	size_t msgSize = msg->m_bufferSize - 2;
	msg->m_currentWritePosition = 0;
	msg->WriteBytes(2, (char*)&msgSize);

	msg->m_address = &NetSession::GetInstance()->m_hostConnection->m_address;
	std::string bit = msg->GetBitString();
	NetSession::GetInstance()->m_hostConnection->Append(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::ApplySnapshot(PlayerSnapShot_t *playerSnapshot)
{
	if(playerSnapshot == nullptr)
	{
		return;
	}
	m_position  = playerSnapshot->m_position;
	m_angle     = playerSnapshot->m_angle;
	m_primary   = playerSnapshot->m_primary;

	if(NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		DebugDraw::GetInstance()->DebugRenderLogf("HOST IDX %d - POSITION X = %f , Y = %f ", m_index, m_position.x, m_position.y);
		DebugDraw::GetInstance()->DebugRenderLogf("HOST IDX %d - ANGLE = %f", m_index, m_angle);
	}
	else
	{
		DebugDraw::GetInstance()->DebugRenderLogf("CLIENT IDX %d - POSITION X = %f , Y = %f ",m_index,m_position.x, m_position.y);
		DebugDraw::GetInstance()->DebugRenderLogf("CLIENT IDX %d - ANGLE = %f", m_index,m_angle);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void * Player::GetLocalSnapShot()
{
	return m_playerSnapshot;
}
