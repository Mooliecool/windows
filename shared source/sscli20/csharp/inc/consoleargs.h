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

#ifndef __CONSOLEARGS_H__
#define __CONSOLEARGS_H__

#if !defined(CLIENT_IS_CSC) && !defined(CLIENT_IS_ALINK)
#error "Must define either CLIENT_IS_CSC or CLIENT_IS_ALINK when using ConsoleOutput"
#endif

#include "list.h"
#include "tree.h"

typedef tree<LPCWSTR> b_tree;
typedef list<WCHAR*> WStrList;

class ConsoleArgs {
public:
    // Change the filename in-place
    bool GetFullFileName(const WCBuffer filenameBuffer, bool fOutputFilename);
    // Place the fully-qualified filename in the given output buffer
    bool GetFullFileName(LPCWSTR szSource, const WCBuffer filenameBuffer, bool fOutputFilename);

    ConsoleArgs(ConsoleOutput * output) :
        m_rgArgs(NULL),
        m_listArgs(NULL),
        m_output(output)
        { };
    ~ConsoleArgs() { CleanUpArgs(); };

    // returns false and returns 0/NULL if there are previous errors
    bool GetArgs(int * pargc,  WCBuffer ** pargv);

    // Frees all memory used by the arg list and the argv/argc array
    void CleanUpArgs();

    // Similar to wcstoul, but returns a 64-bit number, and always assumes base is 0 
    static ULONGLONG wcstoul64( LPCWSTR nptr, LPCWSTR * endptr);

private:

    b_tree * MakeLeaf( LPCWSTR szText);
    void     CleanupTree( b_tree * root);
    HRESULT  TreeAdd( b_tree ** root, LPCWSTR szAdd);
    void     TextToArgs( LPCWSTR szText, WStrList ** listReplace);
    void     ProcessResponseArgs();
#ifdef CLIENT_IS_CSC
    void     ProcessAutoConfig();
#endif

    WCBuffer *        m_rgArgs;
    WStrList *      m_listArgs;
    ConsoleOutput * m_output;
};

#endif // __CONSOLEARGS_H__
