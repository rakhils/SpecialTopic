#pragma once
#include <stdio.h>
#include "stdlib.h"
#include <string>
//#include "Engine/Core/EngineCommon.hpp"
#define     CHECK_FILE_EXIST(path) DoFileExist(path)

void*       FileReadToBuffer(char const *filename);
bool        FileWrite(std::string filename,char *content,int length);
FILE*       FileOpenForAppend(std::string fileName);
void	    FileAppendString(FILE* fp, std::string content);
void		FileAppendChar(FILE *fp, char ch);
FILE*       FileOpenForWrite(std::string filename);
void		FileFlush(FILE *fp);
void		FileClose(FILE* fp);
std::string FileReadNextLine(FILE *fp);
std::string GetFileContentAsString(char const* filename);
bool		DoFileExist(std::string filepath);
void        CreateLocalDirectory(std::string path);

