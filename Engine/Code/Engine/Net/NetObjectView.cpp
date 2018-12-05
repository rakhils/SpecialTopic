#include "Engine/Net/NetObjectView.hpp"

NetObjectView::NetObjectView(uint8_t type, uint8_t id)
{
	m_type = type;
	m_id   = id;
}

// DESTRUCTOR
NetObjectView::~NetObjectView()
{

}
