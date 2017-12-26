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
// dis.cpp
//
// Disassembler
//
#include <stdio.h>
#include <stdlib.h>

#define _gcvt_s(a,b,c,d) _gcvt(c,d,a)

#include <utilcode.h>
#include "specstrings.h"
#include "debugmacros.h"
#include "corpriv.h"
#include "dasmenum.hpp"
#include "dasmgui.h"
#include "formattype.h"
#include "dis.h"
#include "resource.h"
#include "ilformatter.h"
#include "outstring.h"
#include "utilcode.h" // for CQuickByte

#include "ceeload.h"
#include "dynamicarray.h"
extern PELoader * g_pPELoader;
#include <corsym.h>

extern ISymUnmanagedReader*     g_pSymReader;
extern BOOL     g_fDumpAsmCode;
extern char     g_szAsmCodeIndent[];
extern BOOL     g_fShowBytes;
extern BOOL     g_fShowSource;
extern BOOL     g_fInsertSourceLines;
extern BOOL     g_fTryInCode;
extern BOOL     g_fQuoteAllNames;
extern BOOL     g_fDumpTokens;
extern DynamicArray<__int32>  *g_pPtrTags;       //to keep track of all "ldptr"
extern DynamicArray<DWORD>    *g_pPtrSize;       //to keep track of all "ldptr"
extern int                     g_iPtrCount;
static BOOL ConvToLiteral(__inout __nullterminated char* retBuff, const WCHAR* str, int cbStr);
extern DWORD                   g_Mode;
extern unsigned g_uConsoleCP;

#define PADDING 28


extern BOOL                 g_fThisIsInstanceMethod;
extern unsigned             g_uCodePage;
extern HANDLE               hConsoleOut;
extern HANDLE               hConsoleErr;
// globals for source file info
ULONG_PTR ulWasFileToken = 0xFFFFFFFF;
GUID guidWasLang={0}, guidWasLangVendor={0},guidWasDoc={0};
WCHAR wzWasFileName[2048];
ULONG ulWasLine = 0;
FILE* pFile=NULL;
BOOL    bIsNewFile = TRUE;
WCHAR   wzUniBuf[UNIBUF_SIZE];
char    szString[SZSTRING_SIZE];
//-----------------------------------
struct LexScope
{
    DWORD               dwStart;
    DWORD               dwEnd;
    ISymUnmanagedScope* pISymScope;
    DWORD               dwZOrder;
    inline bool IsOpen() { return (dwZOrder != 0); };
    inline bool Covers(DWORD dwOffset) { return ((dwStart <= dwOffset) && (dwOffset < dwEnd)); };
};
//-----------------------------------

OPCODE DecodeOpcode(const BYTE *pCode, DWORD *pdwLen)
{
    OPCODE opcode;

    *pdwLen = 1;
    opcode = OPCODE(pCode[0]);
    switch(opcode) {
        case CEE_PREFIX1:
            opcode = OPCODE(pCode[1] + 256);
            if (opcode < 0 || opcode >= CEE_COUNT)
                opcode = CEE_COUNT;
            *pdwLen = 2;
            break;
        case CEE_PREFIXREF:
        case CEE_PREFIX2:
        case CEE_PREFIX3:
        case CEE_PREFIX4:
        case CEE_PREFIX5:
        case CEE_PREFIX6:
        case CEE_PREFIX7:
            *pdwLen = 3;
            return CEE_COUNT;
        default:
            break;
        }
    return opcode;
}
//------------------------------------------------------------------
WCHAR* UtfToUnicode(__in __nullterminated char* sz)
{
    WCHAR* wz = wzUniBuf;
    WszMultiByteToWideChar(CP_UTF8,0,sz,-1,wz,UNIBUF_SIZE/2);
    return wz;
}
char* UnicodeToAnsi(__in __nullterminated WCHAR* wz)
{
    char* sz = (char*)(&wzUniBuf[UNIBUF_SIZE/2]);
    WszWideCharToMultiByte(g_uConsoleCP,0,wz,-1,sz,UNIBUF_SIZE,NULL,NULL);
    return sz;
}
WCHAR* AnsiToUnicode(__in __nullterminated char* sz)
{
    WCHAR* wz = wzUniBuf;
    WszMultiByteToWideChar(g_uConsoleCP,0,sz,-1,wz,UNIBUF_SIZE/2);
    return wz;
}
char* UnicodeToUtf(__in __nullterminated WCHAR* wz)
{
    char* sz = (char*)(&wzUniBuf[UNIBUF_SIZE/2]);
    WszWideCharToMultiByte(CP_UTF8,0,wz,-1,sz,UNIBUF_SIZE,NULL,NULL);
    return sz;
}
char* AnsiToUtf(__in __nullterminated char* sz) { return UnicodeToUtf(AnsiToUnicode(sz));}
    
static void UnicodeToConsoleOrMsgBox(__in __nullterminated WCHAR* wz)
{
    {
        //DWORD dw;
        //char* sz = UnicodeToAnsi(wz);
        //WriteFile(hConsoleOut,(CONST VOID *)sz, (ULONG32)strlen(sz),&dw,NULL);
        //WriteFile(hConsoleOut,(CONST VOID *)"\r\n", 2,&dw,NULL);
        printf("%s\n",UnicodeToAnsi(wz));
    }
}
static void UnicodeToFile(__in __nullterminated WCHAR* wz, FILE* pF)
{
    unsigned endofline = 0x000A000D;
    int L;
    if((L=(int)wcslen(wz))) fwrite(wz,L*sizeof(WCHAR),1,pF);
    fwrite(&endofline,4,1,pF);
}
static void ToGUIOrFile(__in __nullterminated char* sz, void* GUICookie)
{
    {
        if(g_fDumpRTF) fprintf((FILE*)GUICookie,"%s\\line\n",sz);
        else fprintf((FILE*)GUICookie,"%s\n",sz);
    }
}
//------------------------------------------------------------------
void printError(void* GUICookie, __in __nullterminated char* string)
{
    {
        //DWORD dw;
        char* sz = ERRORMSG(string);
        if(GUICookie) printLine(GUICookie, sz);
        //sz = UnicodeToAnsi(UtfToUnicode(string));
        //WriteFile(hConsoleErr,(CONST VOID *)sz, (ULONG32)strlen(sz),&dw,NULL);
        //WriteFile(hConsoleErr,(CONST VOID *)"\r\n", 2,&dw,NULL);
        fprintf(stderr,"%s\n",UnicodeToAnsi(UtfToUnicode(string)));
    }
}
void printLine(void* GUICookie, __in __nullterminated char* string)
{
    char* sz = string;

    if (GUICookie == NULL)
    {
        UnicodeToConsoleOrMsgBox(UtfToUnicode(string));
        return;
    }


    if(g_uCodePage != CP_UTF8)
    {
        WCHAR* wz = UtfToUnicode(string);
        if(g_uCodePage == g_uConsoleCP)   sz = UnicodeToAnsi(wz);
        else if(GUICookie)
        {
            UnicodeToFile(wz,(FILE*)GUICookie);
            return;
        }
        else sz = (char*)wz;
    }
    ToGUIOrFile(sz,GUICookie);
}

void printLineW(void* GUICookie, __in __nullterminated WCHAR* string)
{
    char* sz = (char*)string;

    if (GUICookie == NULL)
    {
        UnicodeToConsoleOrMsgBox(string);
        return;
    }
    if(g_uCodePage == 0xFFFFFFFF)
    {
        {
            UnicodeToFile(string,(FILE*)GUICookie);
            return;
        }
    }
    else if(g_uCodePage == CP_UTF8)
        sz = UnicodeToUtf(string);
    else if(g_uCodePage == g_uConsoleCP)
        sz = UnicodeToAnsi(string);

    ToGUIOrFile(sz,GUICookie);
}

char * DumpQString(void* GUICookie, 
                   __in __nullterminated char* szToDump, 
                   __in __nullterminated char* szPrefix, 
                   unsigned uMaxLen)
{
    unsigned Lwt = (unsigned)strlen(szString);
    char* szptr = &szString[Lwt];
    char* p = szToDump;
    unsigned L = (unsigned)strlen(szToDump);
    unsigned l,i;
    unsigned tally=0;
    *szptr++ = '"';

    do
    {
        l = L;
        if(l > uMaxLen+2) // +2 - to account for leading/trailing doublequotes in szToDump
        {
            l = uMaxLen;
            while((p[l-1] == '\\')&& l) l--;
            if(l == 0) l = (uMaxLen+1) & 0xFFFFFFFE;
        }
        if(tally)
        {
            printLine(GUICookie,szString);
            szptr = szString;
            szptr+=sprintf_s(szptr,SZSTRING_SIZE,"%s+ \"",szPrefix);
        }
        else uMaxLen = uMaxLen - (ULONG32)strlen(szPrefix) + Lwt;
        tally++;
        for(i=0; i < l; i++, szptr++, p++)
        {
            switch(*p)
            {
                case '\n':  *szptr++ = '\\'; *szptr = 'n'; break;
                case '\r':  *szptr++ = '\\'; *szptr = 'r'; break;
                case '\t':  *szptr++ = '\\'; *szptr = 't'; break;
                case '\b':  *szptr++ = '\\'; *szptr = 'b'; break;
                case '\f':  *szptr++ = '\\'; *szptr = 'f'; break;
                case '\v':  *szptr++ = '\\'; *szptr = 'v'; break;
                case '\a':  *szptr++ = '\\'; *szptr = 'a'; break;
                case '\?':  *szptr++ = '\\'; *szptr = '?'; break;
                case '\\':  *szptr++ = '\\'; *szptr = '\\'; break;
                case '"':   *szptr++ = '\\'; *szptr = '"'; break;
                case '{': szptr += sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),SCOPE()); szptr--; break;
                case '}': szptr += sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),UNSCOPE()); szptr--; break;
                case '<': szptr += sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),LTN()); szptr--; break;
                case '>': szptr += sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),GTN()); szptr--; break;
                default:    *szptr = *p; break;
            }
        }
        *szptr++ = '"';
        *szptr = 0;
        L -= l;
    } while(L);
    return szptr;
}
COR_ILMETHOD_SECT_EH_CLAUSE_FAT* g_ehInfo = NULL;
ULONG   g_ehCount = 0;
/********************************************************************************/
/* used by qsort to sort the g_ehInfo table */
static int __cdecl ehInfoCmp(const void *op1, const void *op2)
{
    COR_ILMETHOD_SECT_EH_CLAUSE_FAT* p1 = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)op1;
    COR_ILMETHOD_SECT_EH_CLAUSE_FAT* p2 = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)op2;
    int d;
    d = p1->GetTryOffset() - p2->GetTryOffset(); if(d) return d;
    d = p1->GetTryLength() - p2->GetTryLength(); if(d) return d;
    d = p1->GetHandlerOffset() - p2->GetHandlerOffset(); if(d) return d;
    d = p1->GetHandlerLength() - p2->GetHandlerLength(); if(d) return d;
    return 0;
}

BOOL enumEHInfo(const COR_ILMETHOD_SECT_EH* eh, IMDInternalImport *pImport, DWORD dwCodeSize)
{
    COR_ILMETHOD_SECT_EH_CLAUSE_FAT ehBuff;
    const COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo;
    BOOL    fTryInCode = FALSE;
    if(g_ehInfo) VDELETE(g_ehInfo);
    g_ehCount = 0;
    if(eh && (g_ehCount = eh->EHCount()))
    {
        g_ehInfo = new COR_ILMETHOD_SECT_EH_CLAUSE_FAT[g_ehCount];
        _ASSERTE(g_ehInfo != NULL);
        unsigned i;
        for (i = 0; i < g_ehCount; i++)
        {
            ehInfo = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)eh->EHClause(i, &ehBuff);
            memcpy(&g_ehInfo[i],ehInfo,sizeof(COR_ILMETHOD_SECT_EH_CLAUSE_FAT));
            //_ASSERTE((ehInfo->GetFlags() & SEH_NEW_PUT_MASK) == 0); // we are using 0x80000000 and 0x40000000
            g_ehInfo[i].SetFlags((CorExceptionFlag)((int)ehInfo->GetFlags() & ~SEH_NEW_PUT_MASK));
        }
        // check if all boundaries are within method code:
        fTryInCode = g_fTryInCode;
        for(i=0; i < g_ehCount; i++)
        {
            if( (g_ehInfo[i].GetTryOffset() >= dwCodeSize) ||
                (g_ehInfo[i].GetTryOffset() + g_ehInfo[i].GetTryLength() >= dwCodeSize)||
                (g_ehInfo[i].GetHandlerOffset() >= dwCodeSize) ||
                (g_ehInfo[i].GetHandlerOffset() + g_ehInfo[i].GetHandlerLength() > dwCodeSize))
            {
                g_ehInfo[i].SetFlags((CorExceptionFlag)((int)g_ehInfo[i].GetFlags() | ERR_OUT_OF_CODE));
                fTryInCode = FALSE; // if out of code, don't expand
            }
        }

        if(fTryInCode)
        {
            DWORD dwWasTryOffset=0xFFFFFFFF, dwWasTryLength=0xFFFFFFFF, dwLastOffset=0xFFFFFFFF;
            unsigned iNewTryBlock=0;
            qsort(g_ehInfo, g_ehCount, sizeof(COR_ILMETHOD_SECT_EH_CLAUSE_FAT), ehInfoCmp);
            for(i=0; i < g_ehCount; i++)
            {
                if((g_ehInfo[i].GetTryOffset() != dwWasTryOffset)||(g_ehInfo[i].GetTryLength() != dwWasTryLength))
                {
                    g_ehInfo[i].SetFlags((CorExceptionFlag)((int)g_ehInfo[i].GetFlags() | NEW_TRY_BLOCK)); // insert try in source
                    dwLastOffset = g_ehInfo[i].GetTryOffset() + g_ehInfo[i].GetTryLength();
                    dwWasTryOffset = g_ehInfo[i].GetTryOffset();
                    dwWasTryLength = g_ehInfo[i].GetTryLength();
                    iNewTryBlock = i;
                }
                if((!(g_ehInfo[i].GetFlags() & COR_ILEXCEPTION_CLAUSE_FILTER))&&(g_ehInfo[i].GetHandlerOffset() == dwLastOffset))
                {
                    if((i == iNewTryBlock)||((int)g_ehInfo[iNewTryBlock].GetFlags() & PUT_INTO_CODE))
                        g_ehInfo[i].SetFlags((CorExceptionFlag)((int)g_ehInfo[i].GetFlags() | PUT_INTO_CODE)); // insert catch/finally in source
                    else
                        g_ehInfo[i].SetFlags((CorExceptionFlag)((int)g_ehInfo[i].GetFlags() | NEW_TRY_BLOCK)); // insert catch/finally in source

                    dwLastOffset += g_ehInfo[i].GetHandlerLength();
                }
                else
                    g_ehInfo[i].SetFlags((CorExceptionFlag)((int)g_ehInfo[i].GetFlags() | NEW_TRY_BLOCK)); // insert try in source
            }
        }
        else
        {
            for(i=0; i < g_ehCount; i++)
            {
                g_ehInfo[i].SetFlags((CorExceptionFlag)((int)g_ehInfo[i].GetFlags() | NEW_TRY_BLOCK)); // insert try in source
            }
        }
    }
    return fTryInCode;
}

void dumpOneEHInfo(COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo, IMDInternalImport *pImport, void *GUICookie)
{
    //char    szString[4096];
    char*   szptr = &szString[0];
    if(!ehInfo) return;
    DWORD dwFlags = ehInfo->GetFlags();
    if(dwFlags & PUT_INTO_CODE) return; // by the time dumpEHInfo is called, this ehInfo is done
    if(dwFlags & NEW_TRY_BLOCK)
    {
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%s%s IL_%04x ",g_szAsmCodeIndent,KEYWORD(".try"),ehInfo->GetTryOffset());
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%s IL_%04x ",KEYWORD("to"),ehInfo->GetTryOffset()+ehInfo->GetTryLength());
    }
    else
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%s                        ",g_szAsmCodeIndent);

    if (dwFlags & COR_ILEXCEPTION_CLAUSE_FILTER)
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%s IL_%04x ",KEYWORD("filter"),ehInfo->GetFilterOffset());
    else if (dwFlags & COR_ILEXCEPTION_CLAUSE_FAULT)
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), KEYWORD("fault "));
    else if (dwFlags & COR_ILEXCEPTION_CLAUSE_FINALLY || IsNilToken(ehInfo->GetClassToken()))
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), KEYWORD("finally "));
    else
    {
        CQuickBytes out;
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%s %s ",KEYWORD("catch"),PrettyPrintClass(&out, ehInfo->GetClassToken(), pImport));
        REGISTER_REF(g_tkRefUser,ehInfo->GetClassToken());
    }
    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%s IL_%04x",KEYWORD("handler"),ehInfo->GetHandlerOffset());
    if(ehInfo->GetHandlerLength() != (DWORD) -1)
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), " %s IL_%04x",KEYWORD("to"),ehInfo->GetHandlerOffset()+ehInfo->GetHandlerLength());

    printLine(GUICookie, szString);
    if(g_fShowBytes)
    {
        BYTE    *pb = (BYTE *)ehInfo;
        size_t i;
        szptr = &szString[0];
        szptr+=sprintf_s(szptr,SZSTRING_SIZE,"%s// HEX:",g_szAsmCodeIndent);
        ehInfo->SetFlags((CorExceptionFlag)((int)ehInfo->GetFlags() & ~SEH_NEW_PUT_MASK));
        for(i = 0; i < sizeof(COR_ILMETHOD_SECT_EH_CLAUSE_FAT); i++)
            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr)," %2.2X",*pb++);
        printLine(GUICookie, COMMENT(szString));
    }
    /*
    if(ehInfo->GetFlags() & ERR_OUT_OF_CODE)
    {
        sprintf(szString,"%s// WARNING: Boundary outside the method code",g_szAsmCodeIndent);
        printLine(GUICookie,szString);
    }
    */
}
void dumpEHInfo(IMDInternalImport *pImport, void *GUICookie)
{
    //char    szString[4096];

    if(! g_ehCount) return;
    sprintf_s(szString,SZSTRING_SIZE, "%s// Exception count %d", g_szAsmCodeIndent,g_ehCount);
    printLine(GUICookie, COMMENT(szString));

    for (unsigned i = 0; i < g_ehCount; i++)
    {
        COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo = &g_ehInfo[i];
        dumpOneEHInfo(ehInfo,pImport,GUICookie);
    }
}

static int __cdecl cmpLineCode(const void *p1, const void *p2)
{   int ret = (((LineCodeDescr*)p1)->PC - ((LineCodeDescr*)p2)->PC);
    if(ret==0)
    {
        ret= (((LineCodeDescr*)p1)->Line - ((LineCodeDescr*)p2)->Line);
        if(ret==0) ret= (((LineCodeDescr*)p1)->Column - ((LineCodeDescr*)p2)->Column);
    }
    return ret;
}

static int __cdecl cmpLexScope(const void *p1, const void *p2)
{
    LexScope* pls1 = (LexScope*)p1;
    LexScope* pls2 = (LexScope*)p2;
    int d;
    if((d = pls1->dwStart - pls2->dwStart)) return d;
    return (pls2->dwEnd - pls1->dwEnd);
}

char* DumpDataPtr(__inout __nullterminated char* buffer, DWORD ptr, DWORD size)
{
    // check if ptr really points to some data in one of sections
    IMAGE_SECTION_HEADER *pSecHdr = NULL;
    DWORD i;
    DWORD dwNumberOfSections;
    if(g_pPELoader->IsPE32())
    {
        pSecHdr = IMAGE_FIRST_SECTION(g_pPELoader->ntHeaders32());
        dwNumberOfSections = VAL16(g_pPELoader->ntHeaders32()->FileHeader.NumberOfSections);
    }
    else
    {
        pSecHdr = IMAGE_FIRST_SECTION(g_pPELoader->ntHeaders64());
        dwNumberOfSections = VAL16(g_pPELoader->ntHeaders64()->FileHeader.NumberOfSections);
    }

    for (i=0; i < dwNumberOfSections; i++,pSecHdr++)
    {
        if((ptr >= VAL32(pSecHdr->VirtualAddress))&&
            (ptr < VAL32(pSecHdr->VirtualAddress)+VAL32(pSecHdr->Misc.VirtualSize)))
        {
            if(ptr+size > VAL32(pSecHdr->VirtualAddress)+VAL32(pSecHdr->Misc.VirtualSize)) i = dwNumberOfSections;
            break;
        }
    }
    if(i < dwNumberOfSections)
    { // yes, the pointer points to real data
        int j;
        for(j=0; (j < g_iPtrCount)&&((*g_pPtrTags)[j] != (__int32)ptr); j++);
        if(j == g_iPtrCount)
        {
            if (g_pPtrSize == NULL)
            {
                g_pPtrSize = new DynamicArray<DWORD>;
            }
            if (g_pPtrTags == NULL)
            {
                g_pPtrTags = new DynamicArray<__int32>;
            }

            (*g_pPtrSize)[g_iPtrCount] = size;
            (*g_pPtrTags)[g_iPtrCount++] = ptr;
        }
        else if((*g_pPtrSize)[j] < size) (*g_pPtrSize)[j] = size;
        const char* szTls = "D_";
        if(strcmp((const char*)(pSecHdr->Name),".tls") == 0) szTls = "T_";
        else if(strcmp((const char*)(pSecHdr->Name),".text") == 0) szTls = "I_";
        buffer+=sprintf_s(buffer,SZSTRING_REMAINING_SIZE(buffer), "%s%8.8X",szTls,ptr);
    } //else print as hex
    else
    {
        buffer+=sprintf_s(buffer,SZSTRING_REMAINING_SIZE(buffer),ERRORMSG("0x%8.8X /* WARNING: rogue pointer! (size 0x%8.8X) */"),ptr,size);
    }
    return buffer;
}

void DumpLocals(IMDInternalImport *pImport,COR_ILMETHOD_DECODER *pMethod, __in __nullterminated char* szVarPrefix, void* GUICookie)
{
    if (pMethod->GetLocalVarSigTok())
    {
        DWORD           cbSigLen;
        PCCOR_SIGNATURE pComSig;
        CQuickBytes     qbMemberSig;
        size_t          dwL;


        pComSig = pImport->GetSigFromToken((mdSignature)(pMethod->GetLocalVarSigTok()), &cbSigLen);

        _ASSERTE(*pComSig == IMAGE_CEE_CS_CALLCONV_LOCAL_SIG);
        pComSig++;

        appendStr(&qbMemberSig, g_szAsmCodeIndent);
        appendStr(&qbMemberSig, KEYWORD(".locals "));
        if(g_fDumpTokens)
        {
            char sz[32];
            sprintf_s(sz,32,"/*%08X*/ ",pMethod->GetLocalVarSigTok());
            appendStr(&qbMemberSig,COMMENT(sz));
        }
        if(pMethod->GetFlags() & CorILMethod_InitLocals) appendStr(&qbMemberSig, KEYWORD("init "));
        dwL = qbMemberSig.Size();

        char* pszTailSig = (char *) PrettyPrintSig(pComSig, cbSigLen, 0, &qbMemberSig, pImport,szVarPrefix);
        if(strlen(pszTailSig) < dwL+3) return;

        {
            ULONG32 i,j,k,indent = (ULONG32)strlen(g_szAsmCodeIndent)+9; // indent+.locals (
            char chAfterComma;
            char *pComma = pszTailSig, *pch;
            while((pComma = strchr(pComma,',')))
            {
                for(pch = pszTailSig, i=0, j=0, k=0; pch < pComma; pch++)
                {
                    if(*pch == '\'') j=1-j;
                    else if(j==0)
                    {
                        if(*pch == '[') i++;
                        else if(*pch == ']') i--;
                        else if(strncmp(pch,LTN(),strlen(LTN()))==0) k++;
                        else if(strncmp(pch,GTN(),strlen(GTN()))==0) k--;
                    }
                }
                pComma++;
                if((i==0)&&(k==0)&&(j==0)) // no brackets or all opened/closed
                {
                    chAfterComma = *pComma;
                    *pComma = 0;
                    printLine(GUICookie,pszTailSig);
                    *pComma = chAfterComma;
                    for(i=0; i<indent; i++) pszTailSig[i] = ' ';
                    strcpy_s(&pszTailSig[indent],strlen(pComma)+1,pComma);
                    pComma = pszTailSig;
                }
            }
        }
        printLine(GUICookie, pszTailSig);
    }
}
void LoadScope(ISymUnmanagedScope       *pIScope,
               DynamicArray<LexScope>   *pdaScope,
               ULONG                    *pulScopes)
{
    ULONG32 dummy;
    ULONG32 ulVars;
    ISymUnmanagedScope**    ppChildScope = new ISymUnmanagedScope*[4096];
    ULONG32              ulChildren;
    unsigned            i;

    pIScope->GetLocalCount(&ulVars);

    if(ulVars)
    {
        pIScope->GetStartOffset(&dummy);
        (*pdaScope)[*pulScopes].dwStart = dummy;
        pIScope->GetEndOffset(&dummy);
        (*pdaScope)[*pulScopes].dwEnd = dummy;
        (*pdaScope)[*pulScopes].dwZOrder = 0;
        (*pdaScope)[*pulScopes].pISymScope = pIScope;
        pIScope->AddRef();
        (*pulScopes)++;
    }
    if(SUCCEEDED(pIScope->GetChildren(4096,&ulChildren,ppChildScope)))
    {
        for(i = 0; i < ulChildren; i++)
        {
            if(ppChildScope[i]) {
                LoadScope(ppChildScope[i],pdaScope,pulScopes);
                ppChildScope[i]->Release();
            }
        }
    }
    VDELETE(ppChildScope);

}
//#define SHOW_LEXICAL_SCOPES
void OpenScope(ISymUnmanagedScope               *pIScope,
               ParamDescriptor                  *pLV,
               ULONG                            ulLocals)
{
    ULONG32 dummy;
    ULONG32 ulVars;
    ISymUnmanagedVariable** pVars = NULL;

    pIScope->GetLocalCount(&ulVars);

    ULONG32 ulNameLen;
#ifdef  SHOW_LEXICAL_SCOPES
    for(unsigned jj = 0; jj < ulLocals; jj++) pLV[jj].attr = 0xFFFFFFFF;
#endif
    if(ulVars)
    {
        pVars = new ISymUnmanagedVariable*[ulVars+4];
        memset(pVars,0,sizeof(PVOID)*(ulVars+4));
        pIScope->GetLocals(ulVars+4,&dummy,pVars);

        WCHAR* wzName = wzUniBuf;
        char*  szName = NULL;
        for(ULONG ilv = 0; ilv < ulVars; ilv++)
        {
            if(pVars[ilv])
            {
                if(SUCCEEDED(pVars[ilv]->GetName(UNIBUF_SIZE/2,&ulNameLen,wzName)))
                {
                    // get the local var slot number:
                    pVars[ilv]->GetAddressField1(&dummy);
                    szName = UnicodeToUtf(wzName);
#ifndef  SHOW_LEXICAL_SCOPES
                    for(ULONG32 j=0; j<ulLocals; j++)
                    {
                        if(j == dummy) continue;
                        if((pLV[dummy].name!=NULL)&&(!strcmp(szName,pLV[j].name)))
                        {
                            sprintf_s(szName,UNIBUF_SIZE/2,"V_%d",dummy);
                            break;
                        }
                    }
#endif
                    ulNameLen = (ULONG32) strlen(szName);
                    if((pLV[dummy].name==NULL)||(ulNameLen > (ULONG32)strlen(pLV[dummy].name)))
                    {
                        VDELETE(pLV[dummy].name);
                        pLV[dummy].name = new char[ulNameLen+1];
                    }
                    if(pLV[dummy].name != NULL)
                        strcpy_s(pLV[dummy].name,ulNameLen+1,szName);
                }
                pLV[dummy].attr = dummy;
                pVars[ilv]->Release();
            }
        }
        VDELETE(pVars);
    }
}

char* DumpUnicodeString(void* GUICookie,
                        __inout __nullterminated char* szString,
                        __in_ecount(cbString) WCHAR* pszString,
                        ULONG cbString, 
                        bool SwapString )
{
    unsigned     i,L;
    char*   szStr=NULL, *szRet = NULL;
#if defined(ALIGN_ACCESS) || BIGENDIAN
    WCHAR* pszStringCopy = NULL;
    // Make a copy if the string is unaligned or we have to modify it
#if !BIGENDIAN 
    if((size_t)pszString & (sizeof(WCHAR)-1))
#endif
    {
        L = (cbString+1)*sizeof(WCHAR);
        pszStringCopy = new WCHAR[cbString+1];
        memcpy(pszStringCopy, pszString, L);
        pszString=pszStringCopy;
    }
#endif

#if BIGENDIAN
    if (SwapString)
        SwapStringLength(pszString, cbString);
#endif

    // first, check for embedded zeros:
    for(i=0; i < cbString; i++)
    {
        if(pszString[i] == 0) goto DumpAsByteArray;
    }
    szStr = new char[cbString*3 + 5];
    memset(szStr,0,cbString*3 + 5);

    WszWideCharToMultiByte(CP_UTF8,0,pszString,cbString,&szStr[0],cbString*3,NULL,NULL);
    L = (unsigned)strlen(szStr);

    szStr[L] = 0;
    for(i=0; i < L; i++)
    {
        if((!isprint(((BYTE)(szStr[i]))))&&(szStr[i]!='\t')&&(szStr[i]!='\n')&&(szStr[i]!='\r')) break;
    }

    if(i == L)
    {
        szRet = DumpQString(GUICookie,szStr,g_szAsmCodeIndent, 50);
    }
    else
    {
DumpAsByteArray:
        strcat_s(szString,SZSTRING_SIZE,KEYWORD("bytearray"));
        strcat_s(szString,SZSTRING_SIZE," (");

#if BIGENDIAN
	SwapStringLength(pszString, cbString);
#endif
        DumpByteArray(szString,(BYTE*)pszString,cbString*sizeof(WCHAR),GUICookie);
        szRet = &szString[strlen(szString)];
    }
    if(szStr) VDELETE(szStr);
#if defined(ALIGN_ACCESS) || BIGENDIAN
    if(pszStringCopy) VDELETE(pszStringCopy);
#endif
    return szRet;
}

// helper to avoid mixing of SEH and stack objects with destructors
BOOL SourceLinesHelper(void *GUICookie, LineCodeDescr* pLCD, __in __nullterminated WCHAR* pFileName, UINT nSize)
{
    BOOL fRet = FALSE;
    //char szString[4096];
    if(pLCD->FileToken == 0) return FALSE;
    PAL_TRY {
        GUID guidLang={0},guidLangVendor={0},guidDoc={0};
        WCHAR wzLang[64],wzVendor[64],wzDoc[64];
        ULONG32 k;
        if(pLCD->FileToken != ulWasFileToken)
        {
            ((ISymUnmanagedDocument*)(pLCD->FileToken))->GetLanguage(&guidLang);
            ((ISymUnmanagedDocument*)(pLCD->FileToken))->GetLanguageVendor(&guidLangVendor);
            ((ISymUnmanagedDocument*)(pLCD->FileToken))->GetDocumentType(&guidDoc);
            if(memcmp(&guidLang,&guidWasLang,sizeof(GUID))
                ||memcmp(&guidLangVendor,&guidWasLangVendor,sizeof(GUID))
                ||memcmp(&guidDoc,&guidWasDoc,sizeof(GUID)))
            {
                GuidToLPWSTR(guidLang,wzLang,64);
                GuidToLPWSTR(guidLangVendor,wzVendor,64);
                GuidToLPWSTR(guidDoc,wzDoc,64);
                sprintf_s(szString,SZSTRING_SIZE,"%s%s '%ls', '%ls', '%ls'", g_szAsmCodeIndent,KEYWORD(".language"),
                    wzLang,wzVendor,wzDoc);
                printLine(GUICookie,szString);
                memcpy(&guidWasLang,&guidLang,sizeof(GUID));
                memcpy(&guidWasLangVendor,&guidLangVendor,sizeof(GUID));
                memcpy(&guidWasDoc,&guidDoc,sizeof(GUID));
            }

            /*
            BOOL fHasEmbeddedSource=FALSE;
            ((ISymUnmanagedDocument*)(pLCD->FileToken))->HasEmbeddedSource(&fHasEmbeddedSource);
            sprintf(szString,"%s// PDB has %sembedded source",g_szAsmCodeIndent,
                fHasEmbeddedSource ? "" : "no ");
            printLine(GUICookie,szString);
            */
        }
        ((ISymUnmanagedDocument*)(pLCD->FileToken))->GetURL(nSize,&k,pFileName);
        ulWasFileToken = pLCD->FileToken;
        fRet = TRUE;
    } PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
        sprintf_s(szString,SZSTRING_SIZE,RstrUTF(IDS_ERRORREOPENINGFILE),pLCD->FileToken);
        printError(GUICookie, szString);
    }
    PAL_ENDTRY

    return fRet;
}

BOOL Disassemble(IMDInternalImport *pImport, BYTE *ILHeader, void *GUICookie, mdToken FuncToken, ParamDescriptor* pszArgname, ULONG ulArgs)
{
    DWORD      PC;
    BOOL    fNeedNewLine = FALSE;
    //char    szString[4096];
    char*   szptr;
    BYTE*   pCode = NULL;
    BOOL    fTryInCode;

    COR_ILMETHOD_DECODER method((COR_ILMETHOD*) ILHeader);

    {
        pCode = const_cast<BYTE*>(method.Code);
    }

    sprintf_s(szString,SZSTRING_SIZE, RstrUTF(IDS_E_CODESIZE),g_szAsmCodeIndent, method.GetCodeSize(), method.GetCodeSize());
    printLine(GUICookie, szString);

    if(method.GetCodeSize() == 0) return TRUE;

    sprintf_s(szString,SZSTRING_SIZE, "%s%s  %d",g_szAsmCodeIndent, KEYWORD(".maxstack"),method.GetMaxStack());
    printLine(GUICookie, szString);

    //------------ Source lines display ---------------------------------
    ULONG32 ulLines =0;
    LineCodeDescr* LineCode = NULL;
    BOOL fShowSource = FALSE;
    BOOL fInsertSourceLines = FALSE;
    LineCodeDescr* pLCD = NULL;
    ParamDescriptor* pszLVname = NULL;
    ULONG ulVars=0;
    char szVarPrefix[64];
    // scope handling:
    DynamicArray<LexScope>          daScope;
    ULONG                           ulScopes=0;
    DWORD                           dwScopeZOrder = 0;
    ISymUnmanagedScope*             pRootScope = NULL;
    ISymUnmanagedMethod*            pSymMethod = NULL;
    char szFileName[2048];
    ISymUnmanagedDocument*          pMethodDoc[2] = {NULL,NULL};
    ULONG32                         ulMethodLine[2];
    ULONG32                         ulMethodCol[2];
    BOOL                            fHasRangeInfo = FALSE;

    strcpy_s(szVarPrefix,64,"V0");
    if(g_pSymReader)
    {
        g_pSymReader->GetMethod(FuncToken,&pSymMethod);
        if(g_fShowSource || g_fInsertSourceLines)
        {
            if(pSymMethod)
            {
                unsigned ulActualLines=0; // VS compilers produce "Hidden" line numbers, don't count them
                HRESULT hrr;
                hrr=pSymMethod->GetSourceStartEnd(pMethodDoc,ulMethodLine,ulMethodCol,&fHasRangeInfo);
                pSymMethod->GetSequencePointCount(&ulLines);
                LineCode = new LineCodeDescr[ulLines+2];
                if(LineCode)
                {
                    pLCD = &LineCode[0];
                    if(fHasRangeInfo)
                    {
                        //printLine(GUICookie,"// Has source range info");
                        pLCD->Line = ulMethodLine[0];
                        pLCD->Column = ulMethodCol[0];
                        pLCD->PC = 0;
                        pLCD->FileToken = (ULONG_PTR)pMethodDoc[0];
                        ulActualLines++;
                        pLCD++;
                    }
                    if(ulLines)
                    {
                        ULONG32 *offsets=new ULONG32[ulLines], *lines=new ULONG32[ulLines], *columns=new ULONG32[ulLines];
                        ULONG32 *endlines=new ULONG32[ulLines], *endcolumns=new ULONG32[ulLines];
                        ISymUnmanagedDocument** docs = (ISymUnmanagedDocument**)(new PVOID[ulLines]);
                        ULONG32 actualCount;
                        pSymMethod->GetSequencePoints(ulLines,&actualCount, offsets,docs,lines,columns, endlines, endcolumns);
                        for(ULONG i = 0; i < ulLines; i++)
                        {
                            if(lines[i] < 0xFEEFED)
                            {
                                pLCD->Line = lines[i];
                                pLCD->Column = columns[i];
                                pLCD->LineEnd = endlines[i];
                                pLCD->ColumnEnd = endcolumns[i];
                                pLCD->PC = offsets[i];
                                pLCD->FileToken = (ULONG_PTR)docs[i];
                                ulActualLines++;
                                pLCD++;
                            }
                        }
                        VDELETE(offsets);
                        VDELETE(lines);
                        VDELETE(columns);
                        VDELETE(endlines);
                        VDELETE(endcolumns);
                        VDELETE(docs);
                    } // end if(ulLines)
                    if(fHasRangeInfo)
                    {
                        pLCD->Line = ulMethodLine[1];
                        pLCD->Column = ulMethodCol[1];
                        pLCD->PC = method.GetCodeSize();
                        pLCD->FileToken = (ULONG_PTR)pMethodDoc[1];
                        ulActualLines++;
                        pLCD++;
                    }

                    ulLines = ulActualLines;
                    qsort(LineCode,ulLines,sizeof(LineCodeDescr),cmpLineCode);
                    fShowSource = g_fShowSource;
                    fInsertSourceLines = g_fInsertSourceLines;
                    pLCD = &LineCode[0];
                } // end if(LineCode)
            } //end if (pSymMethod)
        }//end if(g_fShowSource)
        if (method.GetLocalVarSigTok())
        {
            // first, get the real number of local vars from signature
            DWORD           cbSigLen;
            PCCOR_SIGNATURE pComSig;
            mdSignature mdLocalVarSigTok = method.GetLocalVarSigTok();
            if((TypeFromToken(mdLocalVarSigTok) != mdtSignature) ||
                (!pImport->IsValidToken(mdLocalVarSigTok)) || (RidFromToken(mdLocalVarSigTok)==0))
            {
                sprintf_s(szString,SZSTRING_SIZE,RstrUTF(IDS_E_BOGUSLVSIG),mdLocalVarSigTok);
                printError(GUICookie,szString);
                return FALSE;
            }
            pComSig = pImport->GetSigFromToken(mdLocalVarSigTok, &cbSigLen);
            _ASSERTE(*pComSig == IMAGE_CEE_CS_CALLCONV_LOCAL_SIG);
            pComSig++;

            ULONG ulVarsInSig = CorSigUncompressData(pComSig);
            if(pSymMethod)  pSymMethod->GetRootScope(&pRootScope);
            else pRootScope = NULL;

            if(pRootScope)
            {
                ulVars = ulVarsInSig;
            }
            else ulVars = 0;
            if(ulVars)
            {
                ULONG ilvs;
                pszLVname = new ParamDescriptor[ulVars];
                memset(pszLVname,0,ulVars*sizeof(ParamDescriptor));
                for(ilvs = 0; ilvs < ulVars; ilvs++)
                {
                    pszLVname[ilvs].name = new char[16];
                    sprintf_s(pszLVname[ilvs].name,16,"V_%d",ilvs);
                    pszLVname[ilvs].attr = ilvs;
                }
                LoadScope(pRootScope,&daScope,&ulScopes);
                qsort(&daScope[0],ulScopes,sizeof(LexScope),cmpLexScope);
                OpenScope(pRootScope,pszLVname,ulVars);
                sprintf_s(szVarPrefix,64,"@%d0",(size_t)pszLVname);

#ifndef SHOW_LEXICAL_SCOPES
                for(unsigned jjj = 0; jjj < ulScopes; jjj++)
                {
                    OpenScope(daScope[jjj].pISymScope,pszLVname,ulVars);
                    daScope[jjj].pISymScope->Release();
                }
                ulScopes = 0;
#endif
            } //end if(ulLVScount)
            if(pRootScope) pRootScope->Release();
        } //end if (method.LocalVarSigTok)
    } //end if(g_pDebugImport)
    //-------------------------------------------------------------------
    g_tkRefUser = FuncToken;
    DumpLocals(pImport,&method, szVarPrefix, GUICookie);
    g_tkRefUser = 0;


    PC = 0;
    fTryInCode = enumEHInfo(method.EH, pImport, method.GetCodeSize());
    COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfoToPutNext = NULL;
    while (PC < method.GetCodeSize())
    {
        DWORD   Len;
        DWORD   i,ii;
        OPCODE  instr;
        char sz[1024];

        instr = DecodeOpcode(&pCode[PC], &Len);
        if (instr == CEE_COUNT)
        {
            sprintf_s(szString,SZSTRING_SIZE,RstrUTF(IDS_E_INSTRDECOD), pCode[PC],PC,PC);
            printError(GUICookie, szString);
            /*
            while (PC < method.CodeSize)
            {
                printf("%02x\n", pCode[PC]);
                PC++;
            }
            */
            return FALSE;
        }

        if (fNeedNewLine)
        {
            fNeedNewLine = FALSE;
                printLine(GUICookie,"");
        }

        if(fShowSource || fInsertSourceLines)
        {
            while(PC == pLCD->PC)
            {
                bIsNewFile = FALSE;
                if((pLCD->FileToken != ulWasFileToken) || (pLCD->Line < ulWasLine))
                {
                    WCHAR wzFileName[2048];
                    SourceLinesHelper(GUICookie, pLCD, wzFileName, 2048);
                    bIsNewFile = (wcscmp(wzFileName,wzWasFileName)!=0);
                    if(bIsNewFile||(pLCD->Line < ulWasLine))
                    {
                        wcscpy_s(wzWasFileName,2048,wzFileName);
                        memset(szFileName,0,2048);
                        WszWideCharToMultiByte(CP_UTF8,0,wzFileName,-1,szFileName,2048,NULL,NULL);
                        if(fShowSource)
                        {
                            if(pFile) fclose(pFile);
                            pFile = NULL;
                            if(fopen_s(&pFile,szFileName,"rt") != 0)
                            {
                                char* pch = strrchr(szFileName,'\\');
                                if(pch == NULL) pch = strrchr(szFileName,':');
                                pFile = NULL;
                                if(pch) fopen_s(&pFile,pch+1,"rt");
                            }
                            if(bIsNewFile)
                            {
                                sprintf_s(szString,SZSTRING_SIZE,"// Source File '%s' %s",szFileName,(pFile ? "" : "not found"));
                                printLine(GUICookie, COMMENT(szString));
                            }
                            ulWasLine = 0;
                        }
                    }
                }
                if(fInsertSourceLines)
                {
                    if(bIsNewFile)
                    {
                        char* pszFN = ProperName(szFileName);
                        sprintf_s(szString,SZSTRING_SIZE,(*pszFN == '\'' ? "%s%s %d,%d : %d,%d %s"
                                                         : "%s%s %d,%d : %d,%d '%s'"),
                            g_szAsmCodeIndent,KEYWORD(".line"),pLCD->Line,pLCD->LineEnd,
                                pLCD->Column,pLCD->ColumnEnd,pszFN);
                    }
                    else
                        sprintf_s(szString,SZSTRING_SIZE,"%s%s %d,%d : %d,%d ''",g_szAsmCodeIndent,KEYWORD(".line"),
                                pLCD->Line,pLCD->LineEnd,pLCD->Column,pLCD->ColumnEnd);

                    printLine(GUICookie,szString);
                }

                ULONG k= pLCD->Line;
                if(pFile)
                {
                    for(k = ulWasLine; k < pLCD->Line; k++)
                    {
                        if(NULL==fgets(sz,1024,pFile)) { k--; break;}
                        if((ulWasLine != 0)||(k == (pLCD->Line-1)))
                        {
                            while((sz[strlen(sz)-1]=='\n') || (sz[strlen(sz)-1]=='\r')) sz[strlen(sz)-1]=0;
                            sprintf_s(szString,SZSTRING_SIZE,"//%6.6d: %s",k+1,sz);
                            printLine(GUICookie, COMMENT(szString));
                        }
                    }
                    ulWasLine = k;
                }
                if(pLCD < &LineCode[ulLines-1]) pLCD++;
                else { fShowSource = FALSE; break; }
            }
        }
        if(fTryInCode)
        {
            g_tkRefUser = FuncToken;
            dumpOneEHInfo(ehInfoToPutNext,pImport,GUICookie); //doesn't do anything if ehInfoToPutNext == NULL
            g_tkRefUser = 0;
            ehInfoToPutNext = NULL;

            for(ii = g_ehCount; ii > 0; ii--)
            {
                i = g_ehCount - ii;
                DWORD   theEnd = g_ehInfo[i].GetHandlerOffset()+g_ehInfo[i].GetHandlerLength();
                if(g_ehInfo[i].GetFlags() & PUT_INTO_CODE)
                {
                    if(PC == theEnd)
                    {
                        // reduce indent, close }
                        g_szAsmCodeIndent[strlen(g_szAsmCodeIndent)-2] = 0;
                        sprintf_s(szString,SZSTRING_SIZE,"%s%s  %s",g_szAsmCodeIndent,UNSCOPE(),COMMENT("// end handler"));
                        printLine(GUICookie,szString);
                        if(g_fShowBytes)
                        {
                            BYTE    *pb = (BYTE *)&g_ehInfo[i];
                            char*   psz;
                            size_t j;
                            CorExceptionFlag Flg = g_ehInfo[i].GetFlags();
                            sprintf_s(szString,SZSTRING_SIZE,"%s// HEX:",g_szAsmCodeIndent);
                            g_ehInfo[i].SetFlags((CorExceptionFlag)((int)Flg & ~SEH_NEW_PUT_MASK));
                            psz = &szString[strlen(szString)];
                            for(j = 0; j < sizeof(COR_ILMETHOD_SECT_EH_CLAUSE_FAT); j++)
                                psz += sprintf_s(psz,SZSTRING_REMAINING_SIZE(psz)," %2.2X",*pb++);
                            printLine(GUICookie, COMMENT(szString));
                            g_ehInfo[i].SetFlags(Flg);
                        }
                        g_ehInfo[i].SetTryOffset(0xFF000000);
                        g_ehInfo[i].SetHandlerOffset(0xFF000000);
                    }
                }
                else
                {
                    DWORD theTryEnd = g_ehInfo[i].GetTryOffset()+g_ehInfo[i].GetTryLength();
                    if(theTryEnd > theEnd) theEnd = theTryEnd; // try block after the handler
                    if(PC == theEnd) ehInfoToPutNext = &g_ehInfo[i];
                }
            }

            for(i = 0; i < g_ehCount; i++)
            {
                if(g_ehInfo[i].GetFlags() & PUT_INTO_CODE)
                {
                    if(g_ehInfo[i].GetFlags() & NEW_TRY_BLOCK)
                    {
                        if(PC == g_ehInfo[i].GetTryOffset()+g_ehInfo[i].GetTryLength())
                        {
                            // reduce indent, close }
                            g_szAsmCodeIndent[strlen(g_szAsmCodeIndent)-2] = 0;
                            sprintf_s(szString,SZSTRING_SIZE,"%s%s  %s",g_szAsmCodeIndent,UNSCOPE(),COMMENT("// end .try"));
                            printLine(GUICookie,szString);
                        }
                        if(PC == g_ehInfo[i].GetTryOffset())
                        {
                            // Put try, {, increase indent
                            sprintf_s(szString,SZSTRING_SIZE,"%s%s",g_szAsmCodeIndent,KEYWORD(".try"));
                            printLine(GUICookie,szString);
                            sprintf_s(szString,SZSTRING_SIZE,"%s%s",g_szAsmCodeIndent,SCOPE());
                            printLine(GUICookie,szString);
                            strcat_s(g_szAsmCodeIndent,MAX_MEMBER_LENGTH,"  ");
                        }
                    }
                    if(PC == g_ehInfo[i].GetHandlerOffset())
                    {
                        // Dump catch or finally clause, {, increase indent
                        DWORD dwFlags = g_ehInfo[i].GetFlags();
                        if (dwFlags & COR_ILEXCEPTION_CLAUSE_FAULT)
                            sprintf_s(szString,SZSTRING_SIZE, "%s%s", g_szAsmCodeIndent, KEYWORD("fault"));
                        else if (dwFlags & COR_ILEXCEPTION_CLAUSE_FINALLY || IsNilToken(g_ehInfo[i].GetClassToken()))
                            sprintf_s(szString,SZSTRING_SIZE, "%s%s", g_szAsmCodeIndent,KEYWORD("finally"));
                        else
                        {
                            CQuickBytes out;
                            sprintf_s(szString,SZSTRING_SIZE, "%s%s %s ",g_szAsmCodeIndent,KEYWORD("catch"),
                                PrettyPrintClass(&out, g_ehInfo[i].GetClassToken(), pImport));
                            REGISTER_REF(FuncToken,g_ehInfo[i].GetClassToken());
                        }
                        printLine(GUICookie,szString);
                        sprintf_s(szString,SZSTRING_SIZE,"%s%s",g_szAsmCodeIndent,SCOPE());
                        printLine(GUICookie,szString);
                        strcat_s(g_szAsmCodeIndent,MAX_MEMBER_LENGTH,"  ");
                    }
                } // end if(g_ehInfo[i].Flags & PUT_INTO_CODE)
            } // end for(i<g_ehCount)
        } // end if(fTryInCode)
        //----------------- lexical scope handling -----------------------------
        if(ulScopes) // non-zero only if local var info present
        {
            for(i=0; i < ulScopes; i++)
            {
                if(daScope[i].pISymScope != pRootScope)
                {
                    if(daScope[i].IsOpen())
                    {
                        if(!daScope[i].Covers(PC))
                        {
                            g_szAsmCodeIndent[strlen(g_szAsmCodeIndent)-2] = 0;
                            sprintf_s(szString,SZSTRING_SIZE,"%s%s",g_szAsmCodeIndent,UNSCOPE());
                            printLine(GUICookie,szString);
                            daScope[i].dwZOrder = 0;
                            dwScopeZOrder--;
                            if(PC > daScope[i].dwEnd)
                            {
                                sprintf_s(szString,SZSTRING_SIZE,COMMENT("%s// PDB ERROR: scope end (0x%8.8X) is not opcode-aligned"),
                                        g_szAsmCodeIndent,daScope[i].dwEnd);
                                printLine(GUICookie,szString);
                            }
                        } // end if(!daScope[i].Covers(PC))
                    }
                    else // i.e., if scope is not open
                    {
                        if(daScope[i].Covers(PC))
                        {
                            if(g_fShowBytes)
                                sprintf_s(szString,SZSTRING_SIZE,"%s%s // 0x%8.8X - 0x%8.8X",g_szAsmCodeIndent,SCOPE(),
                                        daScope[i].dwStart,daScope[i].dwEnd);
                            else
                                sprintf_s(szString,SZSTRING_SIZE,"%s%s",g_szAsmCodeIndent,SCOPE());
                            printLine(GUICookie,szString);
                            strcat_s(g_szAsmCodeIndent,MAX_MEMBER_LENGTH,"  ");
                            OpenScope(daScope[i].pISymScope,pszLVname,ulVars);
                            DumpLocals(pImport,&method, szVarPrefix, GUICookie);
                            dwScopeZOrder++;
                            daScope[i].dwZOrder = dwScopeZOrder;
                            if(PC > daScope[i].dwStart)
                            {
                                sprintf_s(szString,SZSTRING_SIZE,COMMENT("%s// PDB ERROR: scope start (0x%8.8X) is not opcode-aligned"),
                                        g_szAsmCodeIndent,daScope[i].dwStart);
                                printLine(GUICookie,szString);
                            }
                        } // end if(daScope[i].Covers(PC))
                    }  // end if(daScope[i].IsOpen()) -- else
                } // end if(daScope[i].pISymScope != pRootScope)
            } // end for(i=0; i < ulScopes; i++)
        } // end if(ulScopes)


        szptr = &szString[0];
        szptr+=sprintf_s(szptr,SZSTRING_SIZE,"%sIL_%04x:  ",g_szAsmCodeIndent, PC);
        if(g_fShowBytes)
        {
            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%s/* ",COMMENT((char*)0));
            for(i=0; i<Len; i++) szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%2.2X",pCode[PC+i]);
            for(; i<2; i++) szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"  "); // 2 is max.opcode length
            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr)," | ");
        }

        PC += Len;
        Len = 0;

        char *pszInstrName = KEYWORD(OpcodeInfo[instr].pszName);

        switch (OpcodeInfo[instr].Type)
        {
            DWORD tk;
            DWORD tkType;
            default:
            {
                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),RstrUTF(IDS_E_INSTRTYPE),
                    OpcodeInfo[instr].Type,instr);
                printLine(GUICookie, szString);
                return FALSE;
            }

#define PadTheString    { if(Len < 16) szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-*s", (16-Len), ""); \
            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr)," */%s ",COMMENT((char*)-1)); }

            case InlineNone:
            {
                if(g_fShowBytes)
                    PadTheString;

                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), pszInstrName);

                switch (instr)
                {
                    case CEE_RET:
                    case CEE_THROW:
                        fNeedNewLine = TRUE;
                        break;
                    default:
                        break;
                }

                break;
            }

            case ShortInlineI:
            case ShortInlineVar:
            {
                unsigned char  ch= pCode[PC];
                short sh = OpcodeInfo[instr].Type==ShortInlineVar ? ch : (ch > 127 ? -(256-ch) : ch);
                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X ", ch);
                    Len += 3;
                    PadTheString;
                }
                switch(instr)
                {
                    case CEE_LDARG_S:
                    case CEE_LDARGA_S:
                    case CEE_STARG_S:
                        if(g_fThisIsInstanceMethod &&(ch==0))
                        { // instance methods have arg0="this", do not label it!
                            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %d", pszInstrName, ch);
                        }
                        else
                        {
                            if(pszArgname)
                            {
                                unsigned char ch1 = g_fThisIsInstanceMethod ? ch-1 : ch;
                                if(ch1 < ulArgs)
                                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%-10s %s",pszInstrName,
                                                    ProperName(pszArgname[ch1].name));
                                else
                                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),ERRORMSG(RstrUTF(IDS_E_ARGINDEX)),pszInstrName, ch,ulArgs);
                            }
                            else szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s A_%d",pszInstrName, ch);
                        }
                        break;

                    case CEE_LDLOC_S:
                    case CEE_LDLOCA_S:
                    case CEE_STLOC_S:
                        if(pszLVname)
                        {
                            if(ch < ulVars) szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %s", pszInstrName,
                                ProperName(pszLVname[ch].name));
                            else
                                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),ERRORMSG(RstrUTF(IDS_E_LVINDEX)),pszInstrName, ch, ulVars);
                        }
                        else szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s V_%d",pszInstrName, ch);
                        break;

                    default:
                        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %d", pszInstrName,sh);
                }

                PC++;
                break;
            }

            case InlineVar:
            {
                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X%2.2X ", pCode[PC], pCode[PC+1]);
                    Len += 5;
                    PadTheString;
                }

                USHORT v = pCode[PC] + (pCode[PC+1] << 8);
                long l = OpcodeInfo[instr].Type==InlineVar ? v : (v > 0x7FFF ? -(0x10000 - v) : v);

                switch(instr)
                {
                    case CEE_LDARGA:
                    case CEE_LDARG:
                    case CEE_STARG:
                        if(g_fThisIsInstanceMethod &&(v==0))
                        { // instance methods have arg0="this", do not label it!
                            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %d", pszInstrName, v);
                        }
                        else
                        {
                            if(pszArgname)
                            {
                                USHORT v1 = g_fThisIsInstanceMethod ? v-1 : v;
                                if(v1 < ulArgs)
                                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%-10s %s",pszInstrName,
                                                    ProperName(pszArgname[v1].name));
                                else
                                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),ERRORMSG(RstrUTF(IDS_E_ARGINDEX)),pszInstrName, v,ulArgs);
                            }
                            else szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s A_%d",pszInstrName, v);
                        }
                        break;

                    case CEE_LDLOCA:
                    case CEE_LDLOC:
                    case CEE_STLOC:
                        if(pszLVname)
                        {
                            if(v < ulVars)  szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %s", pszInstrName,
                                ProperName(pszLVname[v].name));
                            else
                                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),ERRORMSG(RstrUTF(IDS_E_LVINDEX)),pszInstrName, v,ulVars);
                        }
                        else szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s V_%d",pszInstrName, v);
                        break;

                    default:
                        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %d", pszInstrName, l);
                        break;
                }
                PC += 2;
                break;
            }

            case InlineI:
            case InlineRVA:
            {
                DWORD v = pCode[PC] + (pCode[PC+1] << 8) + (pCode[PC+2] << 16) + (pCode[PC+3] << 24);
                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X%2.2X%2.2X%2.2X ", pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                    Len += 9;
                    PadTheString;
                }
                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s 0x%x", pszInstrName, v);
                PC += 4;
                break;
            }

            case InlineI8:
            {
                __int64 v = (__int64) pCode[PC] +
                            (((__int64) pCode[PC+1]) << 8) +
                            (((__int64) pCode[PC+2]) << 16) +
                            (((__int64) pCode[PC+3]) << 24) +
                            (((__int64) pCode[PC+4]) << 32) +
                            (((__int64) pCode[PC+5]) << 40) +
                            (((__int64) pCode[PC+6]) << 48) +
                            (((__int64) pCode[PC+7]) << 56);

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),
                        "%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
                        pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3],
                        pCode[PC+4], pCode[PC+5], pCode[PC+6], pCode[PC+7]);
                    Len += 8*2;
                    PadTheString;
                }

                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s 0x%I64x", pszInstrName, v);
                PC += 8;
                break;
            }

            case ShortInlineR:
            {
                __int32 v = (__int32) pCode[PC] +
                            (((__int32) pCode[PC+1]) << 8) +
                            (((__int32) pCode[PC+2]) << 16) +
                            (((__int32) pCode[PC+3]) << 24);

                float f = (float&)v;

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X%2.2X%2.2X%2.2X ", pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                    Len += 9;
                    PadTheString;
                }

                char szf[32];
                if(f==0.0)
                    strcpy_s(szf,32,((v>>24)==0)? "0.0" : "-0.0");
                else
                    _gcvt_s(szf,32,(double)f, 8);
                float fd = (float)atof(szf);
                // Must compare as underlying bytes, not floating point otherwise optmizier will
                // try to enregister and comapre 80-bit precision number with 32-bit precision number!!!!
                if(((__int32&)fd == v)&&(strchr(szf,'#') == NULL))
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %s", pszInstrName, szf);
                else
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s (%2.2X %2.2X %2.2X %2.2X)",
                        pszInstrName, pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                PC += 4;
                break;
            }

            case InlineR:
            {
                __int64 v = (__int64) pCode[PC] +
                            (((__int64) pCode[PC+1]) << 8) +
                            (((__int64) pCode[PC+2]) << 16) +
                            (((__int64) pCode[PC+3]) << 24) +
                            (((__int64) pCode[PC+4]) << 32) +
                            (((__int64) pCode[PC+5]) << 40) +
                            (((__int64) pCode[PC+6]) << 48) +
                            (((__int64) pCode[PC+7]) << 56);

                double d = (double&)v;

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),
                        "%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
                        pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3],
                        pCode[PC+4], pCode[PC+5], pCode[PC+6], pCode[PC+7]);
                    Len += 8*2;
                    PadTheString;
                }
                char szf[32],*pch;
                if(d==0.0)
                    strcpy_s(szf,32,((v>>56)==0)? "0.0" : "-0.0");
                else
                    _gcvt_s(szf,32,d, 17);
                double df = strtod(szf, &pch); //atof(szf);
                // Must compare as underlying bytes, not floating point otherwise optmizier will
                // try to enregister and comapre 80-bit precision number with 64-bit precision number!!!!
                if(((__int64&)df == v)&&(strchr(szf,'#') == NULL))
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %s", pszInstrName, szf);
                else
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s (%2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X)",
                        pszInstrName, pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3],
                        pCode[PC+4], pCode[PC+5], pCode[PC+6], pCode[PC+7]);
                PC += 8;
                break;
            }

            case ShortInlineBrTarget:
            {
                char offset = (char) pCode[PC];
                long dest = (PC + 1) + (long) offset;

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X ", pCode[PC]);
                    Len += 3;
                    PadTheString;
                }
                PC++;
                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s IL_%04x", pszInstrName, dest);

                fNeedNewLine = TRUE;
                break;
            }

            case InlineBrTarget:
            {
                long offset = pCode[PC] + (pCode[PC+1] << 8) + (pCode[PC+2] << 16) + (pCode[PC+3] << 24);
                long dest = (PC + 4) + (long) offset;

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X%2.2X%2.2X%2.2X ", pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                    Len += 9;
                    PadTheString;
                }
                PC += 4;
                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s IL_%04x", pszInstrName, dest);

                fNeedNewLine = TRUE;
                break;
            }

            case InlineSwitch:
            {
                DWORD cases = pCode[PC] + (pCode[PC+1] << 8) + (pCode[PC+2] << 16) + (pCode[PC+3] << 24);

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X%2.2X%2.2X%2.2X ", pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                    Len += 9;
                    PadTheString;
                }
                if(cases) szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s ( ", pszInstrName);
                else szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s ( )", pszInstrName);
                printLine(GUICookie, szString);
                PC += 4;
                DWORD PC_nextInstr = PC + 4 * cases;
                for (i = 0; i < cases; i++)
                {
                    long offset = pCode[PC] + (pCode[PC+1] << 8) + (pCode[PC+2] << 16) + (pCode[PC+3] << 24);
                    long dest = PC_nextInstr + (long) offset;
                    szptr = &szString[0];
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"%s          ",g_szAsmCodeIndent); //indent+label
                    if(g_fShowBytes)
                    {
                        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"/*      | %2.2X%2.2X%2.2X%2.2X ",         // comment
                            pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                        Len = 9;
                        PadTheString;
                    }

                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),"            IL_%04x%s", dest,(i == cases-1)? ")" : ",");
                    PC += 4;
                    printLine(GUICookie, szString);
                }
                continue;
            }

            case InlinePhi:
            {
                DWORD cases = pCode[PC];
                unsigned short *pus;
                DWORD i;

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X", cases);
                    Len += 2;
                    for(i=0; i < cases*2; i++)
                    {
                        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X", pCode[PC+1+i]);
                        Len += 2;
                    }
                    PadTheString;
                }

                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s", pszInstrName);
                for(i=0, pus=(unsigned short *)(&pCode[PC+1]); i < cases; i++,pus++)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr)," %d",*pus);
                }
                PC += 2 * cases + 1;
                break;
            }

            case InlineString:
            case InlineField:
            case InlineType:
            case InlineTok:
            case InlineMethod:
            {
                tk = pCode[PC] + (pCode[PC+1] << 8) + (pCode[PC+2] << 16) + (pCode[PC+3] << 24);
                tkType = TypeFromToken(tk);

                // Backwards compatible ldstr instruction.
                if (instr == CEE_LDSTR && TypeFromToken(tk) != mdtString)
                {
                    WCHAR *v1 = L"";

                    if(g_fShowBytes)
                    {
                        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X%2.2X%2.2X%2.2X ",
                            pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                        Len += 9;
                        PadTheString;
                    }

                    if(!g_pPELoader->getVAforRVA(tk, (void**) &v1))
                    {
                        char szStr[256];
                        sprintf_s(szStr,256,RstrUTF(IDS_E_SECTHEADER),tk);
                        printLine(GUICookie,szStr);
                    }
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s ", pszInstrName);
                    ConvToLiteral(szptr, v1, 0xFFFF);
                    PC += 4;
                    break;
                }

                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "(%2.2X)%2.2X%2.2X%2.2X ",
                        pCode[PC+3], pCode[PC+2], pCode[PC+1], pCode[PC]);
                    Len += 11;
                    PadTheString;
                }
                PC += 4;

                szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s ", pszInstrName);

                if ((tk & 0xFF000000) == 0)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%#x ", tk);
                    break;
                }
                if(!pImport->IsValidToken(tk))
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),ERRORMSG(" [ERROR: INVALID TOKEN 0x%8.8X] "),tk);
                    break;
                }
                if(OpcodeInfo[instr].Type== InlineTok)
                {
                    switch (tkType)
                    {
                        default:
                            break;

                        case mdtMethodDef:
                            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),KEYWORD("method "));
                            break;

                        case mdtFieldDef:
                            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),KEYWORD("field "));
                            break;

                        case mdtMemberRef:
                            {
                                PCCOR_SIGNATURE typePtr;
                                const char*         pszMemberName;
                                ULONG       cComSig;

                                pszMemberName = pImport->GetNameAndSigOfMemberRef(
                                    tk,
                                    &typePtr,
                                    &cComSig
                                );
                                unsigned callConv = CorSigUncompressData(typePtr);

                                if (isCallConv(callConv, IMAGE_CEE_CS_CALLCONV_FIELD))
                                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),KEYWORD("field "));
                                else
                                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),KEYWORD("method "));
                                break;
                            }
                    }
                }
                PrettyPrintToken(szString, tk, pImport,GUICookie,FuncToken); //TypeDef,TypeRef,TypeSpec,MethodDef,FieldDef,MemberRef,MethodSpec,String
                break;
            }

            case InlineSig:
            {
                if(g_fShowBytes)
                {
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%2.2X%2.2X%2.2X%2.2X ",
                        pCode[PC], pCode[PC+1], pCode[PC+2], pCode[PC+3]);
                    // output the offset and the raw bytes
                    Len += 9;
                    PadTheString;
                }

                // get the signature token
                tk = pCode[PC] + (pCode[PC+1] << 8) + (pCode[PC+2] << 16) + (pCode[PC+3] << 24);
                PC += 4;
                tkType = TypeFromToken(tk);
                if(tkType == mdtSignature)
                {
                    // get the signature from the token
                    DWORD           cbSigLen;
                    PCCOR_SIGNATURE pComSig;
                    CQuickBytes     qbMemberSig;
                    pComSig = pImport->GetSigFromToken(tk, &cbSigLen);

                    qbMemberSig.Shrink(0);
                    const char* pszTailSig = PrettyPrintSig(pComSig, cbSigLen, "", &qbMemberSig, pImport,NULL);
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), "%-10s %s", pszInstrName, pszTailSig);
                    if(g_fDumpTokens) szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr),COMMENT(" /*%08X*/"),tk);
                }
                else
                    szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), ERRORMSG(RstrUTF(IDS_E_BADTOKENTYPE)), pszInstrName, tk);
                break;
            }
        }

        printLine(GUICookie, szString);
    } // end     while (PC < method.CodeSize)

    if(g_ehCount)
    {
        g_tkRefUser = FuncToken;
        if(fTryInCode)
        {
            DWORD   i,ii;
            for(ii = g_ehCount; ii > 0; ii--)
            {
                i = g_ehCount - ii;
                DWORD   theEnd = g_ehInfo[i].GetHandlerOffset()+g_ehInfo[i].GetHandlerLength();
                if(g_ehInfo[i].GetFlags() & PUT_INTO_CODE)
                {
                    if(PC == theEnd)
                    {
                        // reduce indent, close }
                        g_szAsmCodeIndent[strlen(g_szAsmCodeIndent)-2] = 0;
                        sprintf_s(szString,SZSTRING_SIZE,"%s%s  %s",g_szAsmCodeIndent,UNSCOPE(),COMMENT("// end handler"));
                        printLine(GUICookie,szString);
                        if(g_fShowBytes)
                        {
                            BYTE    *pb = (BYTE *)&g_ehInfo[i];
                            char*   psz;
                            size_t j;
                            CorExceptionFlag Flg = g_ehInfo[i].GetFlags();
                            sprintf_s(szString,SZSTRING_SIZE,"%s// HEX:",g_szAsmCodeIndent);
                            g_ehInfo[i].SetFlags((CorExceptionFlag)((int)Flg & ~SEH_NEW_PUT_MASK));
                            psz = &szString[strlen(szString)];
                            for(j = 0; j < sizeof(COR_ILMETHOD_SECT_EH_CLAUSE_FAT); j++)
                                psz += sprintf_s(psz,SZSTRING_REMAINING_SIZE(psz)," %2.2X",*pb++);
                            printLine(GUICookie, COMMENT(szString));
                            g_ehInfo[i].SetFlags(Flg);
                        }
                        g_ehInfo[i].SetTryOffset(0xFF000000);
                        g_ehInfo[i].SetHandlerOffset(0xFF000000);
                    }
                }
            }
            dumpOneEHInfo(ehInfoToPutNext,pImport,GUICookie); //doesn't do anything if ehInfoToPutNext == NULL
        }
        else
        {
            sprintf_s(szString,SZSTRING_SIZE,"%sIL_%04x:  ",g_szAsmCodeIndent, method.GetCodeSize()); //add dummy label
            printLine(GUICookie, szString);
            dumpEHInfo(pImport,GUICookie);
        }
        g_tkRefUser = 0;
    }
    //----------------- lexical scope handling -----------------------------
    if(ulScopes) // non-zero only if local var info present
    {
        for(unsigned i=0; i < ulScopes; i++)
        {
            if(daScope[i].pISymScope != pRootScope)
            {
                if((daScope[i].dwZOrder > 0)&&(PC >= daScope[i].dwEnd))
                {
                    g_szAsmCodeIndent[strlen(g_szAsmCodeIndent)-2] = 0;
                    sprintf_s(szString,SZSTRING_SIZE,"%s%s",g_szAsmCodeIndent,UNSCOPE());
                    printLine(GUICookie,szString);
                    daScope[i].dwZOrder = 0;
                    dwScopeZOrder--;
                    if(PC > daScope[i].dwEnd)
                    {
                        sprintf_s(szString,SZSTRING_SIZE,COMMENT("%s// PDB ERROR: scope end (0x%8.8X) is not opcode-aligned"),
                                g_szAsmCodeIndent,daScope[i].dwEnd);
                        printLine(GUICookie,szString);
                    }
                }
            }
            daScope[i].pISymScope->Release();
        }
    }
    if(dwScopeZOrder != 0)
    {
        sprintf_s(szString,SZSTRING_SIZE,COMMENT("%s// PDB ERROR: %d unclosed lexical scopes, forcing closure."),
                g_szAsmCodeIndent,dwScopeZOrder);
        printLine(GUICookie,szString);
        for(DWORD i=0; i < dwScopeZOrder; i++)
        {
            g_szAsmCodeIndent[strlen(g_szAsmCodeIndent)-2] = 0;
            sprintf_s(szString,SZSTRING_SIZE,"%s%s",g_szAsmCodeIndent,UNSCOPE());
            printLine(GUICookie,szString);
        }
    }
    if(pSymMethod) pSymMethod->Release();
    if(LineCode) VDELETE(LineCode);
    if(pszLVname)
    {
        for(ULONG i = 0; i < ulVars; i++) SDELETE(pszLVname[i].name);
        VDELETE(pszLVname);
    }
//  dumpEHInfo(pImport, GUICookie);

    return TRUE;
}

void SplitSignatureByCommas(__inout __nullterminated char* szString, 
                            __inout __nullterminated char* pszTailSig, 
                            mdToken tk, 
                            size_t indent, 
                            void* GUICookie)
{
    char chAfterComma;
    char *pComma, *pch;
    size_t i,j,k,l;
    if(indent < strlen(pszTailSig))
    {
        pComma = pszTailSig+indent;
        while((pComma = strchr(pComma,',')) != NULL)
        {
            for(pch = pszTailSig, i=0,j=0,k=0,l=0; pch < pComma; pch++)
            {
                if(*pch == '\'') j=1-j;
                else if(j==0)
                {
                    if(*pch == '[') i++;
                    else if(*pch == ']') i--;
                    else if(*pch == '(') k++;
                    else if(*pch == ')') k--;
                    else if(strncmp(pch,LTN(),strlen(LTN()))==0) l++;
                    else if(strncmp(pch,GTN(),strlen(GTN()))==0) l--;
                }
            }
            pComma++;
            if((i==0)&&(j==0)&&(k<=1)&&(l==0)) // no brackets or all opened/closed
            {
                chAfterComma = *pComma;
                *pComma = 0;
                printLine(GUICookie,pszTailSig);
                *pComma = chAfterComma;
                strcpy_s(pszTailSig,strlen(g_szAsmCodeIndent)+1,g_szAsmCodeIndent);
                for(i=(ULONG32)strlen(g_szAsmCodeIndent); i<indent; i++) pszTailSig[i] = ' ';
                strcpy_s(&pszTailSig[indent],strlen(pComma)+1,pComma);
                pComma = pszTailSig;
            }
        }
    }
    pch = szString;
    pch+=sprintf_s(pch,SZSTRING_SIZE,"%s",pszTailSig);
    if(g_fDumpTokens)
        pch+=sprintf_s(pch,SZSTRING_REMAINING_SIZE(pch),COMMENT(" /* %08X */"),tk);
}

//#define TOKEN_SIG
#ifdef TOKEN_SIG
struct TokenSig
{
    size_t    indent;
    char*     str;
    TokenSig() { indent = 0; str = NULL; };
    ~TokenSig() { if(str) VDELETE(str); };
};
struct TokenSigArray
{
    TokenSig*   ptsArray;
    ULONG       ulCount;
    mdToken     tkType;
    TokenSigArray(mdToken tk, IMDInternalImport *pImport)
    {
        HENUMInternal hEnum;
        pImport->EnumAllInit(tk,&hEnum);
        ulCount=pImport->EnumGetCount(&hEnum)+1;
        pImport->EnumClose(&hEnum);
        ptsArray = new TokenSig[ulCount];
        tkType = tk;
    };
    ~TokenSigArray()
    {
        VDELETE(ptsArray);
    }
};

TokenSigArray *TSA_TD = NULL;
TokenSigArray *TSA_TR = NULL;
TokenSigArray *TSA_TS = NULL;
TokenSigArray *TSA_AR = NULL;
TokenSigArray *TSA_MR = NULL;
TokenSigArray *TSA_FD = NULL;
TokenSigArray *TSA_MD = NULL;
TokenSigArray *TSA_ME = NULL;
TokenSigArray *TSA_MS = NULL;

#endif
void TokenSigInit(IMDInternalImport *pImport)
{
#ifdef TOKEN_SIG
    TSA_TD = new TokenSigArray(mdtTypeDef,pImport);
    TSA_TR = new TokenSigArray(mdtTypeRef,pImport);
    TSA_TS = new TokenSigArray(mdtTypeSpec,pImport);
    TSA_AR = new TokenSigArray(mdtAssemblyRef,pImport);
    TSA_MR = new TokenSigArray(mdtModuleRef,pImport);
    TSA_FD = new TokenSigArray(mdtFieldDef,pImport);
    TSA_MD = new TokenSigArray(mdtMethodDef,pImport);
    TSA_ME = new TokenSigArray(mdtMemberRef,pImport);
    TSA_MS = new TokenSigArray(mdtMethodSpec,pImport);
#endif
}
void TokenSigDelete()
{
#ifdef TOKEN_SIG
    SDELETE(TSA_TD);
    SDELETE(TSA_TR);
    SDELETE(TSA_TS);
    SDELETE(TSA_AR);
    SDELETE(TSA_MR);
    SDELETE(TSA_FD);
    SDELETE(TSA_MD);
    SDELETE(TSA_ME);
    SDELETE(TSA_MS);
#endif
}

BOOL IsGenericInst(mdToken tk, IMDInternalImport *pImport)
{
    if(TypeFromToken(tk) == mdtTypeSpec)
    {
        if(pImport->IsValidToken(tk))
        {
            ULONG cSig;
            PCCOR_SIGNATURE sig = pImport->GetSigFromToken(tk, &cSig);
            PCCOR_SIGNATURE sigEnd = sig+cSig;
            while(sig < sigEnd)
            {
                unsigned t = (unsigned)*sig;
                switch(t)
                {
                case ELEMENT_TYPE_PTR:
                case ELEMENT_TYPE_SZARRAY:
                case ELEMENT_TYPE_ARRAY:
                case ELEMENT_TYPE_BYREF:
                    sig++;
                    break;
                default:
                    return (*sig == ELEMENT_TYPE_GENERICINST);
                }
            }
        }
    }
    return FALSE;
}

char* PrettyPrintMemberRef(__inout __nullterminated char* szString, mdToken tk, PCCOR_SIGNATURE pInstSig, ULONG cInstSig,
                           IMDInternalImport *pImport, void* GUICookie, size_t* pIndent,
                           CQuickBytes     *pqbMemberSig)
{
    mdTypeRef       cr;
    const char *    pszMemberName;
    char*           curPos;
    char*           pszTailSig;
    char*           pszOffset;
    PCCOR_SIGNATURE pComSig;
    ULONG       cComSig;
    size_t L, indent;
    mdToken tkVarOwner = g_tkVarOwner;
    DWORD dwAttrs=0xFFFFFFFF; // used only if Parent==MethodDef
    mdToken tkMethodDef = 0; // used only if Parent==MethodDef

    L = strlen(szString);

    pszMemberName = pImport->GetNameAndSigOfMemberRef(
        tk,
        &pComSig,
        &cComSig
    );
    MAKE_NAME_IF_NONE(pszMemberName,tk);

    cr = pImport->GetParentOfMemberRef(tk);
    if(TypeFromToken(cr) == mdtMethodDef) //get the parent's parent
    {
        dwAttrs = pImport->GetMethodDefProps(cr);
        tkMethodDef = cr;

        mdTypeRef cr1;
        if(FAILED(pImport->GetParentToken(cr,&cr1))) cr1 = mdTypeRefNil;
        cr = cr1;
    }
    curPos = &szString[L+1];
    *curPos = 0;
    pqbMemberSig->Shrink(0);
    if(RidFromToken(cr)&&(cr != 0x02000001))
    {
        const char* pszClass = PrettyPrintClass(pqbMemberSig,cr,pImport);
        sprintf_s(curPos,SZSTRING_REMAINING_SIZE(curPos),"%s::",pszClass);
        pqbMemberSig->Shrink(0);
        if(IsGenericInst(cr,pImport)) g_tkVarOwner = 0;
    }
    {
        char* curPos1 = &curPos[strlen(curPos)];
        if(IsMdPrivateScope(dwAttrs))
            sprintf_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),"%s$PST%08X",pszMemberName,tkMethodDef);
        else
            strcpy_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),pszMemberName);
        if(tkMethodDef != 0)
            strcpy_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),JUMPPT(ProperName(curPos1),tkMethodDef));
        else
            strcpy_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),ProperName(curPos1));
    }

    appendStr(pqbMemberSig, szString);
    if (pInstSig)
    {
        CQuickBytes     qbInstSig;
        qbInstSig.Shrink(0);
        PrettyPrintSig(pInstSig, cInstSig, curPos, &qbInstSig, pImport, NULL);
        strcat_s(curPos,SZSTRING_REMAINING_SIZE(curPos), (char*) qbInstSig.Ptr());
    }
    pszTailSig = (char*)PrettyPrintSig(pComSig, cComSig, curPos, pqbMemberSig, pImport,NULL,(pInstSig==NULL));
    pszOffset = strstr(pszTailSig,curPos);
    indent = pszOffset - pszTailSig + strlen(curPos) + 1;
    if(pIndent) *pIndent = indent;
    g_tkVarOwner = tkVarOwner;
    return pszTailSig;
}

char* PrettyPrintMethodDef(__inout __nullterminated char* szString, mdToken tk, PCCOR_SIGNATURE pInstSig, ULONG cInstSig,
                           IMDInternalImport *pImport,void* GUICookie, size_t* pIndent,
                           CQuickBytes     *pqbMemberSig)
{
    HRESULT         hr;
    mdTypeRef       cr;
    const char *    pszMemberName;
    char*           curPos;
    char*           pszTailSig;
    char*           pszOffset;
    DWORD           dwAttrs;
    PCCOR_SIGNATURE pComSig;
    ULONG       cComSig;
    size_t L, indent;
    mdToken tkVarOwner = g_tkVarOwner;
    
    L = strlen(szString);
    pszMemberName = pImport->GetNameOfMethodDef(tk);
    MAKE_NAME_IF_NONE(pszMemberName,tk);
    dwAttrs = pImport->GetMethodDefProps(tk);

    pComSig = pImport->GetSigOfMethodDef(tk, &cComSig);

    hr = pImport->GetParentToken(
        tk,
        &cr
    );
    if (FAILED(hr))
    {
        strcat_s(szString,SZSTRING_SIZE, "??");
        return szString;
    }
    // use the tail as a buffer
    curPos = &szString[L+1];
    *curPos=0;
    pqbMemberSig->Shrink(0);
    if(RidFromToken(cr)&&(cr != 0x02000001))
    {
        const char* pszClass = PrettyPrintClass(pqbMemberSig,cr,pImport);
        sprintf_s(curPos,SZSTRING_REMAINING_SIZE(curPos),"%s::",pszClass);
        pqbMemberSig->Shrink(0);
        if(IsGenericInst(cr,pImport)) g_tkVarOwner = 0;
    }
    {
        char* curPos1 = &curPos[strlen(curPos)];
        if(IsMdPrivateScope(dwAttrs))
            sprintf_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),"%s$PST%08X",pszMemberName,tk);
        else
            strcpy_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),pszMemberName);
        strcpy_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),JUMPPT(ProperName(curPos1),tk));
    }
    appendStr(pqbMemberSig, szString);
    if (pInstSig)
    {
      CQuickBytes     qbInstSig;
      qbInstSig.Shrink(0);
      pszTailSig = (char*)PrettyPrintSig(pInstSig, cInstSig, curPos, &qbInstSig, pImport, NULL);
      strcat_s(curPos,SZSTRING_REMAINING_SIZE(curPos), (char*) qbInstSig.Ptr());
    }

    pszTailSig = (char*)PrettyPrintSig(pComSig, cComSig, curPos, pqbMemberSig, pImport,NULL);
    pszOffset = strstr(pszTailSig,curPos);
    indent = pszOffset - pszTailSig + strlen(curPos) + 1;
    if(pIndent) *pIndent = indent;
    g_tkVarOwner = tkVarOwner;
    return pszTailSig;
}

void PrettyPrintTokenWithSplit(__inout __nullterminated char* szString, mdToken tk, IMDInternalImport *pImport, void* GUICookie)
{
    CQuickBytes     qbMemberSig;
    char*           pszTailSig = NULL;
    size_t L, indent;

    L = strlen(szString);

#ifdef TOKEN_SIG
    TokenSig* SigToken=NULL;
    switch(TypeFromToken(tk))
    {
        case mdtMethodDef:  SigToken = TSA_MD->ptsArray; break;
        case mdtMethodSpec: SigToken = TSA_MS->ptsArray; break;
        case mdtMemberRef:  SigToken = TSA_ME->ptsArray; break;
        default: return; // other token types aren't served
    }
    if(SigToken[RidFromToken(tk)].str)
    {
        qbMemberSig.Shrink(0);
        appendStr(&qbMemberSig, szString);
        appendStr(&qbMemberSig, SigToken[RidFromToken(tk)].str);
        pszTailSig = asString(&qbMemberSig);
        indent = SigToken[RidFromToken(tk)].indent + L;
    }
    else
#endif
    {
        switch(TypeFromToken(tk))
        {
            case mdtMethodDef:
                pszTailSig = PrettyPrintMethodDef(szString,tk,NULL,0,pImport,GUICookie,&indent,&qbMemberSig);
                break;
            case mdtMemberRef:
                pszTailSig = PrettyPrintMemberRef(szString,tk,NULL,0,pImport,GUICookie,&indent,&qbMemberSig);
                break;
            case mdtMethodSpec:
                {
                    mdToken         meth=0;
                    PCCOR_SIGNATURE pSig=NULL;
                    ULONG       cSig=0;
                    mdToken tkMVarOwner = g_tkMVarOwner;
                    g_tkMVarOwner = 0;
                    pImport->GetMethodSpecProps(tk, &meth, &pSig, &cSig);
                    pszTailSig = (TypeFromToken(meth) == mdtMethodDef) ?
                            PrettyPrintMethodDef(szString,meth,pSig,cSig,pImport,GUICookie,&indent,&qbMemberSig) :
                            PrettyPrintMemberRef(szString,meth,pSig,cSig,pImport,GUICookie,&indent,&qbMemberSig);
                    g_tkMVarOwner = tkMVarOwner;
                }
                break;
            default: return;
        }

#ifdef TOKEN_SIG
        SigToken[RidFromToken(tk)].str = new char[strlen(pszTailSig+L)+1];
        if(SigToken[RidFromToken(tk)].str)
        {
            strcpy_s(SigToken[RidFromToken(tk)].str,strlen(pszTailSig+L)+1,pszTailSig+L);
            SigToken[RidFromToken(tk)].indent = indent - L;
        }
#endif
    }
    SplitSignatureByCommas(szString,pszTailSig,tk,indent,GUICookie);
}

void PrettyPrintToken(__inout __nullterminated char* szString, mdToken tk, IMDInternalImport *pImport,void* GUICookie, mdToken FuncToken)
{
#ifdef TOKEN_SIG

    TokenSig* SigToken=NULL;

#endif
    char *szptr = &szString[strlen(szString)];
    if(!pImport->IsValidToken(tk))
    {
        szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), ERRORMSG("<invalid token 0x%8.8x>"), tk);
        return;
    }
    mdToken tkType = TypeFromToken(tk);

    DWORD ix;
    for(ix = 0; ix < g_NumTypedefs; ix++)
    {
        if((*g_typedefs)[ix].tkTypeSpec == tk) break;
    }
    if(ix < g_NumTypedefs)
    {
        REGISTER_REF(FuncToken,tk);
        strcpy_s(szptr,SZSTRING_REMAINING_SIZE(szptr),JUMPPT(ProperName((*g_typedefs)[ix].szName),(*g_typedefs)[ix].tkSelf));
        return;
    }

#ifdef TOKEN_SIG
    switch (tkType)
    {
        case mdtTypeDef:        SigToken = TSA_TD->ptsArray; break;
        case mdtTypeRef:        SigToken = TSA_TR->ptsArray; break;
        case mdtTypeSpec:       SigToken = TSA_TS->ptsArray; break;
        case mdtAssemblyRef:    SigToken = TSA_AR->ptsArray; break;
        case mdtModuleRef:      SigToken = TSA_MR->ptsArray; break;
        case mdtFieldDef:       SigToken = TSA_FD->ptsArray; break;
        default: SigToken = NULL; break;
    }

    if(SigToken && SigToken[RidFromToken(tk)].str)
    {
        REGISTER_REF(FuncToken,tk);
        strcpy_s(szptr,SZSTRING_REMAINING_SIZE(szptr),SigToken[RidFromToken(tk)].str);
        return;
    }
#endif
    CQuickBytes out;
    char*   pszForTokenSig = NULL;
    
    switch (tkType)
    {
        default:
        {
            szptr+=sprintf_s(szptr,SZSTRING_REMAINING_SIZE(szptr), ERRORMSG("<unknown token type 0x%02x>"), (BYTE) (tkType >> 24));
            pszForTokenSig = NULL;
            break;
        }

        case mdtTypeDef:
        case mdtTypeRef:
        case mdtTypeSpec:
        case mdtAssemblyRef:
        case mdtAssembly:
        case mdtModuleRef:
        case mdtModule:
        {
            REGISTER_REF(FuncToken,tk);
            strcpy_s(szptr,SZSTRING_REMAINING_SIZE(szptr), PrettyPrintClass(&out, tk, pImport));
            pszForTokenSig = szptr;
            break;
        }

        case mdtMethodDef:
        case mdtMemberRef:
            PrettyPrintTokenWithSplit(szString,tk,pImport,GUICookie);
            REGISTER_REF(FuncToken,tk);
            break;

        case mdtMethodSpec:
        {
            mdToken         meth=0;
            PCCOR_SIGNATURE pSig;
            ULONG       cSig;
            pImport->GetMethodSpecProps(tk, &meth, &pSig, &cSig);
            //g_tkMVarOwner = 0;
            PrettyPrintTokenWithSplit(szString,tk,pImport,GUICookie);
            REGISTER_REF(FuncToken,meth);
            break;
        }

        case mdtFieldDef:
        {
            HRESULT         hr;
            mdTypeRef       cr=0;
            const char *    pszMemberName;
            CQuickBytes     qbMemberSig;
            DWORD           dwAttrs;

            PCCOR_SIGNATURE pComSig;
            ULONG       cComSig;

            REGISTER_REF(FuncToken,tk);
            pszMemberName = pImport->GetNameOfFieldDef(tk);
            MAKE_NAME_IF_NONE(pszMemberName,tk);
            dwAttrs = pImport->GetFieldDefProps(tk);

            pComSig = pImport->GetSigOfFieldDef(
                tk,
                &cComSig
            );
            hr = pImport->GetParentToken(
                tk,
                &cr
            );
            if (FAILED(hr))
            {
                strcat_s(szString,SZSTRING_SIZE, "??");
                break;
            }

            // use the tail as a buffer
            char* curPos = &szString[strlen(szString)+1];
            *curPos = 0;
            if((cr != 0x02000001) && RidFromToken(cr))
            {
                const char* pszClass = PrettyPrintClass(&out, cr, pImport);
                REGISTER_REF(FuncToken,cr);
                sprintf_s(curPos,SZSTRING_REMAINING_SIZE(curPos),"%s::", pszClass);
            }
            char* curPos1 = &curPos[strlen(curPos)];
            if(IsFdPrivateScope(dwAttrs))
                sprintf_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),"%s$PST%08X",pszMemberName,tk);
            else
                strcpy_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),pszMemberName);
            strcpy_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),JUMPPT(ProperName(curPos1),tk));
            qbMemberSig.Shrink(0);
            strcpy_s(curPos-1,SZSTRING_REMAINING_SIZE(curPos), PrettyPrintSig(pComSig, cComSig, curPos, &qbMemberSig, pImport,NULL));
            curPos1 = &curPos[strlen(curPos)];
            if(g_fDumpTokens)
                sprintf_s(curPos1,SZSTRING_REMAINING_SIZE(curPos1),COMMENT(" /* %08X */"),tk);
            pszForTokenSig = szptr;
            break;
        }

        case mdtString:
        {
            const WCHAR * pszString = NULL;
            ULONG   cbString = 0;
            if(pImport->IsValidToken(tk))
                pszString = pImport->GetUserString(tk, &cbString, 0);
            if (pszString) {
                DumpUnicodeString(GUICookie,szString,(WCHAR *)pszString,cbString, true);
            }
            else
                sprintf_s(szString,SZSTRING_SIZE,ERRORMSG("INVALID TOKEN: 0x%8.8X"),tk);
            if(g_fDumpTokens)
                sprintf_s(&szString[strlen(szString)],SZSTRING_SIZE-strlen(szString),COMMENT(" /* %08X */"),tk);
            break;
        }
    } //end switch(TypeFromToken(tk))

#ifdef TOKEN_SIG
    if(SigToken && pszForTokenSig)
    {
        unsigned N = RidFromToken(tk);
        SigToken[N].str = new char[strlen(pszForTokenSig)+1];
        if(SigToken[N].str)
            strcpy_s(SigToken[N].str,strlen(pszForTokenSig)+1,pszForTokenSig);
    }
#endif
}

static char* keyword[] = {
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) s,
#define OPALIAS(alias_c, s, c) s,
#include "opcode.def"
#undef OPALIAS
#undef OPDEF
#define KYWD(name, sym, val)    name,
#include "il_kywd.h"
#undef KYWD
};
static bool KywdNotSorted = TRUE;
static char* szAllowedSymbols = "#_@$.";
static char DisallowedStarting[256];
static char DisallowedCont[256];

struct Indx
{
    void* table[128];
    Indx() { memset(table,0,sizeof(table)); };
    ~Indx()
    {
        for(int i = 1; i < 128; i++) delete ((Indx*)(table[i]));
    };
    void IndexString(__in __nullterminated char* psz, __out char** pkywd)
    {
        int i = (int) *psz;
        if(i == 0)
            table[0] = pkywd;
        else
        {
            _ASSERTE((i > 0)&&(i <= 127));
            Indx* pInd = (Indx*)(table[i]);
            if(pInd == NULL)
            {
                pInd = new Indx;
                _ASSERTE(pInd);
                table[i] = pInd;
            }
            pInd->IndexString(psz+1,pkywd);
        }
    }
    char**  FindString(__in __nullterminated char* psz)
    {
        if(*psz == 0)
            return (char**)(table[0]);
        if(*psz > 0)
        {
            Indx* pInd = (Indx*)(table[(unsigned char)*psz]);
            return (pInd == NULL) ? NULL : pInd->FindString(psz+1);
        }
        return NULL;
    }
};

/********************************************************************************/
/* looks up the keyword 'name' Returns FALSE on failure */
bool IsNameToQuote(const char *name)
{
    BYTE* p;
    BOOL    bStarting;
    static Indx* indx;

    if (indx==NULL)
        indx = new Indx();

    if((name==NULL)||(*name==0)) return TRUE;
    
    if(g_fQuoteAllNames)
        return ((strcmp(name,COR_CTOR_METHOD_NAME)!=0) && (strcmp(name,COR_CCTOR_METHOD_NAME)!=0));
    
    if(KywdNotSorted)
    {
        int j;
        char** low = keyword;
        char** high = &keyword[sizeof(keyword) / sizeof(char*)];
        char** mid;
        // name must start with alpha or one of allowed chars: #_$@.
        memset(DisallowedStarting,1,256);
        for(p = (BYTE*)szAllowedSymbols; *p; DisallowedStarting[*p]=0,p++);
        for(j='a'; j<= 'z'; DisallowedStarting[j]=0,j++);
        for(j='A'; j<= 'Z'; DisallowedStarting[j]=0,j++);
        // name must continue with the same chars, plus digits, and ? instead of #
        memcpy(DisallowedCont,DisallowedStarting,256);
        for(j='0'; j<= '9'; DisallowedCont[j]=0,j++);
        DisallowedCont[(unsigned char)'#'] = 1;
        DisallowedCont[(unsigned char)'?'] = 0;
        DisallowedCont[(unsigned char)'`'] = 0;
        // Sort the keywords for fast lookup
        for(mid = low; mid < high; mid++)
        {
            indx->IndexString(*mid,mid);
        }
        KywdNotSorted = FALSE;
    }
    //first, check for forbidden characters
    for(p = (BYTE*)name, bStarting = TRUE; *p; p++)
    {
        if(bStarting)
        {
            if(DisallowedStarting[*p]) return TRUE;
        }
        else
        {
            if(DisallowedCont[*p]) return TRUE;
        }
        bStarting = (*p == '.');
        if(bStarting && (*(p+1) == '.')) return TRUE;
    }

    //second, check for matching keywords (remember: .ctor and .cctor are names AND keywords)
    if(indx->FindString((char*)name))
    {
        return ((strcmp(name,COR_CTOR_METHOD_NAME)!=0) && (strcmp(name,COR_CCTOR_METHOD_NAME)!=0));
    }
    //third, check if the name starts or ends with dot (.ctor and .cctor are out of the way)
    return ((*name == '.') || (name[strlen(name)-1] == '.'));
}

/********************************************************************/
/*
 * This is currently unused, with all calls to it commented out. In
 * the event that those calls return, add this function back.
static char* ConvNumericEscape(char* retBuff, unsigned val) {
    _ASSERTE(val < 256);

        // print as octal
    *retBuff++ = '\\';
    *retBuff++ = (val >> 6) + '0';
    *retBuff++ = ((val >> 3) & 7) + '0';
    *retBuff++ = (val & 7) + '0';
    return(retBuff);
}
 */

/********************************************************************/
/* returns the quoted version of a string (including the quotes) */
/*                                                                                 */

static BOOL ConvToLiteral(__inout __nullterminated char* retBuff, const WCHAR* str, int cbString)
{
    BOOL ret = TRUE;
    char* was_retBuff = retBuff;

    *retBuff++ = '"';

    for(int tally=0; (tally < cbString)&&ret; tally++)
    {
        if (*str > 255)
        {
            //retBuff = ConvNumericEscape(retBuff, *str >> 8);
            //retBuff = ConvNumericEscape(retBuff, *str & 0xFF);
            ret = FALSE;
        }
        else if(!isprint(*str))
        {
            if (*str == '\n')
            {
                *retBuff++ = '\\';
                *retBuff++ = 'n';
            }
            else
                //retBuff = ConvNumericEscape(retBuff, *str);
                ret = FALSE;
        }
        else if (*str == '"')
        {
            *retBuff++ = '\\';
            *retBuff++ = '"';
        }
        else if (*str == '\\')
        {
            *retBuff++ = '\\';
            *retBuff++ = '\\';
        }
        else if (*str == '\t')
        {
            *retBuff++ = '\\';
            *retBuff++ = 't';
        }
        else
            *retBuff++ = (char) *str;
        str++;
    }
    *retBuff++ = '"';
    *retBuff++ = 0;
    if(!ret) *was_retBuff = 0;
    return ret;
}

