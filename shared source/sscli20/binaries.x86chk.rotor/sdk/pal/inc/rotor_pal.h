/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


 This file includes parts which are Copyright (c) 1982-1986 Regents
 of the University of California.  All rights reserved.  The
 Berkeley Software License Agreement specifies the terms and
 conditions for redistribution.


Module Name:

    rotor_pal.h

Abstract:

    Rotor Platform Adaptation Layer (PAL) header file.  This file
    defines all types and API calls required by the Rotor port of
    the Microsoft Common Language Runtime.

    Defines which control the behavior of this include file:
      UNICODE - define it to set the Ansi/Unicode neutral names to
                be the ...W names.  Otherwise the neutral names default
                to be the ...A names.
      PAL_IMPLEMENTATION - define it when implementing the PAL.  Otherwise
                leave it undefined when consuming the PAL.

    Note:  some fields in structs have been renamed from the original
    SDK documentation names, with _PAL_Undefined appended.  This leaves
    the structure layout identical to its Win32 version, but prevents
    PAL consumers from inadvertently referencing undefined fields.

    If you want to add a PAL_ wrapper function to a native function in
    here, you also need to edit palinternal.h and win32pal.h.
    
--*/

#ifndef __ROTOR_PAL_H__
#define __ROTOR_PAL_H__

#ifdef  __cplusplus
extern "C" {
#if defined (PLATFORM_UNIX) && defined(__GNUC__)
/* the unix compilers use a 32-bit wchar_t, so we must make a 16 bit wchar_t.
   the windows compilers, gcc and MSVC, both define a 16 bit wchar_t. */
/* note : wchar_t is a built-in type in C++, gcc ignores any attempts to
   typedef it. Using the preprocessor here, we make sure gcc sees
   __wchar_16_cpp__ instead of wchar_t. This is apparently not necessary under
   vc++, for whom wchar_t is already a typedef instead of a built-in. */
#undef wchar_t   
#define wchar_t __wchar_16_cpp__
#endif // PLATFORM_UNIX
#endif

#ifndef _WCHAR_T_DEFINED
#if defined (PLATFORM_UNIX) || !defined(__GNUC__) || !defined (__cplusplus)
typedef unsigned short wchar_t;
#endif
#define _WCHAR_T_DEFINED
#endif

#include <pal_error.h>
#include <pal_mstypes.h>

/******************* Compiler-specific glue *******************************/

#ifndef _MSC_VER
#define PAL_PORTABLE_SEH 1
#endif // !_MSC_VER


#ifdef _MSC_VER
#define DECLSPEC_ALIGN(x)   __declspec(align(x))
#else
#define DECLSPEC_ALIGN(x) 
#endif

#define DECLSPEC_NORETURN   PAL_NORETURN

#ifndef _MSC_VER
#define __assume(x) (void)0
#define __annotation(x)
#endif //!MSC_VER

/******************* Processor-specific glue  *****************************/

#ifndef _MSC_VER

#if defined(__i686__) && !defined(_M_IX86)
#define _M_IX86 600
#elif defined(__i586__) && !defined(_M_IX86)
#define _M_IX86 500
#elif defined(__i486__) && !defined(_M_IX86)
#define _M_IX86 400
#elif defined(__i386__) && !defined(_M_IX86)
#define _M_IX86 300
#elif defined(__ppc__) && !defined(_M_PPC)
#define _M_PPC 100
#endif

#if defined(_M_IX86) && !defined(_X86_)
#define _X86_
#elif defined(_M_PPC) && !defined(_PPC_)
#define _PPC_
#endif

#endif // !_MSC_VER

#ifdef _MSC_VER

#if defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_PPC) || defined(_M_IA64)
#define UNALIGNED __unaligned
#else
#define UNALIGNED
#endif

#else // _MSC_VER

#define UNALIGNED

#endif // _MSC_VER

#if __GNUC__


#ifdef _X86_
typedef char * va_list;
typedef char * __gnuc_va_list;
#else   // _X86_
#if __GNUC__ == 2
typedef void * va_list;
#else
typedef __builtin_va_list va_list;
#endif  // __GNUC__
#endif  // _X86_

/* We should consider if the va_arg definition here is actually necessary.
   Could we use the standard va_arg definition? */

#if __GNUC__ == 2
// GCC 2.95.3 on non-SPARC
#define __va_size(type) (((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
#define va_start(list, v) ((list) = (va_list) __builtin_next_arg(v))
#define va_arg(ap, type) (*(type *)((ap) += __va_size(type), (ap) - __va_size(type)))
#else // __GNUC__ == 2
#define va_start    __builtin_stdarg_start
#define va_arg      __builtin_va_arg
#endif // __GNUC__ == 2

#define va_end(list)

    
#else // __GNUC__

typedef char * va_list;

#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#if _MSC_VER >= 1400

#ifdef  __cplusplus
#define _ADDRESSOF(v)   ( &reinterpret_cast<const char &>(v) )
#else
#define _ADDRESSOF(v)   ( &(v) )
#endif

#define _crt_va_start(ap,v)  ( ap = (va_list)_ADDRESSOF(v) + _INTSIZEOF(v) )
#define _crt_va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define _crt_va_end(ap)      ( ap = (va_list)0 )

#define va_start _crt_va_start
#define va_arg _crt_va_arg
#define va_end _crt_va_end

#else  // _MSC_VER

#define va_start(ap,v)    (ap = (va_list) (&(v)) + _INTSIZEOF(v))
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)

#endif // _MSC_VER

#endif // __GNUC__

/******************* PAL-Specific Entrypoints *****************************/





#define MAX_PATH 260
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */

#define MAXSHORT      0x7fff
#define MAXLONG   0x7fffffff
#define MAXCHAR      0x7f
 
#define MAXIMUM_SUSPEND_COUNT  MAXCHAR

#define CHAR_BIT      8

#define SCHAR_MIN   (-128)
#define SCHAR_MAX     127
#define UCHAR_MAX     0xff

#define SHRT_MIN    (-32768)
#define SHRT_MAX      32767
#define USHRT_MAX     0xffff

#define INT_MIN     (-2147483647 - 1)
#define INT_MAX       2147483647
#define UINT_MAX      0xffffffff

#define LONG_MIN    (-2147483647L - 1)
#define LONG_MAX      2147483647L
#define ULONG_MAX     0xffffffffUL

#define FLT_MAX 3.402823466e+38F
#define DBL_MAX 1.7976931348623157e+308

/* minimum signed 64 bit value */
#define _I64_MIN    (I64(-9223372036854775807) - 1)
/* maximum signed 64 bit value */
#define _I64_MAX      I64(9223372036854775807)
/* maximum unsigned 64 bit value */
#define _UI64_MAX     UI64(0xffffffffffffffff)

#define _I8_MAX   SCHAR_MAX
#define _I8_MIN   SCHAR_MIN
#define _I16_MAX  SHRT_MAX
#define _I16_MIN  SHRT_MIN
#define _I32_MAX  INT_MAX
#define _I32_MIN  INT_MIN
#define _UI8_MAX  UCHAR_MAX
#define _UI8_MIN  UCHAR_MIN
#define _UI16_MAX USHRT_MAX
#define _UI16_MIN USHRT_MIN
#define _UI32_MAX UINT_MAX
#define _UI32_MIN UINT_MIN

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

typedef ULONG64   fpos_t;

#if _WIN64 || _MSC_VER >= 1400
typedef __int64 time_t;
#else
typedef long time_t;
#endif
#define _TIME_T_DEFINED


#define MAKELCID(lgid, srtid)  ((DWORD)((((DWORD)((WORD  )(srtid))) << 16) |  \
                                         ((DWORD)((WORD  )(lgid)))))
#define LANGIDFROMLCID(lcid)   ((WORD)(lcid))

#define LANG_NEUTRAL                     0x00
#define LANG_INVARIANT                   0x7f
#define SUBLANG_NEUTRAL                  0x00    // language neutral
#define SUBLANG_DEFAULT                  0x01    // user default
#define SORT_DEFAULT                     0x0     // sorting default

#define MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define PRIMARYLANGID(lgid)    ((WORD  )(lgid) & 0x3ff)
#define SUBLANGID(lgid)        ((WORD  )(lgid) >> 10)

#define LANG_USER_DEFAULT      (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
#define LOCALE_USER_DEFAULT    (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
#define LOCALE_NEUTRAL         (MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT))
#define LOCALE_US_ENGLISH      (MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT))
#define LOCALE_INVARIANT       (MAKELCID(MAKELANGID(LANG_INVARIANT, SUBLANG_NEUTRAL), SORT_DEFAULT))

#define SUBLANG_ENGLISH_US               0x01
#define SUBLANG_CHINESE_TRADITIONAL      0x01    /* Chinese (Traditional) */

#define LANG_CHINESE                     0x04
#define LANG_ENGLISH                     0x09
#define LANG_JAPANESE                    0x11
#define LANG_KOREAN                      0x12
#define LANG_THAI                        0x1e

#define LOCALE_SDECIMAL               0x0000000E    /* decimal separator */
#define LOCALE_STHOUSAND              0x0000000F    /* thousand separator */
#define LOCALE_ILZERO                 0x00000012    /* leading zeros for decimal */
#define LOCALE_SCURRENCY              0x00000014    /* local monetary symbol */
#define LOCALE_SMONDECIMALSEP         0x00000016    /* monetary decimal separator */
#define LOCALE_SMONTHOUSANDSEP        0x00000017    /* monetary thousand separator */

#define CT_CTYPE1                 0x00000001  /* ctype 1 information */
#define CT_CTYPE2                 0x00000002  /* ctype 2 information */
#define CT_CTYPE3                 0x00000004  /* ctype 3 information */
#define C1_UPPER                  0x0001      /* upper case */
#define C1_LOWER                  0x0002      /* lower case */
#define C1_DIGIT                  0x0004      /* decimal digits */
#define C1_SPACE                  0x0008      /* spacing characters */
#define C1_PUNCT                  0x0010      /* punctuation characters */
#define C1_CNTRL                  0x0020      /* control characters */
#define C1_BLANK                  0x0040      /* blank characters */
#define C1_XDIGIT                 0x0080      /* other digits */
#define C1_ALPHA                  0x0100      /* any linguistic character */
#define C2_LEFTTORIGHT            0x0001      /* left to right */
#define C2_RIGHTTOLEFT            0x0002      /* right to left */
#define C2_EUROPENUMBER           0x0003      /* European number, digit */
#define C2_EUROPESEPARATOR        0x0004      /* European numeric separator */
#define C2_EUROPETERMINATOR       0x0005      /* European numeric terminator */
#define C2_ARABICNUMBER           0x0006      /* Arabic number */
#define C2_COMMONSEPARATOR        0x0007      /* common numeric separator */
#define C2_BLOCKSEPARATOR         0x0008      /* block separator */
#define C2_SEGMENTSEPARATOR       0x0009      /* segment separator */
#define C2_WHITESPACE             0x000A      /* white space */
#define C2_OTHERNEUTRAL           0x000B      /* other neutrals */
#define C2_NOTAPPLICABLE          0x0000      /* no implicit directionality */
#define C3_NONSPACING             0x0001      /* nonspacing character */
#define C3_DIACRITIC              0x0002      /* diacritic mark */
#define C3_VOWELMARK              0x0004      /* vowel mark */
#define C3_SYMBOL                 0x0008      /* symbols */
#define C3_KATAKANA               0x0010      /* katakana character */
#define C3_HIRAGANA               0x0020      /* hiragana character */
#define C3_HALFWIDTH              0x0040      /* half width character */
#define C3_FULLWIDTH              0x0080      /* full width character */
#define C3_IDEOGRAPH              0x0100      /* ideographic character */
#define C3_KASHIDA                0x0200      /* Arabic kashida character */
#define C3_LEXICAL                0x0400      /* lexical character */
#define C3_ALPHA                  0x8000      /* any ling. char (C1_ALPHA) */
#define C3_NOTAPPLICABLE          0x0000      /* ctype 3 is not applicable */

#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3
#define DLL_PROCESS_DETACH 0

typedef DWORD (PALAPI *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

/******************* PAL-Specific Entrypoints *****************************/

PALIMPORT
int
PALAPI
PAL_Initialize(
            int argc,
            char *argv[]);

PALIMPORT
DWORD_PTR
PALAPI
PAL_EntryPoint(
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter);

PALIMPORT
void
PALAPI
PAL_Terminate(
          void);

PALIMPORT
void
PALAPI
PAL_InitializeDebug(
          void);



PALIMPORT
BOOL
PALAPI
PAL_GetUserConfigurationDirectoryW(
                   OUT LPWSTR lpDirectoryName,
                   IN UINT cbDirectoryName);


#ifdef UNICODE
#define PAL_GetUserConfigurationDirectory PAL_GetUserConfigurationDirectoryW
#endif


PALIMPORT
HMODULE
PALAPI
PAL_RegisterLibraryW(
         IN LPCWSTR lpLibFileName);

PALIMPORT
BOOL
PALAPI
PAL_UnregisterLibraryW(
         IN HMODULE hLibModule);

#ifdef UNICODE
#define PAL_RegisterLibrary PAL_RegisterLibraryW
#define PAL_UnregisterLibrary PAL_UnregisterLibraryW
#endif

PALIMPORT
BOOL
PALAPI
PAL_GetPALDirectoryW(
             OUT LPWSTR lpDirectoryName,
             IN UINT cchDirectoryName);

PALIMPORT
BOOL
PALAPI
PAL_GetPALDirectoryA(
             OUT LPSTR lpDirectoryName,
             IN UINT cchDirectoryName);

#ifdef UNICODE
#define PAL_GetPALDirectory PAL_GetPALDirectoryW
#else
#define PAL_GetPALDirectory PAL_GetPALDirectoryA
#endif

PALIMPORT
BOOL
PALAPI
PAL_Random(
        IN BOOL bStrong,
        IN OUT LPVOID lpBuffer,
        IN DWORD dwLength);

typedef LPVOID (__stdcall *PAL_POPTIMIZEDTLSGETTER)();

PALIMPORT
PAL_POPTIMIZEDTLSGETTER
PALAPI
PAL_MakeOptimizedTlsGetter(
        IN DWORD dwTlsIndex);

PALIMPORT
VOID
PALAPI
PAL_FreeOptimizedTlsGetter(
        IN PAL_POPTIMIZEDTLSGETTER pOptimizedTlsGetter);

#ifdef PLATFORM_UNIX

PALIMPORT
DWORD
PALAPI
PAL_CreateExecWatchpoint(
    HANDLE hThread,
    PVOID pvInstruction
    );

PALIMPORT
DWORD
PALAPI
PAL_DeleteExecWatchpoint(
    HANDLE hThread,
    PVOID pvInstruction
    );

#endif


/******************* winuser.h Entrypoints *******************************/

PALIMPORT
LPSTR
PALAPI
CharNextA(
            IN LPCSTR lpsz);

PALIMPORT
LPSTR
PALAPI
CharNextExA(
        IN WORD CodePage,
        IN LPCSTR lpCurrentChar,
        IN DWORD dwFlags);

#ifndef UNICODE
#define CharNext CharNextA
#define CharNextEx CharNextExA
#endif


PALIMPORT
int
PALAPIV
wsprintfA(
      OUT LPSTR,
      IN LPCSTR,
      ...);

PALIMPORT
int
PALAPIV
wsprintfW(
      OUT LPWSTR,
      IN LPCWSTR,
      ...);

#ifdef UNICODE
#define wsprintf wsprintfW
#else
#define wsprintf wsprintfA
#endif


#define MB_OK                   0x00000000L
#define MB_OKCANCEL             0x00000001L
#define MB_ABORTRETRYIGNORE     0x00000002L
#define MB_YESNO                0x00000004L
#define MB_RETRYCANCEL          0x00000005L

#define MB_ICONHAND             0x00000010L
#define MB_ICONQUESTION         0x00000020L
#define MB_ICONEXCLAMATION      0x00000030L
#define MB_ICONASTERISK         0x00000040L

#define MB_ICONINFORMATION      MB_ICONASTERISK
#define MB_ICONSTOP             MB_ICONHAND
#define MB_ICONERROR            MB_ICONHAND

#define MB_DEFBUTTON1           0x00000000L
#define MB_DEFBUTTON2           0x00000100L
#define MB_DEFBUTTON3           0x00000200L

#define MB_SYSTEMMODAL          0x00001000L
#define MB_TASKMODAL            0x00002000L
#define MB_SETFOREGROUND        0x00010000L
#define MB_TOPMOST              0x00040000L

// Note: this is the NT 4.0 and greater value.
#define MB_SERVICE_NOTIFICATION 0x00200000L

#define MB_TYPEMASK             0x0000000FL
#define MB_ICONMASK             0x000000F0L
#define MB_DEFMASK              0x00000F00L

#define IDOK                    1
#define IDCANCEL                2
#define IDABORT                 3
#define IDRETRY                 4
#define IDIGNORE                5
#define IDYES                   6
#define IDNO                    7

PALIMPORT
int
PALAPI
MessageBoxW(
        IN LPVOID hWnd,  // NOTE: diff from winuser.h
        IN LPCWSTR lpText,
        IN LPCWSTR lpCaption,
        IN UINT uType);

#ifdef UNICODE
#define MessageBox MessageBoxW
#endif

/***************** wincon.h Entrypoints **********************************/

#define CTRL_C_EVENT        0
#define CTRL_BREAK_EVENT    1
#define CTRL_CLOSE_EVENT    2
// 3 is reserved!
// 4 is reserved!
#define CTRL_LOGOFF_EVENT   5
#define CTRL_SHUTDOWN_EVENT 6

typedef
BOOL
(PALAPI *PHANDLER_ROUTINE)(
    DWORD CtrlType
    );

PALIMPORT
BOOL
PALAPI
SetConsoleCtrlHandler(
              IN PHANDLER_ROUTINE HandlerRoutine,
              IN BOOL Add);

PALIMPORT
BOOL
PALAPI
GenerateConsoleCtrlEvent(
    IN DWORD dwCtrlEvent,
    IN DWORD dwProcessGroupId
    );

//end wincon.h Entrypoints

/******************* winbase.h Entrypoints *******************************/

PALIMPORT
BOOL
PALAPI
AreFileApisANSI(
        VOID);

typedef struct _SECURITY_ATTRIBUTES {
            DWORD nLength;
            LPVOID lpSecurityDescriptor;
            BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#define GENERIC_READ               (0x80000000L)
#define GENERIC_WRITE              (0x40000000L)

#define FILE_SHARE_READ            0x00000001
#define FILE_SHARE_WRITE           0x00000002
#define FILE_SHARE_DELETE          0x00000004

#define CREATE_NEW                 1
#define CREATE_ALWAYS              2
#define OPEN_EXISTING              3
#define OPEN_ALWAYS                4
#define TRUNCATE_EXISTING          5

#define FILE_ATTRIBUTE_READONLY                 0x00000001
#define FILE_ATTRIBUTE_HIDDEN                   0x00000002
#define FILE_ATTRIBUTE_SYSTEM                   0x00000004
#define FILE_ATTRIBUTE_DIRECTORY                0x00000010
#define FILE_ATTRIBUTE_ARCHIVE                  0x00000020
#define FILE_ATTRIBUTE_NORMAL                   0x00000080

#define FILE_FLAG_WRITE_THROUGH    0x80000000
#define FILE_FLAG_NO_BUFFERING     0x20000000
#define FILE_FLAG_RANDOM_ACCESS    0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN  0x08000000
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000

#define FILE_BEGIN                 0
#define FILE_CURRENT               1
#define FILE_END                   2

#define STILL_ACTIVE (0x00000103L)

#define INVALID_SET_FILE_POINTER   ((DWORD)-1)

PALIMPORT
HANDLE
PALAPI
CreateFileA(
        IN LPCSTR lpFileName,
        IN DWORD dwDesiredAccess,
        IN DWORD dwShareMode,
        IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        IN DWORD dwCreationDisposition,
        IN DWORD dwFlagsAndAttributes,
        IN HANDLE hTemplateFile);

PALIMPORT
HANDLE
PALAPI
CreateFileW(
        IN LPCWSTR lpFileName,
        IN DWORD dwDesiredAccess,
        IN DWORD dwShareMode,
        IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        IN DWORD dwCreationDisposition,
        IN DWORD dwFlagsAndAttributes,
        IN HANDLE hTemplateFile);

#ifdef UNICODE
#define CreateFile CreateFileW
#else
#define CreateFile CreateFileA
#endif

PALIMPORT
BOOL
PALAPI
LockFile(
    IN HANDLE hFile,
    IN DWORD dwFileOffsetLow,
    IN DWORD dwFileOffsetHigh,
    IN DWORD nNumberOfBytesToLockLow,
    IN DWORD nNumberOfBytesToLockHigh
    );

PALIMPORT
BOOL
PALAPI
UnlockFile(
    IN HANDLE hFile,
    IN DWORD dwFileOffsetLow,
    IN DWORD dwFileOffsetHigh,
    IN DWORD nNumberOfBytesToUnlockLow,
    IN DWORD nNumberOfBytesToUnlockHigh
    );

PALIMPORT
DWORD
PALAPI
SearchPathA(
    IN LPCSTR lpPath,
    IN LPCSTR lpFileName,
    IN LPCSTR lpExtension,
    IN DWORD nBufferLength,
    OUT LPSTR lpBuffer,
    OUT LPSTR *lpFilePart
    );

PALIMPORT
DWORD
PALAPI
SearchPathW(
    IN LPCWSTR lpPath,
    IN LPCWSTR lpFileName,
    IN LPCWSTR lpExtension,
    IN DWORD nBufferLength,
    OUT LPWSTR lpBuffer,
    OUT LPWSTR *lpFilePart
    );
#ifdef UNICODE
#define SearchPath  SearchPathW
#else
#define SearchPath  SearchPathA
#endif // !UNICODE


PALIMPORT
BOOL
PALAPI
CopyFileA(
      IN LPCSTR lpExistingFileName,
      IN LPCSTR lpNewFileName,
      IN BOOL bFailIfExists);

PALIMPORT
BOOL
PALAPI
CopyFileW(
      IN LPCWSTR lpExistingFileName,
      IN LPCWSTR lpNewFileName,
      IN BOOL bFailIfExists);

#ifdef UNICODE
#define CopyFile CopyFileW
#else
#define CopyFile CopyFileA
#endif

PALIMPORT
BOOL
PALAPI
DeleteFileA(
        IN LPCSTR lpFileName);

PALIMPORT
BOOL
PALAPI
DeleteFileW(
        IN LPCWSTR lpFileName);

#ifdef UNICODE
#define DeleteFile DeleteFileW
#else
#define DeleteFile DeleteFileA
#endif


PALIMPORT
BOOL
PALAPI
MoveFileA(
     IN LPCSTR lpExistingFileName,
     IN LPCSTR lpNewFileName);

PALIMPORT
BOOL
PALAPI
MoveFileW(
     IN LPCWSTR lpExistingFileName,
     IN LPCWSTR lpNewFileName);

#ifdef UNICODE
#define MoveFile MoveFileW
#else
#define MoveFile MoveFileA
#endif

#define MOVEFILE_REPLACE_EXISTING      0x00000001
#define MOVEFILE_COPY_ALLOWED          0x00000002

PALIMPORT
BOOL
PALAPI
MoveFileExA(
        IN LPCSTR lpExistingFileName,
        IN LPCSTR lpNewFileName,
        IN DWORD dwFlags);

PALIMPORT
BOOL
PALAPI
MoveFileExW(
        IN LPCWSTR lpExistingFileName,
        IN LPCWSTR lpNewFileName,
        IN DWORD dwFlags);

#ifdef UNICODE
#define MoveFileEx MoveFileExW
#else
#define MoveFileEx MoveFileExA
#endif

PALIMPORT
BOOL
PALAPI
CreateDirectoryA(
         IN LPCSTR lpPathName,
         IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);

PALIMPORT
BOOL
PALAPI
CreateDirectoryW(
         IN LPCWSTR lpPathName,
         IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);

#ifdef UNICODE
#define CreateDirectory CreateDirectoryW
#else
#define CreateDirectory CreateDirectoryA
#endif

PALIMPORT
BOOL
PALAPI
RemoveDirectoryW(
         IN LPCWSTR lpPathName);

PALIMPORT
BOOL
PALAPI
RemoveDirectoryA(
         IN LPCSTR lpPathName);

#ifdef UNICODE
#define RemoveDirectory RemoveDirectoryW
#else
#define RemoveDirectory RemoveDirectoryA
#endif

typedef struct _BY_HANDLE_FILE_INFORMATION {  
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[ MAX_PATH ];
    CHAR cAlternateFileName[ 14 ];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR cFileName[ MAX_PATH ];
    WCHAR cAlternateFileName[ 14 ];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

#ifdef UNICODE
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAW PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAW LPWIN32_FIND_DATA;
#else
typedef WIN32_FIND_DATAA WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAA PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAA LPWIN32_FIND_DATA;
#endif

PALIMPORT
HANDLE
PALAPI
FindFirstFileA(
           IN LPCSTR lpFileName,
           OUT LPWIN32_FIND_DATAA lpFindFileData);

PALIMPORT
HANDLE
PALAPI
FindFirstFileW(
           IN LPCWSTR lpFileName,
           OUT LPWIN32_FIND_DATAW lpFindFileData);

#ifdef UNICODE
#define FindFirstFile FindFirstFileW
#else
#define FindFirstFile FindFirstFileA
#endif

PALIMPORT
BOOL
PALAPI
FindNextFileA(
          IN HANDLE hFindFile,
          OUT LPWIN32_FIND_DATAA lpFindFileData);

PALIMPORT
BOOL
PALAPI
FindNextFileW(
          IN HANDLE hFindFile,
          OUT LPWIN32_FIND_DATAW lpFindFileData);

#ifdef UNICODE
#define FindNextFile FindNextFileW
#else
#define FindNextFile FindNextFileA
#endif

PALIMPORT
BOOL
PALAPI
FindClose(
      IN OUT HANDLE hFindFile);

PALIMPORT
DWORD
PALAPI
GetFileAttributesA(
           IN LPCSTR lpFileName);

PALIMPORT
DWORD
PALAPI
GetFileAttributesW(
           IN LPCWSTR lpFileName);

#ifdef UNICODE
#define GetFileAttributes GetFileAttributesW
#else
#define GetFileAttributes GetFileAttributesA
#endif

typedef enum _GET_FILEEX_INFO_LEVELS {
  GetFileExInfoStandard
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD      dwFileAttributes;
    FILETIME   ftCreationTime;
    FILETIME   ftLastAccessTime;
    FILETIME   ftLastWriteTime;
    DWORD      nFileSizeHigh;
    DWORD      nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

PALIMPORT
BOOL
PALAPI
GetFileAttributesExW(
             IN LPCWSTR lpFileName,
             IN GET_FILEEX_INFO_LEVELS fInfoLevelId,
             OUT LPVOID lpFileInformation);

#ifdef UNICODE
#define GetFileAttributesEx GetFileAttributesExW
#endif

PALIMPORT
BOOL
PALAPI
SetFileAttributesA(
           IN LPCSTR lpFileName,
           IN DWORD dwFileAttributes);

PALIMPORT
BOOL
PALAPI
SetFileAttributesW(
           IN LPCWSTR lpFileName,
           IN DWORD dwFileAttributes);

#ifdef UNICODE
#define SetFileAttributes SetFileAttributesW
#else
#define SetFileAttributes SetFileAttributesA
#endif

typedef LPVOID LPOVERLAPPED;  // diff from winbase.h

PALIMPORT
BOOL
PALAPI
WriteFile(
      IN HANDLE hFile,
      IN LPCVOID lpBuffer,
      IN DWORD nNumberOfBytesToWrite,
      OUT LPDWORD lpNumberOfBytesWritten,
      IN LPOVERLAPPED lpOverlapped);

PALIMPORT
BOOL
PALAPI
ReadFile(
     IN HANDLE hFile,
     OUT LPVOID lpBuffer,
     IN DWORD nNumberOfBytesToRead,
     OUT LPDWORD lpNumberOfBytesRead,
     IN LPOVERLAPPED lpOverlapped);

#define STD_INPUT_HANDLE         ((DWORD)-10)
#define STD_OUTPUT_HANDLE        ((DWORD)-11)
#define STD_ERROR_HANDLE         ((DWORD)-12)

PALIMPORT
HANDLE
PALAPI
GetStdHandle(
         IN DWORD nStdHandle);

PALIMPORT
BOOL
PALAPI
SetEndOfFile(
         IN HANDLE hFile);

PALIMPORT
DWORD
PALAPI
SetFilePointer(
           IN HANDLE hFile,
           IN LONG lDistanceToMove,
           IN PLONG lpDistanceToMoveHigh,
           IN DWORD dwMoveMethod);

PALIMPORT
DWORD
PALAPI
GetFileSize(
        IN HANDLE hFile,
        OUT LPDWORD lpFileSizeHigh);

PALIMPORT
BOOL
PALAPI
GetFileInformationByHandle(
        IN HANDLE hFile,
        OUT BY_HANDLE_FILE_INFORMATION* lpFileInformation);

PALIMPORT
LONG
PALAPI
CompareFileTime(
        IN CONST FILETIME *lpFileTime1,
        IN CONST FILETIME *lpFileTime2);

PALIMPORT
BOOL
PALAPI
SetFileTime(
        IN HANDLE hFile,
        IN CONST FILETIME *lpCreationTime,
        IN CONST FILETIME *lpLastAccessTime,
        IN CONST FILETIME *lpLastWriteTime);

PALIMPORT
BOOL
PALAPI
GetFileTime(
        IN HANDLE hFile,
        OUT LPFILETIME lpCreationTime,
        OUT LPFILETIME lpLastAccessTime,
        OUT LPFILETIME lpLastWriteTime);

PALIMPORT
BOOL
PALAPI
FileTimeToLocalFileTime(
            IN CONST FILETIME *lpFileTime,
            OUT LPFILETIME lpLocalFileTime);

PALIMPORT
BOOL
PALAPI
LocalFileTimeToFileTime(
            IN CONST FILETIME *lpLocalFileTime,
            OUT LPFILETIME lpFileTime);

PALIMPORT
VOID
PALAPI
GetSystemTimeAsFileTime(
            OUT LPFILETIME lpSystemTimeAsFileTime);

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

PALIMPORT
VOID
PALAPI
GetSystemTime(
          OUT LPSYSTEMTIME lpSystemTime);

PALIMPORT
BOOL
PALAPI
FileTimeToDosDateTime(
    IN CONST FILETIME *lpFileTime,
    OUT LPWORD lpFatDate,
    OUT LPWORD lpFatTime
    );

PALIMPORT
BOOL
PALAPI
DosDateTimeToFileTime(
    IN WORD wFatDate,
    IN WORD wFatTime,
    OUT LPFILETIME lpFileTime
    );



PALIMPORT
BOOL
PALAPI
FlushFileBuffers(
         IN HANDLE hFile);

#define FILE_TYPE_UNKNOWN         0x0000
#define FILE_TYPE_DISK            0x0001
#define FILE_TYPE_CHAR            0x0002
#define FILE_TYPE_PIPE            0x0003
#define FILE_TYPE_REMOTE          0x8000

PALIMPORT
DWORD
PALAPI
GetFileType(
        IN HANDLE hFile);

PALIMPORT
UINT
PALAPI
GetConsoleCP(
         VOID);

PALIMPORT
UINT
PALAPI
GetConsoleOutputCP(
           VOID);

PALIMPORT
DWORD
PALAPI
GetFullPathNameA(
         IN LPCSTR lpFileName,
         IN DWORD nBufferLength,
         OUT LPSTR lpBuffer,
         OUT LPSTR *lpFilePart);

PALIMPORT
DWORD
PALAPI
GetFullPathNameW(
         IN LPCWSTR lpFileName,
         IN DWORD nBufferLength,
         OUT LPWSTR lpBuffer,
         OUT LPWSTR *lpFilePart);

#ifdef UNICODE
#define GetFullPathName GetFullPathNameW
#else
#define GetFullPathName GetFullPathNameA
#endif

PALIMPORT
DWORD
PALAPI
GetLongPathNameW(
         IN LPCWSTR lpszShortPath,
                 OUT LPWSTR lpszLongPath,
         IN DWORD cchBuffer);

#ifdef UNICODE
#define GetLongPathName GetLongPathNameW
#endif

PALIMPORT
DWORD
PALAPI
GetShortPathNameW(
         IN LPCWSTR lpszLongPath,
                 OUT LPWSTR lpszShortPath,
         IN DWORD cchBuffer);

#ifdef UNICODE
#define GetShortPathName GetShortPathNameW
#endif


PALIMPORT
UINT
PALAPI
GetTempFileNameA(
         IN LPCSTR lpPathName,
         IN LPCSTR lpPrefixString,
         IN UINT uUnique,
         OUT LPSTR lpTempFileName);

PALIMPORT
UINT
PALAPI
GetTempFileNameW(
         IN LPCWSTR lpPathName,
         IN LPCWSTR lpPrefixString,
         IN UINT uUnique,
         OUT LPWSTR lpTempFileName);

#ifdef UNICODE
#define GetTempFileName GetTempFileNameW
#else
#define GetTempFileName GetTempFileNameA
#endif

PALIMPORT
DWORD
PALAPI
GetTempPathA(
         IN DWORD nBufferLength,
         OUT LPSTR lpBuffer);

PALIMPORT
DWORD
PALAPI
GetTempPathW(
         IN DWORD nBufferLength,
         OUT LPWSTR lpBuffer);

#ifdef UNICODE
#define GetTempPath GetTempPathW
#else
#define GetTempPath GetTempPathA
#endif

PALIMPORT
DWORD
PALAPI
GetCurrentDirectoryA(
             IN DWORD nBufferLength,
             OUT LPSTR lpBuffer);

PALIMPORT
DWORD
PALAPI
GetCurrentDirectoryW(
             IN DWORD nBufferLength,
             OUT LPWSTR lpBuffer);

#ifdef UNICODE
#define GetCurrentDirectory GetCurrentDirectoryW
#else
#define GetCurrentDirectory GetCurrentDirectoryA
#endif

PALIMPORT
BOOL
PALAPI
SetCurrentDirectoryA(
            IN LPCSTR lpPathName);

PALIMPORT
BOOL
PALAPI
SetCurrentDirectoryW(
            IN LPCWSTR lpPathName);


#ifdef UNICODE
#define SetCurrentDirectory SetCurrentDirectoryW
#else
#define SetCurrentDirectory SetCurrentDirectoryA
#endif

// maximum length of the NETBIOS name (not including NULL)
#define MAX_COMPUTERNAME_LENGTH 15

// maximum length of the username (not including NULL)
#define UNLEN   256

PALIMPORT
BOOL
PALAPI
GetUserNameW(
    OUT LPWSTR lpBuffer,      // address of name buffer
    IN OUT LPDWORD nSize );   // address of size of name buffer

PALIMPORT
BOOL
PALAPI
GetComputerNameW(
    OUT LPWSTR lpBuffer,     // address of name buffer
    IN OUT LPDWORD nSize);   // address of size of name buffer

#ifdef UNICODE
#define GetUserName GetUserNameW
#define GetComputerName GetComputerNameW
#endif // UNICODE

PALIMPORT
HANDLE
PALAPI
CreateSemaphoreA(
         IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
         IN LONG lInitialCount,
         IN LONG lMaximumCount,
         IN LPCSTR lpName);

PALIMPORT
HANDLE
PALAPI
CreateSemaphoreW(
         IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
         IN LONG lInitialCount,
         IN LONG lMaximumCount,
         IN LPCWSTR lpName);

#ifdef UNICODE
#define CreateSemaphore CreateSemaphoreW
#else
#define CreateSemaphore CreateSemaphoreA
#endif

PALIMPORT
BOOL
PALAPI
ReleaseSemaphore(
         IN HANDLE hSemaphore,
         IN LONG lReleaseCount,
         OUT LPLONG lpPreviousCount);

PALIMPORT
HANDLE
PALAPI
CreateEventA(
         IN LPSECURITY_ATTRIBUTES lpEventAttributes,
         IN BOOL bManualReset,
         IN BOOL bInitialState,
         IN LPCSTR lpName);

PALIMPORT
HANDLE
PALAPI
CreateEventW(
         IN LPSECURITY_ATTRIBUTES lpEventAttributes,
         IN BOOL bManualReset,
         IN BOOL bInitialState,
         IN LPCWSTR lpName);

#ifdef UNICODE
#define CreateEvent CreateEventW
#else
#define CreateEvent CreateEventA
#endif

PALIMPORT
BOOL
PALAPI
SetEvent(
     IN HANDLE hEvent);

PALIMPORT
BOOL
PALAPI
ResetEvent(
       IN HANDLE hEvent);

PALIMPORT
HANDLE
PALAPI
OpenEventW(
       IN DWORD dwDesiredAccess,
       IN BOOL bInheritHandle,
       IN LPCWSTR lpName);

#ifdef UNICODE
#define OpenEvent OpenEventW
#endif

PALIMPORT
HANDLE
PALAPI
CreateMutexW(
    IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
    IN BOOL bInitialOwner,
    IN LPCWSTR lpName);

PALIMPORT
HANDLE
PALAPI
CreateMutexA(
    IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
    IN BOOL bInitialOwner,
    IN LPCSTR lpName);

#ifdef UNICODE
#define CreateMutex  CreateMutexW
#else
#define CreateMutex  CreateMutexA
#endif


PALIMPORT
BOOL
PALAPI
ReleaseMutex(
    IN HANDLE hMutex);

PALIMPORT
DWORD
PALAPI
GetCurrentProcessId(
            VOID);

PALIMPORT
HANDLE
PALAPI
GetCurrentProcess(
          VOID);

PALIMPORT
DWORD
PALAPI
GetCurrentThreadId(
           VOID);

PALIMPORT
HANDLE
PALAPI
GetCurrentThread(
         VOID);


#define STARTF_USESTDHANDLES       0x00000100

typedef struct _STARTUPINFOW {
    DWORD cb;
    LPWSTR lpReserved_PAL_Undefined;
    LPWSTR lpDesktop_PAL_Undefined;
    LPWSTR lpTitle_PAL_Undefined;
    DWORD dwX_PAL_Undefined;
    DWORD dwY_PAL_Undefined;
    DWORD dwXSize_PAL_Undefined;
    DWORD dwYSize_PAL_Undefined;
    DWORD dwXCountChars_PAL_Undefined;
    DWORD dwYCountChars_PAL_Undefined;
    DWORD dwFillAttribute_PAL_Undefined;
    DWORD dwFlags;
    WORD wShowWindow_PAL_Undefined;
    WORD cbReserved2_PAL_Undefined;
    LPBYTE lpReserved2_PAL_Undefined;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;

typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved_PAL_Undefined;
    LPSTR lpDesktop_PAL_Undefined;
    LPSTR lpTitle_PAL_Undefined;
    DWORD dwX_PAL_Undefined;
    DWORD dwY_PAL_Undefined;
    DWORD dwXSize_PAL_Undefined;
    DWORD dwYSize_PAL_Undefined;
    DWORD dwXCountChars_PAL_Undefined;
    DWORD dwYCountChars_PAL_Undefined;
    DWORD dwFillAttribute_PAL_Undefined;
    DWORD dwFlags;
    WORD wShowWindow_PAL_Undefined;
    WORD cbReserved2_PAL_Undefined;
    LPBYTE lpReserved2_PAL_Undefined;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;

#ifdef UNICODE
typedef STARTUPINFOW STARTUPINFO;
typedef LPSTARTUPINFOW LPSTARTUPINFO;
#else
typedef STARTUPINFOA STARTUPINFO;
typedef LPSTARTUPINFOW LPSTARTUPINFO;
#endif

#define CREATE_NEW_CONSOLE          0x00000010

#define NORMAL_PRIORITY_CLASS             0x00000020

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId_PAL_Undefined;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

PALIMPORT
BOOL
PALAPI
CreateProcessA(
           IN LPCSTR lpApplicationName,
           IN LPSTR lpCommandLine,
           IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
           IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
           IN BOOL bInheritHandles,
           IN DWORD dwCreationFlags,
           IN LPVOID lpEnvironment,
           IN LPCSTR lpCurrentDirectory,
           IN LPSTARTUPINFOA lpStartupInfo,
           OUT LPPROCESS_INFORMATION lpProcessInformation);

PALIMPORT
BOOL
PALAPI
CreateProcessW(
           IN LPCWSTR lpApplicationName,
           IN LPWSTR lpCommandLine,
           IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
           IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
           IN BOOL bInheritHandles,
           IN DWORD dwCreationFlags,
           IN LPVOID lpEnvironment,
           IN LPCWSTR lpCurrentDirectory,
           IN LPSTARTUPINFOW lpStartupInfo,
           OUT LPPROCESS_INFORMATION lpProcessInformation);

#ifdef UNICODE
#define CreateProcess CreateProcessW
#else
#define CreateProcess CreateProcessA
#endif

PALIMPORT
PAL_NORETURN
VOID
PALAPI
ExitProcess(
        IN UINT uExitCode);

PALIMPORT
BOOL
PALAPI
TerminateProcess(
         IN HANDLE hProcess,
         IN UINT uExitCode);

PALIMPORT
BOOL
PALAPI
GetExitCodeProcess(
           IN HANDLE hProcess,
           IN LPDWORD lpExitCode);

PALIMPORT
BOOL
PALAPI
GetProcessTimes(
        IN HANDLE hProcess,
        OUT LPFILETIME lpCreationTime,
        OUT LPFILETIME lpExitTime,
        OUT LPFILETIME lpKernelTime,
        OUT LPFILETIME lpUserTime);

#define MAXIMUM_WAIT_OBJECTS  64
#define WAIT_OBJECT_0 0
#define WAIT_ABANDONED   0x00000080
#define WAIT_ABANDONED_0 0x00000080
#define WAIT_TIMEOUT 258
#define WAIT_FAILED ((DWORD)0xFFFFFFFF)

#define INFINITE 0xFFFFFFFF // Infinite timeout

PALIMPORT
DWORD
PALAPI
WaitForSingleObject(
            IN HANDLE hHandle,
            IN DWORD dwMilliseconds);

PALIMPORT
DWORD
PALAPI
WaitForSingleObjectEx(
            IN HANDLE hHandle,
            IN DWORD dwMilliseconds,
            IN BOOL bAlertable);

PALIMPORT
DWORD
PALAPI
WaitForMultipleObjects(
               IN DWORD nCount,
               IN CONST HANDLE *lpHandles,
               IN BOOL bWaitAll,
               IN DWORD dwMilliseconds);

PALIMPORT
DWORD
PALAPI
WaitForMultipleObjectsEx(
             IN DWORD nCount,
             IN CONST HANDLE *lpHandles,
             IN BOOL bWaitAll,
             IN DWORD dwMilliseconds,
             IN BOOL bAlertable);

PALIMPORT
RHANDLE
PALAPI
PAL_LocalHandleToRemote(
            IN HANDLE hLocal);

PALIMPORT
HANDLE
PALAPI
PAL_RemoteHandleToLocal(
            IN RHANDLE hRemote);


#define DUPLICATE_CLOSE_SOURCE      0x00000001
#define DUPLICATE_SAME_ACCESS       0x00000002

PALIMPORT
BOOL
PALAPI
DuplicateHandle(
        IN HANDLE hSourceProcessHandle,
        IN HANDLE hSourceHandle,
        IN HANDLE hTargetProcessHandle,
        OUT LPHANDLE lpTargetHandle,
        IN DWORD dwDesiredAccess,
        IN BOOL bInheritHandle,
        IN DWORD dwOptions);

PALIMPORT
VOID
PALAPI
Sleep(
      IN DWORD dwMilliseconds);

PALIMPORT
DWORD
PALAPI
SleepEx(
    IN DWORD dwMilliseconds,
    IN BOOL bAlertable);

PALIMPORT
BOOL
PALAPI
SwitchToThread(
    VOID);

#define CREATE_SUSPENDED        0x00000004

PALIMPORT
HANDLE
PALAPI
CreateThread(
         IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
         IN DWORD dwStackSize,
         IN LPTHREAD_START_ROUTINE lpStartAddress,
         IN LPVOID lpParameter,
         IN DWORD dwCreationFlags,
         OUT LPDWORD lpThreadId);

PALIMPORT
PAL_NORETURN
VOID
PALAPI
ExitThread(
       IN DWORD dwExitCode);

PALIMPORT
DWORD
PALAPI
SuspendThread(
          IN HANDLE hThread);

PALIMPORT
DWORD
PALAPI
ResumeThread(
         IN HANDLE hThread);

typedef VOID (PALAPI *PAPCFUNC)(ULONG_PTR dwParam);

PALIMPORT
DWORD
PALAPI
QueueUserAPC(
         IN PAPCFUNC pfnAPC,
         IN HANDLE hThread,
         IN ULONG_PTR dwData);

#ifdef _X86_

#define SIZE_OF_80387_REGISTERS      80

#define CONTEXT_i386            0x00010000
#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L)
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // 387 state
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS)
#define CONTEXT_EXTENDED_REGISTERS  (CONTEXT_i386 | 0x00000020L)

#define MAXIMUM_SUPPORTED_EXTENSION     512

typedef struct _FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT {
    ULONG ContextFlags;

    ULONG   Dr0_PAL_Undefined;
    ULONG   Dr1_PAL_Undefined;
    ULONG   Dr2_PAL_Undefined;
    ULONG   Dr3_PAL_Undefined;
    ULONG   Dr6_PAL_Undefined;
    ULONG   Dr7_PAL_Undefined;

    FLOATING_SAVE_AREA FloatSave;

    ULONG   SegGs_PAL_Undefined;
    ULONG   SegFs_PAL_Undefined;
    ULONG   SegEs_PAL_Undefined;
    ULONG   SegDs_PAL_Undefined;

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

    ULONG   Ebp;
    ULONG   Eip;
    ULONG   SegCs;
    ULONG   EFlags;
    ULONG   Esp;
    ULONG   SegSs;

    UCHAR   ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];

} CONTEXT, *PCONTEXT, *LPCONTEXT;

#elif defined(_PPC_)

#define CONTEXT_CONTROL         0x00000001L
#define CONTEXT_FLOATING_POINT  0x00000002L
#define CONTEXT_INTEGER         0x00000004L

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

typedef struct _CONTEXT {

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_FLOATING_POINT.
    //

    double Fpr0;                        // Floating registers 0..31
    double Fpr1;
    double Fpr2;
    double Fpr3;
    double Fpr4;
    double Fpr5;
    double Fpr6;
    double Fpr7;
    double Fpr8;
    double Fpr9;
    double Fpr10;
    double Fpr11;
    double Fpr12;
    double Fpr13;
    double Fpr14;
    double Fpr15;
    double Fpr16;
    double Fpr17;
    double Fpr18;
    double Fpr19;
    double Fpr20;
    double Fpr21;
    double Fpr22;
    double Fpr23;
    double Fpr24;
    double Fpr25;
    double Fpr26;
    double Fpr27;
    double Fpr28;
    double Fpr29;
    double Fpr30;
    double Fpr31;
    double Fpscr;                       // Floating point status/control reg

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_INTEGER.
    //

    ULONG Gpr0;                         // General registers 0..31
    ULONG Gpr1;                         // StackPointer
    ULONG Gpr2;
    ULONG Gpr3;
    ULONG Gpr4;
    ULONG Gpr5;
    ULONG Gpr6;
    ULONG Gpr7;
    ULONG Gpr8;
    ULONG Gpr9;
    ULONG Gpr10;
    ULONG Gpr11;
    ULONG Gpr12;
    ULONG Gpr13;
    ULONG Gpr14;
    ULONG Gpr15;
    ULONG Gpr16;
    ULONG Gpr17;
    ULONG Gpr18;
    ULONG Gpr19;
    ULONG Gpr20;
    ULONG Gpr21;
    ULONG Gpr22;
    ULONG Gpr23;
    ULONG Gpr24;
    ULONG Gpr25;
    ULONG Gpr26;
    ULONG Gpr27;
    ULONG Gpr28;
    ULONG Gpr29;
    ULONG Gpr30;
    ULONG Gpr31;

    ULONG Cr;                           // Condition register
    ULONG Xer;                          // Fixed point exception register

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_CONTROL.
    //

    ULONG Msr;                          // Machine status register
    ULONG Iar;                          // Instruction address register
    ULONG Lr;                           // Link register
    ULONG Ctr;                          // Count register

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a thread's context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;

    ULONG Fill[3];                      // Pad out to multiple of 16 bytes

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //
    ULONG Dr0;                          // Breakpoint Register 1
    ULONG Dr1;                          // Breakpoint Register 2
    ULONG Dr2;                          // Breakpoint Register 3
    ULONG Dr3;                          // Breakpoint Register 4
    ULONG Dr4;                          // Breakpoint Register 5
    ULONG Dr5;                          // Breakpoint Register 6
    ULONG Dr6;                          // Debug Status Register
    ULONG Dr7;                          // Debug Control Register

} CONTEXT, *PCONTEXT, *LPCONTEXT;

#endif // _IA64_


PALIMPORT
BOOL
PALAPI
GetThreadContext(
         IN HANDLE hThread,
         IN OUT LPCONTEXT lpContext);

PALIMPORT
BOOL
PALAPI
SetThreadContext(
         IN HANDLE hThread,
         IN CONST CONTEXT *lpContext);

#define THREAD_BASE_PRIORITY_LOWRT    15
#define THREAD_BASE_PRIORITY_MAX      2
#define THREAD_BASE_PRIORITY_MIN      (-2)
#define THREAD_BASE_PRIORITY_IDLE     (-15)

#define THREAD_PRIORITY_LOWEST        THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL  (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL        0
#define THREAD_PRIORITY_HIGHEST       THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL  (THREAD_PRIORITY_HIGHEST-1)
#define THREAD_PRIORITY_ERROR_RETURN  (MAXLONG)

#define THREAD_PRIORITY_TIME_CRITICAL THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE          THREAD_BASE_PRIORITY_IDLE

PALIMPORT
int
PALAPI
GetThreadPriority(
          IN HANDLE hThread);

PALIMPORT
BOOL
PALAPI
SetThreadPriority(
          IN HANDLE hThread,
          IN int nPriority);

#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)

PALIMPORT
DWORD
PALAPI
TlsAlloc(
     VOID);

PALIMPORT
LPVOID
PALAPI
TlsGetValue(
        IN DWORD dwTlsIndex);

PALIMPORT
BOOL
PALAPI
TlsSetValue(
        IN DWORD dwTlsIndex,
        IN LPVOID lpTlsValue);

PALIMPORT
BOOL
PALAPI
TlsFree(
    IN DWORD dwTlsIndex);


typedef struct _CRITICAL_SECTION {
    PVOID DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;

} CRITICAL_SECTION, *PCRITICAL_SECTION, *LPCRITICAL_SECTION;

PALIMPORT VOID PALAPI EnterCriticalSection(IN OUT LPCRITICAL_SECTION lpCriticalSection);
PALIMPORT VOID PALAPI LeaveCriticalSection(IN OUT LPCRITICAL_SECTION lpCriticalSection);
PALIMPORT VOID PALAPI InitializeCriticalSection(OUT LPCRITICAL_SECTION lpCriticalSection);
PALIMPORT VOID PALAPI DeleteCriticalSection(IN OUT LPCRITICAL_SECTION lpCriticalSection);
PALIMPORT BOOL PALAPI TryEnterCriticalSection(IN OUT LPCRITICAL_SECTION lpCriticalSection);

#define SEM_FAILCRITICALERRORS          0x0001
#define SEM_NOOPENFILEERRORBOX          0x8000

PALIMPORT
UINT
PALAPI
SetErrorMode(
         IN UINT uMode);

#define PAGE_NOACCESS                   0x01
#define PAGE_READONLY                   0x02
#define PAGE_READWRITE                  0x04
#define PAGE_WRITECOPY                  0x08
#define PAGE_EXECUTE                    0x10
#define PAGE_EXECUTE_READ               0x20
#define PAGE_EXECUTE_READWRITE          0x40
#define PAGE_EXECUTE_WRITECOPY          0x80
#define MEM_COMMIT                      0x1000
#define MEM_RESERVE                     0x2000
#define MEM_DECOMMIT                    0x4000
#define MEM_RELEASE                     0x8000
#define MEM_FREE                        0x10000
#define MEM_PRIVATE                     0x20000
#define MEM_MAPPED                      0x40000
#define MEM_TOP_DOWN                    0x100000

PALIMPORT
HANDLE
PALAPI
CreateFileMappingA(
           IN HANDLE hFile,
           IN LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
           IN DWORD flProtect,
           IN DWORD dwMaximumSizeHigh,
           IN DWORD dwMaximumSizeLow,
           IN LPCSTR lpName);

PALIMPORT
HANDLE
PALAPI
CreateFileMappingW(
           IN HANDLE hFile,
           IN LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
           IN DWORD flProtect,
           IN DWORD dwMaxmimumSizeHigh,
           IN DWORD dwMaximumSizeLow,
           IN LPCWSTR lpName);

#ifdef UNICODE
#define CreateFileMapping CreateFileMappingW
#else
#define CreateFileMapping CreateFileMappingA
#endif

#define SECTION_QUERY   0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_ALL_ACCESS  (SECTION_MAP_READ | SECTION_MAP_WRITE) // diff from winnt.h
#define FILE_MAP_WRITE      SECTION_MAP_WRITE
#define FILE_MAP_READ       SECTION_MAP_READ
#define FILE_MAP_ALL_ACCESS SECTION_ALL_ACCESS
#define FILE_MAP_COPY       SECTION_QUERY

PALIMPORT
HANDLE
PALAPI
OpenFileMappingA(
         IN DWORD dwDesiredAccess,
         IN BOOL bInheritHandle,
         IN LPCSTR lpName);

PALIMPORT
HANDLE
PALAPI
OpenFileMappingW(
         IN DWORD dwDesiredAccess,
         IN BOOL bInheritHandle,
         IN LPCWSTR lpName);

#ifdef UNICODE
#define OpenFileMapping OpenFileMappingW
#else
#define OpenFileMapping OpenFileMappingA
#endif

PALIMPORT
LPVOID
PALAPI
MapViewOfFile(
          IN HANDLE hFileMappingObject,
          IN DWORD dwDesiredAccess,
          IN DWORD dwFileOffsetHigh,
          IN DWORD dwFileOffsetLow,
          IN SIZE_T dwNumberOfBytesToMap);

PALIMPORT
BOOL
PALAPI
UnmapViewOfFile(
        IN LPCVOID lpBaseAddress);

PALIMPORT
HMODULE
PALAPI
LoadLibraryA(
         IN LPCSTR lpLibFileName);

PALIMPORT
HMODULE
PALAPI
LoadLibraryW(
         IN LPCWSTR lpLibFileName);

#ifdef UNICODE
#define LoadLibrary LoadLibraryW
#else
#define LoadLibrary LoadLibraryA
#endif

typedef INT_PTR (PALAPI *FARPROC)();

PALIMPORT
FARPROC
PALAPI
GetProcAddress(
           IN HMODULE hModule,
           IN LPCSTR lpProcName);

PALIMPORT
BOOL
PALAPI
FreeLibrary(
        IN OUT HMODULE hLibModule);

PALIMPORT
PAL_NORETURN
VOID
PALAPI
FreeLibraryAndExitThread(
             IN HMODULE hLibModule,
             IN DWORD dwExitCode);

PALIMPORT
BOOL
PALAPI
DisableThreadLibraryCalls(
    IN HMODULE hLibModule);

PALIMPORT
DWORD
PALAPI
GetModuleFileNameA(
           IN HMODULE hModule,
           OUT LPSTR lpFileName,
           IN DWORD nSize);

PALIMPORT
DWORD
PALAPI
GetModuleFileNameW(
           IN HMODULE hModule,
           OUT LPWSTR lpFileName,
           IN DWORD nSize);

#ifdef UNICODE
#define GetModuleFileName GetModuleFileNameW
#else
#define GetModuleFileName GetModuleFileNameA
#endif

PALIMPORT
LPVOID
PALAPI
VirtualAlloc(
         IN LPVOID lpAddress,
         IN SIZE_T dwSize,
         IN DWORD flAllocationType,
         IN DWORD flProtect);

PALIMPORT
BOOL
PALAPI
VirtualFree(
        IN LPVOID lpAddress,
        IN SIZE_T dwSize,
        IN DWORD dwFreeType);

PALIMPORT
BOOL
PALAPI
VirtualProtect(
           IN LPVOID lpAddress,
           IN SIZE_T dwSize,
           IN DWORD flNewProtect,
           OUT PDWORD lpflOldProtect);

typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase_PAL_Undefined;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

PALIMPORT
SIZE_T
PALAPI
VirtualQuery(
         IN LPCVOID lpAddress,
         OUT PMEMORY_BASIC_INFORMATION lpBuffer,
         IN SIZE_T dwLength);

PALIMPORT
BOOL
PALAPI
ReadProcessMemory(
          IN HANDLE hProcess,
          IN LPCVOID lpBaseAddress,
          OUT LPVOID lpBuffer,
          IN SIZE_T nSize,
          OUT SIZE_T * lpNumberOfBytesRead);

PALIMPORT
VOID
PALAPI
RtlMoveMemory(
          IN PVOID Destination,
          IN CONST VOID *Source,
          IN SIZE_T Length);

#define MoveMemory memmove
#define CopyMemory memcpy
#define FillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

PALIMPORT
HANDLE
PALAPI
GetProcessHeap(
           VOID);

#define HEAP_ZERO_MEMORY 0x00000008

PALIMPORT
LPVOID
PALAPI
HeapAlloc(
      IN HANDLE hHeap,
      IN DWORD dwFlags,
      IN SIZE_T dwBytes);

PALIMPORT
LPVOID
PALAPI
HeapReAlloc(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPVOID lpMem,
    IN SIZE_T dwBytes
    );

PALIMPORT
BOOL
PALAPI
HeapFree(
     IN HANDLE hHeap,
     IN DWORD dwFlags,
     IN LPVOID lpMem);

#define LMEM_FIXED          0x0000
#define LMEM_ZEROINIT       0x0040
#define LPTR                (LMEM_FIXED | LMEM_ZEROINIT)

PALIMPORT
HLOCAL
PALAPI
LocalAlloc(
       IN UINT uFlags,
       IN SIZE_T uBytes);

PALIMPORT
HLOCAL
PALAPI
LocalFree(
      IN HLOCAL hMem);

PALIMPORT
BOOL
PALAPI
FlushInstructionCache(
              IN HANDLE hProcess,
              IN LPCVOID lpBaseAddress,
              IN SIZE_T dwSize);

PALIMPORT
BOOL
PALAPI
GetStringTypeExW(
         IN LCID Locale,
         IN DWORD dwInfoType,
         IN LPCWSTR lpSrcStr,
         IN int cchSrc,
         OUT LPWORD lpCharType);

#ifdef UNICODE
#define GetStringTypeEx GetStringTypeExW
#endif

#define NORM_IGNORECASE           0x00000001  // ignore case
#define NORM_IGNOREWIDTH          0x00020000  // ignore width

PALIMPORT
int
PALAPI
CompareStringA(
    IN LCID     Locale,
    IN DWORD    dwCmpFlags,
    IN LPCSTR   lpString1,
    IN int      cchCount1,
    IN LPCSTR   lpString2,
    IN int      cchCount2);

PALIMPORT
int
PALAPI
CompareStringW(
    IN LCID     Locale,
    IN DWORD    dwCmpFlags,
    IN LPCWSTR  lpString1,
    IN int      cchCount1,
    IN LPCWSTR  lpString2,
    IN int      cchCount2);

#ifdef UNICODE
#define CompareString  CompareStringW
#endif

#define MAX_LEADBYTES         12
#define MAX_DEFAULTCHAR       2

PALIMPORT
UINT
PALAPI
GetACP(void);

typedef struct _cpinfo {
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
} CPINFO, *LPCPINFO;

PALIMPORT
BOOL
PALAPI
GetCPInfo(
      IN UINT CodePage,
      OUT LPCPINFO lpCPInfo);

PALIMPORT
BOOL
PALAPI
IsDBCSLeadByteEx(
         IN UINT CodePage,
         IN BYTE TestChar);

PALIMPORT
BOOL
PALAPI
IsValidCodePage(
        IN UINT CodePage);
        

#define MB_PRECOMPOSED            0x00000001
#define MB_ERR_INVALID_CHARS      0x00000008

PALIMPORT
int
PALAPI
MultiByteToWideChar(
            IN UINT CodePage,
            IN DWORD dwFlags,
            IN LPCSTR lpMultiByteStr,
            IN int cbMultiByte,
            OUT LPWSTR lpWideCharStr,
            IN int cchWideChar);

#define WC_NO_BEST_FIT_CHARS      0x00000400

PALIMPORT
int
PALAPI
WideCharToMultiByte(
            IN UINT CodePage,
            IN DWORD dwFlags,
            IN LPCWSTR lpWideCharStr,
            IN int cchWideChar,
            OUT LPSTR lpMultiByteStr,
            IN int cbMultyByte,
            IN LPCSTR lpDefaultChar,
            OUT LPBOOL lpUsedDefaultChar);

PALIMPORT
LANGID
PALAPI
GetSystemDefaultLangID(
               void);

PALIMPORT
LANGID
PALAPI
GetUserDefaultLangID(
             void);

PALIMPORT
BOOL
PALAPI
SetThreadLocale(
        IN LCID Locale);

PALIMPORT
LCID
PALAPI
GetThreadLocale(
        void);


#define CSTR_LESS_THAN     1
#define CSTR_EQUAL         2
#define CSTR_GREATER_THAN  3

PALIMPORT
int
PALAPI
GetLocaleInfoW(
    IN LCID     Locale,
    IN LCTYPE   LCType,
    OUT LPWSTR  lpLCData,
    IN int      cchData);

#ifdef UNICODE
#define GetLocaleInfo GetLocaleInfoW
#endif

PALIMPORT
LCID
PALAPI
GetUserDefaultLCID(
           void);

#define TIME_ZONE_ID_INVALID ((DWORD)0xFFFFFFFF)
#define TIME_ZONE_ID_UNKNOWN  0
#define TIME_ZONE_ID_STANDARD 1
#define TIME_ZONE_ID_DAYLIGHT 2


typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

PALIMPORT
DWORD
PALAPI
GetTimeZoneInformation(
               OUT LPTIME_ZONE_INFORMATION lpTimeZoneInformation);

#define LCID_INSTALLED            0x00000001  // installed locale ids
#define LCID_SUPPORTED            0x00000002  // supported locale ids

PALIMPORT
BOOL
PALAPI
IsValidLocale(
          IN LCID Locale,
          IN DWORD dwFlags);


typedef DWORD CALID;
typedef DWORD CALTYPE;

#define CAL_ITWODIGITYEARMAX 0x00000030 // two digit year max
#define CAL_RETURN_NUMBER    0x20000000 // return number instead of string

#define CAL_GREGORIAN                 1 // Gregorian (localized) calendar
#define CAL_GREGORIAN_US              2 // Gregorian (U.S.) calendar
#define CAL_JAPAN                     3 // Japanese Emperor Era calendar
#define CAL_TAIWAN                    4 // Taiwan Era calendar
#define CAL_KOREA                     5 // Korean Tangun Era calendar
#define CAL_HIJRI                     6 // Hijri (Arabic Lunar) calendar
#define CAL_THAI                      7 // Thai calendar
#define CAL_HEBREW                    8 // Hebrew (Lunar) calendar
#define CAL_GREGORIAN_ME_FRENCH       9 // Gregorian Middle East French calendar
#define CAL_GREGORIAN_ARABIC         10 // Gregorian Arabic calendar
#define CAL_GREGORIAN_XLIT_ENGLISH   11 // Gregorian Transliterated English calendar
#define CAL_GREGORIAN_XLIT_FRENCH    12 // Gregorian Transliterated French calendar
#define CAL_JULIAN                   13

PALIMPORT
int
PALAPI
GetCalendarInfoW(
         IN LCID Locale,
         IN CALID Calendar,
         IN CALTYPE CalType,
         OUT LPWSTR lpCalData,
         IN int cchData,
         OUT LPDWORD lpValue);

#ifdef UNICODE
#define GetCalendarInfo GetCalendarInfoW
#endif

#define DATE_USE_ALT_CALENDAR 0x00000004

PALIMPORT
int
PALAPI
GetDateFormatW(
           IN LCID Locale,
           IN DWORD dwFlags,
           IN CONST SYSTEMTIME *lpDate,
           IN LPCWSTR lpFormat,
           OUT LPWSTR lpDateStr,
           IN int cchDate);

#ifdef UNICODE
#define GetDateFormat GetDateFormatW
#endif

#define EXCEPTION_NONCONTINUABLE 0x1
#define EXCEPTION_UNWINDING 0x2
#define EXCEPTION_IS_SIGNAL 0x100
#define EXCEPTION_MAXIMUM_PARAMETERS 15

typedef struct _EXCEPTION_RECORD {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    DWORD NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS, *LPEXCEPTION_POINTERS;

//
// A function table entry is generated for each frame function.
//
typedef struct _RUNTIME_FUNCTION {
    DWORD BeginAddress;
    DWORD EndAddress;
    DWORD UnwindData;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;

PALIMPORT
BOOL
PALAPI
WriteProcessMemory(IN HANDLE hProcess,
                   IN LPVOID lpBaseAddress,
                   IN LPVOID lpBuffer,
                   IN SIZE_T nSize,
                   OUT SIZE_T * lpNumberOfBytesWritten);

#define STANDARD_RIGHTS_REQUIRED  (0x000F0000L)
#define SYNCHRONIZE               (0x00100000L)

#define EVENT_MODIFY_STATE        (0x0002)
#define EVENT_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0x3) 

#define MUTANT_QUERY_STATE        (0x0001)
#define MUTANT_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   MUTANT_QUERY_STATE)
#define MUTEX_ALL_ACCESS          MUTANT_ALL_ACCESS

#define SEMAPHORE_MODIFY_STATE    (0x0002)
#define SEMAPHORE_ALL_ACCESS      (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0x3)

#define PROCESS_TERMINATE         (0x0001)  
#define PROCESS_CREATE_THREAD     (0x0002)  
#define PROCESS_SET_SESSIONID     (0x0004)  
#define PROCESS_VM_OPERATION      (0x0008)  
#define PROCESS_VM_READ           (0x0010)  
#define PROCESS_VM_WRITE          (0x0020)  
#define PROCESS_DUP_HANDLE        (0x0040)  
#define PROCESS_CREATE_PROCESS    (0x0080)  
#define PROCESS_SET_QUOTA         (0x0100)  
#define PROCESS_SET_INFORMATION   (0x0200)  
#define PROCESS_QUERY_INFORMATION (0x0400)  
#define PROCESS_SUSPEND_RESUME    (0x0800)  
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFF)

PALIMPORT
HANDLE
PALAPI
OpenProcess(
    IN DWORD dwDesiredAccess, /* PROCESS_DUP_HANDLE or PROCESS_ALL_ACCESS */
    IN BOOL bInheritHandle,
    IN DWORD dwProcessId
    );

PALIMPORT
VOID
PALAPI
OutputDebugStringA(
           IN LPCSTR lpOutputString);

PALIMPORT
VOID
PALAPI
OutputDebugStringW(
           IN LPCWSTR lpOutputStrig);

#ifdef UNICODE
#define OutputDebugString OutputDebugStringW
#else
#define OutputDebugString OutputDebugStringA
#endif

PALIMPORT
VOID
PALAPI
DebugBreak(
       VOID);

PALIMPORT
LPWSTR
PALAPI
lstrcatW(
     IN OUT LPWSTR lpString1,
     IN LPCWSTR lpString2);

#ifdef UNICODE
#define lstrcat lstrcatW
#endif

PALIMPORT
LPWSTR
PALAPI
lstrcpyW(
     OUT LPWSTR lpString1,
     IN LPCWSTR lpString2);

#ifdef UNICODE
#define lstrcpy lstrcpyW
#endif

PALIMPORT
int
PALAPI
lstrlenA(
     IN LPCSTR lpString);

PALIMPORT
int
PALAPI
lstrlenW(
     IN LPCWSTR lpString);

#ifdef UNICODE
#define lstrlen lstrlenW
#else
#define lstrlen lstrlenA
#endif

PALIMPORT
LPWSTR
PALAPI
lstrcpynW(
      OUT LPWSTR lpString1,
      IN LPCWSTR lpString2,
      IN int iMaxLength);

#ifdef UNICODE
#define lstrcpyn lstrcpynW
#endif


PALIMPORT
DWORD
PALAPI
GetEnvironmentVariableA(
            IN LPCSTR lpName,
            OUT LPSTR lpBuffer,
            IN DWORD nSize);

PALIMPORT
DWORD
PALAPI
GetEnvironmentVariableW(
            IN LPCWSTR lpName,
            OUT LPWSTR lpBuffer,
            IN DWORD nSize);

#ifdef UNICODE
#define GetEnvironmentVariable GetEnvironmentVariableW
#else
#define GetEnvironmentVariable GetEnvironmentVariableA
#endif

PALIMPORT
BOOL
PALAPI
SetEnvironmentVariableA(
            IN LPCSTR lpName,
            IN LPCSTR lpValue);

PALIMPORT
BOOL
PALAPI
SetEnvironmentVariableW(
            IN LPCWSTR lpName,
            IN LPCWSTR lpValue);

#ifdef UNICODE
#define SetEnvironmentVariable SetEnvironmentVariableW
#else
#define SetEnvironmentVariable SetEnvironmentVariableA
#endif

PALIMPORT
LPSTR
PALAPI
GetEnvironmentStringsA(
               VOID);

PALIMPORT
LPWSTR
PALAPI
GetEnvironmentStringsW(
               VOID);

#ifdef UNICODE
#define GetEnvironmentStrings GetEnvironmentStringsW
#else
#define GetEnvironmentStrings GetEnvironmentStringsA
#endif

PALIMPORT
BOOL
PALAPI
FreeEnvironmentStringsA(
            IN LPSTR);

PALIMPORT
BOOL
PALAPI
FreeEnvironmentStringsW(
            IN LPWSTR);

#ifdef UNICODE
#define FreeEnvironmentStrings FreeEnvironmentStringsW
#else
#define FreeEnvironmentStrings FreeEnvironmentStringsA
#endif

PALIMPORT
BOOL
PALAPI
CloseHandle(
        IN OUT HANDLE hObject);

PALIMPORT
VOID
PALAPI
RaiseException(
           IN DWORD dwExceptionCode,
           IN DWORD dwExceptionFlags,
           IN DWORD nNumberOfArguments,
           IN CONST ULONG_PTR *lpArguments);

PALIMPORT
DWORD
PALAPI
GetTickCount(
         VOID);

PALIMPORT
BOOL
PALAPI
QueryPerformanceCounter(
    OUT LARGE_INTEGER *lpPerformanceCount
    );

PALIMPORT
BOOL
PALAPI
QueryPerformanceFrequency(
    OUT LARGE_INTEGER *lpFrequency
    );


typedef LONG (PALAPI *PTOP_LEVEL_EXCEPTION_FILTER)(
                           struct _EXCEPTION_POINTERS *ExceptionInfo);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

PALIMPORT
LPTOP_LEVEL_EXCEPTION_FILTER
PALAPI
SetUnhandledExceptionFilter(
                IN LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

PALIMPORT
LONG
PALAPI
InterlockedIncrement(
             IN OUT LONG volatile *lpAddend);

PALIMPORT
LONG
PALAPI
InterlockedDecrement(
             IN OUT LONG volatile *lpAddend);

PALIMPORT
LONG
PALAPI
InterlockedExchange(
            IN OUT LONG volatile *Target,
            IN LONG Value);

PALIMPORT
LONG
PALAPI
InterlockedCompareExchange(
               IN OUT LONG volatile *Destination,
               IN LONG Exchange,
               IN LONG Comperand);


#define InterlockedExchangePointer(Target, Value) \
    ((PVOID)(UINT_PTR)InterlockedExchange((PLONG)(UINT_PTR)(Target), (LONG)(UINT_PTR)(Value)))

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    ((PVOID)(UINT_PTR)InterlockedCompareExchange((PLONG)(UINT_PTR)(Destination), (LONG)(UINT_PTR)(ExChange), (LONG)(UINT_PTR)(Comperand)))

// ROTORTODO -- need to decide whether to support InterlockedExchangeAdd on all platforms

PALIMPORT
VOID
PALAPI
MemoryBarrier(
    VOID);

PALIMPORT
VOID
PALAPI
YieldProcessor(
    VOID);

PALIMPORT
BOOL
PALAPI
IsBadReadPtr(
         IN CONST VOID *lp,
             IN UINT_PTR ucb);

PALIMPORT
BOOL
PALAPI
IsBadWritePtr(
          IN LPVOID lp,
          IN UINT_PTR ucb);

PALIMPORT
BOOL
PALAPI
IsBadCodePtr(
          IN FARPROC lp);

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_STRING     0x00000400
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY  0x00002000

PALIMPORT
DWORD
PALAPI
FormatMessageW(
           IN DWORD dwFlags,
           IN LPCVOID lpSource,
           IN DWORD dwMessageId,
           IN DWORD dwLanguageId,
           OUT LPWSTR lpBffer,
           IN DWORD nSize,
           IN va_list *Arguments);

#ifdef UNICODE
#define FormatMessage FormatMessageW
#endif


PALIMPORT
DWORD
PALAPI
GetLastError(
         VOID);

PALIMPORT
VOID
PALAPI
SetLastError(
         IN DWORD dwErrCode);

PALIMPORT
LPWSTR
PALAPI
GetCommandLineW(
        VOID);

#ifdef UNICODE
#define GetCommandLine GetCommandLineW
#endif


#define VER_PLATFORM_WIN32_WINDOWS        1
#define VER_PLATFORM_WIN32_NT        2
#define VER_PLATFORM_UNIX            10

typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber_PAL_Undefined;
    DWORD dwPlatformId;
    CHAR szCSDVersion_PAL_Undefined[ 128 ];
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;

typedef struct _OSVERSIONINFOW {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber_PAL_Undefined;
    DWORD dwPlatformId;
    WCHAR szCSDVersion_PAL_Undefined[ 128 ];
} OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW;

#ifdef UNICODE
typedef OSVERSIONINFOW OSVERSIONINFO;
typedef POSVERSIONINFOW POSVERSIONINFO;
typedef LPOSVERSIONINFOW LPOSVERSIONINFO;
#else
typedef OSVERSIONINFOA OSVERSIONINFO;
typedef POSVERSIONINFOA POSVERSIONINFO;
typedef LPOSVERSIONINFOA LPOSVERSIONINFO;
#endif

PALIMPORT
BOOL
PALAPI
GetVersionExA(
          IN OUT LPOSVERSIONINFOA lpVersionInformation);

PALIMPORT
BOOL
PALAPI
GetVersionExW(
          IN OUT LPOSVERSIONINFOW lpVersionInformation);

#ifdef UNICODE
#define GetVersionEx GetVersionExW
#else
#define GetVersionEx GetVersionExA
#endif

#define IMAGE_FILE_MACHINE_I386              0x014c

typedef struct _SYSTEM_INFO {
    WORD wProcessorArchitecture_PAL_Undefined;
    WORD wReserved_PAL_Undefined; // NOTE: diff from winbase.h - no obsolete dwOemId union
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask_PAL_Undefined;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType_PAL_Undefined;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel_PAL_Undefined;
    WORD wProcessorRevision_PAL_Undefined;
} SYSTEM_INFO, *LPSYSTEM_INFO;

PALIMPORT
VOID
PALAPI
GetSystemInfo(
          OUT LPSYSTEM_INFO lpSystemInfo);

PALIMPORT
BOOL
PALAPI
GetDiskFreeSpaceW(
          LPCWSTR lpDirectoryName,
          LPDWORD lpSectorsPerCluster,
          LPDWORD lpBytesPerSector,
          LPDWORD lpNumberOfFreeClusters,
          LPDWORD lpTotalNumberOfClusters);

#ifdef UNICODE
#define GetDiskFreeSpace GetDiskFreeSpaceW
#endif

PALIMPORT
BOOL
PALAPI
CreatePipe(
    OUT PHANDLE hReadPipe,
    OUT PHANDLE hWritePipe,
    IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
    IN DWORD nSize
    );



/******************* C Runtime Entrypoints *******************************/

#ifdef PLATFORM_UNIX
/* Some C runtime functions needs to be reimplemented by the PAL.
   To avoid name collisions, those functions have been renamed using
   defines */
#define exit          PAL_exit
#define atexit        PAL_atexit
#define printf        PAL_printf
#define vprintf       PAL_vprintf
#define wprintf       PAL_wprintf
#define sprintf       PAL_sprintf
#define swprintf      PAL_swprintf
#define sscanf        PAL_sscanf
#define wcsspn        PAL_wcsspn
#define wcstod        PAL_wcstod
#define wcstol        PAL_wcstol
#define wcstoul       PAL_wcstoul
#define wcscat        PAL_wcscat
#define wcscpy        PAL_wcscpy
#define wcslen        PAL_wcslen
#define wcsncmp       PAL_wcsncmp
#define wcschr        PAL_wcschr
#define wcsrchr       PAL_wcsrchr
#define swscanf       PAL_swscanf
#define wcspbrk       PAL_wcspbrk
#define wcsstr        PAL_wcsstr
#define wcscmp        PAL_wcscmp
#define wcsncat       PAL_wcsncat
#define wcsncpy       PAL_wcsncpy
#define wcstok        PAL_wcstok
#define wcscspn       PAL_wcscspn
#define iswalpha      PAL_iswalpha
#define iswdigit      PAL_iswdigit
#define iswprint      PAL_iswprint
#define iswspace      PAL_iswspace
#define iswupper      PAL_iswupper
#define iswxdigit     PAL_iswxdigit
#define towlower      PAL_towlower
#define towupper      PAL_towupper
#define vsprintf      PAL_vsprintf
#define vswprintf     PAL_vswprintf
#define realloc       PAL_realloc
#define fopen         PAL_fopen
#define strtok        PAL_strtok
#define strtoul       PAL_strtoul
#define fprintf       PAL_fprintf
#define fwprintf      PAL_fwprintf
#define vfprintf      PAL_vfprintf
#define vfwprintf     PAL_vfwprintf
#define ctime         PAL_ctime
#define localtime     PAL_localtime
#define mktime        PAL_mktime
#define rand          PAL_rand
#define getenv        PAL_getenv
#define fgets         PAL_fgets
#define fgetws        PAL_fgetws
#define fputc         PAL_fputc
#define putchar       PAL_putchar
#define qsort         PAL_qsort
#define bsearch       PAL_bsearch
#define ferror        PAL_ferror
#define fread         PAL_fread
#define fwrite        PAL_fwrite
#define feof          PAL_feof
#define ftell         PAL_ftell
#define fclose        PAL_fclose
#define setbuf        PAL_setbuf
#define fflush        PAL_fflush
#define fputs         PAL_fputs
#define fseek         PAL_fseek
#define fgetpos       PAL_fgetpos
#define fsetpos       PAL_fsetpos
#define getc          PAL_getc
#define fgetc         PAL_getc // not a typo
#define ungetc        PAL_ungetc
#define atol          PAL_atol
#define acos          PAL_acos
#define asin          PAL_asin
#define atan2         PAL_atan2
#define exp           PAL_exp
#define labs          PAL_labs
#define log           PAL_log
#define log10         PAL_log10
#define pow           PAL_pow
#define _close        PAL__close

#endif /* PLATFORM_UNIX */

#ifndef _CONST_RETURN
#ifdef  __cplusplus
#define _CONST_RETURN  const
#define _CRT_CONST_CORRECT_OVERLOADS
#else
#define _CONST_RETURN
#endif
#endif

/* For backwards compatibility */
#define _WConst_return _CONST_RETURN

#define EOF     (-1)

PALIMPORT void * __cdecl memcpy(void *, const void *, size_t);
PALIMPORT int    __cdecl memcmp(const void *, const void *, size_t);
PALIMPORT void * __cdecl memset(void *, int, size_t);
PALIMPORT void * __cdecl memmove(void *, const void *, size_t);
PALIMPORT void * __cdecl memchr(const void *, int, size_t);

PALIMPORT size_t __cdecl strlen(const char *);
PALIMPORT int __cdecl strcmp(const char*, const char *);
PALIMPORT int __cdecl strncmp(const char*, const char *, size_t);
PALIMPORT int __cdecl _stricmp(const char *, const char *);
PALIMPORT int __cdecl _strnicmp(const char *, const char *, size_t);
PALIMPORT char * __cdecl strcat(char *, const char *);
PALIMPORT char * __cdecl strncat(char *, const char *, size_t);
PALIMPORT char * __cdecl strcpy(char *, const char *);
PALIMPORT char * __cdecl strncpy(char *, const char *, size_t);
PALIMPORT char * __cdecl strchr(const char *, int);
PALIMPORT char * __cdecl strrchr(const char *, int);
PALIMPORT char * __cdecl strpbrk(const char *, const char *);
PALIMPORT char * __cdecl strstr(const char *, const char *);
PALIMPORT char * __cdecl strtok(char *, const char *);
PALIMPORT size_t __cdecl strspn(const char *, const char *);
PALIMPORT size_t  __cdecl strcspn(const char *, const char *);
PALIMPORT int __cdecl sprintf(char *, const char *, ...);
PALIMPORT int __cdecl vsprintf(char *, const char *, va_list);
PALIMPORT int __cdecl _snprintf(char *, size_t, const char *, ...);
PALIMPORT int __cdecl _vsnprintf(char *, size_t, const char *, va_list);
PALIMPORT int __cdecl sscanf(const char *, const char *, ...);
PALIMPORT char * __cdecl _strlwr(char *);
PALIMPORT int __cdecl atoi(const char *);
PALIMPORT LONG __cdecl atol(const char *);
PALIMPORT ULONG __cdecl strtoul(const char *, char **, int);
PALIMPORT double __cdecl atof(const char *);
PALIMPORT char * __cdecl _gcvt(double, int, char *);
PALIMPORT char * __cdecl _ecvt(double, int, int *, int *);
PALIMPORT double __cdecl strtod(const char *, char **);
PALIMPORT int __cdecl isprint(int);
PALIMPORT int __cdecl isspace(int);
PALIMPORT int __cdecl isalpha(int);
PALIMPORT int __cdecl isalnum(int);
PALIMPORT int __cdecl isdigit(int);
PALIMPORT int __cdecl isxdigit(int);
PALIMPORT int __cdecl isupper(int);
PALIMPORT int __cdecl islower(int);
PALIMPORT int __cdecl __iscsym(int);
PALIMPORT int __cdecl tolower(int);
PALIMPORT int __cdecl toupper(int);

PALIMPORT size_t __cdecl _mbslen(const unsigned char *);
PALIMPORT unsigned char * __cdecl _mbsinc(const unsigned char *);
PALIMPORT unsigned char * __cdecl _mbsninc(const unsigned char *, size_t);
PALIMPORT unsigned char * __cdecl _mbsdec(const unsigned char *, const unsigned char *);

PALIMPORT size_t __cdecl wcslen(const wchar_t *);
PALIMPORT int __cdecl wcscmp(const wchar_t*, const wchar_t*);
PALIMPORT int __cdecl wcsncmp(const wchar_t *, const wchar_t *, size_t);
PALIMPORT int __cdecl _wcsicmp(const wchar_t *, const wchar_t*);
PALIMPORT int __cdecl _wcsnicmp(const wchar_t *, const wchar_t *, size_t);
PALIMPORT wchar_t * __cdecl wcscat(wchar_t *, const wchar_t *);
PALIMPORT wchar_t * __cdecl wcsncat(wchar_t *, const wchar_t *, size_t);
PALIMPORT wchar_t * __cdecl wcscpy(wchar_t *, const wchar_t *);
PALIMPORT wchar_t * __cdecl wcsncpy(wchar_t *, const wchar_t *, size_t);
PALIMPORT wchar_t * __cdecl wcschr(const wchar_t *, wchar_t);
PALIMPORT wchar_t * __cdecl wcsrchr(const wchar_t *, wchar_t);
PALIMPORT wchar_t _WConst_return * __cdecl wcspbrk(const wchar_t *, const wchar_t *);
PALIMPORT wchar_t _WConst_return * __cdecl wcsstr(const wchar_t *, const wchar_t *);
PALIMPORT wchar_t * __cdecl wcstok(wchar_t *, const wchar_t *);
PALIMPORT size_t __cdecl wcscspn(const wchar_t *, const wchar_t *);
PALIMPORT int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
PALIMPORT int __cdecl vswprintf(wchar_t *, const wchar_t *, va_list);
PALIMPORT int __cdecl _snwprintf(wchar_t *, size_t, const wchar_t *, ...);
PALIMPORT int __cdecl _vsnwprintf(wchar_t *, size_t, const wchar_t *, va_list);
PALIMPORT int __cdecl swscanf(const wchar_t *, const wchar_t *, ...);
PALIMPORT wchar_t * __cdecl _wcslwr(wchar_t *);
PALIMPORT LONG __cdecl wcstol(const wchar_t *, wchar_t **, int);
PALIMPORT ULONG __cdecl wcstoul(const wchar_t *, wchar_t **, int);
PALIMPORT wchar_t * __cdecl _itow(int, wchar_t *, int);
PALIMPORT wchar_t * __cdecl _i64tow(__int64, wchar_t *, int);
PALIMPORT wchar_t * __cdecl _ui64tow(unsigned __int64, wchar_t *, int);
PALIMPORT int __cdecl _wtoi(const wchar_t *);
PALIMPORT size_t __cdecl wcsspn (const wchar_t *, const wchar_t *);
PALIMPORT double __cdecl wcstod(const wchar_t *, wchar_t **);
PALIMPORT int __cdecl iswalpha(wchar_t);
PALIMPORT int __cdecl iswprint(wchar_t);
PALIMPORT int __cdecl iswupper(wchar_t);
PALIMPORT int __cdecl iswspace(wchar_t);
PALIMPORT int __cdecl iswdigit(wchar_t);
PALIMPORT int __cdecl iswxdigit(wchar_t);
PALIMPORT wchar_t __cdecl towlower(wchar_t);
PALIMPORT wchar_t __cdecl towupper(wchar_t);


#ifdef __cplusplus
extern "C++" {
inline wchar_t *wcschr(wchar_t *_S, wchar_t _C)
        {return ((wchar_t *)wcschr((const wchar_t *)_S, _C)); }
inline wchar_t *wcsrchr(wchar_t *_S, wchar_t _C)
        {return ((wchar_t *)wcsrchr((const wchar_t *)_S, _C)); }
inline wchar_t *wcspbrk(wchar_t *_S, const wchar_t *_P)
        {return ((wchar_t *)wcspbrk((const wchar_t *)_S, _P)); }
inline wchar_t *wcsstr(wchar_t *_S, const wchar_t *_P)
        {return ((wchar_t *)wcsstr((const wchar_t *)_S, _P)); }
}
#endif

PALIMPORT unsigned int __cdecl _rotl(unsigned int, int);
PALIMPORT unsigned int __cdecl _rotr(unsigned int, int);
PALIMPORT int __cdecl abs(int);
PALIMPORT LONG __cdecl labs(LONG);

PALIMPORT double __cdecl sqrt(double);
PALIMPORT double __cdecl log(double);
PALIMPORT double __cdecl log10(double);
PALIMPORT double __cdecl exp(double);
PALIMPORT double __cdecl pow(double, double);
PALIMPORT double __cdecl acos(double);
PALIMPORT double __cdecl asin(double);
PALIMPORT double __cdecl atan(double);
PALIMPORT double __cdecl atan2(double,double);
PALIMPORT double __cdecl cos(double);
PALIMPORT double __cdecl sin(double);
PALIMPORT double __cdecl tan(double);
PALIMPORT double __cdecl cosh(double);
PALIMPORT double __cdecl sinh(double);
PALIMPORT double __cdecl tanh(double);
PALIMPORT double __cdecl fmod(double, double);
PALIMPORT double __cdecl floor(double);
PALIMPORT double __cdecl ceil(double);
PALIMPORT double __cdecl fabs(double);
PALIMPORT double __cdecl modf(double, double *);

PALIMPORT int __cdecl _finite(double);
PALIMPORT int __cdecl _isnan(double);
PALIMPORT double __cdecl _copysign(double, double);

#ifdef __cplusplus
extern "C++" {
inline float fabsf(float _X)
{
    return ((float)fabs((double)_X)); }
}
#endif

PALIMPORT void * __cdecl malloc(size_t);
PALIMPORT void   __cdecl free(void *);
PALIMPORT void * __cdecl realloc(void *, size_t);

#if defined(_MSC_VER)
#define alloca _alloca
#elif defined(PLATFORM_UNIX)
#define _alloca alloca
#else
// MingW
#define _alloca __builtin_alloca
#define alloca __builtin_alloca
#endif //_MSC_VER

#if !defined(_MINGW_)
PALIMPORT void * __cdecl _alloca(size_t);
#endif // On _MINGW_ alloca is aliased to __builtin_alloca (see above)

#if defined(__GNUC__) && defined(PLATFORM_UNIX)
// we set -fno-builtin on the command line. This requires that if
// we use alloca, with either have to call __builtin_alloca, or
// ensure that the alloca call doesn't happen in code which is
// modifying the stack (such as "memset (alloca(x), y, z)"

#define alloca  __builtin_alloca
#endif // __GNUC__

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

PALIMPORT PAL_NORETURN void __cdecl exit(int);
int __cdecl atexit(void (__cdecl *function)(void));

PALIMPORT void __cdecl qsort(void *, size_t, size_t, int (__cdecl *)(const void *, const void *));
PALIMPORT void * __cdecl bsearch(const void *, const void *, size_t, size_t,
int (__cdecl *)(const void *, const void *));

PALIMPORT void __cdecl _splitpath(const char *, char *, char *, char *, char *);
PALIMPORT void __cdecl _wsplitpath(const wchar_t *, wchar_t *, wchar_t *, wchar_t *, wchar_t *);
PALIMPORT void __cdecl _makepath(char *, const char *, const char *, const char *, const char *);
PALIMPORT void __cdecl _wmakepath(wchar_t *, const wchar_t *, const wchar_t *, const wchar_t *, const wchar_t *);
PALIMPORT char * __cdecl _fullpath(char *, const char *, size_t);

PALIMPORT void __cdecl _swab(char *, char *, int);

PALIMPORT time_t __cdecl time(time_t *);

struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };

PALIMPORT struct tm * __cdecl localtime(const time_t *);
PALIMPORT time_t __cdecl mktime(struct tm *);
PALIMPORT char * __cdecl ctime(const time_t *);

PALIMPORT int __cdecl _open_osfhandle(INT_PTR, int);
PALIMPORT int __cdecl _close(int);

struct _FILE;
typedef struct _FILE FILE;

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

/* Locale categories */
#define LC_ALL          0
#define LC_COLLATE      1
#define LC_CTYPE        2
#define LC_MONETARY     3
#define LC_NUMERIC      4
#define LC_TIME         5

PALIMPORT int __cdecl fclose(FILE *);
PALIMPORT void __cdecl setbuf(FILE *, char*);
PALIMPORT int __cdecl fflush(FILE *);
PALIMPORT size_t __cdecl fwrite(const void *, size_t, size_t, FILE *);
PALIMPORT size_t __cdecl fread(void *, size_t, size_t, FILE *);
PALIMPORT char * __cdecl fgets(char *, int, FILE *);
PALIMPORT wchar_t * __cdecl fgetws(wchar_t *, int, FILE *);
PALIMPORT int __cdecl fputs(const char *, FILE *);
PALIMPORT int __cdecl fputc(int c, FILE *stream);
PALIMPORT int __cdecl putchar(int c);
PALIMPORT int __cdecl fprintf(FILE *, const char *, ...);
PALIMPORT int __cdecl fwprintf(FILE *, const wchar_t *, ...);
PALIMPORT int __cdecl vfprintf(FILE *, const char *, va_list);
PALIMPORT int __cdecl vfwprintf(FILE *, const wchar_t *, va_list);
PALIMPORT int __cdecl _getw(FILE *);
PALIMPORT int __cdecl _putw(int, FILE *);
PALIMPORT int __cdecl fseek(FILE *, LONG, int);
PALIMPORT int __cdecl fgetpos(FILE *, fpos_t *);
PALIMPORT int __cdecl fsetpos(FILE *, const fpos_t *);
PALIMPORT LONG __cdecl ftell(FILE *);
PALIMPORT int __cdecl feof(FILE *);
PALIMPORT int __cdecl ferror(FILE *);
PALIMPORT FILE * __cdecl fopen(const char *, const char *);
PALIMPORT FILE * __cdecl _fdopen(int, const char *);
PALIMPORT FILE * __cdecl _wfopen(const wchar_t *, const wchar_t *);
PALIMPORT int __cdecl getc(FILE *stream);
PALIMPORT int __cdecl fgetc(FILE *stream);
PALIMPORT int __cdecl ungetc(int c, FILE *stream);

/* Maximum value that can be returned by the rand function. */

#define RAND_MAX 0x7fff

PALIMPORT int    __cdecl rand(void);
PALIMPORT void   __cdecl srand(unsigned int);

PALIMPORT int __cdecl printf(const char *, ...);
PALIMPORT int __cdecl vprintf(const char *, va_list);
PALIMPORT int __cdecl wprintf(const wchar_t*, ...);

#ifdef _MSC_VER
#define PAL_get_caller _MSC_VER
#else
#define PAL_get_caller 0
#endif

PALIMPORT FILE * __cdecl PAL_get_stdout(int caller);
PALIMPORT FILE * __cdecl PAL_get_stdin(int caller);
PALIMPORT FILE * __cdecl PAL_get_stderr(int caller);
PALIMPORT int * __cdecl PAL_errno(int caller);

#define stdout (PAL_get_stdout(PAL_get_caller))
#define stdin  (PAL_get_stdin(PAL_get_caller))
#define stderr (PAL_get_stderr(PAL_get_caller))
#define errno   (*PAL_errno(PAL_get_caller))

PALIMPORT char * __cdecl getenv(const char *);
PALIMPORT int __cdecl _putenv(const char *);

#define ERANGE          34

/******************* Sockets *********************************************/

#ifndef _MSC_VER
/* Some socket functions needs to be reimplemented by the PAL.
   To avoid name collisions, those functions have been renamed using
   defines */
#define getpeername   PAL_getpeername
#define getsockopt    PAL_getsockopt
#define setsockopt    PAL_setsockopt
#define connect       PAL_connect
#define send          PAL_send
#define recv          PAL_recv
#define closesocket   PAL_closesocket
#define accept        PAL_accept
#define listen        PAL_listen
#define bind          PAL_bind
#define shutdown      PAL_shutdown
#define sendto        PAL_sendto
#define recvfrom      PAL_recvfrom
#define getsockname   PAL_getsockname
#define select        PAL_select
#define socket        PAL_socket
#define gethostbyname PAL_gethostbyname
#define gethostbyaddr PAL_gethostbyaddr
#define gethostname   PAL_gethostname
#define inet_addr     PAL_inet_addr
#endif /* _MSC_VER */


#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)

typedef UINT_PTR        SOCKET;
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;

#ifdef _MINGW_
typedef unsigned long   u_long;
#else
typedef unsigned int    u_long;
#endif

struct  hostent {
        char    FAR * h_name;           /* official name of host */
        char    FAR * FAR * h_aliases;  /* alias list */
        short   h_addrtype;             /* host address type */
        short   h_length;               /* length of address */
        char    FAR * FAR * h_addr_list; /* list of addresses */
#define h_addr  h_addr_list[0]          /* address, for backward compat */
};

struct sockaddr {
        u_short sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
};

/*
 * Internet address (old style... should be updated)
 */
struct in_addr {
        union {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
    };

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};

/*
 * Structure used for manipulating linger option.
 */
struct  linger {
        u_short l_onoff;                /* option on/off */
        u_short l_linger;               /* linger time */
};

/*
 * Argument structure for IP_ADD_MEMBERSHIP and IP_DROP_MEMBERSHIP.
 */
struct ip_mreq {
        struct in_addr  imr_multiaddr;  /* IP multicast address of group */
        struct in_addr  imr_interface;  /* local IP address of interface */
};


/* From fx\src\net\system\net\sockets\AddressFamily.cs */

#define AF_UNSPEC       0               /* unspecified */
/*
 * Although  AF_UNSPEC  is  defined for backwards compatibility, using
 * AF_UNSPEC for the "af" parameter when creating a socket is STRONGLY
 * DISCOURAGED.    The  interpretation  of  the  "protocol"  parameter
 * depends  on the actual address family chosen.  As environments grow
 * to  include  more  and  more  address families that use overlapping
 * protocol  values  there  is  more  and  more  chance of choosing an
 * undesired address family when AF_UNSPEC is used.
 */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_IPX          AF_NS           /* IPX protocols: IPX, SPX, etc. */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_VOICEVIEW    18              /* VoiceView */
#define AF_FIREFOX      19              /* Protocols from Firefox */
#define AF_UNKNOWN1     20              /* Somebody is using this! */
#define AF_BAN          21              /* Banyan */
#define AF_ATM          22              /* Native ATM Services */
#define AF_INET6        23              /* Internetwork Version 6 */
#define AF_CLUSTER      24              /* Microsoft Wolfpack */
#define AF_12844        25              /* IEEE 1284.4 WG AF */
#define AF_IRDA         26              /* IrDA */
#define AF_NETDES       28              /* Network Designers OSI & gateway
                                           enabled protocols */

/* From fx\src\Net\System\Net\Sockets\ProtocolType.cs */
#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_ICMP            1               /* control message protocol */
#define IPPROTO_IGMP            2               /* internet group management protocol */
#define IPPROTO_GGP             3               /* gateway^2 (deprecated) */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_PUP             12              /* pup */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_IDP             22              /* xns idp */
#define IPPROTO_ND              77              /* UNOFFICIAL net disk proto */
#define IPPROTO_RAW             255             /* raw IP packet */
#define NSPROTO_IPX      1000
#define NSPROTO_SPX      1256
#define NSPROTO_SPXII    1257

/* From fx\src\Net\System\Net\Sockets\SocketFlags.cs */
#define MSG_OOB         0x1             /* process out-of-band data */
#define MSG_PEEK        0x2             /* peek at incoming message */
#define MSG_DONTROUTE   0x4             /* send without using routing tables */
#define MSG_PARTIAL     0x8000          /* partial send or recv for message xport */

/* From fx\src\net\System\Net\Sockets\SocketOptionName.cs */
/*
 * Option flags per-socket.
 */
#define SO_DEBUG        0x0001          /* turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002          /* socket has had listen() */
#define SO_REUSEADDR    0x0004          /* allow local address reuse */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */
#define SO_DONTROUTE    0x0010          /* just use interface addresses */
#define SO_BROADCAST    0x0020          /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040          /* bypass hardware when possible */
#define SO_LINGER       0x0080          /* linger on close if data present */
#define SO_OOBINLINE    0x0100          /* leave received OOB data in line */

#define SO_DONTLINGER   (int)(~SO_LINGER)
#define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR)) /* disallow local address reuse */

/*
 * Additional options.
 */
#define SO_SNDBUF       0x1001          /* send buffer size */
#define SO_RCVBUF       0x1002          /* receive buffer size */
#define SO_SNDLOWAT     0x1003          /* send low-water mark */
#define SO_RCVLOWAT     0x1004          /* receive low-water mark */
#define SO_SNDTIMEO     0x1005          /* send timeout */
#define SO_RCVTIMEO     0x1006          /* receive timeout */
#define SO_ERROR        0x1007          /* get error status and clear */
#define SO_TYPE         0x1008          /* get socket type */

#define IP_OPTIONS      1 /* set/get IP options */
#define IP_HDRINCL      2 /* header is included with data */
#define IP_TOS          3 /* IP type of service and preced*/
#define IP_TTL          4 /* IP time to live */
#define IP_MULTICAST_IF     9 /* set/get IP multicast i/f  */
#define IP_MULTICAST_TTL       10 /* set/get IP multicast ttl */
#define IP_MULTICAST_LOOP      11 /*set/get IP multicast loopback */
#define IP_ADD_MEMBERSHIP      12 /* add an IP group membership */
#define IP_DROP_MEMBERSHIP     13/* drop an IP group membership */
#define IP_DONTFRAGMENT     14 /* don't fragment IP datagrams */
#define IP_ADD_SOURCE_MEMBERSHIP  15 /* join IP group/source */
#define IP_DROP_SOURCE_MEMBERSHIP 16 /* leave IP group/source */
#define IP_BLOCK_SOURCE           17 /* block IP group/source */
#define IP_UNBLOCK_SOURCE         18 /* unblock IP group/source */
#define IP_PKTINFO                19 /* receive packet information for ipv4*/

/*
 * TCP options.
 */
#define TCP_NODELAY     0x0001
#define TCP_BSDURGENT   0x7000

/* Option to use with [gs]etsockopt at the IPPROTO_UDP level */

#define UDP_NOCHECKSUM  1
#define UDP_CHECKSUM_COVERAGE   20  /* Set/get UDP-Lite checksum coverage */

/* from fx\src\Net\System\Net\Sockets\SocketShutdown.cs */
/*
 * WinSock 2 extension -- manifest constants for shutdown()
 */
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

/* From fx\src\net\System\Net\Sockets\SocketOptionLevel.cs */
/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET      0xffff          /* options for socket level */

/* from fx\src\Net\System\Net\Sockets\SocketType.cs */
/*
 * The  following  may  be used in place of the address family, socket type, or
 * protocol  in  a  call  to WSASocket to indicate that the corresponding value
 * should  be taken from the supplied WSAPROTOCOL_INFO structure instead of the
 * parameter itself.
 */
#define FROM_PROTOCOL_INFO (-1)

/* Other misc socket constants */

/*
 * Maximum queue length specifiable by listen.
 */
#define SOMAXCONN       0x7fffffff
#define INADDR_ANY              (u_long)0x00000000
#define INADDR_BROADCAST        (u_long)0xffffffff
#define INADDR_NONE             0xffffffff
#define SO_MAX_MSG_SIZE   0x2003      /* maximum message size */
#define SO_PROTOCOL_INFOA 0x2004      /* WSAPROTOCOL_INFOA structure */
#define SO_PROTOCOL_INFOW 0x2005      /* WSAPROTOCOL_INFOW structure */
#ifdef UNICODE
#define SO_PROTOCOL_INFO  SO_PROTOCOL_INFOW
#else
#define SO_PROTOCOL_INFO  SO_PROTOCOL_INFOA
#endif /* UNICODE */
#define SO_CONDITIONAL_ACCEPT 0x3002   /* enable true conditional accept: */
                                       /*  connection is not ack-ed to the */
                                       /*  other side until conditional */
                                       /*  function returns CF_ACCEPT */

/*
 * Types
 */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */


PALIMPORT struct hostent FAR * PALAPI gethostbyname(IN const char FAR * name);

PALIMPORT struct hostent FAR * PALAPI gethostbyaddr(
                                              IN const char FAR * addr,
                                              IN int len,
                                              IN int type);

PALIMPORT int PALAPI FAR gethostname (
                            OUT char FAR * name,
                            IN int namelen);

PALIMPORT LONG PALAPI inet_addr (IN const char FAR * cp);

PALIMPORT int PALAPI getpeername (
                            IN SOCKET s,
                            OUT struct sockaddr FAR *name,
                            IN OUT int FAR * namelen);

PALIMPORT int PALAPI getsockopt (
                           IN SOCKET s,
                           IN int level,
                           IN int optname,
                           OUT char FAR * optval,
                           IN OUT int FAR *optlen);

PALIMPORT int PALAPI setsockopt (
                           IN SOCKET s,
                           IN int level,
                           IN int optname,
                           IN const char FAR *optval,
                           IN int optlen);

PALIMPORT int PALAPI connect (
                        IN SOCKET s,
                        IN const struct sockaddr FAR *name,
                        IN int namelen);

#define MSG_OOB         0x1         /* process out-of-band data */
#define MSG_PEEK        0x2         /* peek at incoming message */
#define MSG_DONTROUTE   0x4         /* send without using routing tables */

#define MSG_MAXIOVLEN   16

#define MSG_PARTIAL     0x8000      /* partial send or recv for message xport */

PALIMPORT int PALAPI send (
                     IN SOCKET s,
                     IN const char FAR * buf,
                     IN int len,
                     IN int flags);

PALIMPORT int PALAPI recv (
                     IN SOCKET s,
                     OUT char FAR * buf,
                     IN int len,
                     IN int flags);

PALIMPORT int PALAPI closesocket ( IN SOCKET s);

PALIMPORT SOCKET PALAPI accept (
                          IN SOCKET s,
                          OUT struct sockaddr FAR *addr,
                          IN OUT int FAR *addrlen);

PALIMPORT int PALAPI listen (
                       IN SOCKET s,
                       IN int backlog);

PALIMPORT int PALAPI bind (
                     IN SOCKET s,
                     IN const struct sockaddr FAR *addr,
                     IN int namelen);

PALIMPORT int PALAPI shutdown (
                         IN SOCKET s,
                         IN int how);

PALIMPORT int PALAPI sendto (
                       IN SOCKET s,
                       IN const char FAR * buf,
                       IN int len,
                       IN int flags,
                       IN const struct sockaddr FAR *to,
                       IN int tolen);

PALIMPORT int PALAPI recvfrom (
                         IN SOCKET s,
                         OUT char FAR * buf,
                         IN int len,
                         IN int flags,
                         OUT struct sockaddr FAR *from,
                         IN OUT int FAR * fromlen);

PALIMPORT int PALAPI getsockname (
                            IN SOCKET s,
                            OUT struct sockaddr FAR *name,
                            IN OUT int FAR * namelen);

#define FD_SETSIZE      64
typedef struct fd_set {
        u_int   fd_count;               /* how many are SET? */
        SOCKET  fd_array[FD_SETSIZE];   /* an array of SOCKETs */
} fd_set;

#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count; __i++) { \
        if (((fd_set FAR *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set FAR *)(set))->fd_count) { \
        if (((fd_set FAR *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set FAR *)(set))->fd_array[__i] = (fd); \
            ((fd_set FAR *)(set))->fd_count++; \
        } \
    } \
} while(0)

#define FD_ZERO(set) (((fd_set FAR *)(set))->fd_count=0)


// note: diff from win32
struct timeval {
        int    tv_sec;         /* seconds */
        int    tv_usec;        /* and microseconds */
};
PALIMPORT int PALAPI select (
                        IN int nfds,
                        IN OUT fd_set FAR *readfds,
                        IN OUT fd_set FAR *writefds,
                        IN OUT fd_set FAR *exceptfds,
                        IN const struct timeval FAR *timeout);

PALIMPORT SOCKET PALAPI socket (
                          IN int af,
                          IN int type,
                          IN int protocol);

/*************** Winsock 2 **************************************************/

#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128

/* network events used by WSAEventSelect */
#define FD_READ_BIT      0
#define FD_READ          (1 << FD_READ_BIT)
#define FD_WRITE_BIT     1
#define FD_WRITE         (1 << FD_WRITE_BIT)
#define FD_CONNECT_BIT   4
#define FD_CONNECT       (1 << FD_CONNECT_BIT)
#define FD_ACCEPT_BIT    3
#define FD_ACCEPT        (1 << FD_ACCEPT_BIT)

typedef struct WSAData {
        WORD                    wVersion;
        WORD                    wHighVersion;
        char                    szDescription_PAL_Undefined[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus_PAL_Undefined[WSASYS_STATUS_LEN+1];
        unsigned short          iMaxSockets_PAL_Undefined;
        unsigned short          iMaxUdpDg_PAL_Undefined;
        char FAR *              lpVendorInfo_PAL_Undefined;
} WSADATA, FAR * LPWSADATA;

#define MAX_PROTOCOL_CHAIN 7

typedef struct _WSAPROTOCOLCHAIN {
    int ChainLen;                                 /* the length of the chain,     */
                                                  /* length = 0 means layered protocol, */
                                                  /* length = 1 means base protocol, */
                                                  /* length > 1 means protocol chain */
    DWORD ChainEntries[MAX_PROTOCOL_CHAIN];       /* a list of dwCatalogEntryIds */
} WSAPROTOCOLCHAIN, FAR * LPWSAPROTOCOLCHAIN;

#define WSAPROTOCOL_LEN  255

typedef struct _WSAPROTOCOL_INFOA {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    CHAR   szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOA, FAR * LPWSAPROTOCOL_INFOA;
typedef struct _WSAPROTOCOL_INFOW {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    WCHAR  szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOW, FAR * LPWSAPROTOCOL_INFOW;
#ifdef UNICODE
typedef WSAPROTOCOL_INFOW WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOW LPWSAPROTOCOL_INFO;
#else
typedef WSAPROTOCOL_INFOA WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOA LPWSAPROTOCOL_INFO;
#endif /* UNICODE */

/* Flag bit definitions for dwProviderFlags */
#define PFL_MULTIPLE_PROTO_ENTRIES          0x00000001
#define PFL_RECOMMENDED_PROTO_ENTRY         0x00000002
#define PFL_HIDDEN                          0x00000004
#define PFL_MATCHES_PROTOCOL_ZERO           0x00000008

/* Flag bit definitions for dwServiceFlags1 */
#define XP1_CONNECTIONLESS                  0x00000001
#define XP1_GUARANTEED_DELIVERY             0x00000002
#define XP1_GUARANTEED_ORDER                0x00000004
#define XP1_MESSAGE_ORIENTED                0x00000008
#define XP1_PSEUDO_STREAM                   0x00000010
#define XP1_GRACEFUL_CLOSE                  0x00000020
#define XP1_EXPEDITED_DATA                  0x00000040
#define XP1_CONNECT_DATA                    0x00000080
#define XP1_DISCONNECT_DATA                 0x00000100
#define XP1_SUPPORT_BROADCAST               0x00000200
#define XP1_SUPPORT_MULTIPOINT              0x00000400
#define XP1_MULTIPOINT_CONTROL_PLANE        0x00000800
#define XP1_MULTIPOINT_DATA_PLANE           0x00001000
#define XP1_QOS_SUPPORTED                   0x00002000
#define XP1_INTERRUPT                       0x00004000
#define XP1_UNI_SEND                        0x00008000
#define XP1_UNI_RECV                        0x00010000
#define XP1_IFS_HANDLES                     0x00020000
#define XP1_PARTIAL_MESSAGE                 0x00040000

#define SECURITY_PROTOCOL_NONE              0x0000


PALIMPORT
int
PALAPI
WSAStartup(
       IN WORD wVersionRequired,
       OUT LPWSADATA lpWSAData);

PALIMPORT
int
PALAPI
WSACleanup(
       void);

typedef struct _WSABUF {
    u_long      len;     /* the length of the buffer */
    char FAR *  buf;     /* the pointer to the buffer */
} WSABUF, FAR * LPWSABUF;

typedef struct _WSAOVERLAPPED
{
    DWORD  Internal;
    DWORD  InternalHigh;
    DWORD  Offset;
    DWORD  OffsetHigh;
    HANDLE hEvent;
} WSAOVERLAPPED, *LPWSAOVERLAPPED;

typedef DWORD_PTR LPWSAOVERLAPPED_COMPLETION_ROUTINE;  // note: diff from Win32, to catch inadvertant use

typedef ULONG   SERVICETYPE;
 
typedef struct _flowspec
{
    ULONG       TokenRate;              /* In Bytes/sec */
    ULONG       TokenBucketSize;        /* In Bytes */
    ULONG       PeakBandwidth;          /* In Bytes/sec */
    ULONG       Latency;                /* In microseconds */
    ULONG       DelayVariation;         /* In microseconds */
    SERVICETYPE ServiceType;
    ULONG       MaxSduSize;             /* In Bytes */
    ULONG       MinimumPolicedSize;     /* In Bytes */

} FLOWSPEC, *PFLOWSPEC, * LPFLOWSPEC;

typedef struct _QualityOfService
{
    FLOWSPEC      SendingFlowspec;       /* the flow spec for data sending */
    FLOWSPEC      ReceivingFlowspec;     /* the flow spec for data receiving */
    WSABUF        ProviderSpecific;      /* additional provider specific stuff */
} QOS, FAR * LPQOS;

PALIMPORT
int 
PALAPI
WSAConnect(
           IN SOCKET s,
           IN const struct sockaddr* name,
           IN int namelen,
           IN LPWSABUF lpCallerData,
           OUT LPWSABUF lpCalleeData,
           IN LPQOS lpSQOS,
           IN LPQOS lpGQOS);

PALIMPORT
int
PALAPI
WSASend(
    IN SOCKET s,
    IN LPWSABUF lpBuffers,
    IN DWORD dwBufferCount,
    OUT LPDWORD lpNumberOfBytesSent,
    IN DWORD dwFlags,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

PALIMPORT
int
PALAPI
WSASendTo(
      IN SOCKET s,
      IN LPWSABUF lpBuffers,
      IN DWORD dwBufferCount,
      OUT LPDWORD lpNumberOfBytesSent,
      IN DWORD dwFlags,
      IN const struct sockaddr FAR *lpTo,
      IN int iTolen,
      IN LPWSAOVERLAPPED lpOverlapped,
      IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

PALIMPORT
int
PALAPI
WSARecv(
    IN SOCKET s,
    IN OUT LPWSABUF lpBuffers,
    IN DWORD dwBufferCount,
    OUT LPDWORD lpNumberOfBytesRecvd,
    IN OUT LPDWORD lpFlags,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

PALIMPORT
int
PALAPI
WSARecvFrom(
        IN SOCKET s,
        IN OUT LPWSABUF lpBuffers,
        IN DWORD dwBufferCount,
        OUT LPDWORD lpNumberOfBytesRecvd,
        IN OUT LPDWORD lpFlags,
        OUT struct sockaddr FAR *lpFrom,
        IN OUT LPINT lpFromLen,
        IN LPWSAOVERLAPPED lpOverlapped,
        IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef HANDLE WSAEVENT;

PALIMPORT
int
PALAPI
WSAEventSelect(
           IN SOCKET s,
           IN WSAEVENT hEventObject,
           IN int lNetworkEvents);  // note:  diff from win32

typedef unsigned int GROUP;
#define WSA_FLAG_OVERLAPPED      0x01

#define FD_MAX_EVENTS            10
typedef struct _WSANETWORKEVENTS
{
    int lNetworkEvents;
    int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

PALIMPORT
int
PALAPI
WSAEnumNetworkEvents(
          IN SOCKET s,
          IN WSAEVENT hEventObject,
          OUT LPWSANETWORKEVENTS lpNetworkEvents);  

PALIMPORT 
SOCKET 
PALAPI 
WSASocketA(
    IN int af,
    IN int type,
    IN int protocol,
    IN LPWSAPROTOCOL_INFOA lpProtocolInfo,
    IN GROUP g,
    IN DWORD dwFlags);

PALIMPORT 
SOCKET 
PALAPI 
WSASocketW(
    IN int af,
    IN int type,
    IN int protocol,
    IN LPWSAPROTOCOL_INFOW lpProtocolInfo,
    IN GROUP g,
    IN DWORD dwFlags);

#ifdef UNICODE
#define WSASocket  WSASocketW
#else
#define WSASocket  WSASocketA
#endif /* UNICODE */

PALIMPORT
int
PALAPI
WSAIoctl(
     IN SOCKET s,
     IN DWORD dwIoControlCode,
     IN LPVOID lpvInBuffer,
     IN DWORD cbInBuffer,
     OUT LPVOID lpvOutBuffer,
     IN DWORD cbOutBuffer,
     OUT LPDWORD lpcbBytesReturned,
     IN LPWSAOVERLAPPED lpOverlapped,
     IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);


#define IOCPARM_MASK    0x7f            /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000      /* no parameters */
#define IOC_OUT         0x40000000      /* copy out parameters */
#define IOC_IN          0x80000000      /* copy in parameters */
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define IOC_UNIX                      0x00000000
#define IOC_WS2                       0x08000000
#define IOC_PROTOCOL                  0x10000000
#define IOC_VENDOR                    0x18000000

/* BSD Ioctls */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))
#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define FIONREAD    _IOR('f', 127, u_long) /* get # bytes to read */
#define FIONBIO     _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
#define FIOASYNC    _IOW('f', 125, u_long) /* set/clear async i/o */

/* Winsock 2 specific Ioctls */
#define _WSAIO(x,y)                   (IOC_VOID|(x)|(y))
#define _WSAIOR(x,y)                  (IOC_OUT|(x)|(y))
#define _WSAIOW(x,y)                  (IOC_IN|(x)|(y))
#define _WSAIORW(x,y)                 (IOC_INOUT|(x)|(y))

#define SIO_GET_BROADCAST_ADDRESS     _WSAIOR(IOC_WS2,5)
#define SIO_MULTIPOINT_LOOPBACK       _WSAIOW(IOC_WS2,9)
#define SIO_MULTICAST_SCOPE           _WSAIOW(IOC_WS2,10)
#define SIO_FLUSH                     _WSAIO(IOC_WS2,4)
#define SIO_RCVALL            _WSAIOW(IOC_VENDOR,1)
#define SIO_RCVALL_MCAST      _WSAIOW(IOC_VENDOR,2)
#define SIO_RCVALL_IGMPMCAST  _WSAIOW(IOC_VENDOR,3)
#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)


/* structure for SIO_KEEPALIVE_VALS Ioctl */
struct tcp_keepalive
{
    u_long onoff;
    u_long keepalivetime;
    u_long keepaliveinterval;
};


PALIMPORT
int
PALAPI
ioctlsocket(
        IN SOCKET s,
        IN int cmd,  // note: diff from Win32
        IN OUT u_long FAR *argp);

PALIMPORT
BOOL
PALAPI
WSAGetOverlappedResult(
    IN SOCKET s,
    IN LPWSAOVERLAPPED lpOverlapped,
    OUT LPDWORD lpcbTransfer,
    IN BOOL fWait,
    OUT LPDWORD lpdwFlags
);

PALIMPORT
HANDLE
PALAPI
PAL_CreateIoCompletionPort(
    IN HANDLE FileHandle,
    IN HANDLE ExistingCompletionPort,
    IN ULONG_PTR CompletionKey,
    IN DWORD NumberOfConcurrentThreads);

PALIMPORT
BOOL
PALAPI
PostQueuedCompletionStatus(
    IN HANDLE CompletionPort,
    IN DWORD dwNumberOfBytesTransferred,
    IN ULONG_PTR dwCompletionKey,
    IN LPOVERLAPPED lpOverlapped);

PALIMPORT
BOOL
PALAPI
GetQueuedCompletionStatus(
    IN HANDLE CompletionPort,
    OUT LPDWORD lpNumberOfBytesTransferred,
    OUT PULONG_PTR lpCompletionKey,
    OUT LPOVERLAPPED *lpOverlapped,
    IN DWORD dwMilliseconds);


/******************* PAL-specific I/O completion port *****************/
PALIMPORT
BOOL
PALAPI
PAL_GetIOCPThreadIoPendingFlag(VOID);

typedef struct _PAL_IOCP_CPU_INFORMATION {
    union {
        FILETIME ftLastRecordedIdleTime;
        FILETIME ftLastRecordedCurrentTime;
    } LastRecordedTime;
    FILETIME ftLastRecordedKernelTime;
    FILETIME ftLastRecordedUserTime;
} PAL_IOCP_CPU_INFORMATION;

PALIMPORT
INT
PALAPI
PAL_GetCPUBusyTime(
    IN OUT PAL_IOCP_CPU_INFORMATION *lpPrevCPUInfo);

/****************PAL Perf functions for PInvoke*********************/

/******************* PAL-specific Exception Handling  ************************/


// There is a check elsewhere that wants to ensure that PAL_TRY_LOCAL_SIZE is >= sizeof(sigjmp_buf)
#ifdef _X86_
#if defined(_WIN32)
#define PAL_TRY_LOCAL_SIZE  0x40
#else   // Non-Win32 x86
#define PAL_TRY_LOCAL_SIZE  0x9c
#endif
#elif defined(_PPC_)
#define PAL_TRY_LOCAL_SIZE  0x304   // Includes the Altivec registers
#else
#error  PAL_TRY_LOCAL_SIZE is not defined for this architecture
#endif

// frame is the exception filter which handled the exception
#define PAL_EXCEPTION_FLAGS_UNWINDTARGET    0x00000001
#define PAL_EXCEPTION_FLAGS_UNWINDONLY      0x00000002
#define PAL_EXCEPTION_FLAGS_CPPEHUNWIND     0x00000004
#define PAL_EXCEPTION_FLAGS_LONGJMP         0x00000008
#define PAL_EXCEPTION_FLAGS_FILTERCALLED    0x00000010

// type of an exception filter (an expression on Win32, but a function
// on the PAL).  On non-Win32 platforms, the CONTEXT pointer in the
// PEXCEPTION_POINTERS will contain at least the CONTEXT_CONTROL registers.
typedef LONG (*PFN_PAL_EXCEPTION_FILTER)(
    IN PEXCEPTION_POINTERS pExceptionPointers,
    IN LPVOID lpvParam);

typedef enum _TypeOfHandler {
    PALExceptFilter,
    PALExcept,
    PALFinally,
    CatchTyped,
    CatchAll
} TypeOfHandler;

typedef struct _PAL_EXCEPTION_REGISTRATION {
    // pointer to next exception record up the stack
    // the name is intentinally same as in Win32 EXCEPTION_REGISTRATION_RECORD
    struct _PAL_EXCEPTION_REGISTRATION *Next;

    // pointer to the exception filter expression (now packaged in a
    // function by hand)
    // the name is intentinally same as in Win32 EXCEPTION_REGISTRATION_RECORD
    PFN_PAL_EXCEPTION_FILTER Handler;

    // parameter to pass verbatim to the filter function
    PVOID pvFilterParameter;

    // PAL_EXCEPTION_FLAGS_* constants
    DWORD dwFlags;

    // type of exception handler block
    TypeOfHandler typeOfHandler;

    // reserved for the PAL (typically a CRT jmp_buf or sigjmp_buf struct)
    char ReservedForPAL[PAL_TRY_LOCAL_SIZE];
} PAL_EXCEPTION_REGISTRATION, *PPAL_EXCEPTION_REGISTRATION;

#ifdef _MSC_VER
int __cdecl _setjmp(void*);
#define PAL_setjmp(buff) _setjmp(buff)
#else // _MSC_VER
#ifdef PLATFORM_UNIX
int sigsetjmp(LPVOID, int);
void siglongjmp(void*,int);
#define PAL_longjmp(buff,val) siglongjmp(buff,val)
#define PAL_setjmp(buff) sigsetjmp(buff,1)
#else // PLATFORM_UNIX
#ifndef PAL_IMPLEMENTATION
int     _setjmp(void*);
void longjmp(void *, int);
#endif
#define PAL_setjmp(buff) _setjmp(buff)
#define PAL_longjmp(buff,val) longjmp(buff,val)
#endif // PLATFORM_UNIX
#endif // _MSC_VER

PALIMPORT
void
PALAPI
PAL_TryHelper(
        IN OUT PPAL_EXCEPTION_REGISTRATION pRegistration);

PALIMPORT
int
PALAPI
PAL_EndTryHelper(
        IN OUT PPAL_EXCEPTION_REGISTRATION pRegistration,
        IN int ExceptionCode);

PALIMPORT
PPAL_EXCEPTION_REGISTRATION
PALAPI
PAL_GetBottommostRegistration(
              VOID);

PALIMPORT
VOID
PALAPI
PAL_SetBottommostRegistration(
              PPAL_EXCEPTION_REGISTRATION pRegistration);

PALIMPORT
PPAL_EXCEPTION_REGISTRATION *
PALAPI
PAL_GetBottommostRegistrationPtr(
              VOID);

typedef enum _PAL_SEH_DISPATCHER_STATE {
    DoneWithSEH,
    SetUpForSEH,
    InTryBlock,
    InExceptFinallyBlock
} PAL_SEH_DISPATCHER_STATE;

#if DISABLE_EXCEPTIONS
/* Use this if exceptions aren't enabled on the new platform. */
#define PAL_TRY_NAKED     if (1) {
#define PAL_EXCEPT_FINALLY_COMMON(a, b) } else {
#define PAL_EXCEPT_FILTER(b, c)                                            \
    PAL_EXCEPT_FINALLY_COMMON(b, c)
#define PAL_EXCEPT(b)                                                      \
    PAL_EXCEPT_FINALLY_COMMON(b, NULL)
#define PAL_FINALLY                                                        \
    } {
#define PAL_ENDTRY_NAKED  }

#else   /* DISABLE_EXCEPTIONS */

#define PAL_TRY_NAKED {                                                    \
    PAL_SEH_DISPATCHER_STATE PalSEHDispatcherState = SetUpForSEH;          \
    PAL_EXCEPTION_REGISTRATION PalSEHRecord;                               \
    PalSEHRecord.Next = NULL;                                              \
    PalSEHRecord.dwFlags = 0;                                              \
    PalSEHRecord.typeOfHandler = (TypeOfHandler)0;                         \
    PEXCEPTION_RECORD PalCaughtRecord = 0;                                 \
                                                                           \
    do {                                                                   \
        PAL_GOOD(try) {                                                        \
            do {                                                           \
                if (PalSEHDispatcherState == InTryBlock) {

#define PAL_EXCEPT_FINALLY_COMMON(__pfnFilter, __pvFilterParameter, type)  \
                    break; /* break out of the while loop */               \
                } else if (PalSEHDispatcherState == SetUpForSEH) {         \
                    PalSEHRecord.Handler = (__pfnFilter);                  \
                    PalSEHRecord.pvFilterParameter = (__pvFilterParameter);\
                    PalSEHRecord.typeOfHandler = type;                     \
                    PAL_TryHelper(&PalSEHRecord);                          \
                    PalSEHDispatcherState = InTryBlock;                    \
                } /* if (PalSEHDispatcherState...) */                      \
            } while(1);                                                    \
        } PAL_GOOD(catch) (PEXCEPTION_RECORD record) {                         \
            PalCaughtRecord = record;                                      \
            if (PalSEHRecord.dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET)   \
            { /* run the handler block */

#define PAL_EXCEPT_FILTER(__fnFilter, __pvFilterParameter)                 \
            PAL_EXCEPT_FINALLY_COMMON(&(__fnFilter), (__pvFilterParameter), PALExceptFilter)

#define PAL_EXCEPT(Disposition)                                            \
            PAL_EXCEPT_FINALLY_COMMON((PFN_PAL_EXCEPTION_FILTER)(Disposition), NULL, PALExcept)

#define PAL_FINALLY                                                        \
            PAL_EXCEPT_FINALLY_COMMON(NULL, NULL, PALFinally);             \
            } /* if ... */                                                 \
        } /* catch ... */                                                  \
        { /* Proxy for catch */                                            \
            { /* Proxy for if */                                           \
                /* Always run the PAL_FINALLY handler block */

#define PAL_ENDTRY_NAKED                                                   \
            } /* if ... */                                                 \
            if (!(PalSEHRecord.dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET))\
                if (PalCaughtRecord != NULL)                               \
                    PAL_GOOD(throw) PalCaughtRecord;                           \
        } /* catch (...) */                                                \
        if (PalCaughtRecord == NULL)                                       \
            PalSEHDispatcherState = (PAL_SEH_DISPATCHER_STATE)             \
                    PAL_EndTryHelper(&PalSEHRecord, 0);                    \
        else PalSEHDispatcherState = DoneWithSEH;                          \
    } while (PalSEHDispatcherState != DoneWithSEH); }

#define PAL_CPP_EXCEPTION_CODE 0x6e447575

PALIMPORT
void 
PALAPI
PAL_DoLastMomentUnwind(IN PVOID target);

#define PAL_CPP_THROW(type, expr)                                           \
    {                                                                       \
        ULONG_PTR lpArguments[2];                                           \
        lpArguments[0] = (ULONG_PTR)#type;                                  \
        lpArguments[1] = (ULONG_PTR)expr;                                   \
        RaiseException(PAL_CPP_EXCEPTION_CODE, EXCEPTION_NONCONTINUABLE,    \
                       2, lpArguments);                                     \
        /* RaiseException never returns, but the compiler doesn't know ... */\
        for (;;);                                                           \
    }

#define PAL_CPP_RETHROW                                                     \
    /* An argument-less c++ throw only occurs inside a catch */             \
    /* So we have the right record */                                       \
    RaiseException(PalCppCaughtRecord->ExceptionCode,                       \
                       PalCppCaughtRecord->ExceptionFlags,                  \
                       PalCppCaughtRecord->NumberParameters,                \
                       PalCppCaughtRecord->ExceptionInformation);           \
    /* RaiseException never returns, but the compiler doesn't know ... */   \
    for (;;);

#define PAL_CPP_TRY {                                                      \
    PAL_SEH_DISPATCHER_STATE PalCppSEHDispatcherState = SetUpForSEH;       \
    PAL_EXCEPTION_REGISTRATION PalCppSEHRecord;                            \
    PalCppSEHRecord.Next = NULL;                                           \
    PalCppSEHRecord.dwFlags = 0;                                           \
    PalCppSEHRecord.typeOfHandler = (TypeOfHandler)0;                      \
    PEXCEPTION_RECORD PalCppCaughtRecord = 0;                              \
    /* Since we don't use setjmp on PPC, this comment is irrelevant */     \
    /* However, it may become relevant in case we use non-gcc compilers */ \
    /*                        , hence leaving it in. */                                     \
    /* alloca does not mix with setjmp on PPC - catch these cases */       \
    /* @ROTORTODO: Disabled this check for now */                          \
    /* int alloca; alloca = 0; */                                          \
                                                                           \
    do {                                                                   \
        PAL_GOOD(try) {                                                        \
            do {                                                           \
                if (PalCppSEHDispatcherState == InTryBlock) {

#define PAL_CPP_CATCH_EXCEPTION(ident)                                     \
                    break; /* break out of the while loop */               \
                } else if (PalCppSEHDispatcherState == SetUpForSEH) {      \
                    PalCppSEHRecord.Handler = NULL;                        \
                    PalCppSEHRecord.pvFilterParameter = NULL;              \
                    PalCppSEHRecord.typeOfHandler = CatchTyped;            \
                    PAL_TryHelper(&PalCppSEHRecord);                       \
                    PalCppSEHDispatcherState = InTryBlock;                 \
                } /* if (PalCppSEHDispatcherState...) */                   \
            } while(1);                                                    \
        } PAL_GOOD(catch) (PEXCEPTION_RECORD record) {                         \
            PalCppCaughtRecord = record;                                   \
            if (PalCppSEHRecord.dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET)\
            { /* run the handler block */                                  \
                Exception *ident =                                         \
                    (Exception *) record->ExceptionInformation[1];

// This is really unfortunate. This macro is identical to the above
// but compiler is complaining about unused variable ident.
//
#define PAL_CPP_CATCH_EXCEPTION_NOARG                                     \
                    break; /* break out of the while loop */               \
                } else if (PalCppSEHDispatcherState == SetUpForSEH) {      \
                    PalCppSEHRecord.Handler = NULL;                        \
                    PalCppSEHRecord.pvFilterParameter = NULL;              \
                    PalCppSEHRecord.typeOfHandler = CatchTyped;            \
                    PAL_TryHelper(&PalCppSEHRecord);                       \
                    PalCppSEHDispatcherState = InTryBlock;                 \
                } /* if (PalCppSEHDispatcherState...) */                   \
            } while(1);                                                    \
        } PAL_GOOD(catch) (PEXCEPTION_RECORD record) {                         \
            PalCppCaughtRecord = record;                                   \
            if (PalCppSEHRecord.dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET)\
            { 

#define PAL_CPP_CATCH_ALL                                                  \
                    break; /* break out of the while loop */               \
                } else if (PalCppSEHDispatcherState == SetUpForSEH) {      \
                    PalCppSEHRecord.Handler = NULL;                        \
                    PalCppSEHRecord.pvFilterParameter = NULL;              \
                    PalCppSEHRecord.typeOfHandler = CatchAll;              \
                    PAL_TryHelper(&PalCppSEHRecord);                       \
                    PalCppSEHDispatcherState = InTryBlock;                 \
                } /* if (PalCppSEHDispatcherState...) */                   \
            } while(1);                                                    \
        } PAL_GOOD(catch) (PEXCEPTION_RECORD record) {                         \
            PalCppCaughtRecord = record;                                   \
            if (true) {


#define PAL_CPP_ENDTRY                                                     \
            }                                                              \
            if (!(PalCppSEHRecord.dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET))\
                PAL_GOOD(throw) PalCppCaughtRecord;                            \
        } /* catch */                                                      \
        if (PalCppCaughtRecord == NULL)                                    \
            PalCppSEHDispatcherState = (PAL_SEH_DISPATCHER_STATE)          \
                    PAL_EndTryHelper(&PalCppSEHRecord, 0);                 \
        else PalCppSEHDispatcherState = DoneWithSEH;                       \
    } while (PalCppSEHDispatcherState != DoneWithSEH); }

#endif  /* DISABLE_EXCEPTIONS */

#define __try       Use_PAL_TRY
#define __except    Use_PAL_EXCEPT
#define __finally   Use_PAL_FINALLY
#define __leave     Do_not_use___leave

#define try (Use_PAL_CPP_TRY) try
#define catch (Use_PAL_CPP_CATCH) catch
#define throw (Use_PAL_CPP_THROW_OR_RETHROW) throw

#define REMOVE_ARG(x) 
#define PAL_GOOD(x) REMOVE_ARG x

#ifdef _MSC_VER
#pragma warning(disable:4611) // interaction between '_setjmp' and C++ object destruction is non-portable
#endif


#define PAL_TRY         PAL_TRY_NAKED
#define PAL_TRY_FOR_DLLMAIN(_reason) PAL_TRY_NAKED
#define PAL_ENDTRY      PAL_ENDTRY_NAKED

#define EXCEPTION_CONTINUE_SEARCH   0
#define EXCEPTION_EXECUTE_HANDLER   1
#define EXCEPTION_CONTINUE_EXECUTION -1

// Platform-specific library naming
#ifdef PLATFORM_UNIX
#ifdef __APPLE__
#define MAKEDLLNAME_W(name) L"lib" name L".dylib"
#define MAKEDLLNAME_A(name)  "lib" name  ".dylib"
#else
#define MAKEDLLNAME_W(name) L"lib" name L".so"
#define MAKEDLLNAME_A(name)  "lib" name  ".so"
#endif
#else
#define MAKEDLLNAME_W(name) name L".dll"
#define MAKEDLLNAME_A(name) name  ".dll"
#endif

#ifdef UNICODE
#define MAKEDLLNAME(x) MAKEDLLNAME_W(x)
#else
#define MAKEDLLNAME(x) MAKEDLLNAME_A(x)
#endif

#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)

#define WAIT_IO_COMPLETION                  STATUS_USER_APC

#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE

#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT

/*  These are from the <FCNTL.H> file in windows.
    They are needed for _open_osfhandle.*/
#define _O_RDONLY   0x0000
#define _O_APPEND   0x0008
#define _O_TEXT     0x4000
#define _O_BINARY   0x8000

// Why do we need this?
// Short version: our compilers need to read LP64 code but the clr core is 
// written for LLP64.
// Longer version: see comment in the compiler-specific glue section 

#ifdef  __cplusplus
}
#endif

#endif // __ROTOR_PAL_H__
