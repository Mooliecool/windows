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

#ifndef CLR_TESTHOOK_H
#define  CLR_TESTHOOK_H

// {CCB32DA2-2544-4195-8552-AC1986A83C73}
static const GUID IID_ICLRTestHook = 
{ 0xccb32da2, 0x2544, 0x4195, { 0x85, 0x52, 0xac, 0x19, 0x86, 0xa8, 0x3c, 0x73 } };
static const GUID IID_ICLRTestHookManager = 
{ 0xccb32da2, 0x2544, 0x4195, { 0x85, 0x52, 0xac, 0x19, 0x86, 0xa8, 0x3c, 0x74 } };

enum
{
    ADUF_ASYNCHRONOUS,       //no wait
    ADUF_NORMAL,                    //wait, might be kicked out
    ADUF_FORCEFULLGC,           //same as normal, but does full gc
};

enum
{
    RTS_INITIALIZED,
    RTS_DEFAULTADREADY,
    RTS_CALLINGENTRYPOINT   
};


class ICLRTestHook : public IUnknown
{
public:
    STDMETHOD(AppDomainStageChanged)(DWORD adid,DWORD oldstage,DWORD newstage)=0;
    STDMETHOD(NextFileLoadLevel)(DWORD adid, LPVOID domainfile,DWORD newlevel)=0;
    STDMETHOD(CompletingFileLoadLevel)(DWORD adid, LPVOID domainfile,DWORD newlevel)=0;
    STDMETHOD(CompletedFileLoadLevel)(DWORD adid, LPVOID domainfile,DWORD newlevel)=0;
    STDMETHOD(EnteringAppDomain)(DWORD id)=0;
    STDMETHOD(EnteredAppDomain)(DWORD id)=0;
    STDMETHOD(LeavingAppDomain)(DWORD id)=0;
    STDMETHOD(LeftAppDomain)(DWORD id)=0;
    STDMETHOD(UnwindingThreads)(DWORD id) = 0;
    STDMETHOD(UnwoundThreads)(DWORD id) = 0;	
    STDMETHOD(AppDomainCanBeUnloaded)(DWORD id, BOOL bUnsafePoint)=0; 
    STDMETHOD(AppDomainDestroyed)(DWORD id)=0;
    STDMETHOD(RuntimeStarted)(DWORD code)=0;
};

class  ICLRTestHookManager 
{
public:
    STDMETHOD(AddTestHook)(ICLRTestHook* hook)=0;
    STDMETHOD(EnableSlowPath) (BOOL bEnable) =0;
    STDMETHOD(UnloadAppDomain)(DWORD adid,DWORD flags)=0;
    STDMETHOD_(VOID,DoApproriateWait)( int cObjs, HANDLE *pObjs, INT32 iTimeout, BOOL bWaitAll, int* res)=0;	
    STDMETHOD(GC)(int generation)=0;
    STDMETHOD(GetSimpleName)(LPVOID domainfile,LPCUTF8* name)=0;
    STDMETHOD_(INT_PTR,GetCurrentThreadType)(VOID)=0; //see clrhost.h
    STDMETHOD_(INT_PTR,GetCurrentThreadLockCount) (VOID) =0;
    STDMETHOD_(BOOL,IsPreemptiveGC)(VOID) =0;       	
    STDMETHOD_(BOOL,ThreadCanBeAborted) (VOID) = 0;
};

//sample implementation
class CLRTestHook : public ICLRTestHook
{
protected:
    volatile LONG m_cRef;
public:    
    CLRTestHook()
    {
        m_cRef=0;
    }
    STDMETHOD(AppDomainStageChanged)(DWORD adid,DWORD oldstage,DWORD newstage){ return S_OK;};
    STDMETHOD(NextFileLoadLevel)(DWORD adid, LPVOID domainfile,DWORD newlevel){ return S_OK;};
    STDMETHOD(CompletingFileLoadLevel)(DWORD adid, LPVOID domainfile,DWORD newlevel){ return S_OK;};
    STDMETHOD(CompletedFileLoadLevel)(DWORD adid, LPVOID domainfile,DWORD newlevel){ return S_OK;};
    STDMETHOD(EnteringAppDomain)(DWORD id){ return S_OK;};
    STDMETHOD(EnteredAppDomain)(DWORD id){ return S_OK;};
    STDMETHOD(LeavingAppDomain)(DWORD id){ return S_OK;};
    STDMETHOD(LeftAppDomain)(DWORD id){ return S_OK;};
    STDMETHOD(UnwindingThreads)(DWORD id) {return S_OK;};
    STDMETHOD(UnwoundThreads)(DWORD id) {return S_OK;};
    STDMETHOD(AppDomainCanBeUnloaded)(DWORD id, BOOL bUnsafePoint){ return S_OK;}; 
    STDMETHOD(AppDomainDestroyed)(DWORD id){ return S_OK;};
    STDMETHOD(RuntimeStarted)(DWORD code){ return S_OK;};
    STDMETHOD_(ULONG,AddRef)() 
    {
        return InterlockedIncrement(&m_cRef);
    };
    STDMETHOD_(ULONG,Release)()
    {
        ULONG  ulRef = InterlockedDecrement(&m_cRef);
        if (!ulRef) 
            delete this;
        return ulRef;
    };   
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv)
    {
        if (!ppv) 
            return E_POINTER;

        if (   IsEqualIID(riid, IID_IUnknown)
            || IsEqualIID(riid, IID_ICLRTestHook))
        {
            AddRef();
            *ppv = (ICLRTestHook*) (this);
            return S_OK;
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
    };   
};

typedef void CALLBACK CLRTESTHOOKPROC(ICLRTestHookManager*);

#endif
