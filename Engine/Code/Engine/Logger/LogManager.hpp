#pragma once
#include <string>
#include <map>
#include <stdarg.h>
#include <queue>
#include <mutex>
#include "Engine/System/Thread/Thread.hpp"
#include "Engine/Time/TimeSystem.hpp"

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

struct Log_t
{
	std::string m_logTime;
	std::string m_tag;
	std::string m_text;
	Log_t();
	Log_t(std::string tag, std::string text)
	{
		m_text	  = text;
		m_tag	  = tag;
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

public:
	std::queue<Log_t*> m_data;
	std::mutex	  m_lock;
};
class LogManager
{

public:
	//Member_Variables
	static bool									s_logEnabled;
	static std::map<std::string, FILE*>			s_logIdMaps;
	static LogManager*							s_logger;
	ThreadHandle								m_thread; 
	ThreadSafeQueue								m_logQueue; 
	bool										m_isRunning =  true;
	std::string									m_defaultFileName;
	std::string									m_timeStampFilePath;
	std::map<std::string,LogForwardCallBack>    m_logForwardCallBacks;
	//Static_Member_Variables

	//Methods

	LogManager();
	~LogManager();
	static void LogThreadWorker(void* data)
	{
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
	void				LogShowAll();
	void				LogHideAll();
	void				LogShowTag(char const *tag);
	void				LogHideTag(char const *tag);

	

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