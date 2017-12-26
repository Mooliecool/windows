// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <ole2.h> // OLE2 Definitions


HRESULT GetModuleDirectoryW(WCHAR* pwszDir, DWORD nSize);