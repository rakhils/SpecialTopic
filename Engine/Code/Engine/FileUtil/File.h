#pragma once
#include <stdio.h>
#include "stdlib.h"
#include <string>
//#include "Engine/Core/EngineCommon.hpp"
#define     CHECK_FILE_EXIST(path) DoFileExist(path)

void*       FileReadToBuffer(char const *filename);
bool        FileWrite(std::string filename,char *content,int length);
FILE*       FileAppend(std::string fileName);
void	    FileAppendString(FILE* fp, std::string content);
void        CloseFile(FILE *fp);
std::string GetFileContentAsString(char const* filename);
bool		DoFileExist(std::string filepath);

