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
/*============================================================
**
** Header:  AssemblySpec.hpp
**
** Purpose: Implements classes used to bind to assemblies
**
** Date:  May 7, 2000
**
===========================================================*/
#ifndef _ASSEMBLYSPEC_H
#define _ASSEMBLYSPEC_H
#include "fusionbind.h"
#include "hash.h"
#include "memorypool.h"
#include "fusion.h"
#include "domainfile.h"
#include "genericstackprobe.h"
#include "holder.h"

class AppDomain;
class Assembly;
class DomainAssembly;
enum FileLoadLevel;

class AssemblySpec : public FusionBind
{
  private:

    friend class AppDomain;
    friend class AssemblyNameNative;
    
    AppDomain       *m_pAppDomain;
    SBuffer          m_HashForControl;
    DWORD            m_dwHashAlg;
    BOOL             m_fIntrospectionOnly;
    ReleaseHolder<IAssemblyName> m_pNameAfterPolicy;

    BOOL IsValidAssemblyName();
    
  public:

    AssemblySpec()
      : m_pAppDomain(::GetAppDomain()), m_fIntrospectionOnly(0) {
        LEAF_CONTRACT;
        m_context.usMajorVersion = (USHORT) -1;
        m_context.usMinorVersion = (USHORT) -1;
        m_context.usBuildNumber = (USHORT) -1;
        m_context.usRevisionNumber = (USHORT) -1;
    }

    AssemblySpec(AppDomain *pAppDomain)
      : m_pAppDomain(pAppDomain), m_fIntrospectionOnly(0) {LEAF_CONTRACT;}

    BOOL InitializeSpec(mdToken kAssemblyRefOrDef, 
                           IMDInternalImport *pImport, 
                        PEAssembly *pStaticParent = NULL,
                        BOOL fIntrospectionOnly = FALSE,
                        BOOL fThrow = TRUE);
    void InitializeSpec(IAssemblyName *pName,
                        PEAssembly *pStaticParent = NULL,
                        BOOL fIntrospectionOnly = FALSE);
    void InitializeSpec(PEAssembly *pFile);
    HRESULT InitializeSpec(StackingAllocator* alloc,
                        ASSEMBLYNAMEREF* pName,
                        BOOL fParsed = TRUE,
                        BOOL fIntrospectionOnly = FALSE);

    void SetCodeBase(LPCWSTR szCodeBase, DWORD dwCodeBase)
    {
        WRAPPER_CONTRACT;
        FusionBind::SetCodeBase(szCodeBase, dwCodeBase);
    }

    void SetCodeBase(StackingAllocator* alloc, STRINGREF *pCodeBase);

    void SetParentAssembly(PEAssembly* pAssembly);

    BOOL IsIntrospectionOnly()
    {
        LEAF_CONTRACT;

        // Important to ensure we return a normalized boolean (the introspection fields
        // of different AssemblySpecs can be compared.)
        return !!m_fIntrospectionOnly;
    }

    VOID SetIntrospectionOnly(BOOL fIntrospectionOnly)
    {
        LEAF_CONTRACT;
        m_fIntrospectionOnly = !!fIntrospectionOnly;
    }

    IAssemblyName* GetNameAfterPolicy()
    {
        return m_pNameAfterPolicy;
    };

    void ReleaseNameAfterPolicy()
    {
        m_pNameAfterPolicy=NULL;
    };    

    void SetNameAfterPolicy(IAssemblyName* pName)
    {
        m_pNameAfterPolicy=pName;
    };

     

    // Make sure this matches in the managed Assembly.DemandPermission()
    enum FilePermFlag {
        FILE_PATHDISCOVERY   = 0x0,
        FILE_READ            = 0x1,
        FILE_READANDPATHDISC = 0x2,
        FILE_WEBPERM         = 0x3
    };
    static void DemandFileIOPermission(LPCWSTR wszCodeBase,
                                       BOOL fHavePath,
                                       DWORD dwDemandFlag);
    void DemandFileIOPermission(PEAssembly *pFile);

    Assembly *LoadAssembly(FileLoadLevel targetLevel, 
                           OBJECTREF *pEvidence = NULL, 
                           OBJECTREF *pExtraEvidence = NULL,
                           BOOL fDelayPolicyResolution = FALSE,
                           BOOL fThrowOnFileNotFound = TRUE,
                           BOOL fRaisePrebindEvents = TRUE,
                           StackCrawlMark *pCallerStackMark = NULL);
    DomainAssembly *LoadDomainAssembly(FileLoadLevel targetLevel,
                                       OBJECTREF *pEvidence = NULL,
                                       OBJECTREF *pExtraEvidence = NULL,
                                       BOOL fDelayPolicyResolution = FALSE,
                                       BOOL fThrowOnFileNotFound = TRUE,
                                       BOOL fRaisePrebindEvents = TRUE,
                                       StackCrawlMark *pCallerStackMark = NULL);

    //****************************************************************************************
    //
    // Creates and loads an assembly based on the name and context.
    static Assembly *LoadAssembly(LPCSTR pSimpleName, 
                                  AssemblyMetaDataInternal* pContext,
                                  PBYTE pbPublicKeyOrToken,
                                  DWORD cbPublicKeyOrToken,
                                  DWORD dwFlags);

    // Load an assembly based on an explicit path
    static Assembly *LoadAssembly(LPCWSTR pFilePath);
    BOOL FindAssemblyFile(AppDomain *pAppDomain, BOOL fThrowOnFileNotFound,
                          IAssembly** ppIAssembly, IHostAssembly **ppIHostAssembly, IAssembly** pNativeFusionAssembly,
                          IFusionBindLog **ppFusionLog, StackCrawlMark *pCallerStackMark = NULL);

  private:
    void MatchRetargetedPublicKeys(Assembly *pAssembly);
  public:
    void MatchPublicKeys(Assembly *pAssembly);
    
    PEAssembly *ResolveAssemblyFile(AppDomain *pAppDomain, BOOL fPreBind);

    // Encapsulates the logic to identify a spec to mscorlib
    BOOL IsMscorlib();
    // Is this a satellite assembly for mscorlib (for localization)?
    BOOL IsMscorlibSatellite();


    AppDomain *GetAppDomain() 
    {
        LEAF_CONTRACT;
        return m_pAppDomain;
    }

    HRESULT SetHashForControl(PBYTE pHashForControl, DWORD dwHashForControl, DWORD dwHashAlg)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer(pHashForControl));
        }
        CONTRACTL_END;

        m_HashForControl.Set(pHashForControl, dwHashForControl);
        m_dwHashAlg=dwHashAlg; 
        return S_OK;
    };
};


#define INITIAL_ASM_SPEC_HASH_SIZE 7
class AssemblySpecHash
{
    LoaderHeap *m_pHeap;
    PtrHashMap m_map;

  public:

#ifndef DACCESS_COMPILE
    AssemblySpecHash(LoaderHeap *pHeap = NULL)
      : m_pHeap(pHeap)
    {
        CONTRACTL
        {
            CONSTRUCTOR_CHECK;
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
   
        m_map.Init(INITIAL_ASM_SPEC_HASH_SIZE, CompareSpecs, FALSE, NULL);
    }

    ~AssemblySpecHash();
#endif

#ifndef DACCESS_COMPILE
    //
    // Returns TRUE if the spec was already in the table
    //

    BOOL Store(AssemblySpec *pSpec)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        DWORD key = pSpec->Hash();

        AssemblySpec *entry = (AssemblySpec *) m_map.LookupValue(key, pSpec);

        if (entry == (AssemblySpec*) INVALIDENTRY)
        {
            if (m_pHeap != NULL)
                entry = new (m_pHeap->AllocMem(sizeof(AssemblySpec))) AssemblySpec;
            else
                entry = new AssemblySpec;

            GCX_PREEMP();
            entry->Init(pSpec);
            // Since pSpec may be pointing into an ngen image, which can get
            // discarded, ensure that all fields are cloned
            entry->CloneFields(AssemblySpec::ALL_OWNED);

            m_map.InsertValue(key, entry);

            return FALSE;
        }
        else
            return TRUE;
    }
#endif // DACCESS_COMPILE

    DWORD Hash(AssemblySpec *pSpec)
    {
        WRAPPER_CONTRACT;
        return pSpec->Hash();
    }

    static BOOL CompareSpecs(UPTR u1, UPTR u2)
    {
        WRAPPER_CONTRACT;

        AssemblySpec *a1 = (AssemblySpec *) (u1 << 1);
        AssemblySpec *a2 = (AssemblySpec *) u2;

        if((!a1->CompareEx(a2)) ||
           (a1->IsIntrospectionOnly() != a2->IsIntrospectionOnly()))
            return FALSE;

        return TRUE;
    }
};


class AssemblySpecBindingCache
{
    friend class AssemblyBindingHolder;
    struct AssemblyBinding
    {
        public: 
        ~AssemblyBinding()
        {
            WRAPPER_CONTRACT;

            if (m_pFile != NULL)
                m_pFile->Release();

            if (m_exceptionType==EXTYPE_EE)
                delete m_pException;
        };

        void OnAppDomainUnload()
        {
            if (m_exceptionType == EXTYPE_EE)
            {
                m_exceptionType = EXTYPE_NONE;
                delete m_pException;
                m_pException = NULL;
            }
        };

        inline DomainAssembly* GetAssembly(){ LEAF_CONTRACT; return m_pAssembly;};
        inline void SetAssembly(DomainAssembly* pAssembly){ LEAF_CONTRACT;  m_pAssembly=pAssembly;};        
        inline PEAssembly* GetFile(){ LEAF_CONTRACT; return m_pFile;};
        inline BOOL IsError(){ LEAF_CONTRACT; return (m_exceptionType!=EXTYPE_NONE);};
        
        inline void ThrowIfError()
        {
            CONTRACTL
            {
                THROWS;
                GC_TRIGGERS;
                MODE_ANY;
            }
            CONTRACTL_END;

            switch(m_exceptionType)
            {
                case EXTYPE_NONE: return;
                case EXTYPE_HR: ThrowHR(m_hr);
                case EXTYPE_EE:  PAL_CPP_THROW(Exception *, m_pException->DomainBoundClone()); 
                default: _ASSERTE(!"Unexpected exception type");
            }
        };
        inline void Init(AssemblySpec* pSpec, PEAssembly* pFile, DomainAssembly* pAssembly, Exception* pEx, LoaderHeap *pHeap, AllocMemTracker *pamTracker)
        {
            CONTRACTL
            {
                THROWS;
                WRAPPER(GC_TRIGGERS);
                MODE_ANY;
            }
            CONTRACTL_END;

            InitInternal(pSpec,pFile,pAssembly);
            if (pHeap != NULL)
            {
                m_spec.CloneFieldsToLoaderHeap(m_spec.ALL_OWNED, pHeap, pamTracker);
            }
            else
            {
                IfFailThrow(m_spec.CloneFields(m_spec.ALL_OWNED));
            }
            InitException(pEx);

        }

        inline void ShallowInit(AssemblySpec* pSpec,
                                PEAssembly* pFile,
                                DomainAssembly* pAssembly,
                                Exception* pEx )
        {
            WRAPPER_CONTRACT;
            InitInternal(pSpec,pFile,pAssembly);
            InitException(pEx);
        }

        inline HRESULT GetHR()
        {
            LEAF_CONTRACT;
            switch(m_exceptionType)
            {
                case EXTYPE_NONE: return S_OK;
                case EXTYPE_HR: return m_hr;
                case EXTYPE_EE:  return m_pException->GetHR(); 
                default: _ASSERTE(!"Unexpected exception type");
            }
            return E_UNEXPECTED;
        };
        
        inline void InitException(Exception* pEx)
        {
            CONTRACTL
            {
                THROWS;
                WRAPPER(GC_TRIGGERS);
                MODE_ANY;
            }
            CONTRACTL_END;
            
             _ASSERTE(m_exceptionType==EXTYPE_NONE);
             
             if(pEx==NULL)
                return;

             _ASSERTE(!pEx->IsTransient());

            EX_TRY
            {
                m_pException = pEx->DomainBoundClone();
                _ASSERTE(m_pException);
                m_exceptionType=EXTYPE_EE;
            }
            EX_CATCH
            {
                InitException(pEx->GetHR());
            }
            EX_END_CATCH(RethrowTransientExceptions);
             
        };
        
        inline void InitException(HRESULT hr)
        {
             LEAF_CONTRACT;
             _ASSERTE(m_exceptionType==EXTYPE_NONE);
             if (FAILED(hr))
             {
                 m_exceptionType=EXTYPE_HR;
                 m_hr=hr;
             }
        };
        protected:
        
        
        inline void InitInternal(AssemblySpec* pSpec, PEAssembly* pFile, DomainAssembly* pAssembly )            
        {
            WRAPPER_CONTRACT;
            m_spec.Init(pSpec);
            m_spec.SetIntrospectionOnly(pSpec->IsIntrospectionOnly());
            m_pFile = pFile;
            if (m_pFile)
                m_pFile->AddRef();
            m_pAssembly = pAssembly;
            m_exceptionType=EXTYPE_NONE;
        }
            
        AssemblySpec    m_spec;
        PEAssembly      *m_pFile;
        DomainAssembly  *m_pAssembly;
        enum{
        EXTYPE_NONE               = 0x00000000,
        EXTYPE_HR                    = 0x00000001,
        EXTYPE_EE                    = 0x00000002,
        };      
        INT         m_exceptionType;
        union
        {
            HRESULT m_hr;
            Exception* m_pException;
        };
    };

    PtrHashMap m_map;
    LoaderHeap *m_pHeap;

  public:

    AssemblySpecBindingCache() DAC_EMPTY();
    ~AssemblySpecBindingCache() DAC_EMPTY();

    void Init(CrstBase *pCrst, LoaderHeap *pHeap = NULL);
    void Clear();

    void OnAppDomainUnload();

    BOOL Contains(AssemblySpec *pSpec);

    DomainAssembly *LookupAssembly(AssemblySpec *pSpec, BOOL fThrow=TRUE);
    PEAssembly *LookupFile(AssemblySpec *pSpec);

    BOOL StoreAssembly(AssemblySpec *pSpec, DomainAssembly *pAssembly, BOOL clone);
    BOOL StoreFile(AssemblySpec *pSpec, PEAssembly *pFile, BOOL clone);
    BOOL StoreException(AssemblySpec *pSpec, Exception* pEx, BOOL clone);

    DWORD Hash(AssemblySpec *pSpec)
    {
        WRAPPER_CONTRACT;
        return pSpec->Hash();
    }

    static BOOL CompareSpecs(UPTR u1, UPTR u2)
    {
        WRAPPER_CONTRACT;

        AssemblySpec *a1 = (AssemblySpec *) (u1 << 1);
        AssemblySpec *a2 = (AssemblySpec *) u2;

        if ((!a1->CompareEx(a2)) ||
            (a1->IsIntrospectionOnly() != a2->IsIntrospectionOnly()))
            return FALSE;

        return TRUE;  
    }
};

#define INITIAL_DOMAIN_ASSEMBLY_CACHE_SIZE 17
class DomainAssemblyCache
{
    struct AssemblyEntry {
        AssemblySpec spec;
        LPVOID       pData[2];     // Can be an Assembly, PEAssembly, or an Unmanaged DLL
        
        DWORD Hash()
        {
            WRAPPER_CONTRACT;
            return spec.Hash();
        }
    };
        
    PtrHashMap  m_Table;
    AppDomain*  m_pDomain;

public:

    static BOOL CompareBindingSpec(UPTR spec1, UPTR spec2);

    void InitializeTable(AppDomain* pDomain, CrstBase *pCrst)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(pDomain);
        m_pDomain = pDomain;

        LockOwner lock = {pCrst, IsOwnerOfCrst};
        m_Table.Init(INITIAL_DOMAIN_ASSEMBLY_CACHE_SIZE, &CompareBindingSpec, true, &lock);
    }
    
    AssemblyEntry* LookupEntry(AssemblySpec* pSpec);

    LPVOID  LookupEntry(AssemblySpec* pSpec, UINT index)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(index < 2);
        AssemblyEntry* ptr = LookupEntry(pSpec);
        if(ptr == NULL)
            return NULL;
        else
            return ptr->pData[index];
    }

    VOID InsertEntry(AssemblySpec* pSpec, LPVOID pData1, LPVOID pData2 = NULL);

private:

};

#endif
