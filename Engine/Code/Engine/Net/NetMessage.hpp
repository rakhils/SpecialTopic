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
/*
struct UDPData
{
	BytePacker m_bytePacker;
	uint16_t   m_index;
	size_t	   m_bytesRead;
	size_t	   m_totalBytes;
	UDPData()
	{
		m_bytePacker.SetEndianess(LITTLE_ENDIAN);
	}
};*/

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