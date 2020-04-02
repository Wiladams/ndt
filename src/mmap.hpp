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

#include "w32.hpp"
#include <stdio.h>

class mmap
{
    HANDLE filehandle;
    HANDLE maphandle;

public:
    size_t size;
    uint8_t * data;

    // factory method
    static mmap create(const char *filename)
    {
        return mmap(filename);
    }

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
    
    virtual ~mmap() {close();}

    bool isValid() {return maphandle != INVALID_HANDLE_VALUE;}

    uint8_t * getPointer() {return data;}

    size_t length() {return size;}
    
    bool close()
	{
        if (data != nullptr) {
		    UnmapViewOfFile(data);
		    data = nullptr;
        }

	    if (maphandle != INVALID_HANDLE_VALUE) {
		    CloseHandle(maphandle);
		    maphandle = INVALID_HANDLE_VALUE;
        }

	    if (filehandle != INVALID_HANDLE_VALUE) {
		    CloseHandle(filehandle);
		    filehandle = INVALID_HANDLE_VALUE;
        }

        return true;
    }



};
