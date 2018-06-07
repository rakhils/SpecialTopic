#pragma once
//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once
#include <stdint.h>
//#define WIN32_LEAN_AND_MEAN
#include<windows.h>

//-----------------------------------------------------------------------------------------------
double   GetCurrentTimeSeconds();
uint64_t GetPerformanceCounter();
double   PerformanceCounterToSeconds(uint64_t seconds);
uint64_t SecondsToPerformanceCounter(double seconds);