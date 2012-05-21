#ifndef SYSTEM_H
#define SYSTEM_H

#include "defines.h"
#include "tinyxml.h"
#include "DbcFile.h"

class System
{
public:
	System();
	~System();

	void SayHello();
	void HandleArguments(int argc, char * argv[]);
	void ExportDBCs();
	void HandleErrorMessage(const char *fmt, ...);

private:
	// Settings
	string db_user; // MySql User
	string db_pass; // MySql Password
	string db_host; // MySql Host
	uint32 db_port; // MySql Port
	string dbc_db; // The Database name where the Dbc Files will be exported
	string homePath; // The path to application
	string structureXml; // The xml file where the structures are defined;
	bool createSqlFiles;

	// Functions
	void ShowUsage(char *prg);

	bool CreateDir( const char* path );
	const char *GetFileInDir( const char *dir );
#ifdef _WIN32
	HANDLE hFind;
	WIN32_FIND_DATA findFileInfo;
#elif __unix__
	DIR *dirp;
	struct dirent *dp;
#else
	//
#endif
};

#endif // SYSTEM_H