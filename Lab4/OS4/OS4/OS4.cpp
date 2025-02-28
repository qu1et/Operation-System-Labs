#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <strsafe.h>

#define ARR_SIZE 20
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
HANDLE hM;
int *pB;
HANDLE hSlot;
LPCTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest(int*, LPTSTR, LPDWORD);
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
		Sleep(50);
		printf("\nChoose mechanism:\n");
		printf("1. Mailslot\n");
		printf("2. Pipe\n");
		printf("3. FileMapping\n");
		printf("0. Exit\n");
		printf("Input: ");
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

	fResult = WriteFile(hSlot, arr, sizeof(arr), &cbWritten, (LPOVERLAPPED)NULL); magic(arr, 1);

	if (!fResult)
	{
		printf("WriteFile failed with %d.\n", GetLastError());
		return FALSE;
	}

	printf("Slot written to successfully.\n");

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
	magic(arr, 0);

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
	if (t) { hM = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ARR_SIZE * sizeof(int), szName); pB = (int*)MapViewOfFile(hM, FILE_MAP_ALL_ACCESS, 0, 0, ARR_SIZE * sizeof(int)); CopyMemory(pB, arr, ARR_SIZE * sizeof(int)); }
	else { CopyMemory(arr, pB, ARR_SIZE * sizeof(int)); UnmapViewOfFile(pB); CloseHandle(hM); }
}

void Mailslot()
{
	MakeSlot(SlotName);
	HANDLE hFile;

	hFile = CreateFile(SlotName, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d.\n", GetLastError());
		return;
	}

	int *arr = new int[ARR_SIZE];
	srand(time(NULL));
	for (int i = 0; i < ARR_SIZE / 2; i++)
		arr[i] = rand() % 50;
	for (int i = ARR_SIZE / 2; i < ARR_SIZE; i++)
		arr[i] = rand() % 50 + 50;

	printf("\nMassive before\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);
	printf("\n");

	WriteSlot(hFile, arr);

	system("pause");

	ReadSlot(hFile, arr);

	printf("Massive after\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);
	printf("\n");

	delete[]arr;
	CloseHandle(hFile);
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, ARR_SIZE * sizeof(int));
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, ARR_SIZE * sizeof(int));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	if (lpvParam == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	if (pchRequest == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;
	}

	if (pchReply == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	printf("InstanceThread created, receiving and processing messages.\n");

	hPipe = (HANDLE)lpvParam;

	while (1)
	{
		fSuccess = ReadFile(hPipe, pchRequest, ARR_SIZE * sizeof(int), &cbBytesRead, NULL);

		if (!fSuccess || cbBytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				_tprintf(TEXT("InstanceThread: client disconnected.\n"));
			}
			else
			{
				_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
			}
			break;
		}

		int *arr = new int[ARR_SIZE];
		GetAnswerToRequest(arr, pchReply, &cbReplyBytes);

		fSuccess = WriteFile(hPipe, pchReply, cbReplyBytes, &cbWritten, NULL);

		if (!fSuccess || cbReplyBytes != cbWritten)
		{
			printf("InstanceThread WriteFile failed, GLE=%d.\n", GetLastError());
			break;
		}
	}

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	printf("InstanceThread exiting.\n");
	return 1;
}

VOID GetAnswerToRequest(int *arr, LPTSTR pchReply, LPDWORD pchBytes)
{
	if (!pchReply)
	{
		printf("Massive after\n");
		for (int i = 0; i < ARR_SIZE; i++)
			printf("%i ", arr[i]);
	}

	if (FAILED(StringCchCopy(pchReply, ARR_SIZE * sizeof(int), TEXT("default answer from server"))))
	{
		*pchBytes = 0;
		pchReply[0] = 0;
		printf("StringCchCopy failed, no outgoing message.\n");
		return;
	}
	*pchBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);
}

void Pipe()
{
	int *arr = new int[ARR_SIZE];
	srand(time(NULL));
	for (int i = 0; i < ARR_SIZE / 2; i++)
		arr[i] = rand() % 50;
	for (int i = ARR_SIZE / 2; i < ARR_SIZE; i++)
		arr[i] = rand() % 50 + 50;

	printf("\nMassive before\n"); magic(arr, 1);
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);
	printf("\n");

	BOOL   fConnected = FALSE;
	DWORD  dwThreadId = 0; sortBuble(arr);
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	while (!hThread)
	{
		printf("\nPipe Server: Main thread awaiting client connection on %s\n", lpszPipename);
		hPipe = CreateNamedPipe(lpszPipename, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, sizeof(int) * ARR_SIZE, sizeof(int) * ARR_SIZE, 0, NULL);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			printf("CreateNamedPipe failed, GLE=%d.\n", GetLastError());
			return;
		}

		fConnected = ConnectNamedPipe(hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected)
		{
			printf("Client connected, creating a processing thread.\n");

			hThread = CreateThread(NULL, 0, InstanceThread, (LPVOID)hPipe, 0, &dwThreadId);

			if (hThread == NULL)
			{
				printf("CreateThread failed, GLE=%d.\n", GetLastError());
				return;
			}
			else CloseHandle(hThread);
			break;
		}
		else
			CloseHandle(hPipe);
	}
	Sleep(100);
	printf("Massive after\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);
	printf("\n");
}

void FileMapping()
{
	HANDLE hMapFile;
	int *pBuf;

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ARR_SIZE * sizeof(int), szName);

	if (hMapFile == NULL)
	{
		printf("Could not create file mapping object (%d).\n", GetLastError());
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
	srand(time(NULL));
	for (int i = 0; i < ARR_SIZE / 2; i++)
		arr[i] = rand() % 50;
	for (int i = ARR_SIZE / 2; i < ARR_SIZE; i++)
		arr[i] = rand() % 50 + 50;

	printf("\nMassive before\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);
	printf("\n");

	CopyMemory(pBuf, arr, ARR_SIZE * sizeof(int));
	system("pause");
	CopyMemory(arr, pBuf, ARR_SIZE * sizeof(int));

	printf("Massive after\n");
	for (int i = 0; i < ARR_SIZE; i++)
		printf("%i ", arr[i]);
	printf("\n");

	delete[] arr;

	system("pause");
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}