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
};
class NetMessage
{

public:
	//Member_Variables

	char *     m_data;
	BytePacker m_bytePacker;
	uint16_t   m_index;
	size_t	   m_bytesRead;
	size_t	   m_totalBytes;
	size_t	   m_totalChunks;
	//Static_Member_Variables

	//Methods

	NetMessage();
	~NetMessage();
	
	void WriteData(float value);
	void WriteData(std::string value);
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