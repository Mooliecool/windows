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
////////////////////////////////////////////////////////////////////////////////
// This module contains the implementation of the native methods for the
//  varargs class(es)..
////////////////////////////////////////////////////////////////////////////////
#ifndef _COMVARARGS_H_
#define _COMVARARGS_H_


struct VARARGS
{
    VASigCookie *ArgCookie;
    SigPointer  SigPtr;
    BYTE        *ArgPtr;
    int         RemainingArgs;
};

class COMVarArgs
{
public:
    static FCDECL3(void, Init2, VARARGS* _this, LPVOID cookie, LPVOID firstArg);
    static FCDECL2(void, Init, VARARGS* _this, LPVOID cookie);
    static FCDECL1(int, GetRemainingCount, VARARGS* _this);
    static FCDECL1(void*, GetNextArgType, VARARGS* _this);
    //TypedByRef can not be passed by ref, so has to pass it as void pointer
    static FCDECL2(void, DoGetNextArg, VARARGS* _this, void * value);
    //TypedByRef can not be passed by ref, so has to pass it as void pointer
    static FCDECL3(void, GetNextArg2, VARARGS* _this, void * value, TypeHandle typehandle);

    static void GetNextArgHelper(VARARGS *data, TypedByRef *value, BOOL fData);
    static va_list MarshalToUnmanagedVaList(const VARARGS *data);
    static void    MarshalToManagedVaList(va_list va, VARARGS *dataout);

#ifdef CALLDESCR_REGTYPEMAP
    static FCDECL1(UINT64, GetRegisterTypeMap, VARARGS* _this);
#endif // CALLDESCR_REGTYPEMAP
};

#endif // _COMVARARGS_H_
