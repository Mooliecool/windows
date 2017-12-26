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
// File: inputset.h
//
// ===========================================================================

#ifndef __inputset_h__
#define __inputset_h__

#include "csiface.h"
#include "table.h"

class CController;

////////////////////////////////////////////////////////////////////////////////
// CSourceFileInfo

class CSourceFileInfo
{
public:
    NAME                *m_pName;               // Name of source file (case preserved)
    FILETIME            m_ft;                   // Last-write time
    CSourceFileInfo     *m_pNext;               // Next SourceFile (preserves ordering of files)
};

class CResourceFileInfo
{
public:
    NAME                *m_pFileName;           // Name of Resource file name
    NAME                *m_pIdent;              // Name of Identity.
    BOOL                 m_fEmbed;              // true if resource is embedded
    BOOL                 m_fVisible;            // true if resource is visible (public)
    CResourceFileInfo   *m_pNext;               // Next Resource file
};

typedef CAutoDelTable<CSourceFileInfo> CSourceTable;
typedef CAutoDelTable<CResourceFileInfo> CResourceTable;

////////////////////////////////////////////////////////////////////////////////
// CInputSet
//
// Represents an input set for adding sources/resources to compile into a single
// output file

class CInputSet :
    public CComObjectRootMT,
    public ICSInputSet
{
private:
    CController         *m_pController;             // Owning compiler controller
    CComBSTR            m_sbstrOutputName;          // Output file name
    CComBSTR            m_sbstrResourceFile;        // Win32 Resource file
    CComBSTR            m_sbstrIconFile;            // Win32 Icon file
    CComBSTR            m_sbstrMainClass;           // Fully Qualified class name to use for Main()
    CComBSTR            m_sbstrPDBFile;             // PDB file name.
    CSourceTable        m_tableSources;             // Source files
    CSourceFileInfo     *m_pSrcHead;                // Head of Source list, but in reverse order
    CResourceTable      m_tableResources;           // Resource files
    CResourceFileInfo   *m_pResHead;                // Head of Source list, but in reverse order
    unsigned            m_fDLL:1;                   // TRUE if creating a DLL
    unsigned            m_fNoOutput:1;              // TRUE if creating nothing
    unsigned            m_fWinApp:1;                // TRUE if creating a Windows exe
    unsigned            m_fAssemble:1;              // TRUE if creating an assembly
    ULONGLONG           m_ImageBase;                // Image Base
    DWORD               m_dwFileAlign;              // File Alignment

public:
    BEGIN_COM_MAP(CInputSet)
        COM_INTERFACE_ENTRY(ICSInputSet)
    END_COM_MAP()

    CInputSet ();
    ~CInputSet ();

    HRESULT     Initialize (CController *pController);
    HRESULT     IsSourceFileInInputSet (NAME *pName);

    CInputSet   *m_pNext;

    CSourceTable        *GetSourceTable() { return &m_tableSources; }
    BOOL                CopySources(CSourceFileInfo **ppSrcFiles, size_t iCount);
    BOOL                CopyResources(CResourceFileInfo **ppResFiles, size_t iCount);
    CResourceTable      *GetResourceTable() { return &m_tableResources; }
    PCWSTR              GetOutputName () { return m_fNoOutput ? NULL : (PCWSTR)m_sbstrOutputName; }
    PCWSTR              GetWin32Resource() { return m_sbstrResourceFile; }
    BOOL                DLL () { return m_fDLL; }
    ULONGLONG           ImageBase () { return m_ImageBase; }
    DWORD               FileAlignment () { return m_dwFileAlign; }
    PCWSTR              GetMainClass () { return m_fDLL ? NULL : (PCWSTR)m_sbstrMainClass; }
    BOOL                WinApp () { return m_fWinApp; }
    BOOL                MakeAssembly() { return m_fAssemble; }
    PCWSTR              GetWin32Icon() { return m_sbstrIconFile; }
    PCWSTR              GetPDBFile() { return m_sbstrPDBFile; }

    // ICSInputSet
    STDMETHOD(GetCompiler)(ICSCompiler **ppCompiler);
    STDMETHOD(AddSourceFile)(PCWSTR pszFileName, FILETIME* ft);
    STDMETHOD(RemoveSourceFile)(PCWSTR pszFileName);
    STDMETHOD(RemoveAllSourceFiles)();
    STDMETHOD(AddResourceFile)(PCWSTR pszFileName, PCWSTR pszIdent, BOOL bEmbed, BOOL bVis);
    STDMETHOD(RemoveResourceFile)(PCWSTR pszFileName, PCWSTR pszIdent, BOOL bEmbed, BOOL bVis);
    STDMETHOD(SetWin32Resource)(PCWSTR pszFileName);
    STDMETHOD(GetWin32Resource)(PCWSTR* ppszFileName);
    STDMETHOD(SetOutputFileName)(PCWSTR pszFileName);
    STDMETHOD(SetOutputFileType)(DWORD dwFileType);
    STDMETHOD(SetImageBase)(DWORD dwImageBase) { return SetImageBase2(dwImageBase); }
    STDMETHOD(SetMainClass)(PCWSTR pszFQClassName);
    STDMETHOD(SetWin32Icon)(PCWSTR pszIconFileName);
    STDMETHOD(SetFileAlignment)(DWORD dwAlign);
    STDMETHOD(SetImageBase2)(ULONGLONG ImageBase);
    STDMETHOD(SetPDBFileName)(PCWSTR pszFileName);
};

#endif //__inputset_h__

