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
//
// verifier.hpp
// Dead code verification is for supporting FJIT. If FJIT gets fixed so that it
// can handle dead code, remove code #ifdefed in _VER_VERIFY_DEAD_CODE
//

#ifndef _VERIFIER_HPP
#define _VERIFIER_HPP

#define _VER_VERIFY_DEAD_CODE   1       // Verifies dead code

#include "ivehandler.h"
#include "vererror.h"

class Verifier;
class CValidator;
class ValidateWorkerArgs;

#define VER_FORCE_VERIFY        0x0001      // Fail even for fully trusted code
#define VER_STOP_ON_FIRST_ERROR 0x0002      // Tools can handle multiple errors 

// Extensions to ELEMENT_TYPE_* enumeration in cor.h

// Any objref
#define VER_ELEMENT_TYPE_OBJREF         (ELEMENT_TYPE_MAX)

// Any value class
#define VER_ELEMENT_TYPE_VALUE_CLASS    (ELEMENT_TYPE_MAX+1)

// A by-ref anything
#define VER_ELEMENT_TYPE_BYREF          (ELEMENT_TYPE_MAX+2)

// Unknown/invalid type
#define VER_ELEMENT_TYPE_UNKNOWN        (ELEMENT_TYPE_MAX+3)

// Sentinel value (stored at slots -1 and -2 of the stack to catch stack overflow)
#define VER_ELEMENT_TYPE_SENTINEL       (ELEMENT_TYPE_MAX+4)

#define VER_LAST_BASIC_TYPE             (ELEMENT_TYPE_MAX+4)

#define VER_ARG_RET     VER_ERR_ARG_RET
#define VER_NO_ARG      VER_ERR_NO_ARG



#include "cor.h"
#include "veropcodes.hpp"
#include "util.hpp"


#define MAX_SIGMSG_LENGTH 100
#define MAX_FAILMSG_LENGTH 384 + MAX_SIGMSG_LENGTH


struct VerExceptionInfo;
struct VerExceptionBlock;
class  Verifier;



class Verifier
{
    friend class VerSig;
    friend class Item;

public:
    static WCHAR*   GetErrorMsg(HRESULT hError, VerError err, WCHAR *wszMsg, int len, ValidateWorkerArgs* pArgs);
    static HRESULT  ReportError(IVEHandler *pVeh, HRESULT hrError, VEContext* pVec, ValidateWorkerArgs* pArgs);

private:
    static VOID Verifier::GetDefaultMessage(HRESULT hrError, SString& sMessage, SString& sRes, SString& sPrint);
    static VOID Verifier::GetAssemblyName(HRESULT hrError, SString& sMessage, SString& sRes, SString& sPrint, ValidateWorkerArgs* pArgs);
};


class ValidateWorkerArgs
{
public:
    CValidator *val;
    HRESULT hr;
    bool fDeletePEFile;
    MethodDesc* pMethodDesc;
    LPWSTR wszFileName;
    BYTE *pe;
    unsigned long size;
    bool fVerbose;
    bool fShowSourceLines;

    ValidateWorkerArgs()
        :   val(NULL),
            hr(S_OK),
            fDeletePEFile(true),
            pMethodDesc(NULL),
            wszFileName(NULL),
            fVerbose(false),
            fShowSourceLines(false)
    {LEAF_CONTRACT; }
};

#endif /* _VERIFIER_HPP */
