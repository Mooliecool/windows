/*=====================================================================
**
** Source:  SearchPathW.c
**
** Purpose: Tests the PAL implementation of the SearchFileW function.
**
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
** TODO: Write a test where complete path is passed (say c:\?)
**===================================================================*/
//SearchPath
//
//The SearchPath function searches for the specified file in the specified path.
//
//
//DWORD SearchPath(
//  LPCTSTR lpPath,
//  LPCTSTR lpFileName,
//  LPCTSTR lpExtension,
//  DWORD nBufferLength,
//  LPTSTR lpBuffer,
//  LPTSTR* lpFilePart
//);
//
//Parameters
//lpPath 
//[in] Pointer to a null-terminated string that specifies the path to be searched for the file. If this parameter is NULL, the function searches for a matching file in the following directories in the following sequence: 
//The directory from which the application loaded. 
//The current directory. 
//The system directory. Use the GetSystemDirectory function to get the path of this directory. 
//The 16-bit system directory. There is no function that retrieves the path of this directory, but it is searched. 
//The Windows directory. Use the GetWindowsDirectory function to get the path of this directory. 
//The directories that are listed in the PATH environment variable. 

//lpFileName 
//[in] Pointer to a null-terminated string that specifies the name of the file to search for. 

//lpExtension 
//[in] Pointer to a null-terminated string that specifies an extension to be added to the file name when searching for the file. The first character of the file name extension must be a period (.). The extension is added only if the specified file name does not end with an extension. 
//If a file name extension is not required or if the file name contains an extension, this parameter can be NULL.
//
//nBufferLength 
//[in] Size of the buffer that receives the valid path and file name, in TCHARs. 

//lpBuffer 
//[out] Pointer to the buffer that receives the path and file name of the file found. 

//lpFilePart 
//[out] Pointer to the variable that receives the address (within lpBuffer) of the last component of the valid path and file name, which is the address of the character immediately following the final backslash (\) in the path. 

//Return Values
//If the function succeeds, the value returned is the length, in TCHARs, of the string copied to the buffer, not including the terminating null character. If the return value is greater than nBufferLength, the value returned is the size of the buffer required to hold the path.
//
//If the function fails, the return value is zero. To get extended error information, call GetLastError.


#include <palsuite.h>
const char* szDir                   =          ".";

const char* szNoFileName            =          "333asdf";
const char* szNoFileNameExt         =          ".x77t";

const char* szFileNameExists        =          "searchpathw";
const char* szFileNameExtExists     =          ".c";

const char* szFileNameExistsWithExt =          "searchpathw.c";


int __cdecl main(int argc, char *argv[]) {

    WCHAR* lpPath        = NULL;
    WCHAR* lpFileName    = NULL;
    WCHAR* lpExtension   = NULL;
    DWORD  nBufferLength = 0;
    WCHAR  lpBuffer[_MAX_PATH];
    WCHAR** lpFilePart    = NULL;
    DWORD  error         = 0;
    DWORD  result        = 0;

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    //
    // find a file that doesn't exist
    //
    ZeroMemory( lpBuffer, sizeof(lpBuffer));
    lpPath        = convert((LPSTR)szDir);
    lpFileName    = convert((LPSTR)szNoFileName);
    lpExtension   = convert((LPSTR)szNoFileNameExt);

    if( SearchPathW( lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart) != 0 ){
        error = GetLastError();
        free(lpPath);
        free(lpFileName);
        free(lpExtension);
        Fail ("SearchPathW: ERROR1 -> Found invalid file[%s][%s][%d]\n", szNoFileName, szNoFileNameExt, error);
    }

    free(lpPath);
    free(lpFileName);
    free(lpExtension);

    //
    // find a file that exists, when path is mentioned explicitly
    //
    ZeroMemory( lpBuffer, sizeof(lpBuffer));
    lpPath        = convert((LPSTR)szDir);
    lpFileName    = convert((LPSTR)szFileNameExists);
    lpExtension   = convert((LPSTR)szFileNameExtExists);

    result  = SearchPathW( lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

    if( result == 0 ){
        error = GetLastError();
        free(lpPath);
        free(lpFileName);
        free(lpExtension);
        Fail ("SearchPathW: ERROR2 -> Did not Find valid file[%s][%s][%d]\n", szFileNameExists, szFileNameExtExists, error);
    }

    free(lpPath);
    free(lpFileName);
    free(lpExtension);

    //
    // find a file that exists, when path is not mentioned explicitly
    //
    ZeroMemory( lpBuffer, sizeof(lpBuffer));
    lpFileName    = convert((LPSTR)szFileNameExists);
    lpExtension   = convert((LPSTR)szFileNameExtExists);

    result  = SearchPathW( NULL, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

    if( result == 0 ){
        error = GetLastError();
        free(lpFileName);
        free(lpExtension);
        Fail ("SearchPathW: ERROR3 -> Did not Find valid file[%s][%s][%d]\n", szFileNameExists, szFileNameExtExists, error);
    }

    free(lpFileName);
    free(lpExtension);

    //
    // find a file that exists, when path is not mentioned explicitly, and file name has
    // extension and file extension is made null
    //
    ZeroMemory( lpBuffer, sizeof(lpBuffer));
    lpFileName    = convert((LPSTR)szFileNameExistsWithExt);

    result  = SearchPathW( NULL, lpFileName, NULL, nBufferLength, lpBuffer, lpFilePart);

    if( result == 0 ){
        error = GetLastError();
        free(lpFileName);
        Fail ("SearchPathW: ERROR4 -> Did not Find valid file[%s][%d]\n", szFileNameExists, error);
    }

    free(lpFileName);

    //
    // find a file that exists, when path is not mentioned explicitly, and file name has
    // extension and file extension is made null, also there is not enough space in buffer
    //
    lpFileName    = convert((LPSTR)szFileNameExistsWithExt);

    result  = SearchPathW( NULL, lpFileName, NULL, nBufferLength, NULL, lpFilePart);

    if( result == 0 ){
        error = GetLastError();
        free(lpFileName);
        Fail ("SearchPathW: ERROR5 -> Did not Find valid file[%s][%d]\n", szFileNameExists, error);
    }

    free(lpFileName);

    PAL_Terminate();
    return PASS; 
}
