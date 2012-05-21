#include "System.h"

System::System()
{
	db_host = "localhost";
	db_port = 3306;
	dbc_db = "dbc";
	createSqlFiles = false;
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
			case 'o':
				if(i + 1 < argc)
					db_port = atoi(argv[i + 1]);
				else
					ShowUsage(argv[0]);
				break;
			}
			i++;
		}
	}
	if (db_user.empty() || db_pass.empty() || structureXml.empty())
	{
		printf("Some information missing, please check again\n");
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
		"-h set mysql host (OPTIONAL - default: localhost)\n"\
		"-o set mysql port (OPTIONAL - default: 3306)\n"\
		"-d set mysql database (OPTIONAL - default: dbc)\n"\
		"-s (yes/no) dump all dbc to sql files (OPTIONAL - default: no)\n"\
		"-x set the structure.xml file\n"\

		"Example: %s -u root -p pass -x StructureWotLK.xml\n\n", prg, prg);
	exit(1);
}

bool System::CreateDir( const char* Path )
{
#ifdef _WIN32
	if( CreateDirectory( Path, NULL ) == TRUE )
		return true;
#elif __unix__
	if( mkdir( Path, 0777 ) == 0 )
		return true;
#else
	//
#endif
	return false;
}

void System::ExportDBCs()
{
	FILE *f;
	MySqlConnection MySql(BUFFER_LEN, db_host.c_str( ), db_user.c_str( ), db_pass.c_str( ), NULL, db_port, NULL, 0 );

	MySql.Append( "DROP DATABASE IF EXISTS `%s`;\n", dbc_db.c_str( ) );
	if( MySql.SendQuery( ) )
		HandleErrorMessage("Error %u: %s\n%s\n", MySql.GetErrNo( ), MySql.GetError( ), MySql.DumpCommand( ) );

	MySql.Append( "CREATE DATABASE IF NOT EXISTS `%s` CHARACTER SET utf8;\n", dbc_db.c_str( ) );
	if( MySql.SendQuery( ) )
		HandleErrorMessage("Error %u: %s\n%s\n", MySql.GetErrNo( ), MySql.GetError( ), MySql.DumpCommand( ) );

	MySql.Append( "USE `%s`;\n", dbc_db.c_str( ) );
	if( MySql.SendQuery( ) )
		HandleErrorMessage("Error %u: %s\n%s\n", MySql.GetErrNo( ), MySql.GetError( ), MySql.DumpCommand( ) );

	//Open the XML
	TiXmlDocument doc((homePath + structureXml).c_str());
	TiXmlElement *pRoot = NULL;
	if (doc.LoadFile())
	{
		printf("\"%s\" loaded\n", structureXml.c_str());
		pRoot = doc.FirstChildElement("DBFilesClient");
		if(pRoot == NULL)
			HandleErrorMessage("Couldn't find XML element DBFilesClient. Check the provided XML file.");
	}
	else
		HandleErrorMessage("Failed to load file \"%s\"\n", structureXml.c_str());

	string sqlDir = homePath + "sql/";
	if(createSqlFiles == true)
		CreateDir(sqlDir.c_str( ));

	WoWClientDBFile *file = NULL;

	string dbcDir = homePath + "dbc/";
	//const char* p = GetFileInDir( dbcDir.c_str( ) );
	for(const char* p = GetFileInDir( dbcDir.c_str( ) ); p != NULL; p = GetFileInDir( NULL ) )
	{
		const char *dot = strrchr( p, '.' );
		if( !dot )
			continue;

		if( strcmp( dot + 1, "dbc" ) == 0 )
			file = new DbcFile();
		else
			continue;

		printf("Found %s \n", p);
		TiXmlElement *pDbcFile = NULL;
		vector<uint32> format;
		pDbcFile = pRoot->FirstChildElement( "table" );
		while( pDbcFile != NULL )
			if( strcmp( pDbcFile->Attribute( "name" ), p ) == 0 )
				break;
			else
				pDbcFile = pDbcFile->NextSiblingElement( );

		MySql.ResetCommand( );
		if( pDbcFile != NULL )
		{
			MySql.Append( "CREATE TABLE `%s`(\n", p );
			TiXmlElement *pField = pDbcFile->FirstChildElement( "field" );
			while( pField != NULL )
			{
				MySql.Append( "  `%s` ", pField->Attribute( "name" ) );
				const char *att = pField->Attribute( "type" );

				if( !strcmp( att, "uint8" ) )
				{
					MySql.Append( "TINYINT UNSIGNED " );
					format.push_back( FT_UINT8 );
				}
				else if( !strcmp( att, "int8" ) )
				{
					MySql.Append( "TINYINT " );
					format.push_back( FT_INT8 );
				}
				else if( !strcmp( att, "uint16" ) )
				{
					MySql.Append( "SMALLINT UNSIGNED " );
					format.push_back( FT_UINT16 );
				}
				else if( !strcmp( att, "int16" ) )
				{
					MySql.Append( "SMALLINT " );
					format.push_back( FT_INT16 );
				}
				else if( !strcmp( att, "uint32" ) )
				{
					MySql.Append( "INT UNSIGNED " );
					format.push_back(FT_UINT32);
				}
				else if( !strcmp( att, "int32" ) )
				{
					MySql.Append( "INT " );
					format.push_back( FT_INT32 );
				}
				else if( !strcmp( att, "uint64" ) )
				{
					MySql.Append( "BIGINT UNSIGNED " );
					format.push_back( FT_UINT64 );
				}
				else if( !strcmp( att, "int64" ) )
				{
					MySql.Append( "BIGINT " );
					format.push_back( FT_INT64 );
				}
				else if( !strcmp( att, "float" ) )
				{
					MySql.Append( "FLOAT " );
					format.push_back( FT_FLOAT );
				}
				else if( !strcmp( att, "double" ) )
				{
					MySql.Append( "DOUBLE " );
					format.push_back( FT_DOUBLE );
				}
				else if( !strcmp( att, "string" ) )
				{
					MySql.Append( "TEXT " );
					format.push_back( FT_STRING );
				}
				else if( !strcmp( att, "loc" ) )
				{
					MySql.Append( "TEXT " );
					format.push_back( FT_STRING );
					for( uint32 x = 0; x < 16; x++ )
						format.push_back( FT_IGNORED );
				}
				else if( !strcmp( att, "ignored" ) )
					format.push_back( FT_IGNORED );
				else
					HandleErrorMessage( "%s is not a valid type", att );

				MySql.Append( "NOT NULL,\n" );
				pField = pField->NextSiblingElement( "field" );
			}
			if( pDbcFile->Attribute( "primary" ) != NULL )
				MySql.Append( " PRIMARY KEY (`%s`)", pDbcFile->Attribute( "primary" ) );
			else
				MySql.EraseCommand( 2 );
			MySql.Append( "\n) ENGINE=MyISAM DEFAULT CHARSET=utf8\n" );
		}

		if( file->Load(dbcDir + p, format) == false)
		{
			delete file;
			file = NULL;
			continue;
		}

		if( format.size() == 0 )
		{
			MySql.Append( "CREATE TABLE `%s`(\n", p );
			for( uint32 i = 0; i < file->getFieldCount( ) - 1; i++ )
				MySql.Append( "  `unk_uint32_%u` INT UNSIGNED NOT NULL,\n", i );
			MySql.Append( "  `unk_uint32_%u` INT UNSIGNED NOT NULL\n) ENGINE=MyISAM DEFAULT CHARSET=utf8\n", file->getFieldCount( ) - 1 );
		}

		if( createSqlFiles == true )
		{
			f = fopen( (sqlDir + file->getName( ) + ".sql").c_str( ), "w" );
			fprintf( f, "%s", MySql.DumpCommand( ) );
		}

		if ( MySql.SendQuery( ) )
			HandleErrorMessage("Error %u: %s\nCommand: %s\n", MySql.GetErrNo( ), MySql.GetError( ), MySql.DumpCommand( ) );

		MySql.Append( "INSERT INTO `%s` VALUES\n", file->getName( ).c_str( ) );

		for (uint32 i = 0; i < file->getRecordCount(); i++)
		{
			if( MySql.IsBufferFull( ) )
			{
				MySql.RestoreCommandToMarker( );
				MySql.EraseCommand( 2 );
				if( createSqlFiles == true )
					fprintf( f, "%s", MySql.DumpCommand( ) );
				if ( MySql.SendQuery( ) )
					HandleErrorMessage("Error %u: %s\nCommand: %s\n", MySql.GetErrNo( ), MySql.GetError( ), MySql.DumpCommand( ) );

				MySql.Append( "INSERT INTO `%s` VALUES\n", file->getName( ).c_str( ) );
				i--;
			}
			MySql.InsertCommandMarker( );
			MySql.Append( "(" );

			for(uint32 j = 0; j < file->getFieldCount(); j++)
			{
				if(j != 0 && file->getFormat(j) != FT_IGNORED)
					MySql.Append( "," );
				switch ( file->getFormat( j ) )
				{
				case FT_UINT8:  MySql.Append( "'%u'", file->getRecord( i ).getUInt8( j ) ); break;
				case FT_INT8:   MySql.Append( "'%i'", file->getRecord( i ).getInt8( j ) ); break;
				case FT_UINT16: MySql.Append( "'%u'", file->getRecord( i ).getUInt16( j ) ); break;
				case FT_INT16:  MySql.Append( "'%i'", file->getRecord( i ).getInt16( j ) ); break;
				case FT_UINT32: MySql.Append( "'%u'", file->getRecord( i ).getUInt32( j ) ); break;
				case FT_INT32:  MySql.Append( "'%i'", file->getRecord( i ).getInt32( j ) ); break;
				case FT_UINT64: MySql.Append( "'%lu'", file->getRecord( i ).getUInt64( j ) ); break;
				case FT_INT64:  MySql.Append( "'%li'", file->getRecord( i ).getInt64( j ) ); break;
				case FT_FLOAT:  MySql.Append( "'%f'", file->getRecord( i ).getFloat( j ) ); break;
				case FT_DOUBLE: MySql.Append( "'%f'", file->getRecord( i ).getDouble( j ) ); break;
				case FT_STRING: MySql.Append( "'%s'", Terminator( file->getRecord( i ).getString( j ) ).c_str( ) ); break;
				}
				if (j == file->getFieldCount() - 1)
					MySql.Append( "),\n" );
			}
		}
		MySql.EraseCommand( 2 );
		if( file->getRecordCount() != 0 )
		{
			if( createSqlFiles == true )
			{
				fprintf( f, "%s", MySql.DumpCommand( ) );
				fclose( f );
			}
			if( MySql.SendQuery ( ) )
				HandleErrorMessage("Error %u: %s\n%s\n", MySql.GetErrNo( ), MySql.GetError( ), MySql.DumpCommand( ) );
		}
		delete file;
		file = NULL;
	}
}

void System::HandleErrorMessage(const char *fmt, ...)
{
	char msg[BUFFER_LEN];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(msg, BUFFER_LEN, fmt, ap);
	va_end(ap);

	FILE *f = fopen("error.log", "a"); 
	time_t tmm = time ( NULL );
	struct tm * timeStruct = localtime( &tmm );
	fprintf(f, "%02u:%02u:%02u - %s\n", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec, msg );
	fclose(f);

	//Important, NO NOT REMOVE, unless you check all the calls of this function
	printf("Fatal error occured... check error.log file for details.\n");

	exit(1);
}

const char* System::GetFileInDir( const char *dir )
{
#ifdef _WIN32
	if( dir != NULL )
	{
		char directory[256];
		strcpy( directory, dir );
		strcat( directory, "*" );
		hFind = FindFirstFile( directory, &findFileInfo );
		if( hFind == INVALID_HANDLE_VALUE )
			return NULL;
	}
	BOOL test = FindNextFile( hFind, &findFileInfo );
	while( test == TRUE && ( findFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		test = FindNextFile( hFind, &findFileInfo );
	if( test == FALSE )
	{
		FindClose( hFind );
		return NULL;
	}
	return findFileInfo.cFileName;
#elif __unix__
	if( dir != NULL )
	{
		dirp = opendir( dir );
		if( dirp == NULL )
			return NULL;
	}
	dp = readdir( dirp );
	while( dp != NULL && dp->d_type != DT_REG )
		dp = readdir( dirp );
	if( dp == NULL )
	{
		closedir( dirp );
		return NULL;
	}
	return dp->d_name;
#else
	return NULL;
#endif
}
