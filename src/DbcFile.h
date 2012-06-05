#ifndef __DBCFILE_H__
#define __DBCFILE_H__

#include "WoWClientDBFile.h"

class DbcFile : public WoWClientDBFile
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

#endif//__DBCFILE_H__
