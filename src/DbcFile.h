#ifndef DBCFILE_H
#define DBCFILE_H

#include "defines.h"
#include "tinyxml.h"

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
	FT_STRING,
	FT_IGNORED
};

class DbcFile
{
public:
	DbcFile();
	~DbcFile();

	class Record
	{
	public:
		uint8 getUInt8(uint32 field) const
		{
			assert(field < file.fieldCount);
			uint8 val = *reinterpret_cast<uint8 *>(offset + file.GetOffset(field));
			return val;
		}
		
		int8 getInt8(uint32 field) const
		{
			assert(field < file.fieldCount);
			int8 val = *reinterpret_cast<int8 *>(offset + file.GetOffset(field));
			return val;
		}

		uint16 getUInt16(uint32 field) const
		{
			assert(field < file.fieldCount);
			uint16 val = *reinterpret_cast<uint16 *>(offset + file.GetOffset(field));
			return val;
		}
		
		int16 getInt16(uint32 field) const
		{
			assert(field < file.fieldCount);
			int16 val = *reinterpret_cast<int16 *>(offset + file.GetOffset(field));
			return val;
		}

		uint32 getUInt32(uint32 field) const
		{
			assert(field < file.fieldCount);
			uint32 val = *reinterpret_cast<uint32 *>(offset + file.GetOffset(field));
			return val;
		}

		int32 getInt32(uint32 field) const
		{
			assert(field<file.fieldCount);
			int32 val = *reinterpret_cast<int32 *>(offset + file.GetOffset(field));
			return val;
		}

		uint64 getUInt64(uint32 field) const
		{
			assert(field < file.fieldCount);
			uint64 val = *reinterpret_cast<uint64 *>(offset + file.GetOffset(field));
			return val;
		}

		int64 getInt64(uint32 field) const
		{
			assert(field<file.fieldCount);
			int64 val = *reinterpret_cast<int64 *>(offset + file.GetOffset(field));
			return val;
		}

		float getFloat(uint32 field) const
		{
			assert(field < file.fieldCount);
			float val = *reinterpret_cast<float *>(offset + file.GetOffset(field));
			return val;
		}

		const char *getString(uint32 field) const
		{
			assert(field < file.fieldCount);
			size_t stringOffset = getUInt32(field);
			assert(stringOffset < file.stringSize);
			return reinterpret_cast<char *>(file.stringTable + stringOffset);
		}

	private:
		Record(DbcFile &file_, unsigned char *offset_) : offset(offset_), file(file_) {}
		unsigned char *offset;
		DbcFile &file;

		friend class DbcFile;
	};

	Record getRecord(uint32 id);

	uint32 GetNumRows() const { return recordCount; }
	uint32 GetNumCols() const { return fieldCount; }
	uint32 GetRecordSize() const { return recordSize; }
	uint32 GetStringSize() const { return stringSize; }
	string GetName() const { return name; }
	uint32 GetOffset(uint32 id) const { return (fieldsOffset != NULL && id < fieldCount) ? fieldsOffset[id] : 0; }
	uint32 GetFormat(uint32 id) const { return (format != NULL && id < fieldCount) ? format[id] : 99;}
	bool IsLoaded() { return (dataTable != NULL); }
	
	bool Load(const char *filepath);
	void Close();

	//Based on the info from a given TiXmlElement this will return the CREATE TABLE query.
	//If TiXmlElement is NULL, the table fields will have Unknown_?? names.
	//It also generate the format and fieldsOffset arrays, both needed for data extraction.
	string MakeMySqlStructure(TiXmlElement *pDbcFile, string name);

	const char *Terminator(string &str);

private:
	uint32 recordSize;
	uint32 recordCount;
	uint32 fieldCount;
	uint32 stringSize;
	uint32 *fieldsOffset;
	uint32 *format;
	string name;
	unsigned char *dataTable;
	unsigned char *stringTable;
};
#endif
