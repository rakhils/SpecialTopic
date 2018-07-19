#include "Engine/Logger/LogManager.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/Rgba.hpp"
bool								 LogManager::s_logEnabled = true;
std::map<std::string, FILE*>		 LogManager::s_logIdMaps;
LogManager *						 LogManager::s_logger = nullptr;
std::map<std::string, LogDefine>     LogManager::s_logDefines;
// CONSTRUCTOR

LogManager::LogManager()
{

}

// DESTRUCTOR
LogManager::~LogManager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : Gets the instance of logger
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LogManager* LogManager::GetInstance()
{
	if(s_logger == nullptr)
	{
		s_logger = new LogManager();
	}
	return s_logger;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : Returns running status of logger
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool LogManager::isRunning()
{
	return m_isRunning;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : Stops the logger
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::Stop()
{
	m_isRunning = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : Starting up logging system with root name
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogSystemStartup(std::string fileName)
{
	if (!s_logEnabled)
	{
		return;
	}
	if(s_logger == nullptr)
	{
		s_logger = new LogManager();
	}
	
	s_logger->Init(fileName);
	//CREATING DEFAULT DEFINE TAGS WARNING,ERROR
	LogDefine warningDefine;
	warningDefine.m_tag   = "warning";
	warningDefine.m_color = Rgba::YELLOW;
	s_logDefines["warning"] = warningDefine;

	LogDefine errorDefine;
	errorDefine.m_tag     = "error";
	errorDefine.m_color   = Rgba::RED;
	s_logDefines["error"] = errorDefine;

	LogDefine defautlDefine;
	defautlDefine.m_tag     = "default";
	defautlDefine.m_color   = Rgba::GREEN;
	s_logDefines["default"] = defautlDefine;
	 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : Shuts down log system
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogSystemShutdown()
{
	s_logger->Stop();
	Thread::ThreadJoin(s_logger->m_thread);

	std::map<std::string, FILE*>::iterator it = s_logIdMaps.begin();
	for(;it != s_logIdMaps.end();it++)
	{
		FileClose(it->second);
	}
	s_logger->m_thread = nullptr;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : Flushes out log to file
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogFlush()
{
	Log_t *data = nullptr;
	while ((data = m_logQueue.dequeue()) != nullptr)
	{
		/*if(m_globalFilterCheck)
		{
			delete data;
			continue;
		}*/


		if(m_isActingAsBlackList)
		{
			if (m_filters.HasTag(data->m_define.m_tag))
			{
				delete data;
				continue;
			}
			std::map<std::string, LogForwardCallBack>::iterator it = m_logForwardCallBacks.begin();
			for (; it != m_logForwardCallBacks.end(); it++)
			{
				if (it->first == "devconsole")
				{
					if (m_devConsolefilters.HasTag(data->m_define.m_tag))
					{
						continue;
					}
				}
				(*it->second)(data);
			}
			
		}
		else
		{
			if (!m_filters.HasTag(data->m_define.m_tag))
			{
				delete data;
				continue;
			}
			std::map<std::string, LogForwardCallBack>::iterator it = m_logForwardCallBacks.begin();
			for (; it != m_logForwardCallBacks.end(); it++)
			{
				if (it->first == "devconsole")
				{
					if (m_devConsolefilters.HasTag(data->m_define.m_tag))
					{
						continue;
					}
				}
				(*it->second)(data);
			}
		}
		delete data;
		data = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/17
*@purpose : Flushes all files immediately
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogFlushImmediate()
{
	std::map<std::string, FILE*>::iterator it = LogManager::GetInstance()->s_logIdMaps.find(LogManager::GetInstance()->m_defaultHTMLFileName);
	FILE *fileHTMLPtr = it->second;
	if(fileHTMLPtr != nullptr)
	{
		FileFlush(fileHTMLPtr);
	}
	it = LogManager::GetInstance()->s_logIdMaps.find(LogManager::GetInstance()->m_defaultFileName);
	FILE *filePtr = it->second;
	if (filePtr != nullptr)
	{
		FileFlush(filePtr);
	}
	it = LogManager::GetInstance()->s_logIdMaps.find(LogManager::GetInstance()->m_timeStampFilePath);
	FILE *filePtrTimeStamp = it->second;
	if (filePtrTimeStamp != nullptr)
	{
		FileFlush(filePtrTimeStamp);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Flushes everything to files
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogFlushAll()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Tests logs flushing
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogFlushTest()
{
	LogPrintf("FLUSH TEST");
	LogFlush();
	LogFlushImmediate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogToFile(std::string filename, char const *format, ...)
{
	if(!s_logEnabled)
	{
		return;
	}
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	std::string datetime = TimeSystem::GetCurrentDateAndTimeInString();

	FILE *filePtr = s_logIdMaps[filename];

	if(filePtr == nullptr)
	{
		filePtr = FileOpenForAppend(filename);
		s_logIdMaps[filename] = filePtr;
	}
	FileAppendString(filePtr, datetime + " :: " + std::string(buffer));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::Init(std::string fileName)
{
	m_defaultFileName = fileName;
	m_defaultFileName.append(".txt");
	m_defaultHTMLFileName = fileName + "HTML.html";
	/////////////////////////////////////////////
	//TIME STAMP
	time_t rawtime;
	char buffer[80];
	time(&rawtime);
	struct tm info;
	localtime_s(&info, &rawtime);
	strftime(buffer, 50, "_%Y-%m-%d_%H-%M-%S", &info);

	std::string timeStamp = fileName;
	timeStamp.append(buffer);// +".png");
	timeStamp.append(".txt");
	m_timeStampFilePath = timeStamp;
	/////////////////////////////////////////////
	std::wstring folder(L"Logs");
	if (CreateDirectory(folder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		m_thread = Thread::ThreadCreate("loggerThread", LogThreadWorker, nullptr);

		FILE *filePtr = nullptr;
		filePtr = FileOpenForAppend(m_defaultFileName);
		s_logIdMaps[m_defaultFileName] = filePtr;

		FILE *filePtrTimeStamp = nullptr;
		filePtrTimeStamp = FileOpenForAppend(m_timeStampFilePath);
		s_logIdMaps[m_timeStampFilePath] = filePtrTimeStamp;

		FILE *filePtrHTML = nullptr;
		filePtrHTML       = FileOpenForAppend(m_defaultHTMLFileName);
		s_logIdMaps[m_defaultHTMLFileName] = filePtrHTML;
	}
	AttachLogForwardCallBacks("defualtfilewrite",LogFileWriteCallBack);
	AttachLogForwardCallBacks("defualthtmlwrite", LogHTMLWriteCallBack);

	//AttachLogForwardCallBacks(LogDevConsoleWriteCallBack);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : Logs to file
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogTaggedPrintf(std::string tag, char const *format, ...)
{
	if (!s_logEnabled)
	{
		return;
	}
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);
	LogTaggedPrintf(tag, std::string(buffer));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Logs the text with given tag
*@param   : Tag and text
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogTaggedPrintf(std::string tag, std::string text,bool needDetails)
{
	std::map<std::string, LogDefine>::iterator it = s_logDefines.find(tag);
	LogDefine define;
	if(it != s_logDefines.end())
	{
		define = s_logDefines[tag];
	}
	else
	{
		define = s_logDefines["default"]; 
	}

	Log_t *log = nullptr;// new Log_t(define, text);
	if(needDetails)
	{
		log = new Log_t(define, text);
	}
	else
	{
		log = new Log_t(text);
	}
	m_logQueue.enqueue(log);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Logs the text with "log" as tag
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void LogManager::LogPrintf(bool needDetails,char const *format,...)
{
	if (!s_logEnabled)
	{
		return;
	}
	va_list args;
	va_start(args, format);
	char buffer[5000];
	vsnprintf_s(buffer, 5000, format, args);
	va_end(args);
	LogTaggedPrintf("log", std::string(buffer),needDetails);
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogPrintf(char const *format, ...)
{
	if (!s_logEnabled)
	{
		return;
	}
	va_list args;
	va_start(args, format);
	char buffer[5000];
	vsnprintf_s(buffer, 5000, format, args);
	va_end(args);
	LogTaggedPrintf("log", std::string(buffer));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Logs the text with warning tag
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogWarningf(char const *format,...)
{
	if (!s_logEnabled)
	{
		return;
	}
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);
	LogTaggedPrintf("warning", std::string(buffer));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Logs with error tag
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogErrorf(char const *format, ...)
{
	if (!s_logEnabled)
	{
		return;
	}
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);
	LogTaggedPrintf("error", std::string(buffer));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Attach log forwarding call backs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::AttachLogForwardCallBacks(std::string id,LogForwardCallBack cb)
{
	m_logForwardCallBacks[id] = cb;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::DetachLogForwardCallBacks(std::string id)
{
	m_logForwardCallBacks.erase(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogShowTag(std::string tag)
{
	if(m_isActingAsBlackList)
	{
		m_filters.Remove(tag);
	}
	else
	{
		m_filters.Add(tag);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogHideTag(std::string tag)
{
	if(m_isActingAsBlackList)
	{
		m_filters.Add(tag);
	}
	else
	{
		m_filters.Remove(tag);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogShowAllTag()
{
	m_isActingAsBlackList = true;
	m_globalFilterCheck = false;
	m_filters.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogHideAllTag()
{
	m_globalFilterCheck = true;
	m_filters.RemoveAll();
	m_isActingAsBlackList = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogShowDevConsoleTag(std::string tag)
{
	m_devConsolefilters.Remove(tag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogHideDevConsoleTag(std::string tag)
{
	m_devConsolefilters.Add(tag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogShowAllDevConsoleTag()
{
	m_devConsoleFilterCheck = false;
	m_devConsolefilters.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::LogHideAllDevConsoleTag()
{
	m_devConsoleFilterCheck = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : F
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogFileWriteCallBack(Log_t* log_t)
{
	std::string tag		 = log_t->m_define.m_tag;
	std::string text	 = log_t->m_text;
	std::string dateTime = log_t->m_logTime;

	std::map<std::string, FILE*>::iterator it = LogManager::GetInstance()->s_logIdMaps.find(LogManager::GetInstance()->m_defaultFileName);
	FILE *filePtr = it->second;
	it = LogManager::GetInstance()->s_logIdMaps.find(LogManager::GetInstance()->m_timeStampFilePath);
	FILE *filePtrTimeStamp = it->second;

	if (filePtr == nullptr)
	{
		filePtr = FileOpenForAppend(LogManager::GetInstance()->m_defaultFileName);
		LogManager::GetInstance()->s_logIdMaps[LogManager::GetInstance()->m_defaultFileName] = filePtr;
	}
	FileAppendString(filePtr, dateTime + " :: [" + tag + "]" + "[" + text + "]\n");
	if (filePtrTimeStamp == nullptr)
	{
		filePtrTimeStamp = FileOpenForAppend(LogManager::GetInstance()->m_defaultFileName);
		LogManager::GetInstance()->s_logIdMaps[LogManager::GetInstance()->m_defaultFileName] = filePtrTimeStamp;
	}
	FileAppendString(filePtrTimeStamp, dateTime + " :: [" + tag + "]" + "[" + text + "]\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Writes logs to devconsole
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogDevConsoleWriteCallBack(Log_t* log_t)
{
	std::string tag       = log_t->m_define.m_tag;
	std::string text      = log_t->m_text;
	std::string dateTime  = log_t->m_logTime;
	std::string finalText = dateTime + " :: [" + tag + "]" + "[" + text + "]\n";
	DevConsole::GetInstance()->PushToOutputText(finalText, log_t->m_define.m_color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogHTMLWriteCallBack(Log_t* log_t)
{
	LogDefine def        = log_t->m_define;
	std::string text     = log_t->m_text;
	std::string dateTime = log_t->m_logTime;
	Rgba color			 = def.m_color;

	std::map<std::string, FILE*>::iterator it = LogManager::GetInstance()->s_logIdMaps.find(LogManager::GetInstance()->m_defaultHTMLFileName);
	FILE *filePtr = it->second;

	if (filePtr == nullptr)
	{
		filePtr = FileOpenForAppend(LogManager::GetInstance()->m_defaultHTMLFileName);
		LogManager::GetInstance()->s_logIdMaps[LogManager::GetInstance()->m_defaultHTMLFileName] = filePtr;
	}
	//<p><span style="color: #ffffff;">asdasd</span></p>


	std::string format;
	format.append("<p><span style=\"color :#");
	format.append(def.m_color.GetAsHexaEquivalent());
	format.append(";\">");
	format.append(dateTime + " :: ");
	format.append(text);
	format.append("</span></p>");

	FileAppendString(filePtr, format);
}
