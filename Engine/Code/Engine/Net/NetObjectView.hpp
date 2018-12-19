#pragma once
#include <stdint.h>

/*\class  : NetObjectView		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/5/2018 2:36:21 AM
* \contact: srsrakhil@gmail.com
*/
class NetObject;
class NetMessage;
class NetObjectView
{

public:
	//Member_Variables
	uint8_t			m_type		= 0;
	uint8_t			m_id		= 0;
	NetObject *		m_netObject = nullptr;
	float			m_age       = 0;
	NetMessage *    m_netMsg    = nullptr;
	//Static_Member_Variables

	//Methods

	NetObjectView(uint8_t m_type,uint8_t id);
	~NetObjectView();
	
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