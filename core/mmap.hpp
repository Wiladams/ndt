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

#include <windows.h>
#include <cstdio>
#include <string>
#include <cstdint>

class mmap
{
    HANDLE fFileHandle;
    HANDLE fMapHandle;
    size_t fSize;
    void * fData;

    bool fIsValid;

    mmap()
        : fFileHandle(nullptr),
        fMapHandle(nullptr),
        fIsValid(false),
        fSize(0),
        fData(nullptr)
    {}

    mmap(HANDLE filehandle, HANDLE maphandle, void *data, size_t length)
        :fFileHandle(filehandle),
        fMapHandle(maphandle),
        fData(data),
        fSize(length)
    {
        fIsValid = true;
    }

public:

/*
    mmap(const char * filename)
        : size(0),
        data(nullptr),
        filehandle(INVALID_HANDLE_VALUE),
        maphandle(INVALID_HANDLE_VALUE)
    {	
        // Open file
        //print("mmap:__new - ", filename)
        filehandle = CreateFileA(filename,
            GENERIC_READ, 
            0, 
            nullptr,
            OPEN_EXISTING, 
            (FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_RANDOM_ACCESS), 
            nullptr);
    
    //print("    File Handle: ", filehandle)
    
        if (filehandle == INVALID_HANDLE_VALUE) {
            // BUGBUG - throw exception
		    //error("Could not create/open file for mmap: "..tostring(ffi.C.GetLastError()))
            return;
        }
	
        // Set file size if new
        // print("GET File Size")

        bool exists = filehandle != INVALID_HANDLE_VALUE;
        if (exists) {
		    size_t fsize = GetFileSize(filehandle, nullptr);

		    if (fsize == 0) {
			    // Windows will error if mapping a 0-length file, fake a new one
			    exists = false;
                return;
            } else {
			    size = fsize;
            }
	    } else {
		    return;
	    }

	    // Open mapping
        maphandle = CreateFileMappingA(filehandle, nullptr, PAGE_READONLY, 0, size, nullptr);
        //printf("CREATE File Mapping: ", maphandle)
	    
        if (maphandle == INVALID_HANDLE_VALUE) {
		    //error("Could not create file map: "..tostring(ffi.C.GetLastError()))
            // close file handle and set it to invalid
            CloseHandle(filehandle);
            filehandle = INVALID_HANDLE_VALUE;

            return ;
        }
	
	    // Open view
	    data = (uint8_t *)MapViewOfFile(maphandle, FILE_MAP_READ, 0, 0, 0);
	    //print("MAP VIEW: ", m.map)
	    if (data == nullptr) {
            CloseHandle(maphandle);
            CloseHandle(filehandle);
            maphandle = INVALID_HANDLE_VALUE;
            filehandle = INVALID_HANDLE_VALUE;

            return ;
        }
    }
    */
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
    static mmap create(const std::string &filename, uint32_t desiredAccess=GENERIC_READ, uint32_t shareMode=FILE_SHARE_READ, uint32_t disposition= OPEN_EXISTING)
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
	
        uint64_t size = ::GetFileSize(filehandle, nullptr);

	    // Open mapping
        HANDLE maphandle = CreateFileMappingA(filehandle, nullptr, PAGE_READONLY, 0, size, nullptr);
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

        return mmap(filehandle, maphandle, data, size);
    }
};
