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
#ifndef __data_h__
#define __data_h__

#include "cor.h"
#include "corhdr.h"
#include "cor.h"
#include "dacprivate.h"

BOOL FileExist (const char *filename);
BOOL FileExist (const WCHAR *filename);

// We use global variables
// because move returns void if it fails
//typedef DWORD DWORD_PTR;
//typedef ULONG ULONG_PTR;

// Max length in WCHAR for a buffer to store metadata name
const int mdNameLen = 2048;
extern WCHAR g_mdName[mdNameLen];

const int nMDIMPORT = 128;
struct MDIMPORT
{
    enum MDType {InMemory, InFile, Dynamic};
    WCHAR *name;
    size_t base;    // base of the PE module
    size_t mdBase;  // base of the metadata
    char *metaData;
    ULONG metaDataSize;
    MDType type;
    IMetaDataImport *pImport;

    MDIMPORT *left;
    MDIMPORT *right;
};

class Module;

extern BOOL ControlC;
extern IMetaDataDispenserEx *pDisp;

#endif // __data_h__
