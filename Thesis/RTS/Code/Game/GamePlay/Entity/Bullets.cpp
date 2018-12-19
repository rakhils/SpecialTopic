#include "Game/GamePlay/Entity/Bullets.hpp"

#include "Engine/Net/NetObject.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Net/NetObjectSystem.hpp"

#include "Game/GameCommon.hpp"

// CONSTRUCTOR

// CONSTRUCTOR
// CONSTRUCTOR
Bullets::Bullets()
{
	m_spawnTime = static_cast<float>(GetCurrentTimeSeconds());
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

	if (!NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		m_bulletSnapshot = (BulletSnapShot_t*)m_netObject->GetLatestReceivedSnapshot();
		ApplySnapshot(m_bulletSnapshot);
		return;
	}
	m_position += deltaTime * m_velocity * Vector2(CosDegrees(m_angle), SinDegrees(m_angle));
	SendLocalSnapshot();
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
	g_theRenderer->BindMaterial(defaultMaterial);

	Disc2 disc2D(m_position, 20);
	g_theRenderer->DrawCircle(disc2D, Rgba::RED);
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullets::SetNetObject(NetObject *netObject)
{
	m_netObject = netObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullets::ApplySnapshot(BulletSnapShot_t *snapShot)
{
	UNUSED(snapShot);
	if (m_bulletSnapshot == nullptr)
	{
		return;
	}
	m_position = m_bulletSnapshot->m_position;
	m_angle    = m_bulletSnapshot->m_angle;
	m_primary  = m_bulletSnapshot->m_primary;

	/*DebugDraw::GetInstance()->DebugRenderLogf("CLIENT IDX %d - POSITION X = %f , Y = %f ", m_index, m_position.x, m_position.y);
	DebugDraw::GetInstance()->DebugRenderLogf("CLIENT IDX %d - ANGLE = %f", m_index, m_angle);*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bullets::SendLocalSnapshot()
{
	NetMessage *msg = NetMessage::CreateObjectUpdateMsg(NETOBJ_BULLET, m_index);
	msg->WriteBytes(4, (void*)&m_position.x);
	msg->WriteBytes(4, (void*)&m_position.y);
	msg->WriteBytes(4, (void*)&m_angle);
	msg->WriteBytes(4, (void*)&m_localBulletSnapshot.m_primary);

	size_t msgSize = msg->m_bufferSize - 2;
	msg->m_currentWritePosition = 0;
	msg->WriteBytes(2, (char*)&msgSize);

	NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[m_index]->UpdateAgeInAllNetObjectViews();

	msg->m_address = &NetSession::GetInstance()->m_hostConnection->m_address;
	std::string bit = msg->GetBitString();

	NetSession::GetInstance()->BroadcastMsg(msg,CONNECTION_READY);
}
