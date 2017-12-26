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
// File: compiler.h
//
// Defined the main compiler class, which contains all the other
// sub-parts of the compiler.
// ===========================================================================

#ifndef __compiler_h__
#define __compiler_h__

/*
 *   My message DLL module handle.
 */
extern HINSTANCE hModuleMessages;
extern LANGID uiLangId;
#define VERIFYLOCALHANDLER ASSERT(1)

////////////////////////////////////////////////////////////////////////////////
// EXCEPT_EXCEPTION
//
// This macro should be used in all __try/__except blocks (for 2nd chance exceptions)
// In the debug build, set breakpoints on DebugExceptionFilter
// to debug cleanup-code exceptions
//
// also see CLEANUP_STACK macro in stdafx.h which should be used inside the exception block.
extern LONG GenericExceptionFilter(EXCEPTION_POINTERS *exceptionInfo, PVOID pv);
#define EXCEPT_EXCEPTION PAL_EXCEPT_FILTER(GenericExceptionFilter, NULL)

/*
 * This NT exception code is used to propagate a fatal
 * exception within the compiler. It has the "customer" bit
 * set and is chosen to probably not collide with any other
 * code.
 */
#define FATAL_EXCEPTION_CODE ((DWORD)0xE004BEBE)

#define UNDEFINEDINDEX 0xffffffff

////////////////////////////////////////////////////////////////////////////////
// EXCEPT_COMPILER_EXCEPTION (or EXCEPT_NON_COMPILER_EXCEPTION if scope does not have compiler pointer)
//
// These macros should be used in all __try/__except blocks inside the compiler (for 1st chance exceptions)
// This allows for JIT debugging, as well as normal-compiler handled exceptions in both debug and retail

extern LONG WatsonFilter(EXCEPTION_POINTERS * ep, WatsonOperationKindEnum howToReport, PCWSTR wzAdditionalFiles);
extern LONG CompilerExceptionFilter(EXCEPTION_POINTERS* ep, LPVOID pv);

#define EXCEPT_COMPILER_EXCEPTION PAL_EXCEPT_FILTER(CompilerExceptionFilter, compiler())

#define EXCEPT_NON_COMPILER_EXCEPTION PAL_EXCEPT_FILTER(CompilerExceptionFilter, NULL)

// Structure for custom debug information:


#define CDIVERSION 4

enum CDIKind {
    CDIKindUsingInfo = 0,
    CDIKindForwardInfo = 1,
    CDIKindForwardToModuleInfo = 2,
    CDIKindIteratorLocals = 3,
    CDIKindForwardIterator = 4,
};

struct CDIGlobalInfo {
    BYTE version;
    BYTE count;

    CDIGlobalInfo (BYTE count) : version(CDIVERSION), count(count) {};
    static size_t Size() { return RoundUpAllocSizeTo(sizeof (CDIGlobalInfo), 4); }
    void CopyInto(BYTE ** pBuffer) {
        memcpy(*pBuffer, this, sizeof(*this));
        memset((*pBuffer) + sizeof(*this) , 0, Size() - sizeof(*this));
        (*pBuffer ) += Size();
    }
};

struct CDIBaseInfo {
    BYTE version;
    BYTE kind;
    DWORD size;

    CDIBaseInfo (DWORD size, CDIKind kind) 
    {
        version = CDIVERSION;
        this->kind = kind;
        this->size = size;
    }
#ifdef _MSC_VER
#pragma push_macro("new")
#undef new
#endif // _MSC_VER
    void * operator new(size_t sz, void * space)
    {
        return space;
    };
#ifdef _MSC_VER
#pragma pop_macro("new")
#endif // _MSC_VER
    static size_t Size() { return RoundUpAllocSizeTo(sizeof (CDIBaseInfo), 4); }

    void CopyInto(BYTE ** pBuffer)
    {
        memcpy(*pBuffer, this, this->size);
        (*pBuffer) += this->size;
    }
};


struct CDIUsingBucket {
    unsigned short countOfUsing;
    static size_t Size() { return sizeof (CDIUsingBucket); }
};

struct CDIIteratorLocalBucket {
    int ilOffsetStart;
    int ilOffsetEnd;
    static size_t Size() { return sizeof (CDIIteratorLocalBucket); }
};

struct CDIIteratorLocalsInfo : public CDIBaseInfo {
    int cBuckets;
    CDIIteratorLocalBucket rgBuckets[];

    CDIIteratorLocalsInfo (int cLocals) : CDIBaseInfo(ComputeSize(cLocals), CDIKindIteratorLocals), cBuckets(cLocals) {
        ClearOffsets();
    }

    void ClearOffsets() {
        memset( rgBuckets, 0 , ComputeSizeOfBuckets(cBuckets));
    }

    static DWORD ComputeSize(int cLocals) {
        return ((DWORD)(RoundUpAllocSizeTo(((DWORD)(offsetof(CDIIteratorLocalsInfo, rgBuckets)  + ComputeSizeOfBuckets(cLocals))), 4)));
    }

    bool Verify() {
        return ComputeSize(cBuckets) <= size;
    }

private:
    static DWORD ComputeSizeOfBuckets(int cLocals) {
        return (DWORD) (CDIIteratorLocalBucket::Size() * cLocals);
    }
};

struct CDIForwardIteratorInfo : public CDIBaseInfo {
    WCHAR szIteratorClassName[];

    CDIForwardIteratorInfo (PCWSTR szIteratorClassName) : CDIBaseInfo(ComputeSize(szIteratorClassName), CDIKindForwardIterator) {
        size_t len = wcslen(szIteratorClassName);
        memcpy( this->szIteratorClassName, szIteratorClassName, len * sizeof(WCHAR));
        memset( this->szIteratorClassName + len, 0, size - (len * sizeof(WCHAR) + offsetof(CDIForwardIteratorInfo, szIteratorClassName)));
    }

    static DWORD ComputeSize(PCWSTR szIteratorClassName) {
        return ((DWORD)(RoundUpAllocSize(((DWORD)(offsetof(CDIForwardIteratorInfo, szIteratorClassName) + 
            (wcslen(szIteratorClassName) + 1) * sizeof(WCHAR))))));
    }

    bool Verify() {
        return ComputeSize(szIteratorClassName) <= size;
    }

};


struct CDIUsingInfo : public CDIBaseInfo {
    unsigned short countOfUsingNamespaces;
    CDIUsingBucket usingCounts[];

    CDIUsingInfo (unsigned short ImpliedCount) : CDIBaseInfo(ComputeSize(ImpliedCount), CDIKindUsingInfo), countOfUsingNamespaces(ImpliedCount) {
        memset(usingCounts, 0, ComputeSizeOfBuckets(countOfUsingNamespaces));
    }

    static DWORD ComputeSize(unsigned short countOfUsingNamespaces) {
        return RoundUpAllocSizeTo((DWORD)(offsetof(CDIUsingInfo, usingCounts) + ComputeSizeOfBuckets(countOfUsingNamespaces)), 4);
    }

    bool Verify() {
        return ComputeSize(countOfUsingNamespaces) <= size;
    }

private:
    static DWORD ComputeSizeOfBuckets(unsigned short countOfUsingNamespaces) {
        return (DWORD)(CDIUsingBucket::Size() * (countOfUsingNamespaces));
    }
};

struct CDIForwardingInfo : public CDIBaseInfo{
    DWORD tokenToForwardTo;
    CDIForwardingInfo (DWORD token) : CDIBaseInfo(sizeof(CDIForwardingInfo), CDIKindForwardInfo), tokenToForwardTo(token) {}
    bool Verify() {
        return Size() <= size;
    }
    static size_t Size() { return RoundUpAllocSizeTo(sizeof (CDIForwardingInfo), 4); }
};

struct CDIForwardToModuleInfo : public CDIBaseInfo {
    DWORD tokenOfModuleInfo;
    CDIForwardToModuleInfo () : CDIBaseInfo(sizeof(CDIForwardToModuleInfo), CDIKindForwardToModuleInfo), tokenOfModuleInfo(0) {}
    bool Verify() {
        return Size() <= size;
    }
    static size_t Size() { return RoundUpAllocSizeTo(sizeof (CDIForwardToModuleInfo), 4); }
};


#define MSCUSTOMDEBUGINFO (L"MD2")


/*
 * ERRLOC location for reporting errors
 */
class ERRLOC
{
public:
    ERRLOC() :
        m_sourceData(NULL),
        m_fileName(NULL),
        m_mapFile(NULL)
    {
    }

    ERRLOC(INFILESYM *inputFile, BASENODE *node) :
        m_sourceData(inputFile->pData),
        m_fileName(inputFile->name->text),
        m_mapFile(inputFile->name->text)
    {
        if (node) {
            SetLine(node);
            SetStart(node);
            end.SetUninitialized();
            SetEnd(node);
            mapStart = start;
            mapEnd = end;
            m_sourceData->GetModule()->MapLocation(&mapStart, &m_mapFile, NULL, NULL);
            m_sourceData->GetModule()->MapLocation(&mapEnd, (NAME**)NULL, NULL, NULL);
        }
    }

    ERRLOC(BSYMMGR * symmgr, BASENODE *node) 
    {
        if (node) {
            NAME * inputFileName = node->GetContainingFileName();
            INFILESYM * inputFile = symmgr->FindInfileSym(inputFileName);

            m_fileName = inputFileName->text;
            m_mapFile = inputFileName->text;

            if (inputFile) {
                m_sourceData = inputFile->pData;
                SetLine(node);
                SetStart(node);
                SetEnd(node);
                mapStart = start;
                mapEnd = end;
                m_sourceData->GetModule()->MapLocation(&mapStart, &m_mapFile, NULL, NULL);
                m_sourceData->GetModule()->MapLocation(&mapEnd, (NAME**)NULL, NULL, NULL);
                return;
            }
        } else {
            m_fileName = m_mapFile = NULL;
        }

        start.SetUninitialized();
        mapEnd = mapStart = end = start;
    }

    ERRLOC(PINFILESYM sym)
    {
        m_sourceData = sym->pData;
        m_fileName = sym->name->text;
        m_mapFile = m_fileName;
    }

    ERRLOC(ImportScope & scope)
    {
        m_sourceData = NULL;
        m_fileName = scope.GetFileName();
        m_mapFile = m_fileName;
    }

    ERRLOC(CSourceData *sourceData, POSDATA start, POSDATA end) :
        m_sourceData(sourceData),
        start(start),
        end(end),
        mapStart(start),
        mapEnd(end)
    {
        InitFromSourceModule(m_sourceData->GetModule());
    }

    ERRLOC(CSourceModuleBase *sourceModule, POSDATA start, POSDATA end) :
        m_sourceData(NULL),
        start(start),
        end(end),
        mapStart(start),
        mapEnd(end)
    {
        InitFromSourceModule(sourceModule);
    }

    ERRLOC(PCWSTR filename, POSDATA start, POSDATA end) :
        m_sourceData(NULL),
        start(start),
        end(end),
        mapStart(start),
        mapEnd(end)
    {
            m_mapFile = m_fileName = filename;
    }

    void InitFromSourceModule(CSourceModuleBase *sourceModule)
    {
        m_fileName = sourceModule->GetFileName();
        sourceModule->MapLocation(&mapStart, &m_mapFile, NULL, NULL);
        sourceModule->MapLocation(&mapEnd, (NAME**)NULL, NULL, NULL);
    }

    void            SetStart(BASENODE* node);
    void            SetLine(BASENODE* node);
    void            SetEnd(BASENODE* node);


    PCWSTR         fileName() const      { return m_fileName; }
    PCWSTR         mapFile() const       { return m_mapFile; }
    BOOL            hasLocation() const   { return !start.IsUninitialized(); }
    int             line() const          { return !start.IsUninitialized() ? (int) start.iLine : -1; }
    int             mapLine() const       { return !mapStart.IsUninitialized() ? (int) mapStart.iLine : -1; }
    int             column() const        { return !start.IsUninitialized() ? (int) start.iChar : -1; }
    int             extent() const        { return start.iLine == end.iLine ? end.iChar - start.iChar : 1; }
    ICSSourceData * sourceData() const    { return m_sourceData; }

private:
    void            SetStartInternal(LEXDATA &ld, BASENODE* node);
    void            SetStartInternal(LEXDATA &ld, long tokidx, long tokOffset = 0);
    void            SetEndInternal(LEXDATA &ld, BASENODE* node);
    void            SetEndInternal(LEXDATA &ld, long tokidx, long tokOffset = 0);

    CSourceData *   m_sourceData;
    PCWSTR         m_fileName;
    PCWSTR         m_mapFile;
    POSDATA         start;
    POSDATA         end;
    POSDATA         mapStart;
    POSDATA         mapEnd;
};

/*
 * ALink Error catcher/filter
 * All the interesting code is in ALinkError::OnError
 */
class ALinkError : public IMetaDataError {
public:
    STDMETHOD_(ULONG, AddRef) () { return 1; }
    STDMETHOD_(ULONG, Release) () { return 1; }
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID* obp) {
        if (obp == NULL)
            return E_POINTER;
        if (riid == IID_IMetaDataError) {
            *obp = (IMetaDataError*)this;
        } else if (riid != IID_IUnknown) {
            *obp = (IUnknown*)this;
        } else {
            return E_NOINTERFACE;
        }
        return S_OK;
    }
    STDMETHOD(OnError)(HRESULT hrError, mdToken tkHint);

    COMPILER* compiler();
};

class BSYMHOST 
{
public:
    virtual NAMEMGR * getNamemgr() = 0;
    virtual void ErrorLocArgs(const ERRLOC * loc, int id, int carg, ErrArg * prgarg) = 0;
    virtual PAGEHEAP    *GetPageHeap () = 0;
    virtual void        NoMemory () = 0;
};

class LSYMHOST
{
public:
    virtual BSYMMGR & getBSymmgr() = 0;
};



// The phases of compilation.
namespace CompilerPhase {
    enum _Type {
        None,
        Init,

        DeclareTypes,
        ImportTypes,
        InitPredefTypes,
        ResolveInheritance,
        DefineBounds,
        DefineMembers,

        // We shouldn't ever be in FUNCBREC before here.
        EvalConstants,

        Prepare,
        PostPrepare,

        CompileMembers,

        Lim
    };
}
typedef CompilerPhase::_Type CompilerPhaseEnum;


/*
 * The main class that holds everything together.
 */
class COMPILER 
    :public ALLOCHOST,
    public LSYMHOST,
    public BSYMHOST
{
    friend class LocalContext;

public:
    //OPTIONS options;            // Current compiler options.
    COptionData options;        // Compiler options, kindly provided by our controller

    CController *pController;   // This is our parent "controller"

    // Embedded components of the compiler.
    PAGEHEAP &pageheap;         // heap for allocating pages.  NOTE:  This is a reference to the page heap in our controller!
    NRHEAP localSymAlloc;       // Allocator for local symbols & names.  Again, specific to this compiler instance.
    FUNCBREC funcBRec;          // Record for compiling an individual function
    CLSDREC clsDeclRec;         // Record for declaring a namespace & classes in it
    MEMHEAP globalHeap;         // General heap for allocating global memory.  This heap is SPECIFIC TO THIS COMPILER INSTANCE!  When this COMPILER is destroyed, so is this heap.
    ILGENREC ilGenRec;          // Record for generating il for a method


    ALinkError alError;         // Catches ALink OnError calls

    NAMEMGR  *namemgr;          // Name table (NOTE:  This is a referenced pointer!)
private:
    NRHEAP privGlobalSymAlloc;      // Allocator for global symbols & names.  Again, specific to this compiler instance.
    LSYMMGR privlsymmgr;           // the local symbol manager
    SYMMGR  privsymmgr;            // Global Symbol manager (also local to compiler in cscomp scenario)
    friend COMPILER * SYMMGR::compiler();
    friend BSYMHOST * BSYMMGR::host();
    PARENTSYM *privExternAliasContainer; // The parent for extern aliases
public:
    NRHEAP & getGlobalSymAlloc() { return privGlobalSymAlloc; }
    BSYMMGR & getBSymmgr() {return privsymmgr.getBSymmgr();}
    LSYMMGR & getLSymmgr() {return privlsymmgr; }
    // WARNING: compiler only function!!! Does not exist on EE
    SYMMGR & getCSymmgr() {return privsymmgr;}
    HINSTANCE getMessageDll() { return hModuleMessages; }
    PARENTSYM * GetExternAliasContainer() { return privExternAliasContainer; }
    IMPORTER importer;          // Metadata importer
    NAMETAB  ccsymbols;         // table of CC symbols
    EMITTER  emitter;           // the file emitter.
    PEFile   *curFile;          // The file currently being emitted
    PEFile   assemFile;         // The file with the Assembly Manifest
    GLOBALATTRSYM *assemblyAttributes;      // the attributes for the current assembly
    GLOBALATTRSYM *unknownGlobalAttributes; // global attributes which have an unknown location
    mdAssembly assemID;         // The global assembly ID for the ALink interface

    ULONG    cInputFiles;       // Number of input files
    ULONG    cOutputFiles;      // Number of output files

    ICSCompilerHost * host;           // compiler host.
    ICSCommandLineCompilerHost * cmdHost;
    IALink2         * linker;         // Assembly linker

    /*
     * the current compiler stage
     */
    enum STAGE
    {
#define DEFINE_STAGE(stage) stage,
#include "stage.h"
#undef DEFINE_STAGE
    };

    CompilerPhaseEnum CompPhase() {
        return compPhase;
    }

    /*
     * CONSIDER: may want to remove this for release builds
     */
    class LOCATION *    location;       // head of current location stack

    // This is where we put stuff for reporting a stack overflow
    SYM * stackOverflowLocation;
    void ReportStackOverflow();

    // Create/destruction.
#ifdef _MSC_VER
#pragma push_macro("new")
#undef new
#endif // _MSC_VER
    DECLARE_CLASS_NEW(size) { return VSAlloc(size); }
#ifdef _MSC_VER
#pragma pop_macro("new")
#endif // _MSC_VER
    void operator delete(void * p) { VSFree(p); }
    COMPILER (CController *pController, PAGEHEAP &ph, NAMEMGR *pNameMgr);
    ~COMPILER();

    HRESULT     Init ();
    HRESULT     InitWorker ();
    void        Term (bool normalTerm);
    HRESULT     CleanUp(HRESULT hr);


    HRESULT InitErrorBuffer();

    void DiscardLocalState();

    // Error building and handling methods. NOTE: We explicitly DO NOT want these to be inlined - for the sake
    // of code size and stack usage.
    void AddLocationToError(CError * err, const ERRLOC * ploc);
    void AddLocationToError(CError * err, const ERRLOC loc) { AddLocationToError(err, &loc); }
    void AddRelatedSymLoc(CError * err, SYM * sym);
    void SubmitError(CError * err);

    // Methods to make an error object. These DO NOT submit the error.
    CError * MakeErrorLocArgs(const ERRLOC * loc, int id, int carg, ErrArg * prgarg, bool fWarnOverride = false);
    CError * MakeErrorTreeArgs(BASENODE * tree, int id, int carg, ErrArg * prgarg);

    // NOTE: We explicitly DO NOT want these to be inlined - for the sake of code size and stack usage.

    // By default these DO NOT add related locations. To add a related location, pass an ErrArgRef.
    CError * MakeError(BASENODE * tree, int id) { return MakeErrorTreeArgs(tree, id, 0, NULL); }
    CError * MakeError(BASENODE * tree, int id, ErrArg a);
    CError * MakeError(BASENODE * tree, int id, ErrArg a, ErrArg b);
    CError * MakeError(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c);
    CError * MakeError(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d);

    CError * MakeError(ERRLOC loc, int id) { return MakeErrorLocArgs(&loc, id, 0, NULL); }
    CError * MakeError(ERRLOC loc, int id, ErrArg a);
    CError * MakeError(ERRLOC loc, int id, ErrArg a, ErrArg b);
    CError * MakeError(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c);
    CError * MakeError(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d);

    // Methods to report an error. These DO submit the error.
    virtual void ErrorLocArgs(const ERRLOC * loc, int id, int carg, ErrArg * prgarg)
        { SubmitError(MakeErrorLocArgs(loc, id, carg, prgarg)); }
    void ErrorTreeArgs(BASENODE * tree, int id, int carg, ErrArg * prgarg)
        { SubmitError(MakeErrorTreeArgs(tree, id, carg, prgarg)); }

    // By default these DO NOT add related locations. To add a related location, pass an ErrArgRef.
    void Error(BASENODE * tree, int id) { ErrorTreeArgs(tree, id, 0, NULL); }
    void Error(BASENODE * tree, int id, ErrArg a);
    void Error(BASENODE * tree, int id, ErrArg a, ErrArg b);
    void Error(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c);
    void Error(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d);

    void Error(ERRLOC loc, int id) { ErrorLocArgs(&loc, id, 0, NULL); }
    void Error(ERRLOC loc, int id, ErrArg a);
    void Error(ERRLOC loc, int id, ErrArg a, ErrArg b);
    void Error(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c);
    void Error(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d);

    // By default these DO add related locations.
    void ErrorRef(BASENODE * tree, int id, ErrArgRef a);
    void ErrorRef(BASENODE * tree, int id, ErrArgRef a, ErrArgRef b);
    void ErrorRef(BASENODE * tree, int id, ErrArgRef a, ErrArgRef b, ErrArgRef c);
    void ErrorRef(BASENODE * tree, int id, ErrArgRef a, ErrArgRef b, ErrArgRef c, ErrArgRef d);

    void ReportICE(EXCEPTION_POINTERS * exceptionInfo);
    void HandleException(DWORD exceptionCode);
#if DEBUG
    static DWORD GetRegDWORD(PCSTR value);
    static bool IsRegString(PCWSTR string, PCWSTR value);
#endif
    static DWORD GetRegDWORDRet(PCSTR value);
    static BSTR GetRegStringRet(PCSTR value);
    static bool IsRegStringRet(PCWSTR string, PCWSTR value);

    void ErrAppendString(PCWSTR str, size_t len = (size_t) -1);
    void ErrAppendChar(WCHAR ch);
    void ErrAppendPrintf(PCWSTR format, ...);
    void ErrAppendId(int id);
    void ErrAppendSym(SYM * sym, SubstContext * pctx, bool fArgs = true);
    void ErrAppendName(NAME *name);
    void ErrAppendNameNode(BASENODE *node);
    void ErrAppendParamList(TypeArray *params, bool isVarargs, bool isParamArray);
    void ErrAppendTypeNode(TYPEBASENODE *type);
    PCWSTR ErrStrWithLoc(PCWSTR pszBase, SYM * sym, BASENODE * node, bool * pfSource);

    void ErrAppendParentSym(SYM * sym, SubstContext * pctx);
    void ErrAppendMethodParentSym(METHSYM * sym, SubstContext * pcxt, TypeArray ** substMethTyParams);
    void ErrAppendTypeParameters(TypeArray * params, SubstContext * pctx, bool forClass);
    void ErrAppendMethod(METHSYM * meth, SubstContext * pctx, bool fArgs);
    void ErrAppendIndexer(INDEXERSYM * indexer, SubstContext * pctx);
    void ErrAppendProperty(PROPSYM * prop, SubstContext * pctx);
    void ErrAppendEvent(EVENTSYM * event, SubstContext * pctx);

    PCWSTR ErrId(int id);
    PCWSTR ErrSym(SYM * sym, SubstContext * pctx = NULL, bool fArgs = true);
    PCWSTR ErrName(PNAME name);
    PCWSTR ErrNameNode(BASENODE *name);
    PCWSTR ErrParamList(TypeArray *params, bool isVarargs, bool isParamArray);

    PCWSTR ErrHR(HRESULT hr, bool useGetErrorInfo = true);
    PCWSTR ErrGetLastError();
    PCWSTR ErrDelegate(AGGTYPESYM * type);
    PCWSTR ErrSK(SYMKIND sk);
    PCWSTR ErrAggKind(AggKindEnum ak);
    PCWSTR ErrTypeNode(TYPEBASENODE *type);
    PCWSTR ErrAccess(ACCESS acc);

    void NotifyHostOfBinaryFile(PCWSTR filename)   { if (cmdHost) cmdHost->NotifyBinaryFile(filename); }
    void NotifyHostOfMetadataFile(PCWSTR filename)   { if (cmdHost) cmdHost->NotifyMetadataFile(filename, GetCorSystemDirectory()); }

    bool FAbortEarly(int cerrPrev, CErrorSuppression * pes = NULL) {
        if (cerrPrev == ErrorCount())
            return isCanceled;
        return true;
    }
    bool FAbortCodeGen(int cerrPrev) {
        return cerrPrev != ErrorCount();
    }
    bool FAbortOutFile() {
        return pController->ErrorsReported() && !options.m_fCompileSkeleton;
    }
    int ErrorCount(){ return pController->ErrorsReported() - pController->WarnAsErrorsReported(); }
    bool GetCheckedMode() { return !!(options.m_fCHECKED); } // for now...
    HRESULT Compile(ICSCompileProgress * progressSink, bool * pfNeedsCleanUp);
    bool ReportCompileProgress(ICSCompileProgress *progressSink, int iFile, int iFileCount);

    bool IsCanceled() { return isCanceled; }

    // ALLOCHOST methods
    _declspec(noreturn) void NoMemory () { Error (NULL, FTL_NoMemory); ALLOCHOST::ThrowOutOfMemoryException(); }
    MEMHEAP     *GetStandardHeap () { return &globalHeap; }
    PAGEHEAP    *GetPageHeap () { return &pageheap; }

    // ISTHOST methods
    NAMEMGR     *getNamemgr() {return namemgr; }
    void        prepareAggregate(AGGSYM * cls) { clsDeclRec.prepareAggregate(cls); }
    NRHEAP * getLocalSymHeap() { return &localSymAlloc;}

    bool        IsBuildingMSCORLIB();

    bool        checkForValidIdentifier(STRCONST *str, bool isPreprocessorString, BASENODE * tree, int id, ErrArg a);

    // Miscellaneous.
    IMetaDataDispenserEx * GetMetadataDispenser();
    ITypeNameFactory * GetTypeNameFactory();
    ITypeNameBuilder * GetTypeNameBuilder();
    void ReleaseTypeNameBuilder() { m_cnttnbUsage--; }
    PCWSTR GetCorSystemDirectory();
    OUTFILESYM * GetFirstOutFile();
    OUTFILESYM * GetManifestOutFile();

    HRESULT     AddInputSet (CInputSet *pSet);
    HRESULT     AddInputSetWorker (CInputSet *pSet);
    INFILESYM * FindAndAddMetadataFile(PCWSTR file, int aid); // searches for the file
    INFILESYM * AddOneMetadataFile(PCWSTR filename, int aid); // assumes a fully qualified filename
    void        AddStandardMetadata();
    void        AddInfileToExternAliasWithErrors(INFILESYM * infile, PCWSTR pszAlias);
    void        AddInfileToExternAlias(INFILESYM * infile, NAME * alias);

    void        ResetErrorBuffer () { errBufferNext = errBufferStart; }
    PCWSTR     GetErrorBufferCurrent () { return this->errBufferNext; }
    PCWSTR     FinishErrorString(PCWSTR start);
    bool        ScanAttributesForCLS(GLOBALATTRSYM * attributes, GLOBALATTRSYM ** ppAttrLoc, bool * pbVal);
    bool        CheckForCLS() { return checkCLS; }
    bool        AllowCLSErrors() { return !options.m_fCompileSkeleton && !FEncBuild(); }
    bool        BuildAssembly() { return m_fAssemble; }
    bool        FriendsDeclared() { return m_fFriendsDeclared; }
    void        SetFriendsDeclared() { m_fFriendsDeclared = true; }

    bool        IsCLSAccessible(AGGSYM *context, TYPESYM *type);
    bool        isCLS_Type(SYM *context, TYPESYM * type);
    bool        CheckSymForCLS(SYM * sym, bool FailIfCantBeDeclared); // If FailIfCantBeDeclared is false, this might cause types ot be declared

    bool        EmitRelaxations() { return m_fEmitRelaxations && m_fAssemble; }
    void        SuppressRelaxations() { m_fEmitRelaxations = false; }

    bool        EmitRuntimeCompatibility();
    void        SuppressRuntimeCompatibility() { m_fEmitRuntimeCompatibility = false; }
    bool        WrapNonExceptionThrows();
    void        SuppressWrapNonExceptionThrows() { m_fWrapNonExceptionThrows = false; }
    
    void        RecordAssemblyRefToOutput( NAME * nameRef, MODULESYM * modSrc, bool fIsFriendAssemblyRef);
    void        MarkUsedFriendAssemblyRef(INFILESYM *infile);

    // Returns the max state that EnsureState will force.
    AggStateEnum AggStateMax() {
        return aggStateMax;
    }

private:
    void ForceAggStates(TYPESYM * type, AggStateEnum aggStateMin);
    void ForceAggStates(TypeArray * ta, AggStateEnum aggStateMin);
    void ForceAggStates(AGGSYM * agg, AggStateEnum aggStateMin);

public:

    bool FBelow(TYPESYM * type, AggStateEnum asMin) {
        return type->AggState() < asMin;
    }
    bool FBelow(TypeArray * ta, AggStateEnum asMin) {
        return ta->AggState() < asMin;
    }
    bool FBelow(AGGSYM * agg, AggStateEnum asMin) {
        return agg->AggState() < asMin;
    }

    AggStateEnum EnsureState(TYPESYM * type, AggStateEnum aggStateMin = AggState::Prepared) {
        ASSERT(aggStateMin < AggState::Lim);
        if (aggStateMin > this->aggStateMax)
            aggStateMin = this->aggStateMax;
        if (FBelow(type, aggStateMin))
            ForceAggStates(type, aggStateMin);
        return type->AggState();
    }
    AggStateEnum EnsureState(TypeArray * ta, AggStateEnum aggStateMin = AggState::Prepared) {
        ASSERT(aggStateMin < AggState::Lim);
        if (aggStateMin > this->aggStateMax)
            aggStateMin = this->aggStateMax;
        if (FBelow(ta, aggStateMin))
            ForceAggStates(ta, aggStateMin);
        return ta->AggState();
    }
    AggStateEnum EnsureState(AGGSYM * agg, AggStateEnum aggStateMin = AggState::Prepared) {
        ASSERT(aggStateMin < AggState::Lim);
        if (aggStateMin > this->aggStateMax)
            aggStateMin = this->aggStateMax;
        if (FBelow(agg, aggStateMin))
            ForceAggStates(agg, aggStateMin);
        return agg->AggState();
    }


    // Update the value of aggStateMin. For the EE this also recomputes fDirty and tsDirty.
    void ComputeAggState(TYPESYM * type);
    void ComputeAggState(TypeArray * ta);

    SYM * LookupGlobalSym(NAME * name, PARENTSYM * parent, symbmask_t mask);

    SYM * LookupInBagAid(NAME * name, BAGSYM * bag, int aid, symbmask_t mask);
    SYM * LookupNextInAid(SYM * sym, int aid, symbmask_t mask);

    void SetBaseType(AGGSYM *cls, AGGTYPESYM *baseClass);
    void SetIfaces(AGGSYM *agg, TypeArray * ifaces);
    void SetIfaces(AGGSYM *agg, AGGTYPESYM ** prgiface, int ciface);
    void SetBounds(TYVARSYM * var, TypeArray * bnds, bool fReset = false);
    bool ResolveBounds(TYVARSYM * var, bool fInherited);

    bool IsBaseAggregate(AGGSYM * derived, AGGSYM * base);
    bool IsBaseType(AGGTYPESYM * atsDer, AGGTYPESYM * atsBase);
    bool IsBaseType(TYPESYM * typeDer, TYPESYM * typeBase);

    bool CheckBogus(SYM * sym) {
        if (!sym)
            return false;
        if (sym->hasBogus())
            return sym->checkBogus();
        return CheckBogusCore(sym, aggStateMax < AggState::Prepared, NULL);
    }

    bool CheckBogusNoEnsure(SYM * sym) {
        if (!sym)
            return false;
        if (sym->hasBogus())
            return sym->checkBogus();
        return CheckBogusCore(sym, true, NULL);
    }
    void ReportStaticClassError(BASENODE * tree, SYM * symCtx, TYPESYM * type, int err);
    bool CheckForStaticClass(BASENODE * tree, SYM * symCtx, TYPESYM * type, int err);

    AGGSYM * GetReqPredefAgg(PREDEFTYPE pt, bool fEnsureState = true);
    AGGTYPESYM * GetReqPredefType(PREDEFTYPE pt, bool fEnsureState = true);
    AGGSYM * GetOptPredefAgg(PREDEFTYPE pt, bool fEnsureState = true);
    AGGTYPESYM * GetOptPredefType(PREDEFTYPE pt, bool fEnsureState = true);
    AGGSYM * GetOptPredefAggErr(PREDEFTYPE pt, bool fEnsureState = true);
    AGGTYPESYM * GetOptPredefTypeErr(PREDEFTYPE pt, bool fEnsureState = true);

    bool CanAggsymBeDeclared(PAGGSYM sym);
    void UndeclarableType(UNRESAGGSYM * ura);
    static unsigned int getPredefIndex(TYPESYM * type);

    bool FCanLift() {
        return !!GetOptPredefAgg(PT_G_OPTIONAL, false);
    }

    // Refactoring
    CCompileCallbackForward compileCallback;

    // Edit And Continue

#define TEMPORARY_NAME_PREFIX L"CS$"
#define DELETED_NAME_PREFIX L"__Deleted$"

#ifdef DEBUG
    bool        haveDefinedAnyType;
#endif //DEBUG


    ICeeFileGen        *CreateCeeFileGen();
    void                DestroyCeeFileGen(ICeeFileGen *ceefile);
    
private:
    bool checkCLS;              // If true, the global CLS compliant attribute is set and we should enforce CLS compliance.
                                // If false, compile anything (don't care about CLS rules).
    bool m_fAssemble;           // True if we are building an assembly (i.e. the first output file is NOT a module)
    bool m_fFriendsDeclared;    // True if source or any added modules contain a friend declaration.
    bool m_fEmitRelaxations;    // True if no user CompilationRelaxationsAttribute was found
    bool m_fEmitRuntimeCompatibility;   // True if no user RuntimeCompatibilityAttribute was specified by the user
    bool m_fWrapNonExceptionThrows; // True if non-exception exceptions are wrapped 

    // Compiler hosting data.
    ULONG cRef;                       // COM reference count
    bool isInited;                    // Have we been initialized (and how far did Init() succeed)?
    bool isCanceled;                  // Has compile been canceled?

    // These evolve together
    CompilerPhaseEnum compPhase;  // The compilation phase.
    AggStateEnum aggStateMax;     // The max agg state that we can handle at this point.

    // Keep track of all assembly references that we bind to the current output
    // Then validate them when the compilation is finished and we can compare against
    // the actual assembly def record
    struct AssemblyRefList {
        NAME *      nameRef;     // The stringized assembly ref
        SYMLIST *   listModSrc;  // List of MODULESYMs where the ref came from
        AssemblyRefList * next;
        bool        fIsFriendAssemblyRefOnly;  // If this is just a friend assembly, we change the error for circular refs which do not match output.
    } * m_arlRefsToOutput;

    int sidLast;

    // General compilation.
    void CompileAll(ICSCompileProgress * progressSink);
    void ParseOneFile(INFILESYM *infile);
    void DeclareOneFile(INFILESYM *infile);
    void ResolveInheritanceHierarchy();
    void DefineBounds();
    void CheckForTypeErrors();
    void DefineMembers();
    void DefineOneFile(INFILESYM *pInfile);
    void EvaluateConstants();
    void CheckCLS();
    void PostCompileChecks();
    void EmitTokens(OUTFILESYM *outfile);
    void SetSigningOptions(mdToken FileToken);

    void DeclareTypes(ICSCompileProgress * progressSink);
    void AddConditionalSymbol(PNAME name);
    COMPILER * compiler()                     { return this; }
    void SetDispenserOptions();
    bool ReportProgress(ICSCompileProgress * progressSink, int phase, int itemsComplete, int itemsTotal);

    bool CheckBogusCore(SYM * sym, bool fNoEnsure, bool * pfUndeclared);

    void EnsureTypesInNsAid(NSSYM * ns, int aid);

    // Error handling methods and data (error.cpp)
    //int cWarn, cError;
    // Buffer for accumulating error messages; cleared when error is reported.
    // 2MB is Reservced and individual pages are committed as needed
#define ERROR_BUFFER_MAX_WCHARS (1024*1024)
#define ERROR_BUFFER_MAX_BYTES  (ERROR_BUFFER_MAX_WCHARS*sizeof(WCHAR))
    friend LONG CompilerExceptionFilter(EXCEPTION_POINTERS* exceptionInfo, LPVOID pvData);
    WCHAR * errBuffer;
    WCHAR * errBufferNext;
    WCHAR * errBufferStart;
    BYTE * errBufferNextPage;
    int ErrBufferLeftTot() // WARNING: Don't ever pass this to wcsncpy_s! It zeros out the entire remaining buffer!
        { return (int)(ERROR_BUFFER_MAX_WCHARS - (errBufferNext - errBuffer)); }
    size_t ErrBufferLeft(size_t cchNeed)
    {
        size_t cch = ERROR_BUFFER_MAX_WCHARS - (errBufferNext - errBuffer);
        return min(cch, cchNeed);
    }

    void ThrowFatalException();

    // Miscellaneous.
    IMetaDataDispenserEx * dispenser;
    CComPtr<ITypeNameFactory> m_qtypenamefactory;
    CComPtr<ITypeNameBuilder> m_qtypenamebuilder;
    int                       m_cnttnbUsage;
    NAME *            corSystemDirectory;
    void CheckSearchPath(__inout PWSTR wzPathList, int idsSource);
    PCWSTR GetSearchPath();
    PWSTR m_pszLibPath;

    // No import library is provided for these guys, so we
    // bind to them dynamically.
    HRESULT (__stdcall *pfnCreateCeeFileGen)(ICeeFileGen **ceeFileGen); // call this to instantiate
    HRESULT (__stdcall *pfnDestroyCeeFileGen)(ICeeFileGen **ceeFileGen); // call this to delete
    HMODULE hmodALink;
    HMODULE hmodCorPE;


public:
    bool FEncBuild() { return false; }
    void RecordEncMethRva(METHSYM * meth, ulong rva) { }

};


/*
 * Define inline routine for getting to the COMPILER class from
 * various embedded classes.
 */
__forceinline BSYMHOST * BSYMMGR::host()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, privsymmgr)); }

__forceinline COMPILER * SYMMGR::compiler()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, privsymmgr)); }



__forceinline COMPILER * IMPORTER::compiler()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, importer)); }

__forceinline COMPILER * FUNCBREC::compiler()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, funcBRec)); }

__forceinline COMPILER * CLSDREC::compiler()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, clsDeclRec)); }


__forceinline COMPILER * EMITTER::compiler()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, emitter)); }

__forceinline COMPILER * ILGENREC::compiler()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, ilGenRec)); }


__forceinline COMPILER * ALinkError::compiler()
{ return (COMPILER *) (((BYTE *)this) - offsetof(COMPILER, alError)); }

__forceinline CController * CLSDREC::controller() { return compiler()->pController; }

/*
 * LOCATION for reporting errors
 *
 * they link together in a stack, top of the stack
 * is the most recent location.
 *
 * they should only be created as local variables
 * using the SETLOCATIONXXX(xxx) macros below
 *
 * these guys are designed for fast construction/destruction
 * and slow query since they are only queried after an ICE
 *
 * CONSIDER:
 *
 *  - making an empty version of the stage/location for release builds
 *  - making super debug/loggin versions of SETLOCATIONXXX(xxx)
 *
 */
class LOCATION
{
public:

            ~LOCATION()                     { *listHead = previousLocation; }

    virtual SYM *           getSymbol()     = 0;
    virtual INFILESYM *     getFile()       = 0;
    virtual BASENODE *      getNode()       = 0;
    virtual COMPILER::STAGE getStage()      { return getPrevious()->getStage(); }    

            LOCATION *      getPrevious()   { return previousLocation; }
   
protected:

    LOCATION(LOCATION **lh) : previousLocation(*lh), listHead(lh)
    {
        *listHead = this;
    }

    

private:

    LOCATION *  previousLocation;
    LOCATION ** listHead;
};

class SYMLOCATION : LOCATION
{
public:

    SYMLOCATION(LOCATION **lh, SYM *sym) :
        LOCATION(lh),
        symbol(sym)        
    {}

    SYM *           getSymbol() { return symbol; }
    INFILESYM *     getFile()   { return symbol->GetSomeInputFile(); }
    BASENODE *      getNode()   { return symbol->GetSomeParseTree(); }

private:

    SYM *       symbol;

};

class NODELOCATION : LOCATION
{
public:

    NODELOCATION(LOCATION **lh, BASENODE *n) :
        LOCATION(lh),
        node(n)
    {}

    SYM *           getSymbol() { return NULL; }
    INFILESYM *     getFile()   { return getPrevious()->getFile(); }
    BASENODE *      getNode()   { return node; }

private:

    BASENODE *      node;

};

class FILELOCATION : LOCATION
{
public:

    FILELOCATION(LOCATION **lh, INFILESYM * f) :
        LOCATION(lh),
        file(f)
    {}

    SYM *           getSymbol() { return NULL; }
    INFILESYM *     getFile()   { return file; }
    BASENODE *      getNode()   { return NULL; }  

private:

    INFILESYM *     file;

};

class STAGELOCATION : LOCATION
{
public:

    STAGELOCATION(LOCATION **lh, COMPILER::STAGE s) :
        LOCATION(lh),
        stage(s)
    {}

    SYM *           getSymbol() { return getPrevious() ? getPrevious()->getSymbol() : NULL; }
    INFILESYM *     getFile()   { return getPrevious() ? getPrevious()->getFile()   : NULL; }
    BASENODE *      getNode()   { return getPrevious() ? getPrevious()->getNode()   : NULL; }
    COMPILER::STAGE getStage()  { return stage; }

private:

    COMPILER::STAGE stage;

};

#define SETLOCATIONFILE(file)   FILELOCATION    _fileLocation(&compiler()->location,    (file));
#define SETLOCATIONNODE(node)   NODELOCATION    _nodeLocation(&compiler()->location,    (node));
#define SETLOCATIONSYM(sym)     SYMLOCATION     _symLocation (&compiler()->location,    (sym));
#define SETLOCATIONSTAGE(stage) STAGELOCATION   _stageLocation(&compiler()->location,   (COMPILER::stage));

// Utility function I couldn't find a better place for.
extern void RoundToFloat(double d, float * f);
extern BOOL SearchPathDual(PCWSTR lpPath, PCWSTR lpFileName, WCBuffer lpBuffer);
extern HINSTANCE FindAndLoadHelperLibrary(PCWSTR filename);

// Return the predefined void type
__forceinline TYPESYM * FUNCBREC::getVoidType()
{
    return compiler()->getBSymmgr().GetVoid();
}


// macros used for building up strings in the error buffer
#define START_ERR_STRING(compiler)                              \
    PCWSTR __errstr = (compiler)->GetErrorBufferCurrent();

#define END_ERR_STRING(compiler)                        \
    (compiler)->FinishErrorString(__errstr)

bool GUIDFromString( PCWSTR szGuidStart,  PCWSTR szGuidEnd, GUID * pGUID);


#endif //__compiler_h__

