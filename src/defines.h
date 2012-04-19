#ifndef __DEFINES_H__
#define __DEFINES_H__

typedef __int64            int64;
typedef __int32            int32;
typedef __int16            int16;
typedef __int8             int8;
typedef unsigned __int64   uint64;
typedef unsigned __int32   uint32;
typedef unsigned __int16   uint16;
typedef unsigned __int8    uint8;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

#define MAX_QUERY_LEN		32*1024

#ifdef _WIN32
# include <windows.h>
#elif defined __unix__
# include <dirent.h>
#else
// MacOS things?
#endif


inline bool getDirContents(vector<string> &fileList, string dirpath = ".", string filter = "*", bool includeSubDirs = false)
{
#ifdef _WIN32
	HANDLE hFind;
	WIN32_FIND_DATA findFileInfo;
	string directory;

	directory = dirpath + "/" + filter;

	hFind = FindFirstFile(directory.c_str(), &findFileInfo);

	if(hFind == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if(includeSubDirs || (findFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			fileList.push_back(string(findFileInfo.cFileName));
	}
	while (FindNextFile(hFind, &findFileInfo));

	FindClose(hFind);

#elif defined __unix__ 
	const char *p = dirpath.c_str();
	DIR * dirp = opendir(p);
	struct dirent * dp;

	while (dirp)
	{
		errno = 0;
		if ((dp = readdir(dirp)) != NULL)
		{
			if(matchWildcardFilter(filter.c_str(), dp->d_name))
				fileList.push_back(string(dp->d_name));
		}
		else
			break;
	}

	if(dirp)
		closedir(dirp);
	else
		return false;
#else
	return false;
#endif

	return true;
}

#endif