#ifndef SYSTEM_H
#define SYSTEM_H

#include "defines.h"
#include "tinyxml.h"
#include "dirent.h"
#include "direct.h"
#include "DbcFile.h"
#include "mysql.h"

class System
{
public:
	System();
	~System();

	void SayHello();
	void HandleArguments(int argc, char * argv[]);
	void ExportDBCs();

private:
	// Settings
	string db_user; // MySql User
	string db_pass; // MySql Password
	string db_host; // MySql Host
	string dbc_db; // The Database name where the Dbc Files will be exported
	string homePath; // The path to application
	string structureXml; // The xml file where the structures are defined;
	bool createSqlFiles;

	// Functions
	void ShowUsage(char *prg);
	void CreateDir(string path);
	string Terminator(string &str);
};

#endif // SYSTEM_H