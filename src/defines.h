#ifndef __DEFINES_H__
#define __DEFINES_H__

#ifdef _WIN32
# include <windows.h>
#elif defined __unix__
# include <dirent.h>
//# include <stddef.h>
# include <sys/stat.h>
#else
// MacOS things?
#endif

#ifdef _WIN32
typedef signed __int64		int64;
typedef signed __int32		int32;
typedef signed __int16		int16;
typedef signed __int8		int8;
typedef unsigned __int64	uint64;
typedef unsigned __int32	uint32;
typedef unsigned __int16	uint16;
typedef unsigned __int8		uint8;
#elif defined __unix__
#include <stdint.h>
typedef int64_t				int64;
typedef int32_t				int32;
typedef int16_t				int16;
typedef int8_t				int8;
typedef uint64_t			uint64;
typedef uint32_t			uint32;
typedef uint16_t			uint16;
typedef uint8_t				uint8;
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
using namespace std;

#define MAX_QUERY_LEN		32*1024

#ifdef __unix__
inline bool matchWildcardFilter(const char* filter, const char* str)
{
	if(!filter || !str)
		return false;

	// end on null character
	while(*filter && *str)
	{
		if(*filter == '*')
		{
			if(*++filter == '\0')   // wildcard at end of filter means all remaing chars match
				return true;

			while(true)
			{
				if(*filter == *str)
					break;
				if(*str == '\0')
					return false;   // reached end of string without matching next filter character
				str++;
			}
		}
		else if(*filter != *str)
			return false;           // mismatch

		filter++;
		str++;
	}

	return ((*filter == '\0' || (*filter == '*' && *++filter == '\0')) && *str == '\0');
}
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
	//const char *p = dirpath.c_str();
	DIR * dirp = opendir(dirpath.c_str());
	struct dirent * dp;

	while (dirp)
	{
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

inline string Terminator(const char *s)
{
	string str = s;

	if (str.size() == 0)
		return str.c_str();

	size_t found = str.find("\\");
	while (found != str.npos)
	{
		str.insert(found, "\\");
		found = str.find("\\", found + 2);
	}

	found = str.find("'");
	while (found != str.npos)
	{
		str.insert(found, "\\");
		found = str.find("'", found + 2);
	}

	found = str.find("\"");
	while (found != str.npos)
	{
		str.insert(found, "\\");
		found = str.find("\"", found + 2);
	}

	found = str.find("\r\n");
	while (found != str.npos)
	{
		str.replace(found, 2, "\\r\\n");
		found = str.find("\r\n", found + 1);
	}

	return str;
}

#endif
