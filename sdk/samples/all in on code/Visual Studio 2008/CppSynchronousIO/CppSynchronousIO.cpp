/****************************** Module Header ******************************\
* Module Name:  CppSynchronousIO.cpp
* Project:      CppSynchronousIO
* Copyright (c) Microsoft Corporation.
* 
* CppSynchronousIO demonstrates the synchronous file I/O operations. In 
* synchronous file I/O, a thread starts an I/O operation and immediately 
* enters a wait state until the I/O request has completed. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#pragma endregion


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hFile;
	wchar_t szFileName[] = L"MSDN.tmp";
	DWORD dwError;


	/////////////////////////////////////////////////////////////////////////
	// Create/open a file for write asynchronously.
	// 

	wprintf(L"Open the file %s\n", szFileName);

	hFile = CreateFile(szFileName,		// Name of the file
		GENERIC_WRITE | GENERIC_READ,	// Open for writing and reading
		0,								// Do not share
		NULL,							// Default security
		OPEN_ALWAYS,					// Always open
		// When the device is opened, you must NOT specify the 
		// FILE_FLAG_OVERLAPPED flag, or the system will think that you want 
		// to perform asynchronous I/O with the device. 
		FILE_ATTRIBUTE_NORMAL,			// Normal file
		NULL);							// No attr. template

	if (hFile == INVALID_HANDLE_VALUE) 
    { 
        wprintf(L"Could not create file w/err 0x%08lx\n", GetLastError());
        return 1;
    }


	/////////////////////////////////////////////////////////////////////////
	// Synchronously write bytes to the file.
	// 

	BYTE bWriteBuffer[] = "0123456789";
	DWORD dwBytesWritten;

	// Write 11 bytes from the buffer to the file
	if (!WriteFile(hFile,				// File handle
		bWriteBuffer,					// Buffer to be write from
		sizeof(bWriteBuffer),			// Number of bytes to write
		&dwBytesWritten,				// Number of bytes that were written
		NULL))							// No overlapped structure
	{
		// WriteFile returns FALSE because of some error

		wprintf(L"Could not write to file w/err 0x%08lx\n", GetLastError());
		CloseHandle(hFile);
		return 1;
	}

	// Handle the result
	wprintf(L"%d bytes were written: %s\n", dwBytesWritten, bWriteBuffer);


	/////////////////////////////////////////////////////////////////////////
	// Synchronously read bytes from the file.
	// 

	BYTE bReadBuffer[100];
	DWORD dwBytesRead;

	// Set the file point to the beginning of the file
	// (After the above Write operation, the current file pointer is at the 
	// 11th charater, instead of the beginning of the file)

	LARGE_INTEGER liPosition = { 0 };
	SetFilePointerEx(hFile, liPosition, &liPosition, FILE_BEGIN);

	// Read max. 100 bytes from the file to a buffer
	if (!ReadFile(hFile,				// File handle
		bReadBuffer,					// Buffer to be read to
		sizeof(bReadBuffer),			// Max number of bytes to read
		&dwBytesRead,					// Number of bytes that were read
		NULL))							// No overlapped structure
	{
		// ReadFile returns FALSE because of some error
		wprintf(L"Could not read from file w/err 0x%08lx\n", GetLastError());

		CloseHandle(hFile);
		return 1;
	}

	// Handle the result
	if (dwBytesRead > 0)
	{
		wprintf(L"%d bytes were read: %s\n", dwBytesRead, bReadBuffer);
	}
	else
	{
		_putws(L"No bytes were read from the file");
	}


	/////////////////////////////////////////////////////////////////////////
	// Close the file.
	// 

	CloseHandle(hFile);

	return 0;
}