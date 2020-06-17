#pragma once

#include "binstream.hpp"
#include "mmap.hpp"

class FileStream : public BinStream
{
	std::string fFileName;
	std::shared_ptr<mmap> fFileMap;

public:
	// BUGBUG
	// should be able to specify read/write/create
	// currently assume readonly
	// default to little-endian 
	// If the file can not be mapped, then the data pointer
	// of the BinStream will not be set and isValid() will 
	// be false.
	FileStream(const char* filename, bool littleendian = true)
		: BinStream(nullptr, 0, 0, littleendian),
		fFileName(filename)
	{
		fFileMap = mmap::create_shared(filename);
		if (fFileMap->isValid())
		{
			setData((uint8_t *)fFileMap->data(), fFileMap->size());
		}
	}
};
