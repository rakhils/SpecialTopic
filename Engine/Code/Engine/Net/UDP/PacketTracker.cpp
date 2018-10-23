#include "Engine/Net/UDP/PacketTracker.hpp"
#include "Engine/Core/Time.hpp"
// CONSTRUCTOR
PacketTracker::PacketTracker(uint16_t ack)
{
	m_ackID = ack;
	m_sendTime = static_cast<float>(GetCurrentTimeSeconds());
}

// DESTRUCTOR
PacketTracker::~PacketTracker()
{

}
