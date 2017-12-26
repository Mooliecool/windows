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
#ifndef _H_CONTEXT_
#define _H_CONTEXT_

#include "specialstatics.h"
#include "fcall.h"


typedef DPTR(class Context) PTR_Context;

class Context
{
public:
    enum CallbackType
    {
        Wait_callback = 0,
        MonitorWait_callback = 1,
        ADTransition_callback = 2,
        SignalAndWait_callback = 3
    };

    typedef struct
    {
        int     numWaiters;
        HANDLE* waitHandles;
        BOOL    waitAll;
        DWORD   millis;
        BOOL    alertable;
        DWORD*  pResult;    
    } WaitArgs;

    typedef struct
    {
        HANDLE* waitHandles;
        DWORD   millis;
        BOOL    alertable;
        DWORD*  pResult;    
    } SignalAndWaitArgs;

    typedef struct
    {
        INT32           millis;          
        PendingSync*    syncState;     
        BOOL*           pResult;
    } MonitorWaitArgs;


    typedef struct
    {
        enum CallbackType   callbackId;
        void*               callbackData;
    } CallBackInfo;

    typedef void (*ADCallBackFcnType)(LPVOID);

    struct ADCallBackArgs
    {
        ADCallBackFcnType pTarget;
        LPVOID pArguments;
    };

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

private:
#ifndef DACCESS_COMPILE
    void SetDomain(AppDomain *pDomain)
    {
        LEAF_CONTRACT;
        m_pDomain = pDomain;
    }
#endif

public:

friend class Thread;
friend class ThreadNative;
friend class ContextBaseObject;
friend class CRemotingServices;
friend struct PendingSync;

    Context(AppDomain *pDomain);
    ~Context();    
    static void Initialize();

    // Get and Set the exposed System.Runtime.Remoting.Context
    // object which corresponds to this context.
    OBJECTREF   GetExposedObject();
    OBJECTREF   GetExposedObjectRaw();
    Object*     GetExposedObjectRawUnchecked();
    void        SetExposedObject(OBJECTREF exposed);
    
    // Query whether the exposed object exists
    BOOL IsExposedObjectSet();

    AppDomain* GetDomain()
    {
        LEAF_CONTRACT;
        return m_pDomain;
    }

    static LPVOID GetStaticFieldAddress(FieldDesc *pFD);

    LPVOID GetStaticFieldAddrNoCreate(FieldDesc *pFD);

    static Context* CreateNewContext(AppDomain *pDomain);

    static void FreeContext(Context* victim)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(victim));
        }
        CONTRACTL_END;

        delete victim;
    }

    static Context* GetExecutionContext(OBJECTREF pObj);
    static Context* SetupDefaultContext(AppDomain *pDomain);
    static void CleanupDefaultContext(AppDomain *pDomain);
    static void RequestCallBack(ADID appDomain, Context* targetCtxID, void* privateData);    

    static BOOL ValidateContext(Context *pCtx);  

    inline STATIC_DATA *GetSharedStaticData()
    {
        LEAF_CONTRACT;
        return m_pSharedStaticData;
    }
    
    inline void SetSharedStaticData(STATIC_DATA *pData)
    {
        LEAF_CONTRACT;
        m_pSharedStaticData = pData;
    }

    inline STATIC_DATA *GetUnsharedStaticData()
    {
        LEAF_CONTRACT;
        return m_pUnsharedStaticData;
    }
    
    inline void SetUnsharedStaticData(STATIC_DATA *pData)
    {
        LEAF_CONTRACT;
        m_pUnsharedStaticData = pData;
    }

    // Functions called from BCL on a managed context object
    static FCDECL2(void, SetupInternalContext, ContextBaseObject* pThisUNSAFE, CLR_BOOL bDefault);
    static FCDECL1(void, CleanupInternalContext, ContextBaseObject* pThisUNSAFE);
    static FCDECL1(void, ExecuteCallBack, LPVOID privateData);

private:
    // Static helper functions:
    static void InitializeFields();
    static BOOL InitContexts();

    inline static MethodDesc *MDofDoCallBackFromEE()
    {
        LEAF_CONTRACT;
        return s_pDoCallBackFromEE;
    }
    
    inline static MethodDesc *MDofReserveSlot()
    {
        LEAF_CONTRACT;
        return s_pReserveSlot;
    }
    
    inline static MethodDesc *MDofManagedThreadCurrentContext()
    {
        LEAF_CONTRACT;
        return s_pThread_CurrentContext;
    }

    static void ExecuteWaitCallback(WaitArgs* waitArgs);
    static void ExecuteMonitorWaitCallback(MonitorWaitArgs* waitArgs);
    static void ExecuteSignalAndWaitCallback(SignalAndWaitArgs* signalAndWaitArgs);
    void GetStaticFieldAddressSpecial(FieldDesc *pFD, MethodTable *pMT, int *pSlot, LPVOID *ppvAddress);
    LPVOID CalculateAddressForManagedStatic(int slot);

    // Static Data Members:

    static BOOL s_fInitializedContext;
    static MethodTable *s_pContextMT;
    static MethodDesc *s_pDoCallBackFromEE;
    static MethodDesc *s_pReserveSlot;
    static MethodDesc *s_pThread_CurrentContext;
    static CrstStatic s_ContextCrst;
    

    // Non-static Data Members:
    // Pointer to native context static data
    STATIC_DATA*        m_pUnsharedStaticData;
    
    // Pointer to native context static data
    STATIC_DATA*        m_pSharedStaticData;

    typedef SimpleList<OBJECTHANDLE> ObjectHandleList;

    ObjectHandleList    m_PinnedContextStatics;

    PTR_AppDomain       m_pDomain;

    OBJECTHANDLE        m_ExposedObjectHandle;

    DWORD               m_Signature;
    // NOTE: please maintain the signature as the last member field!!!
};

FCDECL0(LPVOID, GetPrivateContextsPerfCountersEx);

#endif
