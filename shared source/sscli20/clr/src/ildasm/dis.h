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
// dis.h
//
#include "formattype.h"

#define MAX_INTERFACES_IMPLEMENTED  256     // unused
#define MAX_CLASSNAME_LENGTH        1024    // single global buffer size
#define MAX_MEMBER_LENGTH           1024    // single global buffer size
#define MAX_SIGNATURE_LENGTH        2048    // single global buffer size
#define DESCR_SIZE                  8       // unused

#define MAX_FILENAME_LENGTH         2048     //256

#define MODE_DUMP_ALL               0
#define MODE_DUMP_CLASS             1
#define MODE_DUMP_CLASS_METHOD      2
#define MODE_DUMP_CLASS_METHOD_SIG  3

BOOL Disassemble(IMDInternalImport *pImport, BYTE *pCode, void *GUICookie, mdToken FuncToken, ParamDescriptor* pszArgname, ULONG ulArgs);
BOOL Decompile(IMDInternalImport *pImport, BYTE *pCode);
OPCODE DecodeOpcode(const BYTE *pCode, DWORD *pdwLen);
struct LineCodeDescr
{
    ULONG Line;
    ULONG Column;
    ULONG LineEnd;
    ULONG ColumnEnd;
    ULONG PC;
    ULONG_PTR FileToken;
};

void printLine(void* GUICookie, __in __nullterminated char* string);
void printLineW(void* GUICookie, __in __nullterminated WCHAR* string);
void printError(void* GUICookie, __in __nullterminated char* string);

char* AnsiToUtf(__in __nullterminated char* sz);
char* UnicodeToAnsi(__in __nullterminated WCHAR* wz);
char* UnicodeToUtf(__in __nullterminated WCHAR* wz);
WCHAR* UtfToUnicode(__in __nullterminated char* sz);
WCHAR* AnsiToUnicode(__in __nullterminated char* sz);

char* RstrUTF(unsigned id);
WCHAR* RstrW(unsigned id);
char* RstrANSI(unsigned id);


BOOL DumpMethod(mdToken FuncToken, const char *pszClassName, DWORD dwEntryPointToken,void *GUICookie,BOOL DumpBody);
BOOL DumpField(mdToken FuncToken, const char *pszClassName,void *GUICookie, BOOL DumpBogy);
BOOL DumpEvent(mdToken FuncToken, const char *pszClassName, DWORD dwClassAttrs, void *GUICookie, BOOL DumpBody);
BOOL DumpProp(mdToken FuncToken, const char *pszClassName, DWORD dwClassAttrs, void *GUICookie, BOOL DumpBody);
void dumpEHInfo(IMDInternalImport *pImport, void *GUICookie);
BOOL DumpClass(mdTypeDef cl, DWORD dwEntryPointToken, void* GUICookie, ULONG WhatToDump);
// WhatToDump: 0-title only; 1-pack,size and custom attrs; 2-everything
BOOL GetClassLayout(mdTypeDef cl, ULONG* pulPackSize, ULONG* pulClassSize);
void DumpCustomAttribute(mdCustomAttribute tkCA, void *GUICookie, bool bWithOwner);
void DumpCustomAttributes(mdToken tkOwner, void *GUICookie);
void DumpByteArray(__inout __nullterminated char* szString, BYTE* pBlob, ULONG ulLen, void* GUICookie);
char* DumpDataPtr(__inout __nullterminated char* buffer, DWORD ptr, DWORD size);

void PrettyPrintToken(__inout __nullterminated char* szString, mdToken tk, IMDInternalImport *pImport,void* GUICookie,mdToken FuncToken); //TypeDef,TypeRef,TypeSpec,MethodDef,FieldDef,MemberRef,MethodSpec,String
void DumpPermissions(mdToken tkOwner, void* GUICookie);
void DumpHeader(IMAGE_COR20_HEADER *CORHeader, void* GUICookie);
void DumpHeaderDetails(IMAGE_COR20_HEADER *CORHeader, void* GUICookie);
void DumpMetaInfo(__in __nullterminated char* pszFileName, __in_opt __nullterminated char* pszObjFileName, void* GUICookie);
void DumpStatistics(IMAGE_COR20_HEADER *CORHeader, void* GUICookie);
BOOL DumpFile(__in __nullterminated char *pszFilename);
void Cleanup();
char * DumpQString(void* GUICookie, 
                   __inout __nullterminated char* szToDump, 
                   __in __nullterminated char* szPrefix, 
                   unsigned uMaxLen);
void DumpVtable(void* GUICookie);
char* DumpUnicodeString(void* GUICookie,
                        __inout __nullterminated char* szString,
                        __in_ecount(cbString) WCHAR* pszString,
                        ULONG cbString, 
                        bool SwapString = false);

void TokenSigInit(IMDInternalImport *pImport);
void TokenSigDelete();


//---------------- see DMAN.CPP--------------------------------------------------
struct LocalComTypeDescr
{
    mdExportedType      tkComTypeTok;
    mdTypeDef           tkTypeDef;
    mdToken             tkImplementation;
    WCHAR*              wzName;
    DWORD               dwFlags;
    LocalComTypeDescr()  { wzName=NULL; };
    ~LocalComTypeDescr() { if(wzName) SDELETE(wzName); };
};

struct  MTokName
{
    mdFile  tok;
    WCHAR*  name;
    MTokName() { tok = 0; name = NULL; };
    ~MTokName() { if(name) SDELETE(name); };
};
extern BOOL g_fPrettyPrint;
extern MTokName*    rExeloc;
extern ULONG    nExelocs;
void DumpImplementation(mdToken tkImplementation, 
                        DWORD dwOffset, 
                        __inout __nullterminated char* szString, 
                        void* GUICookie);
void DumpComType(LocalComTypeDescr* pCTD,
                 __inout __nullterminated char* szString,
                 void* GUICookie);
void DumpManifest(void* GUICookie);
void DumpTypedefs(void* GUICookie);
IMetaDataAssemblyImport* GetAssemblyImport(void* GUICookie);

void DumpRTFPrefix(void* GUICookie, BOOL fFontDefault);
void DumpRTFPostfix(void* GUICookie);

//-------------------------------------------------------------------------------
#define NEW_TRY_BLOCK   0x80000000
#define PUT_INTO_CODE   0x40000000
#define ERR_OUT_OF_CODE 0x20000000
#define SEH_NEW_PUT_MASK    (NEW_TRY_BLOCK | PUT_INTO_CODE | ERR_OUT_OF_CODE)
//-------------------------------------------------------------------------------

#define UNIBUF_SIZE 131072
extern WCHAR   wzUniBuf[]; // defined in dis.cpp

#define SZSTRING_SIZE 131072
extern char   szString[]; // defined in dis.cpp

char *DumpGenericPars(__inout_ecount(SZSTRING_SIZE) char* szString, 
                      mdToken tok, 
                      void* GUICookie=NULL, 
                      BOOL fSplit=FALSE);

#include "safemath.h"
#define SZSTRING_SIZE_M4 (SZSTRING_SIZE - 4)
//#define CHECK_REMAINING_SIZE if((szptr - szString) >= (SZSTRING_SIZE-4)) break;
#define CHECK_REMAINING_SIZE if(ovadd_le((size_t)szString, SZSTRING_SIZE_M4, (size_t)szptr)) break;
//#define SZSTRING_REMAINING_SIZE (SZSTRING_SIZE - (szptr-szString))
#define SZSTRING_REMAINING_SIZE(x) (ovadd_le((size_t)szString,SZSTRING_SIZE,(size_t)(x))?0:(SZSTRING_SIZE-((size_t)(x)-(size_t)szString)))

