#pragma once

/*
	mmap is the rough equivalent of the mmap() function on Linux
	This basically allows you to memory map a file, which means you 
	can access a pointer to the file's contents without having to 
	go through IO routines.

	Usage:
	local m = mmap(filename)

	local bs = binstream(m:getPointer(), #m)
*/
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <string>
#include <cstdint>
#include <memory>

class mmap
{
    HANDLE fFileHandle;
    HANDLE fMapHandle;
    size_t fSize;
    void * fData;

    bool fIsValid;




public:
    mmap(HANDLE filehandle, HANDLE maphandle, void* data, size_t length)
        :fFileHandle(filehandle),
        fMapHandle(maphandle),
        fData(data),
        fSize(length)
    {
        fIsValid = true;
    }


    mmap()
        : fFileHandle(nullptr),
        fMapHandle(nullptr),
        fIsValid(false),
        fSize(0),
        fData(nullptr)
    {}

    virtual ~mmap() {close();}

    bool isValid() {return fIsValid;}
    void * data() {return fData;}
    size_t size() {return fSize;}
    
    bool close()
	{
        if (fData != nullptr) {
		    UnmapViewOfFile(fData);
		    fData = nullptr;
        }

	    if (fMapHandle != INVALID_HANDLE_VALUE) {
		    CloseHandle(fMapHandle);
		    fMapHandle = INVALID_HANDLE_VALUE;
        }

	    if (fFileHandle != INVALID_HANDLE_VALUE) {
		    CloseHandle(fFileHandle);
		    fFileHandle = INVALID_HANDLE_VALUE;
        }

        return true;
    }


    // factory method
    // desiredAccess - GENERIC_READ, GENERIC_WRITE, GENERIC_EXECUTE
    // shareMode - FILE_SHARE_READ, FILE_SHARE_WRITE
    // creationDisposition - CREATE_ALWAYS, CREATE_NEW, OPEN_ALWAYS, OPEN_EXISTING, TRUNCATE_EXISTING
    static std::shared_ptr<mmap> create_shared(const std::string &filename, 
        uint32_t desiredAccess=GENERIC_READ, 
        uint32_t shareMode=FILE_SHARE_READ, 
        uint32_t disposition= OPEN_EXISTING)
    {
        uint32_t flagsAndAttributes = (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS);

        HANDLE filehandle = CreateFileA(filename.c_str(), 
            desiredAccess, 
            shareMode, 
            nullptr,
            disposition, 
            flagsAndAttributes, 
            nullptr);
    
    //print("mmap.create() -    File Handle: ", filehandle)
    
        if (filehandle == INVALID_HANDLE_VALUE) {
            // BUGBUG - do anything more than returning invalid?
		    //error("Could not create/open file for mmap: "..tostring(ffi.C.GetLastError()))
            return {};
        }
	
        // BUGBUG
        // Need to check whether we're opening for writing or not
        // if we're opening for writing, then we don't want to 
        // limit the size in CreateFileMappingA
        LARGE_INTEGER psize;
        BOOL bResult = GetFileSizeEx(filehandle, &psize);
        int64_t size = psize.QuadPart;

	    // Open mapping
        HANDLE maphandle = CreateFileMappingA(filehandle, nullptr, PAGE_READONLY, psize.HighPart, psize.LowPart, nullptr);
        //printf("CREATE File Mapping: ", maphandle)
	    
        if (maphandle == INVALID_HANDLE_VALUE) 
        {
		    //error("Could not create file map: "..tostring(ffi.C.GetLastError()))
            // close file handle and set it to invalid
            CloseHandle(filehandle);
            filehandle = INVALID_HANDLE_VALUE;

            return {} ;
        }
	
	    // Open view
	    void * data = MapViewOfFile(maphandle, FILE_MAP_READ, 0, 0, 0);
	    //print("MAP VIEW: ", m.map)
	    
        if (data == nullptr) {
            CloseHandle(maphandle);
            CloseHandle(filehandle);
            maphandle = INVALID_HANDLE_VALUE;
            filehandle = INVALID_HANDLE_VALUE;

            return {};
        }

        return std::make_shared<mmap>(filehandle, maphandle, data, size);
    }
};
