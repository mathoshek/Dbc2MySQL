#include "System.h"

System::System()
{
	//
}

System::~System()
{
	//
}

void System::SayHello()
{
	printf("=================================================\n");
	printf("===============Welcome to Dbc2MySQL==============\n");
	printf("===================by Mathoshek==================\n");
	printf("=================================================\n\n");
}

void System::HandleArguments(int argc, char * argv[])
{
	// Get the running program's path, including the program's name
	homePath = argv[0];

	// Delete the program name
	size_t found = homePath.find_last_of("/\\");
	if (found == -1)
		homePath = ".\\";
	else
		homePath = homePath.substr(0, found + 1);

	// Get information from program arguments
	for (uint8 i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
			case 'u':
				if(i + 1 < argc)
					db_user = argv[i + 1];
				else
					ShowUsage(argv[0]);
				break;
			case 'p': 
				if(i + 1 < argc)
					db_pass = argv[i + 1];
				else
					ShowUsage(argv[0]);
				break;
			case 'h':
				if(i + 1 < argc)
					db_host = argv[i + 1];
				else
					ShowUsage(argv[0]);
				break;
			case 'd':
				if(i + 1 < argc)
					dbc_db = argv[i + 1];
				else
					ShowUsage(argv[0]);
				break;
			case 's':
				if(i + 1 < argc)
					if(strcmp(argv[i + 1], "yes") == 0)
						createSqlFiles = true;
					else
						createSqlFiles = false;
				else
					ShowUsage(argv[0]);
				break;
			case 'x':
				if(i + 1 < argc)
					structureXml = argv[i + 1];
				else
					ShowUsage(argv[0]);
				break;
			}
			i++;
		}
	}
	if (db_user.empty() || db_pass.empty() || db_host.empty() || dbc_db.empty() || structureXml.empty())
	{
		printf("Some MySql Information missing, please check again\n");
		ShowUsage(argv[0]);
	}
}

void System::ShowUsage(char *prg)
{
	printf(
		"Usage:\n"\
		"%s -[var] [value]\n"\
		"-u set mysql user\n"\
		"-p set mysql password\n"\
		"-h set mysql host\n"\
		"-d set the database name in which the dbc files will be extracted\n"\
		"-s y if you want to create a folder with sql files\n"\
		"-x set the structure.xml file\n"\
		"Example: %s -u root -p pass -h localhost -d dbc -s no -x StructureWotLK.xml", prg, prg);
	exit(1);
}

void System::CreateDir(string path)
{
#ifdef _WIN32
	CreateDirectory( path.c_str(), NULL );
#elif defined __unix__
	mkdir( Path.c_str(), 0777 );
#else
// MacOS things?
#endif
}

string System::Terminator(string &str)
{
	if (&str == NULL)
		return NULL;

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

	return str.c_str();
}

void System::ExportDBCs()
{
	MYSQL *conn;
	DbcFile dbcfile;
	FILE *f;

	// Connect to MySql Database
	conn = mysql_init(NULL);
	if (conn == NULL)
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	if (mysql_real_connect(conn, db_host.c_str(), db_user.c_str(), db_pass.c_str(), NULL, 3306, NULL, 0) == NULL)
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	// Drop the old database if exists, create a new one, and use it
	string dropcommand = "DROP DATABASE IF EXISTS `";
	dropcommand += dbc_db.c_str();
	dropcommand += "`;\n";
	if (mysql_query(conn, dropcommand.c_str()))
	{
		FILE *error = fopen("error.txt","w");
		fprintf(error, dropcommand.c_str());
		fclose(error);
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	string createcommand = "CREATE DATABASE IF NOT EXISTS `";
	createcommand += dbc_db.c_str();
	createcommand += "` CHARACTER SET utf8;\n";
	if (mysql_query(conn, createcommand.c_str()))
	{
		FILE *error = fopen("error.txt","w");
		fprintf(error, createcommand.c_str());
		fclose(error);
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	string usecommand = "USE `";
	usecommand += dbc_db.c_str();
	usecommand += "`;\n";
	if (mysql_query(conn, usecommand.c_str()))
	{
		FILE *error = fopen("error.txt","w");
		fprintf(error, usecommand.c_str());
		fclose(error);
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	//Open the XML
	string xmlpath = homePath.c_str();
	xmlpath += structureXml;
	TiXmlDocument doc(xmlpath.c_str());
	bool loadOkay = doc.LoadFile();
	TiXmlElement *pRoot = NULL;
	if (loadOkay)
	{
		printf("\"%s\" loaded\n", structureXml.c_str());
		pRoot = doc.FirstChildElement("DBFilesClient");
	}
	else
	{
		printf("Failed to load file \"%s\"\n", structureXml.c_str());
		exit(1);
	}

	string filename;
	if(createSqlFiles == true)
	{
		filename = homePath;
		filename += "sql";
		CreateDir(filename);
	}

	string dpath = homePath.c_str();
	dpath += "dbc\\";
	vector<string> dbcFiles;
	if( getDirContents( dbcFiles, dpath, "*.dbc" ) == true )
	{
		for( vector<string>::iterator i = dbcFiles.begin(); i < dbcFiles.end(); ++i )
		{

			printf("Found %s \n", (*i).c_str());

			string fpath = dpath;
			fpath += (*i).c_str();

			if( dbcfile.Load(fpath.c_str()) == false )
				continue;

			TiXmlElement *pDbcFile = NULL, *pField = NULL;
			if ( pRoot )
			{
				pDbcFile = pRoot->FirstChildElement(dbcfile.GetName().c_str());

				string sqlstructure;
				sqlstructure = dbcfile.MakeMySqlStructure(pDbcFile, dbcfile.GetName());

				//Send sqlstructure query
				if(createSqlFiles == true)
				{
					filename = homePath;
					filename += "sql\\";
					filename += dbcfile.GetName();
					filename += ".sql";
					f = fopen(filename.c_str(), "w");
					fprintf(f, "%s", sqlstructure);
				}
				if (mysql_query(conn, sqlstructure.c_str()))
				{
					FILE *error = fopen("error.txt","w");
					fprintf(error, sqlstructure.c_str());
					fclose(error);
					printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
					exit(1);
				}

				string sqlinsert;
				sqlinsert = "INSERT INTO `";
				sqlinsert += dbcfile.GetName();
				sqlinsert += "` VALUES\n";

				for (uint32 i = 0; i < dbcfile.GetNumRows(); i++)
				{
					if (sqlinsert.size() + 1024 > MAX_QUERY_LEN)
					{
						sqlinsert.resize(sqlinsert.size() - 2);
						sqlinsert += "\n";
						if(createSqlFiles == true)
							fprintf(f, "%s", sqlinsert.c_str());
						if (mysql_query(conn, sqlinsert.c_str()))
						{
							FILE *error = fopen("error.txt","w");
							fprintf(error, sqlinsert.c_str());
							fclose(error);
							printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
							exit(1);
						}
						sqlinsert.clear();
						sqlinsert = "INSERT INTO `";
						sqlinsert += dbcfile.GetName();
						sqlinsert += "` VALUES\n(";
					}
					else
						sqlinsert += "(";

					for(uint32 j = 0; j < dbcfile.GetNumCols(); j++)
					{
						char *tmp;
						string tmpstr;
						switch (dbcfile.GetFormat(j))
						{
						case FT_UINT8:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%u", dbcfile.getRecord(i).getUInt8(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_INT8:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%i", dbcfile.getRecord(i).getInt8(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_UINT16:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%u", dbcfile.getRecord(i).getUInt16(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_INT16:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%i", dbcfile.getRecord(i).getInt16(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_UINT32:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%u", dbcfile.getRecord(i).getUInt32(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_INT32:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%i", dbcfile.getRecord(i).getInt32(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_UINT64:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%u", dbcfile.getRecord(i).getUInt64(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_INT64:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%i", dbcfile.getRecord(i).getInt64(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_FLOAT:
							tmp = (char *)malloc(256);
							sprintf(tmp, "%f", dbcfile.getRecord(i).getFloat(j));
							sqlinsert +="'";
							sqlinsert += tmp;
							sqlinsert +="',";
							free(tmp);
							break;
						case FT_STRING:
							tmpstr += dbcfile.getRecord(i).getString(j);
							sqlinsert +="'";
							sqlinsert += Terminator(tmpstr);
							sqlinsert +="',";
							tmpstr.clear();
							break;
						case FT_IGNORED:
							break;
						}
						if (j == dbcfile.GetNumCols() - 1)
						{
							sqlinsert.resize(sqlinsert.size() - 1);
							sqlinsert +="),\n";
						}
					}
				}
				sqlinsert.resize(sqlinsert.size() - 2);
				if (dbcfile.GetNumRows() != 0)
				{
					if(createSqlFiles == true)
					{
						fprintf(f, "%s", sqlinsert.c_str());
						fclose(f);
					}
					if (mysql_query(conn, sqlinsert.c_str()))
					{
						FILE *error = fopen("error.txt","w");
						fprintf(error, sqlinsert.c_str());
						fclose(error);
						printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
						exit(1);
					}
				}
			}
		}
		mysql_close(conn);
	}
	else
	{
		printf("dbc directory doesn't exist");
		exit(1);
	}
}