#include "Game/GamePlay/Entity/Player.hpp"

#include "Engine/Net/NetObject.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
//#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Net/NetObjectSystem.hpp"
#include "Engine/Net/NetObjectConnectionView.hpp"
#include "Engine/Net/NetObjectView.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
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
	bool isTurning = false;
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_LEFT_ARROW))
	{
		m_localPlayerSnapshot.m_angle += deltaTime * 50;
		isTurning = true;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_RIGHT_ARROW))
	{
		m_localPlayerSnapshot.m_angle -= deltaTime * 50;
		isTurning = true;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_UP_ARROW) && !isTurning)
	{
		Vector2 direction(CosDegrees(m_angle), SinDegrees(m_angle));
		m_localPlayerSnapshot.m_position += deltaTime * direction * 50;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::GetInstance()->KEYBOARD_DOWN_ARROW) && !isTurning)
	{
		Vector2 direction(CosDegrees(m_angle), SinDegrees(m_angle));
		m_localPlayerSnapshot.m_position -= deltaTime * direction * 50;
		keyUpdate = true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_SPACE))
	{
		m_localPlayerSnapshot.m_primary++;
		keyUpdate = true;
	}
	if(keyUpdate)
	{
		//m_localPlayerSnapshot.m_primary++;
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
	g_theRenderer->DrawCircle(disc2D,Rgba::RED);
	g_theRenderer->DrawLine(m_position, m_position + Vector2(CosDegrees(m_angle + 10), SinDegrees(m_angle + 10)) * 50);
	g_theRenderer->DrawLine(m_position, m_position + Vector2(CosDegrees(m_angle - 10), SinDegrees(m_angle - 10)) * 50);
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
	float temp  = m_primary;
	m_position  = playerSnapshot->m_position;
	m_angle     = playerSnapshot->m_angle;
	m_primary   = playerSnapshot->m_primary;

	if (m_playerSnapshot->m_primary > temp)
	{
		Game::GetInstance()->CreateBullet(Game::GetInstance()->GetUniqueBulletID(), m_position, m_angle);
	}
	/*if(NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		DebugDraw::GetInstance()->DebugRenderLogf("HOST IDX %d - POSITION X = %f , Y = %f ", m_index, m_position.x, m_position.y);
		DebugDraw::GetInstance()->DebugRenderLogf("HOST IDX %d - ANGLE = %f", m_index, m_angle);\
		DebugDraw::GetInstance()->DebugRenderLogf("HOST IDX %d - FIRE COUNT = %f", m_index, m_primary);
	}
	else
	{
		DebugDraw::GetInstance()->DebugRenderLogf("CLIENT IDX %d - POSITION X = %f , Y = %f ",m_index,m_position.x, m_position.y);
		DebugDraw::GetInstance()->DebugRenderLogf("CLIENT IDX %d - ANGLE = %f", m_index,m_angle);
		DebugDraw::GetInstance()->DebugRenderLogf("CLIENT IDX %d - FIRE COUNT = %f", m_index, m_localPlayerSnapshot.m_primary);
	}*/
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
