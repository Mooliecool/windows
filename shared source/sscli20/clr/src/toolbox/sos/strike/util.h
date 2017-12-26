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
#ifndef __util_h__
#define __util_h__

#include <cor.h>
#include <clrdata.h>

#ifdef STRIKE
#if defined(_MSC_VER)
#pragma warning(disable:4200)
#pragma warning(default:4200)
#endif
#include "data.h"

#endif //STRIKE

#define volatile

#include "cor.h"

typedef LPCSTR  LPCUTF8;
typedef LPSTR   LPUTF8;

DECLARE_HANDLE(OBJECTHANDLE);

struct IMDInternalImport;

#define WIN64_8SPACES ""
#define POINTERSIZE "8"
#define POINTERSIZE_TYPE "I32"

#if defined(_MSC_VER)
#pragma warning(disable:4510 4512 4610)
#endif
#include "corinfo.h"

#ifndef _ASSERTE
#ifdef _DEBUG
#define _ASSERTE(expr) 		\
        do { if (!(expr) ) { ExtOut("_ASSERTE fired:\n"); ExtOut(#expr); ExtOut("\n"); DebugBreak(); } } while (0)
#else
#define _ASSERTE(x)
#endif
#endif // ASSERTE

#ifdef _DEBUG
#define ASSERT_CHECK(expr, msg, reason) 		\
        do { if (!(expr) ) { ExtOut(reason); ExtOut(msg); ExtOut(#expr); DebugBreak(); } } while (0)
#endif

// PREFIX macros - Begin

// SOS does not have support for Contracts.  Therefore we needed to duplicate
// some of the PREFIX infrastructure from inc\check.h in here.

// Bug PREFast_:510  v4.51 does not support __assume(0)
#if (defined(_MSC_VER) && !defined(_PREFAST_)) || defined(_PREFIX_)
# define __UNREACHABLE() __assume(0)
#else
#define __UNREACHABLE()  do { } while(true)
#endif



#if defined(DACCESS_COMPILE)
#define COMPILER_ASSUME_MSG(_condition, _message) do { } while (0)
#else

#if defined(_DEBUG)
#define COMPILER_ASSUME_MSG(_condition, _message) \
    ASSERT_CHECK(_condition, _message, "Compiler optimization assumption invalid")
#else
#define COMPILER_ASSUME_MSG(_condition, _message) __assume(_condition)
#endif // _DEBUG

#endif // DACCESS_COMPILE


#define PREFIX_ASSUME(_condition) \
    COMPILER_ASSUME_MSG(_condition, "")

// PREFIX macros - End

class MethodTable;

enum JitType {UNKNOWN=0, JIT, EJIT, PJIT};

#define MD_NOT_YET_LOADED ((DWORD_PTR)-1)
/*
 * HANDLES
 *
 * The default type of handle is a strong handle.
 *
 */
#define HNDTYPE_DEFAULT                         HNDTYPE_STRONG
#define HNDTYPE_WEAK_DEFAULT                    HNDTYPE_WEAK_LONG
#define HNDTYPE_WEAK_SHORT                      (0)
#define HNDTYPE_WEAK_LONG                       (1)
#define HNDTYPE_STRONG                          (2)
#define HNDTYPE_PINNED                          (3)
#define HNDTYPE_VARIABLE                        (4)
#define HNDTYPE_REFCOUNTED                      (5)
#define HNDTYPE_ASYNCPINNED                      (7)

class BaseObject
{
    MethodTable    *m_pMethTab;
};


const DWORD gElementTypeInfo[] = {
#define TYPEINFO(e,ns,c,s,g,ia,ip,if,im)    s,
#include "cortypeinfo.h"
#undef TYPEINFO
};


typedef struct tagLockEntry
{
    tagLockEntry *pNext;    // next entry
    tagLockEntry *pPrev;    // prev entry
    DWORD dwULockID;
    DWORD dwLLockID;        // owning lock
    WORD wReaderLevel;      // reader nesting level    
} LockEntry;

enum ICodeManagerFlags 
{
    ActiveStackFrame =  0x0001, // this is the currently active function
    ExecutionAborted =  0x0002, // execution of this function has been aborted
                                    // (i.e. it will not continue execution at the
                                    // current location)
    AbortingCall    =   0x0004, // The current call will never return
    UpdateAllRegs   =   0x0008, // update full register set
    CodeAltered     =   0x0010, // code of that function might be altered
                                    // (e.g. by debugger), need to call EE
                                    // for original code
};

#define MAX_CLASSNAME_LENGTH    1024

#define OS_PAGE_SIZE   4096

enum EEFLAVOR {UNKNOWNEE, MSCOREE, MSCORWKS, MSCOREND};

// Error codes returned rom GeMethodDescsFromName.
// 0 is returned on success
#define ERROR_BAD_MODULE                  1
#define ERROR_BAD_METHODDESC          2
#define ERROR_NOT_A_METHOD               3
#define ERROR_OUT_OF_MEMORY                   4
#define ERROR_UNSPECIFIED                   5

ULONG32 GetMethodDescsFromName(DWORD_PTR ModulePtr, const char* name, 
    DWORD_PTR *&pOut, int &numMethodDescs);

void FileNameForModule (DacpModuleData *pModule, __out_ecount (MAX_PATH) WCHAR *fileName);
void FileNameForModule (DWORD_PTR pModuleAddr, __out_ecount (MAX_PATH) WCHAR *fileName);
void IP2MethodDesc (DWORD_PTR IP, DWORD_PTR &methodDesc, JitType &jitType,
                    DWORD_PTR &gcinfoAddr);
char *ElementTypeName (unsigned type);
void DisplayFields (DacpEEClassData *pEECls,
                    DWORD_PTR dwStartAddr = 0, BOOL bFirst=TRUE, BOOL bValueClass=FALSE);
int GetObjFieldOffset (CLRDATA_ADDRESS objAddr, CLRDATA_ADDRESS methodTable, __in __in_z LPWSTR wszFieldName,
                    BOOL bFirst=TRUE);
BOOL IsValidToken(DWORD_PTR ModuleAddr, mdTypeDef mb);
void NameForToken_s(DacpModuleData *pModule, mdTypeDef mb, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName, 
                  bool bClassName=true);
void NameForToken_s(DWORD_PTR ModuleAddr, mdTypeDef mb, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName, 
                  bool bClassName=true);
HRESULT NameForToken_s(mdTypeDef mb, IMetaDataImport *pImport, __out_ecount (capacity_mdName) WCHAR *mdName,  size_t capacity_mdName, 
                     bool bClassName);
HRESULT NameForTokenNew_s(mdTypeDef mb, IMDInternalImport *pImport, __out_ecount (capacity_mdName) WCHAR *mdName,  size_t capacity_mdName, 
                     bool bClassName);

void vmmap();
void vmstat();

///////////////////////////////////////////////////////////////////////////////////////////////////
// Support for managed stack tracing
//

DWORD_PTR GetDebuggerJitInfo(DWORD_PTR md);

///////////////////////////////////////////////////////////////////////////////////////////////////

VOID
DllsName(
    ULONG_PTR addrContaining,
    __out_ecount (MAX_PATH) WCHAR *dllName
    );

#define safemove(dst, src) \
SafeReadMemory ((ULONG_PTR) (src), &(dst), sizeof(dst), NULL)

#define safemove_ret(dst, src) \
if (safemove(dst, src) == 0)   \
    return 0;

class ToDestroy
{
public:
    ToDestroy(void **toDestroy)
        : mem(toDestroy)
    {}
    ~ToDestroy()
    {
        if (*mem)
        {
            free (*mem);
            *(DWORD_PTR**)mem = NULL;
        }
    }
private:
    void **mem;
};

template <class A>
class ToDestroyCxx
{
public:
    ToDestroyCxx(A **toDestroy)
        : mem(toDestroy)
    {}
    ~ToDestroyCxx()
    {
        if (*mem)
        {
            delete *mem;
            *mem = NULL;
        }
    }
private:
    A **mem;
};

template <class A>
class ToDestroyCxxArray
{
public:
    ToDestroyCxxArray(A **toDestroy)
        : mem(toDestroy)
    {}
    ~ToDestroyCxxArray()
    {
        if (*mem)
        {
            delete[] *mem;
            *mem = NULL;
        }
    }
private:
    A **mem;
};

struct ModuleInfo
{
    ULONG64 baseAddr;
    ULONG64 size;
    BOOL hasPdb;
};
extern ModuleInfo moduleInfo[];

BOOL InitializeHeapData();
BOOL IsServerBuild ();
UINT GetMaxGeneration();
UINT GetGcHeapCount();
BOOL GetGcStructuresValid();

ULONG GetILSize(DWORD_PTR ilAddr); // REturns 0 if error occurs
void DecodeIL(IMetaDataImport *pImport, BYTE *buffer, ULONG bufSize);
void DecodeDynamicIL(BYTE *data, ULONG Size, DacpObjectData& tokenArray);

BOOL IsRetailBuild (size_t base);
EEFLAVOR GetEEFlavor ();

BOOL IsDumpFile ();

// IsMiniDumpFile will return true if 1) we are in
// a small format minidump, and g_InMinidumpSafeMode is true.
extern BOOL g_InMinidumpSafeMode;
BOOL IsMiniDumpFile ();

BOOL SafeReadMemory (ULONG_PTR offset, PVOID lpBuffer, ULONG_PTR cb,
                     PULONG lpcbBytesRead);

void ReportOOM(); // call if out of memory
BOOL NameForMD_s (DWORD_PTR pMD, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName);
BOOL NameForMT_s (DWORD_PTR MTAddr, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName);
BOOL NameForObject_s (DWORD_PTR ObjAddr, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName);
void isRetAddr(DWORD_PTR retAddr, DWORD_PTR* whereCalled);
void GetMethodTable(DWORD_PTR MDAddr, DWORD_PTR &methodTable);
DWORD PrintString (DWORD_PTR strAddr, BOOL bWCHAR = FALSE, DWORD_PTR length=-1,
    __out_ecount (capacity_buffer) __out_opt WCHAR *buffer = NULL, size_t capacity_buffer=0, BOOL fPrint=TRUE);
#define GetStringLength(strAddr) PrintString(strAddr, FALSE, -1, NULL, FALSE) 
DWORD_PTR GetValueFromExpression (__in __in_z char *str);
void DomainInfo (DacpAppDomainData *pDomain);
void AssemblyInfo (DacpAssemblyData *pAssembly);
DWORD_PTR LoaderHeapInfo (CLRDATA_ADDRESS pLoaderHeapAddr);
DWORD_PTR JitHeapInfo ();
DWORD_PTR VSDHeapInfo(CLRDATA_ADDRESS AppDomainPtr);

class HeapStat
{
protected:
    struct Node
    {
        DWORD_PTR data;
        DWORD count;
        size_t totalSize;
        Node* left;
        Node* right;
        Node ()
            : data(0), count(0), totalSize(0), left(NULL), right(NULL)
        {
        }
    };
    BOOL bHasStrings;
    Node *head;
public:
    HeapStat ()
        : head(NULL), bHasStrings(FALSE)
    {}
    void Add (DWORD_PTR aData, DWORD aSize);
    void Sort ();
    void Print ();
    void Delete ();
    void HasStrings(BOOL abHasStrings)
        {
            bHasStrings = abHasStrings;
        }
private:
    int CompareData(DWORD_PTR n1, DWORD_PTR n2);
    void SortAdd (Node *&root, Node *entry);
    void LinearAdd (Node *&root, Node *entry);
    void ReverseLeftMost (Node *root);
};

extern HeapStat *stat;

class MethodTableCache
{
protected:

    struct Node
    {
        DWORD_PTR data;
        DWORD BaseSize;         // Caching BaseSize and ComponentSize for a MethodTable
        DWORD ComponentSize;    // here has HUGE perf benefits in heap traversals.
        BOOL  bContainsPointers;
        Node* left;
        Node* right;
        Node ()
            : data(0), left(NULL), right(NULL), BaseSize(0), ComponentSize(0), bContainsPointers(0)
        {
        }
    };
    Node *head;
public:
    MethodTableCache ()
        : head(NULL)
    {}
    ~MethodTableCache() { Clear(); }
    void Add (DWORD_PTR aData, DWORD BaseSize, DWORD ComponentSize, BOOL bContainsPointers);
    BOOL Lookup (DWORD_PTR aData, DWORD& BaseSize, DWORD& ComponentSize, BOOL& bContainsPointers);
    void Clear ();
private:
    int CompareData(DWORD_PTR n1, DWORD_PTR n2);    
    void ReverseLeftMost (Node *root);    
};

extern MethodTableCache g_special_mtCache;

struct DumpArrayFlags
{
    DWORD_PTR startIndex;
    DWORD_PTR Length;
    BOOL bDetail;
    LPSTR strObject;
    BOOL bNoFieldsForElement;
    
    DumpArrayFlags ()
        : startIndex(0), Length((DWORD_PTR)-1), bDetail(FALSE), strObject (0), bNoFieldsForElement(FALSE)
    {}
    ~DumpArrayFlags ()
    {
        if (strObject)
            delete [] strObject;
    }
}; //DumpArrayFlags

struct DumpHeapFlags
{
    DWORD_PTR min_size;
    DWORD_PTR max_size;
    BOOL bStat;
    BOOL bStrings;
    BOOL bFixRange;
    BOOL bThinLock;
    BOOL bVerify;
    BOOL bDetail;
    BOOL bStartAtLowerBound;
    BOOL bShort;
    DWORD_PTR startObject;
    DWORD_PTR endObject;
    DWORD_PTR MT;
    LPSTR strType;
    LPWSTR wstrType;
    
    DumpHeapFlags ()
        : min_size(0), max_size((DWORD_PTR)-1), bStat(FALSE), bStrings(FALSE), bFixRange(FALSE), bThinLock(FALSE), bVerify(FALSE),         
          bDetail(FALSE), bStartAtLowerBound(FALSE), bShort(FALSE), startObject(0), endObject(0), MT(0),strType(0),wstrType(0)
    {}

    ~DumpHeapFlags() {
        if(strType)
            delete [] strType;
        if(wstrType)
            delete [] wstrType;
    }
};

// -----------------------------------------------------------------------
//
//  Used by !dumpheap to report gc heap fragmentation
//
#define MIN_FRAGMENTATIONBLOCK_BYTES (1024*512)
struct FragmentationBlock
{
    DWORD_PTR dwBlockAddr;
    DWORD        dwBlockSize;
    DWORD_PTR dwNextObjectAddr;
    DWORD_PTR dwNextObjectMethodTable;
};

// Use these to traverse the blocks yourself
extern FragmentationBlock *g_pBlocks;
extern int g_BlocksLength;

// Call before populating the block array
BOOL InitializeBlocks();
BOOL AddBlock(FragmentationBlock *pBlock);

// -----------------------------------------------------------------------

#define BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX    0x08000000
#define     BIT_SBLK_SPIN_LOCK          0x10000000
#define SBLK_MASK_LOCK_THREADID             0x000003FF   // special value of 0 + 1023 thread ids
#define SBLK_MASK_LOCK_RECLEVEL             0x0000FC00   // 64 recursion levels
#define SBLK_APPDOMAIN_SHIFT                16           // shift right this much to get appdomain index
#define SBLK_MASK_APPDOMAININDEX            0x000007FF   // 2048 appdomain indices
#define SBLK_RECLEVEL_SHIFT                 10           // shift right this much to get recursion level
#define BIT_SBLK_IS_HASHCODE            0x04000000
#define MASK_HASHCODE                   ((1<<HASHCODE_BITS)-1)
#define SYNCBLOCKINDEX_BITS             26
#define MASK_SYNCBLOCKINDEX             ((1<<SYNCBLOCKINDEX_BITS)-1)

struct ObjHeaderSOS
{
    DWORD  m_SyncBlockValue;      // the Index and the Bits
};

struct ThinLockInfo
{
    int ThreadId;
    CLRDATA_ADDRESS threadPtr;
    int Recursion;
};

BOOL HasThinLock(DWORD_PTR objAddr, ThinLockInfo *pOut);

struct needed_alloc_context 
{
	BYTE*   alloc_ptr;
	BYTE*   alloc_limit;
};

struct AllocInfo
{
    needed_alloc_context *array;
    int num;
};

struct GCHandleStatistics
{
    HeapStat hs;
    
    DWORD strongHandleCount;
    DWORD pinnedHandleCount;
    DWORD asyncPinnedHandleCount;
    DWORD refCntHandleCount;        
    DWORD weakLongHandleCount;    
    DWORD weakShortHandleCount;    
    DWORD unknownHandleCount;        
    GCHandleStatistics()
        : strongHandleCount(0), pinnedHandleCount(0), asyncPinnedHandleCount(0), refCntHandleCount(0),
          weakLongHandleCount(0), weakShortHandleCount(0), unknownHandleCount(0)
    {}
    ~GCHandleStatistics()
    {
        hs.Delete();
    }
};

struct SegmentLookup
{
    DacpHeapSegmentData *m_segments;
    int m_iSegmentsSize;
    int m_iSegmentCount;
        
    SegmentLookup();
    ~SegmentLookup();

    void Clear();
    BOOL AddSegment(DacpHeapSegmentData *pData);
    CLRDATA_ADDRESS GetHeap(CLRDATA_ADDRESS object, BOOL& bFound);
};

class GCHeapSnapshot
{
private:
    BOOL m_isBuilt;
    DacpGcHeapDetails *m_heapDetails;
    DacpGcHeapData m_gcheap;
    SegmentLookup m_segments;

    BOOL AddSegments(DacpGcHeapDetails& details);
public:
    GCHeapSnapshot();

    BOOL Build();
    void Clear();
    BOOL IsBuilt() { return m_isBuilt; }

    DacpGcHeapData *GetHeapData() { return &m_gcheap; }
    
    int GetHeapCount() { return m_gcheap.HeapCount; }    
    
    DacpGcHeapDetails *GetHeap(CLRDATA_ADDRESS objectPointer);
    int GetGeneration(CLRDATA_ADDRESS objectPointer);

    
};
extern GCHeapSnapshot g_snapshot;
	
BOOL IsSameModuleName (const char *str1, const char *str2);
BOOL IsModule (DWORD_PTR moduleAddr);
BOOL IsMethodDesc (DWORD_PTR value);
BOOL IsMethodTable (DWORD_PTR value);
BOOL IsObject (size_t obj, BOOL verifyFields=FALSE);
BOOL IsStringObject (size_t obj);
BOOL IsObjectArray (DWORD_PTR objPointer);
BOOL IsObjectArray (DacpObjectData *pData);

// if mName is NULL, you get all modules
void ModuleFromName(DWORD_PTR * &vModule, __in __in_z __in_opt LPSTR mName, int &numModule);
void GetInfoFromName(DWORD_PTR ModuleAddr, const char* name);
void GetInfoFromModule (DWORD_PTR ModuleAddr, ULONG token, DWORD_PTR *ret=NULL);

///////////////////////////////////////////////////////////////////////////////////////////
//
// Methods for creating a database out of the gc heap and it's roots in xml format or CLRProfiler format
//

class TypeTree;
enum { FORMAT_XML=0, FORMAT_CLRPROFILER=1 };
enum { TYPE_START=0,TYPE_TYPES=1,TYPE_ROOTS=2,TYPE_OBJECTS=3,TYPE_HIGHEST=4};
class HeapTraverser
{
private:
    TypeTree *m_pTypeTree;
    size_t m_curNID;
    FILE *m_file;
    int m_format; // from the enum above
    size_t m_objVisited; // for UI updates
    
public:           
    HeapTraverser();
    ~HeapTraverser();

    FILE *getFile() { return m_file; }

    BOOL Initialize();
    BOOL CreateReport (FILE *fp, int format);

private:    
    // First all types are added to a tree
    void insert(size_t mTable);
    size_t getID(size_t mTable);    
    
    // Functions for writing to the output file.
    void PrintType(size_t ID,LPCWSTR name);

    void PrintObjectHead(size_t objAddr,size_t typeID,size_t Size);
    void PrintObjectMember(size_t memberValue);
    void PrintObjectTail();

    void PrintRootHead();
    void PrintRoot(LPCWSTR kind,size_t Value);
    void PrintRootTail();
    
    void PrintSection(int Type,BOOL bOpening);

    // Root and object member helper functions
    static void FindGCRootsCallback (size_t obj, const char* name, void *token);
    void FindGCRootOnStacks ();
    void PrintRefs(size_t obj, size_t methodTable, size_t size);
    
    // Callback functions used during traversals
    static void GatherTypes(DWORD_PTR objAddr,size_t Size,DWORD_PTR methodTable, LPVOID token);
    static void PrintHeap(DWORD_PTR objAddr,size_t Size,DWORD_PTR methodTable, LPVOID token);
    static void PrintOutTree(size_t methodTable, size_t ID, LPVOID token);
    static BOOL TraceRoots(CLRDATA_ADDRESS HandleAddr,CLRDATA_ADDRESS HandleValue,int HandleType, 
        CLRDATA_ADDRESS appDomainPtr, LPVOID token);    
};
    
typedef void (*VISITGCHEAPFUNC)(DWORD_PTR objAddr,size_t Size,DWORD_PTR methodTable,LPVOID token);
BOOL GCHeapsTraverse(VISITGCHEAPFUNC pFunc, LPVOID token);

/////////////////////////////////////////////////////////////////////////////////////////////////////////


// Just to make figuring out which fill pointer element matches a generation
// a bit less confusing. This gen_segment function is ported from gc.cpp.
inline unsigned int gen_segment (int gen)
{
    return (DAC_NUMBERGENERATIONS - gen - 1);
}

inline CLRDATA_ADDRESS SegQueue(DacpGcHeapDetails& heapDetails, int seg)
{
    return heapDetails.finalization_fill_pointers[seg - 1];
}

inline CLRDATA_ADDRESS SegQueueLimit(DacpGcHeapDetails& heapDetails, int seg)
{
    return heapDetails.finalization_fill_pointers[seg];
}

#define FinalizerListSeg (DAC_NUMBERGENERATIONS+1)
#define CriticalFinalizerListSeg (DAC_NUMBERGENERATIONS)


void GatherOneHeapFinalization(DacpGcHeapDetails& heapDetails, HeapStat *stat);
CLRDATA_ADDRESS GetAppDomain(CLRDATA_ADDRESS objPtr);
void GCHeapInfo(DacpGcHeapDetails &heap, DWORD_PTR &total_size);
void GCHeapDump(DacpGcHeapDetails &heap, DWORD_PTR &nObj, DumpHeapFlags &flags,
                AllocInfo* pallocInfo, BOOL bLarge);
BOOL VerifyObject (DacpGcHeapDetails &heap, DWORD_PTR objAddr, DWORD_PTR MTAddr, size_t objSize, 
    BOOL bVerifyMember);

BOOL IsMTForFreeObj(DWORD_PTR pMT);

void DumpMDInfo(DWORD_PTR dwStartAddr, BOOL fStackTraceFormat = FALSE);
void GetDomainList (DWORD_PTR *&domainList, int &numDomain);
void GetThreadList (DWORD_PTR *&threadList, int &numThread);
CLRDATA_ADDRESS GetCurrentManagedThread (); // returns current managed thread if any
void GetAllocContextPtrs(AllocInfo *pallocInfo);

void ReloadSymbolWithLineInfo();

size_t FunctionType (size_t EIP);

size_t Align (size_t nbytes);
// Aligns large objects
size_t AlignLarge (size_t nbytes);

ULONG OSPageSize ();
size_t NextOSPageAddress (size_t addr);

// This version of objectsize reduces the lookup of methodtables in the DAC.
// It uses g_special_mtCache for it's work.
BOOL GetSizeEfficient(DWORD_PTR dwAddrCurrObj, 
    DWORD_PTR dwAddrMethTable, BOOL bLarge, size_t& s, BOOL& bContainsPointers);

// ObjSize now uses the methodtable cache for it's work too.
size_t ObjectSize (DWORD_PTR obj, BOOL fIsLargeObject=FALSE);
size_t ObjectSize(DWORD_PTR obj, DWORD_PTR mt, BOOL fIsValueClass, BOOL fIsLargeObject=FALSE);

void StringObjectContent (size_t obj, BOOL fLiteral=FALSE, const int length=-1);  // length=-1: dump everything in the string object.
void FindGCRoot (size_t obj, BOOL bNoStacks);
void FindAllRootSize ();
UINT FindAllPinnedAndStrong (DWORD_PTR handlearray[],UINT arraySize);
void FindObjSize (size_t obj);

char *EHTypeName(EHClauseType et);

struct StringHolder
{
    LPSTR data;
    StringHolder() : data(NULL) { }
    ~StringHolder() { if(data) delete [] data; }
};

enum ARGTYPE {COBOOL,COSIZE_T,COHEX,COSTRING};
struct CMDOption
{
    const char* name;
    void *vptr;
    ARGTYPE type;
    BOOL hasValue;
    BOOL hasSeen;
};
struct CMDValue
{
    void *vptr;
    ARGTYPE type;
};
BOOL GetCMDOption(const char *string, CMDOption *option, size_t nOption,
                  CMDValue *arg, size_t maxArg, size_t *nArg);

ULONG TargetPlatform();
ULONG DebuggeeType();

inline BOOL IsKernelDebugger ()
{
    return DebuggeeType() == DEBUG_CLASS_KERNEL;
}

extern IXCLRDataProcess* g_clrData;

void    ResetGlobals(void);

HRESULT LoadClrDebugDll(void);
void    UnloadClrDebugDll(void);

typedef enum CorElementTypeInternal
{
    ELEMENT_TYPE_VAR_INTERNAL            = 0x13,     // a type variable VAR <U1>

    ELEMENT_TYPE_VALUEARRAY_INTERNAL     = 0x17,     // VALUEARRAY <type> <bound>

    ELEMENT_TYPE_R_INTERNAL              = 0x1A,     // native real size

    ELEMENT_TYPE_GENERICARRAY_INTERNAL   = 0x1E,     // Array with unknown rank
                                            // GZARRAY <type>

} CorElementTypeInternal;

#define ELEMENT_TYPE_VAR           ((CorElementType) ELEMENT_TYPE_VAR_INTERNAL          )
#define ELEMENT_TYPE_VALUEARRAY    ((CorElementType) ELEMENT_TYPE_VALUEARRAY_INTERNAL   )
#define ELEMENT_TYPE_R             ((CorElementType) ELEMENT_TYPE_R_INTERNAL            )
#define ELEMENT_TYPE_GENERICARRAY  ((CorElementType) ELEMENT_TYPE_GENERICARRAY_INTERNAL )

extern IMetaDataImport* MDImportForModule (DacpModuleData *pModule);
extern IMetaDataImport* MDImportForModule (DWORD_PTR pModule);

//*****************************************************************************
//
// **** CQuickBytes
// This helper class is useful for cases where 90% of the time you allocate 512
// or less bytes for a data structure.  This class contains a 512 byte buffer.
// Alloc() will return a pointer to this buffer if your allocation is small
// enough, otherwise it asks the heap for a larger buffer which is freed for
// you.  No mutex locking is required for the small allocation case, making the
// code run faster, less heap fragmentation, etc...  Each instance will allocate
// 520 bytes, so use accordinly.
//
//*****************************************************************************
template <DWORD SIZE, DWORD INCREMENT> 
class CQuickBytesBase
{
public:
    CQuickBytesBase() :
        pbBuff(0),
        iSize(0),
        cbTotal(SIZE)
    { }

    void Destroy()
    {
        if (pbBuff)
        {
            free(pbBuff);
            pbBuff = 0;
        }
    }

    void *Alloc(SIZE_T iItems)
    {
        iSize = iItems;
        if (iItems <= SIZE)
        {
            cbTotal = SIZE;
            return (&rgData[0]);
        }
        else
        {
            if (pbBuff) free(pbBuff);
            pbBuff = malloc(iItems);
            cbTotal = pbBuff ? iItems : 0;
            return (pbBuff);
        }
    }

    // This is for conformity to the CQuickBytesBase that is defined by the runtime so
    // that we can use it inside of some GC code that SOS seems to include as well.
    //
    // The plain vanilla "Alloc" version on this CQuickBytesBase doesn't throw either,
    // so we'll just forward the call.
    void *AllocNoThrow(SIZE_T iItems)
    {
        return Alloc(iItems);
    }

    HRESULT ReSize(SIZE_T iItems)
    {
        void *pbBuffNew;
        if (iItems <= cbTotal)
        {
            iSize = iItems;
            return NOERROR;
        }

        pbBuffNew = malloc(iItems + INCREMENT);
        if (!pbBuffNew)
            return E_OUTOFMEMORY;
        if (pbBuff) 
        {
            memcpy(pbBuffNew, pbBuff, cbTotal);
            free(pbBuff);
        }
        else
        {
            memcpy(pbBuffNew, rgData, cbTotal);
        }
        cbTotal = iItems + INCREMENT;
        iSize = iItems;
        pbBuff = pbBuffNew;
        return NOERROR;
        
    }

    operator PVOID()
    { return ((pbBuff) ? pbBuff : &rgData[0]); }

    void *Ptr()
    { return ((pbBuff) ? pbBuff : &rgData[0]); }

    SIZE_T Size()
    { return (iSize); }

    SIZE_T MaxSize()
    { return (cbTotal); }

    void        *pbBuff;
    SIZE_T      iSize;              // number of bytes used
    SIZE_T      cbTotal;            // total bytes allocated in the buffer
    // use UINT64 to enforce the alignment of the memory
    UINT64 rgData[(SIZE+sizeof(UINT64)-1)/sizeof(UINT64)];
};

#define     CQUICKBYTES_BASE_SIZE           512
#define     CQUICKBYTES_INCREMENTAL_SIZE    128

class CQuickBytesNoDtor : public CQuickBytesBase<CQUICKBYTES_BASE_SIZE, CQUICKBYTES_INCREMENTAL_SIZE>
{
};

class CQuickBytes : public CQuickBytesNoDtor
{
public:
    CQuickBytes() { }

    ~CQuickBytes()
    {
        Destroy();
    }
};

template <DWORD CQUICKBYTES_BASE_SPECIFY_SIZE> 
class CQuickBytesNoDtorSpecifySize : public CQuickBytesBase<CQUICKBYTES_BASE_SPECIFY_SIZE, CQUICKBYTES_INCREMENTAL_SIZE>
{
};

template <DWORD CQUICKBYTES_BASE_SPECIFY_SIZE> 
class CQuickBytesSpecifySize : public CQuickBytesNoDtorSpecifySize<CQUICKBYTES_BASE_SPECIFY_SIZE>
{
public:
    CQuickBytesSpecifySize() { }

    ~CQuickBytesSpecifySize()
    {
        Destroy();
    }
};


#define STRING_SIZE 10
class CQuickString : public CQuickBytesBase<STRING_SIZE, STRING_SIZE> 
{
public:
    CQuickString() { }

    ~CQuickString()
    {
        Destroy();
    }
    
    void *Alloc(SIZE_T iItems)
    {
        return CQuickBytesBase<STRING_SIZE, STRING_SIZE>::Alloc(iItems*sizeof(WCHAR));
    }

    HRESULT ReSize(SIZE_T iItems)
    {
        return CQuickBytesBase<STRING_SIZE, STRING_SIZE>::ReSize(iItems * sizeof(WCHAR));
    }

    SIZE_T Size()
    {
        return CQuickBytesBase<STRING_SIZE, STRING_SIZE>::Size() / sizeof(WCHAR);
    }

    SIZE_T MaxSize()
    {
        return CQuickBytesBase<STRING_SIZE, STRING_SIZE>::MaxSize() / sizeof(WCHAR);
    }

    WCHAR* String()
    {
        return (WCHAR*) Ptr();
    }

};

enum GetSignatureStringResults
{
    GSS_SUCCESS,
    GSS_ERROR,
    GSS_INSUFFICIENT_DATA,
};

GetSignatureStringResults GetMethodSignatureString (PCCOR_SIGNATURE pbSigBlob, ULONG ulSigBlob, DWORD_PTR dwModuleAddr, CQuickBytes *sigString);
GetSignatureStringResults GetSignatureString (PCCOR_SIGNATURE pbSigBlob, ULONG ulSigBlob, DWORD_PTR dwModuleAddr, CQuickBytes *sigString);

#define     itoa_s_ptr _itoa_s
#define     itow_s_ptr _itow_s
#define     itoa_ptr   _itoa
#define     itow_ptr   _itow

struct MemRange
{
    MemRange (ULONG64 s = NULL, size_t l = 0, MemRange * n = NULL) 
        : start(s), len (l), next (n)
        {}

    bool InRange (ULONG64 addr)
    {
        return addr >= start && addr < start + len;
    }
        
    ULONG64 start;
    size_t len;
    MemRange * next;
}; //struct MemRange

class StressLogMem
{
private:
    // use a linked list for now, could be optimazied later
    MemRange * list;

    void AddRange (ULONG64 s, size_t l)
    {
        list = new MemRange (s, l, list);
    }
    
public:
    StressLogMem () : list (NULL)
        {}
    ~StressLogMem ();
    bool Init (ULONG64 stressLogAddr, IDebugDataSpaces* memCallBack);
    bool IsInStressLog (ULONG64 addr);
}; //class StressLogMem

#endif // __util_h__
