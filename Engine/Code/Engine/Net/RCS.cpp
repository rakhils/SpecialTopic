#include "Engine/Net/RCS.hpp"
#include "Engine/Net/TCP/TCPServer.hpp"
#include "Engine/Net/TCP/TCPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/System/Thread/Thread.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Net/TCP/TCPServer.hpp"
#include "Engine/Core/EngineCommon.hpp"

RCS * RCS::s_theRCS = nullptr;
// CONSTRUCTOR
RCS::RCS()
{
	m_state = RCS_STATE_CLIENT;
}

// DESTRUCTOR
RCS::~RCS()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : inits RCS
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::Initialize()
{
	m_ipaddress = NetAddress::GetIP();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Joins the session
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RCS::Join()
{
	TCPSocket *tcpSocket = new TCPSocket();
	tcpSocket->m_blocking = false;
	if(tcpSocket->Connect((char*)m_ipaddress.c_str(),m_rcsPort))
	{
		m_state = RCS_STATE_CLIENT;
		m_tcpSocketArray.push_back(tcpSocket);
		return true;
	}
	m_state = RCS_STATE_CLIENT_FAILED;
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : Hosts a server 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RCS::Host()
{
	m_tcpServer = new TCPServer();
	m_tcpServer->m_port = m_rcsPort;
	Thread::ThreadCreateAndDetach("net_listen", TCPServer::ListenOnThread,((void *)m_tcpServer));
	m_state = RCS_STATE_HOST;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : Updates RCP
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::Update(float deltaTime)
{
	switch (m_state)
	{

	case RCS_STATE_CLIENT:
		if(m_tcpSocketArray.size() == 0)
		{
			Join();
			break;
		}
		if(m_tcpSocketArray.size() == 1)
		{
			TCPSocket *tcpSocket = m_tcpSocketArray.at(0);
			if(tcpSocket->m_isDisconnected)
			{
				m_tcpSocketArray.erase(m_tcpSocketArray.begin(), m_tcpSocketArray.begin() + 1);
			}
		}
		break;
	case RCS_STATE_CLIENT_FAILED:
		Host();
		break;
	case RCS_STATE_CLIENT_FAILED_SUPPLIED_ADDRESS:
		m_currentDelay += deltaTime;
		if(m_currentDelay > m_maxDelay)
		{
			m_state = RCS_STATE_CLIENT;
		}
		break;
	case RCS_STATE_HOST:
		for(size_t index = 0;index < m_tcpSocketArray.size();index++)
		{
			TCPSocket *tcpSocket = m_tcpSocketArray.at(index);
			ProcessConnection(tcpSocket);
		}
		break;
	case RCS_STATE_HOST_FAILED:
		m_currentDelay += deltaTime;
		if (m_currentDelay > m_maxDelay)
		{
			m_state = RCS_STATE_CLIENT;
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : Disconnects the server
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::DisconnectAndCleanUpAllConnections()
{
	for(size_t index = 0;index < m_tcpSocketArray.size();index++)
	{
		TCPSocket *tcpSocket = m_tcpSocketArray.at(index);
		tcpSocket->Disconnect();
		delete tcpSocket;
		m_tcpSocketArray.erase(m_tcpSocketArray.begin() + index, m_tcpSocketArray.begin() + index + 1);
		index--;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::FailedToHost()
{
	m_state = RCS_STATE_HOST_FAILED;
	ResetDelay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::ResetDelay()
{
	m_currentDelay = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::ConstructBytePacketer(BytePacker *bp, void *data, size_t size)
{
	if(bp->GetWrittenBytesCount() < 2)
	{
		bp->WriteBytes(size, data);
	}
	else
	{
		bp->GetWritableBytesCount();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RCS::SendMsg(int idx, bool isEcho, char const *str)
{
	BytePacker message(BIG_ENDIAN);
	TCPSocket *sock = GetConnectionByIndex(idx);
	if (sock == nullptr) 
	{
		return false;
	}
	
	message.WriteBool(isEcho);
	std::string bitString = message.GetBitString();
	message.WriteString(str);
	bitString = message.GetBitString();

	uint16_t uslen;
	uslen = static_cast<uint16_t>(message.m_bufferSize);
	Endianness::ToEndianness(2, (void *)&uslen, BIG_ENDIAN);
	
	sock->Send((char*)&uslen,2);   
	sock->Send(((char *)message.m_buffer),message.m_bufferSize);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : Sends msg to all connections
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RCS::SendMsg(bool isEcho, char const *str)
{
	for(int socketIndex = 0;socketIndex < m_tcpSocketArray.size();socketIndex++)
	{
		BytePacker message(BIG_ENDIAN);
		TCPSocket *sock = m_tcpSocketArray.at(socketIndex);
		if (sock == nullptr)
		{
			continue;
		}

		message.WriteBool(isEcho);
		std::string bitString = message.GetBitString();
		message.WriteString(str);
		bitString = message.GetBitString();

		uint16_t uslen;
		uslen = static_cast<uint16_t>(message.m_bufferSize);
		Endianness::ToEndianness(2, (void *)&uslen, BIG_ENDIAN);

		sock->Send((char*)&uslen, 2);
		sock->Send(((char *)message.m_buffer), message.m_bufferSize);
	}
	return true;
}

void RCS::ProcessConnection(TCPSocket* tcp)
{
	if (tcp->m_isDisconnected)
	{
		tcp->Disconnect();
		return;
	}

	bool isReadyToProcess = false;
	BytePacker* buffer = tcp->m_bytePacketer;
	
	if (buffer->GetWrittenBytesCount() < (size_t)2)
	{
		tcp->Recv(buffer->GetWriteHead(), static_cast<unsigned int>(2 - buffer->GetWrittenBytesCount()));
	}

	if (buffer->GetWrittenBytesCount() >= 2)
	{
		std::string strb = buffer->GetBitString();
		std::string str    = buffer->GetAsString();
		
		uint16_t len = 0;
		memcpy((void *)&len,(char *)buffer->m_buffer, 2);
		//buffer->GetLengthFromData(&len);
		Endianness::FromEndianness(2, (void*)&len, LITTLE_ENDIAN);
		size_t bytesNeeded = len + 2 - buffer->GetWrittenBytesCount();
		if (bytesNeeded > 0)
		{
			size_t read = tcp->Recv(0, bytesNeeded);
			bytesNeeded -= read;
		}
		isReadyToProcess = (bytesNeeded == 0);
	}

	if (isReadyToProcess)
	{
		//buffer->AdvanceReadHead(2U);
		ProcessMessage(tcp, buffer);
		buffer->ResetWrite();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RCS::ProcessMessage(TCPSocket *socket, BytePacker *data)
{
	UNUSED(socket);
	std::string str = data->GetAsString();
	if(m_isHookedToDevConsole)
	{
		DevConsole::GetInstance()->PushToOutputText(str, Rgba::YELLOW);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::PushNewConnection(TCPSocket *socket)
{
	m_tcpSocketArray.push_back(socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TCPSocket * RCS::GetConnectionByIndex(int index)
{
	if(m_tcpSocketArray.size() < index)
	{
		return nullptr;
	}
	return m_tcpSocketArray.at(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RCS * RCS::GetInstance()
{
	if(s_theRCS == nullptr)
	{
		s_theRCS = new RCS();
	}
	return s_theRCS;
}

/*
void RCS::ProcessMessage(TCPSocket* socket, BytePacker *payload)
{
	bool isEcho;
	payload->Read(isEcho);

	string str;
	if (payload->Read(&str))
	{
		ASSERT(payload->GetReadableByteCount == 0);

		//succeeded in getting a command/string
		if (isEcho)
		{
			ConsolePrintf(str...);
		}
		else
		{
			RunCommand(str);
		}
	}
}*/

/*
template <typename T>
bool TCPSocket::Receive(T* out)
{
	return ::receive(out, sizeof(T));
}*/
