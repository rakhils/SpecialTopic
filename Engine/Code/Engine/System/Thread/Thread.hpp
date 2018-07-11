#pragma once
#include <process.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
/*\class  : Thread	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 7/8/2018 11:34:53 AM
* \contact: srsrakhil@gmail.com
*/
struct ThreadHandle_t;
//typedef ThreadHandle_t* ThreadHandle;
typedef void* ThreadHandle;
typedef void (*threadCB)( void *userData );
typedef void (*threadEntryCB)(void *data);
struct StartThreadArgs_t
{
	char const *m_name;
	threadEntryCB m_function;
	void *m_arg;
};
class Thread
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	Thread();
	~Thread();
	static DWORD WINAPI ThreadEntryPoint(void *arg)
	{
		StartThreadArgs_t *args = (StartThreadArgs_t*)arg;
		//SetName(args->m_name);

		threadEntryCB cb = args->m_function;
		void* parg = args->m_arg;

		// free up the memory used for passing the data forward; 
		delete args;

		// call the callback with the void*
		cb(parg);
		return 0;
	}
	//DWORD WINAPI ThreadEntryPoint(void *arg);
	static ThreadHandle ThreadCreate(threadEntryCB entry, void *userData = nullptr);
	static ThreadHandle ThreadCreate(char const *name, threadEntryCB entry, void *arg);
	static ThreadHandle ThreadCreate(char const *name, size_t stack_size, threadEntryCB entry, void *arg);
	
	static void ThreadJoin(ThreadHandle threadHandle);
	static void ThreadJoin( ThreadHandle* threadHandle,size_t const count);
	static void ThreadDetach( ThreadHandle threadHandle );
	static void ThreadCreateAndDetach( char const *name, size_t const stack_size,threadEntryCB entry, void *arg );
	static void ThreadCreateAndDetach( char const *name, threadEntryCB entry, void *arg);
	static void ThreadYield();
	static void ThreadSleep( int ms );

	static void TestOnThread(int numThreads);
	static void TestOnMainThread();
	static void TestWriteToFile(void *data);
	void		SetName( char const *name );

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