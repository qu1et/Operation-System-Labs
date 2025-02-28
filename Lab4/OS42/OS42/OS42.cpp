#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <strsafe.h>
#define BUFSIZE 512

#define ARR_SIZE 10
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
HANDLE hM;
int *pB;
HANDLE hSlot;
LPCTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
void magic(int*, bool);
void Mailslot();
void Pipe();
void FileMapping();

void sortBuble(int *arr)
{
	int temp;
	for (int i = 0; i < ARR_SIZE - 1; i++) {
		for (int j = 0; j < ARR_SIZE - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}
int main()
{
	int n = -1;
	while (n != 0)
	{
		std::cout << "\nChoose mechanism:\n"
			<< "1. Mailslot\n"
			<< "2. Pipe\n"
			<< "3. FileMapping\n"
			<< "0. Exit\n"
			<< "Input: ";
		std::cin >> n;
		switch (n)
		{
		case 1:
		{
			Mailslot();
			break;
		}
		case 2:
		{
			Pipe();
			break;
		}
		case 3:
		{
			FileMapping();
			break;
		}
		}
	}
	return 0;
}

BOOL WINAPI MakeSlot(LPCTSTR lpszSlotName)
{
	hSlot = CreateMailslot(lpszSlotName, 0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL);

	if (hSlot == INVALID_HANDLE_VALUE)
	{
		printf("CreateMailslot failed with %d\n", GetLastError());
		return FALSE;
	}
	else printf("Mailslot created successfully.\n");
	return TRUE;
}

BOOL WriteSlot(HANDLE hSlot, int *arr)
{
	BOOL fResult;
	DWORD cbWritten;

	fResult = WriteFile(hSlot, arr, sizeof(arr), &cbWritten, (LPOVERLAPPED)NULL); magic(arr, 0);

	if (!fResult)
	{
		printf("WriteFile failed with %d.\n", GetLastError());
		return FALSE;
	}

	printf("\nSlot written to successfully.\n");

	return TRUE;
}

BOOL ReadSlot(HANDLE hSlot, int *arr)
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	LPTSTR lpszBuffer;
	TCHAR achID[80];
	DWORD cAllMessages;
	HANDLE hEvent;
	OVERLAPPED ov;

	cbMessage = cMessage = cbRead = 0;

	hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
	if (NULL == hEvent)
		return FALSE;
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;

	fResult = GetMailslotInfo(hSlot, (LPDWORD)NULL, &cbMessage, &cMessage, (LPDWORD)NULL);

	if (!fResult)
	{
		printf("GetMailslotInfo failed with %d.\n", GetLastError());
		return FALSE;
	}

	if (cbMessage == MAILSLOT_NO_MESSAGE)
	{
		printf("Waiting for a message...\n");
		return TRUE;
	}

	cAllMessages = cMessage;
	magic(arr, 1);

	while (cMessage != 0)
	{
		StringCchPrintf((LPTSTR)achID, 80, TEXT("\nMessage #%d of %d\n"), cAllMessages - cMessage + 1, cAllMessages);

		lpszBuffer = (LPTSTR)GlobalAlloc(GPTR,
			lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage);
		if (NULL == lpszBuffer)
			return FALSE;
		lpszBuffer[0] = '\0';

		fResult = ReadFile(hSlot, arr, cbMessage, &cbRead, &ov);

		if (!fResult)
		{
			//printf("ReadFile failed with %d.\n", GetLastError());
			GlobalFree((HGLOBAL)lpszBuffer);
			return FALSE;
		}

		StringCbCat(lpszBuffer, lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage, (LPTSTR)achID);

		_tprintf(TEXT("Contents of the mailslot: %s\n"), lpszBuffer);

		GlobalFree((HGLOBAL)lpszBuffer);

		fResult = GetMailslotInfo(hSlot, (LPDWORD)NULL, &cbMessage, &cMessage, (LPDWORD)NULL);

		if (!fResult)
		{
			printf("GetMailslotInfo failed (%d)\n", GetLastError());
			return FALSE;
		}
	}
	CloseHandle(hEvent);
	return TRUE;
}
void magic(int *arr, bool t)
{
	if (t) { hM = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName); pB = (int*)MapViewOfFile(hM, FILE_MAP_ALL_ACCESS, 0, 0, ARR_SIZE * sizeof(int)); CopyMemory(arr, &pB[ARR_SIZE], ARR_SIZE * sizeof(int)); }
	else { CopyMemory(&pB[ARR_SIZE], arr, ARR_SIZE * sizeof(int));; UnmapViewOfFile(pB); CloseHandle(hM); }
}

void Mailslot()
{
	//MakeSlot(SlotName);
	HANDLE hFile;

	hFile = CreateFile(SlotName, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d.\n", GetLastError());
		return;
	}
	int *arr = new int[ARR_SIZE];
	ReadSlot(hFile, arr);
	printf("Getting an array:\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);

	sortBuble(arr);

	WriteSlot(hFile, arr);

	delete[]arr;
	CloseHandle(hFile);
	system("pause");
}

void Pipe()
{
	int *arr = new int[ARR_SIZE];
	HANDLE hPipe; magic(arr, 1);
	TCHAR chReadBuf[BUFSIZE];
	BOOL fSuccess;
	DWORD cbRead, dwMode;
	LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	while (1)
	{
		hPipe = CreateFile(lpszPipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			printf("Could not open pipe\n");
			return;
		}

		if (!WaitNamedPipe(lpszPipename, 200))
		{
			break;
		}
	}

	printf("\nGetting an array:\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);
	printf("\n");
	sortBuble(arr); return;
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);
	if (!fSuccess)
	{
		printf("SetNamedPipeHandleState failed.\n");
		return;
	}

	fSuccess = TransactNamedPipe(hPipe, arr, ARR_SIZE, chReadBuf, ARR_SIZE * sizeof(int), &cbRead, NULL);

	if (!fSuccess && (GetLastError() != ERROR_MORE_DATA))
	{
		printf("TransactNamedPipe failed.\n");
		return;
	}

	while (1)
	{
		_tprintf(TEXT("%s\n"), chReadBuf);

		if (fSuccess)
			break;

		fSuccess = ReadFile(hPipe, chReadBuf, BUFSIZE * sizeof(TCHAR), &cbRead, NULL);

		if (!fSuccess && (GetLastError() != ERROR_MORE_DATA))
			break;
		else _tprintf(TEXT("%s\n"), chReadBuf);
	}

	system("pause");
	CloseHandle(hPipe);
}

void FileMapping()
{
	HANDLE hMapFile;
	int *pBuf;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);

	if (hMapFile == NULL)
	{
		printf("Could not open file mapping object (%d).\n", GetLastError());
		return;
	}

	pBuf = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, ARR_SIZE * sizeof(int));

	if (pBuf == NULL)
	{
		printf("Could not map view of file (%d).\n", GetLastError());
		CloseHandle(hMapFile);
		return;
	}

	int *arr = new int[ARR_SIZE];
	CopyMemory(arr, &pBuf[ARR_SIZE], ARR_SIZE * sizeof(int));
	printf("Getting an array:\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);

	sortBuble(arr);
	CopyMemory(&pBuf[ARR_SIZE], arr, ARR_SIZE * sizeof(int));

	delete[] arr;
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	system("pause");
}