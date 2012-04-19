#include "DbcFile.h"

DbcFile::DbcFile()
{
	dataTable = NULL;
	stringTable = NULL;
	format = NULL;
	fieldsOffset = NULL;
}

bool DbcFile::Load(const char *filepath)
{
	uint32 header;

	//Delete data, if there is data
	Close();

	//Get the name
	name = filepath;
	name = name.substr(name.find_last_of("/") + 1);
	name.resize(name.size() - 4);

	//Open the Dbc files
	FILE *f = fopen(filepath, "rb");
	if (f == NULL)
	{
		printf("WARNING: Can't load '%s' - File Not Found - skipping it!\n", name.c_str());
		return false;
	}

	//Check if that file really is a Dbc file
	fread(&header, 4, 1, f);			// Signature
	if (header != 0x43424457)
	{
		printf("WARNING: Can't load '%s' - Not A Dbc File - skipping it!\n", name.c_str());
		return false;					// 'WDBC'
	}

	//Read the header
	fread(&recordCount, 4, 1, f);		// Number of records
	fread(&fieldCount, 4, 1, f);		// Number of fields
	fread(&recordSize, 4, 1, f);		// Size of a record
	fread(&stringSize, 4, 1, f);		// String size

	dataTable = new unsigned char[recordSize * recordCount];
	stringTable = new unsigned char[stringSize];
	fread(dataTable, recordSize * recordCount, 1, f);
	fread(stringTable, stringSize, 1, f);

	fclose(f);
	return true;
}

void DbcFile::Close()
{
	if (dataTable != NULL)
	{
		delete [] dataTable;
		dataTable = NULL;
	}
	if (stringTable != NULL)
	{
		delete [] stringTable;
		stringTable = NULL;
	}
	if (format != NULL)
	{
		delete [] format;
		format = NULL;
	}
	if (fieldsOffset != NULL)
	{
		delete [] fieldsOffset;
		fieldsOffset = NULL;
	}
}

DbcFile::~DbcFile()
{
	Close();
}

DbcFile::Record DbcFile::getRecord(uint32 id)
{
	assert(dataTable);
	return Record(*this, dataTable + id * recordSize);
}

string DbcFile::MakeMySqlStructure(TiXmlElement *pDbcFile, string name)
{
	if (pDbcFile != NULL)
	{	
		string sqlstructure;
		sqlstructure += "CREATE TABLE `";
		sqlstructure += name;
		sqlstructure += "` (\n";
		
		uint32 NumberOfFieldsDefined = 0;
		fieldsOffset = new uint32[fieldCount + 1];
		format = new uint32[fieldCount];
		uint32 k = 1;
		fieldsOffset[0] = 0;

		TiXmlElement *pField = pDbcFile->FirstChildElement("field");
		while(pField != NULL)
		{
			sqlstructure += " `";
			sqlstructure += pField->Attribute("name");
			sqlstructure += "`";
			
			const char *att = pField->Attribute("type");

			if(!strcmp(att, "uint8"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 1;
				format[k - 1] = FT_UINT8;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " TINYINT UNSIGNED NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					sqlstructure += Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "int8"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 1;
				format[k - 1] = FT_INT8;
				k++;
				NumberOfFieldsDefined++;
				
				sqlstructure += " TINYINT NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "uint16"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 2;
				format[k - 1] = FT_UINT16;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " SMALLINT UNSIGNED NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "int16"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 2;
				format[k - 1] = FT_INT16;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " SMALLINT NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "uint32"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 4;
				format[k - 1] = FT_UINT32;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " INT UNSIGNED NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "int32"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 4;
				format[k - 1] = FT_INT32;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " INT NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "uint64"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 8;
				format[k - 1] = FT_UINT64;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " BIGINT UNSIGNED NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "int64"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 8;
				format[k - 1] = FT_INT64;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " BIGINT NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "float"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 4;
				format[k - 1] = FT_FLOAT;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " FLOAT NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "string"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 4;
				format[k - 1] = FT_STRING;
				k++;
				NumberOfFieldsDefined++;

				sqlstructure += " TEXT NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else if(!strcmp(att, "loc"))
			{
				fieldsOffset[k] = fieldsOffset[k - 1] + 4;
				format[k - 1] = FT_STRING;
				k++;
				NumberOfFieldsDefined++;
				for (int x = 0; x < 16; x++, k++)
				{
					format[k - 1] = FT_IGNORED;
					fieldsOffset[k] = fieldsOffset[k - 1] + 4;
					NumberOfFieldsDefined++;
				}

				sqlstructure += " TEXT NOT NULL";
				if(pField->Attribute("comment") != NULL)
				{
					sqlstructure += " COMMENT '";
					Terminator((string)pField->Attribute("comment"));
					sqlstructure += "',\n";
				}
				else
					sqlstructure +=",\n";
			}
			else
			{
				printf("%s is not a valid type", Terminator((string)pDbcFile->Attribute("type")));
				exit(1);
			}
			pField = pField->NextSiblingElement("field");
		}

		if(pDbcFile->Attribute("primary") != NULL)
		{
			sqlstructure += " PRIMARY KEY (`";
			sqlstructure += pDbcFile->Attribute("primary");
			sqlstructure += "`)";
			sqlstructure += "\n) ENGINE=MyISAM DEFAULT CHARSET=utf8\n";
		}
		else
		{
			sqlstructure.resize(sqlstructure.size() - 2);
			sqlstructure += "\n) ENGINE=MyISAM DEFAULT CHARSET=utf8\n";
		}
		
		//Check if number of fields found in xml file coresponds to real field number
		if(NumberOfFieldsDefined != fieldCount)
		{
			printf("The number of fields that you defined in dbcstructure.xml(%d) is not equal to number of fields that the dbc has(%d).\nPress any key to exit.", NumberOfFieldsDefined, fieldCount);
			getchar();
			exit(1);
		}

		return sqlstructure;
	}
	else
	{
		string sqlstructure;
		sqlstructure += "CREATE TABLE `";
		sqlstructure += name;
		sqlstructure += "` (\n";
		
		fieldsOffset = new uint32[fieldCount + 1];
		format = new uint32[fieldCount];
		uint32 k = 1;
		fieldsOffset[0] = 0;

		for (uint32 i = 0; i < fieldCount; i++)
		{
			format[i] = FT_INT32;
			fieldsOffset[k] = fieldsOffset[i] + 4;
			k++;
			
			sqlstructure += " `Unknown_";
			char a[255];
			sprintf(a, "%u", i);
			sqlstructure += a;
			sqlstructure += "`";
			sqlstructure += " INT NOT NULL,\n";
		}
		
		sqlstructure.resize(sqlstructure.size() - 2);
		sqlstructure += "\n) ENGINE=MyISAM DEFAULT CHARSET=utf8\n";
		
		return sqlstructure;
	}
}

const char *DbcFile::Terminator(string str)
{
	if (str.size() == 0)
		return str.c_str();

	size_t found = str.find("\\");
	while (found != str.npos)
	{
		str.insert(found, "\\");
		found = str.find("\\", found+2);
	}

	found = str.find("'");
	while (found != str.npos)
	{
		str.insert(found, "\\");
		found = str.find("'", found+2);
	}

	found = str.find("\"");
	while (found != str.npos)
	{
		str.insert(found, "\\");
		found = str.find("\"", found+2);
	}

	found = str.find("\r\n");
	while (found != str.npos)
	{
		str.replace(found, 2, "\\r\\n");
		found = str.find("\r\n", found+1);
	}
	return str.c_str();
}
