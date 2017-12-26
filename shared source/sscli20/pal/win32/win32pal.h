/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    win32pal.h

Abstract:

    PAL for Win32 subsystem processes.

--*/

//
// Create the PAL API name-to-PAL_name mappings
//
#define CharNextA PAL_CharNextA
#define CharNextExA PAL_CharNextExA
#define wsprintfA PAL_wsprintfA
#define wsprintfW PAL_wsprintfW
#define MessageBoxW PAL_MessageBoxW
#define SetConsoleCtrlHandler PAL_SetConsoleCtrlHandler
#define GenerateConsoleCtrlEvent PAL_GenerateConsoleCtrlEvent
#define AreFileApisANSI PAL_AreFileApisANSI
#define CreateFileA PAL_CreateFileA
#define CreateFileW PAL_CreateFileW
#define LockFile PAL_LockFile
#define UnlockFile PAL_UnlockFile
#define SearchPathA PAL_SearchPathA
#define SearchPathW PAL_SearchPathW
#define CreatePipe PAL_CreatePipe
#define CopyFileA PAL_CopyFileA
#define CopyFileW PAL_CopyFileW
#define DeleteFileA PAL_DeleteFileA
#define DeleteFileW PAL_DeleteFileW
#define MoveFileA PAL_MoveFileA
#define MoveFileW PAL_MoveFileW
#define MoveFileExA PAL_MoveFileExA
#define MoveFileExW PAL_MoveFileExW
#define CreateDirectoryA PAL_CreateDirectoryA
#define CreateDirectoryW PAL_CreateDirectoryW
#define RemoveDirectoryA PAL_RemoveDirectoryA
#define RemoveDirectoryW PAL_RemoveDirectoryW
#define FindFirstFileA PAL_FindFirstFileA
#define FindFirstFileW PAL_FindFirstFileW
#define FindNextFileA PAL_FindNextFileA
#define FindNextFileW PAL_FindNextFileW
#define FindClose PAL_FindClose
#define GetFileAttributesA PAL_GetFileAttributesA
#define GetFileAttributesW PAL_GetFileAttributesW
#define GetFileAttributesExA PAL_GetFileAttributesExA
#define GetFileAttributesExW PAL_GetFileAttributesExW
#define SetFileAttributesA PAL_SetFileAttributesA
#define SetFileAttributesW PAL_SetFileAttributesW
#define WriteFile PAL_WriteFile
#define ReadFile PAL_ReadFile
#define GetStdHandle PAL_GetStdHandle
#define SetEndOfFile PAL_SetEndOfFile
#define SetFilePointer PAL_SetFilePointer
#define GetFileSize PAL_GetFileSize
#define GetFileInformationByHandle PAL_GetFileInformationByHandle
#define CompareFileTime PAL_CompareFileTime
#define SetFileTime PAL_SetFileTime
#define GetFileTime PAL_GetFileTime
#define FileTimeToLocalFileTime PAL_FileTimeToLocalFileTime
#define LocalFileTimeToFileTime PAL_LocalFileTimeToFileTime
#define GetSystemTimeAsFileTime PAL_GetSystemTimeAsFileTime
#define GetSystemTime PAL_GetSystemTime
#define FileTimeToDosDateTime PAL_FileTimeToDosDateTime
#define DosDateTimeToFileTime PAL_DosDateTimeToFileTime
#define FlushFileBuffers PAL_FlushFileBuffers
#define GetFileType PAL_GetFileType
#define GetConsoleCP PAL_GetConsoleCP
#define GetConsoleOutputCP PAL_GetConsoleOutputCP
#define GetFullPathNameA PAL_GetFullPathNameA
#define GetFullPathNameW PAL_GetFullPathNameW
#define GetLongPathNameW PAL_GetLongPathNameW
#define GetShortPathNameW PAL_GetShortPathNameW
#define GetTempFileNameA PAL_GetTempFileNameA
#define GetTempFileNameW PAL_GetTempFileNameW
#define GetTempPathA PAL_GetTempPathA
#define GetTempPathW PAL_GetTempPathW
#define GetCurrentDirectoryA PAL_GetCurrentDirectoryA
#define GetCurrentDirectoryW PAL_GetCurrentDirectoryW
#define SetCurrentDirectoryA PAL_SetCurrentDirectoryA
#define SetCurrentDirectoryW PAL_SetCurrentDirectoryW
#define GetUserNameW PAL_GetUserNameW
#define GetComputerNameW PAL_GetComputerNameW
#define CreateSemaphoreA PAL_CreateSemaphoreA
#define CreateSemaphoreW PAL_CreateSemaphoreW
#define ReleaseSemaphore PAL_ReleaseSemaphore
#define CreateEventA PAL_CreateEventA
#define CreateEventW PAL_CreateEventW
#define SetEvent PAL_SetEvent
#define ResetEvent PAL_ResetEvent
#define OpenEventW PAL_OpenEventW
#define CreateMutexA PAL_CreateMutexA
#define CreateMutexW PAL_CreateMutexW
#define ReleaseMutex PAL_ReleaseMutex
#define GetCurrentProcessId PAL_GetCurrentProcessId
#define GetCurrentProcess PAL_GetCurrentProcess
#define GetCurrentThreadId PAL_GetCurrentThreadId
#define GetCurrentThread PAL_GetCurrentThread
#define CreateProcessW PAL_CreateProcessW
#define CreateProcessA PAL_CreateProcessA
#define ExitProcess PAL_ExitProcess
#define TerminateProcess PAL_TerminateProcess
#define GetExitCodeProcess PAL_GetExitCodeProcess
#define GetProcessTimes PAL_GetProcessTimes
#define WaitForSingleObject PAL_WaitForSingleObject
#define WaitForSingleObjectEx PAL_WaitForSingleObjectEx
#define WaitForMultipleObjects PAL_WaitForMultipleObjects
#define WaitForMultipleObjectsEx PAL_WaitForMultipleObjectsEx
#define DuplicateHandle PAL_DuplicateHandle
#define Sleep PAL_Sleep
#define SleepEx PAL_SleepEx
#define SwitchToThread PAL_SwitchToThread
#define CreateThread PAL_CreateThread
#define ExitThread PAL_ExitThread
#define SuspendThread PAL_SuspendThread
#define ResumeThread PAL_ResumeThread
#define QueueUserAPC PAL_QueueUserAPC
#define GetThreadContext PAL_GetThreadContext
#define SetThreadContext PAL_SetThreadContext
#define GetThreadPriority PAL_GetThreadPriority
#define SetThreadPriority PAL_SetThreadPriority
#define TlsAlloc PAL_TlsAlloc
#define TlsGetValue PAL_TlsGetValue
#define TlsSetValue PAL_TlsSetValue
#define TlsFree PAL_TlsFree
#define EnterCriticalSection PAL_EnterCriticalSection
#define LeaveCriticalSection PAL_LeaveCriticalSection
#define InitializeCriticalSection PAL_InitializeCriticalSection
#define DeleteCriticalSection PAL_DeleteCriticalSection
#define TryEnterCriticalSection PAL_TryEnterCriticalSection
#define SetErrorMode PAL_SetErrorMode
#define CreateFileMappingA PAL_CreateFileMappingA
#define CreateFileMappingW PAL_CreateFileMappingW
#define OpenFileMappingA PAL_OpenFileMappingA
#define OpenFileMappingW PAL_OpenFileMappingW
#define MapViewOfFile PAL_MapViewOfFile
#define UnmapViewOfFile PAL_UnmapViewOfFile
#define LoadLibraryA PAL_LoadLibraryA
#define LoadLibraryW PAL_LoadLibraryW
#define GetProcAddress PAL_GetProcAddress
#define FreeLibrary PAL_FreeLibrary
#define FreeLibraryAndExitThread PAL_FreeLibraryAndExitThread
#define DisableThreadLibraryCalls PAL_DisableThreadLibraryCalls
#define GetModuleFileNameA PAL_GetModuleFileNameA
#define GetModuleFileNameW PAL_GetModuleFileNameW
#define VirtualAlloc PAL_VirtualAlloc
#define VirtualFree PAL_VirtualFree
#define VirtualProtect PAL_VirtualProtect
#define VirtualQuery PAL_VirtualQuery
#define ReadProcessMemory PAL_ReadProcessMemory
#define WriteProcessMemory PAL_WriteProcessMemory
#define RtlMoveMemory PAL_RtlMoveMemory
#define GetProcessHeap PAL_GetProcessHeap
#define HeapAlloc PAL_HeapAlloc
#define HeapReAlloc PAL_HeapReAlloc
#define HeapFree PAL_HeapFree
#define LocalAlloc PAL_LocalAlloc
#define LocalFree PAL_LocalFree
#define FlushInstructionCache PAL_FlushInstructionCache
#define GetStringTypeExW PAL_GetStringTypeExW
#define CompareStringA PAL_CompareStringA
#define CompareStringW PAL_CompareStringW
#define GetACP PAL_GetACP
#define GetCPInfo PAL_GetCPInfo
#define IsDBCSLeadByteEx PAL_IsDBCSLeadByteEx
#define IsValidCodePage PAL_IsValidCodePage
#define MultiByteToWideChar PAL_MultiByteToWideChar
#define WideCharToMultiByte PAL_WideCharToMultiByte
#define GetSystemDefaultLangID PAL_GetSystemDefaultLangID
#define GetUserDefaultLangID PAL_GetUserDefaultLangID
#define SetThreadLocale PAL_SetThreadLocale
#define GetThreadLocale PAL_GetThreadLocale
#define GetLocaleInfoW PAL_GetLocaleInfoW
#define GetUserDefaultLCID PAL_GetUserDefaultLCID
#define GetTimeZoneInformation PAL_GetTimeZoneInformation
#define IsValidLocale PAL_IsValidLocale
#define GetCalendarInfoW PAL_GetCalendarInfoW
#define GetDateFormatW PAL_GetDateFormatW
#define OutputDebugStringA PAL_OutputDebugStringA
#define OutputDebugStringW PAL_OutputDebugStringW
#define DebugBreak PAL_DebugBreak
#define WaitForDebugEvent PAL_WaitForDebugEvent
#define ContinueDebugEvent PAL_ContinueDebugEvent
#define DebugActiveProcess PAL_DebugActiveProcess
#define OpenProcess PAL_OpenProcess
#define lstrcatW PAL_lstrcatW
#define lstrcpyW PAL_lstrcpyW
#define lstrlenA PAL_lstrlenA
#define lstrlenW PAL_lstrlenW
#define lstrcpynW PAL_lstrcpynW
#define GetEnvironmentVariableA PAL_GetEnvironmentVariableA
#define GetEnvironmentVariableW PAL_GetEnvironmentVariableW
#define SetEnvironmentVariableA PAL_SetEnvironmentVariableA
#define SetEnvironmentVariableW PAL_SetEnvironmentVariableW
#define GetEnvironmentStringsA PAL_GetEnvironmentStringsA
#define GetEnvironmentStringsW PAL_GetEnvironmentStringsW
#define FreeEnvironmentStringsA PAL_FreeEnvironmentStringsA
#define FreeEnvironmentStringsW PAL_FreeEnvironmentStringsW
#define CloseHandle PAL_CloseHandle
#define RaiseException PAL_RaiseException
#define GetTickCount PAL_GetTickCount
#define SetUnhandledExceptionFilter PAL_SetUnhandledExceptionFilter
#define InterlockedIncrement PAL_InterlockedIncrement
#define InterlockedDecrement PAL_InterlockedDecrement
#define InterlockedExchange PAL_InterlockedExchange
#define InterlockedCompareExchange PAL_InterlockedCompareExchange
#define MemoryBarrier PAL_MemoryBarrier
#define YieldProcessor PAL_YieldProcessor
#define IsBadReadPtr PAL_IsBadReadPtr
#define IsBadWritePtr PAL_IsBadWritePtr
#define IsBadCodePtr PAL_IsBadCodePtr
#define FormatMessageW PAL_FormatMessageW
#define GetLastError PAL_GetLastError
#define SetLastError PAL_SetLastError
#define GetCommandLineW PAL_GetCommandLineW
#define GetVersionExA PAL_GetVersionExA
#define GetVersionExW PAL_GetVersionExW
#define GetSystemInfo PAL_GetSystemInfo
#define GetDiskFreeSpaceW PAL_GetDiskFreeSpaceW
#define memcpy PAL_memcpy
#define memcmp PAL_memcmp
#define memset PAL_memset
#define memmove PAL_memmove
#define strlen PAL_strlen
#define sprintf PAL_sprintf
#define vsprintf PAL_vsprintf
#define _snprintf PAL__snprintf
#define _vsnprintf PAL__vsnprintf
#define sscanf PAL_sscanf
#define _stricmp PAL__stricmp
#define strstr PAL_strstr
#define strtok PAL_strtok
#define strspn PAL_strspn
#define strcspn PAL_strspn
#define strtoul PAL_strtoul
#define atof PAL_atof
#define _gcvt PAL__gcvt
#define _ecvt PAL__ecvt
#define strtod PAL_strtod
#define _strlwr PAL__strlwr
#define _mbslen PAL__mbslen
#define _mbsinc PAL__mbsinc
#define _mbsninc PAL__mbsninc
#define _mbsdec PAL__mbsdec
#define wcstol PAL_wcstol
#define wcstoul PAL_wcstoul
#define wcstod PAL_wcstod
#define wcscat PAL_wcscat
#define wcscpy PAL_wcscpy
#define wcslen PAL_wcslen
#define wcsncmp PAL_wcsncmp
#define _wcsicmp PAL__wcsicmp
#define wcschr PAL_wcschr
#define wcsrchr PAL_wcsrchr
#define _wcsnicmp PAL__wcsnicmp
#define swprintf PAL_swprintf
#define vswprintf PAL_vswprintf
#define _snwprintf PAL__snwprintf
#define _vsnwprintf PAL__vsnwprintf
#define swscanf PAL_swscanf
#define wcspbrk PAL_wcspbrk
#define wcsstr PAL_wcsstr
#define wcstok PAL_wcstok
#define wcscspn PAL_wcscspn
#define _wcslwr PAL__wcslwr
#define isspace PAL_isspace
#define isprint PAL_isprint
#define isalpha PAL_isalpha
#define isalnum PAL_isalnum
#define isdigit PAL_isdigit
#define __iscsym PAL___iscsym
#define iswdigit PAL_iswdigit
#define iswxdigit PAL_iswxdigit
#define isxdigit PAL_isxdigit
#define _finite PAL__finite
#define _isnan PAL__isnan
#define _copysign PAL__copysign
#define abs PAL_abs
#define labs PAL_labs
#define _rotl PAL__rotl
#define _rotr PAL__rotr
#define _alloca PAL__alloca
#define ceil PAL_ceil
#define floor PAL_floor
#define modf PAL_modf
#define log PAL_log
#define malloc PAL_malloc
#define free PAL_free
#define realloc PAL_realloc
#define _strdup PAL__strdup
#define exit PAL_exit
#define atexit PAL_atexit
#define qsort PAL_qsort
#define bsearch PAL_bsearch
#define _splitpath PAL__splitpath
#define _wsplitpath PAL__wsplitpath
#define _makepath PAL__makepath
#define _wmakepath PAL__wmakepath
#define _fullpath PAL__fullpath
#define _swab PAL__swab
#define time PAL_time
#define localtime PAL_localtime
#define mktime PAL_mktime
#define _open_osfhandle PAL__open_osfhandle
#define fclose PAL_fclose
#define setbuf PAL_setbuf
#define fwrite PAL_fwrite
#define fread PAL_fread
#define fgets PAL_fgets
#define fputs PAL_fputs
#define fprintf PAL_fprintf
#define fwprintf PAL_fwprintf
#define vfprintf PAL_vfprintf
#define vfwprintf PAL_vfwprintf
#define _getw PAL__getw
#define _putw PAL__putw
#define fseek PAL_fseek
#define ftell PAL_ftell
#define fgetpos PAL_fgetpos
#define fsetpos PAL_fsetpos
#define feof PAL_feof
#define ferror PAL_ferror
#define fopen PAL_fopen
#define _fdopen PAL__fdopen
#define _close PAL__close
#define _wfopen PAL__wfopen
#define fputc PAL_fputc
#define putchar PAL_putchar
#define fgetws PAL_fgetws
#define getc PAL_getc
#define fgetc PAL_getc
#define ungetc PAL_ungetc
#define rand PAL_rand
#define srand PAL_srand
#define fgets PAL_fgets
#define ctime PAL_ctime
#define printf PAL_printf
#define vprintf PAL_vprintf
#define wprintf PAL_wprintf
#define memchr PAL_memchr
#define strcmp PAL_strcmp
#define strncmp PAL_strncmp
#define _strnicmp PAL__strnicmp
#define strcat PAL_strcat
#define strncat PAL_strncat
#define strcpy PAL_strcpy
#define strncpy PAL_strncpy
#define strchr PAL_strchr
#define strrchr PAL_strrchr
#define strpbrk PAL_strpbrk
#define atoi PAL_atoi
#define atol PAL_atol
#define tolower PAL_tolower
#define toupper PAL_toupper
#define wcsspn PAL_wcsspn
#define wcscmp PAL_wcscmp
#define wcsncat PAL_wcsncat
#define wcsncpy PAL_wcsncpy
#define _itow PAL__itow
#define _i64tow PAL__i64tow
#define _ui64tow PAL__ui64tow
#define _wtoi PAL__wtoi
#define iswalpha PAL_iswalpha
#define iswprint PAL_iswprint
#define iswspace PAL_iswspace
#define iswupper PAL_iswupper
#define isupper PAL_isupper
#define islower PAL_islower
#define towlower PAL_towlower
#define towupper PAL_towupper
#define fflush PAL_fflush
#define getenv PAL_getenv
#define _putenv PAL__putenv
#define gethostbyname PAL_gethostbyname
#define gethostbyaddr PAL_gethostbyaddr
#define gethostname PAL_gethostname
#define inet_addr PAL_inet_addr
#define getpeername PAL_getpeername
#define getsockopt PAL_getsockopt
#define setsockopt PAL_setsockopt
#define connect PAL_connect
#define send PAL_send
#define recv PAL_recv
#define closesocket PAL_closesocket
#define accept PAL_accept
#define listen PAL_listen
#define bind PAL_bind
#define shutdown PAL_shutdown
#define sendto PAL_sendto
#define recvfrom PAL_recvfrom
#define getsockname PAL_getsockname
#define select PAL_select
#define socket PAL_socket
#define WSAStartup PAL_WSAStartup
#define WSACleanup PAL_WSACleanup
#define WSAConnect PAL_WSAConnect
#define WSASend PAL_WSASend
#define WSASendTo PAL_WSASendTo
#define WSARecv PAL_WSARecv
#define WSARecvFrom PAL_WSARecvFrom
#define WSAEventSelect PAL_WSAEventSelect
#define WSAEnumNetworkEvents PAL_WSAEnumNetworkEvents
#define WSASocketA PAL_WSASocketA
#define WSASocketW PAL_WSASocketW
#define WSAIoctl PAL_WSAIoctl
#define ioctlsocket PAL_ioctlsocket
#define WSAGetOverlappedResult PAL_WSAGetOverlappedResult
#define PostQueuedCompletionStatus PAL_PostQueuedCompletionStatus
#define GetQueuedCompletionStatus PAL_GetQueuedCompletionStatus
#define QueryPerformanceCounter PAL_QueryPerformanceCounter
#define QueryPerformanceFrequency PAL_QueryPerformanceFrequency


//
// Rename PAL typedefs whose names collide with the Win32 versions
//
#define _EXCEPTION_RECORD PAL__EXCEPTION_RECORD
#define EXCEPTION_RECORD PAL_EXCEPTION_RECORD
#define PEXCEPTION_RECORD PAL_PEXCEPTION_RECORD
#define _EXCEPTION_POINTERS PAL__EXCEPTION_POINTERS
#define EXCEPTION_POINTERS PAL_EXCEPTION_POINTERS
#define PEXCEPTION_POINTERS PAL_PEXCEPTION_POINTERS
#define LPEXCEPTION_POINTERS PAL_LPEXCEPTION_POINTERS
#define _EXCEPTION_DISPOSITION PAL__EXCEPTION_DISPOSITION
#define EXCEPTION_DISPOSITION PAL_EXCEPTION_DISPOSITION
#define ExceptionContinueExecution PAL_ExceptionContinueExecution
#define ExceptionContinueSearch PAL_ExceptionContinueSearch
#define _MEMORY_BASIC_INFORMATION PAL__MEMORY_BASIC_INFOMRATION
#define MEMORY_BASIC_INFORMATION PAL_MEMORY_BASIC_INFORMATION
#define PMEMORY_BASIC_INFORMATION PAL_PMEMORY_BASIC_INFORMATION
#define _OSVERSIONINFOA PAL__OSVERSIONINFOA
#define OSVERSIONINFOA PAL_OSVERSIONINFOA
#define LPOSVERSIONINFOA PAL_LPOSVERSIONINFOA
#define POSVERSIONINFOA PAL_POSVERSIONINFOA
#define _OSVERSIONINFOW PAL__OSVERSIONINFOW
#define OSVERSIONINFOW PAL_OSVERSIONINFOW
#define POSVERSIONINFOW PAL_POSVERSIONINFOW
#define LPOSVERSIONINFOW PAL_LPOSVERSIONINFOW
#define OSVERSIONINFO PAL_OSVERSIONINFO
#define POSVERSIONINFO PAL_POSVERSIONINFO
#define LPOSVERSIONINFO PAL_LPOSVERSIONINFO
#define OVERLAPPED PAL_OVERLAPPED
#define LPOVERLAPPED PAL_LPOVERLAPPED
#define _GET_FILEEX_INFO_LEVELS PAL__GET_FILEEX_INFO_LEVELS
#define GET_FILEEX_INFO_LEVELS PAL_GET_FILEEX_INFO_LEVELS
#define GetFileExInfoStandard PAL_GetFileExInfoStandard
#define _STARTUPINFOW PAL__STARTUPINFOW
#define STARTUPINFOW PAL_STARTUPINFOW
#define LPSTARTUPINFOW PAL_LPSTARTUPINFOW
#define _STARTUPINFOA PAL__STARTUPINFOA
#define STARTUPINFOA PAL_STARTUPINFOA
#define LPSTARTUPINFOA PAL_LPSTARTUPINFOA
#define STARTUPINFO PAL_STARTUPINFO
#define LPSTARTUPINFO PAL_LPSTARTUPINFO
#define _PROCESS_INFORMATION PAL__PROCESS_INFORMATION
#define PROCESS_INFORMATION PAL_PROCESS_INFORMATION
#define PPROCESS_INFORMATION PAL_PPROCESS_INFORMATION
#define LPPROCESS_INFORMATION PAL_LPPROCESS_INFORMATION
#define _SYSTEM_INFO PAL__SYSTEM_INFO
#define SYSTEM_INFO PAL_SYSTEM_INFO
#define LPSYSTEM_INFO PAL_LPSYSTEM_INFO
#define _CRITICAL_SECTION PAL__CRITICAL_SECTION
#define CRITICAL_SECTION PAL_CRITICAL_SECTION
#define PCRITICAL_SECTION PAL_PCRITICAL_SECTION
#define LPCRITICAL_SECTION PAL_LPCRITICAL_SECTION
#define _FLOATING_SAVE_AREA PAL__FLOATING_SAVE_AREA
#define FLOATING_SAVE_AREA PAL_FLOATING_SAVE_AREA
#define PFLOATING_SAVE_AREA PAL_PFLOATING_SAVE_AREA
#define _CONTEXT PAL__CONTEXT
#define CONTEXT PAL_CONTEXT
#define PCONTEXT PAL_PCONTEXT
#define LPCONTEXT PAL_LPCONTEXT
#define PTOP_LEVEL_EXCEPTION_FILTER PAL_PTOP_LEVEL_EXCEPTION_FILTER
#define LPTOP_LEVEL_EXCEPTION_FILTER PAL_LPTOP_LEVEL_EXCEPTION_FILTER
#define HMODULE PAL_HMODULE
#define HINSTANCE PAL_HINSTANCE
#define _SECURITY_ATTRIBUTES PAL__SECURITY_ATTRIBUTES
#define SECURITY_ATTRIBUTES PAL_SECURITY_ATTRIBUTES
#define PSECURITY_ATTRIBUTES PAL_PSECURITY_ATTRIBUTES
#define LPSECURITY_ATTRIBUTES PAL_LPSECURITY_ATTRIBUTES
#define _FILETIME PAL__FILETIME
#define FILETIME PAL_FILETIME
#define PFILETIME PAL_PFILETIME
#define LPFILETIME PAL_LPFILETIME
#define BY_HANDLE_FILE_INFORMATION PAL_BY_HANDLE_FILE_INFORMATION
#define _BY_HANDLE_FILE_INFORMATION PAL__BY_HANDLE_FILE_INFORMATION
#define PBY_HANDLE_FILE_INFORMATION PAL_PBY_HANDLE_FILE_INFORMATION
#define LPBY_HANDLE_FILE_INFORMATION PAL_LPBY_HANDLE_FILE_INFORMATION
#define WIN32_FIND_DATA PAL_WIN32_FIND_DATA
#define PWIN32_FIND_DATA PAL_PWIN32_FIND_DATA
#define LPWIN32_FIND_DATA PAL_LPWIN32_FIND_DATA
#define _WIN32_FIND_DATAA PAL__WIN32_FIND_DATAA
#define WIN32_FIND_DATAA PAL_WIN32_FIND_DATAA
#define PWIN32_FIND_DATAA PAL_PWIN32_FIND_DATAA
#define LPWIN32_FIND_DATAA PAL_LPWIN32_FIND_DATAA
#define _WIN32_FIND_DATAW PAL__WIN32_FIND_DATAW
#define WIN32_FIND_DATAW PAL_WIN32_FIND_DATAW
#define PWIN32_FIND_DATAW PAL_PWIN32_FIND_DATAW
#define LPWIN32_FIND_DATAW PAL_LPWIN32_FIND_DATAW
#define _WIN32_FILE_ATTRIBUTE_DATA PAL__WIN32_FILE_ATTRIBUTE_DATA
#define WIN32_FILE_ATTRIBUTE_DATA PAL_WIN32_FILE_ATTRIBUTE_DATA
#define LPWIN32_FILE_ATTRIBUTE_DATA PAL_LPWIN32_FILE_ATTRIBUTE_DATA
#define _SYSTEMTIME PAL__SYSTEMTIME
#define SYSTEMTIME PAL_SYSTEMTIME
#define PSYSTEMTIME PAL_PSYSTEMTIME
#define LPSYSTEMTIME PAL_LPSYSTEMTIME
#define _cpinfo PAL__cpinfo
#define CPINFO PAL_CPINFO
#define LPCPINFO PAL_LPCPINFO
#define _TIME_ZONE_INFORMATION PAL__TIME_ZONE_INFORMATION
#define TIME_ZONE_INFORMATION PAL_TIME_ZONE_INFORMATION
#define PTIME_ZONE_INFORMATION PAL_PTIME_ZONE_INFORMATION
#define LPTIME_ZONE_INFORMATION PAL_LPTIME_ZONE_INFORMATION
#define _GUID PAL__GUID
#define GUID PAL_GUID
#define _FILE PAL__FILE
#define FILE PAL_FILE
#define tm PAL_tm
#define hostent PAL_hostent
#define sockaddr PAL_sockaddr
#define fd_set PAL_fd_set
#define timeval PAL_timeval
#define WSAData PAL_WSAData
#define WSADATA PAL_WSADATA
#define LPWSADATA PAL_LPWSADATA
#define _WSABUF PAL__WSABUF
#define WSABUF PAL_WSABUF
#define LPWSABUF PAL_LPWSABUF
#define _WSAOVERLAPPED PAL__WSAOVERLAPPED
#define WSAOVERLAPPED PAL_WSAOVERLAPPED
#define LPWSAOVERLAPPED PAL_LPWSAOVERLAPPED
#define LPWSAOVERLAPPED_COMPLETION_ROUTINE PAL_LPWSAOVERLAPPED_COMPLETION_ROUTINE
#define SERVICETYPE PAL_SERVICETYPE
#define _flowspec PAL__flowspec
#define FLOWSPEC PAL_FLOWSPEC
#define PFLOWSPEC PAL_PFLOWSPEC
#define LPFLOWSPEC PAL_LPFLOWSPEC
#define _QualityOfService PAL__QualityOfService
#define QOS PAL_QOS
#define LPQOS PAL_LPQOS
#define _RIP_INFO PAL__RIP_INFO
#define RIP_INFO PAL_RIP_INFO
#define LPRIP_INFO PAL_LPRIP_INFO
#define _OUTPUT_DEBUG_STRING_INFO PAL__OUTPUT_DEBUG_STRING_INFO
#define OUTPUT_DEBUG_STRING_INFO PAL_OUTPUT_DEBUG_STRING_INFO
#define _UNLOAD_DLL_DEBUG_INFO PAL__UNLOAD_DLL_DEBUG_INFO
#define UNLOAD_DLL_DEBUG_INFO PAL_UNLOAD_DLL_DEBUG_INFO
#define _LOAD_DLL_DEBUG_INFO PAL__LOAD_DLL_DEBUG_INFO
#define LOAD_DLL_DEBUG_INFO PAL_LOAD_DLL_DEBUG_INFO
#define _EXIT_PROCESS_DEBUG_INFO PAL__EXIT_PROCESS_DEBUG_INFO
#define EXIT_PROCESS_DEBUG_INFO PAL_EXIT_PROCESS_DEBUG_INFO
#define _EXIT_THREAD_DEBUG_INFO PAL__EXIT_THREAD_DEBUG_INFO
#define EXIT_THREAD_DEBUG_INFO PAL_EXIT_THREAD_DEBUG_INFO
#define _CREATE_PROCESS_DEBUG_INFO PAL__CREATE_PROCESS_DEBUG_INFO
#define CREATE_PROCESS_DEBUG_INFO PAL_CREATE_PROCESS_DEBUG_INFO
#define _CREATE_THREAD_DEBUG_INFO PAL__CREATE_THREAD_DEBUG_INFO
#define CREATE_THREAD_DEBUG_INFO PAL_CREATE_THREAD_DEBUG_INFO
#define LPCREATE_THREAD_DEBUG_INFO PAL_LPCREATE_THREAD_DEBUG_INFO
#define _EXCEPTION_DEBUG_INFO PAL__EXCEPTION_DEBUG_INFO
#define EXCEPTION_DEBUG_INFO PAL_EXCEPTION_DEBUG_INFO
#define _DEBUG_EVENT PAL__DEBUG_EVENT
#define DEBUG_EVENT PAL_DEBUG_EVENT
#define LPDEBUG_EVENT PAL_LPDEBUG_EVENT
#define _WSAPROTOCOLCHAIN PAL__WSAPROTOCOLCHAIN
#define WSAPROTOCOLCHAIN PAL_WSAPROTOCOLCHAIN
#define LPWSAPROTOCOLCHAIN PAL_LPWSAPROTOCOLCHAIN
#define _WSAPROTOCOL_INFOA PAL__WSAPROTOCOL_INFOA
#define WSAPROTOCOL_INFOA PAL_WSAPROTOCOL_INFOA
#define LPWSAPROTOCOL_INFOA PAL_LPWSAPROTOCOL_INFOA
#define _WSAPROTOCOL_INFOW PAL__WSAPROTOCOL_INFOW
#define WSAPROTOCOL_INFOW PA_WSAPROTOCOL_INFOW
#define LPWSAPROTOCOL_INFOW PAL_LPWSAPROTOCOL_INFOW
#define WSAPROTOCOL_INFO PAL_WSAPROTOCOL_INFO
#define LPWSAPROTOCOL_INFO PAL_LPWSAPROTOCOL_INFO
#define in_addr PAL_in_addr
#define sockaddr_in PAL_sockaddr_in
#define linger PAL_linger
#define _WSANETWORKEVENTS PAL__WSANETWORKEVENTS
#define WSANETWORKEVENTS PAL_WSANETWORKEVENTS
#define LPWSANETWORKEVENTS PAL_LPWSANETWORKEVENTS
#define _LARGE_INTEGER PAL__LARGE_INTEGER
#define LARGE_INTEGER PAL_LARGE_INTEGER
#define PLARGE_INTEGER PAL_PLARGE_INTEGER

#define fpos_t PAL_fpos_t

#pragma warning(disable:4142) // benign redefinition of type
//
// Pull in the PAL API declarations - the PALEXPORT/NOTLOGGED macros will
// rename all of the APIs to PAL_xxxxx so they don't collide with the
// Win32 API names
//
#include <rotor_pal.h>

// Undo the API renames
#undef CharNextA
#undef CharNextExA
#undef wsprintfA
#undef wsprintfW
#undef MessageBoxW
#undef SetConsoleCtrlHandler
#undef GenerateConsoleCtrlEvent
#undef AreFileApisANSI
#undef CreateFileA
#undef CreateFileW
#undef LockFile
#undef UnlockFile
#undef SearchPathA
#undef SearchPathW
#undef CreatePipe
#undef CopyFileA
#undef CopyFileW
#undef DeleteFileA
#undef DeleteFileW
#undef MoveFileA
#undef MoveFileW
#undef MoveFileExA
#undef MoveFileExW
#undef CreateDirectoryA
#undef CreateDirectoryW
#undef RemoveDirectoryA
#undef RemoveDirectoryW
#undef FindFirstFileA
#undef FindFirstFileW
#undef FindNextFileA
#undef FindNextFileW
#undef FindClose
#undef GetFileAttributesA
#undef GetFileAttributesW
#undef GetFileAttributesExA
#undef GetFileAttributesExW
#undef SetFileAttributesA
#undef SetFileAttributesW
#undef WriteFile
#undef ReadFile
#undef GetStdHandle
#undef SetEndOfFile
#undef SetFilePointer
#undef GetFileSize
#undef GetFileInformationByHandle
#undef CompareFileTime
#undef SetFileTime
#undef GetFileTime
#undef FileTimeToLocalFileTime
#undef LocalFileTimeToFileTime
#undef GetSystemTimeAsFileTime
#undef GetSystemTime
#undef FileTimeToDosDateTime
#undef DosDateTimeToFileTime
#undef FlushFileBuffers
#undef GetFileType
#undef GetConsoleCP
#undef GetConsoleOutputCP
#undef GetFullPathNameA
#undef GetFullPathNameW
#undef GetLongPathNameW
#undef GetShortPathNameW
#undef GetTempFileNameA
#undef GetTempFileNameW
#undef GetTempPathA
#undef GetTempPathW
#undef GetCurrentDirectoryA
#undef GetCurrentDirectoryW
#undef SetCurrentDirectoryA
#undef SetCurrentDirectoryW
#undef GetUserNameW
#undef GetComputerNameW
#undef CreateSemaphoreA
#undef CreateSemaphoreW
#undef ReleaseSemaphore
#undef CreateEventA
#undef CreateEventW
#undef SetEvent
#undef ResetEvent
#undef OpenEventW
#undef CreateMutexA
#undef CreateMutexW
#undef ReleaseMutex
#undef GetCurrentProcessId
#undef GetCurrentProcess
#undef GetCurrentThreadId
#undef GetCurrentThread
#undef CreateProcessW
#undef CreateProcessA
#undef ExitProcess
#undef TerminateProcess
#undef GetExitCodeProcess
#undef GetProcessTimes
#undef WaitForSingleObject
#undef WaitForSingleObjectEx
#undef WaitForMultipleObjects
#undef WaitForMultipleObjectsEx
#undef DuplicateHandle
#undef Sleep
#undef SleepEx
#undef SwitchToThread
#undef CreateThread
#undef ExitThread
#undef SuspendThread
#undef ResumeThread
#undef QueueUserAPC
#undef GetThreadContext
#undef SetThreadContext
#undef GetThreadPriority
#undef SetThreadPriority
#undef TlsAlloc
#undef TlsGetValue
#undef TlsSetValue
#undef TlsFree
#undef EnterCriticalSection
#undef LeaveCriticalSection
#undef InitializeCriticalSection
#undef DeleteCriticalSection
#undef TryEnterCriticalSection
#undef SetErrorMode
#undef CreateFileMappingA
#undef CreateFileMappingW
#undef OpenFileMappingA
#undef OpenFileMappingW
#undef MapViewOfFile
#undef UnmapViewOfFile
#undef LoadLibraryA
#undef LoadLibraryW
#undef GetProcAddress
#undef FreeLibrary
#undef FreeLibraryAndExitThread
#undef DisableThreadLibraryCalls
#undef GetModuleFileNameA
#undef GetModuleFileNameW
#undef VirtualAlloc
#undef VirtualFree
#undef VirtualProtect
#undef VirtualQuery
#undef ReadProcessMemory
#undef WriteProcessMemory
#undef GetProcessHeap
#undef HeapAlloc
#undef HeapReAlloc
#undef HeapFree
#undef LocalAlloc
#undef LocalFree
#undef FlushInstructionCache
#undef GetStringTypeExW
#undef CompareStringA
#undef CompareStringW
#undef GetACP
#undef GetCPInfo
#undef IsDBCSLeadByteEx
#undef IsValidCodePage
#undef MultiByteToWideChar
#undef WideCharToMultiByte
#undef GetSystemDefaultLangID
#undef GetUserDefaultLangID
#undef SetThreadLocale
#undef GetThreadLocale
#undef GetLocaleInfoW
#undef GetUserDefaultLCID
#undef GetTimeZoneInformation
#undef IsValidLocale
#undef GetCalendarInfoW
#undef GetDateFormatW
#undef OutputDebugStringA
#undef OutputDebugStringW
#undef DebugBreak
#undef WaitForDebugEvent
#undef ContinueDebugEvent
#undef DebugActiveProcess
#undef OpenProcess
#undef lstrcatW
#undef lstrcpyW
#undef lstrlenA
#undef lstrlenW
#undef lstrcpynW
#undef GetEnvironmentVariableA
#undef GetEnvironmentVariableW
#undef SetEnvironmentVariableA
#undef SetEnvironmentVariableW
#undef GetEnvironmentStringsA
#undef GetEnvironmentStringsW
#undef FreeEnvironmentStringsA
#undef FreeEnvironmentStringsW
#undef CloseHandle
#undef RaiseException
#undef GetTickCount
#undef SetUnhandledExceptionFilter
#undef InterlockedIncrement
#undef InterlockedDecrement
#undef InterlockedExchange
#undef InterlockedCompareExchange
#undef InterlockedExchangePointer
#undef InterlockedCompareExchangePointer
#undef MemoryBarrier
#undef YieldProcessor
#undef IsBadReadPtr
#undef IsBadWritePtr
#undef IsBadCodePtr
#undef FormatMessageW
#undef GetLastError
#undef SetLastError
#undef GetCommandLineW
#undef GetVersionExA
#undef GetVersionExW
#undef GetSystemInfo
#undef GetDiskFreeSpaceW
#undef memcpy
#undef memcmp
#undef memset
#undef memmove
#undef strlen
#undef sprintf
#undef vsprintf
#undef _snprintf
#undef _vsnprintf
#undef sscanf
#undef _stricmp
#undef strstr
#undef strtok
#undef strspn
#undef strcspn
#undef strtoul
#undef atof
#undef _gcvt
#undef _ecvt
#undef strtod
#undef _strlwr
#undef _mbslen
#undef _mbsinc
#undef _mbsninc
#undef _mbsdec
#undef wcstol
#undef wcstoul
#undef wcstod
#undef wcscat
#undef wcscpy
#undef wcslen
#undef wcsncmp
#undef _wcsicmp
#undef wcschr
#undef wcsrchr
#undef _wcsnicmp
#undef swprintf
#undef vswprintf
#undef _snwprintf
#undef _vsnwprintf
#undef swscanf
#undef wcspbrk
#undef wcsstr
#undef wcstok
#undef wcscspn
#undef _wcslwr
#undef isprint
#undef isspace
#undef isalpha
#undef isalnum
#undef isdigit
#undef __iscsym
#undef iswdigit
#undef iswxdigit
#undef isxdigit
#undef _finite
#undef _isnan
#undef _copysign
#undef abs
#undef labs
#undef _rotl
#undef _rotr
#undef _alloca
#undef ceil
#undef floor
#undef modf
#undef log
#undef malloc
#undef free
#undef realloc
#undef _strdup
#undef exit
#undef atexit
#undef qsort
#undef bsearch
#undef _splitpath
#undef _wsplitpath
#undef _makepath
#undef _wmakepath
#undef _fullpath
#undef _swab
#undef time
#undef localtime
#undef mktime
#undef _open_osfhandle
#undef fclose
#undef setbuf
#undef fwrite
#undef fread
#undef fgets
#undef fputs
#undef fprintf
#undef fwprintf
#undef vfprintf
#undef vfwprintf
#undef _getw
#undef _putw
#undef fseek
#undef ftell
#undef fgetpos
#undef fsetpos
#undef feof
#undef ferror
#undef fopen
#undef _fdopen
#undef _close
#undef _wfopen
#undef fputc
#undef putchar
#undef fgetws
#undef getc
#undef fgetc
#undef ungetc
#undef fgets
#undef srand
#undef rand
#undef ctime
#undef printf
#undef vprintf
#undef wprintf
#undef memchr
#undef strcmp
#undef strncmp
#undef _strnicmp
#undef strcat
#undef strncat
#undef strcpy
#undef strncpy
#undef strchr
#undef strrchr
#undef strpbrk
#undef atoi
#undef atol
#undef tolower
#undef toupper
#undef wcsspn
#undef wcscmp
#undef wcsncat
#undef wcsncpy
#undef _itow
#undef _i64tow
#undef _ui64tow
#undef _wtoi
#undef iswalpha
#undef iswprint
#undef iswupper
#undef iswspace
#undef isupper
#undef islower
#undef towlower
#undef towupper
#undef fflush
#undef getenv
#undef _putenv
#undef gethostbyname
#undef gethostbyaddr
#undef gethostname
#undef inet_addr
#undef getpeername
#undef getsockopt
#undef setsockopt
#undef connect
#undef send
#undef recv
#undef closesocket
#undef accept
#undef listen
#undef bind
#undef shutdown
#undef sendto
#undef recvfrom
#undef getsockname
#undef select
#undef socket
#undef WSAStartup
#undef WSACleanup
#undef WSAConnect 
#undef WSASend
#undef WSASendTo
#undef WSARecv
#undef WSARecvFrom
#undef WSAEventSelect
#undef WSAEnumNetworkEvents
#undef WSASocketA
#undef WSASocketW
#undef WSAIoctl
#undef ioctlsocket
#undef WSAGetOverlappedResult
#undef PostQueuedCompletionStatus
#undef GetQueuedCompletionStatus
#undef QueryPerformanceCounter
#undef QueryPerformanceFrequency


// Undo the typedef renames
#undef _EXCEPTION_RECORD
#undef EXCEPTION_RECORD
#undef PEXCEPTION_RECORD
#undef _EXCEPTION_POINTERS
#undef EXCEPTION_POINTERS
#undef PEXCEPTION_POINTERS
#undef LPEXCEPTION_POINTERS
#undef _EXCEPTION_DISPOSITION
#undef EXCEPTION_DISPOSITION
#undef ExceptionContinueExecution
#undef ExceptionContinueSearch
#undef _MEMORY_BASIC_INFORMATION
#undef MEMORY_BASIC_INFORMATION
#undef PMEMORY_BASIC_INFORMATION
#undef _OSVERSIONINFOA
#undef OSVERSIONINFOA
#undef POSVERSIONINFOA
#undef LPOSVERSIONINFOA
#undef _OSVERSIONINFOW
#undef OSVERSIONINFOW
#undef POSVERSIONINFOW
#undef LPOSVERSIONINFOW
#undef OSVERSIONINFO
#undef POSVERSIONINFO
#undef LPOSVERSIONINFO
#undef FLOATING_SAVE_AREA
#undef _FLOATING_SAVE_AREA
#undef PFLOATING_SAVE_AREA
#undef CONTEXT
#undef PCONTEXT
#undef LPCONTEXT
#undef OVERLAPPED
#undef LPOVERLAPPED
#undef _GET_FILEEX_INFO_LEVELS
#undef GET_FILEX_INFO_LEVELS
#undef GetFileExInfoStandard
#undef _STARTUPINFOW
#undef STARTUPINFOW
#undef LPSTARTUPINFOW
#undef _STARTUPINFOA
#undef STARTUPINFOA
#undef LPSTARTUPINFOA
#undef STARTUPINFO
#undef LPSTARTUPINFO
#undef _PROCESS_INFORMATION
#undef PROCESS_INFORMATION
#undef PPROCESS_INFORMATION
#undef LPPROCESS_INFORMATION
#undef _CRITICAL_SECTION
#undef CRITICAL_SECTION
#undef PCRITICAL_SECTION
#undef LPCRITICAL_SECTION
#undef _SYSTEM_INFO
#undef SYSTEM_INFO
#undef LPSYSTEM_INFO
#undef _CONTEXT
#undef CONTEXT
#undef PCONTEXT
#undef LPCONTEXT
#undef PTOP_LEVEL_EXCEPTION_FILTER
#undef LPTOP_LEVEL_EXCEPTION_FILTER
#undef HMODULE
#undef HINSTANCE
#undef _SECURITY_ATTRIBUTES
#undef SECURITY_ATTRIBUTES
#undef PSECURITY_ATTRIBUTES
#undef LPSECURITY_ATTRIBUTES
#undef _FILETIME
#undef FILETIME
#undef PFILETIME
#undef LPFILETIME
#undef _BY_HANDLE_FILE_INFORMATION
#undef BY_HANDLE_FILE_INFORMATION
#undef PBY_HANDLE_FILE_INFORMATION
#undef LPBY_HANDLE_FILE_INFORMATION
#undef WIN32_FIND_DATA
#undef PWIN32_FIND_DATA
#undef LPWIN32_FIND_DATA
#undef _WIN32_FIND_DATAA
#undef WIN32_FIND_DATAA
#undef PWIN32_FIND_DATAA
#undef LPWIN32_FIND_DATAA
#undef _WIN32_FIND_DATAW
#undef WIN32_FIND_DATAW
#undef PWIN32_FIND_DATAW
#undef LPWIN32_FIND_DATAW
#undef _WIN32_FILE_ATTRIBUTE_DATA
#undef WIN32_FILE_ATTRIBUTE_DATA
#undef LPWIN32_FILE_ATTRIBUTE_DATA
#undef _SYSTEMTIME
#undef SYSTEMTIME
#undef PSYSTEMTIME
#undef LPSYSTEMTIME
#undef _cpinfo
#undef CPINFO
#undef LPCPINFO
#undef _TIME_ZONE_INFORMATION
#undef TIME_ZONE_INFORMATION
#undef PTIME_ZONE_INFORMATION
#undef LPTIME_ZONE_INFORMATION
#undef _GUID
#undef GUID
#undef _FILE
#undef FILE
#undef tm
#undef hostent
#undef sockaddr
#undef fd_set
#undef timeval
#undef WSAData
#undef WSADATA
#undef LPWSADATA
#undef _WSABUF
#undef WSABUF
#undef LPWSABUF
#undef _WSAOVERLAPPED
#undef WSAOVERLAPPED
#undef LPWSAOVERLAPPED
#undef LPWSAOVERLAPPED_COMPLETION_ROUTINE
#undef SERVICETYPE
#undef _flowspec
#undef FLOWSPEC
#undef PFLOWSPEC
#undef LPFLOWSPEC
#undef _QualityOfService
#undef QOS
#undef LPQOS
#undef _RIP_INFO
#undef RIP_INFO
#undef LPRIP_INFO
#undef _OUTPUT_DEBUG_STRING_INFO
#undef OUTPUT_DEBUG_STRING_INFO
#undef _UNLOAD_DLL_DEBUG_INFO
#undef UNLOAD_DLL_DEBUG_INFO
#undef _LOAD_DLL_DEBUG_INFO
#undef LOAD_DLL_DEBUG_INFO
#undef _EXIT_PROCESS_DEBUG_INFO
#undef EXIT_PROCESS_DEBUG_INFO
#undef _EXIT_THREAD_DEBUG_INFO
#undef EXIT_THREAD_DEBUG_INFO
#undef _CREATE_PROCESS_DEBUG_INFO
#undef CREATE_PROCESS_DEBUG_INFO
#undef _CREATE_THREAD_DEBUG_INFO
#undef CREATE_THREAD_DEBUG_INFO
#undef LPCREATE_THREAD_DEBUG_INFO
#undef _EXCEPTION_DEBUG_INFO
#undef EXCEPTION_DEBUG_INFO
#undef _DEBUG_EVENT
#undef DEBUG_EVENT
#undef LPDEBUG_EVENT
#undef STILL_ACTIVE
#undef _WSAPROTOCOLCHAIN
#undef WSAPROTOCOLCHAIN
#undef LPWSAPROTOCOLCHAIN
#undef _WSAPROTOCOL_INFOA
#undef WSAPROTOCOL_INFOA
#undef LPWSAPROTOCOL_INFOA
#undef _WSAPROTOCOL_INFOW
#undef WSAPROTOCOL_INFOW
#undef LPWSAPROTOCOL_INFOW
#undef WSAPROTOCOL_INFO
#undef LPWSAPROTOCOL_INFO
#undef in_addr
#undef sockaddr_in
#undef linger
#undef _WSANETWORKEVENTS
#undef WSANETWORKEVENTS
#undef LPWSANETWORKEVENTS
#undef _LARGE_INTEGER
#undef LARGE_INTEGER
#undef PLARGE_INTEGER

#undef fpos_t

//
// Undefine some Win32 defines that will conflict with PAL versions
//
#undef STATUS_USER_APC
#undef STATUS_GUARD_PAGE_VIOLATION
#undef STATUS_DATATYPE_MISALIGNMENT
#undef STATUS_BREAKPOINT
#undef STATUS_SINGLE_STEP
#undef STATUS_ACCESS_VIOLATION
#undef STATUS_IN_PAGE_ERROR
#undef STATUS_INVALID_HANDLE
#undef STATUS_NO_MEMORY
#undef STATUS_ILLEGAL_INSTRUCTION
#undef STATUS_NONCONTINUABLE_EXCEPTION
#undef STATUS_INVALID_DISPOSITION
#undef STATUS_ARRAY_BOUNDS_EXCEEDED
#undef STATUS_FLOAT_DENORMAL_OPERAND
#undef STATUS_FLOAT_DIVIDE_BY_ZERO
#undef STATUS_FLOAT_INEXACT_RESULT
#undef STATUS_FLOAT_INVALID_OPERATION
#undef STATUS_FLOAT_OVERFLOW
#undef STATUS_FLOAT_STACK_CHECK
#undef STATUS_FLOAT_UNDERFLOW
#undef STATUS_INTEGER_DIVIDE_BY_ZERO
#undef STATUS_INTEGER_OVERFLOW
#undef STATUS_PRIVILEGED_INSTRUCTION
#undef STATUS_STACK_OVERFLOW
#undef STATUS_CONTROL_C_EXIT
#undef INVALID_FILE_ATTRIBUTES
#undef memcpy
#undef memcmp
#undef memset
#undef strlen
#undef FAR
#undef TEXT
#undef iswupper
#undef iswspace
#undef VOID
#undef INVALID_HANDLE_VALUE
#undef LANGIDFROMLCID
#undef WAIT_OBJECT_0
#undef WAIT_ABANDONED
#undef WAIT_ABANDONED_0
#undef WAIT_TIMEOUT
#undef WAIT_FAILED
#undef INFINITE
#undef SECTION_ALL_ACCESS
#undef EVENT_ALL_ACCESS
#undef EVENT_MODIFY_STATE
#undef MUTANT_QUERY_STATE
#undef MUTANT_ALL_ACCESS
#undef MUTEX_ALL_ACCESS
#undef SEMAPHORE_MODIFY_STATE
#undef SEMAPHORE_ALL_ACCESS
#undef CopyMemory
#undef MoveMemory
#undef FillMemory
#undef ZeroMemory
#undef min
#undef max
#undef CAL_RETURN_NUMBER
#undef va_start
#undef va_end
#undef stdout
#undef stdin
#undef stderr
#undef errno
#undef RtlMoveMemory
#undef FLT_MAX
#undef DBL_MAX
#undef GUID_DEFINED
#undef EXTERN_C
#undef _I8_MIN
#undef _I8_MAX
#undef _UI8_MAX
#undef _I16_MIN
#undef _I16_MAX
#undef _UI16_MAX
#undef _I32_MIN
#undef _I32_MAX
#undef _UI32_MAX
#undef _I64_MIN
#undef _I64_MAX
#undef _UI64_MAX
//
// Pull in standard Win32 declarations
//
#pragma warning(disable : 4996) // 'function': was declared deprecated
#include <windows.h>
#include <winnls.h>
#include <wincrypt.h>
#include <objbase.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <io.h>
#include <float.h>
#include <winsock2.h>
#include <setjmp.h>
#include <locale.h>
#include <math.h>

#ifdef __GNUC__
// Extra headers and definitions for mingw build
// The headers we pulled in above were from the MinGW runtim
// but they are missing certain definitions.
// So we pull in more MingW headers
// or borrow definitions from the Win32 headers.

#define	EH_NONCONTINUABLE	0x01
#define	EH_UNWINDING		0x02
#define	EH_EXIT_UNWIND		0x04
#define	EH_STACK_INVALID	0x08
#define	EH_NESTED_CALL		0x10

// from excpt.h (Win32 CRT header)
typedef enum {
	ExceptionContinueExecution,
	ExceptionContinueSearch,
	ExceptionNestedException,
	ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;

// from float.h (Win32 header)
#define _DN_SAVE 0x00000000
#define _MCW_DN 0x03000000

// reversed the definitions from winnt.h (Win32 header)
// The MingW headers do not define the symbols being defined
// winnt.h (Win32 header) #defines the latter symbols as the former
#define IMAGE_NT_HEADERS32 IMAGE_NT_HEADERS
#define IMAGE_SIZEOF_NT_OPTIONAL32_HEADER IMAGE_SIZEOF_NT_OPTIONAL_HEADER
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC IMAGE_NT_OPTIONAL_HDR_MAGIC

// directly from winnt.h (Win32 header)
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

int
WINAPI
GetCalendarInfoW(
    LCID     Locale,
    CALID    Calendar,
    CALTYPE  CalType,
    LPWSTR   lpCalData,
    int      cchData,
    LPDWORD  lpValue);

#include <wincrypt.h>

// According to wincrypt.h (Win32 header), the two are the same
// Although not explicitly defined like that
// MinGW headers don't define CALG_SHA1
#define CALG_SHA1 CALG_SHA

#endif

#if DBG

void
PalAssertFailed(
    const char *FileName, 
    int LineNumber,
    const char *Expr);

#define PALASSERT(Expr) \
    do { if (!(Expr)) PalAssertFailed(__FILE__, __LINE__, #Expr); } while (0)

void
__cdecl
PalLogApi(
          const char *fmt,
          ...);

#define LOGAPI PalLogApi

#else //DBG

#define PALASSERT(Expr)
#define LOGAPI

#endif //DBG



#undef __try
#undef __except
#undef __finally
#undef __leave

