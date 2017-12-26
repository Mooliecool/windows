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
// File: options.h
//
// ===========================================================================

#ifndef __options_h__
#define __options_h__

#include "csiface.h"


////////////////////////////////////////////////////////////////////////////////
// Struct used for the static options table

struct OPTIONDEF
{
    PCWSTR      pszShortSwitch;     // Short Name of switch
    PCWSTR      pszLongSwitch;      // Long Name of switch
    PCWSTR      pszDescSwitch;      // Descriptive Name of switch
    long        iOptionID;          // Option ID
    size_t      iOffset;            // Offset of COptionData member holding value
    long        iDescID;            // Description string (res id)
    BSTR        bstrDesc;           // Description (loaded from resource)
    DWORD       dwFlags;            // Option flags
};

////////////////////////////////////////////////////////////////////////////////
// Struct used for the static warnings info (parallel to warnign ID's array)

struct WARNINGDEF
{
    long        iWarnDescID;        // Description string (res id)
    BSTR        bstrDesc;           // Description (loaded from resource)
};


namespace WatsonOperationKind {
    enum _Enum {
        None = 0,
        Prompt = 1,
        Send = 2,
        Queue = 3,
    };
};
DECLARE_ENUM_TYPE(WatsonOperationKind);


////////////////////////////////////////////////////////////////////////////////
// COptionData
//
// This is the table-driven options structure using by the controller and the
// config objects.  Each member is referred to by the option table, and changed
// and/or accessed via member pointers.
//
// Note: the data in this structure is copied and assigned around. Members should not
// point to allocated data unless appropriate copy constructors/assignment operators exist,
// such as for CComBSTR.

class CCoreOptionData
{
public:

    #define BOOLOPTDEF(id,descid,sh,lg,des,f) BOOL m_f##id;
    #define CORESTROPTDEF(id,descid,sh,lg,des,f) CComBSTR m_sbstr##id;
    #define STROPTDEF(id,descid,sh,lg,des,f)
    #include "optdef.h"

    BOOL m_fNOCODEGEN;  // obsolete option (always FALSE), but code still references is.

    // Here are some mappings for some of the special-cased options
    // that are parsed upon commit (like warning level)
    int  warnLevel;
    bool pdbOnly;

    WatsonOperationKindEnum m_howToReportWatsons;

    PlatformType m_platform;

    CompatibilityMode compatMode;
    // Compatibility Mode Accessors (assumes each mode is mutually exclusive)
    // Examples:
    // bool     IsNoneMode(); // The default
    // bool     IsECMA1Mode();
#define COMPATDEF( name, value, text) \
    bool Is ## name ## Mode() { return compatMode == Compatibility ## name ; }
#define COMPATNAME( name, text)
#include "compat.h"
    // End compatibility Accessors

    // List of warnings that are being suppressed.
    // N.B.: This is a BSTR, but is not text -- each character is a warning number!
    // We use CComBSTR because it handles memory allocation issues cleanly and robustly.
    CComBSTR noWarnNumbers;
    bool IsNoWarnNumber(WORD id);
    bool Is64Bit() { return m_platform >= platformIA64; };

};


class COptionData : public CCoreOptionData
{
public:
    // This is the option table.  It's static since it doesn't change per
    // instance of configurations or options objects.
    static  OPTIONDEF   m_rgOptionTable[];
    static  long        m_rgiOptionIndexMap[];
    static  long        m_rgiWarningIDs[];
    static  WARNINGDEF  m_rgWarningInfo[];
    static  BOOL        m_fMapCreated;
    static  void        CreateIndexMap ();

    // Initialize global string instance from resources in a thread safe manner
    static HRESULT      LoadCachedBSTR(BSTR* pbstr, long iResID);
    // Cleanup of global datastructures
    static  void        Shutdown();

    // Accessors to static option table
    static  OPTIONDEF   *GetOptionDef (long i) { return m_rgOptionTable + i; }
    static  long        GetOptionIndex (long iOptionID) { if (!m_fMapCreated) CreateIndexMap(); return m_rgiOptionIndexMap[iOptionID]; }

    // This defines all of the member variables that correspond to options.
    // Boolean options are named "m_f<id-base>" -- for example, m_fNOSTDLIB
    // String options are named "m_sbstr<id-base>" -- such as m_sbstrCCSYMBOLS
    #define BOOLOPTDEF(id,descid,sh,lg,des,f)
    #define CORESTROPTDEF(id,descid,sh,lg,des,f)
    #define STROPTDEF(id,descid,sh,lg,des,f) CComBSTR m_sbstr##id;
    #include "optdef.h"

    CComBSTR unknownTestSwitch; // test code only: set if bogus switch given to -test:xxx

    COptionData () 
    {
        ResetToDefaults(); 
    }

    void    ResetToDefaults ();
    const DWORD CountOfWarnings() const;
    bool IsWarnAsError(WORD id);
};


class CController;

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig
//
// Implementation of ISCCompilerConfig

class CCompilerConfig :
    public CComObjectRootMT,
    public ICSCompilerConfig
{
private:
    CController     *m_pController;         // NOTE:  Addref'd!
    COptionData     *m_pData;               // Our option data

    HRESULT ReportOptionErrorArgs(ICSError **ppError, int id, int carg, ErrArg * prgarg);
    HRESULT ReportOptionError(ICSError **ppError, int id) { return ReportOptionErrorArgs(ppError, id, 0, NULL); }
    HRESULT ReportOptionError(ICSError **ppError, int id, ErrArg a);
    HRESULT ReportOptionError(ICSError **ppError, int id, ErrArg a, ErrArg b);

public:
    BEGIN_COM_MAP(CCompilerConfig)
        COM_INTERFACE_ENTRY(ICSCompilerConfig)
    END_COM_MAP()

    CCompilerConfig();
    ~CCompilerConfig();

    HRESULT     Initialize (CController *pController, COptionData *pData);

    // ICSCompilerConfig
    STDMETHOD(GetOptionCount)(long *piCount);
    STDMETHOD(GetOptionInfoAt)(long iIndex, long *piOptionID, PCWSTR *ppszSwitchName, PCWSTR *ppszDescription, DWORD *pdwFlags);
    STDMETHOD(GetOptionInfoAtEx)(long iIndex, long *piOptionID, PCWSTR *ppszShortSwitchName, PCWSTR *ppszLongSwitchName, PCWSTR *ppszDescSwitchName, PCWSTR *ppszDescription, DWORD *pdwFlags);
    STDMETHOD(ResetAllOptions)();
    STDMETHOD(SetOption)(long iOptionID, VARIANT vtValue);
    STDMETHOD(GetOption)(long iOptionID, VARIANT *pvtValue);
    STDMETHOD(GetWarnNumbers)(long *piCount, const long **pWarnIds);
    STDMETHOD(GetWarnInfo)(long iWarnIndex, PCWSTR *ppszWarnDescription);
    STDMETHOD(CommitChanges)(ICSError **ppError);
    STDMETHOD(GetCompiler)(ICSCompiler **ppCompiler);
};


#endif //__options_h__

