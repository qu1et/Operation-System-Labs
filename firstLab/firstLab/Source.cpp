#include <windows.h>
#include <iostream>
#pragma warning(disable : 4996)
#define BUFSIZE MAX_PATH
using namespace std;
void main()
{
	char buffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size;
	size = sizeof(buffer);
	GetComputerNameA(buffer, &size);
	cout << "Computer name: " << buffer << endl;
	char buf[257];
	DWORD size2 = sizeof(buf);
	GetUserNameA(buf, &size2);
	cout << "Username: " << buf << endl;
	char path[20];
	GetSystemDirectoryA(path, 20);
	cout << "System directory:" << path << endl;
	char wpath[20];
	GetWindowsDirectoryA(wpath, 20);
	cout << "Windows directory:" << wpath << endl;
	char tpath[50];
	GetTempPathA(50, tpath);
	cout << "Temporary path: " << tpath << endl;
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionExA(&osvi);
	cout << "OS version: " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "." << osvi.dwBuildNumber << endl;
	BOOL mrez1 = GetSystemMetrics(SM_SWAPBUTTON);
	cout << "Swwapable buttons: " << mrez1 << endl;
	BOOL mrez2 = GetSystemMetrics(43);
	cout << "Mouse buttons: " << mrez2 << endl;
	BOOL prez1 = SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, NULL, 0);
	cout << "Window draging: " << prez1 << endl;
	int aMouseInfo[3];
	BOOL prez2 = SystemParametersInfo(SPI_GETMOUSE, 0, &aMouseInfo, 0);
	cout << "Mouse existance: " << prez2 << endl;

	int aElements[2] = { COLOR_WINDOW, COLOR_WINDOWFRAME };
	DWORD aOldColors[2];
	DWORD aNewColors[2];

	aOldColors[0] = GetSysColor(COLOR_WINDOW);
	printf("Current window color: {0x%x, 0x%x, 0x%x}\n",
		GetRValue(aOldColors[0]),
		GetGValue(aOldColors[0]),
		GetBValue(aOldColors[0]));

	aOldColors[1] = GetSysColor(aElements[1]);
	printf("Current text color: {0x%x, 0x%x, 0x%x}\n",
		GetRValue(aOldColors[1]),
		GetGValue(aOldColors[1]),
		GetBValue(aOldColors[1]));

	aNewColors[1] = RGB(0x80, 0x80, 0x80);
	aNewColors[0] = RGB(0x80, 0x00, 0x80);  // dark purple
	printf("\nNew text color: {0x%x, 0x%x, 0x%x}\n",
		GetRValue(aNewColors[0]),
		GetGValue(aNewColors[0]),
		GetBValue(aNewColors[0]));
	SetSysColors(1, aElements, aNewColors);

	SetSysColors(1, aElements, aOldColors);

	int a = GetTickCount();
	cout << a << endl;
	SYSTEMTIME st;
	GetLocalTime(&st);
	cout << "Hours: " << st.wHour << " Minutes: " << st.wMinute << endl;


	TCHAR Buffer[BUFSIZE];
	GetCurrentDirectory(BUFSIZE, Buffer);
	cout << "Current dir: " << Buffer << endl;


	SYSTEM_POWER_STATUS status;
	GetSystemPowerStatus(&status);
	int charge = status.BatteryLifePercent;
	cout << "Current batery charge: " << charge << endl;
	FIRMWARE_TYPE tp;

	cout << "System firmware type: " << GetFirmwareType(&tp) << ". What means thet it is bios type.\n";
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	cout << "All physical memory: " << statex.ullTotalPhys << endl;
	cout << "Used memory: " << statex.dwMemoryLoad << "%\n";


	system("PAUSE");
}