#pragma once
#include <string>
#include <map>
#include <stdarg.h>
#include <queue>
#include <mutex>
#include "Engine/System/Thread/Thread.hpp"
#include "Engine/Time/TimeSystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
/*\class  : LogManager		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 5/20/2018 2:03:29 AM
* \contact: srsrakhil@gmail.com
*/
struct LogDefine
{
	std::string m_tag;
	Rgba        m_color;
	LogDefine() 
	{
		m_tag   = "default";
		m_color = Rgba::WHITE;
	};
	LogDefine(char *tag, Rgba color)
	{
		m_tag = tag;
		m_color = color;
	}
	LogDefine(const LogDefine &copy)
	{
		m_tag   = copy.m_tag;
		m_color = copy.m_color;
	}
};
struct Log_t
{
	std::string m_logTime;
	//std::string m_tag;
	std::string m_text;
	LogDefine   m_define;
	Log_t();
	Log_t(std::string tag, std::string text)
	{
		m_text			  = text;
		m_define.m_tag	  = tag;
		m_logTime = TimeSystem::GetCurrentDateAndTimeInString();
	}
	Log_t(LogDefine define, std::string text)
	{
		m_text    = text;
		m_define  = define;
		m_logTime = TimeSystem::GetCurrentDateAndTimeInString();
	}
};
typedef void (*LogForwardCallBack)(Log_t *cmd);
//template <typename T>
class ThreadSafeQueue
{
public:
	void enqueue(Log_t* log)
	{
		m_lock.lock();
		m_data.push(log);
		m_lock.unlock();
	}

	Log_t* dequeue()
	{
		m_lock.lock();
		Log_t *ret = nullptr;
		bool hasItem = !m_data.empty();
		if (hasItem)
		{
			ret = m_data.front();
			m_data.pop();
		}

		m_lock.unlock();
		return ret;
	}

	std::queue<Log_t*> m_data;
	std::mutex	  m_lock;
};
class ThreadSafeSet 
{
public:
	void Add(std::string tag)
	{
		m_lock.lock();
		m_data.push_back(tag);
		m_lock.unlock();
	}

	void Remove(std::string tag)
	{
		m_lock.lock();
		for(int index = 0; index < m_data.size();index++)
		{
			if(m_data.at(index) == tag)
			{
				m_data.erase(m_data.begin() + index, m_data.begin() + index + 1);
				break;
			}
		}
		m_lock.unlock();
	}

	bool HasTag(std::string tag)
	{	
		bool hasTag = false;
		m_lock.lock();
		for (int index = 0; index < m_data.size(); index++)
		{
			if (m_data.at(index) == tag)
			{
				hasTag = true;
				break;
			}
		}
		m_lock.unlock();
		return hasTag;
	}
	std::vector<std::string> m_data;
	std::mutex	  m_lock;
};
class LogManager
{

public:
	//Member_Variables
	
	ThreadHandle								m_thread; 
	ThreadSafeQueue								m_logQueue; 
	bool										m_isRunning =  true;
	std::string									m_defaultFileName;
	std::string									m_defaultHTMLFileName;
	std::string									m_timeStampFilePath;
	std::map<std::string,LogForwardCallBack>    m_logForwardCallBacks;
	ThreadSafeSet						    	m_filters; 
	ThreadSafeSet						    	m_devConsolefilters;
	bool										m_globalFilterCheck		= false;
	bool										m_devConsoleFilterCheck	= false;
	//Static_Member_Variables
	static bool									s_logEnabled;
	static std::map<std::string, FILE*>			s_logIdMaps;
	static LogManager*							s_logger;
	static std::map<std::string, LogDefine>     s_logDefines;
	//Methods

	LogManager();
	~LogManager();
	static void LogThreadWorker(void* data)
	{
		UNUSED(data);
		while (s_logger->isRunning())
		{
			s_logger->LogFlush();
			Thread::ThreadSleep(10);
		}

		s_logger->LogFlush();
	}
	static LogManager*	GetInstance();
	bool				isRunning();
	void				Stop();
	static void			LogSystemStartup(std::string fileRootName = "Logs\\defaultLog");
	static void			LogSystemShutdown();
	void				LogToFile(std::string filename, char const *, ...);
	
	void				Init(std::string fileName);
	void				LogFlush();
	void				LogFlushAll();
	void				LogFlushTest();
	void				LogTaggedPrintv(std::string tag,char const *format, va_list args);
	void				LogTaggedPrintf(std::string tag,char const *format, ...);
	void				LogTaggedPrintf(std::string tag,std::string text);
	void				LogPrintf(char const *format,...);
	void				LogWarningf(char const *format,...);
	void				LogErrorf( char const *format, ... );

	void				AttachLogForwardCallBacks(std::string id,LogForwardCallBack cb);
	void				DetachLogForwardCallBacks(std::string id);
	void				AddFilter(std::string tag);
	void				RemoveFilter(std::string tag);
	void				AddDevConsoleFilter(std::string tag);
	void			    RemoveDevConsoleFilter(std::string tag);

	void				LogShowAll();
	void				LogHideAll();
	void				LogShowTag(char const *tag);
	void				LogHideTag(char const *tag);
	void				LogShowDevConsole();
	void				LogHideDevConsole();

	

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

void				LogFileWriteCallBack(Log_t* log_t);
void				LogDevConsoleWriteCallBack(Log_t* log_t);
void				LogHTMLWriteCallBack(Log_t* log_t);