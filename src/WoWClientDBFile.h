#ifndef WOWCLIENTDBFILE_H
#define WOWCLIENTDBFILE_H

#include "defines.h"

enum DataTypes
{
	FT_UINT8 = 0,
	FT_INT8,
	FT_UINT16,
	FT_INT16,
	FT_UINT32,
	FT_INT32,
	FT_UINT64,
	FT_INT64,
	FT_FLOAT,
	FT_DOUBLE,
	FT_STRING,
	FT_IGNORED,
};

class WoWClientDBFile
{
public:
	WoWClientDBFile();
	~WoWClientDBFile();

	virtual bool Load(string filePath, vector<uint32> format) = 0;
	void Close();

	class Record
	{
	public:
		uint8 getUInt8(uint32 field) const;

		int8 getInt8(uint32 field) const;

		uint16 getUInt16(uint32 field) const;

		int16 getInt16(uint32 field) const;

		uint32 getUInt32(uint32 field) const;

		int32 getInt32(uint32 field) const;

		uint64 getUInt64(uint32 field) const;

		int64 getInt64(uint32 field) const;

		float getFloat(uint32 field) const;

		double getDouble(uint32 field) const;

		const char *getString(uint32 field) const;

	private:
		Record(uint8 *offset_, uint32 *fieldOffset_, uint32 fieldCount_, uint8 *stringTable_);
		uint8 *offset;
		uint32 *fieldOffset;
		uint32 fieldCount;
		uint8 *stringTable;

		uint32 getOffset( uint32 field ) const;

		friend class WoWClientDBFile;
	};

	Record getRecord(uint32 id);

	inline uint32 getRecordCount() { return _recordCount; }
	inline uint32 getFieldCount() { return _fieldCount; }
	inline string getName() { return _name; }
	inline uint32 getFormat(uint32 field) { assert(field < _fieldCount); return _format[field]; }
protected:
	virtual uint32 getRecordOffset( uint32 id ) = 0;
	virtual uint32* getFieldOffset( uint32 id ) = 0;

	// Must be filled in Load function
	uint32 *_format;
	uint8 *_dataTable;
	uint8 *_stringTable;
	string _name;
	uint32 _recordCount;
	uint32 _fieldCount;
};

#endif//WOWCLIENTDBFILE_H

