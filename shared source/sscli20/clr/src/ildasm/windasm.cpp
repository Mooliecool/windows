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
/************************************************************************************************
 *                                                                                              *
 *  File:    winmain.cpp                                                                        *
 *                                                                                              *
 *  Purpose: Main program for graphic COM+ 2.0 disassembler ILDASM.exe                          *
 *                                                                                              *
 ************************************************************************************************/


#define INITGUID

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <crtdbg.h>
#include <utilcode.h>
#include <string.h>

#include "specstrings.h"

#include <palstartup.h>

#include "dynamicarray.h"

#define DO_DASM_GUI
#include "dasmgui.h"
#include "dasmenum.hpp"
#include "dis.h"
#include <ndpversion.h>
#include "resource.h"

#include "new.hpp"

#define MODE_DUMP_ALL               0
#define MODE_DUMP_CLASS             1
#define MODE_DUMP_CLASS_METHOD      2
#define MODE_DUMP_CLASS_METHOD_SIG  3

// All externs are defined in DASM.CPP
extern BOOL                    g_fDumpIL;
extern BOOL                    g_fDumpHeader;
extern BOOL                    g_fDumpAsmCode;
extern BOOL                    g_fDumpTokens;
extern BOOL                    g_fDumpStats;
extern BOOL                    g_fDumpClassList;
extern BOOL                    g_fDumpTypeList;
extern BOOL                    g_fDumpSummary;
extern BOOL                    g_fDecompile; // still in progress
extern BOOL                    g_fShowRefs;

extern BOOL                    g_fDumpToPerfWriter;

extern BOOL                    g_fShowBytes;
extern BOOL                    g_fShowSource;
extern BOOL                    g_fInsertSourceLines;
extern BOOL                    g_fTryInCode;
extern BOOL                    g_fQuoteAllNames;
extern BOOL                    g_fTDC;
extern BOOL                    g_fShowCA;
extern BOOL                    g_fCAVerbal;

extern char                    g_pszClassToDump[];
extern char                    g_pszMethodToDump[];
extern char                    g_pszSigToDump[];

extern char                    g_szAsmCodeIndent[];

extern DWORD                   g_Mode;

extern char                     g_szInputFile[]; // in UTF-8
extern char                     g_szOutputFile[]; // in UTF-8
extern char*                    g_pszObjFileName;
extern FILE*                    g_pFile;

extern BOOL                 g_fLimitedVisibility;
extern BOOL                 g_fHidePub;
extern BOOL                 g_fHidePriv;
extern BOOL                 g_fHideFam;
extern BOOL                 g_fHideAsm;
extern BOOL                 g_fHideFAA;
extern BOOL                 g_fHideFOA;
extern BOOL                 g_fHidePrivScope;
extern unsigned             g_uCodePage;
extern unsigned             g_uConsoleCP;
extern BOOL                 g_fOnUnicode;
extern BOOL                 g_fForwardDecl;
extern BOOL                 g_fUseProperName;

#include "../tools/metainfo/mdinfo.h"
extern BOOL                 g_fDumpMetaInfo;
extern ULONG                g_ulMetaInfoFilter;
HANDLE                      hConsoleOut=NULL;
HANDLE                      hConsoleErr=NULL;
// These are implemented in DASM.CPP:
BOOL Init();
void Uninit();
void Cleanup();
void DumpMetaInfo(__in __nullterminated char* pszFileName, __in __nullterminated char* pszObjFileName, void* GUICookie);
FILE* OpenOutput(__in __nullterminated char* szFileName);


void PrintLogo()
{
    printf("Microsoft (R) Shared Source CLI IL Disassembler.  Version " VER_FILEVERSION_STR);
    printf("\n%S\n\n", VER_LEGALCOPYRIGHT_LOGO_STR_L);
}
void SyntaxCon()
{
    DWORD l;

    for(l=IDS_USAGE_01; l<= IDS_USAGE_23; l++) printf(RstrANSI(l));
    {
        for(l=IDS_USAGE_24; l<= IDS_USAGE_32; l++) printf(RstrANSI(l));
        for(l=IDS_USAGE_34; l<= IDS_USAGE_36; l++) printf(RstrANSI(l));
        for(l=IDS_USAGE_37; l<= IDS_USAGE_39; l++) printf(RstrANSI(l));
    }
    for(l=IDS_USAGE_41; l<= IDS_USAGE_42; l++) printf(RstrANSI(l));

}

char* CheckForDQuotes(__inout __nullterminated char* sz)
{
    char* ret = sz;
    if(*sz == '"')
    {
        ret++;
        sz[strlen(sz)-1] = 0;
    }
    return ret;
}

char* EqualOrColon(__in __nullterminated char* szArg)
{
    char* pchE = strchr(szArg,'=');
    char* pchC = strchr(szArg,':');
    char* ret;
    if(pchE == NULL) ret = pchC;
    else if(pchC == NULL) ret = pchE;
    else ret = (pchE < pchC)? pchE : pchC;
    return ret;
}

int ProcessOneArg(__in __nullterminated char* szArg, __out char** ppszObjFileName)
{
    char        szOpt[128];
    if(strlen(szArg) == 0) return 0;
    if ((strcmp(szArg, "/?") == 0) || (strcmp(szArg, "-?") == 0)) return 1;

    if((szArg[0] == '/') || (szArg[0] == '-'))
    {
        strncpy_s(szOpt,128, &szArg[1],10);
        szOpt[3] = 0;
        if (_stricmp(szOpt, "dec") == 0)
        {
            g_fDecompile = TRUE;
        }
        else if (_stricmp(szOpt, "hea") == 0)
        {
            g_fDumpHeader = TRUE;
        }
        else if (_stricmp(szOpt, "adv") == 0)
        {
            g_fTDC = TRUE;
        }
        else if (_stricmp(szOpt, "tok") == 0)
        {
            g_fDumpTokens = TRUE;
        }
        else if (_stricmp(szOpt, "noi") == 0)
        {
            g_fDumpAsmCode = FALSE;
        }
        else if (_stricmp(szOpt, "noc") == 0)
        {
            g_fShowCA = FALSE;
        }
        else if (_stricmp(szOpt, "cav") == 0)
        {
            g_fCAVerbal = TRUE;
        }
        else if (_stricmp(szOpt, "not") == 0)
        {
            g_fTryInCode = FALSE;
        }
        else if (_stricmp(szOpt, "raw") == 0)
        {
            g_fTryInCode = FALSE;
        }
        else if (_stricmp(szOpt, "byt") == 0)
        {
            g_fShowBytes = TRUE;
        }
        else if (_stricmp(szOpt, "sou") == 0)
        {
            g_fShowSource = TRUE;
        }
        else if (_stricmp(szOpt, "lin") == 0)
        {
            g_fInsertSourceLines = TRUE;
        }
        else if ((_stricmp(szOpt, "sta") == 0)&&g_fTDC)
        {
            g_fDumpStats = g_fTDC;
        }
        else if ((_stricmp(szOpt, "cla") == 0)&&g_fTDC)
        {
            g_fDumpClassList = g_fTDC;
        }
        else if (_stricmp(szOpt, "typ") == 0)
        {
            g_fDumpTypeList = TRUE;
        }
        else if (_stricmp(szOpt, "sum") == 0)
        {
            g_fDumpSummary = TRUE;
        }
        else if (_stricmp(szOpt, "per") == 0)
        {
            g_fDumpToPerfWriter = TRUE;
        }
        else if (_stricmp(szOpt, "for") == 0)
        {
            g_fForwardDecl = TRUE;
        }
        else if (_stricmp(szOpt, "ref") == 0)
        {
            g_fShowRefs = TRUE;
        }
        else if (_stricmp(szOpt, "pub") == 0)
        {
            g_fLimitedVisibility = TRUE;
            g_fHidePub = FALSE;
        }
        else if (_stricmp(szOpt, "pre") == 0)
        {
            //g_fPrettyPrint = TRUE;
        }
        else if (_stricmp(szOpt, "vis") == 0)
        {
            char *pc = EqualOrColon(szArg);
            char *pStr;
            if(pc == NULL) return -1;
            do {
                pStr = pc+1;
                pStr = CheckForDQuotes(pStr);
                if((pc = strchr(pStr,'+'))) *pc=0;
                if     (!_stricmp(pStr,"pub")) g_fHidePub = FALSE;
                else if(!_stricmp(pStr,"pri")) g_fHidePriv = FALSE;
                else if(!_stricmp(pStr,"fam")) g_fHideFam = FALSE;
                else if(!_stricmp(pStr,"asm")) g_fHideAsm = FALSE;
                else if(!_stricmp(pStr,"faa")) g_fHideFAA = FALSE;
                else if(!_stricmp(pStr,"foa")) g_fHideFOA = FALSE;
                else if(!_stricmp(pStr,"psc")) g_fHidePrivScope = FALSE;
            } while(pc);
            g_fLimitedVisibility = g_fHidePub || g_fHidePriv || g_fHideFam || g_fHideFAA
                                                || g_fHideFOA || g_fHidePrivScope;
        }
        else if (_stricmp(szOpt, "quo") == 0)
        {
            g_fQuoteAllNames = TRUE;
        }
        else if (_stricmp(szOpt, "utf") == 0)
        {
            g_uCodePage = CP_UTF8;
        }
        else if (_stricmp(szOpt, "uni") == 0)
        {
            g_uCodePage = 0xFFFFFFFF;
        }
        else if (_stricmp(szOpt, "rtf") == 0)
        {
            g_fDumpRTF = TRUE;
            g_fDumpHTML = FALSE;
        }
        else if (_stricmp(szOpt, "htm") == 0)
        {
            g_fDumpRTF = FALSE;
            g_fDumpHTML = TRUE;
        }
        else if (_stricmp(szOpt, "all") == 0)
        {
            g_fDumpStats = g_fTDC;
            g_fDumpHeader = TRUE;
            g_fShowBytes = TRUE;
            g_fDumpClassList = g_fTDC;
            g_fDumpTokens = TRUE;
        }
        else if (_stricmp(szOpt, "ite") == 0)
        {
            char *pStr = EqualOrColon(szArg);
            char *p, *q;
            if(pStr == NULL) return -1;
            pStr++;
            pStr = CheckForDQuotes(pStr);
            // treat it as meaning "dump only class X" or "class X method Y"
            p = strchr(pStr, ':');

            if (p == NULL)
            {
                // dump one class
                g_Mode = MODE_DUMP_CLASS;
                strcpy_s(g_pszClassToDump, MAX_CLASSNAME_LENGTH, pStr);
            }
            else
            {
                *p++ = '\0';
                if (*p != ':') return -1;

                strcpy_s(g_pszClassToDump, MAX_CLASSNAME_LENGTH, pStr);

                p++;

                q = strchr(p, '(');
                if (q == NULL)
                {
                    // dump class::method
                    g_Mode = MODE_DUMP_CLASS_METHOD;
                    strcpy_s(g_pszMethodToDump, MAX_MEMBER_LENGTH, p);
                }
                else
                {
                    // dump class::method(sig)
                    g_Mode = MODE_DUMP_CLASS_METHOD_SIG;
                    *q = '\0';
                    strcpy_s(g_pszMethodToDump, MAX_MEMBER_LENGTH, p);
                    // get rid of external parentheses:
                    q++;
                    strcpy_s(g_pszSigToDump, MAX_SIGNATURE_LENGTH, q);
                }
            }
        }
        else if ((_stricmp(szOpt, "met") == 0)&&g_fTDC)
        {
            if(g_fTDC)
            {
                char *pStr = EqualOrColon(szArg);
                g_fDumpMetaInfo = TRUE;
                if(pStr)
                {
                    char szOptn[64];
                    strncpy_s(szOptn, 64, pStr+1,10);
                    szOptn[3] = 0; // recognize metainfo specifier by first 3 chars
                    if     (_stricmp(szOptn, "hex") == 0) g_ulMetaInfoFilter |= MDInfo::dumpMoreHex;
                    else if(_stricmp(szOptn, "csv") == 0) g_ulMetaInfoFilter |= MDInfo::dumpCSV;
                    else if(_stricmp(szOptn, "mdh") == 0) g_ulMetaInfoFilter |= MDInfo::dumpHeader;
                    else if(_stricmp(szOptn, "raw") == 0) g_ulMetaInfoFilter |= MDInfo::dumpRaw;
                    else if(_stricmp(szOptn, "hea") == 0) g_ulMetaInfoFilter |= MDInfo::dumpRawHeaps;
                    else if(_stricmp(szOptn, "sch") == 0) g_ulMetaInfoFilter |= MDInfo::dumpSchema;
                    else if(_stricmp(szOptn, "unr") == 0) g_ulMetaInfoFilter |= MDInfo::dumpUnsat;
                    else if(_stricmp(szOptn, "val") == 0) g_ulMetaInfoFilter |= MDInfo::dumpValidate;
                    else if(_stricmp(szOptn, "sta") == 0) g_ulMetaInfoFilter |= MDInfo::dumpStats;
                    else return -1;
                }
            }
        }
        else if (_stricmp(szOpt, "out") == 0)
        {
            char *pStr = EqualOrColon(szArg);
            if(pStr == NULL) return -1;
            pStr++;
            pStr = CheckForDQuotes(pStr);
            if(*pStr == 0) return -1;
            if(_stricmp(pStr,"con"))
            {
                strncpy_s(g_szOutputFile, MAX_FILENAME_LENGTH, pStr,MAX_FILENAME_LENGTH-1);
                g_szOutputFile[MAX_FILENAME_LENGTH-1] = 0;
            }
        }
        else
        {
#ifdef PLATFORM_UNIX
            if (szArg[0] == L'/')
            {
                // On BSD it could be a fully qualified file that starts with '/'
                // Not a switch, so it might be a FileName
                goto AssumeFileName;
            }
            else
#endif
            {
            PrintLogo();
            printf(RstrANSI(IDS_E_INVALIDOPTION),szArg); //"INVALID COMMAND LINE OPTION: %s\n\n",szArg);
            return -1;
        }
    }
    }
    else
    {
#ifdef PLATFORM_UNIX
AssumeFileName:
#endif
        if(g_szInputFile[0])
        {
            PrintLogo();
            printf(RstrANSI(IDS_E_MULTIPLEINPUT)); //"MULTIPLE INPUT FILES SPECIFIED\n\n");
            return -1; // check if it was already specified
        }
        szArg = CheckForDQuotes(szArg);
        strncpy_s(g_szInputFile, MAX_FILENAME_LENGTH,szArg,MAX_FILENAME_LENGTH-1);
        g_szInputFile[MAX_FILENAME_LENGTH-1] = 0;
    }
    return 0;
}

char* UTF8toANSI(__in __nullterminated char* szUTF)
{
    ULONG32 L = (ULONG32) strlen(szUTF)+16;
    WCHAR* wzUnicode = new WCHAR[L];
    memset(wzUnicode,0,L*sizeof(WCHAR));
    WszMultiByteToWideChar(CP_UTF8,0,szUTF,-1,wzUnicode,L);
    L <<= 2;
    char* szANSI = new char[L];
    memset(szANSI,0,L);
    WszWideCharToMultiByte(g_uConsoleCP,0,wzUnicode,-1,szANSI,L,NULL,NULL);
    VDELETE(wzUnicode);
    return szANSI;
}
char* ANSItoUTF8(__in __nullterminated char* szANSI)
{
    ULONG32 L = (ULONG32) strlen(szANSI)+16;
    WCHAR* wzUnicode = new WCHAR[L];
    memset(wzUnicode,0,L*sizeof(WCHAR));
    WszMultiByteToWideChar(g_uConsoleCP,0,szANSI,-1,wzUnicode,L);
    L *= 3;
    char* szUTF = new char[L];
    memset(szUTF,0,L);
    WszWideCharToMultiByte(CP_UTF8,0,wzUnicode,-1,szUTF,L,NULL,NULL);
    VDELETE(wzUnicode);
    return szUTF;
}


int __cdecl main(int argc, char **argv)
{
    // ildasm does not need to be SO-robust.
    SO_NOT_MAINLINE_FUNCTION;

#ifdef _DEBUG
    DisableThrowCheck();
    //CONTRACT_VIOLATION(ThrowsViolation);
#endif

    int     iCommandLineParsed = 0;

    if (!PAL_RegisterLibraryW(L"rotor_palrt") ||
        !PAL_RegisterLibraryW(L"sscoree"))
    {
        printError(NULL, RstrUTF(IDS_UNABLETOREGLIBS));
        exit(1);
    }

    g_fOnUnicode = OnUnicodeSystem();
    g_fUseProperName = TRUE;

    g_pszClassToDump[0]=0;
    g_pszMethodToDump[0]=0;
    g_pszSigToDump[0]=0;
    memset(g_szInputFile,0,MAX_FILENAME_LENGTH);
    memset(g_szOutputFile,0,MAX_FILENAME_LENGTH);
    g_fTDC = TRUE;

#undef GetCommandLineW
#undef CreateProcessW
    g_pszObjFileName = NULL;

    g_szAsmCodeIndent[0] = 0;
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hConsoleErr = GetStdHandle(STD_ERROR_HANDLE);

    for(int i=1; i < argc; i++)
    {
        if((iCommandLineParsed = ProcessOneArg(argv[i],&g_pszObjFileName)))
            break;
    }

    if(!g_fLimitedVisibility)
    {
        g_fHidePub = FALSE;
        g_fHidePriv = FALSE;
        g_fHideFam = FALSE;
        g_fHideAsm = FALSE;
        g_fHideFAA = FALSE;
        g_fHideFOA = FALSE;
        g_fHidePrivScope = FALSE;
    }
    if(hConsoleOut != INVALID_HANDLE_VALUE) //First pass: console
    {
        if(iCommandLineParsed)
        {
            if(iCommandLineParsed > 0) PrintLogo();
            SyntaxCon();
            exit((iCommandLineParsed == 1) ? 0 : 1);
        }
        {
            DWORD   exitCode = 1;
            if(g_szInputFile[0] == 0)
            {
                SyntaxCon();
                exit(1);
            }
            g_pFile = NULL;
            if(g_szOutputFile[0])
            {
                g_pFile = OpenOutput(g_szOutputFile);
                if(g_pFile == NULL)
                {
                    char sz[4096];
                    sprintf_s(sz,4096,RstrUTF(IDS_E_CANTOPENOUT)/*"Unable to open '%s' for output."*/,   g_szOutputFile);
                    g_uCodePage = CP_ACP;
                    printError(NULL,sz);
                    exit(1);
                }
            }
            else // console output -- force the code page to ANSI
            {
                g_uConsoleCP = GetConsoleOutputCP();
                g_uCodePage = g_uConsoleCP;
                g_fDumpRTF = FALSE;
                g_fDumpHTML = FALSE;
            }
            if (Init() == TRUE)
            {
                exitCode = DumpFile(g_szInputFile) ? 0 : 1;
                Cleanup();
            }
            Uninit();
            exit(exitCode);
        }
    }
    return 0;
}

