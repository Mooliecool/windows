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
/* ------------------------------------------------------------------------- *
 * debug\comshell.h: com debugger shell class
 * ------------------------------------------------------------------------- */

#ifndef __DSHELL_H__
#define __DSHELL_H__

#include <stdio.h>


#undef CreateProcess

#include "cor.h"
#include "shell.h"
#include "corpub.h"
#include "corsym.h"
#include "cordebug.h"
#include "corerror.h"
#include "palclr.h"
#ifdef _X86_
#define PTR_TO_CORDB_ADDRESS(_ptr) (CORDB_ADDRESS)(ULONG)(_ptr)
#else
#define PTR_TO_CORDB_ADDRESS(_ptr) (CORDB_ADDRESS)(ULONG_PTR)(_ptr)
#endif //_X86_

#define CORDB_ADDRESS_TO_PTR(_cordb_addr) ((LPVOID)(SIZE_T)(_cordb_addr))


//
// inline function to access the CONTEXT
//
#ifdef _X86_
inline LPVOID GetIP(CONTEXT *context) {
    return (LPVOID)(size_t)(context->Eip);
}
inline LPVOID GetSP(CONTEXT *context) {
    return (LPVOID)(size_t)(context->Esp);
}
inline LPVOID GetFP(CONTEXT* context) {
    return (LPVOID)(UINT_PTR)context->Ebp;
}
#elif defined(_PPC_)
inline LPVOID GetIP(CONTEXT *context) {
    return (LPVOID)(size_t)(context->Iar);
}
inline LPVOID GetSP(CONTEXT *context) {
    return (LPVOID)(size_t)(context->Gpr1);
}
inline LPVOID GetFP(CONTEXT* context) {
    return (LPVOID)(UINT_PTR)context->Gpr30;
}
#else
inline LPVOID GetIP(CONTEXT *context) {
    PORTABILITY_ASSERT("Need to define CONTEXT access/modify functions for shell");
    return NULL;
}
inline LPVOID GetSP(CONTEXT* context)
{
    PORTABILITY_ASSERT("Need to define CONTEXT access/modify functions for shell");
    return NULL;
}
inline LPVOID GetFP(CONTEXT* context) {
    PORTABILITY_ASSERT("Need to define CONTEXT access/modify functions for shell");
    return NULL;
}
#endif




#define REG_COMPLUS_DEBUGGER_KEY "DbgManagedDebugger"

// Names of registry keys used to hold the source files path.
#define REG_SOURCES_KEY  "CorDbgSourceFilePath"
#define REG_MODE_KEY     "CorDbgModes"

#define MAX_MODULES                     512
#define MAX_FILE_MATCHES_PER_MODULE     4
#define MAX_EXT                         20
#define MAX_PATH_ELEMS                  64
#define MAX_CACHE_ELEMS                 256

#define MAX_SYMBOL_NAME_LENGTH          256

#if defined(_X86_) || defined(_PPC_)
#define PAGE_SIZE   0x1000
#else
#error Page size is not known on this architecture
#endif

#define ON_ERROR_EXIT() if(hr != S_OK) { shell->ReportError(hr); goto done; }
#define ON_ERROR_BREAK() if(hr != S_OK) { shell->ReportError(hr); break; }
#define EXIT_WITH_MESSAGE(msg) { shell->Error(msg); goto done; }

enum
{
    NULL_THREAD_ID = -1,
    NULL_PROCESS_ID = -1
};

enum ListType
{
    LIST_MODULES = 0,
    LIST_CLASSES,
    LIST_FUNCTIONS
};

#define SETBITULONG64( x ) ( (ULONG64)1 << (x) )

// Define max source file buckets for source file cache present for each module
#define MAX_SF_BUCKETS      9

// Modes used in the shell to control various global settings.
enum DebuggerShellModes
{
    DSM_DISPLAY_REGISTERS_AS_HEX        = 0x00000001,
    DSM_SEPARATE_CONSOLE                = 0x00000004,
    DSM_ENABLE_JIT_OPTIMIZATIONS        = 0x00000008,
    DSM_SHOW_CLASS_LOADS                = 0x00000020,
    DSM_SHOW_MODULE_LOADS               = 0x00000040,
    DSM_SHOW_UNMANAGED_TRACE            = 0x00000080,
    DSM_IL_NATIVE_PRINTING              = 0x00000100,
    DSM_SHOW_ARGS_IN_STACK_TRACE        = 0x00000200,
    DSM_UNMAPPED_STOP_PROLOG            = 0x00000400,
    DSM_UNMAPPED_STOP_EPILOG            = 0x00000800,
    DSM_UNMAPPED_STOP_ALL               = 0x00002000,
    DSM_INTERCEPT_STOP_CLASS_INIT       = 0x00004000,
    DSM_INTERCEPT_STOP_EXCEPTION_FILTER = 0x00008000,
    DSM_INTERCEPT_STOP_SECURITY         = 0x00010000,
    DSM_INTERCEPT_STOP_CONTEXT_POLICY   = 0x00020000,
    DSM_INTERCEPT_STOP_INTERCEPTION     = 0x00040000,
    DSM_INTERCEPT_STOP_ALL              = 0x00080000,
    DSM_SHOW_APP_DOMAIN_ASSEMBLY_LOADS  = 0x00100000,
    DSM_ENHANCED_DIAGNOSTICS            = 0x00200000,
    DSM_SHOW_MODULES_IN_STACK_TRACE     = 0x00400000,
    DSM_LOGGING_MESSAGES                = 0x01000000,
    DSM_DUMP_MEMORY_IN_BYTES            = 0x02000000,
    DSM_SHOW_SUPERCLASS_ON_PRINT        = 0x04000000,
    DSM_SHOW_STATICS_ON_PRINT           = 0x08000000,
    DSM_SHOW_TYARGS_IN_STACK_TRACE      = 0x20000000,
    DSM_ALLOW_JMC_STEPPING              = 0x40000000,
    DSM_ENABLE_ENC                      = 0x80000000,

    DSM_MAXIMUM_MODE             = 27, // count of all modes, not a mask.
    DSM_INVALID_MODE             = 0x00000000,
    DSM_DEFAULT_MODES            = DSM_DISPLAY_REGISTERS_AS_HEX |
                                   DSM_SHOW_ARGS_IN_STACK_TRACE |
                                   DSM_SHOW_MODULES_IN_STACK_TRACE,
};

// A helper function which will return the generic interface for
// either the appdomain or process.
void GetControllerInterface(ICorDebugAppDomain *pAppDomain, ICorDebugController ** ppController);

// Structure used to define information about debugger shell modes.
struct DSMInfo
{
    DebuggerShellModes  modeFlag;
    WCHAR              *name;
    WCHAR              *onDescription;
    WCHAR              *offDescription;
    WCHAR              *generalDescription;
    WCHAR              *descriptionPad;
};


/* ------------------------------------------------------------------------- *
 * Forward declarations
 * ------------------------------------------------------------------------- */

class DebuggerBreakpoint;
class DebuggerCodeBreakpoint;
class DebuggerSourceCodeBreakpoint;
class DebuggerModule;
class DebuggerUnmanagedThread;
class DebuggerManagedThread;
class DebuggerSourceFile;
class DebuggerFunction;
class DebuggerFilePathCache;
class ModuleSourceFile;
class StepDebuggerCommand;


//---------------------------------------------------------------------------
// Simple release holder. This has the same semantics as src\inc\Holder.h
//---------------------------------------------------------------------------

template <class T>
class ReleaseHolder
{
public:
    ReleaseHolder()
    {
        m_Ptr = NULL;
    }

    FORCEINLINE ~ReleaseHolder()
    {
        Clear();
    }

    FORCEINLINE void Clear()
    {
        if (m_Ptr != NULL)
        {
            m_Ptr->Release();
        }
        m_Ptr = NULL;
    }

    FORCEINLINE void Assign(T * p)
    {
        if (p == m_Ptr)
        {
            return;
        }

        Clear();
        m_Ptr = p;
        if (p != NULL)
        {
            p->AddRef();
        }

    }

    FORCEINLINE operator T*() const
    {
        return m_Ptr;
    }

    FORCEINLINE T** operator & ()
    {
        // We allow getting the address so we can pass it in as an outparam.
        // But if we have a non-null m_Ptr, then it may get silently overwritten,
        // and thus we'll lose the chance to call release on it.
        // So we'll just avoid that pattern and assert to enforce it.
        _ASSERTE(m_Ptr == NULL);
        return &m_Ptr;
    }

    FORCEINLINE T* operator->()
    {
        return m_Ptr;
    }

    FORCEINLINE int operator==(T* p)
    {
        return (m_Ptr == p);
    }

    FORCEINLINE int operator!= (T* p)
    {
        return (m_Ptr != p);
    }

protected:
    // Don't allow copy ctor.
    ReleaseHolder(ReleaseHolder<T> & other);
    void operator=(ReleaseHolder<T> & other);

protected:
    T* m_Ptr;
};


/* ------------------------------------------------------------------------- *
 * Debugger FilePathCache
 * This class keeps track of the fully qualified filename for each module
 * for files which were opened as a result of hitting a breakpoint, stack
 * trace, etc. This will be persisted for later runs of the debugger.
 * ------------------------------------------------------------------------- */
class DebuggerFilePathCache
{
private:
    CHAR            *m_rstrPath [MAX_PATH_ELEMS];
    int              m_iPathCount;
    CHAR            *m_rpstrModName [MAX_CACHE_ELEMS];
    ISymUnmanagedDocument    *m_rDocs [MAX_CACHE_ELEMS];
    CHAR            *m_rpstrFullPath [MAX_CACHE_ELEMS];
    int              m_iCacheCount;

    WCHAR            m_szExeName [MAX_PATH];

public:
    // Constructor
    DebuggerFilePathCache()
    {
        for (int i=0; i<MAX_PATH_ELEMS; i++)
            m_rstrPath [i] = NULL;
        m_iPathCount = 0;

        m_iCacheCount = 0;

        m_szExeName [0] = L'\0';
    }

    // Destructor
    ~DebuggerFilePathCache()
    {
        int i;
        for (i=0; i<m_iPathCount; i++)
            delete [] m_rstrPath [i];

        for (i=0; i<m_iCacheCount; i++)
        {
            delete [] m_rpstrModName [i];
            delete [] m_rpstrFullPath [i];
        }
    }

    HRESULT Init (void);
    HRESULT InitPathArray (__in_z WCHAR *pstrName);
    int   GetPathElemCount (void) { return m_iPathCount;}
    CHAR* GetPathElem (int iIndex) { return m_rstrPath [iIndex];}
    int   GetFileFromCache (DebuggerModule *pModule, ISymUnmanagedDocument *doc,
                            __deref_out_z_opt CHAR **ppstrFName);
    BOOL UpdateFileCache (DebuggerModule *pModule, ISymUnmanagedDocument *doc,
                          __inout_z_opt CHAR *pFullPath);
};

class ModuleSearchElement
{
private:
    DebuggerString szName;
    ModuleSearchElement *pNext;

public:
    ModuleSearchElement()
    {
        pNext = NULL;
    }

    ~ModuleSearchElement()
    {
    }

    void SetName (const WCHAR *szModName)
    {
        szName.CopyFrom(szModName);
    }

    const WCHAR *GetName (void) { return szName;}

    void SetNext (ModuleSearchElement *pEle) { pNext = pEle;}
    ModuleSearchElement *GetNext (void) { return pNext;}
};

class ModuleSearchList
{
private:
    ModuleSearchElement *pHead;
public:
    ModuleSearchList()
    {
        pHead = NULL;
    }

    ~ModuleSearchList()
    {
        ModuleSearchElement *pTemp;
        while (pHead)
        {
            pTemp = pHead;
            pHead = pHead->GetNext();
            delete pTemp;
        }
    }

    BOOL ModuleAlreadySearched (const WCHAR *szModName)
    {
        ModuleSearchElement *pTemp = pHead;
        while (pTemp)
        {
            const WCHAR *pszName = pTemp->GetName();
            if (pszName)
            {
                if (0 == wcscmp(pszName, szModName))
                {
                    return TRUE;
                }
            }
            pTemp = pTemp->GetNext();
        }

        return FALSE;
    }
    void AddModuleToAlreadySearchedList (__in_z WCHAR *szModName)
    {
        ModuleSearchElement *pTemp = new ModuleSearchElement;
        if (pTemp)
        {
            pTemp->SetName(szModName);
            pTemp->SetNext(pHead);
            pHead = pTemp;
        }
    }
};



/* ------------------------------------------------------------------------- *
 * Base class
 * ------------------------------------------------------------------------- */

class DebuggerBase
{
public:
    DebuggerBase(ULONG_PTR token) : m_token(token)
    {

    }
    virtual ~DebuggerBase()
    {

    }

    ULONG_PTR GetToken()
    {
        return(m_token);
    }

protected:
    ULONG_PTR   m_token;
};

/* ------------------------------------------------------------------------- *
 * HashTable class
 * ------------------------------------------------------------------------- */

struct DebuggerHashEntry
{
    FREEHASHENTRY entry;
    DebuggerBase* pBase;
};

class DebuggerHashTable : private CHashTableAndData<CNewData>
{
    friend class DebuggerRCThread;
private:
    bool    m_initialized;

    BOOL Cmp(const BYTE* pc1, const HASHENTRY* pc2)
    {
        return((ULONG_PTR)pc1) != ((DebuggerHashEntry*)pc2)->pBase->GetToken();
    }

    USHORT HASH(ULONG_PTR token)
    {
        return(USHORT) (token ^ (token>>16));
    }

    BYTE* KEY(ULONG_PTR token)
    {
        return(BYTE* )(ULONG_PTR) token;
    }

public:

    DebuggerHashTable(USHORT size)
    : CHashTableAndData<CNewData>(size), m_initialized(false)
    {

    }
    ~DebuggerHashTable();

    HRESULT AddBase(DebuggerBase* pBase);
    DebuggerBase* GetBase(ULONG_PTR token);
    BOOL RemoveBase(ULONG_PTR token);
    void RemoveAll();
    bool IsInitialized();
    DebuggerBase* FindFirst(HASHFIND* find);
    DebuggerBase* FindNext(HASHFIND* find);
};

/* ------------------------------------------------------------------------- *
 * Debugger Stepper Table class
 * ------------------------------------------------------------------------- *

    class StepperHashTable: It's possible for there to be multiple,
    outstanding,uncompleted steppers within the debuggee, and any of them
    can complete after a given 'continue'. Thus, instead of a 'last stepper'
    field off of the thread object, we really need a table of active steppers
    off the thread object, which is what a StepperHashTable is.
*/
struct StepperHashEntry
{
    FREEHASHENTRY       entry;
    ICorDebugStepper*   pStepper;
};

class StepperHashTable : private CHashTableAndData<CNewData>
{
private:
    bool    m_initialized;

    BOOL Cmp(const BYTE* pc1, const HASHENTRY* pc2)
    {
        return((ICorDebugStepper*)pc1) != ((StepperHashEntry*)pc2)->pStepper;
    }

    USHORT HASH(ICorDebugStepper *pStepper)
    {
        return(USHORT) ((UINT)(UINT_PTR)pStepper ^ ((UINT)(UINT_PTR)pStepper >>16));
    }

    BYTE* KEY(ICorDebugStepper *pStepper)
    {
        return(BYTE* ) pStepper;
    }

public:

    StepperHashTable(USHORT size)
    : CHashTableAndData<CNewData>(size), m_initialized(false)
    {
    }
    ~StepperHashTable();

    HRESULT Initialize(void);

    HRESULT AddStepper(ICorDebugStepper *pStepper);
    //Also does an AddRef, of course

    bool IsStepperPresent(ICorDebugStepper *pStepper);

    BOOL RemoveStepper(ICorDebugStepper *pStepper);

    void ReleaseAll(); //will go through & release all the steppers
                       //in the table, twice, then delete them.  This should deallocate
                       //them both from the table & from cordbg

    ICorDebugStepper *FindFirst(HASHFIND* find);
    ICorDebugStepper *FindNext(HASHFIND* find);
};


class DebuggerManagedThread : public DebuggerBase
{
public:

    DebuggerManagedThread(DWORD_PTR dwThreadId,ICorDebugThread *icdThread) :
        DebuggerBase(dwThreadId), m_thread(icdThread),
        m_steppingForStartup(false)
    {
        fSuperfluousFirstStepCompleteMessageSuppressed = false;

        _ASSERTE( sizeof(dwThreadId) == sizeof(m_token));

        if (m_thread != NULL )
            m_thread->AddRef();


        m_pendingSteppers = new StepperHashTable(7);
    }

    virtual ~DebuggerManagedThread()
    {
        _ASSERTE( m_thread != NULL );
        m_thread->Release();
        m_thread = NULL;

        if (m_pendingSteppers != NULL )
        {
            m_pendingSteppers->ReleaseAll();
        }

    }

    StepperHashTable*      m_pendingSteppers;
    ICorDebugThread*       m_thread;
    bool                   fSuperfluousFirstStepCompleteMessageSuppressed;
    ReleaseHolder<ICorDebugEval> m_lastFuncEval;
    bool                   m_steppingForStartup;
};

/* ------------------------------------------------------------------------- *
 * DebuggerShell class
 * ------------------------------------------------------------------------- */

// Describe a list of exceptions to skip and handle.
struct ExceptionHandlingInfo
{
    // This is an unresolved name. It may be set even when we don't have an active process.
    // It can be either a module shortname or a class name.
    DebuggerString          m_exceptionType;
    bool                    m_catch;
    ExceptionHandlingInfo  *m_next;
};

class NamedHandle
{
public:
    NamedHandle(WCHAR *pwzHandleName, ICorDebugHandleValue *pHandleValue);
    ~NamedHandle();

    WCHAR                *m_pwzHandleName;   // Name of handle
    ICorDebugHandleValue *m_pHandle;          // Handle value
    NamedHandle         *m_pNext;            // Next handle
};   // NamedHandle

// Dynamic Imports from mscoree.idl
typedef HRESULT (STDAPICALLTYPE *FPCORGetVersion)(LPWSTR pbBuffer, DWORD cchBuffer, DWORD* dwLength);

typedef HRESULT (STDAPICALLTYPE *FPCreateCordb)(int iDebuggerVersion, LPCWSTR szDebuggeeVersion, IUnknown ** ppCordb);


class DebuggerShell : public Shell
{
public:
    DebuggerShell(FILE* in, FILE* out);
    virtual ~DebuggerShell();

    HRESULT Init();
    HRESULT InitCor();

    CorDebugUnmappedStop ComputeStopMask( void );
    CorDebugIntercept    ComputeInterceptMask( void );
    bool                 SetJMCStepper(bool fJMC, ICorDebugStepper * pStepper);
    bool                 SetJMCStepperToDefault(ICorDebugStepper * pStepper);


    bool ReadLine(__inout_ecount(maxCount) WCHAR* buffer, int maxCount);

    virtual HRESULT Write(const WCHAR* buffer, ...);

    // WriteBigString will loop over the character array, calling Write on
    // subportions of it.  It may still fail, though.
    virtual HRESULT WriteBigString(__inout_ecount(count) WCHAR *s, ULONG32 count);
    virtual void Error(const WCHAR* buffer, ...);

    // Do a command once for every thread in the process
    virtual void DoCommandForAllThreads(const WCHAR *string);

    // Right now, this will return E_OUTOFMEMORY if it can't get enough space
    HRESULT CommonWrite(FILE *out, const WCHAR *buffer, va_list args);

    void AddCommands();

    void Kill();
    virtual void Run(bool fNoInitialContinue = false);
    void KillAllSteppers();
    void Stop(ICorDebugController *controller,
              ICorDebugThread* thread,
              DebuggerUnmanagedThread *unmanagedThread = NULL);
    HRESULT AsyncStop(ICorDebugController *controller,
                      DWORD dwTimeout = 500);
    void Continue(ICorDebugController* process,
                  ICorDebugThread* thread,
                  DebuggerUnmanagedThread *unmanagedThread = NULL,
                  BOOL fIsOutOfBand = FALSE);
    void Interrupt();
    ICorDebugProcess* GetCurrentProcess()
    {
        return m_currentProcess;
    }
    void SetTargetProcess(ICorDebugProcess* process);

    ICorDebugThread *GetCurrentThread()
    {
        return m_currentThread;
    }
    void SetCurrentThread(ICorDebugProcess* process, ICorDebugThread* thread,
                          DebuggerUnmanagedThread *unmanagedThread = NULL);
    void SetCurrentChain(ICorDebugChain* chain);
    void SetCurrentFrame(ICorDebugFrame* frame);
    void SetDefaultFrame();

    HRESULT PrintThreadState(ICorDebugThread* thread);
    HRESULT PrintChain(ICorDebugChain *chain, int *frameIndex = NULL,
                       int *iNumFramesToShow = NULL);
    HRESULT PrintFrame(ICorDebugFrame *frame);

    HRESULT EvaluateTypeExpressions(const WCHAR* startOfParse,
                                           ICorDebugILFrame* context,
                                           ICorDebugType **tyargArray,
                                           unsigned int *pTyargCount,
                                           const WCHAR **endOfParse);
    ICorDebugValue* EvaluateExpression(const WCHAR* exp, ICorDebugILFrame* context, bool silently = false);
    ICorDebugValue* EvaluateName(const WCHAR* name, ICorDebugILFrame* context,
                                 bool* unavailable);
    void PrintVariable(const WCHAR* name, ICorDebugValue* &value,
                       unsigned int indent, BOOL expandObjects);
    void PrintType(const WCHAR* name, ICorDebugType* value,
                           unsigned int indent);
    void PrintArrayVar(ICorDebugArrayValue *iarray,
                       const WCHAR* name,
                       unsigned int indent, BOOL expandObjects);
    void PrintStringVar(ICorDebugStringValue *istring,
                        const WCHAR* name,
                        unsigned int indent, BOOL expandObjects);
    void PrintObjectVar(ICorDebugObjectValue *iobject,
                        const WCHAR* name,
                        unsigned int indent, BOOL expandObjects);
    bool EvaluateAndPrintGlobals(const WCHAR *exp);
    void PrintGlobalVariable (mdFieldDef md,
                              __in_z WCHAR  *wszName,
                              DebuggerModule *dm);
    void DumpMemory(BYTE *pbMemory,
                    CORDB_ADDRESS ApparantStartAddr,
                    ULONG32 cbMemory,
                    ULONG32 WORD_SIZE,
                    ULONG32 iMaxOnOneLine,
                    BOOL showAddr);

    static DebuggerModule* ModuleOfType(ICorDebugType* type);
    static mdTypeDef TokenOfType(ICorDebugType* type);

    HRESULT ResolveClassName(const WCHAR *className,
                             DebuggerModule **pDM, mdTypeDef *pTD);
    HRESULT FindTypeDefByName(DebuggerModule *m,
                              const WCHAR *className,
                              mdTypeDef *pTD);
    HRESULT ResolveTypeRef(DebuggerModule *currentDM, mdTypeRef tr,
                           DebuggerModule **pDM, mdTypeDef *pTD);
    HRESULT ResolveQualifiedFieldName(ICorDebugType *itype,
                                      __in_z WCHAR *fieldName,
                                      ICorDebugType **pIClass,
                                      mdFieldDef *pFD,
                                      bool *pbIsStatic);

    HRESULT ResolveFullyQualifiedMethodName(const WCHAR *methodName,
                                            ICorDebugFunction **ppFunc,
                                            DebuggerFunction **ppDebuggerFunc,
                                            ICorDebugAppDomain * pAppDomainHint = NULL);

    HRESULT ResolveFullyQualifiedModuleName(const WCHAR * moduleName,
                                            ICorDebugModule **ppMod,
                                            ICorDebugAppDomain * pAppDomainHint = NULL);

    HRESULT GetArrayIndicies(__inout_z __deref_inout WCHAR **pp,
                             ICorDebugILFrame *context,
                             ULONG32 rank,
                             ULONG32 *indicies);

    HRESULT StripReferences(ICorDebugValue **ppValue, bool printAsYouGo);
    BOOL PrintCurrentSourceLine(unsigned int around);
    virtual void ActivateSourceView(DebuggerSourceFile *psf, unsigned int lineNumber);
    BOOL PrintCurrentInstruction(unsigned int around,
                                 int          offset,
                                 DWORD        startAddr);
    BOOL PrintCurrentUnmanagedInstruction(unsigned int around,
                                          int          offset,
                                          DWORD_PTR    startAddr);
    void PrintIndent(unsigned int level);
    void PrintVarName(const WCHAR* name);
    void PrintBreakpoint(DebuggerBreakpoint* breakpoint);

    void PrintThreadPrefix(ICorDebugThread* pThread, bool forcePrint = false);
    HRESULT  StepStart(ICorDebugThread *pThread,
                   ICorDebugStepper* pStepper);
    void StepNotify(ICorDebugThread* pThread,
                    ICorDebugStepper* pStepper);

    DebuggerBreakpoint* FindBreakpoint(SIZE_T id);
    void RemoveAllBreakpoints();
    virtual void OnActivateBreakpoint(DebuggerBreakpoint *pb);
    virtual void OnDeactivateBreakpoint(DebuggerBreakpoint *pb);
    virtual void OnUnBindBreakpoint(DebuggerBreakpoint *pb, DebuggerModule *pm);
    virtual void OnBindBreakpoint(DebuggerBreakpoint *pb, DebuggerModule *pm);

    BOOL ReadSourcesPath(DebuggerString * currentPath);
    BOOL WriteSourcesPath(__in_z WCHAR* newPath);
    BOOL ReadDebuggerModes(void);
    BOOL WriteDebuggerModes(void);
    BOOL AppendSourcesPath(const WCHAR *newpath);

    DebuggerModule* ResolveModule(ICorDebugModule *pIModule);
    DebuggerSourceFile* LookupSourceFile(const WCHAR* name);
    mdTypeDef LookupClass(const WCHAR* name);

    virtual HRESULT ResolveSourceFile(DebuggerSourceFile *pf,
                                      __in_z CHAR *pszPath,
                                      __out_z __inout_ecount(iMaxLen) CHAR *pszFullyQualName,
                                      int iMaxLen,
                                      bool bChangeOfFile);
    virtual ICorDebugManagedCallback *GetDebuggerCallback();

    bool SkipCompilerStubs(ICorDebugAppDomain *pAppDomain,
                           ICorDebugThread *pThread);

    BOOL SkipProlog(ICorDebugAppDomain *pAD,
                    ICorDebugThread *thread,
                    bool gotFirstThread);


    virtual WCHAR *GetJITLaunchCommand(void)
    {
#ifdef PLATFORM_UNIX
        return L"cordbg !a 0x%x";
#else
        return L"cordbg.exe !a 0x%x";
#endif
    }

    void HandleUnmanagedThreadCreate(DWORD dwThreadId, HANDLE hThread);

    // !!! Move to process object
    HRESULT AddManagedThread( ICorDebugThread *icdThread,
                              DWORD_PTR dwThreadId )
    {
        DebuggerManagedThread *pdt = new DebuggerManagedThread( dwThreadId,
                                                                icdThread);
        if (pdt == NULL)
            return E_OUTOFMEMORY;

        if (FAILED(pdt->m_pendingSteppers->Initialize()))
            return E_OUTOFMEMORY;

        return m_managedThreads.AddBase( (DebuggerBase *)pdt );
    }

    DebuggerManagedThread *GetManagedDebuggerThread(
                                     ICorDebugThread *icdThread )
    {
        DWORD dwThreadId = 0;

        HRESULT hr;
        hr = icdThread->GetID(&dwThreadId);
        _ASSERTE( !FAILED(hr));

        return (DebuggerManagedThread *)m_managedThreads.GetBase( dwThreadId );
    }

    BOOL RemoveManagedThread( DWORD_PTR dwThreadId )
    {
        return m_managedThreads.RemoveBase( dwThreadId );
    }


    int GetUserSelection(DebuggerModule *rgpDebugModule[],
                         __in_ecount(iModuleCount) LPWSTR rgpstrFileName[][MAX_FILE_MATCHES_PER_MODULE],
                         int rgiCount[],
                         int iModuleCount,
                         int iCumulCount);

    BOOL ChangeCurrStackFile (const WCHAR *fileName);
    BOOL UpdateCurrentPath (const WCHAR *args);
    void ListAllModules (ListType lt);
    void ListAllGlobals (DebuggerModule *m);
    void ListAllSteppers();

    bool MatchAndPrintSymbols (const WCHAR *pszArg, BOOL fSymbol, bool fSilently = false);

    FILE *GetM_in(void) { return m_in; };
    void  PutM_in(FILE *f) { m_in = f; };
    HRESULT NotifyModulesOfEnc(ICorDebugModule *pModule, IStream *pSymStream);

    void ClearDebuggeeState(void); //when we Restart, for example, we'll want to
                                   //reset some flags.

    HRESULT HandleSpecificException(const WCHAR *exType, bool shouldCatch);
    bool ShouldHandleSpecificException(ICorDebugValue *pException);
    void ClearExceptionHandlingList();

    HRESULT GetTypeName(ICorDebugClass *iclass,
                        ULONG bufLength,
                        __inout_ecount(bufLength) WCHAR* nameBuf,
                        ULONG *nameSize);

    HRESULT GetTypeName(ICorDebugType *iclass,
                        ULONG ufLength,
                        __inout_ecount(bufLength) WCHAR* nameBuf,
                        ULONG *nameSize);

    HRESULT AddString(__in_z CHAR* s,
                      ULONG bufLength,
                      __inout_z __inout_ecount(bufLength) WCHAR* nameBuf,
                      ULONG *nameSize);

    NamedHandle *FindHandleWithName(__in_z WCHAR *pwzHandleName);
    void AddNamedHandle(NamedHandle *pHandle);
    void ClearHandles();
    NamedHandle *GetHandleList() { return m_pHandleNameList;}
    HRESULT DeleteNamedHandle(NamedHandle *pHandle);

    HRESULT RecycleCorForVersion(__in_z __in_opt WCHAR *strVersion);

    void GetDefaultVersion(__inout_ecount(cSize) WCHAR *szVersionOut, DWORD cSize);
    bool SetDefaultVersion(const WCHAR * szVersion);

    void GetForceVersion(__inout_ecount(cSize) WCHAR *szVersionOut, DWORD cSize);
    bool SetForceVersion(const WCHAR * szVersion);

    HRESULT GetVersionForExe(__in_z WCHAR *szExeName,
                             __inout_ecount(cSize) WCHAR *szVersionOut,
                             DWORD cSize);

    HRESULT GetVersionFromPid(DWORD pid,
                              __inout_ecount(cSize) WCHAR *szVersionOut,
                              DWORD cSize);

    void GetPublish(ICorPublish ** ppPublish);
private:
    FILE*                  m_in;
    FILE*                  m_out;
    NamedHandle*           m_pHandleNameList;

    HMODULE                m_hMscoree;
    FPCORGetVersion        m_fpCorGetVersion;
    FPCreateCordb           m_fpCreateCordb;


    // Do we recycle the Cor versions or not?
    bool                   m_fLockCor;

    // Default version is always set, it's what we use if we don't know what else to do.
    // Force-version, if set, overrides everything else and must be explictly set.
    WCHAR                  m_strDefaultVersion[MAX_PATH];
    WCHAR                  m_strForceVersion[MAX_PATH];
public:
    ICorDebug*             m_cor;

    ICorDebugProcess*      m_targetProcess;
    bool                   m_targetProcessHandledFirstException;

    ICorDebugProcess*      m_currentProcess;
    ICorDebugThread*       m_currentThread;
    ICorDebugThread2*       m_currentThread2;
    ICorDebugChain*        m_currentChain;
    ReleaseHolder<ICorDebugILFrame> m_currentFrame;
    ReleaseHolder<ICorDebugFrame>   m_rawCurrentFrame;

    DebuggerUnmanagedThread* m_currentUnmanagedThread;

    DWORD                  m_lastThread;
    ICorDebugStepper*      m_lastStepper;

    bool                   m_showSource;
    bool                   m_silentTracing;

    DebuggerString         m_currentSourcesPath;

    HANDLE                 m_stopEvent;
    HANDLE                 m_hProcessCreated;
    bool                   m_stop;
    bool                   m_quit;

    bool                   m_gotFirstThread;

    DebuggerBreakpoint*    m_breakpoints;
    SIZE_T                 m_lastBreakpointID;

    DebuggerHashTable      m_modules;

    DebuggerHashTable      m_unmanagedThreads;
    DebuggerHashTable      m_managedThreads;


    WCHAR*                 m_lastRunArgs;

    bool                   m_catchException;
    bool                   m_catchUnhandled;
    bool                   m_catchClass;
    bool                   m_catchModule;
    bool                   m_catchThread;

    bool                   m_needToSkipCompilerStubs;
    DWORD                  m_rgfActiveModes;
    bool                   m_invalidCache; //if true, we've affected the left
                            //  side & anything that has cached information
                            //  should refresh

    DebuggerFilePathCache  m_FPCache;

    DEBUG_EVENT            m_lastUnmanagedEvent;

    bool                   m_unmanagedDebuggingEnabled;

    ULONG                  m_cEditAndContinues;

    ReleaseHolder<ICorDebugEval> m_pCurrentEval;
    bool                   m_rudeAbortNextTime;

    // This indicates whether or not a ctrl-break will do anything
    bool                   m_enableCtrlBreak;
    // This indicates whether or not a looping command should stop (like s 1000)
    bool                   m_stopLooping;

    ExceptionHandlingInfo *m_exceptionHandlingList;
};

/* ------------------------------------------------------------------------- *
 * Breakpoint class
 * ------------------------------------------------------------------------- */

class DebuggerBreakpoint
{
public:
    DebuggerBreakpoint(const WCHAR* name, SIZE_T nameLength, SIZE_T functionVersion, SIZE_T index, DWORD threadID);
    DebuggerBreakpoint(DebuggerFunction* f, SIZE_T functionVersion, SIZE_T offset, DWORD threadID);
    DebuggerBreakpoint(DebuggerSourceFile* file, SIZE_T functionVersion, SIZE_T lineNumber, DWORD threadID);

    ~DebuggerBreakpoint();

    // Create/remove a breakpoint.
    bool Bind(DebuggerModule* m_module, ISymUnmanagedDocument *doc);
    bool BindUnmanaged(ICorDebugProcess *m_process,
                       DWORD_PTR moduleBase = 0);
    void Unbind();

    // Enable/disable an active breakpoint.
    void Activate();
    void Deactivate();

    // Leave bp active; tear down or reset CLR bp object.
    void Detach();
    // Detaches the break point from the specified module
    void DetachFromModule(DebuggerModule * pModule);
    void Attach();

    bool Match(ICorDebugBreakpoint* ibreakpoint);
    bool MatchUnmanaged(CORDB_ADDRESS address);

    SIZE_T GetId (void) {return m_id;}
    SIZE_T GetIndex (void) { return m_index;}
    WCHAR *GetName (void) { return m_name;}
    void UpdateName (__in_z WCHAR *pstrName);

    void ChangeSourceFile (const WCHAR *filename);

    DebuggerBreakpoint*          m_next;
    SIZE_T                       m_id;

    WCHAR*                       m_name;
    // May be NULL if no module name was specified.
    WCHAR*                       m_moduleName;
    SIZE_T                       m_index;
    DWORD                        m_threadID;

    bool                         m_active;

    bool                         m_managed;

    ISymUnmanagedDocument                *m_doc;

    ICorDebugProcess    *m_process;
    CORDB_ADDRESS        m_address;
    BYTE                 m_patchedValue;
    DWORD                m_skipThread;
    CORDB_ADDRESS        m_unmanagedModuleBase;
    bool                 m_deleteLater;
    SIZE_T               m_functionVersion;

public:
    struct BreakpointModuleNode
    {
        DebuggerModule *m_pModule;
        BreakpointModuleNode *m_pNext;
    };

    // Will be a list of modules for which this breakpoint is
    // associated.  This is necessary because the same module
    // may be loaded into separate AppDomains, but the breakpoint
    // should still be valid for all instances of the module.
    BreakpointModuleNode *m_pModuleList;

    // This will return true if this breakpoint is associated
    // with the pModule argument
    bool IsBoundToModule(DebuggerModule *pModule);

    // This will add the provided module to the list of bound
    // modules
    bool AddBoundModule(DebuggerModule *pModule);

    // This will remove the specified module from the list of
    // bound modules
    bool RemoveBoundModule(DebuggerModule *pModule);

private:
    void CommonCtor(void);
    void Init(DebuggerModule* module,
              bool bProceed,
              __in_z __in_opt WCHAR *szModuleName);

    void ApplyUnmanagedPatch();
    void UnapplyUnmanagedPatch();
};


//
// DebuggerVarInfo
//
// Holds basic information about type variables, local variables, method arguments,
// and class static and instance variables.
//
class DebuggerVarInfo
{
    LPCSTR                 m_name;

public:
    void   SetName(LPCSTR szName); // { m_name = szName;}
    LPCSTR GetName() { return m_name; }

    PCCOR_SIGNATURE        sig;  // GENERICS: null for type variables
    unsigned long          varNumber;  // placement info for IL code

    DebuggerVarInfo() : m_name(NULL), sig(NULL), varNumber(0)
                         {}
};


/* ------------------------------------------------------------------------- *
 * Class class
 * ------------------------------------------------------------------------- */

class DebuggerClass : public DebuggerBase
{
public:
    DebuggerClass (ICorDebugClass *pClass);
    ~DebuggerClass ();

    void SetName(__in_z __in_opt WCHAR *pszName,
                 __in_z __in_opt WCHAR *pszNamespace);
    WCHAR *GetName (void);
    WCHAR *GetNamespace (void);

private:
    WCHAR   *m_szName;
    WCHAR   *m_szNamespace;
};


/* ------------------------------------------------------------------------- *
 * Module class
 * ------------------------------------------------------------------------- */

class DebuggerModule : public DebuggerBase
{
public:
    DebuggerModule(ICorDebugModule* module);
    ~DebuggerModule();

    HRESULT Init(const WCHAR *pSearchPath);

    DebuggerSourceFile* LookupSourceFile(const WCHAR* name);
    DebuggerSourceFile* ResolveSourceFile(ISymUnmanagedDocument *doc);

    DebuggerFunction* ResolveFunction(mdMethodDef mb,
                                      ICorDebugFunction* iFunction);
    DebuggerFunction* ResolveFunction(ISymUnmanagedMethod *method,
                                      ICorDebugFunction* iFunction);

    static DebuggerModule* FromCorDebug(ICorDebugModule* module);

    IMetaDataImport *GetMetaData(void)
    {
        return m_pIMetaDataImport;
    }

    // This can return NULL if running on a down-level runtime.
    IMetaDataImport2 *GetMetaData2(void)
    {
        return m_pIMetaDataImport2;
    }

    ISymUnmanagedReader *GetSymbolReader(void)
    {
        return m_pISymUnmanagedReader;
    }

    ICorDebugModule *GetICorDebugModule(void)
    {
        return (ICorDebugModule*)(ULONG_PTR)m_token;
    }

    HRESULT LoadSourceFileNames (void);
    void DeleteModuleSourceFiles(void);

    HRESULT MatchStrippedFNameInModule (
                    __in_z WCHAR *pstrFileName,
                    __deref_inout_ecount(*piCount) WCHAR **ppstrMatchedNames,
                    ISymUnmanagedDocument **ppDocs,
                    int *piCount);
    HRESULT MatchFullFileNameInModule (__in_z WCHAR *pstrFileName,
                                       ISymUnmanagedDocument **ppDocs);

    ISymUnmanagedDocument *FindDuplicateDocumentByURL(ISymUnmanagedDocument *pDoc);
    ISymUnmanagedDocument *SearchForDocByString(__in_z WCHAR *szUrl);

    BOOL PrintMatchingSymbols (const WCHAR *szSearchString, const WCHAR *szModName);
    BOOL PrintGlobalVariables (const WCHAR *szSearchString,
                               const WCHAR *szModName,
                               DebuggerModule *dm);

    void    SetName (__in_z __in_opt WCHAR *pszFullName);
    WCHAR*  GetName (void) { return m_szName.GetData();}
    WCHAR * GetShortName() { return m_szShortName.GetData(); }

    HRESULT UpdateSymbols(IStream *pSymbolStream);

    ICorDebugAppDomain *    GetIAppDomain()
    {
        return  m_pIAppDomain;
    }
public:
    IMetaDataImport        *m_pIMetaDataImport;
    IMetaDataImport2       *m_pIMetaDataImport2;

    // The symbol reader holds a file lock to the pdb. That lock is not released until the symbol reader, and
    // all objects obtained from the reader (Eg ISymUnmanagedMethod, ISymUnmanagedDocument), are released.
    ISymUnmanagedReader    *m_pISymUnmanagedReader;

    DebuggerHashTable       m_sourceFiles;
    DebuggerHashTable       m_functions;
    DebuggerHashTable       m_functionsByIF;
    DebuggerHashTable       m_loadedClasses;

    DebuggerCodeBreakpoint* m_breakpoints;

private:
    ICorDebugAppDomain      *m_pIAppDomain; // we own a reference to this.
    ModuleSourceFile        *m_pModSourceFile [MAX_SF_BUCKETS];
    bool                     m_fSFNamesLoaded;
    DebuggerString           m_szName;
    DebuggerString           m_szShortName;
    ULONG                   m_EnCLastUpdated;
};

class DebuggerCodeBreakpoint
{
public:
    DebuggerCodeBreakpoint(int breakpointID,
                           DebuggerModule* module,
                           DebuggerFunction* function,
                           SIZE_T functionVersion,
                           SIZE_T offset, BOOL il,
                           DWORD threadID);
    DebuggerCodeBreakpoint(int breakpointID,
                           DebuggerModule* module,
                           DebuggerSourceCodeBreakpoint* parent,
                           DebuggerFunction* function,
                           SIZE_T functionVersion,
                           SIZE_T offset, BOOL il,
                           DWORD threadID);

    virtual ~DebuggerCodeBreakpoint();

    virtual bool Activate();
    virtual void Deactivate();

    virtual bool Match(ICorDebugBreakpoint* ibreakpoint);

    virtual void Print();

    ICorDebugFunction *GetFunctionVersion(DebuggerFunction *currentFunction,
                                          SIZE_T functionVersion);

public:
    DebuggerCodeBreakpoint      *m_next;
    int                         m_id;
    DebuggerModule              *m_module;
    DebuggerFunction            *m_function;
    SIZE_T                       m_functionVersion;
    SIZE_T                      m_offset;
    BOOL                        m_il;
    DWORD                       m_threadID;

    ICorDebugFunctionBreakpoint* m_ibreakpoint;

    DebuggerSourceCodeBreakpoint* m_parent;
};

class DebuggerSourceCodeBreakpoint : public DebuggerCodeBreakpoint
{
public:
    DebuggerSourceCodeBreakpoint(int breakpointID,
                                 DebuggerSourceFile* file,
                                 SIZE_T functionVersion,
                                 SIZE_T lineNumber,
                                 DWORD threadID);
    ~DebuggerSourceCodeBreakpoint();

    bool Activate();
    void Deactivate();
    bool Match(ICorDebugBreakpoint *ibreakpoint);
    void Print();

public:
    DebuggerSourceFile*     m_file;
    SIZE_T                  m_lineNumber;

    DebuggerCodeBreakpoint* m_breakpoints;
    bool                    m_initSucceeded;
};

/* ------------------------------------------------------------------------- *
 * SourceFile class
 * ------------------------------------------------------------------------- */

class DebuggerSourceFile : public DebuggerBase
{
public:
    //-----------------------------------------------------------
    // Create a DebuggerSourceFile from a scope and a SourceFile
    // token.
    //-----------------------------------------------------------
    DebuggerSourceFile(DebuggerModule* m, ISymUnmanagedDocument *doc);
    ~DebuggerSourceFile();

    //-----------------------------------------------------------
    // Given a line find the closest line which has code
    //-----------------------------------------------------------
    unsigned int FindClosestLine(unsigned int line, bool silently);

    const WCHAR* GetName(void)
    {
        return(m_name);
    }
    const WCHAR* GetPath(void)
    {
        return(m_path);
    }
    DebuggerModule* GetModule()
    {
        return(m_module);
    }

    //-----------------------------------------------------------
    // Methods to load the text of a source file and provide
    // access to it a line at a time.
    //-----------------------------------------------------------
    BOOL LoadText(const WCHAR* path, bool bChangeOfName);
    BOOL ReloadText(const WCHAR* path, bool bChangeOfName);
    unsigned int TotalLines(void)
    {
        return(m_totalLines);
    }
    const WCHAR* GetLineText(unsigned int lineNumber)
    {
        _ASSERTE((lineNumber > 0) && (lineNumber <= m_totalLines));
        return(m_lineStarts[lineNumber - 1]);
    }

    ISymUnmanagedDocument   *GetDocument (void) {return (ISymUnmanagedDocument*)(ULONG_PTR)m_token;}

public:
    ISymUnmanagedDocument *m_doc;
    DebuggerModule*        m_module;
    DebuggerString         m_name;
    DebuggerString         m_path;

    // Pointer indexes into m_source for each line.
    unsigned int           m_totalLines;
    WCHAR**                m_lineStarts;

    // Single string buffer containing the entire file.
    DebuggerString         m_source;
    BOOL                   m_sourceTextLoaded;
    BOOL                   m_allBlocksLoaded;
    BOOL                   m_sourceNotFound;
};

/* ------------------------------------------------------------------------- *
 * DebuggerVariable struct
 * ------------------------------------------------------------------------- */

// Holds basic info about local variables and method arguments within
// the debugger. This is really only the name and variable number. No
// signature is required.
struct DebuggerVariable
{
    DebuggerString m_name;
    ULONG32        m_varNumber;

    DebuggerVariable() : m_varNumber(0) {}

    ~DebuggerVariable()
    {
    }
};

/* ------------------------------------------------------------------------- *
 * Function class
 * ------------------------------------------------------------------------- */

// DebuggerFunctionILRange
//
// This class encapsulates a range of IL instructions for stepping.
//
// Each IL Range is associated with a particular line of source in a particular file. The DebuggerFunctionILRange class
// stores these pieces of information.
//

class DebuggerFunctionILRange
{
public:
    DebuggerFunctionILRange(const COR_DEBUG_STEP_RANGE& range, ISymUnmanagedDocument* document, ULONG32 sourceLine);
    ~DebuggerFunctionILRange();

    // Returns a copy of the IL range associated with this file
    COR_DEBUG_STEP_RANGE GetRange() const;

    // This accessor does NOT AddRef the Document returned.
    ISymUnmanagedDocument* GetDocument();
    ULONG32 GetSourceLine() const;

    // Returns true if and only if "ip" is within this IL range
    BOOL Contains(UINT_PTR ip) const;

private:
    COR_DEBUG_STEP_RANGE    m_range;
    ISymUnmanagedDocument*  m_document;
    ULONG32                 m_sourceLine;
};

class DebuggerFunction : public DebuggerBase
{
public:
    //-----------------------------------------------------------
    // Create from scope and member tokens.
    //-----------------------------------------------------------
    DebuggerFunction(DebuggerModule* m, mdMethodDef md,
                     ICorDebugFunction* iFunction);
    ~DebuggerFunction();

    HRESULT Init(void);

    static const unsigned int       kNoSourceForIL = 0;
    HRESULT FindLineFromIP(UINT_PTR ip,
                           DebuggerSourceFile** sourceFile,
                           unsigned int* line);
    void FindLineClosestToIP(UINT_PTR ip,
                             DebuggerSourceFile** sourceFile,
                             unsigned int* line);

    void GetStepRangesFromIP(UINT_PTR ip,
                             COR_DEBUG_STEP_RANGE** range,
                             SIZE_T* rangeCount);

    //-----------------------------------------------------------
    // These allow you to get the count of method argument and
    // get access to the info for each individual argument.
    // Ownership of the DebugVarInfo returned from GetArgumentAt
    // is retained by the DebugFunction.
    //-----------------------------------------------------------
    unsigned int GetArgumentCount(void)
    {
        return(m_argCount);
    }
    DebuggerVarInfo* GetArgumentAt(unsigned int index)
    {
        if (m_arguments)
        {
            if (index < m_argCount)
            {
                return(&m_arguments[index]);
            }
        }

        return NULL;
    }

    unsigned int GetTypeArgumentCount(void)
    {
        return(m_typeArgCount);
    }
    DebuggerVarInfo* GetTypeArgumentAt(unsigned int index)
    {
        if (m_typeArguments)
            if (index < m_typeArgCount)
                return(&m_typeArguments[index]);

        return NULL;
    }
    PCCOR_SIGNATURE GetReturnType()
    {
        return(m_returnType);
    }

    //-----------------------------------------------------------
    // This returns an array of pointers to DebugVarInfo blocks,
    // each representing a local variable that in in scope given
    // a certian IP. The variables are ordered in the list are in
    // increasingly larger lexical scopes, i.e., variables in the
    // smallest scope are first, then variables in the enclosing
    // scope, and so on. So, to find a certian variable "i",
    // search the list of "i" and take the first one you find.
    // If there are other "i"s, then they are shadowed by the
    // first one.
    // You must free the array returned in vars with delete [].
    // RETURNS: true if we succeeded, or at least found some debugging info
    //          false if we couldn't find any debugging info
    //-----------------------------------------------------------
    bool GetActiveLocalVars(UINT_PTR IP,
                            DebuggerVariable** vars, unsigned int* count);

    //-----------------------------------------------------------
    // Misc methods to get basic method information.
    //-----------------------------------------------------------
    WCHAR* GetName(void)
    {
        return(m_name.GetData());
    }
    PCCOR_SIGNATURE GetSignature(void)
    {
        return(m_signature);
    }
    WCHAR* GetNamespaceName(void)
    {
        return(m_namespaceName.GetData());
    }
    WCHAR* GetClassName(void)
    {
        return(m_className.GetData());
    }
    DebuggerModule* GetModule(void)
    {
        return(m_module);
    }
    BOOL IsStatic(void)
    {
        return(m_isStatic);
    }


    static DebuggerFunction* FromCorDebug(ICorDebugFunction* function);

    //-----------------------------------------------------------
    // EE interaction methods
    //-----------------------------------------------------------
    HRESULT LoadCode(BOOL native);

    static SIZE_T WalkInstruction(BOOL native,
                                  SIZE_T offset,
                                  BYTE *codeStart,
                                  BYTE *codeEnd);
    static SIZE_T Disassemble(BOOL native,
                              SIZE_T offset,
                              BYTE *codeStart,
                              BYTE *codeEnd,
                              __inout_z WCHAR* buffer,
                              BOOL noAddress,
                              DebuggerModule *module,
                              BYTE *ilcode);

public:
    BOOL ValidateInstruction(BOOL native, SIZE_T offset);

public:
    DebuggerModule*           m_module;
    mdTypeDef                 m_class;
    ICorDebugFunction*        m_ifunction;
    BOOL                      m_isStatic;
    BOOL                      m_allBlocksLoaded;
    BOOL                      m_allScopesLoaded;
    DebuggerString            m_name;
    PCCOR_SIGNATURE           m_signature;
    DebuggerString            m_namespaceName;
    DebuggerString            m_className;
    BOOL                      m_VCHack;

    DebuggerVarInfo*          m_arguments;
    unsigned int              m_argCount;
    DebuggerVarInfo*          m_typeArguments;
    unsigned int              m_typeArgCount;
    PCCOR_SIGNATURE           m_returnType;

    void CountActiveLocalVars(ISymUnmanagedScope* head,
                              unsigned int line,
                              unsigned int* varCount);
    void FillActiveLocalVars(ISymUnmanagedScope* head,
                             unsigned int line,
                             unsigned int varCount,
                             unsigned int* currentVar,
                             DebuggerVariable* varPtrs);

    BYTE*                   m_ilCode;
    ULONG32                 m_ilCodeSize;
    BYTE*                   m_nativeCode;
    ULONG32                 m_nativeCodeSize;
    ULONG                   m_nEditAndContinueLastSynched;


private:
    HRESULT CacheSequencePoints(void);
    HRESULT SynchronizeSequencePoints();

    HRESULT GetRanges(ISymUnmanagedDocument* pDocument,
                      const unsigned int line,
                      const unsigned int cRangeArraySize,
                      unsigned int* const pcRangeArraySizeOut,
                      COR_DEBUG_STEP_RANGE*const pRanges) const;

    //
    // deletes
    //
    void CleanupRanges();
    ReleaseHolder<ISymUnmanagedMethod> m_symMethod;

    DebuggerFunctionILRange**   m_sourcefulRanges;
    ULONG32                     m_sourcefulRangeCount;

    DebuggerFunctionILRange**     m_sourcelessRanges;
    ULONG32                     m_sourcelessRangeCount;

};

/* ------------------------------------------------------------------------- *
 * DebuggerCallback
 * ------------------------------------------------------------------------- */

#define COM_METHOD HRESULT STDMETHODCALLTYPE

class DebuggerCallback : public ICorDebugManagedCallback, public ICorDebugManagedCallback2
{
public:
    DebuggerCallback() : m_refCount(0)
    {
    }

    //
    // IUnknown
    //

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (InterlockedIncrement(&m_refCount));
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        LONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0)
            delete this;

        return (refCount);
    }

    COM_METHOD QueryInterface(REFIID riid, void **ppInterface)
    {
        if (riid == IID_IUnknown)
            *ppInterface = (IUnknown *) (ICorDebugManagedCallback *) this;
        else if (riid == IID_ICorDebugManagedCallback)
            *ppInterface = (ICorDebugManagedCallback *) this;
        else if (riid == IID_ICorDebugManagedCallback2)
            *ppInterface = (ICorDebugManagedCallback2 *) this;
        else
            return (E_NOINTERFACE);

        this->AddRef();
        return (S_OK);
    }

    //
    // ICorDebugManagedCallback
    //

    COM_METHOD CreateProcess(ICorDebugProcess *pProcess);
    COM_METHOD ExitProcess(ICorDebugProcess *pProcess);
    COM_METHOD DebuggerError(ICorDebugProcess *pProcess,
                             HRESULT errorHR,
                             DWORD errorCode);

    COM_METHOD CreateAppDomain(ICorDebugProcess *pProcess,
                               ICorDebugAppDomain *pAppDomain);

    COM_METHOD ExitAppDomain(ICorDebugProcess *pProcess,
                             ICorDebugAppDomain *pAppDomain);

    COM_METHOD LoadAssembly(ICorDebugAppDomain *pAppDomain,
                            ICorDebugAssembly *pAssembly);

    COM_METHOD UnloadAssembly(ICorDebugAppDomain *pAppDomain,
                              ICorDebugAssembly *pAssembly);

    COM_METHOD Breakpoint( ICorDebugAppDomain *pAppDomain,
                           ICorDebugThread *pThread,
                           ICorDebugBreakpoint *pBreakpoint);

    COM_METHOD StepComplete( ICorDebugAppDomain *pAppDomain,
                             ICorDebugThread *pThread,
                             ICorDebugStepper *pStepper,
                             CorDebugStepReason reason);

    COM_METHOD Break( ICorDebugAppDomain *pAppDomain,
                      ICorDebugThread *thread);

    COM_METHOD Exception( ICorDebugAppDomain *pAppDomain,
                          ICorDebugThread *pThread,
                          BOOL unhandled);

    COM_METHOD EvalComplete( ICorDebugAppDomain *pAppDomain,
                             ICorDebugThread *pThread,
                             ICorDebugEval *pEval);

    COM_METHOD EvalException( ICorDebugAppDomain *pAppDomain,
                              ICorDebugThread *pThread,
                              ICorDebugEval *pEval);

    COM_METHOD CreateThread( ICorDebugAppDomain *pAppDomain,
                             ICorDebugThread *thread);

    COM_METHOD ExitThread( ICorDebugAppDomain *pAppDomain,
                           ICorDebugThread *thread);

    COM_METHOD LoadModule( ICorDebugAppDomain *pAppDomain,
                           ICorDebugModule *pModule);

    COM_METHOD UnloadModule( ICorDebugAppDomain *pAppDomain,
                             ICorDebugModule *pModule);

    COM_METHOD LoadClass( ICorDebugAppDomain *pAppDomain,
                          ICorDebugClass *c);

    COM_METHOD UnloadClass( ICorDebugAppDomain *pAppDomain,
                            ICorDebugClass *c);

    COM_METHOD LogMessage(ICorDebugAppDomain *pAppDomain,
                          ICorDebugThread *pThread,
                          LONG lLevel,
                          __in_z WCHAR *pLogSwitchName,
                          __in_z WCHAR *pMessage);

    COM_METHOD LogSwitch(ICorDebugAppDomain *pAppDomain,
                         ICorDebugThread *pThread,
                         LONG lLevel,
                         ULONG ulReason,
                         __in_z WCHAR *pLogSwitchName,
                         __in_z WCHAR *pParentName);

    COM_METHOD ControlCTrap(ICorDebugProcess *pProcess);

    COM_METHOD NameChange(ICorDebugAppDomain *pAppDomain,
                          ICorDebugThread *pThread);

    COM_METHOD UpdateModuleSymbols(ICorDebugAppDomain *pAppDomain,
                                   ICorDebugModule *pModule,
                                   IStream *pSymbolStream);

    COM_METHOD EditAndContinueRemap(ICorDebugAppDomain *pAppDomain,
                                    ICorDebugThread *pThread,
                                    ICorDebugFunction *pFunction,
                                    BOOL fAccurate);

    COM_METHOD BreakpointSetError(ICorDebugAppDomain *pAppDomain,
                                  ICorDebugThread *pThread,
                                  ICorDebugBreakpoint *pBreakpoint,
                                  DWORD dwError);

    COM_METHOD EditAndContinueRemap2(ICorDebugAppDomain *pAppDomain,
                                    ICorDebugThread *pThread,
                                    ICorDebugFunction *pOldFunction,
                                    ICorDebugFunction *pNewFunction,
                                    ULONG32 oldOffset);

    COM_METHOD FunctionRemapOpportunity(ICorDebugAppDomain *pAppDomain,
                                        ICorDebugThread *pThread,
                                        ICorDebugFunction *pOldFunction,
                                        ICorDebugFunction *pNewFunction,
                                        ULONG32 oldOffset);

    COM_METHOD FunctionRemapComplete(ICorDebugAppDomain *pAppDomain,
                                     ICorDebugThread *pThread,
                                     ICorDebugFunction *pFunction);

    COM_METHOD FunctionApplyChange(ICorDebugAppDomain *pAppDomain,
                                     ICorDebugThread *pThread,
                                     ICorDebugFunction *pFunction);

    // SQL related fiber call back
    COM_METHOD CreateConnection(ICorDebugProcess *pProcess,
                                CONNID dwConnectionId,
                                __in_z WCHAR *pConnName);
    COM_METHOD ChangeConnection(ICorDebugProcess *pProcess,
                                CONNID dwConnectionId );
    COM_METHOD DestroyConnection(ICorDebugProcess *pProcess,
                                 CONNID dwConnectionId );

    //
    // Callback2 Exception call backs.
    //
    COM_METHOD Exception(ICorDebugAppDomain *pAppDomain,
                         ICorDebugThread *pThread,
                         ICorDebugFrame *pFrame,
                         ULONG32 nOffset,
                         CorDebugExceptionCallbackType eventType,
                         DWORD dwFlags );

    COM_METHOD ExceptionUnwind(ICorDebugAppDomain *pAppDomain,
                               ICorDebugThread *pThread,
                               CorDebugExceptionUnwindCallbackType eventType,
                               DWORD dwFlags );



    COM_METHOD MDANotification(
        ICorDebugController * pController,
        ICorDebugThread *pThread,
        ICorDebugMDA * pMDA
    );



protected:
    LONG        m_refCount;
};




/* ------------------------------------------------------------------------- *
 * Unmanaged Thread class
 * ------------------------------------------------------------------------- */

class DebuggerUnmanagedThread : public DebuggerBase
{
public:
    DebuggerUnmanagedThread(DWORD dwThreadId, HANDLE hThread)
      : DebuggerBase(dwThreadId), m_stepping(FALSE),
        m_unmanagedStackEnd(NULL), m_hThread(hThread) { }

    HANDLE GetHandle(void) { return m_hThread; }
    DWORD_PTR GetId(void) { return m_token; }

    BOOL            m_stepping;
    CORDB_ADDRESS   m_unmanagedStackEnd;

private:
    HANDLE          m_hThread;
};

/* ------------------------------------------------------------------------- *
 * Debugger ShellCommand classes
 * ------------------------------------------------------------------------- */

class DebuggerCommand : public ShellCommand
{
public:
    DebuggerCommand(const WCHAR *name, int minMatchLength = 0)
        : ShellCommand(name, minMatchLength)
    {
    }

    void Do(Shell *shell, const WCHAR *args)
    {
        DebuggerShell *dsh = static_cast<DebuggerShell *>(shell);

        Do(dsh, dsh->m_cor, args);
    }

    virtual void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args) = 0;
};


/* ------------------------------------------------------------------------- *
 * class ModuleSourceFile
 * ------------------------------------------------------------------------- */

class ModuleSourceFile
{
private:
    ISymUnmanagedDocument    *m_SFDoc;       // Symbol reader document
    DebuggerString    m_pstrFullFileName;    // File name along with path (as returned by the metadata API)
    DebuggerString    m_pstrStrippedFileName;// The barebone file name (eg. foo.cpp)
    ModuleSourceFile *m_pNext;

public:
    ModuleSourceFile()
    {
        m_SFDoc = NULL;
        m_pNext = NULL;
    }

    ~ModuleSourceFile()
    {
        if (m_SFDoc)
        {
            m_SFDoc->Release();
            m_SFDoc = NULL;
        }
    }

    ISymUnmanagedDocument   *GetDocument (void) {return m_SFDoc;}

    // This sets the full file name as well as the stripped file name
    BOOL    SetFullFileName (ISymUnmanagedDocument *doc, LPCSTR pstrFullFileName);
    WCHAR  *GetFullFileName (void) { return m_pstrFullFileName.GetData();}
    WCHAR  *GetStrippedFileName (void) { return m_pstrStrippedFileName.GetData();}

    void    SetNext (ModuleSourceFile *pNext) { m_pNext = pNext;}
    ModuleSourceFile *GetNext (void) { return m_pNext;}

};

/* ------------------------------------------------------------------------- *
 * Utility helper functions
 * ------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------- *
 * Global variables
 * ------------------------------------------------------------------------- */

extern DebuggerShell        *g_pShell;

#endif // __DSHELL_H__

