#include "apphost.h"

//
// https://cpp.hotexamples.com/examples/-/-/GetUserObjectInformationA/cpp-getuserobjectinformationa-function-examples.html
//
struct Desktop
{
private:
	HDESK fHandle;
	std::string fName;

	void open()
	{
		//ACCESS_MASK dwDesiredAccess = GENERIC_ALL;
		ACCESS_MASK dwDesiredAccess = DESKTOP_ENUMERATE;
		/*
		ACCESS_MASK dwDesiredAccess = DESKTOP_SWITCHDESKTOP |
			DESKTOP_WRITEOBJECTS |
			DESKTOP_READOBJECTS |
			DESKTOP_ENUMERATE |
			DESKTOP_CREATEWINDOW |
			DESKTOP_CREATEMENU;
			*/

		fHandle = OpenDesktopA(fName.c_str(), 0, TRUE, dwDesiredAccess);
		auto err = GetLastError();

		printf("Error: 0x%X\n", err);
	}

public:
	Desktop(const char* name)
		:fName(name)
	{
		open();
	}

	virtual ~Desktop() 
	{ 
		//close(); 
	}

	const std::string& name() const { return fName; }

	void close()
	{
		::CloseDesktop(fHandle); 
	}

	void printUserObjects()
	{
		uint8_t buff[256];
		int nLength = 256;
		DWORD lengthNeeded;
		USEROBJECTFLAGS objflags;

		GetUserObjectInformationA(fHandle, UOI_FLAGS, &objflags, sizeof(USEROBJECTFLAGS), &lengthNeeded);
		GetUserObjectInformationA(fHandle, UOI_HEAPSIZE, buff, nLength, &lengthNeeded);
		GetUserObjectInformationA(fHandle, UOI_IO, buff, nLength, &lengthNeeded);
		GetUserObjectInformationA(fHandle, UOI_NAME, buff, nLength, &lengthNeeded);
		GetUserObjectInformationA(fHandle, UOI_TYPE, buff, nLength, &lengthNeeded);
		GetUserObjectInformationA(fHandle, UOI_USER_SID, nullptr, nLength, &lengthNeeded);

	}
};

void onLoad()
{
	std::vector<Desktop> desktops{};

	EnumDesktopsA(NULL, [](LPSTR desktopname, LPARAM lParam) {
		auto tops = (std::vector<Desktop> *)lParam;

		//printf("DESKTOP: %s\n", desktopname);
		Desktop desk(desktopname);
		tops->push_back(desk);
		return TRUE;
		}, (LPARAM)& desktops);


	for (auto& desk : desktops)
	{
		printf("DESKTOPPER: %s\n", desk.name().c_str());
		desk.printUserObjects();
	}

	halt();
}