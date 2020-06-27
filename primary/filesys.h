#pragma once

#include <fileapi.h>
#include <string>

class FileSystemItem
{
	HANDLE fSearchHandle;
	WIN32_FIND_DATAA findFileData;
	DWORD fAttributes;


public:

	FileSystemItem(std::string filename)
	{
		auto infoLevelId = FindExInfoBasic;
		auto searchOp = FindExSearchNameMatch;
		auto searchFilter = nullptr;
		DWORD dwAdditionalFlags = 0;

		fSearchHandle = FindFirstFileExA(filename.c_str(),
			infoLevelId,
			&findFileData,
			searchOp,
			searchFilter,
			dwAdditionalFlags);
	}

	~FileSystemItem()
	{
		FindClose(fSearchHandle);
	}

	bool isArchive() {return (fAttributes & FILE_ATTRIBUTE_ARCHIVE)!= 0;}
	bool isCompressed() { return (fAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0; }
	bool isDevice() { return (fAttributes & FILE_ATTRIBUTE_DEVICE) != 0; }
	bool isDirectory() { return (fAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
	bool isEncrypted() { return (fAttributes & FILE_ATTRIBUTE_ENCRYPTED) != 0; }
	bool isHidden() { return (fAttributes & FILE_ATTRIBUTE_HIDDEN) != 0; }
	bool isNormal() { return (fAttributes & FILE_ATTRIBUTE_NORMAL) != 0; }
	bool isNotContentIndexed() { return (fAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) != 0; }
	bool isOffline() { return (fAttributes & FILE_ATTRIBUTE_OFFLINE) != 0; }
	bool isReadOnly() { return (fAttributes & FILE_ATTRIBUTE_READONLY) != 0; }
	bool isReparsePoint() { return (fAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0; }
	bool isSparse() { return (fAttributes & FILE_ATTRIBUTE_SPARSE_FILE) != 0; }
	bool isSystem() { return (fAttributes & FILE_ATTRIBUTE_SYSTEM) != 0; }
	bool isTemporary() { return (fAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0; }
	bool isVirtual() { return (fAttributes & FILE_ATTRIBUTE_VIRTUAL) != 0; }

};