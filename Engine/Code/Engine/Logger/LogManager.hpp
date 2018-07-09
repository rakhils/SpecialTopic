#pragma once
#include <string>
#include <map>
#include <stdarg.h>
#include <queue>
#include <mutex>
#include "Engine/System/Thread/Thread.hpp"
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
	std::string m_tag;
	std::string m_text;
	Log_t(std::string tag, std::string text)
	{
		m_text = text;
		m_tag  = tag;
	}
};
template <typename T>
class ThreadSafeQueue
{
public:
	void enqueue(T *log)
	{
		m_lock.lock();
		m_data.push(log);
		m_lock.unlock();
	}

	bool dequeue(T *out_v)
	{
		m_lock.lock();

		bool hasItem = !m_data.empty();
		if (hasItem)
		{
			out_v = m_data.front();
			m_data.pop();
		}

		m_lock.unlock();
		return hasItem;
	}

public:
	std::queue<T*> m_data;
	std::mutex	  m_lock;
};
class LogManager
{

public:
	//Member_Variables
	static bool						    s_logEnabled;
	static std::map<std::string, FILE*> s_logIdMaps;
	static LogManager*					s_logger;
	ThreadHandle						m_thread; 
	ThreadSafeQueue<Log_t>				m_logQueue; 
	bool								m_isRunning =  true;
	std::string							m_defaultFilePath;
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
	static void			LogSystemStartup(std::string fileRootName = "Data\\Logs\\defaultLog.txt");
	static void			LogSystemShutdown();
	void				LogToFile(std::string filename, char const *, ...);
	
	void				LogTaggedPrintv(std::string tag,char const *format, va_list args);
	void				LogTaggedPrintf(std::string tag,char const *format, ...);
	void				LogFlush();
	void				LogPrintf(char const *format);
	void				LogWarningf(char const *format);
	void				LogErrorf( char const *format, ... );

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