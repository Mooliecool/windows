// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==

#ifndef __CONSOLEOUTPUT_H__
#define __CONSOLEOUTPUT_H__

#if !defined(CLIENT_IS_CSC) && !defined(CLIENT_IS_ALINK)
#error "Must define either CLIENT_IS_CSC or CLIENT_IS_ALINK when using ConsoleOutput"
#endif

#include "cscommon.h"
#include <io.h>
#include <fcntl.h>

struct ERROR_INFO {
    short number;       // error number
    short level;        // warning level; 0 means error
    int   resid;        // resource id.
};

// ConsoleOutput contains all shared code relative to console output for both CSC and Alink.  Be sure to integrate any
// changes into both both.
class ConsoleOutput
{
public:
    // used as global object - no constructor and destructor is necessary, everything is zero initialized
    // ConsoleOutput               ();
    // ~ConsoleOutput              ();

    bool    FIsConsole          (FILE *fd);
    bool    Initialize          (HINSTANCE hinstMessages, LPCWSTR messageDLL, const ERROR_INFO *errorInfo);

    void    ShowErrorId         (int id, ERRORKIND kind);
    void    ShowError           (int id, ERRORKIND kind, PCWSTR inputFile, int line, int col, int lineEnd, int colEnd, LPCWSTR text);

    LPCWSTR ErrorSystemError    (DWORD error); 
    LPCWSTR ErrorHR             (HRESULT hr);  // also checks for COM error description
    LPCWSTR ErrorLastError      (); // call instead of GetLastError

    void _cdecl ShowErrorIdString (int id, ERRORKIND kind, ...);
    void _cdecl ShowErrorIdLocation (int id, ERRORKIND kind, LPCWSTR szLocation, ...);
    bool    HadError            () { return m_fHadError; }
    bool    HadFatalError       () { return m_fHadFatalError; }

    void    PrintString         (unsigned id, bool newline = true, bool prettyprint = false);
    void    PrintBanner         ();
    void    PrintBlankLine      ()  { print(L"\n"); }
    void __cdecl print          (const WCHAR * format, ...);

#ifdef CLIENT_IS_CSC
    // functions currently only being used by csc.
    HRESULT PrintOption         (DWORD CurrentGrp, LPCWSTR pszLongSwitch, LPCWSTR pszShortSwitch, LPCWSTR pszDescription, DWORD dwFlags);
    HRESULT PrintOptionId       (DWORD CurrentGrp, LPCWSTR pszLongSwitch, LPCWSTR pszShortSwitch, DWORD dwDescId, DWORD dwFlags);
    void    ShowRelatedLocation (ERRORKIND kind, PCWSTR inputFile, int line, int col, int lineEnd, int colEnd);
#endif

    void    EnableFullPaths     () { m_fFullPaths = true; }
    void    EnableUTF8Output    ();
    void    ShowErrorExtents    (bool showErrorEx){ m_fShowErrorExtents = showErrorEx; }

    HRESULT InitBugReport       (LPCWSTR pszFilename, int argc, WCBuffer argv[], bool fPromptUser);
    void    CloseBugReport      ();
    bool    BugReportEnabled    () {
        return FALSE;
    }
    void    OutputFileToReproFile  (PCWSTR pszName, PCWSTR pcsText);
    void    OutputBinaryFileToRepro(LPCWSTR pszFilename, LPCWSTR pszAssemblyFile);
    void    OutputUserReproDescription ();
    HRESULT GetBugReportFileName(WCBuffer pszFileName);
    void    OutputCommandLine(bool fBugReport, int argc, WCBuffer argv[]);

protected:

    int     LoadStringWide      (HINSTANCE hInstance, UINT uID, WCBuffer lpBuffer);
    void    print_internal      (LPCWSTR text);
    void    PrettyPrint         (LPCWSTR pszText, size_t cchIndent);
    void    Indent              (size_t cchIndent);
    void    GetCLRVersion       (WCBuffer szVersion);
    void    GetFileVersion      (HINSTANCE hinst, WCBuffer szVersion);

    void    DisableReproOutput  ();
    void    EnableReproOutput   ();
    LPWSTR  FormatErrorId       (int id, ERRORKIND kind, va_list args);
    LPWSTR  FormatErrorIdW      (int id, ERRORKIND kind, va_list args);

    UINT    GetConsoleOutputCP  ();
    int     WideCharToConsole   (LPCWSTR wideStr, LPSTR lpBuffer, int nBufferMax);
    HRESULT WideToConsole       (LPCWSTR wideStr, CAllocBuffer &buffer);


    bool    m_fHadError;
    bool    m_fHadFatalError;
    HINSTANCE m_hinstMessages;

    bool    m_fFullPaths;
    bool    m_fUTF8Output;
    bool    m_fShowErrorExtents;
    bool    m_fPrettyPrint;
    bool    m_fRedirectingToFile;
    size_t  m_width;
    size_t  m_currentColumn;
    WCHAR   m_errorBuffer[2048];
    UINT    m_ConsoleCodepage;

#ifdef CLIENT_IS_CSC
    // alink currently does not display related locations.
    WCHAR   m_szRelatedWarning[256], m_szRelatedError[256];
    static const LPWSTR m_errorPrefix;
#elif defined(CLIENT_IS_ALINK)
    static const LPWSTR m_errorPrefix;
#endif

    const ERROR_INFO* m_pErrorInfo;


};

#endif
