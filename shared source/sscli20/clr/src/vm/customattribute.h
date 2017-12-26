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
#ifndef _CUSTOMATTRIBUTE_H_
#define _CUSTOMATTRIBUTE_H_

#include "fcall.h"

struct CustomAttributeType;
struct CustomAttributeValue;
struct CustomAttributeArgument;
struct CustomAttributeNamedArgument;

typedef Array<CustomAttributeArgument> CaArgArray;
typedef Array<CustomAttributeNamedArgument> CaNamedArgArray;
typedef Array<CustomAttributeValue> CaValueArray;

typedef DPTR(CaArgArray) PTR_CaArgArray;
typedef DPTR(CaNamedArgArray) PTR_CaNamedArgArray;
typedef DPTR(CaValueArray) PTR_CaValueArray;

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<CaArgArray> CaArgArrayREF;
typedef REF<CaNamedArgArray> CaNamedArgArrayREF;
typedef REF<CaValueArray> CaValueArrayREF;
#else
typedef PTR_CaArgArray CaArgArrayREF;
typedef PTR_CaNamedArgArray CaNamedArgArrayREF;
typedef PTR_CaValueArray CaValueArrayREF;
#endif


#include <pshpack1.h>
struct CustomAttributeType
{
    STRINGREF m_enumName;
    CorSerializationType m_tag;
    CorSerializationType m_enumType;
    CorSerializationType m_arrayType;    
    CorSerializationType m_padding;
};

struct CustomAttributeValue
{
    // longs come before refs on x86
    INT64               m_rawValue;
    CaValueArrayREF     m_value;
    STRINGREF           m_enumOrTypeName;
    CustomAttributeType m_type;
};

struct CustomAttributeArgument
{
    CustomAttributeType m_type;
#if !defined(_WIN64) && (DATA_ALIGNMENT > 4)
    DWORD m_padding;
#endif
    CustomAttributeValue m_value;
};

struct CustomAttributeNamedArgument
{
    STRINGREF m_argumentName;
    CorSerializationType m_propertyOrField;
    CorSerializationType m_padding;
#if !defined(_WIN64) && (DATA_ALIGNMENT > 4)
    DWORD m_padding2;
#endif
    CustomAttributeType m_type;
#if !defined(_WIN64) && (DATA_ALIGNMENT > 4)
    DWORD m_padding3;
#endif
    CustomAttributeValue m_value;
};
#include <poppack.h>


class Attribute
{
public:
    static FCDECL5(VOID, ParseAttributeArguments, 
        void* pCa, 
        INT32 cCa, 
        CaArgArrayREF* ppCustomAttributeArguments, 
        CaNamedArgArrayREF* ppCustomAttributeNamedArguments,
        DomainAssembly* pDomainAssembly);
};

class CORSEC_ATTRSET_ARRAY: public StackSArray<CORSEC_ATTRSET> 
{
    public:
    CORSEC_ATTRSET_ARRAY()
    {
    }
    ~CORSEC_ATTRSET_ARRAY()
    {
        WRAPPER_CONTRACT;
        for (COUNT_T i = 0; i < GetCount(); i++)
        {
            (*this)[i].CORSEC_ATTRSET::~CORSEC_ATTRSET();
        }

    }
};

class COMCustomAttribute
{
public:

    // custom attributes utility functions
    static FCDECL5(VOID, ParseAttributeUsageAttribute, PVOID pData, ULONG cData, ULONG* pTargets, CLR_BOOL* pInherited, CLR_BOOL* pAllowMultiple);
    static FCDECL5(LPVOID, CreateCaObject, Module* pAttributedModule, MethodDesc* pCtor, BYTE** ppBlob, BYTE* pEndBlob, INT32* pcNamedArgs);
    static FCDECL7(void, GetPropertyOrFieldData, Module* pModule, BYTE** ppBlobStart, BYTE* pBlobEnd, STRINGREF* pName, CLR_BOOL* pbIsProperty, OBJECTREF* pType, OBJECTREF* value);
    static FCDECL3(VOID, GetSecurityAttributes, Module *pModule, DWORD tkToken, PTRARRAYREF* ppArray);    
    static FCDECL2(VOID, PushSecurityContextFrame, SecurityContextFrame *pFrame, AssemblyBaseObject *pAssemblyObjectUNSAFE);
    static FCDECL1(VOID, PopSecurityContextFrame, SecurityContextFrame *pFrame);
};

#endif

