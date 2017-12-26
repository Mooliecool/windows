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
//*****************************************************************************
// File: ildbdump.cpp
//*****************************************************************************

#include "stdafx.h"
#include <palstartup.h>

extern "C" const GUID ILDB_VERSION_GUID_FSR;
extern "C" const GUID ILDB_VERSION_GUID;

BOOL g_fullDump = FALSE;    // Dump all the information
BOOL g_headerOnly = FALSE;  // Dump only the header information
char *g_szInputFile = NULL; // ILDB Input File
PDBInfo *g_pPDBInfo = NULL; // PDB Information
IMetaDataImport *g_pMetaImport = NULL;  // Metadata import for assembly
PDBDataPointers g_DataPointers; // DataPointers

HANDLE g_hFile = INVALID_HANDLE_VALUE;
HANDLE g_hMap = NULL;
HMODULE g_hMod = NULL;


//
//  Print out the usage information
//
void Usage()
{
    printf("Microsoft (R) Shared Source CLI Debug Information Dumper.  Version %s\n", SSCLI_VERSION_STR);
    printf("Copyright (C) Microsoft Corporation 1998-2002. All rights reserved.\n");
    printf("\n");
    printf("Usage: ildbdump [/full][/header][/assembly:<assemblyfilename>] <ildb filename>\n");
    printf("\n");
    printf("\tThe filename should be an ildb file\n");
    printf("\n");
    printf("\t/full : Dump full debug information\n");
    printf("\t/header : Dump the header information but not the individual methods\n");
    printf("\t/assembly:<assemblyfilename> : If present we will use this to extract information from the metadata\n");
    printf("\n");
}

//
// Helper to indent the output
//
void IndentLevel(unsigned level)
{
  unsigned i;
  for (i = 0; i < level; i++)
  {
    printf("\t");
  }
}

//
// Dump the info in the PDBDataPointers for the methods
//
void DumpMethodsInfo()
{
    unsigned i;
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfMethods); i++)
    {
        printf("Method entry %d\n", i);
        printf("\tMethod Token: 0x%X\n", g_DataPointers.m_pMethods[i].MethodToken());
        printf("\tScopes start: 0x%X\n", g_DataPointers.m_pMethods[i].StartScopes());
        printf("\tScopes end: 0x%X\n", g_DataPointers.m_pMethods[i].EndScopes());
        printf("\tVariables start: 0x%X\n", g_DataPointers.m_pMethods[i].StartVars());
        printf("\tVariables end: 0x%X\n", g_DataPointers.m_pMethods[i].EndVars());
        printf("\tUsings start: 0x%X\n", g_DataPointers.m_pMethods[i].StartUsing());
        printf("\tUsings end: 0x%X\n", g_DataPointers.m_pMethods[i].EndUsing());
        printf("\tConstants start: 0x%X\n", g_DataPointers.m_pMethods[i].StartConstant());
        printf("\tConstants end: 0x%X\n", g_DataPointers.m_pMethods[i].EndConstant());
        printf("\tDocuments start: 0x%X\n", g_DataPointers.m_pMethods[i].StartDocuments());
        printf("\tDocuments end: 0x%X\n", g_DataPointers.m_pMethods[i].EndDocuments());
        printf("\tSequence points start: 0x%X\n", g_DataPointers.m_pMethods[i].StartSequencePoints());
        printf("\tSequence points end: 0x%X\n", g_DataPointers.m_pMethods[i].EndSequencePoints());
    }
}

//
// Dump the info in the PDBDataPointers for the scopes
//
void DumpScopesInfo()
{
    unsigned i;
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfScopes); i++)
    {
        printf("Scope entry %d\n", i);
        printf("\tParent Index: 0x%X\n", g_DataPointers.m_pScopes[i].ParentScope());
        printf("\tOffset start: 0x%X\n", g_DataPointers.m_pScopes[i].StartOffset());
        printf("\tOffset end: 0x%X\n", g_DataPointers.m_pScopes[i].EndOffset());
        printf("\tHas children: 0x%X\n", g_DataPointers.m_pScopes[i].HasChildren());
        printf("\tHas variables: 0x%X\n", g_DataPointers.m_pScopes[i].HasVars());
    }
}

//
// Dump the info in the PDBDataPointers for the variables
//
void DumpVariablesInfo()
{
    unsigned i;
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfVars); i++)
    {
        printf("Variable entry %d\n", i);
        printf("\tScope Index: 0x%X\n", g_DataPointers.m_pVars[i].Scope());
        printf("\tName index: 0x%X\n", g_DataPointers.m_pVars[i].Name());
        printf("\tAttribute: 0x%X\n", g_DataPointers.m_pVars[i].Attributes());
        printf("\tSignature index: 0x%X\n", g_DataPointers.m_pVars[i].Signature());
        printf("\tSignature size: 0x%X\n", g_DataPointers.m_pVars[i].SignatureSize());
        printf("\tAddress Kind: 0x%X\n", g_DataPointers.m_pVars[i].AddrKind());
        printf("\tAddress1: 0x%X\n", g_DataPointers.m_pVars[i].Addr1());
        printf("\tAddress2: 0x%X\n", g_DataPointers.m_pVars[i].Addr2());
        printf("\tAddress3: 0x%X\n", g_DataPointers.m_pVars[i].Addr3());
        printf("\tStart Offset: 0x%X\n", g_DataPointers.m_pVars[i].StartOffset());
        printf("\tEnd Offset: 0x%X\n", g_DataPointers.m_pVars[i].EndOffset());
        printf("\tSequence: 0x%X\n", g_DataPointers.m_pVars[i].Sequence());
        printf("\tIs Parameter: 0x%X\n", g_DataPointers.m_pVars[i].IsParam());
        printf("\tIs Hidden: 0x%X\n", g_DataPointers.m_pVars[i].IsHidden());
    }
}

//
// Dump the info in the PDBDataPointers for the using info
//
void DumpUsingsInfo()
{
    unsigned i;
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfUsing); i++)
    {
        printf("Using entry %d\n", i);
        printf("\tScope Index: 0x%X\n", g_DataPointers.m_pUsings[i].ParentScope());
        printf("\tName index: 0x%X\n", g_DataPointers.m_pUsings[i].Name());
    }
}

//
// Dump the constant value
//
void DumpConstantValue(unsigned ConstantEntry)
{
    VARIANT VariantValue;
    UINT32 ValueBstrEntry;
    VariantInit(&VariantValue);
    VariantValue = g_DataPointers.m_pConstants[ConstantEntry].Value(&ValueBstrEntry);
    if (ValueBstrEntry)
    {
        ULONG32 cchValue;
        WCHAR *wcsValue;
        // Convert the UTF8 string to Wide
        cchValue = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                                0,
                                                (LPCSTR)&(g_DataPointers.m_pStringsBytes[ValueBstrEntry]),
                                                -1,
                                                0,
                                                NULL);
        wcsValue = (WCHAR *)_alloca(cchValue * sizeof(WCHAR));

        MultiByteToWideChar(CP_UTF8,
                            0,
                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[ValueBstrEntry]),
                            -1,
                            wcsValue,
                            cchValue);

        printf("Value: '%S'\n", wcsValue);
    }
    if (SUCCEEDED(VariantChangeType(&VariantValue,&VariantValue, 0, VT_BSTR)))
    {
        printf("Value: %S\n", V_BSTR(&VariantValue));
    }
    else
    {
        printf("\tError:Unable to convert value to a string\n");
    }
    VariantClear(&VariantValue);
}

//
// Dump the info in the PDBDataPointers for the Constants
//
void DumpConstantsInfo()
{
    unsigned i;
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfConstants); i++)
    {
        printf("Constant entry %d\n", i);
        printf("\tScope Index: 0x%X\n", g_DataPointers.m_pConstants[i].ParentScope());
        printf("\tName index: 0x%X\n", g_DataPointers.m_pConstants[i].Name());
        printf("\tSignature index: 0x%X\n", g_DataPointers.m_pConstants[i].Signature());
        printf("\tSignature size: 0x%X\n", g_DataPointers.m_pConstants[i].SignatureSize());
        DumpConstantValue(i);
    }
}

//
// Dump the info in the PDBDataPointers for the Sequence Points
//
void DumpSequencePointsInfo()
{
    unsigned i;
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfSequencePoints); i++)
    {
        printf("Sequence point entry %d\n", i);
        printf("\tOffset: 0x%X\n", g_DataPointers.m_pSequencePoints[i].Offset());
        printf("\tStart line: 0x%X\n", g_DataPointers.m_pSequencePoints[i].StartLine());
        printf("\tStart column: 0x%X\n", g_DataPointers.m_pSequencePoints[i].StartColumn());
        printf("\tEnd line: 0x%X\n", g_DataPointers.m_pSequencePoints[i].EndLine());
        printf("\tEnd column: 0x%X\n", g_DataPointers.m_pSequencePoints[i].EndColumn());
        printf("\tDocument Index: 0x%X\n", g_DataPointers.m_pSequencePoints[i].Document());
    }
}

//
// Dump the info in the PDBDataPointers for the Documents
//
void DumpDocumentsInfo()
{
    unsigned i;
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfDocuments); i++)
    {
        WCHAR strClsid[39];

        printf("Sequence point entry %d\n", i);
        StringFromGUID2(g_DataPointers.m_pDocuments[i].Language(), strClsid, sizeof(strClsid)/sizeof(WCHAR));
        printf("\tLanguage guid: %S\n", strClsid);
        StringFromGUID2(g_DataPointers.m_pDocuments[i].LanguageVendor(), strClsid, sizeof(strClsid)/sizeof(WCHAR));
        printf("\tLanguageVendor guid: %S\n", strClsid);
        StringFromGUID2(g_DataPointers.m_pDocuments[i].DocumentType(), strClsid, sizeof(strClsid)/sizeof(WCHAR));
        printf("\tDocumentType guid: %S\n", strClsid);
        StringFromGUID2(g_DataPointers.m_pDocuments[i].AlgorithmId(), strClsid, sizeof(strClsid)/sizeof(WCHAR));
        printf("\tAlgorithmId guid: %S\n", strClsid);
        printf("\tCheckSum size: 0x%X\n", g_DataPointers.m_pDocuments[i].CheckSumSize());
        printf("\tCheckSum entry: 0x%X\n", g_DataPointers.m_pDocuments[i].CheckSumEntry());
        printf("\tSource size: 0x%X\n", g_DataPointers.m_pDocuments[i].SourceSize());
        printf("\tSource entry: 0x%X\n", g_DataPointers.m_pDocuments[i].SourceEntry());
        printf("\tURL entry: 0x%X\n", g_DataPointers.m_pDocuments[i].UrlEntry());
    }
}

//
// Dump the info in the PDBDataPointers for the Bytes
//
void DumpBytesInfo()
{
    unsigned i;
    printf("***Bytes***\n");
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfBytes); i++)
    {
      printf("%c", g_DataPointers.m_pBytes[i]);
    }
    printf("\n***Done dumping the Bytes***\n");
}

//
// Dump the info in the PDBDataPointers for the Strings
//
void DumpStringsInfo()
{
    unsigned i;
    printf("***Strings***\n");
    for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfStringBytes); i++)
    {
      printf("%C", g_DataPointers.m_pStringsBytes[i]);
    }
    printf("\n***Done dumping the Strings***\n");
}

//
// Dump the given signature
//
void DumpSignature(ULONG32 SigSize, UINT32 SigEntry, unsigned level)
{
    unsigned i;
    if (SigSize)
    {
        // Print the signature
        IndentLevel(level+1);
        printf("Signature Size: %d\n", SigSize);
        IndentLevel(level+1);
        printf("Signature:\n");
        IndentLevel(level+2);
        for (i = 0; i < SigSize; i++)
        {
          printf("%.2X", g_DataPointers.m_pBytes[SigEntry+i]);
        }
        printf("\n");
    }
}

//
// Dump the constant
//
void DumpConstant(unsigned MethodEntry, unsigned ConstantEntry, unsigned level)
{
    int cchName;
    WCHAR *wcsName;

    IndentLevel(level);
    printf("Constant entry %d\n", ConstantEntry);
    IndentLevel(level+1);
    
    // Convert the UTF8 string to Wide
    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pConstants[ConstantEntry].Name()]),
                                            -1,
                                            0,
                                            NULL);
    wcsName = (WCHAR *)_alloca(cchName * sizeof(WCHAR));

    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pConstants[ConstantEntry].Name()]),
                                            -1,
                                            wcsName,
                                            cchName);

    printf("Name: %S\n", wcsName);

    // Print the signature
    DumpSignature(g_DataPointers.m_pConstants[ConstantEntry].Signature(),
                  g_DataPointers.m_pConstants[ConstantEntry].SignatureSize(),
                  level+1);
    
    DumpConstantValue(ConstantEntry);
}

//
// Dump the variable
//
void DumpVariable(unsigned MethodEntry, unsigned VariableEntry, unsigned level)
{
    int cchName;
    WCHAR *wcsName;

    IndentLevel(level);
    printf("Variable entry %d\n", VariableEntry);
    IndentLevel(level+1);
    // Convert the UTF8 string to Wide
    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pVars[VariableEntry].Name()]),
                                            -1,
                                            0,
                                            NULL);
    wcsName = (WCHAR *)_alloca(cchName * sizeof(WCHAR));

    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pVars[VariableEntry].Name()]),
                                            -1,
                                            wcsName,
                                            cchName);

    printf("Name: %S\n", wcsName);

    IndentLevel(level+1);
    printf("Attribute: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].Attributes());

    // Print the signature
    DumpSignature(g_DataPointers.m_pVars[VariableEntry].SignatureSize(),
                  g_DataPointers.m_pVars[VariableEntry].Signature(),
                  level);

    IndentLevel(level+1);
    printf("Address Kind: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].AddrKind());
    IndentLevel(level+1);
    printf("Address1: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].Addr1());
    IndentLevel(level+1);
    printf("Address2: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].Addr2());
    IndentLevel(level+1);
    printf("Address3: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].Addr3());
    IndentLevel(level+1);
    printf("Start Offset: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].StartOffset());
    IndentLevel(level+1);
    printf("End Offset: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].EndOffset());
    IndentLevel(level+1);
    printf("Sequence: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].Sequence());
    IndentLevel(level+1);
    printf("Is Parameter: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].IsParam());
    IndentLevel(level+1);
    printf("Is Hidden: 0x%X\n", g_DataPointers.m_pVars[VariableEntry].IsHidden());
}

//
// Dump the scope information (variables, constants and sub scopes)
//
void DumpScope(unsigned MethodEntry, unsigned ScopeEntry, unsigned level)
{
    IndentLevel(level);
    printf("Scope entry %d\n", ScopeEntry);
    level++;
    IndentLevel(level);
    printf("Parent Index: 0x%X\n", g_DataPointers.m_pScopes[ScopeEntry].ParentScope());
    IndentLevel(level);
    printf("Offset start: 0x%X\n", g_DataPointers.m_pScopes[ScopeEntry].StartOffset());
    IndentLevel(level);
    printf("Offset end: 0x%X\n", g_DataPointers.m_pScopes[ScopeEntry].EndOffset());

    // Dump any Variables
    if (g_DataPointers.m_pMethods[MethodEntry].StartVars() != g_DataPointers.m_pMethods[MethodEntry].EndVars())
    {
        IndentLevel(level);
        printf("Variables in this scope:\n");

        unsigned i;
        // Walk all the variables in the method and see if they point to this scope
        for (i = g_DataPointers.m_pMethods[MethodEntry].StartVars();
             i < g_DataPointers.m_pMethods[MethodEntry].EndVars();
             i++)
        {
            if (g_DataPointers.m_pVars[i].Scope() == ScopeEntry)
            {
              DumpVariable(MethodEntry, i, level+1);
            }
        }
    }
    else
    {
        IndentLevel(level);
        printf("No variables in this scope\n");
    }

    // Dump any constants
    if (g_DataPointers.m_pMethods[MethodEntry].StartConstant() != g_DataPointers.m_pMethods[MethodEntry].EndConstant())
    {
        unsigned i;
        IndentLevel(level);
        printf("Constants in this scope:\n");
        for (i = g_DataPointers.m_pMethods[MethodEntry].StartConstant();
             i < g_DataPointers.m_pMethods[MethodEntry].EndConstant();
             i++)
        {
            if (g_DataPointers.m_pConstants[i].ParentScope() == ScopeEntry)
            {
              DumpConstant(MethodEntry, i, level+1);
            }
        }

    }
    else
    {
        IndentLevel(level);
        printf("No constants in this scope\n");
    }

    // Dump any sub scopes
    if (g_DataPointers.m_pScopes[ScopeEntry].HasChildren())
    {
        unsigned i;

        IndentLevel(level);
        printf("SubScopes\n");
        // Walk all the variables in the method and see if they point to this scope
        for (i = g_DataPointers.m_pMethods[MethodEntry].StartScopes();
             i < g_DataPointers.m_pMethods[MethodEntry].EndScopes();
             i++)
        {
            if (g_DataPointers.m_pScopes[i].ParentScope() == ScopeEntry)
            {
              DumpScope(MethodEntry, i, level+1);
            }
        }
    }
    else
    {
        IndentLevel(level);
        printf("No subscopes\n");
    }
}

//
// Dump the given Using entry
//
void DumpUsing(unsigned UsingEntry, unsigned level)
{
    int cchName;
    WCHAR *wcsName;
    
    IndentLevel(level);
    // Convert the UTF8 string to Wide
    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pUsings[UsingEntry].Name()]),
                                            -1,
                                            0,
                                            NULL);
    wcsName = (WCHAR *)_alloca(cchName * sizeof(WCHAR));

    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pUsings[UsingEntry].Name()]),
                                            -1,
                                            wcsName,
                                            cchName);

    printf("Using: %S\n", wcsName);
}

//
// Dump the sequence point
//
void DumpSeqencePoint(unsigned SequencePointEntry, unsigned level)
{
    int cchName;
    WCHAR *wcsName;

    IndentLevel(level);
    printf("Sequence Point entry: 0x%X\n", SequencePointEntry);
    IndentLevel(level+1);
    // Convert the UTF8 string to Wide
    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pDocuments[g_DataPointers.m_pSequencePoints[SequencePointEntry].Document()].UrlEntry()]),
                                            -1,
                                            0,
                                            NULL);
    wcsName = (WCHAR *)_alloca(cchName * sizeof(WCHAR));

    cchName = (ULONG32) MultiByteToWideChar(CP_UTF8,
                                            0,
                                            (LPCSTR)&(g_DataPointers.m_pStringsBytes[g_DataPointers.m_pDocuments[g_DataPointers.m_pSequencePoints[SequencePointEntry].Document()].UrlEntry()]),
                                            -1,
                                            wcsName,
                                            cchName);

    printf("Document: %S\n", wcsName);

    IndentLevel(level+1);
    printf("Offset: 0x%X\n", g_DataPointers.m_pSequencePoints[SequencePointEntry].Offset());
    IndentLevel(level+1);
    printf("Start line, column: %d, %d\n", g_DataPointers.m_pSequencePoints[SequencePointEntry].StartLine(),
                                               g_DataPointers.m_pSequencePoints[SequencePointEntry].StartColumn());
    IndentLevel(level+1);
    printf("End line, column: %d, %d\n", g_DataPointers.m_pSequencePoints[SequencePointEntry].EndLine(),
                                             g_DataPointers.m_pSequencePoints[SequencePointEntry].EndColumn());
}

//
// Dump the Method information (Token, scopes, etc)
//
void DumpMethod(unsigned MethodEntry)
{
    printf("Method entry %d\n", MethodEntry);
    printf("Method Token: 0x%X\n", g_DataPointers.m_pMethods[MethodEntry].MethodToken());

    // If we have the assembly information, use it to get the method and class names
    if (g_pMetaImport)
    {
        mdTypeDef   mdClass;
        ULONG cchMethod;
        WCHAR *wcsMethodName = NULL;
        ULONG cchClass;
        WCHAR *wcsClassName = NULL;

        // If there's any error, ignore the error and just keep going
        if (SUCCEEDED(g_pMetaImport->GetMethodProps(g_DataPointers.m_pMethods[MethodEntry].MethodToken(),
                                      NULL, // Class token
                                      NULL,
                                      0,
                                      &cchMethod, // Length of Method
                                      NULL, NULL, NULL, NULL, NULL)))
        {
            wcsMethodName = (WCHAR *)_alloca(cchMethod * sizeof (WCHAR));
            if (SUCCEEDED(g_pMetaImport->GetMethodProps(g_DataPointers.m_pMethods[MethodEntry].MethodToken(),
                                          &mdClass, // Class token
                                          wcsMethodName,
                                          cchMethod,
                                          NULL, NULL, NULL, NULL, NULL, NULL)))
            {
                printf("Method Name: %S\n", wcsMethodName);
                if (SUCCEEDED(g_pMetaImport->GetTypeDefProps(mdClass, NULL, 0, &cchClass, 0, NULL)))
                {
                    wcsClassName = (WCHAR *)_alloca(cchClass * sizeof (WCHAR));
                    if (SUCCEEDED(g_pMetaImport->GetTypeDefProps(mdClass, wcsClassName, cchClass, NULL, 0, NULL)))
                    {
                        printf("Class Name: %S\n", wcsClassName);
                    }
                }
            }
        }
    }

    // If there's any scopes, walk them
    if (g_DataPointers.m_pMethods[MethodEntry].StartScopes() != g_DataPointers.m_pMethods[MethodEntry].EndScopes())
    {
        DumpScope(MethodEntry, g_DataPointers.m_pMethods[MethodEntry].StartScopes(), 0);
    }

    // Dump the using information for the method
    if (g_DataPointers.m_pMethods[MethodEntry].StartUsing() !=
        g_DataPointers.m_pMethods[MethodEntry].EndUsing())
    {
        unsigned i;
        printf("Using Information:\n");
        for (i = g_DataPointers.m_pMethods[MethodEntry].StartUsing();
             i < g_DataPointers.m_pMethods[MethodEntry].EndUsing();
             i++)
        {
            DumpUsing(i, 1);
        }
    }

    // Dump the sequence points
    if (g_DataPointers.m_pMethods[MethodEntry].StartSequencePoints() != g_DataPointers.m_pMethods[MethodEntry].EndSequencePoints())
    {
        unsigned i;
        printf("Sequence Points:\n");
        for (i = g_DataPointers.m_pMethods[MethodEntry].StartSequencePoints();
             i < g_DataPointers.m_pMethods[MethodEntry].EndSequencePoints();
             i++)
        {
            DumpSeqencePoint(i, 1);
        }
    }
}

//
// Dump the ildb file
//
HRESULT DumpIldbFile(char *szIldbFileName)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapFile = INVALID_HANDLE_VALUE;
    HMODULE hMod = NULL;
    BYTE *CurrentOffset;
    DWORD dwFileSize;
    DWORD dwDataSize;
    GUID VersionInfo;
  
    HRESULT hr = NOERROR;

    hFile = CreateFileA(szIldbFileName,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
      IfFailGo(HrFromWin32(GetLastError()));
    }

    dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize < ILDB_HEADER_SIZE)
    {
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapFile == NULL)
        IfFailGo(HrFromWin32(GetLastError()));

    hMod = (HMODULE) MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    if (hMod == NULL)
        IfFailGo(HrFromWin32(GetLastError()));

    // We've opened the file, now let's get the pertinent info
    CurrentOffset = (BYTE *)hMod;

    // Verify that we're looking at an ILDB File
    if (memcmp(CurrentOffset, ILDB_SIGNATURE, ILDB_SIGNATURE_SIZE))
    {
        IfFailGo(E_FAIL);
    }
    CurrentOffset += ILDB_SIGNATURE_SIZE;

    memcpy( &VersionInfo, CurrentOffset, sizeof(GUID));
    SwapGuid( &VersionInfo );
    CurrentOffset += sizeof(GUID);
    
    if (memcmp(&VersionInfo, &ILDB_VERSION_GUID, sizeof(GUID)))
    {  
        if (memcmp(&VersionInfo, &ILDB_VERSION_GUID_FSR, sizeof(GUID)) == 0)
        {
            fprintf(stderr, "First Source Release format not supported by ildbdump\n");
        }
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    g_pPDBInfo = (PDBInfo *)CurrentOffset;

    // Check to make sure we have enough data to be read in.
    dwDataSize = ILDB_HEADER_SIZE +
                 (VAL32(g_pPDBInfo->m_CountOfMethods) * sizeof(SymMethodInfo)) +
                 (VAL32(g_pPDBInfo->m_CountOfScopes)*sizeof(SymLexicalScope)) +
                 (VAL32(g_pPDBInfo->m_CountOfVars)*sizeof(SymVariable)) +
                 (VAL32(g_pPDBInfo->m_CountOfUsing)*sizeof(SymUsingNamespace)) +
                 (VAL32(g_pPDBInfo->m_CountOfConstants)*sizeof(SymConstant)) +
                 (VAL32(g_pPDBInfo->m_CountOfSequencePoints)*sizeof(SequencePoint)) +
                 (VAL32(g_pPDBInfo->m_CountOfDocuments)*sizeof(DocumentInfo)) +
                 (VAL32(g_pPDBInfo->m_CountOfBytes)*sizeof(BYTE)) +
                 (VAL32(g_pPDBInfo->m_CountOfStringBytes)*sizeof(BYTE));

    if (dwFileSize < dwDataSize)
    {
        IfFailGo(HrFromWin32(ERROR_INVALID_DATA));
    }

    printf("Number of methods: %d\n", VAL32(g_pPDBInfo->m_CountOfMethods));

    CurrentOffset += sizeof(PDBInfo);
    if (VAL32(g_pPDBInfo->m_CountOfMethods))
    {
        g_DataPointers.m_pMethods = (SymMethodInfo *)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfMethods)*sizeof(SymMethodInfo));
        if (g_fullDump)
        {
          DumpMethodsInfo();
        }
    }

    printf("Number of scopes: %d\n", VAL32(g_pPDBInfo->m_CountOfScopes));

    if (VAL32(g_pPDBInfo->m_CountOfScopes))
    {
        g_DataPointers.m_pScopes = (SymLexicalScope *)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfScopes)*sizeof(SymLexicalScope));
        if (g_fullDump)
        {
          DumpScopesInfo();
        }
    }

    printf("Number of vars: %d\n", VAL32(g_pPDBInfo->m_CountOfVars));

    if (VAL32(g_pPDBInfo->m_CountOfVars))
    {
        g_DataPointers.m_pVars = (SymVariable *)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfVars)*sizeof(SymVariable));
        if (g_fullDump)
        {
          DumpVariablesInfo();
        }
    }

    printf("Number of Using entries: %d\n", VAL32(g_pPDBInfo->m_CountOfUsing));

    if (VAL32(g_pPDBInfo->m_CountOfUsing))
    {
        g_DataPointers.m_pUsings = (SymUsingNamespace *)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfUsing)*sizeof(SymUsingNamespace));
        if (g_fullDump)
        {
          DumpUsingsInfo();
        }
    }

    printf("Number of constants: %d\n", VAL32(g_pPDBInfo->m_CountOfConstants));

    if (VAL32(g_pPDBInfo->m_CountOfConstants))
    {
        g_DataPointers.m_pConstants = (SymConstant*)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfConstants)*sizeof(SymConstant));
        if (g_fullDump)
        {
          DumpConstantsInfo();
        }
    }

    printf("Number of sequence points: %d\n", VAL32(g_pPDBInfo->m_CountOfSequencePoints));

    if (VAL32(g_pPDBInfo->m_CountOfSequencePoints))
    {
        g_DataPointers.m_pSequencePoints = (SequencePoint*)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfSequencePoints)*sizeof(SequencePoint));
        if (g_fullDump)
        {
          DumpSequencePointsInfo();
        }
    }

    printf("Number of Documents: %d\n", VAL32(g_pPDBInfo->m_CountOfDocuments));

    if (VAL32(g_pPDBInfo->m_CountOfDocuments))
    {
        g_DataPointers.m_pDocuments = (DocumentInfo*)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfDocuments)*sizeof(DocumentInfo));
        if (g_fullDump)
        {
          DumpDocumentsInfo();
        }
    }

    printf("Number of Bytes: %d\n", VAL32(g_pPDBInfo->m_CountOfBytes));

    if (VAL32(g_pPDBInfo->m_CountOfBytes))
    {
        g_DataPointers.m_pBytes = (BYTE*)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfBytes)*sizeof(BYTE));
        if (g_fullDump)
        {
          DumpBytesInfo();
        }
    }

    printf("Number of string bytes: %d\n", VAL32(g_pPDBInfo->m_CountOfStringBytes));

    if (VAL32(g_pPDBInfo->m_CountOfStringBytes))
    {
        g_DataPointers.m_pStringsBytes = (BYTE *)CurrentOffset;
        CurrentOffset += (VAL32(g_pPDBInfo->m_CountOfStringBytes)*sizeof(BYTE));
        if (g_fullDump)
        {
          DumpStringsInfo();
        }
    }

    // If we don't just want the headers, dump the individual methods
    if (!g_headerOnly)
    {
        unsigned i;
        for (i = 0; i < VAL32(g_pPDBInfo->m_CountOfMethods); i++)
        {
            DumpMethod(i);
        }
    }
        
ErrExit:
    
    return hr;
}

//
//  Return the IMetaDataImport for the passed file.
//
HRESULT GetMetaFromFile(LPCWSTR wcsFileName, IMetaDataImport **ppMetaImport)
{
    IMAGE_NT_HEADERS * pNtHeader;
    IMAGE_COR20_HEADER         *pICH;
    HRESULT hr = NOERROR;
    DWORD dwLength;
    IMetaDataDispenserEx *pDisp = NULL;

    // Create a dispenser
    IfFailGo(PAL_CoCreateInstance(CLSID_CorMetaDataDispenser,
                              IID_IMetaDataDispenserEx,
                              (void **) &pDisp));

    // Open and map
    g_hFile = CreateFileW( wcsFileName, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (g_hFile == INVALID_HANDLE_VALUE) {
        IfFailGo(HrLastError());
    }

    dwLength = GetFileSize( g_hFile, NULL);

    // Map it into memory
    g_hMap = CreateFileMappingA( g_hFile, NULL, PAGE_READONLY, 0, dwLength, NULL);
    if (g_hMap == NULL) {
        IfFailGo(HrLastError());
    }

    g_hMod = (HMODULE) MapViewOfFile(g_hMap, FILE_MAP_READ, 0, 0, 0);
    if (g_hMod == NULL)
        IfFailGo(HrFromWin32(GetLastError()));


    // Locate standard NT image header.
    pNtHeader = Cor_RtlImageNtHeader(g_hMod, dwLength);
    if (pNtHeader == NULL) {
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }

    pICH = (IMAGE_COR20_HEADER*) Cor_RtlImageRvaToVa(pNtHeader, (PBYTE)g_hMod, 
        VAL32(pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER].VirtualAddress),
        dwLength);

    if (pICH == NULL)
    {
        IfFailGo(HrFromWin32(ERROR_BAD_FORMAT));
    }

    // Open the scope on memory
    IfFailGo(pDisp->OpenScopeOnMemory(
             Cor_RtlImageRvaToVa(pNtHeader, (BYTE *)g_hMod, VAL32(pICH->MetaData.VirtualAddress), dwLength),
             VAL32(pICH->MetaData.Size),
             ofRead, 
             IID_IMetaDataImport, 
             (IUnknown **)ppMetaImport));

ErrExit:
    RELEASE(pDisp);

    if (FAILED(hr))
    {
        return hr;
    }
    // Could return S_FALSE
    if (hr != S_OK)
    {
        return E_FAIL;
    }
    return hr;
}

//
//  Process one argument
//
int ProcessOneArg(char* szArg)
{
	if(strlen(szArg) == 0) return 0;
    if ((strcmp(szArg, "/?") == 0) || (strcmp(szArg, "-?") == 0)) return 1;

	if((szArg[0] == '/') || (szArg[0] == '-'))
    {
        if (_stricmp(&szArg[1], "full") == 0)
        {
            g_fullDump = TRUE;
        }
        else if (_stricmp(&szArg[1], "header") == 0)
        {
            g_headerOnly = TRUE;
        }
        else if (_strnicmp(&szArg[1], "assembly", 8) == 0)
        {
            if (g_pMetaImport)
            {
                printf("Error: MULTIPLE ASSEMBLY FILES SPECIFIED\n\n");
                return -1;
            }

            int cchWideLength;
            WCHAR *wcsAssemblyFileName = NULL;
            char *szAssemblyName;

            szAssemblyName = &szArg[10];
            cchWideLength = MultiByteToWideChar(CP_ACP, 0, szAssemblyName, -1, NULL, 0);
            wcsAssemblyFileName = (WCHAR *)_alloca(cchWideLength * sizeof (WCHAR));
            MultiByteToWideChar(CP_ACP, 0, szAssemblyName, -1, wcsAssemblyFileName, cchWideLength);

            if (FAILED(GetMetaFromFile(wcsAssemblyFileName, &g_pMetaImport)))
            {
                printf("Error: Unable to open assembly file: %s\n\n", &szArg[10]);
                return -1;
            }
        }
        else
        {
            printf("Error: Invalid parameter %s\n\n", szArg);
        }
    }
    else
    {
        // check if it was already specified
        if(g_szInputFile)
		{
            printf("Error: MULTIPLE INPUT FILES SPECIFIED\n\n");
            return -1;
		}
        g_szInputFile = szArg;
    }
    return 0;
}

//
//  Main entry point
//
int __cdecl main(int argc, char **argv)
{
    HRESULT hr = NOERROR;
    char *szIldbFileName;
    int iCommandLineParsed = 0;

    char szFileName[_MAX_FNAME],
          szExt[_MAX_EXT],
          szDir[_MAX_DIR],
          szDrive[_MAX_DRIVE];

    // Process the arguments
    for(int i=1; i < argc; i++)
    {
        if((iCommandLineParsed = ProcessOneArg(argv[i]))) 
            break;
    }

    // Display the usage information if the arguments aren't correct
    if(g_szInputFile == NULL || iCommandLineParsed != 0) 
    {
        Usage();
        exit(1);
    }

    _splitpath(g_szInputFile, szDrive, szDir, szFileName, szExt);
    if (szExt == NULL)
    {

        // .ildb + Null terminator
        size_t OutputFileNameLength = strlen(szDrive) + 
                                      strlen(szDir) +
                                      strlen(szFileName)
                                      + 6;

        szIldbFileName = (char *)_alloca(OutputFileNameLength * sizeof (WCHAR));
        strcpy(szIldbFileName, szDrive);
        strcat(szIldbFileName, szDir);
        strcat(szIldbFileName, szFileName);
        strcat(szIldbFileName, ".ildb");
    }
    else
    {
      szIldbFileName = g_szInputFile;
    }

    IfFailGo(DumpIldbFile(szIldbFileName));

ErrExit:

    if (g_hMod)
    {
        UnmapViewOfFile(g_hMod);
    }
    if (g_hMap)
    {
        CloseHandle(g_hMap);
    }
    if (g_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_hFile);
    }

    // Display error information if possible
    if (FAILED(hr))
    {
        IErrorInfo *pErrorInfo;
        fprintf(stderr, "Error 0x%X: ", hr);
        if (GetErrorInfo(0, &pErrorInfo) == S_OK)
        {    
            BSTR ErrorString;
            if (pErrorInfo->GetDescription(&ErrorString) == S_OK)
            {
                fprintf(stderr, "%S\n", ErrorString);
                SysFreeString(ErrorString);
                hr = NOERROR;
            }
        }
        else
        {
            WCHAR wszDescription[1024];
            // Get the description from the system
            wszDescription[0] = '\0';
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
                           NULL,
                           hr,
                           0,
                           wszDescription,
                           sizeof(wszDescription) / sizeof(WCHAR),
                           NULL);
            // If the system has a string for this error
            if (*wszDescription != '\0')
            {
                fprintf(stderr, "%S\n", wszDescription);
                hr = NOERROR;
            }
        }

        // Make sure something was printed
        if (FAILED(hr))
        {
            fprintf(stderr, "Terminating\n", hr);
        }
        return 1;
    }

    return 0;
}
