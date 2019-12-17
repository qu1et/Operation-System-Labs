#include <iostream>
#include <Windows.h>
#include <mutex>
#include <thread>
#include <fstream>

using namespace std;

LPCWSTR mutexName = L"Global\\myMute";

HANDLE mute;
ofstream fo;

void print_block(int n, char с) {
	WaitForSingleObject(mute, 1000);
	
	for (int i = 0; i < n; ++i) { 
		if (fo.is_open()) {
			fo << с << endl;
		}
	}
	ReleaseMutex(mute);
}

int main()
{
	mute = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"Global\\myMute");

	if (mute == NULL) {
		std::cout << "OpenMutex failed: " << GetLastError() << endl;
		return 1;
	}

	std::cout << "Open mutex\n";
	fo.open("C:\\mutex.txt", std::ios::app);
	std::thread th1(print_block, 5, '&');
	std::thread th2(print_block, 5, '*');

	th1.join();
	th2.join();

	return 0;
}