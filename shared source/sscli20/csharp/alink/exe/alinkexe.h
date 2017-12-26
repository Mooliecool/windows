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
// File: alinkexe.h
// 
// standard header for ALink front end.
// ===========================================================================

#ifndef __alinkexe_h__
#define __alinkexe_h__

enum ERRORKIND {
    ERROR_FATAL,
    ERROR_ERROR,
    ERROR_WARNING
};

#define CLIENT_IS_ALINK
#include "consoleoutput.h"
#include "consoleargs.h"
#undef CLIENT_IS_ALINK

struct resource {
    LPWSTR pszIdent;    // Do this first so we can use _wcsicmp as comparison function
    LPWSTR pszSource;
    bool   bVis;
    bool   bEmbed;
    union {
        LPWSTR pszTarget;
        DWORD  dwOffset;
    };

    resource() {
        pszIdent = pszSource = pszTarget = NULL;
        bVis = bEmbed = false;
    }
    ~resource() {
        if (pszIdent != NULL)
            delete [] pszIdent;
        if (pszSource != NULL)
            delete [] pszSource;
        if (!bEmbed && pszTarget != NULL)
            delete [] pszTarget;
        pszIdent = pszSource = pszTarget = NULL;
    }
};

struct addfile {
    LPWSTR pszSource;
    LPWSTR pszTarget;
    bool   bManaged;
    addfile() { pszSource = pszTarget = NULL; }
    ~addfile() {
        if (pszSource != NULL)
            delete [] pszSource;
        if (pszTarget != NULL)
            delete [] pszTarget;
        pszSource = pszTarget = NULL;
    }
};

enum TargetType {
    ttDll,
    ttConsole,
    ttWinApp
};

struct _options {
    AssemblyOptions opt;
    LPCWSTR         ShortName;
    LPCWSTR         LongName;
    LPCWSTR         CA;
    int             HelpId;
    VARTYPE         vt;
    BYTE            flag;
};

struct _optionused {
#define OPTION(name, type, flags, id, shortname, longname, CA) bool b_##name : 1;
#include "options.h"
    bool get(AssemblyOptions opt) {
        switch (opt) {
#define OPTION(name, type, flags, id, shortname, longname, CA) case name: return b_##name;
#include "options.h"
            default:
                break;
        }
        return false;
    }
    void set(AssemblyOptions opt, bool val = true) {
        switch (opt) {
#define OPTION(name, type, flags, id, shortname, longname, CA) case name: b_##name = val; break;
#include "options.h"
            default:
                break;
        }
    }

    // There is only one static instance of this class - it will be initialized to zeros by default
    // _optionused() {
    //     memset(this, 0, sizeof(*this));
    // }
};

typedef list<addfile> FileList;
typedef list<resource*> ResList;

// Options
extern LPWSTR          g_szAssemblyFile;
extern LPWSTR          g_szAppMain;
extern LPWSTR          g_szCopyAssembly;
extern VARIANT         g_Options[optLastAssemOption];
extern FileList       *g_pFileListHead;
extern FileList      **g_ppFileListEnd;
extern ResList        *g_pResListHead;
extern ResList       **g_ppResListEnd;
extern DWORD           g_dwDllBase;
extern DWORD           g_dwCodePage;
extern bool            g_bNoLogo;
extern bool            g_bShowHelp;
extern bool            g_bTime;
extern bool            g_bFullPaths;
extern bool            g_bInstall;
extern bool            g_bMake;
extern TargetType      g_Target;
// If non-NULL - the following is the output file for a repro case.
extern bool            g_fullPaths;
extern bool            g_bUnicode;
extern bool            g_bOnErrorReported;
extern HINSTANCE       g_hinstMessages;
extern _optionused     g_bCopyOptions;


#define OPTION(name, type, flags, id, shortname, longname, CA) {name, shortname, longname, CA, id, type, flags},
const _options Options [] = {
#include "options.h"
    {optLastAssemOption,   L"", L"", L"", IDS_InternalError, VT_EMPTY, 0x00}
};

// Linker error numbers.
#define ERRORDEF(num, level, name, strid) name,
enum ERRORIDS {
    ERR_NONE,
    #include "errors.h"
    ERR_COUNT
};
#undef ERRORDEF

#define ERRORDEF(num, level, name, strid) {num, level, strid},
static const ERROR_INFO errorInfo[ERR_COUNT] = {
    {0000, -1, 0},          // ERR_NONE - no error.
    #include "errors.h"
};
#undef ERRORDEF

enum ILCODE {
#define OPDEF(id, name, pop, push, operand, type, len, b1, b2, cf) id = ((b1 << 8) | b2),
#include "opcode.def"
#undef OPDEF
};

ICeeFileGen* CreateCeeFileGen();
void DestroyCeeFileGen(ICeeFileGen *ceefilegen);
HRESULT SetDispenserOptions(IMetaDataDispenserEx * dispenser);
HRESULT SetAssemblyOptions(mdAssembly assemID, IALink *pLinker);
void MakeAssembly(ICeeFileGen *pFileGen, IMetaDataDispenserEx *pDisp, IALink2 *pLinker, IMetaDataError *pErrHandler);
HRESULT CopyAssembly(mdAssembly assemID, IALink * pLinker);
HRESULT ImportCA( const BYTE * pvData, DWORD cbSize, AssemblyOptions opt, mdAssembly assemID, IALink *pLinker);
bool AddVariantOption( VARIANT &var, VARIANT &val, bool fAllowMulti);
HRESULT FindMain( IMetaDataImport *pImport, IMetaDataEmit *pEmit, LPCWSTR pszTypeName, LPCWSTR pszMethodName, mdMemberRef &tkMain, mdMethodDef &tkNewMain, int &countArgs, bool &bHadBogusMain);
HRESULT MakeMain(ICeeFileGen *pFileGen, HCEEFILE file, HCEESECTION ilSection, IMetaDataEmit *pEmit, mdMethodDef tkMain, mdMemberRef tkEntryPoint, int iArgs);
HRESULT CheckHR(HRESULT hr);
HRESULT IsNonGeneric( IMetaDataImport * pImport, mdToken tkClassOrMethod);


class CErrors :
    public IMetaDataError
{
public:
    STDMETHOD(QueryInterface)(const GUID &iid, void** ppUnk) {
        if (iid == IID_IMetaDataError) {
            *ppUnk = (IMetaDataError*)this;
            return S_OK;
        } else {
            *ppUnk = NULL;
            return E_FAIL;
        }
    }
    STDMETHOD_(DWORD, AddRef)() { return 1; }
    STDMETHOD_(DWORD, Release)() { return 1; }

    // Interface methods here
    STDMETHOD(OnError)(HRESULT hr, mdToken tkLocation);
};

#endif //__alinkexe_h__

