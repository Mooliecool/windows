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
#ifndef __disasm_h__
#define __disasm_h__


struct InfoHdr;
class GCDump;


struct DumpStackFlag
{
    BOOL fEEonly;
    DWORD_PTR top;
    DWORD_PTR end;
};

struct GCEncodingInfo
{
    LPVOID pvMainFiber;
    LPVOID pvGCTableFiber;

    BYTE *table;
    unsigned int methodSize;
    GCDump *pGCDump;
#ifdef _X86_
    InfoHdr *pHeader;
#endif // _X86_

    char buf[1000];
    int cch;

    SIZE_T ofs;
    
    // When decoding a cold region, set this to the size of the hot region to keep offset
    // calculations working.
    SIZE_T hotSizeToAdd;    
    bool fDoneDecoding;
};


struct SOSEHInfo
{
    DACEHInfo *m_pInfos;
    UINT EHCount;
    CLRDATA_ADDRESS methodStart;

    SOSEHInfo() { ZeroMemory(this,sizeof(SOSEHInfo)); }
    ~SOSEHInfo() { if (m_pInfos) { delete [] m_pInfos; } }    

    void FormatForDisassembly(CLRDATA_ADDRESS offSet);
};

void Unassembly (DWORD_PTR IPBegin, DWORD_PTR IPEnd, DWORD_PTR IPAskedFor, 
    DWORD_PTR GCStressCodeCopy, GCEncodingInfo *pGCEncodingInfo,
    SOSEHInfo *pEHInfo);

void DumpStackDummy (DumpStackFlag &DSFlag);
void DumpStackObjectsHelper (size_t StackTop, size_t StackBottom, BOOL verifyFields);

void UnassemblyUnmanaged (DWORD_PTR IP);

BOOL GetCalleeSite (DWORD_PTR IP, DWORD_PTR &IPCallee);

HRESULT CheckEEDll ();

void DisasmAndClean (DWORD_PTR &IP, __out_ecount (length) __out_opt char *line, ULONG length);

INT_PTR GetValueFromExpr(__in __in_z char *ptr, INT_PTR &value);

void NextTerm (__deref_inout_z char *& ptr);

BOOL IsByRef (__deref_inout_z char *& ptr);

BOOL IsTermSep (char ch);

const char * HelperFuncName (size_t IP);


#endif // __disasm_h__
