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
//
// Internal data access functionality.
//
//*****************************************************************************

#ifndef _DACPRIVATE_H_
#define _DACPRIVATE_H_

#include <cor.h>
#include <clrdata.h>
#include <xclrdata.h>


//----------------------------------------------------------------------------
//
// LiveProcDataTarget.
//
//----------------------------------------------------------------------------

class LiveProcDataTarget : public ICLRDataTarget
{
public:
    LiveProcDataTarget(HANDLE process,
                       DWORD processId);

    //
    // IUnknown.
    //
    // This class is intended to be kept on the stack
    // or as a member and does not maintain a refcount.
    //
    
    STDMETHOD(QueryInterface)(
        THIS_
        IN REFIID InterfaceId,
        OUT PVOID* Interface
        );
    STDMETHOD_(ULONG, AddRef)(
        THIS
        );
    STDMETHOD_(ULONG, Release)(
        THIS
        );

    //
    // ICLRDataTarget.
    //
    
    virtual HRESULT STDMETHODCALLTYPE GetMachineType( 
        /* [out] */ ULONG32 *machine);
    virtual HRESULT STDMETHODCALLTYPE GetPointerSize( 
        /* [out] */ ULONG32 *size);
    virtual HRESULT STDMETHODCALLTYPE GetImageBase( 
        /* [string][in] */ LPCWSTR name,
        /* [out] */ CLRDATA_ADDRESS *base);
    virtual HRESULT STDMETHODCALLTYPE ReadVirtual( 
        /* [in] */ CLRDATA_ADDRESS address,
        /* [length_is][size_is][out] */ PBYTE buffer,
        /* [in] */ ULONG32 request,
        /* [optional][out] */ ULONG32 *done);
    virtual HRESULT STDMETHODCALLTYPE WriteVirtual( 
        /* [in] */ CLRDATA_ADDRESS address,
        /* [size_is][in] */ PBYTE buffer,
        /* [in] */ ULONG32 request,
        /* [optional][out] */ ULONG32 *done);
    virtual HRESULT STDMETHODCALLTYPE GetTLSValue(
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 index,
        /* [out] */ CLRDATA_ADDRESS* value);
    virtual HRESULT STDMETHODCALLTYPE SetTLSValue(
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 index,
        /* [in] */ CLRDATA_ADDRESS value);
    virtual HRESULT STDMETHODCALLTYPE GetCurrentThreadID(
        /* [out] */ ULONG32* threadID);
    virtual HRESULT STDMETHODCALLTYPE GetThreadContext(
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 contextFlags,
        /* [in] */ ULONG32 contextSize,
        /* [out, size_is(contextSize)] */ PBYTE context);
    virtual HRESULT STDMETHODCALLTYPE SetThreadContext(
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 contextSize,
        /* [in, size_is(contextSize)] */ PBYTE context);
    virtual HRESULT STDMETHODCALLTYPE Request( 
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

private:
    HANDLE m_process;
    DWORD m_processId;
};

//----------------------------------------------------------------------------
//
// Support functions.
//
//----------------------------------------------------------------------------

HRESULT
LoadDataAccessDll(HINSTANCE mscorModule,
                  REFIID ifaceId,
                  ICLRDataTarget* target,
                  HMODULE* dllHandle,
                  void** iface);

//----------------------------------------------------------------------------
//
// Internal CLRData requests.
//
//----------------------------------------------------------------------------

// Private requests for a process.
enum
{
    // Locate these enumerants up in a high index region
    // to avoid any possible conflicts with the public enumerants
    // that begin at zero.
    DACPRIV_REQUEST_THREAD_STORE_DATA = 0xf0000000,
    DACPRIV_REQUEST_APPDOMAIN_STORE_DATA,
    DACPRIV_REQUEST_APPDOMAIN_LIST,
    DACPRIV_REQUEST_APPDOMAIN_DATA,
    DACPRIV_REQUEST_APPDOMAIN_NAME,
    DACPRIV_REQUEST_APPDOMAIN_APP_BASE,
    DACPRIV_REQUEST_APPDOMAIN_PRIVATE_BIN_PATHS,
    DACPRIV_REQUEST_APPDOMAIN_CONFIG_FILE,
    DACPRIV_REQUEST_ASSEMBLY_LIST,
    DACPRIV_REQUEST_FAILED_ASSEMBLY_LIST,
    DACPRIV_REQUEST_ASSEMBLY_DATA,
    DACPRIV_REQUEST_ASSEMBLY_NAME,
    DACPRIV_REQUEST_ASSEMBLY_DISPLAY_NAME,
    DACPRIV_REQUEST_ASSEMBLY_LOCATION,
    DACPRIV_REQUEST_FAILED_ASSEMBLY_DATA,
    DACPRIV_REQUEST_FAILED_ASSEMBLY_DISPLAY_NAME,
    DACPRIV_REQUEST_FAILED_ASSEMBLY_LOCATION,
    DACPRIV_REQUEST_THREAD_DATA,
    DACPRIV_REQUEST_THREAD_THINLOCK_DATA,
    DACPRIV_REQUEST_CONTEXT_DATA,
    DACPRIV_REQUEST_METHODDESC_DATA,
    DACPRIV_REQUEST_METHODDESC_IP_DATA,
    DACPRIV_REQUEST_METHODDESC_NAME,
    DACPRIV_REQUEST_METHODDESC_FRAME_DATA,
    DACPRIV_REQUEST_CODEHEADER_DATA,
    DACPRIV_REQUEST_THREADPOOL_DATA,
    DACPRIV_REQUEST_WORKREQUEST_DATA,
    DACPRIV_REQUEST_OBJECT_DATA,
    DACPRIV_REQUEST_FRAME_NAME,
    DACPRIV_REQUEST_OBJECT_STRING_DATA,
    DACPRIV_REQUEST_OBJECT_CLASS_NAME,
    DACPRIV_REQUEST_METHODTABLE_NAME,
    DACPRIV_REQUEST_METHODTABLE_DATA,
    DACPRIV_REQUEST_EECLASS_DATA,
    DACPRIV_REQUEST_FIELDDESC_DATA,
    DACPRIV_REQUEST_MANAGEDSTATICADDR,
    DACPRIV_REQUEST_MODULE_DATA,
    DACPRIV_REQUEST_MODULEMAP_TRAVERSE,
    DACPRIV_REQUEST_MODULETOKEN_DATA,
#ifdef _DEBUG
    DACPRIV_REQUEST_MDA,
#endif
    DACPRIV_REQUEST_PEFILE_DATA,
    DACPRIV_REQUEST_PEFILE_NAME,
    DACPRIV_REQUEST_ASSEMBLYMODULE_LIST,
    DACPRIV_REQUEST_GCHEAP_DATA,
    DACPRIV_REQUEST_GCHEAP_LIST,
    DACPRIV_REQUEST_GCHEAPDETAILS_DATA,
    DACPRIV_REQUEST_GCHEAPDETAILS_STATIC_DATA,
    DACPRIV_REQUEST_HEAPSEGMENT_DATA,
    DACPRIV_REQUEST_UNITTEST_DATA,
    DACPRIV_REQUEST_ISSTUB,
    DACPRIV_REQUEST_DOMAINLOCALMODULE_DATA,
    DACPRIV_REQUEST_DOMAINLOCALMODULEFROMAPPDOMAIN_DATA,
    DACPRIV_REQUEST_DOMAINLOCALMODULE_DATA_FROM_MODULE,
    DACPRIV_REQUEST_SYNCBLOCK_DATA,
    DACPRIV_REQUEST_SYNCBLOCK_CLEANUP_DATA,
    DACPRIV_REQUEST_HANDLETABLE_TRAVERSE,
    DACPRIV_REQUEST_RCWCLEANUP_TRAVERSE,
    DACPRIV_REQUEST_EHINFO_TRAVERSE,
    DACPRIV_REQUEST_STRESSLOG_DATA,
    DACPRIV_REQUEST_JITLIST,
    DACPRIV_REQUEST_JIT_HELPER_FUNCTION_NAME,
    DACPRIV_REQUEST_JUMP_THUNK_TARGET,
    DACPRIV_REQUEST_LOADERHEAP_TRAVERSE,
    DACPRIV_REQUEST_MANAGER_LIST,
    DACPRIV_REQUEST_JITHEAPLIST,
    DACPRIV_REQUEST_CODEHEAP_LIST,
    DACPRIV_REQUEST_METHODTABLE_SLOT,
    DACPRIV_REQUEST_VIRTCALLSTUBHEAP_TRAVERSE,
    DACPRIV_REQUEST_NESTEDEXCEPTION_DATA,
    DACPRIV_REQUEST_USEFULGLOBALS,
    DACPRIV_REQUEST_CLRTLSDATA_INDEX,
    DACPRIV_REQUEST_MODULE_FINDIL
};

// Private requests for tasks.
enum
{
    DACTASKPRIV_REQUEST_IS_CANTSTOP = 0xf0000000
};

// Private requests for DataModules
enum
{
    DACDATAMODULEPRIV_REQUEST_GET_MODULEPTR = 0xf0000000
};


// Private requests for stack walkers.
enum
{
    DACSTACKPRIV_REQUEST_FRAME_DATA = 0xf0000000
};

enum DacpObjectType { OBJ_STRING=0,OBJ_FREE,OBJ_OBJECT,OBJ_ARRAY,OBJ_OTHER };
struct DacpObjectData
{
    CLRDATA_ADDRESS EEClass;
    CLRDATA_ADDRESS MethodTable;
    DacpObjectType ObjectType;
    DWORD Size;
    CLRDATA_ADDRESS ElementTypeHandle;
    CorElementType ElementType;
    DWORD dwRank;
    DWORD dwNumComponents;
    DWORD dwComponentSize;
    CLRDATA_ADDRESS ArrayDataPtr;
    CLRDATA_ADDRESS ArrayBoundsPtr;
    CLRDATA_ADDRESS ArrayLowerBoundsPtr;

    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_OBJECT_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT GetObjectClassName(IXCLRDataProcess* dac,
                                      CLRDATA_ADDRESS addr, 
                                      ULONG32 iNameChars,
                                      __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_OBJECT_CLASS_NAME,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }

    static HRESULT GetStringObjectData(IXCLRDataProcess* dac,
                                       CLRDATA_ADDRESS addr, 
                                       ULONG32 iNameChars,
                                       __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_OBJECT_STRING_DATA,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }
};

struct DacpUsefulGlobalsData
{
    CLRDATA_ADDRESS ArrayMethodTable;
    CLRDATA_ADDRESS StringMethodTable;
    CLRDATA_ADDRESS ObjectMethodTable;
    CLRDATA_ADDRESS ExceptionMethodTable;
    CLRDATA_ADDRESS FreeMethodTable;

    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_USEFULGLOBALS,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);            
    }    
};

struct DacpManagedStaticArgs
{
    CLRDATA_ADDRESS ThreadAddr;
    DWORD Slot;
};

struct DacpFieldDescData
{
    CorElementType Type;
    CorElementType sigType;     // ELEMENT_TYPE_XXX from signature. We need this to disply pretty name for String in minidump's case
    CLRDATA_ADDRESS MTOfType; // NULL if Type is not loaded
    
    CLRDATA_ADDRESS ModuleOfType;
    mdTypeDef TokenOfType;
    
    mdFieldDef mb;
    CLRDATA_ADDRESS MTOfEnclosingClass;
    DWORD dwOffset;
    BOOL bIsThreadLocal;
    BOOL bIsContextLocal;
    BOOL bIsStatic;
    CLRDATA_ADDRESS NextField;
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_FIELDDESC_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }

    // Only to be called for ThreadLocal statics which are of ELEMENT_TYPE_CLASS
    static HRESULT CalculateAddrForManagedStatic(IXCLRDataProcess* dac, CLRDATA_ADDRESS ThreadAddr,
        DWORD Slot,CLRDATA_ADDRESS *pOutAddr)
    {
        DacpManagedStaticArgs args = { ThreadAddr, Slot };
        return dac->Request(DACPRIV_REQUEST_MANAGEDSTATICADDR,
                            sizeof(DacpManagedStaticArgs), (PBYTE) &args,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) pOutAddr);

    }
};

struct DacpEEClassData
{
    CLRDATA_ADDRESS MethodTable;
    CLRDATA_ADDRESS Module;
    WORD wNumVtableSlots;
    WORD wNumMethodSlots;
    WORD wNumInstanceFields;
    WORD wNumStaticFields;
    DWORD dwClassDomainNeutralIndex;
    DWORD dwAttrClass; // cached metadata
    mdTypeDef cl; // Metadata token    

    CLRDATA_ADDRESS FirstField; // If non-null, you can retrieve more
    
    WORD wThreadStaticOffset;
    WORD wThreadStaticsSize;
    WORD wContextStaticOffset;
    WORD wContextStaticsSize;
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_EECLASS_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }
};

struct DacpDomainLocalModuleData
{
    // These two parameters are used as input params when calling the
    // no-argument form of Request below.
    CLRDATA_ADDRESS appDomainAddr;
    SIZE_T  ModuleID;
    
    CLRDATA_ADDRESS pClassData;   
    CLRDATA_ADDRESS pDynamicClassTable;   
    CLRDATA_ADDRESS pGCStaticDataStart;
    CLRDATA_ADDRESS pNonGCStaticDataStart; 

    // This is called when you have a
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_DOMAINLOCALMODULE_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }

    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->
            Request(DACPRIV_REQUEST_DOMAINLOCALMODULEFROMAPPDOMAIN_DATA,
                    0, (PBYTE) NULL,
                    sizeof(*this), (PBYTE)this);
    }

    // Called when you have a Module pointer
    HRESULT RequestFromModulePtr(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_DOMAINLOCALMODULE_DATA_FROM_MODULE,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }
};


typedef enum ModuleMapType
{
    TYPEDEFTOMETHODTABLE, TYPEREFTOMETHODTABLE
};
typedef void (*MODULEMAPTRAVERSE)(UINT index, CLRDATA_ADDRESS methodTable,LPVOID token);
struct DacpModuleMapTraverseArgs
{
    ModuleMapType mmt;
    CLRDATA_ADDRESS Module;    
    MODULEMAPTRAVERSE Callback;
    LPVOID token;
};

struct DacpModuleFindILArgs 
{
    CLRDATA_ADDRESS Module;    
    CLRDATA_ADDRESS rva;
};


struct DacpModuleData
{
    CLRDATA_ADDRESS File; // A PEFile addr
    CLRDATA_ADDRESS  ilBase;
    CLRDATA_ADDRESS metadataStart;
    SIZE_T metadataSize;
    CLRDATA_ADDRESS Assembly; // Assembly pointer
    BOOL bIsReflection;
    BOOL bIsPEFile;
    SIZE_T dwBaseClassIndex;
    IXCLRDataModule *ModuleDefinition;
    SIZE_T dwDomainNeutralIndex;

    DWORD dwTransientFlags;
    
    CLRDATA_ADDRESS TypeDefToMethodTableMap;
    CLRDATA_ADDRESS TypeRefToMethodTableMap;
    CLRDATA_ADDRESS MethodDefToDescMap;
    CLRDATA_ADDRESS FieldDefToDescMap;
    CLRDATA_ADDRESS MemberRefToDescMap;
    CLRDATA_ADDRESS FileReferencesMap;
    CLRDATA_ADDRESS ManifestModuleReferencesMap;

    CLRDATA_ADDRESS pLookupTableHeap;
    CLRDATA_ADDRESS pThunkHeap;
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_MODULE_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT DoMapTraverse(IXCLRDataProcess* dac,CLRDATA_ADDRESS addr, ModuleMapType mmt, 
        MODULEMAPTRAVERSE pFunc, LPVOID token)
    {
        DacpModuleMapTraverseArgs args = { mmt, addr, pFunc, token };
        return dac->Request(DACPRIV_REQUEST_MODULEMAP_TRAVERSE,
                            sizeof(DacpModuleMapTraverseArgs), (BYTE *)&args,
                            0, NULL);    
    }    

    static HRESULT FindIL(IXCLRDataProcess * dac, CLRDATA_ADDRESS addrModule, CLRDATA_ADDRESS rva,
            CLRDATA_ADDRESS *pAddr) // pAddr is the IL address on successful completion.
    {
        DacpModuleFindILArgs args = { addrModule, rva };
        return dac->Request(DACPRIV_REQUEST_MODULE_FINDIL,
            sizeof(DacpModuleFindILArgs), (BYTE *)&args,
            sizeof(CLRDATA_ADDRESS), (PBYTE)pAddr);
    }
};

struct DacpPEFileData
{
    CLRDATA_ADDRESS Base;

    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_PEFILE_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT GetPEFileName(IXCLRDataProcess* dac,
                                 CLRDATA_ADDRESS pefileaddr,
                                 ULONG32 iNameChars,
                                 __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_PEFILE_NAME,
                            sizeof(pefileaddr), (PBYTE)&pefileaddr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }
};

struct DacpModuleTokenData
{
    CLRDATA_ADDRESS Module;         // Fill this in
    mdToken Token;                          // And this
    // If successful, this is filled in on return
    CLRDATA_ADDRESS ReturnValue;

    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_MODULETOKEN_DATA,
                            sizeof(*this), (PBYTE) this,
                            sizeof(*this), (PBYTE) this);
    }
};

struct DacpSlotArgs
{
    CLRDATA_ADDRESS mtaddr;
    ULONG32 SlotNumber;	
};

struct DacpMethodTableData
{
    BOOL bIsFree; // everything else is NULL if this is true.
    CLRDATA_ADDRESS Class;
    CLRDATA_ADDRESS ParentMethodTable;
    WORD wNumInterfaces;
    WORD wTotalVtableSlots;
    DWORD BaseSize;
    DWORD ComponentSize;
    BOOL bIsShared; // flags & enum_flag_DomainNeutral
    DWORD sizeofMethodTable;
    BOOL bIsDynamic;
    BOOL bContainsPointers;
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_METHODTABLE_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT GetMethodTableName(IXCLRDataProcess* dac,
                                      CLRDATA_ADDRESS mtaddr,
                                      ULONG32 iNameChars,
                                      __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_METHODTABLE_NAME,
                            sizeof(mtaddr), (PBYTE)&mtaddr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }

    static HRESULT GetSlot(IXCLRDataProcess* dac,
                                    CLRDATA_ADDRESS mtaddr,
                                    ULONG32 SlotNumber,
                                    CLRDATA_ADDRESS *SlotValue)
    {
        DacpSlotArgs dsa = { mtaddr, SlotNumber };
        return dac->Request(DACPRIV_REQUEST_METHODTABLE_SLOT,
                            sizeof(DacpSlotArgs), (PBYTE) &dsa,
                            sizeof(CLRDATA_ADDRESS),(PBYTE) SlotValue);
    }
};


// Copied from util.hpp, for DacpThreadStoreData.fHostConfig below.
#define CLRMEMORYHOSTED                             0x1
#define CLRTASKHOSTED                               0x2
#define CLRSYNCHOSTED                               0x4
#define CLRTHREADPOOLHOSTED                         0x8
#define CLRIOCOMPLETIONHOSTED                       0x10
#define CLRASSEMBLYHOSTED                           0x20
#define CLRGCHOSTED                                 0x40
#define CLRSECURITYHOSTED                           0x80
#define CLRHOSTED           0x80000000

struct DacpThreadStoreData
{
    LONG threadCount;
    LONG unstartedThreadCount;
    LONG backgroundThreadCount;
    LONG pendingThreadCount;
    LONG deadThreadCount;
    CLRDATA_ADDRESS firstThread;
    CLRDATA_ADDRESS finalizerThread;
    CLRDATA_ADDRESS gcThread;
    DWORD fHostConfig;          // Uses hosting flags defined above
	
    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_THREAD_STORE_DATA,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);
    }
};

struct DacpAppDomainStoreData
{
    CLRDATA_ADDRESS sharedDomain;
    CLRDATA_ADDRESS systemDomain;
    LONG DomainCount;

    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_APPDOMAIN_STORE_DATA,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT GetDomains(IXCLRDataProcess* dac, ULONG32 iArraySize,
                              CLRDATA_ADDRESS Domains[])
    {
        return dac->Request(DACPRIV_REQUEST_APPDOMAIN_LIST,
                            0, NULL,
                            sizeof(CLRDATA_ADDRESS)*iArraySize,
                            (PBYTE) Domains);
    }
};

enum DacpAppDomainDataStage {
    STAGE_CREATING,
    STAGE_READYFORMANAGEDCODE,
    STAGE_ACTIVE,
    STAGE_OPEN,
    STAGE_UNLOAD_REQUESTED,
    STAGE_EXITING,
    STAGE_EXITED,
    STAGE_FINALIZING,
    STAGE_FINALIZED,
    STAGE_HANDLETABLE_NOACCESS,
    STAGE_CLEARED,
    STAGE_COLLECTED,
    STAGE_CLOSED
};

struct DacpAppDomainData
{
    CLRDATA_ADDRESS AppDomainPtr;   // useful to keep in the structure
    CLRDATA_ADDRESS AppSecDesc;
    CLRDATA_ADDRESS pLowFrequencyHeap;
    CLRDATA_ADDRESS pHighFrequencyHeap;
    CLRDATA_ADDRESS pStubHeap;
    CLRDATA_ADDRESS DomainLocalBlock;
    CLRDATA_ADDRESS pDomainLocalModules;    
    // The creation sequence number of this app domain (starting from 1)
    DWORD dwId;
    LONG AssemblyCount;
    LONG FailedAssemblyCount;
    DacpAppDomainDataStage appDomainStage; 
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_APPDOMAIN_DATA,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT GetName(IXCLRDataProcess* dac,
                           CLRDATA_ADDRESS AppDomainaddr,
                           ULONG32 iNameChars,
                           __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_APPDOMAIN_NAME,
                            sizeof(AppDomainaddr), (PBYTE)&AppDomainaddr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }

    static HRESULT GetApplicationBase(IXCLRDataProcess* dac,
                                      CLRDATA_ADDRESS AppDomainaddr,
                                      ULONG32 iNameChars,
                                      __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_APPDOMAIN_APP_BASE,
                            sizeof(AppDomainaddr), (PBYTE)&AppDomainaddr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }

    static HRESULT GetPrivateBinPaths(IXCLRDataProcess* dac,
                                      CLRDATA_ADDRESS AppDomainaddr,
                                      ULONG32 iNameChars,
                                      __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_APPDOMAIN_PRIVATE_BIN_PATHS,
                            sizeof(AppDomainaddr), (PBYTE)&AppDomainaddr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }

    static HRESULT GetConfigFile(IXCLRDataProcess* dac,
                                 CLRDATA_ADDRESS AppDomainaddr,
                                 ULONG32 iNameChars,
                                 __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_APPDOMAIN_CONFIG_FILE,
                            sizeof(AppDomainaddr), (PBYTE)&AppDomainaddr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }

    static HRESULT GetAssemblies(IXCLRDataProcess* dac,
                                 CLRDATA_ADDRESS addr, ULONG32 iArraySize,
                                 CLRDATA_ADDRESS Assemblies[])
    {
        return dac->Request(DACPRIV_REQUEST_ASSEMBLY_LIST,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(CLRDATA_ADDRESS)*iArraySize,
                            (PBYTE) Assemblies);
    }

    static HRESULT GetFailedAssemblies(IXCLRDataProcess* dac,
                                 CLRDATA_ADDRESS addr, ULONG32 iArraySize,
                                 CLRDATA_ADDRESS Assemblies[])
    {
        return dac->Request(DACPRIV_REQUEST_FAILED_ASSEMBLY_LIST,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(CLRDATA_ADDRESS)*iArraySize,
                            (PBYTE) Assemblies);
    }
};

struct DacpAssemblyData
{
    CLRDATA_ADDRESS AssemblyPtr; //useful to have
    CLRDATA_ADDRESS ClassLoader;
    CLRDATA_ADDRESS ParentDomain;
    CLRDATA_ADDRESS AppDomainPtr;
    CLRDATA_ADDRESS AssemblySecDesc;
    BOOL isDynamic;
    UINT ModuleCount;
    UINT LoadContext;
    BOOL isDomainNeutral;
    DWORD dwLocationFlags;

    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr, CLRDATA_ADDRESS appDomainPtr)
    {
        this->AppDomainPtr = appDomainPtr;
        return dac->Request(DACPRIV_REQUEST_ASSEMBLY_DATA,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(*this), (PBYTE)this);
    }

    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return Request(dac, addr, NULL);
    }

    static HRESULT GetName(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr,
                           ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_ASSEMBLY_NAME,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }    

    static HRESULT GetDisplayName(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr,
                                  ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_ASSEMBLY_DISPLAY_NAME,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }    

    static HRESULT GetLocation(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr,
                               ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_ASSEMBLY_LOCATION,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }    

    static HRESULT GetModules(IXCLRDataProcess* dac,
                              CLRDATA_ADDRESS addr, ULONG32 iArraySize,
                              CLRDATA_ADDRESS Modules[])
    {
        return dac->Request(DACPRIV_REQUEST_ASSEMBLYMODULE_LIST,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(CLRDATA_ADDRESS)*iArraySize,
                            (PBYTE) Modules);
    }    
};

struct DacpFailedAssemblyData
{
    CLRDATA_ADDRESS AssemblyPtr; //useful to have
    UINT LoadContext;
    DWORD hResult;
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_FAILED_ASSEMBLY_DATA,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT GetDisplayName(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr,
                                  ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_FAILED_ASSEMBLY_DISPLAY_NAME,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }    

    static HRESULT GetLocation(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr,
                               ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_FAILED_ASSEMBLY_LOCATION,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }
};

struct DacpNestedExceptionInfo
{
    CLRDATA_ADDRESS exceptionObject;
    CLRDATA_ADDRESS nextNestedException;

    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_NESTEDEXCEPTION_DATA,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(*this), (PBYTE)this);    
    } 
};

struct DacpThreadArgs
{
    CLRDATA_ADDRESS addrThread;
    BOOL isFullDump;	// pass FALSE here if you don't have full memory access.
};

struct DacpThreadData
{
    DWORD corThreadId;
    DWORD osThreadId;
    int state;
    ULONG preemptiveGCDisabled;
    CLRDATA_ADDRESS allocContextPtr;
    CLRDATA_ADDRESS allocContextLimit;
    CLRDATA_ADDRESS context;
    CLRDATA_ADDRESS domain;
    CLRDATA_ADDRESS pSharedStaticData;
    CLRDATA_ADDRESS pUnsharedStaticData;
    CLRDATA_ADDRESS pFrame;
    DWORD lockCount;
    CLRDATA_ADDRESS firstNestedException; // Pass this pointer to DacpNestedExceptionInfo
    CLRDATA_ADDRESS fiberData;
    CLRDATA_ADDRESS lastThrownObjectHandle;
    CLRDATA_ADDRESS nextThread;

    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr, BOOL isFullDump=TRUE)
    {
        DacpThreadArgs dta = { addr, isFullDump };
        return dac->Request(DACPRIV_REQUEST_THREAD_DATA,
                            sizeof(DacpThreadArgs), (PBYTE)&dta,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT ThreadFromThinlockId(IXCLRDataProcess* dac, UINT thinLockId, CLRDATA_ADDRESS *outThread)
    {
        return dac->Request(DACPRIV_REQUEST_THREAD_THINLOCK_DATA,
                            sizeof(UINT *), (PBYTE)&thinLockId,
                            sizeof(CLRDATA_ADDRESS *), (PBYTE)outThread);
    }    
};

struct DacpContextData
{
    CLRDATA_ADDRESS domain;
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_CONTEXT_DATA,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(*this), (PBYTE)this);
    }
};

struct DacpMethodDescData
{
    BOOL            bHasNativeCode;
    BOOL            bIsDynamic;
    WORD            wSlotNumber;
    CLRDATA_ADDRESS NativeCodeAddr;
    // Useful for breaking when a method is jitted.
    CLRDATA_ADDRESS AddressOfNativeCodeSlot;
    
    CLRDATA_ADDRESS MethodDescPtr;
    CLRDATA_ADDRESS MethodTablePtr;
    CLRDATA_ADDRESS EEClassPtr;
    CLRDATA_ADDRESS ModulePtr;
    
    CLRDATA_ADDRESS PreStubAddr;
    mdToken                  MDToken;
    CLRDATA_ADDRESS GCInfo;
    WORD                      JITType;
    CLRDATA_ADDRESS GCStressCodeCopy;

    // This is only valid if bIsDynamic is true
    CLRDATA_ADDRESS managedDynamicMethodObject;
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_METHODDESC_DATA,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(*this), (PBYTE)this);
    }

    HRESULT RequestFromIP(IXCLRDataProcess* dac, CLRDATA_ADDRESS IPaddr)
    {
        return dac->Request(DACPRIV_REQUEST_METHODDESC_IP_DATA,
                            sizeof(IPaddr), (PBYTE)&IPaddr,
                            sizeof(*this), (PBYTE)this);
    }

    HRESULT RequestFromFrame(IXCLRDataProcess* dac, CLRDATA_ADDRESS FrameAddr)
    {
        return dac->Request(DACPRIV_REQUEST_METHODDESC_FRAME_DATA,
                            sizeof(FrameAddr), (PBYTE)&FrameAddr,
                            sizeof(*this), (PBYTE)this);
    }


    static HRESULT GetMethodName(IXCLRDataProcess* dac,
                                 CLRDATA_ADDRESS addrMethodDesc,
                                 ULONG32 iNameChars,
                                 __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_METHODDESC_NAME,
                            sizeof(addrMethodDesc), (PBYTE)&addrMethodDesc,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }
};

// for JITType
typedef enum JITTypes {TYPE_UNKNOWN=0,TYPE_JIT,TYPE_EJIT,TYPE_PJIT};

struct DacpCodeHeaderData
{        
    CLRDATA_ADDRESS GCInfo;
    JITTypes                   JITType;
    CLRDATA_ADDRESS MethodDescPtr;
    CLRDATA_ADDRESS MethodStart;
    DWORD                    MethodSize;
    CLRDATA_ADDRESS ColdRegionStart;
    DWORD           ColdRegionSize;
    DWORD           HotRegionSize;
    
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS IPAddr)
    {
        return dac->Request(DACPRIV_REQUEST_CODEHEADER_DATA,
                            sizeof(IPAddr), (PBYTE)&IPAddr,
                            sizeof(*this), (PBYTE)this);    
    }
};

typedef enum WorkRequestFunctionTypes
{
    QUEUEUSERWORKITEM, TIMERDELETEWORKITEM, ASYNCCALLBACKCOMPLETION,
    ASYNCTIMERCALLBACKCOMPLETION, UNKNOWNWORKITEM
};

struct DacpWorkRequestData
{
    WorkRequestFunctionTypes FunctionType;
    CLRDATA_ADDRESS Function;
    CLRDATA_ADDRESS Context;
    CLRDATA_ADDRESS NextWorkRequest;
        
    HRESULT Request(IXCLRDataProcess* dac,CLRDATA_ADDRESS data)
    {
        return dac->Request(DACPRIV_REQUEST_WORKREQUEST_DATA,
                            sizeof(data), (PBYTE) &data,
                            sizeof(*this), (PBYTE)this);    
    }
};

struct DacpThreadpoolData
{
    LONG cpuUtilization;    
    LONG NumWorkerThreads;
    LONG MinLimitTotalWorkerThreads;
    LONG MaxLimitTotalWorkerThreads;
    LONG NumRunningWorkerThreads;
    LONG NumIdleWorkerThreads;
    LONG NumQueuedWorkRequests;

    CLRDATA_ADDRESS FirstWorkRequest;

    DWORD NumTimers;

    LONG   NumCPThreads;
    LONG   NumFreeCPThreads;
    LONG   MaxFreeCPThreads; 
    LONG   NumRetiredCPThreads;
    LONG   MaxLimitTotalCPThreads;
    LONG   CurrentLimitTotalCPThreads;
    LONG   MinLimitTotalCPThreads;

    CLRDATA_ADDRESS QueueUserWorkItemCallbackFPtr;
    CLRDATA_ADDRESS AsyncCallbackCompletionFPtr;
    CLRDATA_ADDRESS AsyncTimerCallbackCompletionFPtr;
    
    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_THREADPOOL_DATA,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);    
    }
};

struct DacpGenerationData
{    
    CLRDATA_ADDRESS start_segment;
    CLRDATA_ADDRESS allocation_start;

    // These are examined only for generation 0, otherwise NULL
    CLRDATA_ADDRESS allocContextPtr;
    CLRDATA_ADDRESS allocContextLimit;
};

#define DAC_NUMBERGENERATIONS 4

struct DacpGcHeapDetails
{
    CLRDATA_ADDRESS heapAddr; // Only filled in in server mode, otherwise NULL
#ifndef GC_SMP
    CLRDATA_ADDRESS alloc_allocated;
#endif
    DacpGenerationData generation_table [DAC_NUMBERGENERATIONS]; 
    CLRDATA_ADDRESS ephemeral_heap_segment;        
    CLRDATA_ADDRESS finalization_fill_pointers [DAC_NUMBERGENERATIONS + 2];
    CLRDATA_ADDRESS lowest_address;
    CLRDATA_ADDRESS highest_address;
    CLRDATA_ADDRESS card_table;

    // Use this for workstation mode (DacpGcHeapDat.bServerMode==FALSE).
    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_GCHEAPDETAILS_STATIC_DATA,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);    
    }

    // Use this for Server mode, as there are multiple heaps,
    // and you need to pass a heap address in addr.
    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_GCHEAPDETAILS_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);    
    }
};

struct DacpGcHeapData
{
    BOOL bServerMode;
    BOOL bGcStructuresValid;
    UINT HeapCount;
    UINT g_max_generation;
    
    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_GCHEAP_DATA,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);    
    }

    static HRESULT GetHeaps(IXCLRDataProcess* dac, ULONG32 iArraySize,
                            CLRDATA_ADDRESS Heaps[])
    {
        return dac->Request(DACPRIV_REQUEST_GCHEAP_LIST,
                            0, NULL,
                            sizeof(CLRDATA_ADDRESS)*iArraySize, (PBYTE) Heaps);
    }
};

struct DacpHeapSegmentData
{
    CLRDATA_ADDRESS segmentAddr;
    CLRDATA_ADDRESS allocated;
    CLRDATA_ADDRESS committed;
    CLRDATA_ADDRESS reserved;
    CLRDATA_ADDRESS used;
    CLRDATA_ADDRESS mem;
    // pass this to request if non-null to get the next segments.
    CLRDATA_ADDRESS next;
    CLRDATA_ADDRESS gc_heap; // only filled in in server mode, otherwise NULL
    // computed field: if this is the ephemeral segment highMark includes the ephemeral generation
    CLRDATA_ADDRESS highAllocMark;

    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr, const DacpGcHeapDetails& heap)
    {
        HRESULT hr = dac->Request(DACPRIV_REQUEST_HEAPSEGMENT_DATA,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            sizeof(*this), (PBYTE)this);
        if (hr==S_OK)
        {
            if (this->segmentAddr == heap.generation_table[0].start_segment)
#ifndef GC_SMP
                highAllocMark = heap.alloc_allocated;
#else
                highAllocMark = allocated;
#endif
            else
                highAllocMark = allocated;
        }    
        return hr;
    }
};

struct DacpDACUnitTest
{
    CLRDATA_ADDRESS threadStoreAddr;
    CLRDATA_ADDRESS threadClassAddr;
    bool fProcessDetach;
    
    HRESULT Request(IXCLRDataProcess* dac)
    {
        return dac->Request(DACPRIV_REQUEST_UNITTEST_DATA,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);    
    }
};

// Is the given address a stub?
// This is similar to IsCorCode, but Normal jitted code is not a stub.
// fIsStub - true iff the given address is a CLR stub.
struct DacpIsStub
{
    BOOL fIsStub;

    HRESULT Request(IXCLRDataProcess* dac, CLRDATA_ADDRESS addr)
    {
        return dac->Request(DACPRIV_REQUEST_ISSTUB,
                            sizeof(addr), (PBYTE)&addr,
                            sizeof(*this), (PBYTE) this);
    }

};
#define SYNCBLOCKDATA_COMFLAGS_CCW 1
#define SYNCBLOCKDATA_COMFLAGS_RCW 2
#define SYNCBLOCKDATA_COMFLAGS_CF 4

struct DacpSyncBlockData
{        
    CLRDATA_ADDRESS Object;
    BOOL            bFree; // if set, no other fields are useful
    
    // fields below provide data from this, so it's just for display
    CLRDATA_ADDRESS SyncBlockPointer;
    UINT            MonitorHeld;
    UINT            Recursion;
    CLRDATA_ADDRESS HoldingThread;
    UINT            AdditionalThreadCount;
    CLRDATA_ADDRESS appDomainPtr;
    
    // SyncBlockCount will always be filled in with the number of SyncBlocks.
    // SyncBlocks may be requested from [1,SyncBlockCount]
    UINT            SyncBlockCount;

    // SyncBlockNumber must be from [1,SyncBlockCount]    
    // If there are no SyncBlocks, a call to Request with SyncBlockCount = 1
    // will return E_FAIL.
    HRESULT Request(IXCLRDataProcess* dac, UINT SyncBlockNumber)
    {
        return dac->Request(DACPRIV_REQUEST_SYNCBLOCK_DATA,
                            sizeof(SyncBlockNumber), (PBYTE)&SyncBlockNumber,
                            sizeof(*this), (PBYTE)this);    
    }
};

struct DacpSyncBlockCleanupData
{
    CLRDATA_ADDRESS SyncBlockPointer;
    
    CLRDATA_ADDRESS nextSyncBlock;
    CLRDATA_ADDRESS blockRCW;
    CLRDATA_ADDRESS blockClassFactory;
    CLRDATA_ADDRESS blockCCW;
    
    // Pass NULL on the first request to start a traversal.
    HRESULT Request(IXCLRDataProcess *dac, CLRDATA_ADDRESS psyncBlock)
    {
        return dac->Request(DACPRIV_REQUEST_SYNCBLOCK_CLEANUP_DATA,
                            sizeof(CLRDATA_ADDRESS),(PBYTE) &psyncBlock,
                            sizeof(*this), (PBYTE) this);
    }
};

typedef BOOL (*VISITRCWFORCLEANUP)(CLRDATA_ADDRESS RCW,CLRDATA_ADDRESS Context,CLRDATA_ADDRESS Thread,
    LPVOID token);

struct DacpRcwCleanupTraverseArgs
{
    VISITRCWFORCLEANUP Callback;
    LPVOID token;
    CLRDATA_ADDRESS cleanupListPtr;
};

struct DacpRcwCleanupTraverse
{
    static HRESULT DoTraverse(IXCLRDataProcess* dac,VISITRCWFORCLEANUP pFunc, LPVOID token, 
        CLRDATA_ADDRESS cleanupList=NULL)
    {
        DacpRcwCleanupTraverseArgs args = { pFunc, token, cleanupList };
        return dac->Request(DACPRIV_REQUEST_RCWCLEANUP_TRAVERSE,
                            sizeof(DacpRcwCleanupTraverseArgs), (BYTE *)&args,
                            0, NULL);    
    }
};

///////////////////////////////////////////////////////////////////////////

enum EHClauseType {EHFault, EHFinally, EHFilter, EHTyped, EHUnknown};
                
struct DACEHInfo
{
    EHClauseType clauseType;
    CLRDATA_ADDRESS tryStartOffset;
    CLRDATA_ADDRESS tryEndOffset;
    CLRDATA_ADDRESS handlerStartOffset;
    CLRDATA_ADDRESS handlerEndOffset;
    BOOL isDuplicateClause;

    CLRDATA_ADDRESS filterOffset; // valid when clauseType is EHFilter
    BOOL isCatchAllHandler; // valid when clauseType is EHTyped
};

typedef BOOL (*DUMPEHINFO)(UINT clauseIndex,UINT totalClauses,DACEHInfo *pEHInfo,LPVOID token);

struct DacpEHInfoTraverseArgs
{
    DUMPEHINFO Callback;
    LPVOID token;
    CLRDATA_ADDRESS IPAddress;
};

struct DacpEHInfoTraverse
{
    static HRESULT DoTraverse(IXCLRDataProcess* dac,DUMPEHINFO pFunc, LPVOID token, 
        CLRDATA_ADDRESS IPAddress)
    {
        DacpEHInfoTraverseArgs args = { pFunc, token, IPAddress };
        return dac->Request(DACPRIV_REQUEST_EHINFO_TRAVERSE,
                            sizeof(DacpEHInfoTraverseArgs), (BYTE *)&args,
                            0, NULL);    
    }
};

///////////////////////////////////////////////////////////////////////////
typedef BOOL (*VISITHANDLE)(CLRDATA_ADDRESS HandleAddr,CLRDATA_ADDRESS HandleValue,int HandleType,
    CLRDATA_ADDRESS appDomainPtr,LPVOID token);

struct DacpHandleTableTraverseArgs
{
    VISITHANDLE Callback;
    LPVOID token;
};

struct DacpHandleTableTraverse
{
    static HRESULT DoTraverse(IXCLRDataProcess* dac,VISITHANDLE pFunc, LPVOID token)
    {
        DacpHandleTableTraverseArgs args = { pFunc, token };
        return dac->Request(DACPRIV_REQUEST_HANDLETABLE_TRAVERSE,
                            sizeof(DacpHandleTableTraverseArgs), (BYTE *)&args,
                            0, NULL);    
    }
};

typedef void (*VISITHEAP)(CLRDATA_ADDRESS blockData,size_t blockSize,BOOL blockIsCurrentBlock);
struct DacpLoaderHeapTraverseArgs
{
    CLRDATA_ADDRESS LoaderHeapAddr;
    VISITHEAP Callback;
};
struct DacpLoaderHeapTraverse
{
    static HRESULT DoTraverse(IXCLRDataProcess* dac,CLRDATA_ADDRESS LoaderHeapAddr,
                                 VISITHEAP pFunc)
    {
        DacpLoaderHeapTraverseArgs args = { LoaderHeapAddr, pFunc };

        return dac->Request(DACPRIV_REQUEST_LOADERHEAP_TRAVERSE,
                            sizeof(DacpLoaderHeapTraverseArgs), (BYTE *)&args,
                            0, NULL);    
    }
};

enum VCSHeapType {IndcellHeap, LookupHeap, ResolveHeap, DispatchHeap, CacheEntryHeap};
struct DacpVirtualCallStubHeapTraverseArgs
{
    CLRDATA_ADDRESS pAppDomain;
    VCSHeapType heaptype;    
    VISITHEAP Callback;
};
struct DacpVirtualCallStubHeapTraverse
{
    static HRESULT DoTraverse(IXCLRDataProcess* dac,CLRDATA_ADDRESS pAppDomain, VCSHeapType heaptype,
                                 VISITHEAP pFunc)
    {
        DacpVirtualCallStubHeapTraverseArgs args = { pAppDomain, heaptype, pFunc };

        return dac->Request(DACPRIV_REQUEST_VIRTCALLSTUBHEAP_TRAVERSE,
                            sizeof(DacpVirtualCallStubHeapTraverseArgs), (BYTE *)&args,
                            0, NULL);    
    }
};

struct DacpStressLogData
{
    CLRDATA_ADDRESS StressLogAddress;
    HRESULT Request(IXCLRDataProcess* pThread)
    {
        return pThread->Request(DACPRIV_REQUEST_STRESSLOG_DATA,
                                0, NULL,
                                sizeof(*this), (PBYTE) this);
    }
};

typedef void (*PRINTF)(PCSTR Format, ...);

#ifdef _DEBUG
struct DacMdaArgs
{
    PRINTF pfPrintf;
    PCSTR szCommand;
};

struct DacpMda
{
    HRESULT Request(IXCLRDataProcess* dac, PRINTF pFunc, PCSTR szCommand)
    {
        DacMdaArgs args = { pFunc, szCommand };
        return dac->Request(DACPRIV_REQUEST_MDA,
                            sizeof(DacMdaArgs), (BYTE*)&args,
                            0, NULL);
    }
};
#endif

// Is the given thread inside the runtime?
struct DacpIsCantStop
{
    BOOL fIsCantStop;
    
    HRESULT Request(IXCLRDataTask* pThread)
    {
        return pThread->Request(DACTASKPRIV_REQUEST_IS_CANTSTOP,
                                0, NULL,
                                sizeof(*this), (PBYTE) this);
    }
};

struct DacpGetModuleAddress
{
    CLRDATA_ADDRESS ModulePtr;
    HRESULT Request(IXCLRDataModule* pDataModule)
    {
        return pDataModule->Request(DACDATAMODULEPRIV_REQUEST_GET_MODULEPTR,
                                0, NULL,
                                sizeof(*this), (PBYTE) this);
    }
};

struct DacpFrameData
{
    CLRDATA_ADDRESS frameAddr;

    // Could also be implemented for IXCLRDataFrame if desired.
    HRESULT Request(IXCLRDataStackWalk* dac)
    {
        return dac->Request(DACSTACKPRIV_REQUEST_FRAME_DATA,
                            0, NULL,
                            sizeof(*this), (PBYTE)this);
    }

    static HRESULT GetFrameName(IXCLRDataProcess* dac,
                                 CLRDATA_ADDRESS VTableForFrame,
                                 ULONG32 iNameChars,
                                 __out_ecount (iNameChars) LPWSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_FRAME_NAME,
                            sizeof(VTableForFrame), (PBYTE)&VTableForFrame,
                            sizeof(WCHAR)*iNameChars, (PBYTE) pwszName);
    }

};

struct DacpJitManagerInfo
{
    CLRDATA_ADDRESS managerAddr;
    DWORD codeType; // for union below

    union
    {
        CLRDATA_ADDRESS ptrHeapList;    // A HeapList * if IsMiIL(codeType)
    };
};
            
        
struct DacpJitListInfo
{
    LONG JitCount;
    
    HRESULT Request(IXCLRDataProcess* pThread)
    {
        return pThread->Request(DACPRIV_REQUEST_JITLIST,
                                0, NULL,
                                sizeof(*this), (PBYTE) this);
    }

    static HRESULT GetJitManagers(IXCLRDataProcess* dac, ULONG32 iArraySize,
                              DacpJitManagerInfo Managers[])
    {
        return dac->Request(DACPRIV_REQUEST_MANAGER_LIST,
                            0, NULL,
                            sizeof(DacpJitManagerInfo)*iArraySize,
                            (PBYTE) Managers);
    }

};

struct DacpJitHelperFunctionData
{
    static HRESULT GetName(IXCLRDataProcess* dac,
                           CLRDATA_ADDRESS addr,
                           ULONG32 iNameChars,
                           __out_ecount (iNameChars) LPSTR pwszName)
    {
        return dac->Request(DACPRIV_REQUEST_JIT_HELPER_FUNCTION_NAME,
                            sizeof(CLRDATA_ADDRESS), (PBYTE) &addr,
                            iNameChars, (PBYTE) pwszName);
    }
};

struct DacpJumpThunkData
{
    CLRDATA_ADDRESS Target;
    CLRDATA_ADDRESS TargetMethodDesc;

    HRESULT GetJumpThunkTarget (IXCLRDataProcess* dac,
                                       CONTEXT *pctx)
    {
        return dac->Request(DACPRIV_REQUEST_JUMP_THUNK_TARGET,
                            sizeof(*pctx), (PBYTE) pctx,
                            sizeof(*this), (PBYTE) this);
    }
};

typedef enum CodeHeapType {CODEHEAP_LOADER=0,CODEHEAP_HOST,CODEHEAP_UNKNOWN};

struct DacpJitCodeHeapInfo
{
    DWORD codeHeapType; // for union below

    union
    {
        CLRDATA_ADDRESS LoaderHeap;    // if CODEHEAP_LOADER
        struct
        {
            CLRDATA_ADDRESS baseAddr; // if CODEHEAP_HOST
            CLRDATA_ADDRESS currentAddr;
        } HostData;
    };
};
            
        
struct DacpJitHeapList
{
    CLRDATA_ADDRESS eeJitManager; // fill this in before calling Request
    LONG heapListCount;
    
    HRESULT Request(IXCLRDataProcess* pThread)
    {
        return pThread->Request(DACPRIV_REQUEST_JITHEAPLIST,
                                sizeof(*this), (PBYTE) this,
                                sizeof(*this), (PBYTE) this);
    }

    HRESULT GetCodeHeaps(IXCLRDataProcess* dac, ULONG32 iArraySize,
                              DacpJitCodeHeapInfo codeHeaps[])
    {
        return dac->Request(DACPRIV_REQUEST_CODEHEAP_LIST,
                            sizeof(*this), (PBYTE) this,
                            sizeof(DacpJitCodeHeapInfo)*iArraySize,
                            (PBYTE) codeHeaps);
    }
};


#endif  // _DACPRIVATE_H_
