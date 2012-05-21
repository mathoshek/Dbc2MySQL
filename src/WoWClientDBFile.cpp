#include "WoWClientDBFile.h"

WoWClientDBFile::WoWClientDBFile()
{
	_dataTable = NULL;
	_stringTable = NULL;
	_format = NULL;
}

WoWClientDBFile::~WoWClientDBFile()
{
	Close();
}

void WoWClientDBFile::Close()
{
	if( _dataTable != NULL )
	{
		delete [] _dataTable;
		_dataTable = NULL;
	}

	if( _stringTable != NULL )
	{
		delete [] _stringTable;
		_stringTable = NULL;
	}

	if( _format != NULL )
	{
		delete [] _format;
		_format = NULL;
	}
}

uint8 WoWClientDBFile::Record::getUInt8( uint32 field ) const
{
	uint8 val = *reinterpret_cast<uint8 *>( offset + getOffset( field ) );
	return val;
}
int8 WoWClientDBFile::Record::getInt8(uint32 field) const
{
	int8 val = *reinterpret_cast<int8 *>(offset + getOffset( field ) );
	return val;
}

uint16 WoWClientDBFile::Record::getUInt16(uint32 field) const
{
	uint16 val = *reinterpret_cast<uint16 *>(offset + getOffset( field ) );
	return val;
}

int16 WoWClientDBFile::Record::getInt16(uint32 field) const
{
	int16 val = *reinterpret_cast<int16 *>(offset + getOffset( field ) );
	return val;
}

uint32 WoWClientDBFile::Record::getUInt32(uint32 field) const
{
	uint32 val = *reinterpret_cast<uint32 *>(offset + getOffset( field ) );
	return val;
}

int32 WoWClientDBFile::Record::getInt32(uint32 field) const
{
	int32 val = *reinterpret_cast<int32 *>(offset + getOffset( field ) );
	return val;
}

uint64 WoWClientDBFile::Record::getUInt64(uint32 field) const
{
	uint64 val = *reinterpret_cast<uint64 *>(offset + getOffset( field ) );
	return val;
}

int64 WoWClientDBFile::Record::getInt64(uint32 field) const
{
	int64 val = *reinterpret_cast<int64 *>(offset + getOffset( field ) );
	return val;
}

float WoWClientDBFile::Record::getFloat(uint32 field) const
{
	float val = *reinterpret_cast<float *>(offset + getOffset( field ) );
	return val;
}

double WoWClientDBFile::Record::getDouble(uint32 field) const
{
	double val = *reinterpret_cast<double *>(offset + getOffset( field ) );
	return val;
}

const char* WoWClientDBFile::Record::getString(uint32 field) const
{
	char *val = NULL;

	if( stringTable == NULL )
		val = reinterpret_cast<char *>( offset + getOffset( field ) );

	val = reinterpret_cast<char *>( stringTable + getUInt32( field ) );
	return val;
}

WoWClientDBFile::Record::Record(uint8 *offset_, uint32 *fieldOffset_, uint32 fieldCount_, uint8 *stringTable_)
{
	offset = offset_;
	fieldOffset = fieldOffset_;
	stringTable = stringTable_;
	fieldCount = fieldCount_;
}

WoWClientDBFile::Record WoWClientDBFile::getRecord( uint32 id )
{
	assert( _dataTable );
	return Record(_dataTable + getRecordOffset( id ), getFieldOffset( id ), _fieldCount, _stringTable );
}

uint32 WoWClientDBFile::Record::getOffset( uint32 field ) const
{
	assert( field < fieldCount );
	return fieldOffset[field];
}
