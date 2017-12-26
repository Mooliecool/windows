#include "stdafx.h"


HRESULT GetModuleDirectoryW(WCHAR* pwszDir, DWORD nSize)
{
	// Retrieves the path of the executable file of the current process.
	nSize = GetModuleFileNameW(NULL, pwszDir, nSize);
	if (!nSize || GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		*pwszDir = '\0'; // Ensure it's NULL terminated
		return S_FALSE;
	}

	// Run through looking for the last slash in the file path.
    // When we find it, NULL it to truncate the following filename part.

    for (int index = nSize - 1; index >= 0; index --)
	{
        if (pwszDir[index] == '\\' || pwszDir[index] == '/')
		{
			pwszDir[index + 1] = '\0';
            break;
		}
    }
	return S_OK;
}