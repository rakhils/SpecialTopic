#include "Engine/Net/RCS.hpp"
#include "Engine/Net/TCP/TCPServer.hpp"
#include "Engine/Net/TCP/TCPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/System/Thread/Thread.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
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
/*DATE    : 2018/09/09
*@purpose : Shuts down engine system
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::ShutDown()
{
	if(m_tcpServer != nullptr && m_tcpServer->m_isListening)
	{
		m_tcpServer->m_isListening = false;
	}
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
		if (m_state == RCS_STATE_CLIENT)
		{
			m_state = RCS_STATE_CLIENT;
		}
		else if (m_state == RCS_STATE_FORCE_JOIN)
		{
			m_state = RCS_STATE_FORCE_JOIN;
		}
		else
		{
			m_state = RCS_STATE_CLIENT;
		}

		m_tcpSocketArray.push_back(tcpSocket);
		return true;
	}
	if(m_state == RCS_STATE_FORCE_JOIN)
	{
		m_state = RCS_STATE_CLIENT;
		return false;
	}
	m_state = RCS_STATE_CLIENT_FAILED;
	CleanUpHosting();
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
	if(m_tcpServer == nullptr)
	{
		m_tcpServer = new TCPServer();
		m_tcpServer->m_port = m_rcsPort;
		Thread::ThreadCreateAndDetach("net_listen", TCPServer::ListenOnThread,((void *)m_tcpServer));
		m_state = RCS_STATE_HOST;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::CleanUpHosting()
{
	if(m_tcpServer != nullptr)
	{
		m_tcpServer->Disconnect();
		delete m_tcpServer;
		m_tcpServer = nullptr;
	}
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
			m_rcsPort = m_rcsDefaultPort;
			Join();
			break;
		}
		if(m_tcpSocketArray.size() == 1)
		{
			TCPSocket *tcpSocket = m_tcpSocketArray.at(0);
			if (tcpSocket->IsDisconnected())
			{
				m_tcpSocketArray.erase(m_tcpSocketArray.begin(), m_tcpSocketArray.begin() + 1);
				delete tcpSocket;
			}
			else
			{
				TCPSocket *tcpSocket = m_tcpSocketArray.at(0);
				ProcessConnection(tcpSocket);
			}
		}
		break;
	case RCS_STATE_FORCE_JOIN:
		if (m_tcpSocketArray.size() == 0)
		{
			Join();
			break;
		}
		if (m_tcpSocketArray.size() == 1)
		{
			TCPSocket *tcpSocket = m_tcpSocketArray.at(0);
			if (tcpSocket->IsDisconnected())
			{
				m_tcpSocketArray.erase(m_tcpSocketArray.begin(), m_tcpSocketArray.begin() + 1);
				delete tcpSocket;
				m_state = RCS_STATE_CLIENT;
			}
			else
			{
				TCPSocket *tcpSocket = m_tcpSocketArray.at(0);
				ProcessConnection(tcpSocket);
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
			if(m_tcpSocketArray.at(index)->m_isDisconnected)
			{
				TCPSocket *tcpSocket = m_tcpSocketArray.at(index);
				m_tcpSocketArray.erase(m_tcpSocketArray.begin() + index, m_tcpSocketArray.begin() + index + 1);
				delete tcpSocket;
				index--;
				continue;
			}
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
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::RenderInfo()
{
	Material *textmaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawAABB(AABB2(Vector2(1650, 900), 250, 100), Rgba::FADED_WHITE);
	float m_fontSize = 7.5;
	std::string rcsState = RCS::GetInstance()->GetState();
	Vector2 RCSInfoTextPosition(1450, 990);
	Renderer::GetInstance()->BindMaterial(textmaterial);
	Renderer::GetInstance()->DrawTextOnPoint(rcsState, 0, static_cast<int>(rcsState.length()), RCSInfoTextPosition, m_fontSize, Rgba::WHITE);
	RCSInfoTextPosition.y -= 2*m_fontSize;

	if (RCS::GetInstance()->m_state == RCS_STATE_HOST)
	{
		std::string joinAddress = "JOIN ADDRESS ";
		Renderer::GetInstance()->DrawTextOnPoint(joinAddress, 0, static_cast<int>(joinAddress.length()), RCSInfoTextPosition, m_fontSize, Rgba::WHITE);
		RCSInfoTextPosition.y -= 2*m_fontSize;
		if(RCS::GetInstance()->m_tcpServer != nullptr)
		{
		joinAddress = RCS::GetInstance()->m_tcpServer->GetIp() + ":" + ToString(RCS::GetInstance()->m_tcpServer->m_port);
		Renderer::GetInstance()->DrawTextOnPoint(joinAddress, 0, static_cast<int>(joinAddress.length()), RCSInfoTextPosition+ Vector2(m_fontSize*2,0), m_fontSize, Rgba::WHITE);
		RCSInfoTextPosition.y -= 2*m_fontSize;

		}

	}

	if (RCS::GetInstance()->m_state == RCS_STATE_HOST || RCS::GetInstance()->m_state == RCS_STATE_CLIENT)
	{
		std::string text = "CONNECTIONS ";
		Renderer::GetInstance()->DrawTextOnPoint(text, 0, static_cast<int>(text.length()), RCSInfoTextPosition , m_fontSize, Rgba::WHITE);
		RCSInfoTextPosition.y -= 2*m_fontSize;

		for (size_t index = 0; index < m_tcpSocketArray.size(); index++)
		{
			std::string RCSInfoText = "["+ToString((int)index)+"]";
			RCSInfoText += " " + m_tcpSocketArray.at(index)->GetRemoteIp()+":"+ToString(m_tcpSocketArray.at(index)->m_port);

			RCSInfoTextPosition.y -= index*2*m_fontSize;
			Renderer::GetInstance()->BindMaterial(textmaterial);
			Renderer::GetInstance()->DrawTextOnPoint(RCSInfoText, 0, static_cast<int>(RCSInfoText.length()), RCSInfoTextPosition+ Vector2(m_fontSize*2,0), static_cast<float>(m_fontSize), Rgba::WHITE);
		}
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCS::SendMsg(TCPSocket *tcpSocket, char* data)
{
	for(int index = 0;index < m_tcpSocketArray.size();index++)
	{
		if(tcpSocket == m_tcpSocketArray.at(index))
		{
			SendMsg(index,true,data);
			return;
		}
	}
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
		buffer->AdvanceReadHead(2U);
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
bool RCS::ProcessMessage(TCPSocket *tcpSocket, BytePacker *data)
{
	bool isEcho = false;
	data->ReadBool(&isEcho);

	size_t size = 0;
	data->ReadSize(&size);

	char str[1000];
	data->ReadString(str, size);

	
	std::string dataStr(str, size);

	std::string strr = data->GetAsString();
	
	std::string netaddr = "[" + tcpSocket->GetRemoteIp() + ":" + ToString(tcpSocket->m_port) + "]";
	if(m_isHookedToDevConsole)
	{
		if(isEcho)
		{
			DevConsole::GetInstance()->PushToOutputText(netaddr+dataStr, Rgba::YELLOW);
		}
		else
		{
			DevConsole::GetInstance()->PushToOutputText(netaddr+dataStr,Rgba::YELLOW);
			RCSStruct *rcsData = new RCSStruct(tcpSocket);
			DevConsole::GetInstance()->AttachDevConsoleCallBacks(rcsData);
			CommandRun(dataStr.c_str());
		}
	}
	data->ResetRead();
	
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
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string RCS::GetState()
{
	switch (m_state)
	{
	case RCS_STATE_CLIENT:
		return "RCS_STATE_CLIENT";
		break;
	case RCS_STATE_FORCE_JOIN:
		return "RCS_STATE_FORCE_JOIN";
		break;
	case RCS_STATE_CLIENT_FAILED:
		return "RCS_STATE_CLIENT_FAILED";
		break;
	case RCS_STATE_CLIENT_FAILED_SUPPLIED_ADDRESS:
		return "RCS_STATE_CLIENT_FAILED_SUPPLIED_ADDRESS";
		break;
	case RCS_STATE_HOST:
		return "RCS_STATE_HOST";
		break;
	case RCS_STATE_HOST_FAILED:
		return "RCS_STATE_HOST_FAILED";
		break;
	default:
		break;
	}
	return "INVALID";
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
