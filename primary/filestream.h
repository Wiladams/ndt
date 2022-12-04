#pragma once

/*
	FileStream

	A marriage between a BinStream and a 
	memory mapped file.  Construct from a filename
	then treat it as a BinStream.

	Currently only read only.  Additional 
	constructors in the future should be added to 
	support creating file in write mode as well.
*/

#include "binstream.hpp"
#include "mmap.hpp"

namespace ndt {
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
		FileStream(const std::string& s, bool littleendian = true)
			:FileStream(s.c_str(), littleendian)
		{
		}

		FileStream(const char* filename, bool littleendian = true)
			: BinStream(nullptr, 0, 0, littleendian),
			fFileName(filename)
		{
			fFileMap = mmap::create_shared(filename);
			if ((fFileMap != nullptr) && fFileMap->isValid())
			{
				setData((uint8_t*)fFileMap->data(), fFileMap->size());
			}
		}

		virtual ~FileStream()
		{
			close();
		}

		void close()
		{
			if ((fFileMap != nullptr) && fFileMap->isValid())
			{
				fFileMap->close();
			}
		}
	};
}
