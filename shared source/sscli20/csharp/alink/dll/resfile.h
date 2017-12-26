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
// ===========================================================================
// File: resfile.h
//
// This handles Win32Resources
// ===========================================================================

#ifndef __resfile_h__
#define __resfile_h__

class CFile;

class Win32Res {
public:
    Win32Res();
    ~Win32Res();

    HRESULT SetInfo(CFile *pFile, LPCWSTR szTitle, LPCWSTR szIconName, LPCWSTR szDescription,
        LPCWSTR szCopyright, LPCWSTR szTrademark, LPCWSTR szCompany, LPCWSTR szProduct, LPCWSTR szProductVersion,
        BOOL fDLL, WORD wVersion[4], LPCWSTR szFileVersion);
    HRESULT MakeResFile(const void **pData, DWORD  *pcbData);

private:
#define PadKeyLen(cb) ((((cb) + 5) & ~3) - 2)
#define PadValLen(cb) ((cb + 3) & ~3)
#define KEYSIZE(sz) (PadKeyLen(sizeof(sz)*sizeof(WCHAR))/sizeof(WCHAR))
#define KEYBYTES(sz) (KEYSIZE(sz)*sizeof(WCHAR))
#define HDRSIZE (3 * sizeof(WORD))

    static WORD             SizeofVerString(LPCWSTR lpszKey, LPCWSTR lpszValue);
    HRESULT                 WriteVerString(LPCWSTR lpszKey, LPCWSTR lpszValue);
    HRESULT                 WriteVerResource();
    HRESULT                 WriteIconResource();
    static void             StringToVersion(LPCWSTR szVersion, DWORD &dwHigh, DWORD &dwLow);
    static void             StringToVersion(LPCWSTR szVersion, WORD ver[4]);

    HRESULT                 Write(const void *pData, size_t len);
    CFile      *m_pFile;
    LPCWSTR     m_Icon;
    LPCWSTR     m_Values[8];
    WORD        m_Version[4];
    BOOL        m_fDll;
    PBYTE       m_pData;
    PBYTE       m_pCur;
    PBYTE       m_pEnd;


    // RES file structs (borrowed from MSDN)
#include <pshpack1.h>
    struct RESOURCEHEADER {
        DWORD DataSize;
        DWORD HeaderSize;
        WORD  Magic1;
        WORD  Type;
        WORD  Magic2;
        WORD  Name;
        DWORD DataVersion;
        WORD  MemoryFlags;
        WORD  LanguageId;
        DWORD Version;
        DWORD Characteristics;
    };

    struct ICONDIRENTRY {
        BYTE  bWidth;
        BYTE  bHeight;
        BYTE  bColorCount;
        BYTE  bReserved;
        WORD  wPlanes;
        WORD  wBitCount;
        DWORD dwBytesInRes;
        DWORD dwImageOffset;
    };

    struct ICONRESDIR {
        BYTE  Width;        // = ICONDIRENTRY.bWidth;
        BYTE  Height;       // = ICONDIRENTRY.bHeight;
        BYTE  ColorCount;   // = ICONDIRENTRY.bColorCount;
        BYTE  reserved;     // = ICONDIRENTRY.bReserved;
        WORD  Planes;       // = ICONDIRENTRY.wPlanes;
        WORD  BitCount;     // = ICONDIRENTRY.wBitCount;
        DWORD BytesInRes;   // = ICONDIRENTRY.dwBytesInRes;
        WORD  IconId;       // = RESOURCEHEADER.Name
    };
    struct EXEVERRESOURCE {
        WORD cbRootBlock;                                     // size of whole resource
        WORD cbRootValue;                                     // size of VS_FIXEDFILEINFO structure
        WORD fRootText;                                       // root is text?
        WCHAR szRootKey[KEYSIZE("VS_VERSION_INFO")];          // Holds "VS_VERSION_INFO"
        VS_FIXEDFILEINFO vsFixed;                             // fixed information.
        WORD cbVarBlock;                                      //   size of VarFileInfo block
        WORD cbVarValue;                                      //   always 0
        WORD fVarText;                                        //   VarFileInfo is text?
        WCHAR szVarKey[KEYSIZE("VarFileInfo")];               //   Holds "VarFileInfo"
        WORD cbTransBlock;                                    //     size of Translation block
        WORD cbTransValue;                                    //     size of Translation value
        WORD fTransText;                                      //     Translation is text?
        WCHAR szTransKey[KEYSIZE("Translation")];             //     Holds "Translation"
        WORD langid;                                          //     language id
        WORD codepage;                                        //     codepage id
        WORD cbStringBlock;                                   //   size of StringFileInfo block
        WORD cbStringValue;                                   //   always 0
        WORD fStringText;                                     //   StringFileInfo is text?
        WCHAR szStringKey[KEYSIZE("StringFileInfo")];         //   Holds "StringFileInfo"
        WORD cbLangCpBlock;                                   //     size of language/codepage block
        WORD cbLangCpValue;                                   //     always 0
        WORD fLangCpText;                                     //     LangCp is text?
        WCHAR szLangCpKey[KEYSIZE("12345678")];               //     Holds hex version of language/codepage
        // followed by strings
    };
#include <poppack.h>
};
#endif // __resfile_h__
