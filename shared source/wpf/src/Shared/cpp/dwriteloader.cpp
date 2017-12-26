#include "dwriteloader.h"

namespace WPFUtils
{

#if _MANAGED
/// <SecurityNote>
/// Critical - Receives a native pointer as parameter.
///            Loads a dll from an input path.
/// </SecurityNote>
[System::Security::SecurityCritical]
#endif
HMODULE LoadDWriteLibraryAndGetProcAddress(const wchar_t *pwszWpftxtPath, void **pfncptrDWriteCreateFactory)
{
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osvi.dwMajorVersion = 6;
    osvi.dwMinorVersion = 1;
    osvi.dwBuildNumber  = 7226; // 6/2/2009

    DWORDLONG dwlConditionMask = 0;
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);

    HMODULE hDWriteLibrary = NULL;
    if (VerifyVersionInfo(
        &osvi,
        VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER,
        dwlConditionMask
        ))
    {
        // Perhaps this could fail on Server 2008 R2? The wpftxt load below
        // will then happen.
        hDWriteLibrary = LoadLibrary(L"dwrite.dll");
        if (hDWriteLibrary)
        {
            *pfncptrDWriteCreateFactory = GetProcAddress(hDWriteLibrary, "DWriteCreateFactory");
        }
    }

    if (!hDWriteLibrary)
    {
        hDWriteLibrary = LoadLibrary(pwszWpftxtPath);

        if (hDWriteLibrary)
        {
            // In our private wpftxt_v0400 binary, DWriteCreateFactory is exported only by ordinal 1.
            *pfncptrDWriteCreateFactory = GetProcAddress(hDWriteLibrary, (LPCSTR)1);
        }
    }

    return hDWriteLibrary;
}

}//namespace
