#pragma once
#include <string>
#include "Engine/Core/BytePacker.hpp"
/*\class  : NetMessage	   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0	   
* \date   : 9/16/2018 7:32:44 PM
* \contact: srsrakhil@gmail.com
*/
class NetConnection;
class NetMessage : public BytePacker
{

public:
	//Member_Variables
	//char m_localBuffer[ETHERNET_MTU];
	//NetMessageDefinition* m_definition;
	uint8_t				  m_definitionIndex;
	std::string			  m_definitionName;
	int					  m_connectionIndex;
	//Static_Member_Variables

	//Methods

	NetMessage(std::string cmd);
	~NetMessage();

	void WriteCommandIndex();
	//Static_Methods
	static NetMessage * CreateAddMessage(float value1, float value2);
	static NetMessage * CreatePingMessage(std::string msg);
	static NetMessage * CreateHeartBeatMessage();
	static NetMessage * CreateHeartBeatMessage(NetConnection *connection);
	static NetMessage * CreateBadMessage();
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