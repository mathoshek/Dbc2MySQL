#ifndef DB2FILE_H
#define DB2FILE_H

#include "WoWClientDBFile.h"

class Db2File : public WoWClientDBFile
{
public:
	bool Load(string filePath, vector<uint32> format);

private:
	uint32 _recordSize;
	uint32 _stringOffset;
	uint32 *_fieldOffset;

	uint32 getRecordOffset( uint32 id );
	uint32 *getFieldOffset( uint32 id );
	bool BuildFormat( vector<uint32> Format );
};

#endif//DB2FILE_H