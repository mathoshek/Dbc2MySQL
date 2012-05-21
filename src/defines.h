#ifndef __DEFINES_H__
#define __DEFINES_H__

#ifdef _WIN32
# include <windows.h>
#elif __unix__
# include <dirent.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/time.h>  // OK
# include <stdarg.h>    // OK
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
#elif __unix__
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

#include <ctime>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
using namespace std;

#define BUFFER_LEN		32768 // 32 * 1024

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

inline int getNumCores() {
#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#elif __unix__
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

extern double avg;
extern uint64 num;

class Timer
{
public:
	Timer()
	{
#ifdef WIN32
		QueryPerformanceFrequency(&frequency);
		startCount.QuadPart = 0;
		endCount.QuadPart = 0;
#else
		startCount.tv_sec = startCount.tv_usec = 0;
		endCount.tv_sec = endCount.tv_usec = 0;
#endif
		startTimeInMicroSec = 0;
		endTimeInMicroSec = 0;
	}

	void   start()
	{
#ifdef WIN32
		QueryPerformanceCounter(&startCount);
#else
		gettimeofday(&startCount, NULL);
#endif
	}
	double getElapsedTime()
	{
#ifdef WIN32
		QueryPerformanceCounter(&endCount);

		startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
		endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
		gettimeofday(&endCount, NULL);

		startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
		endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif
		return endTimeInMicroSec - startTimeInMicroSec;
	}

private:
    double startTimeInMicroSec;
    double endTimeInMicroSec;
#ifdef WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER startCount;
    LARGE_INTEGER endCount;
#else
    timeval startCount;
    timeval endCount;
#endif
};

#include "mysql.h"

class MySqlConnection
{
public:
	MySqlConnection( uint32 bufferMaxLen, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag )
	{
		_bufferMaxLen = bufferMaxLen;
		_conn = mysql_init( NULL );
		mysql_real_connect( _conn, host, user, passwd, db, port, unix_socket, clientflag );
		_buffer = new char[_bufferMaxLen];
		ResetCommand( );
	}

	void ResetCommand( )
	{
		_bufferPos = 0;
		_buffer[_bufferPos] = '\0';
		_markerPos = 0;
		_isBufferFull = false;
	}

	void Append( const char *fmt, ... )
	{
		va_list ap;
		int32 nr;
		int32 size = _bufferMaxLen - _bufferPos;

		va_start( ap, fmt );
		nr = vsnprintf( _buffer + _bufferPos, size, fmt, ap );
		if( nr > size || nr == -1 )
			_isBufferFull = true;
		else
			_bufferPos += nr;

		va_end( ap );
	}

	int SendQuery( )
	{
		int error =  mysql_query( _conn, _buffer );
		if(!error)
			ResetCommand( );
		return error;
	}

	int GetErrNo( )
	{
		return mysql_errno( _conn );
	}

	const char *GetError( )
	{
		return mysql_error( _conn );
	}

	const char *DumpCommand( )
	{
		return _buffer;
	}

	void EraseCommand( uint32 amount )
	{
		if( amount >= _bufferPos )
			ResetCommand( );
		else
		{
			_bufferPos -= amount;
			_buffer[_bufferPos] = '\0';

			if( _markerPos > _bufferPos )
				_markerPos = 0;
		}
	}

	void InsertCommandMarker( )
	{
		_markerPos = _bufferPos;
	}

	void RestoreCommandToMarker( )
	{
		_bufferPos = _markerPos;
		_buffer[_bufferPos] = '\0';
	}

	bool IsBufferFull( )
	{
		return _isBufferFull;
	}

	~MySqlConnection( )
	{
		mysql_close( _conn );
	}

private:
	MYSQL* _conn;
	uint32 _bufferMaxLen;
	char *_buffer;
	uint32 _bufferPos;
	uint32 _markerPos;
	bool _isBufferFull;
};

#endif
