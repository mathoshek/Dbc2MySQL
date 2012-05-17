#include "DbcFile.h"

bool DbcFile::Load(string filePath, vector<uint32> format)
{
	uint32 header;

	//Delete data, if there is data
	Close();

	//Get the name
	_name = filePath.substr(filePath.find_last_of("/") + 1);

	//Open the Dbc files
	FILE *f = fopen(filePath.c_str(), "rb");
	if (f == NULL)
	{
		printf("WARNING: Can't load '%s' - File Not Found - skipping it!\n", filePath.c_str());
		return false;
	}

	//Check if that file really is a Dbc file
	fread(&header, 4, 1, f);			// Signature
	if (header != 0x43424457)
	{
		printf("WARNING: Can't load '%s' - Not A Dbc File - skipping it!\n", filePath.c_str());
		fclose(f);
		return false;					// 'WDBC'
	}

	uint32 _stringSize;

	//Read the header
	fread(&_recordCount, 4, 1, f);		// Number of records
	fread(&_fieldCount, 4, 1, f);		// Number of fields
	fread(&_recordSize, 4, 1, f);		// Size of a record
	fread(&_stringSize, 4, 1, f);		// String size

	_dataTable = new uint8[_recordSize * _recordCount];
	fread(_dataTable, _recordSize * _recordCount, 1, f);

	_stringTable = new uint8[_stringSize];
	fread(_stringTable, _stringSize, 1, f);

	_stringOffset = _recordCount * _recordSize;

	fclose(f);

	_format = new uint32[_fieldCount];
	_fieldOffset = new uint32[_fieldCount];

	if(format.size() == 0)
	{
		if(_fieldCount * 4 == _recordSize)
			return BuildFormat(vector<uint32>(_fieldCount, FT_UINT32));
		else
		{
			printf("WARNING: Can't load '%s' - cannot find a default format - skipping it\n", _name.c_str());
			return false;
		}
	}

	return BuildFormat(format);
}

uint32 DbcFile::getRecordOffset( uint32 id )
{
	return id * _recordSize;
}

uint32 *DbcFile::getFieldOffset( uint32 field )
{
	return _fieldOffset;
}

bool DbcFile::BuildFormat( vector<uint32> Format )
{
	if( _dataTable == NULL )
		return false;
	if(_fieldCount != Format.size() )
	{
		printf("WARNING: The field count mismatch (defined:%u, real:%lu)\n", Format.size(), _fieldCount);
		return false;
	}
	_format[0] = Format[0];
	_fieldOffset[0] = 0;
	for(uint32 i = 1; i < Format.size(); i++)
	{
		_format[i] = Format[i];
		switch(_format[i])
		{
		case FT_INT8:
		case FT_UINT8:
			_fieldOffset[i] = _fieldOffset[i-1] + 1;
			break;
		case FT_INT16:
		case FT_UINT16:
			_fieldOffset[i] = _fieldOffset[i-1] + 2;
			break;
		case FT_INT32:
		case FT_UINT32:
		case FT_FLOAT:
		case FT_STRING:
		case FT_IGNORED:
			_fieldOffset[i] = _fieldOffset[i-1] + 4;
			break;
		case FT_INT64:
		case FT_UINT64:
		case FT_DOUBLE:
			_fieldOffset[i] = _fieldOffset[i-1] + 8;
			break;
		}
	}
	return true;
}

