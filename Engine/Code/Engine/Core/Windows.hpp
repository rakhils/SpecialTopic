#pragma once
#include <vector>

#include "Engine/Math/IntVector2.hpp"
typedef bool (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 

class Windows
{
   private:
	   /*static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	   static int numProcessors;
	   static HANDLE self;*/
	   IntVector2 m_dimensions;
	   Windows( char const *title, float clientAspect );

   public:
	  ~Windows(); 

	  void* GetHandle() const { return m_hwnd; }

	  void AddHandler( windows_message_handler_cb cb );
	  void RemoveHandler(windows_message_handler_cb cb);
	  static double GetCurrentMemoryUsage();
	  IntVector2 GetDimensions();

   public:
	  void *m_hwnd; 

	  std::vector<windows_message_handler_cb> m_message_handlers; 

   public:
	  static Windows* CreateInstance( char const *title, float aspect );
	  
	  static Windows* GetInstance();
}; 

