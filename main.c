#define _CRT_SECURE_NO_WARNINGS
#define  _CRT_NON_CONFORMING_SWPRINTFS
#include <Windows.h>
#include <stdio.h>

void FractionizeFile(WCHAR path[], WCHAR chunkname[]) {
	HANDLE hFile;

	hFile = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}
	DWORD dwFileSz = GetFileSize(hFile, NULL);

	DWORD dwFullSz = 0;
	DWORD dwOffset = (dwFileSz / 10);

	for (DWORD dwCounter = 0; dwFullSz < dwFileSz; dwCounter++)
	{


		PBYTE pszBuffer = (PBYTE)GlobalAlloc(GPTR, dwFileSz + 1);
		PWCHAR pwFullchunkname = (PWCHAR)GlobalAlloc(GPTR, MAX_PATH);
		DWORD dwChunkSize = 0;
		DWORD dwX = 0;
		DWORD dwBytesToRead = 0;

		dwX = dwFullSz;
		dwX += dwOffset;
		if (dwX > dwFileSz)
		{
			if (dwFullSz != dwFileSz)
			{
				dwOffset = (dwFileSz - dwFullSz);
			}
		}

		swprintf(pwFullchunkname, L"%ws.%ld", chunkname, dwCounter);

		if (!ReadFile(hFile, pszBuffer, dwOffset, &dwBytesToRead, NULL)) {
			dwX = GetLastError();
			break;
		}

		HANDLE hFileOut = CreateFile(pwFullchunkname, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFileOut == INVALID_HANDLE_VALUE)
		{
			dwX = GetLastError();
			break;
		}

		if (!WriteFile(hFileOut, pszBuffer, dwOffset, &dwChunkSize, NULL))
		{
			dwX = GetLastError();
			break;
		}

		dwFullSz += dwOffset;

		if (pszBuffer)
			GlobalFree(pszBuffer);

		if (pwFullchunkname)
			GlobalFree(pwFullchunkname);

		if (hFileOut)
			CloseHandle(hFileOut);

	}

	if (hFile)
		CloseHandle(hFile);
}

void JoinFile(PWCHAR pwChunkNameEx, PWCHAR pwOutFileEx) {
	HANDLE hFileOut;
	hFileOut = CreateFile(pwOutFileEx, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFileOut == INVALID_HANDLE_VALUE) {
		return;
	}

	BOOL bFileFound = TRUE;
	DWORD dwCounter = 0;
	DWORD dwFileSize = 0;
	WCHAR wcName[256] = { 0 };

	while (bFileFound) {

		bFileFound = FALSE;

		swprintf(wcName, L"%ws.%ld", pwChunkNameEx, dwCounter);

		HANDLE hFileRead;
		hFileRead = CreateFile(wcName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);

		if (hFileRead == INVALID_HANDLE_VALUE) {
			return;
		}

		bFileFound = TRUE;
		DWORD dwFileSize = GetFileSize(hFileRead, NULL);
		PBYTE pszBuffer = (PBYTE)GlobalAlloc(GPTR, dwFileSize + 1);
		DWORD dwFullSize = dwFileSize;

	
		DWORD dwNumberBytes = 0;


		ReadFile(hFileRead, pszBuffer, dwFullSize, &dwNumberBytes, NULL);
		WriteFile(hFileOut, pszBuffer, dwFullSize, &dwNumberBytes, NULL);

		if (pszBuffer)
			GlobalFree(pszBuffer);

		dwCounter++;
		CloseHandle(hFileRead);


	}
	CloseHandle(hFileOut);

}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
	if (argc != 3) {
		printf("USAGE: chunker.exe <INPUT FILE> <OUTPUT FILE> \r\n");
		return 1;
	}
	
	wchar_t *wcOutFile = argv[2];
	wchar_t *wcInFile = argv[1];

	printf("[*] SPLITTING UP FILE\n");
	FractionizeFile(wcInFile, L"Segment");
	
	printf("[*] JOINING FILE\n");
	
	JoinFile(L"Segment", wcOutFile);

	printf("[*] FILE JOINED!!! uwu");

	getchar();
	return 0;
}
