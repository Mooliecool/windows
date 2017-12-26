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
// File: DIValue.cpp
//
//*****************************************************************************
#include "stdafx.h"
#include "primitives.h"

// Is this a signed type or unsigned type?
// Useful to known when we need to sign-extend.
bool IsSigned(CorElementType et)
{
    switch (et)
    {
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_I4:
        return true;

    default:
        return false;
    }
}

/* ------------------------------------------------------------------------- *
 * CordbValue class
 * ------------------------------------------------------------------------- */

CordbValue::CordbValue(
               CordbAppDomain *appdomain,
               CordbType *type,
               REMOTE_PTR remoteAddress,
               void *localAddress,
               RemoteAddress *remoteRegAddr,
               bool isLiteral,
               NeuterList * pList)
    : CordbBase(
            ((appdomain != NULL) ? (appdomain->GetProcess()) : (type->GetProcess())),
            (UINT_PTR)remoteAddress, enumCordbValue),
      m_appdomain(appdomain),
      m_type(type), // implicit InternalAddRef
      //m_sigCopied(false),
      m_size(0),
      m_localAddress(localAddress),
      m_isLiteral(isLiteral),
      m_pParent(NULL)
{
    if (remoteRegAddr != NULL)
    {
        _ASSERTE(remoteAddress == NULL);
        m_remoteRegAddr = *remoteRegAddr;
    }
    else
    {
        m_remoteRegAddr.kind = RAK_NONE;
    }

    _ASSERTE(GetProcess() != NULL);

    // Add to a neuter list. If none is provided, use the ExitProcess list as a default.
    // The main neuter lists of interest here are:
    // - CordbProcess::GetContinueNeuterList() - Shortest. Neuter when the process continues.
    // - CordbAppDomain::GetExitNeuterList() - Middle. Neuter when the AD exits. Since most Values (except globals) are in 
    //                                  a specific AD, this almost catches all; and keeps us safe in AD-unload scenarios.
    // - CordbProcess::GetExitNeuterList() - Worst. Doesn't neuter until the process exits (or we detach). 
    //                                  This could be a long time.
    if (pList == NULL)
    {
        pList = GetProcess()->GetExitNeuterList();
    }

    pList->Add(this);
}

CordbValue::~CordbValue()
{
    DTOR_ENTRY(this);

    _ASSERTE(this->IsNeutered());
    
    _ASSERTE(m_type == NULL);
    _ASSERTE(m_pParent == NULL);
}

void CordbValue::Neuter(NeuterTicket ticket)
{
    m_appdomain = NULL;
    m_type.Clear();

    if (m_pParent != NULL)
    {
        m_pParent->Release();
        m_pParent = NULL;
    }
    

    CordbBase::Neuter(ticket);    
}


//
// Init for the base value class.
HRESULT CordbValue::Init(void)
{
    return S_OK;
}



//
// Create the proper value object based on the given element type.
//
/*static*/ HRESULT CordbValue::CreateValueByType(CordbAppDomain *appdomain,
                                                 CordbType *type,
                                                 bool boxed,
                                                 REMOTE_PTR remoteAddress,
                                                 void *localAddress,
                                                 RemoteAddress *remoteRegAddr,
                                                 IUnknown *pParent,
                                                 ICorDebugValue** ppValue)
{
    INTERNAL_SYNC_API_ENTRY(appdomain->GetProcess()); //

    // We'd really hope that our callers give us a valid appdomain, but in case
    // they don't, we'll fail gracefully.
    if ((appdomain != NULL) && appdomain->IsNeutered())
    {
        STRESS_LOG1(LF_CORDB, LL_EVERYTHING, "CVBT using neutered AP, %p\n", appdomain);
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    LOG((LF_CORDB,LL_INFO100000,"CV::CreateValueByType\n"));

    *ppValue = NULL;

    switch(type->m_elementType)
    {
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
        {
            LOG((LF_CORDB,LL_INFO100000,"CV::CreateValueByType CreateGenericValue\n"));

            // A generic value
            // By using s RSSmartPtr we ensure that in both success and failure cases, 
            // this object is cleaned up properly (deleted or not depending on ref counts).  
            // Specifically, the object has probably been placed on a neuter list so we 
            // can't delete it (but this is a detail we shouldn't rely on)
            RSSmartPtr<CordbGenericValue> pGenValue( 
                new (nothrow) CordbGenericValue(appdomain, type, 
                                          remoteAddress, localAddress, remoteRegAddr) );

            if (pGenValue != NULL)
            {
                // we need to set the parent of a value before we initialize it
                // (since initializing it may require reading data from the left
                // side, which requires information about the parent
                // value/frame).
                pGenValue->SetParent(pParent);
                hr = pGenValue->Init();

                if (SUCCEEDED(hr))
                {
                    pGenValue->AddRef();
                    *ppValue = (ICorDebugValue*)(ICorDebugGenericValue*)pGenValue;
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }

            break;
        }

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_TYPEDBYREF:
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_FNPTR:
        {
            LOG((LF_CORDB,LL_INFO1000000,"CV::CreateValueByType Creating ReferenceValue\n"));
            bool isUnboxedVCObject = false;

            if (!boxed)
            {
                hr = type->IsValueType(&isUnboxedVCObject);

                if (FAILED(hr))
                    return hr;
            }

            if (!isUnboxedVCObject)
            {
                CordbReferenceValue * pRef = NULL;
                hr = CordbReferenceValue::Build(
                    appdomain, type,
                    remoteAddress, localAddress, LSPTR_OBJECTHANDLE::NullPtr(), remoteRegAddr, // Home
                    pParent,
                    &pRef);

                *ppValue = static_cast<ICorDebugValue*> (pRef);
                     
                break;
            }
            else
            {

                // A value class object.
                RSSmartPtr<CordbVCObjectValue> pVCValue(
                    new (nothrow) CordbVCObjectValue(appdomain, type,
                                                remoteAddress, localAddress, remoteRegAddr) );

                if (pVCValue != NULL)
                {
                    pVCValue->SetParent(pParent);
                    hr = pVCValue->Init();

                    if (SUCCEEDED(hr))
                    {
                        pVCValue->AddRef();
                        *ppValue = (ICorDebugValue*)(ICorDebugObjectValue*)pVCValue;
                    }
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }

                break;
            }
        }

    default:
        _ASSERTE(!"Bad value type!");
        hr = E_FAIL;
    }

    return hr;
}

HRESULT CordbValue::CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
{
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoint, ICorDebugValueBreakpoint **);

    return E_NOTIMPL;
}


//
// This will update a register in a given context, and in the
// regdisplay of a given frame.
//
HRESULT CordbValue::SetContextRegister(CONTEXT *c,
                                       CorDebugRegister reg,
                                       SIZE_T newVal,
                                       CordbNativeFrame *frame)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    HRESULT hr = S_OK;
    LPVOID rdRegAddr;


#define _UpdateFrame() \
    if (frame != NULL) \
    { \
        rdRegAddr = frame->GetAddressOfRegister(reg); \
        *(SIZE_T *)rdRegAddr = newVal; \
    }

    switch(reg)
    {
    case REGISTER_INSTRUCTION_POINTER:
        CORDbgSetIP(c, (LPVOID)newVal);
        break;

    case REGISTER_STACK_POINTER:
        CORDbgSetSP(c, (LPVOID)newVal);
        break;

    case REGISTER_FRAME_POINTER:
        CORDbgSetFP(c, (LPVOID)newVal);
        _UpdateFrame();
        break;

#if defined(_X86_)
    case REGISTER_X86_EAX:
        c->Eax = newVal;
        _UpdateFrame();
        break;

    case REGISTER_X86_ECX:
        c->Ecx = newVal;
        _UpdateFrame();
        break;

    case REGISTER_X86_EDX:
        c->Edx = newVal;
        _UpdateFrame();
        break;

    case REGISTER_X86_EBX:
        c->Ebx = newVal;
        _UpdateFrame();
        break;

    case REGISTER_X86_ESI:
        c->Esi = newVal;
        _UpdateFrame();
        break;

    case REGISTER_X86_EDI:
        c->Edi = newVal;
        _UpdateFrame();
         break;

#endif

    default:
        _ASSERTE(!"Invalid register number!");
        hr = E_FAIL;
    }

    return hr;
}

HRESULT CordbValue::SetEnregisteredValue(void *pFrom)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    HRESULT hr = S_OK;

    // Get the thread's context so we can update it.
    CONTEXT *cTemp;
    CordbNativeFrame *frame = (CordbNativeFrame*)m_remoteRegAddr.frame;

    // Can't set an enregistered value unless the frame the value was
    // from is also the current leaf frame. This is because we don't
    // track where we get the registers from every frame from.

    if (! frame->IsLeafFrame())
        return CORDBG_E_SET_VALUE_NOT_ALLOWED_ON_NONLEAF_FRAME;

    if (FAILED(hr = frame->m_thread->GetManagedContext(&cTemp)))
    {
        // If we failed to get the context, then we must not be in a leaf frame.
        hr = CORDBG_E_SET_VALUE_NOT_ALLOWED_ON_NONLEAF_FRAME;
        goto Exit;
    }

    // Its important to copy this context that we're given a ptr to.
    CONTEXT c;
    c = *cTemp;

    // Update the context based on what kind of enregistration we have.
    switch (m_remoteRegAddr.kind)
    {
    case RAK_REG:
        {
            // If it's in a reg, then it's going to be a register's width (regardless of
            // the actual width of the data).
            // For signed types, like i2, i1, make sure we sign extend.
            bool fIsSigned = IsSigned(m_type->m_elementType);

            SIZE_T newVal = 0;

            if (fIsSigned)
            {
                // Sign extend. SSIZE_T is a register size signed value.
                // Casting
                switch(m_size)
                {
                    case 1:  _ASSERTE(sizeof( BYTE) == 1); newVal = (SSIZE_T) *(char*)pFrom; break;
                    case 2:  _ASSERTE(sizeof( WORD) == 2); newVal = (SSIZE_T) *(short*)pFrom; break;
                    case 4:  _ASSERTE(sizeof(DWORD) == 4); newVal = (SSIZE_T) *(int*)pFrom; break;
                    default: _ASSERTE(!"bad m_size");
                }
            }
            else
            {
                // Zero extend.
                switch(m_size)
                {
                    case 1:  _ASSERTE(sizeof( BYTE) == 1); newVal = *( BYTE*)pFrom; break;
                    case 2:  _ASSERTE(sizeof( WORD) == 2); newVal = *( WORD*)pFrom; break;
                    case 4:  _ASSERTE(sizeof(DWORD) == 4); newVal = *(DWORD*)pFrom; break;
                    default: _ASSERTE(!"bad m_size");
                }
            }

            hr = SetContextRegister(&c, m_remoteRegAddr.reg1, newVal, frame);
        }
        break;

    case RAK_REGREG:
        {
            _ASSERTE(m_size == 8);
            _ASSERTE(sizeof(SIZE_T) == sizeof(void*));

            // Split the new value into high and low parts.
            SIZE_T highPart;
            SIZE_T lowPart;

            memcpy(&lowPart, pFrom, sizeof(SIZE_T));
            memcpy(&highPart, (void*)((BYTE*)pFrom + sizeof(SIZE_T)), sizeof(SIZE_T));

            // Update the proper registers.
            hr = SetContextRegister(&c, m_remoteRegAddr.reg1, highPart, frame);

            if (SUCCEEDED(hr))
                hr = SetContextRegister(&c, m_remoteRegAddr.u.reg2, lowPart, frame);
        }
        break;

    case RAK_REGMEM:
        {
            _ASSERTE(m_size == 8);
            _ASSERTE(sizeof(SIZE_T) == sizeof(void*));

            // Split the new value into high and low parts.
            SIZE_T highPart;
            SIZE_T lowPart;

            memcpy(&lowPart, pFrom, sizeof(SIZE_T));
            memcpy(&highPart, (void*)((BYTE*)pFrom + sizeof(SIZE_T)), sizeof(SIZE_T));

            // Update the proper registers.
            hr = SetContextRegister(&c, m_remoteRegAddr.reg1, highPart, frame);

            if (SUCCEEDED(hr))
            {
                BOOL succ = WriteProcessMemory(
                                frame->GetProcess()->m_handle,
                                CORDB_ADDRESS_TO_PTR(m_remoteRegAddr.addr),
                                &lowPart,
                                sizeof(SIZE_T),
                                NULL);

                if (!succ)
                    hr = HRESULT_FROM_GetLastError();
            }
        }
        break;

    case RAK_MEMREG:
        {
            _ASSERTE(m_size == 8);
            _ASSERTE(sizeof(SIZE_T) == sizeof(void*));

            // Split the new value into high and low parts.
            SIZE_T highPart;
            SIZE_T lowPart;

            memcpy(&lowPart, pFrom, sizeof(SIZE_T));
            memcpy(&highPart, (void*)((BYTE*)pFrom + sizeof(SIZE_T)), sizeof(SIZE_T));

            // Update the proper registers.
            hr = SetContextRegister(&c, m_remoteRegAddr.reg1, lowPart, frame);

            if (SUCCEEDED(hr))
            {
                BOOL succ = WriteProcessMemory(
                                frame->GetProcess()->m_handle,
                                CORDB_ADDRESS_TO_PTR(m_remoteRegAddr.addr),
                                &highPart,
                                sizeof(SIZE_T),
                                NULL);

                if (!succ)
                    hr = HRESULT_FROM_GetLastError();
            }
        }
        break;

    case RAK_FLOAT:
        {
            _ASSERTE((m_size == 4) || (m_size == 8));

            // Convert the input to a double.
            double newVal = 0.0;

            memcpy(&newVal, pFrom, m_size);

            _ASSERTE(!"RAK_FLOAT not supported on Rotor\n");
            hr = E_NOTIMPL;
        }

        break;

    default:
        _ASSERTE(!"Yikes -- invalid RemoteAddressKind");
    }

    if (FAILED(hr))
        goto Exit;

    // Set the thread's modified context.
    if (FAILED(hr = frame->m_thread->SetManagedContext(&c)))
        goto Exit;

    // If all has gone well, update whatever local address points to.
    if (m_localAddress)
        memcpy(m_localAddress, pFrom, m_size);

Exit:
    return hr;
}

void CordbValue::GetRegisterInfo(DebuggerIPCE_FuncEvalArgData *pFEAD)
{
    // Copy the register info into the FuncEvalArgData.
    pFEAD->argHome = m_remoteRegAddr;
}

HRESULT CordbValue::GetExactType(ICorDebugType **ppType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppType, ICorDebugType **);
    FAIL_IF_NEUTERED(this);    

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    *ppType = static_cast<ICorDebugType*> (m_type);

    if (*ppType != NULL)
        (*ppType)->AddRef();

    return S_OK;
}

HRESULT CordbValue::InternalCreateHandle(
    CorDebugHandleType handleType,
    ICorDebugHandleValue ** ppHandle)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess());
    LOG((LF_CORDB,LL_INFO1000,"CV::CreateHandle\n"));

    DebuggerIPCEvent      event;
    CordbProcess              *process;
    BOOL                          fStrong = FALSE;

    if (ppHandle == NULL)
    {
        return E_INVALIDARG;
    }

    *ppHandle = NULL;        

    if (handleType == HANDLE_STRONG)
    {
        fStrong = TRUE;
    }
#if _DEBUG
    else
    {
        _ASSERTE(handleType == HANDLE_WEAK_TRACK_RESURRECTION);
    }
#endif // _DEBUG

    // Create the ICorDebugHandleValue object
    RSSmartPtr<CordbHandleValue> chv( 
        new (nothrow) CordbHandleValue(m_appdomain, m_type, handleType) );

    if (chv == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Send the event to create the handle.
    process = m_appdomain->GetProcess();
    _ASSERTE(process != NULL);

    process->InitIPCEvent(&event,
                          DB_IPCE_CREATE_HANDLE,
                          true,
                          m_appdomain->GetADToken());

    event.CreateHandle.objectToken = (void *)m_id;
    event.CreateHandle.fStrong = fStrong;

    // Note: two-way event here...
    HRESULT hr = process->m_cordb->SendIPCEvent(process, &event,
                                        sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);

    if (SUCCEEDED(hr))
    {
        _ASSERTE(event.type == DB_IPCE_CREATE_HANDLE_RESULT);

        // Initialize the handle value object.
        hr = chv->Init(event.CreateHandleResult.objectHandle);
    }

    if (!SUCCEEDED(hr))
    {
        // Neuter the new object we've been working on. This will call Dispose(), and that will go back to the left side and free
        // the handle that we got above.
        NeuterTicket ticket(GetProcess());
        chv->Neuter(ticket);

        // The handle value may be on a neuter list, so don't delete manually.
        return hr;
    }

    // Pass out the new handle value object.
    *ppHandle = static_cast<ICorDebugHandleValue*>(chv);
    (*ppHandle)->AddRef();

    return S_OK;
}   // CreateHandle

//
// CordbValueHome class
//

CordbValueHome::CordbValueHome(CordbValue* value) :
    m_value(value)
{
}

HRESULT
CordbValueHome::CreateValueHome(CordbValue* value,
                                CordbValueHome** newValueHome)
{
    INTERNAL_SYNC_API_ENTRY(value->GetProcess()); //

    // we're the only ones creating value homes, so we can safley assert that we
    // always want to track the creation of a vlaue home
    _ASSERTE(NULL != newValueHome);
    CordbValueHome* temp = new (nothrow) CordbValueHome(value);
    if (NULL == temp)
    {
        return E_OUTOFMEMORY;
    }

    *newValueHome = temp;
    return S_OK;
}


//
//
HRESULT
CordbValueHome::Read(BYTE* buffer, SIZE_T size)
{
    return m_value->GetEnregisteredValue(buffer, size);
}

HRESULT
CordbValueHome::Write(BYTE* buffer, SIZE_T size)
{
    INTERNAL_SYNC_API_ENTRY(m_value->GetProcess()); //
    return m_value->SetEnregisteredValue(buffer);
}



/* ------------------------------------------------------------------------- *
 * Generic Value class
 * ------------------------------------------------------------------------- */

//
// CordbGenericValue constructor that builds a generic value from
// local and remote addresses. This one is just when a single address
// is enough to specify the location of a value.
//
CordbGenericValue::CordbGenericValue(CordbAppDomain *appdomain,
                                     CordbType *type,
                                     REMOTE_PTR remoteAddress,
                                     void *localAddress,
                                     RemoteAddress *remoteRegAddr)
    : CordbValue(appdomain, type, remoteAddress, localAddress, remoteRegAddr, false)
{
    _ASSERTE(type->m_elementType != ELEMENT_TYPE_END &&
             type->m_elementType != ELEMENT_TYPE_VOID &&
             type->m_elementType < ELEMENT_TYPE_MAX);

    // We can fill in the size now for generic values.
    HRESULT hr;
    hr = type->GetUnboxedObjectSize(&m_size);
    _ASSERTE (!FAILED(hr));

}

//
// CordbGenericValue constructor that builds a generic value from two
// halves of data. This is valid only for 64-bit values.
//
CordbGenericValue::CordbGenericValue(CordbAppDomain *appdomain,
                                     CordbType *type,
                                     DWORD highWord,
                                     DWORD lowWord,
                                     RemoteAddress *remoteRegAddr)
    : CordbValue(appdomain, type, NULL, NULL, remoteRegAddr, false)
{
    _ASSERTE((type->m_elementType == ELEMENT_TYPE_I8) ||
             (type->m_elementType == ELEMENT_TYPE_U8) ||
             (type->m_elementType == ELEMENT_TYPE_R8));

    // We know the size is always 64-bits for these types of values.
    // We can also go ahead and initialize the value right here, making
    // the call to Init() for this object superflous.
    m_size = 8;

    *((DWORD*)(&m_copyOfData[0])) = lowWord;
    *((DWORD*)(&m_copyOfData[4])) = highWord;
}

//
// CordbGenericValue constructor that builds an empty generic value
// from just an element type. Used for literal values for func evals
// only.
//
CordbGenericValue::CordbGenericValue(CordbType *type)
    : CordbValue(NULL, type, NULL, NULL, NULL, true)
{
    // The only purpose of a literal value is to hold a RS literal value.
    HRESULT hr;
    hr = type->GetUnboxedObjectSize(&m_size);
    _ASSERTE (!FAILED(hr));
    memset(m_copyOfData, 0, sizeof(m_copyOfData));
}

HRESULT CordbGenericValue::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugValue)
    {
        *pInterface = static_cast<ICorDebugValue*>(static_cast<ICorDebugGenericValue*>(this));
    }
    else if (id == IID_ICorDebugValue2)
    {
        *pInterface = static_cast<ICorDebugValue2*>(this);
    }
    else if (id == IID_ICorDebugGenericValue)
    {
        *pInterface = static_cast<ICorDebugGenericValue*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugGenericValue*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

//
// initialize a generic value by copying the necessary data, either
// from the remote process or from another value in this process.
//
HRESULT CordbGenericValue::Init(void)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    HRESULT hr = CordbValue::Init();

    if (FAILED(hr))
    {
        return hr;
    }

    // If neither m_localAddress nor m_id are set, then all that means
    // is that we've got a pre-initialized 64-bit value.
    if (m_localAddress != NULL)
    {
        // Copy the data out of the local address space.
        //
        memcpy(&m_copyOfData[0], m_localAddress, m_size);
        return S_OK;
    }
    else if (m_id != NULL)
    {
        // Copy the data out of the remote process.
        BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                      (const void*) m_id,
                                      &m_copyOfData[0],
                                      m_size,
                                      NULL);

        if (!succ)
            return HRESULT_FROM_GetLastError();
    }
    else
    {
        _ASSERTE(RAK_NONE != m_remoteRegAddr.kind || m_isLiteral);
        if (RAK_NONE != m_remoteRegAddr.kind)
        {
            HRESULT localHR =  CordbValueHome::CreateValueHome(this,
                                                               &m_valueHome);
            if (FAILED(localHR))
            {
                return localHR;
            }
            return m_valueHome->Read(m_copyOfData, m_size);
        }
        else
        {
            // if this value doesn't have any sort of home (a local address, a
            // remote address, or a remote register), then it must be a literal.
            _ASSERTE(m_isLiteral);
        }
    }

    return S_OK;
}

HRESULT CordbGenericValue::GetValue(void *pTo)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pTo, BYTE, m_size, false, true);

    // Copy out the value
    memcpy(pTo, &m_copyOfData[0], m_size);

    return S_OK;
}

HRESULT CordbGenericValue::SetValue(void *pFrom)
{
    HRESULT hr = S_OK;
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pFrom, BYTE, m_size, true, false);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    // We only need to send to the left side to update values that are
    // object references. For generic values, we can simply do a write
    // memory.

    // We had better have a remote address.
    _ASSERTE((m_id != NULL) || (m_remoteRegAddr.kind != RAK_NONE) ||
             m_isLiteral);

    // Write the new value into the remote process if we have a remote
    // address. Otherwise, update the thread's context.
    if (m_id != NULL)
    {
        BOOL succ = WriteProcessMemory(GetProcess()->m_handle,
                                       (void*)m_id,
                                       pFrom,
                                       m_size,
                                       NULL);

        if (!succ)
            hr = HRESULT_FROM_GetLastError();
    }
    else if (!m_isLiteral)
    {
        hr = SetEnregisteredValue(pFrom);
    }

    // That worked, so update the copy of the value we have in
    // m_copyOfData.
    if (SUCCEEDED(hr))
    {
        memcpy(&m_copyOfData[0], pFrom, m_size);
    }

    return hr;
}

bool CordbGenericValue::CopyLiteralData(BYTE *pBuffer)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    // If this is a RS fabrication, copy the literal data into the
    // given buffer and return true.
    if (m_isLiteral)
    {
        memcpy(pBuffer, m_copyOfData, sizeof(m_copyOfData));
        return true;
    }
    else
        return false;
}

/* ------------------------------------------------------------------------- *
 * Reference Value class
 * ------------------------------------------------------------------------- */

CordbReferenceValue::CordbReferenceValue(CordbAppDomain *appdomain,
                                         CordbType *type,
                                         REMOTE_PTR remoteAddress,
                                         void *localAddress,
                                         LSPTR_OBJECTHANDLE objectHandle,
                                         RemoteAddress *remoteRegAddr)
    : CordbValue(appdomain, type, remoteAddress, localAddress, remoteRegAddr, false, 
            appdomain->GetSweepableExitNeuterList()),
            
      m_objectHandle(objectHandle),
      m_realTypeOfTypedByref(NULL)
{
    memset(&m_info, 0, sizeof(m_info));

    if (!objectHandle.IsNull())
    {
        // Case where the reference is backed by an object handle
        _ASSERTE(remoteAddress != NULL); // caller supplies remoteAddress, to work w/ Func-eval.
        _ASSERTE(remoteRegAddr == NULL);
        _ASSERTE(localAddress == NULL);
    }
    else
    {
        // No object handle backing us. We're either in memory or a register.
        // If we're in a register, that's been copied over as a "localAddress"
        if (remoteRegAddr != NULL)
        {
            _ASSERTE(localAddress != NULL);
        }
        _ASSERTE((remoteAddress != NULL) || (localAddress != NULL));
    }

    LOG((LF_CORDB,LL_EVERYTHING,"CRV::CRV: this:0x%x\n",this));
    m_size = sizeof(void*);
}

CordbReferenceValue::CordbReferenceValue(CordbType *type)
    : CordbValue(NULL, type, NULL, NULL, NULL, true, type->GetAppDomain()->GetSweepableExitNeuterList())
{
    memset(&m_info, 0, sizeof(m_info));
    m_objectHandle.Set(NULL);
    
    // The only purpose of a literal value is to hold a RS literal value.
    m_size = sizeof(void*);
}

bool CordbReferenceValue::CopyLiteralData(BYTE *pBuffer)
{
    // If this is a RS fabrication, then its a null reference.
    if (m_isLiteral)
    {
        void *n = NULL;
        memcpy(pBuffer, &n, sizeof(n));
        return true;
    }
    else
        return false;
}

CordbReferenceValue::~CordbReferenceValue()
{
    DTOR_ENTRY(this);

    LOG((LF_CORDB,LL_EVERYTHING,"CRV::~CRV: this:0x%x\n",this));

    _ASSERTE(IsNeutered());
    _ASSERTE(m_objectWeak == NULL);
}

void CordbReferenceValue::Neuter(NeuterTicket ticket)
{
    m_objectWeak.Clear();
    m_realTypeOfTypedByref = NULL;
    CordbValue::Neuter(ticket);
}


HRESULT CordbReferenceValue::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugValue)
    {
        *pInterface = static_cast<ICorDebugValue*>(static_cast<ICorDebugReferenceValue*>(this));
    }
    else if (id == IID_ICorDebugValue2)
    {
        *pInterface = static_cast<ICorDebugValue2*>(this);
    }
    else if (id == IID_ICorDebugReferenceValue)
    {
        *pInterface = static_cast<ICorDebugReferenceValue*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugReferenceValue*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbReferenceValue::GetType(CorElementType *pType)
{
    LEAF_CONTRACT;

    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pType, CorElementType *);

    if( m_type == NULL )
    {
        // We may not have a CordbType if we were created from a GC handle to NULL
        _ASSERTE( m_info.objTypeData.elementType == ELEMENT_TYPE_CLASS );
        _ASSERTE( !m_objectHandle.IsNull() );
        _ASSERTE( m_info.objRef == NULL );
        *pType = m_info.objTypeData.elementType;
    }
    else
    {
        // The element type stored in both places should match
        _ASSERTE( m_info.objTypeData.elementType == m_type->m_elementType );
        *pType = m_type->m_elementType;
    }

    return S_OK;
}

HRESULT CordbReferenceValue::GetAddress(CORDB_ADDRESS *pAddress)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    
    return (CordbValue::GetAddress(pAddress));
}
    
HRESULT CordbReferenceValue::IsNull(BOOL *pbNULL)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pbNULL, BOOL *);

   if (m_isLiteral || (m_info.objRef == NULL))
        *pbNULL = TRUE;
    else
        *pbNULL = FALSE;

    return S_OK;
}

HRESULT CordbReferenceValue::GetValue(CORDB_ADDRESS *pTo)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pTo, CORDB_ADDRESS *);
    FAIL_IF_NEUTERED(this);

    // Copy out the value, which is simply the value the object reference.
    if (m_isLiteral)
        *pTo = NULL;
    else
        *pTo = PTR_TO_CORDB_ADDRESS(m_info.objRef);

    return S_OK;
}

HRESULT CordbReferenceValue::SetValue(CORDB_ADDRESS pFrom)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    HRESULT hr = S_OK;

    // If this is a heap object, ideally we'd prevent violations of AppDomain isolation 
    // here.  However, we have no reliable way of determining what AppDomain pFrom is in.

    // Can't change literal refs.
    if (m_isLiteral)
    {
        return E_INVALIDARG;
    }        

    // We had better have a remote address.
    _ASSERTE((m_id != NULL) || (m_remoteRegAddr.kind != RAK_NONE) || !m_objectHandle.IsNull());

    // Either we know the type, or it's a handle to a null value
    _ASSERTE( (m_type != NULL) || ( (!m_objectHandle.IsNull()) && (m_info.objRef == NULL) ) );
    
    // If not enregistered, send a Set Reference message to the right
    // side with the address of this reference and whether or not the
    // reference points to a handle.
    if ((m_id != NULL) || !m_objectHandle.IsNull())
    {

        // If it's a reference but not a GC root then just we can just treat it like raw data (like a DWORD).
        // This would include things like "int*", and E_T_FNPTR.
        // If it is a GC root, then we need to go over to the LS to update the WriterBarrier.
        if ( (m_type != NULL) && !m_type->IsGCRoot() )
        {
            // Non-GCroots shouldn't be in object handles.
            _ASSERTE(m_objectHandle.IsNull());
        
            void * pData = CORDB_ADDRESS_TO_PTR(pFrom);

            SIZE_T cbWritten = 0;
            BOOL fOk = WriteProcessMemory(
                GetProcess()->m_handle,
                (void*)  m_id, // remote address to write at
                &pData, sizeof(pData), // data to write
                &cbWritten
            );

            hr = (fOk) ? S_OK : HRESULT_FROM_GetLastError();
        }
        else
        {
            DebuggerIPCEvent event;

            GetProcess()->InitIPCEvent(&event, DB_IPCE_SET_REFERENCE, true, m_appdomain->GetADToken());

            event.SetReference.objectRefAddress = (void*)m_id;
            event.SetReference.objectHandle = m_objectHandle;
            _ASSERTE(m_size == sizeof(void*));
            event.SetReference.newReference = CORDB_ADDRESS_TO_PTR(pFrom);

            // Note: two-way event here...
            hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event,
                                                  sizeof(DebuggerIPCEvent));

            // Stop now if we can't even send the event.
            if (!SUCCEEDED(hr))
                return hr;

            _ASSERTE(event.type == DB_IPCE_SET_REFERENCE_RESULT);

            hr = event.hr;
        }
    }
    else
    {
        // The object reference is enregistered, so we don't have to
        // go through the write barrier. Simply update the proper
        // register.

        // Coerce the CORDB_ADDRESS to a DWORD, which is what we're
        // using for register values these days, and pass in that.
        DWORD newValue = (DWORD)pFrom;
        hr = SetEnregisteredValue((void*)&newValue);
    }

    if (SUCCEEDED(hr))
    {
        // That worked, so update the copy of the value we have in
        // m_copyOfData.
        m_info.objRef = CORDB_ADDRESS_TO_PTR(pFrom);

        // Now, dump any cache of object values hanging off of this
        // reference.
        m_objectWeak.Clear();

        if (m_info.objTypeData.elementType == ELEMENT_TYPE_STRING)
        {
            InitRef();
        } 

        // All other data in m_info is no longer valid, and we may have invalidated other
        // ICDRVs at this address.  We have to invalidate all cached debugee data.
        m_appdomain->GetProcess()->m_continueCounter++;
    }

    return hr;
}

HRESULT CordbReferenceValue::DereferenceStrong(ICorDebugValue **ppValue)
{
    return E_NOTIMPL;
}

HRESULT CordbReferenceValue::Dereference(ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    // Can't dereference literal refs.
    if (m_isLiteral)
        return E_INVALIDARG;

    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    if (m_continueCounterLastSync != m_appdomain->GetProcess()->m_continueCounter)
    {
        IfFailRet( InitRef() );
    }

    // We may know ahead of time (depending on the reference type) if
    // the reference is bad.
    if ((m_info.objRefBad) || (m_info.objRef == NULL))
    {
        return CORDBG_E_BAD_REFERENCE_VALUE;
    }

    RSSmartPtr<CordbObjectValue>* pHolder = m_objectWeak.GetAddr();
    
    return DereferenceCommon(m_appdomain, m_type, m_realTypeOfTypedByref, &m_info, 
        pHolder,
        ppValue);

}

//-----------------------------------------------------------------------------
// Common helper to dereferefence.
// Parameters:
//     pAppDomain, pType, pInfo - necessary paramters to create the value
//     pRealTypeOfTypedByref - type for a potential TypedByRef. Can be NULL if we know
//        that we're not a typed-byref (this is true if we're definitely an object handle)
//     ppCachedObject - optional parameter for cached object. If this is non-null
//        this will be used to cache / re-use the return value.
//     ppValue - outparameter for newly created value. This will get an Ext AddRef.
//-----------------------------------------------------------------------------
HRESULT CordbReferenceValue::DereferenceCommon(
    CordbAppDomain * pAppDomain,
    CordbType * pType, 
    CordbType * pRealTypeOfTypedByref,
    DebuggerIPCE_ObjectData * pInfo,
    RSSmartPtr<CordbObjectValue> * ppCachedObject,
    ICorDebugValue **ppValue
)
{
    INTERNAL_SYNC_API_ENTRY(pAppDomain->GetProcess());
    
    // pCachedObject may be NULL if we're not caching.
    _ASSERTE(pType != NULL);
    _ASSERTE(pAppDomain != NULL);
    _ASSERTE(pInfo != NULL);
    _ASSERTE(ppValue != NULL);

    HRESULT hr = S_OK;
    *ppValue = NULL; // just to be safe.
        
    switch(pType->m_elementType)
    {

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_STRING:
        {
            LOG((LF_CORDB, LL_INFO1000, "DereferenceInternal: type class/object/string\n"));
            // An object value (possibly a string value, too.) If the
            // class of this object is a value class, then we have a
            // reference to a boxed object. So we create a box instead
            // of an object value.
            bool isBoxedVCObject = false;

            if ((pType->m_class != NULL) && (pType->m_elementType != ELEMENT_TYPE_STRING))
            {
                hr = pType->m_class->IsValueClass(&isBoxedVCObject);

                if (FAILED(hr))
                {
                    return hr;
                }
            }

            if (isBoxedVCObject)
            {
                RSSmartPtr<CordbBoxValue> pBoxValue(
                    new (nothrow) CordbBoxValue(pAppDomain, pType,
                                                             pInfo->objRef, pInfo->objSize, pInfo->objOffsetToVars) );

                if (pBoxValue != NULL)
                {
                    hr = pBoxValue->Init();

                    if (SUCCEEDED(hr))
                    {
                        pBoxValue->ExternalAddRef();
                        *ppValue = (ICorDebugValue*)(ICorDebugBoxValue*)pBoxValue;
                    }
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }
            }
            else
            {
                RSSmartPtr<CordbObjectValue> pObj;
                // If we have a cached object, just return it.
                if (ppCachedObject != NULL)
                {
                    pObj.Assign( *ppCachedObject );
                    if (pObj != NULL)
                    {
                        pObj->ExternalAddRef();
                        *ppValue = static_cast<ICorDebugValue*>( static_cast<ICorDebugObjectValue*>(pObj) );
                        return S_OK;
                    }                    
                }

                // Note: we call Init() by default when we create (or refresh) a reference value, so we
                // never have to do it again.
                pObj.Assign( new (nothrow) CordbObjectValue(pAppDomain,
                                                    pType,
                                                    pInfo ) );
                if (pObj != NULL)
                {
                    hr = pObj->Init();

                    if (SUCCEEDED(hr))
                    {
                        if (ppCachedObject != NULL)
                        {
                            ppCachedObject->Assign(pObj);
                        }
                        pObj->ExternalAddRef();
                        *ppValue = static_cast<ICorDebugValue*>( static_cast<ICorDebugObjectValue*>(pObj) );
                        return S_OK;
                    }
                    else
                    {
                        return hr;
                    }
                }
                else
                {
                    return E_OUTOFMEMORY;
                }               
            } // boxed?

            break;
        }

    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
        {
            LOG((LF_CORDB, LL_INFO1000, "DereferenceInternal: type array/szarray\n"));
            RSSmartPtr<CordbArrayValue> pArrayValue(
                new (nothrow) CordbArrayValue(pAppDomain, pType, pInfo) );

            if (pArrayValue != NULL)
            {
                hr = pArrayValue->Init();

                if (SUCCEEDED(hr))
                {
                    pArrayValue->ExternalAddRef();
                    *ppValue = (ICorDebugValue*)(ICorDebugArrayValue*)pArrayValue;
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }

            break;
        }

    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_PTR:
        {
            //_ASSERTE(pInfo->objToken.IsNull()); // can't get this type w/ an object handle
                    
            LOG((LF_CORDB, LL_INFO1000, "DereferenceInternal: type byref/ptr\n"));
            CordbType *ptrType;
            pType->DestUnaryType(&ptrType);

            CorElementType et = ptrType->m_elementType;

            if (et == ELEMENT_TYPE_VOID)
            {
                *ppValue = NULL;
                return CORDBG_S_VALUE_POINTS_TO_VOID;
            }

            // Create a value for what this reference points to. Note:
            // this could be almost any type of value.
            hr = CordbValue::CreateValueByType(pAppDomain,
                                               ptrType,
                                               false,
                                               pInfo->objRef,
                                               NULL,
                                               NULL,
                                               NULL,
                                               ppValue);

            break;
        }

    case ELEMENT_TYPE_TYPEDBYREF:
        {
            //_ASSERTE(pInfo->objToken.IsNull()); // can't get this type w/ an object handle
            _ASSERTE(pRealTypeOfTypedByref != NULL);
            
            LOG((LF_CORDB, LL_INFO1000, "DereferenceInternal: type typedbyref\n"));
            // Create the value for what this reference points
            // to.
            hr = CordbValue::CreateValueByType(pAppDomain,
                                               pRealTypeOfTypedByref,
                                               false,
                                               pInfo->objRef,
                                               NULL,
                                               NULL,
                                               NULL,
                                               ppValue);

            break;
        }

    default:
        LOG((LF_CORDB, LL_INFO1000, "DereferenceInternal: Fail!\n"));
        _ASSERTE(!"Bad reference type!");
        hr = E_FAIL;
    }

    return hr;
}

// static helper to build a CordbReferenceValue from a general variable home.
// We can find the CordbType from the object instance.
HRESULT CordbReferenceValue::Build(
    CordbAppDomain *appdomain,
    CordbType * type, 
    REMOTE_PTR remoteAddress,
    void *localAddress,
    LSPTR_OBJECTHANDLE objectHandle,
    RemoteAddress *remoteRegAddr,
    IUnknown *pParent,
    CordbReferenceValue** ppValue)
{
    HRESULT hr;

    // We can find the AD from an object handle (but not a normal object), so the AppDomain may
    // be NULL if if it's an OH.
    //_ASSERTE((appdomain != NULL) || objectRefsInHandles);
    
    // A reference, possibly to an object or value class
    // Weak by default 
    RSSmartPtr<CordbReferenceValue> pRefValue( 
        new (nothrow) CordbReferenceValue(appdomain, type,
                                      remoteAddress, localAddress, objectHandle,
                                      remoteRegAddr) );

    if (pRefValue != NULL)
    {
        pRefValue->SetParent(pParent);
        hr = pRefValue->InitRef();

        if (SUCCEEDED(hr))
        {
            pRefValue->AddRef();
            *ppValue = pRefValue;
        }
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
    return hr;
}

//-----------------------------------------------------------------------------
// Static helper to build a CordbReferenceValue from a GCHandle
// The LS can actually determine an AppDomain from an OBJECTHandles, however, the RS
// should already have this infromation too, so we pass it in.
// We also supply the AppDomain here because it provides the CordbValue with 
// process affinity.
// Note that the GC handle may point to a NULL reference, in which case we should still create
// an appropriate ICorDebugReferenceValue for which IsNull returns TRUE.
//-----------------------------------------------------------------------------
HRESULT CordbReferenceValue::BuildFromGCHandle(
    CordbAppDomain *pAppDomain, 
    LSPTR_OBJECTHANDLE gcHandle, 
    ICorDebugReferenceValue ** pOutRef
)
{
    _ASSERTE(pAppDomain != NULL);
    _ASSERTE(pOutRef != NULL);

    CordbProcess * pProc;
    pProc = pAppDomain->GetProcess();
    INTERNAL_SYNC_API_ENTRY(pProc);    

    HRESULT hr = S_OK;

    *pOutRef = NULL;

    // Make sure we even have a GC handle. 
    // Also, We may have a handle, but its contents may be null.
    if (gcHandle.IsNull())
    {
        // We've seen this assert fire in the wild, but have never gotten a repro.
        // so we'll include a runtime check to avoid the AV.
        _ASSERTE(false || !"We got a bad reference value.");
        return CORDBG_E_BAD_REFERENCE_VALUE;
    }

    // Now that we've got an AppDomain, we can go ahead and create the reference value normally.

    CordbReferenceValue * pRefValue = NULL;

    hr = CordbReferenceValue::Build(
        pAppDomain, 
        NULL, // unknown type
        gcHandle.UnsafeGet(), // REMOTE_PTR remoteAddress,
        NULL, // void *localAddress,
        gcHandle, // objectRefsInHandles,
        NULL, // RemoteAddress *remoteRegAddr,        
        NULL, // parent
        &pRefValue);

    *pOutRef = static_cast<ICorDebugReferenceValue*> (pRefValue);

    return hr;
}

// Initialize this CordbReferenceValue. This will involve inspecting the LS
// to get our geometery and CordbType.
HRESULT CordbReferenceValue::InitRef()
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    HRESULT hr = S_OK;

    hr = CordbValue::Init();

    if (FAILED(hr))
        return hr;


    // Simple init needed for literal refs. Literals may have a null process / appdomain ptr.
    if (m_isLiteral)
    {
        _ASSERTE(m_type != NULL);
        m_info.objTypeData.elementType = m_type->m_elementType;
        return hr;
    }

    _ASSERTE(GetProcess()->GetSynchronized());

    // If the helper thread id dead, then pretend this is a bad reference.
    if (GetProcess()->m_helperThreadDead)
    {
        m_info.objRef = NULL;
        m_info.objRefBad = TRUE;
        return hr;
    }

    m_continueCounterLastSync = GetProcess()->m_continueCounter;

    // If we have a byref, ptr, or refany type then we go ahead and
    // get the true remote ptr now. All the other info we need to
    // dereference one of these is held in the base value class and in
    // the signature.

    // If no type provided, then it's b/c we're a class and we'll get the type when we get Created.
    CorElementType type = (m_type != NULL) ? (m_type->m_elementType) : ELEMENT_TYPE_CLASS;
    _ASSERTE (type != ELEMENT_TYPE_GENERICINST);
    _ASSERTE (type != ELEMENT_TYPE_VAR);
    _ASSERTE (type != ELEMENT_TYPE_MVAR);

    if ((type == ELEMENT_TYPE_BYREF) ||
        (type == ELEMENT_TYPE_PTR) ||
        (type == ELEMENT_TYPE_FNPTR))
    {
        // Fill in some m_info details since we will not be getting it from the LS below
        m_info.objRefBad = FALSE;
        m_info.objTypeData.elementType = type;

        if (m_id == NULL)
        {
            m_info.objRef = (void*)*((DWORD_PTR*)(m_localAddress));
        }
        else
        {
            BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                          (void*)m_id,
                                          &(m_info.objRef),
                                          sizeof(void*),
                                          NULL);

            if (!succ)
            {
                m_info.objRef = NULL;
                m_info.objRefBad = TRUE;
                return hr;
            }
        }

        // We should never dereference a funtion-pointers, so all references
        // are considered bad.
        if (type == ELEMENT_TYPE_FNPTR)
        {
            m_info.objRefBad = TRUE;
            return hr;
        }

        // The only way to tell if the reference in PTR's is bad or
        // not is to try to deref the darn thing.
        if (m_info.objRef != NULL)
        {
            if (type == ELEMENT_TYPE_PTR)
            {
                CordbType *ptrType;
                m_type->DestUnaryType(&ptrType);
                ULONG32 dataSize;
                hr = ptrType->GetUnboxedObjectSize(&dataSize);
                if (FAILED(hr))
                    return hr;
                if (dataSize == 0)
                    dataSize = 1; // Read at least one byte.
                else if (dataSize >= 8)
                    dataSize = 8; // Read at most eight bytes

                BYTE dummy[8];

                BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                               m_info.objRef,
                                               dummy,
                                               dataSize,
                                               NULL);

                if (!succ)
                    m_info.objRefBad = TRUE;
            }
        }
        else
        {
            // Null refs are considered "bad".
            m_info.objRefBad = TRUE;
        }

        return hr;
    }

    // We've got a remote address that points to the object reference.
    // We need to send to the left side to get real information about
    // the reference, including info about the object it points to.
    DebuggerIPCEvent event;

    GetProcess()->InitIPCEvent(&event,
                            DB_IPCE_GET_OBJECT_INFO,
                            true,
                            m_appdomain->GetADToken());

    // If we've got a NULL remote address, then all we have is a local address.
    // So we grab the object ref out of the local address and pass it
    // directly over to the left side instead of simply passing the remote
    // address of the object ref.

    event.GetObjectInfo.objectHandle = m_objectHandle;
    if (!m_objectHandle.IsNull())
    {
        // Case 1: we're a reference based off a GC handle.
        _ASSERTE(m_localAddress == NULL);
        event.GetObjectInfo.objectRefAddress = NULL; // ignored
        event.GetObjectInfo.objectRefIsValue = false; // ignored
    }
    else if (m_id == NULL)
    {
        // Local
        event.GetObjectInfo.objectRefAddress = *((void**)m_localAddress);
        event.GetObjectInfo.objectRefIsValue = true;
    }
    else
    {
        // Remote
        event.GetObjectInfo.objectRefAddress = (void*) m_id;
        event.GetObjectInfo.objectRefIsValue = false;
    }
 
    event.GetObjectInfo.objectType = (CorElementType)type;

    // Note: two-way event here...
    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event,
                                          sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);
    
    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        return hr;

    _ASSERTE(event.type == DB_IPCE_GET_OBJECT_INFO_RESULT);

    // Save the results for later.
    m_info = event.GetObjectInfoResult;

    if (m_info.objRefBad)
    {
        return S_OK;
    }


    // If the object type that we got back is different than the one
    // we sent, then it means that we orignally had a CLASS and now
    // have something more specific, like a SDARRAY, MDARRAY, or
    // STRING or a constructed type.
    // Update our signature accordingly, which is okay since
    // we always have a copy of our sig. This ensures that the
    // reference's signature accuratley reflects what the Runtime
    // knows its pointing to.
    //
    // GENERICS: do this for all types: for example, an array might have been
    // discovered to be a more specific kind of array (String[] where an Object[] was
    // expected).
    CordbType *newtype;

    IfFailRet( CordbType::TypeDataToType(m_appdomain, &m_info.objTypeData, &newtype) );

    _ASSERTE(newtype->m_elementType != ELEMENT_TYPE_VALUETYPE);
    m_type.Assign(newtype); // implicit Release + AddRef

    // For typed-byref's the act of dereferencing the object also reveals to us
    // what the "real" type of the object is...
    if (m_info.objTypeData.elementType == ELEMENT_TYPE_TYPEDBYREF)
    {
        IfFailRet( CordbType::TypeDataToType(m_appdomain, &m_info.typedByrefInfo.typedByrefType,
                                             &m_realTypeOfTypedByref) );
    }

    return hr;
}

/* ------------------------------------------------------------------------- *
 * Object Value class
 * ------------------------------------------------------------------------- */


#define COV_VALIDATE_OBJECT() do {         \
    BOOL bValid;                           \
    HRESULT hr;                            \
    if (FAILED(hr = IsValid(&bValid)))     \
        return hr;                         \
                                           \
        if (!bValid)                       \
        {                                  \
            return CORDBG_E_INVALID_OBJECT; \
        }                                  \
    }while(0)

CordbObjectValue::CordbObjectValue(CordbAppDomain *appdomain,
                                   CordbType *type,
                                   DebuggerIPCE_ObjectData *pObjectData )
    : CordbValue(appdomain, type, pObjectData->objRef, NULL, NULL, false, appdomain->GetProcess()->GetContinueNeuterList()),
      m_info(*pObjectData),
      m_objectCopy(NULL), m_objectLocalVars(NULL), m_stringBuffer(NULL)
{
    _ASSERTE(appdomain != NULL);

    m_size = (ULONG32)m_info.objSize;
}

CordbObjectValue::~CordbObjectValue()
{
    DTOR_ENTRY(this);

    _ASSERTE(IsNeutered());    
}

void CordbObjectValue::Neuter(NeuterTicket ticket)
{
    // Destroy the copy of the object.
    if (m_objectCopy != NULL)
    {
        delete [] m_objectCopy;
        m_objectCopy = NULL;
    }    

    CordbValue::Neuter(ticket);
}

HRESULT CordbObjectValue::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugValue)
    {
        *pInterface = static_cast<ICorDebugValue*>(static_cast<ICorDebugObjectValue*>(this));
    }
    else if (id == IID_ICorDebugValue2)
    {
        *pInterface = static_cast<ICorDebugValue2*>(this);
    }
    else if (id == IID_ICorDebugObjectValue)
    {
        *pInterface = static_cast<ICorDebugObjectValue*>(this);
    }
    else if (id == IID_ICorDebugObjectValue2)
    {
        *pInterface = static_cast<ICorDebugObjectValue2*>(this);
    }
    else if (id == IID_ICorDebugGenericValue)
    {
        *pInterface = static_cast<ICorDebugGenericValue*>(this);
    }
    else if (id == IID_ICorDebugHeapValue)
    {
        *pInterface = static_cast<ICorDebugHeapValue*>(this);
    }
    else if (id == IID_ICorDebugHeapValue2)
    {
        *pInterface = static_cast<ICorDebugHeapValue2*>(this);
    }
    else if ((id == IID_ICorDebugStringValue) &&
             (m_info.objTypeData.elementType == ELEMENT_TYPE_STRING))
    {
        *pInterface = static_cast<ICorDebugStringValue*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugObjectValue*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbObjectValue::GetType(CorElementType *pType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return (CordbValue::GetType(pType));
}

HRESULT CordbObjectValue::GetSize(ULONG32 *pSize)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return (CordbValue::GetSize(pSize));
}

HRESULT CordbObjectValue::GetAddress(CORDB_ADDRESS *pAddress)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    COV_VALIDATE_OBJECT();    

    return (CordbValue::GetAddress(pAddress));
}

HRESULT CordbObjectValue::CreateBreakpoint(ICorDebugValueBreakpoint
    **ppBreakpoint)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);    
    COV_VALIDATE_OBJECT();

    return (CordbValue::CreateBreakpoint(ppBreakpoint));
}

HRESULT CordbObjectValue::IsValid(BOOL *pbValid)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pbValid, BOOL *);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // We're neutered on continue, so we're valid up until the time we're neutered
    (*pbValid) = TRUE;
    return S_OK;
}

HRESULT CordbObjectValue::CreateRelocBreakpoint(
                                      ICorDebugValueBreakpoint **ppBreakpoint)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoint, ICorDebugValueBreakpoint **);

   COV_VALIDATE_OBJECT();

   return E_NOTIMPL;
}

/*
* Creates a handle of the given type for this heap value.
*
* Not Implemented In-Proc.
*/
HRESULT CordbObjectValue::CreateHandle(
    CorDebugHandleType handleType,
    ICorDebugHandleValue ** ppHandle)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    
    return CordbValue::InternalCreateHandle(handleType, ppHandle);
}   // CreateHandle

HRESULT CordbObjectValue::GetClass(ICorDebugClass **ppClass)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppClass, ICorDebugClass **);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    
    HRESULT hr = S_OK;
    if (m_type->m_class == NULL)
    {
        if (FAILED(hr = m_type->Init(FALSE)))
            return hr;
    }

    _ASSERTE(m_type->m_class);
    *ppClass = (ICorDebugClass*) m_type->m_class;

    if (*ppClass != NULL)
        (*ppClass)->AddRef();

    return hr;
}

HRESULT CordbObjectValue::GetFieldValueForType(ICorDebugType *pType,
                                        mdFieldDef fieldDef,
                                        ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pType, ICorDebugType *);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);

    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    COV_VALIDATE_OBJECT();
    
    CordbType *typ;
    HRESULT hr = S_OK;
    BOOL fSyncBlockField = FALSE;
    SIZE_T fldOffset;
    void* localAddr;
    
    //
    //
    if (pType == NULL)
        typ = m_type;
    else
        typ = static_cast<CordbType*> (pType);

    // Validate the token.
    if (typ->m_class == NULL || !typ->m_class->GetModule()->m_pIMImport->IsValidToken(fieldDef))
    {
        hr = E_INVALIDARG;
        goto LExit;
    }

    DebuggerIPCE_FieldData *pFieldData;

#ifdef _DEBUG
    pFieldData = NULL;
#endif

    hr = typ->GetFieldInfo(fieldDef, &pFieldData);

    if (hr == CORDBG_E_ENC_HANGING_FIELD)
    {
        hr = typ->m_class->GetSyncBlockField(fieldDef,
                                        &pFieldData,
                                        this);

        if (SUCCEEDED(hr))
            fSyncBlockField = TRUE;
    }

    if (SUCCEEDED(hr))
    {
        _ASSERTE(pFieldData != NULL);
        if( pFieldData->fldIsStatic )
        {
            hr = CORDBG_E_FIELD_NOT_INSTANCE;      
            goto LExit;
        }

        // Compute the remote address, too, so that SetValue will work.
        // Note that if pFieldData is a syncBlock field, fldOffset will have been cooked
        // to produce the correct result here.
        fldOffset = pFieldData->GetInstanceOffset();
        BYTE *ra = (BYTE *)m_id + m_info.objOffsetToVars + fldOffset;

        CordbModule *pModule = typ->m_class->GetModule();
        
        SigParser sigParser;
        IfFailRet( pFieldData->GetFieldSignature(pModule, &sigParser) );
        
        CordbType *fldtyp;
        IfFailRet( CordbType::SigToType(pModule, &sigParser, typ->m_inst, &fldtyp) );

        // Compute the local field address
        // Don't claim we have a local addr if the fldOffset has been cooked to point
        // us to a sync block field.
        localAddr = NULL;
        if( !fSyncBlockField )
        {
            localAddr = &(m_objectLocalVars[fldOffset]);
        }
        
        hr = CordbValue::CreateValueByType(m_appdomain,
                                           fldtyp,
                                           false,
                                           (void*)ra,
                                           localAddr,
                                           NULL,
                                           NULL,
                                           ppValue);
    }

    // If we can't get it b/c it's a constant, then say so.
    hr = CordbClass::PostProcessUnavailableHRESULT(hr, typ->m_class->GetModule()->m_pIMImport, fieldDef);

LExit:
    return hr;
}

HRESULT CordbObjectValue::GetFieldValue(ICorDebugClass *pClass,
                                        mdFieldDef fieldDef,
                                        ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(pClass, ICorDebugClass *);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);

    COV_VALIDATE_OBJECT();

    HRESULT hr;
    _ASSERTE(m_type);

    if (m_type->m_elementType != ELEMENT_TYPE_CLASS &&
        m_type->m_elementType != ELEMENT_TYPE_VALUETYPE)
    {
        return E_INVALIDARG;
    }

    RSExtSmartPtr<CordbType> relevantType;

    if (FAILED (hr= m_type->GetParentType((CordbClass *) pClass, &relevantType)))
    {
        return hr;
    }
    // Upon exit relevantType will either be the appropriate type for the
    // class we're looking for.

    hr = GetFieldValueForType(relevantType, fieldDef, ppValue);
    // GetParentType ands one reference to relevantType., Holder dtor releases
    return hr;

}

HRESULT CordbObjectValue::GetVirtualMethod(mdMemberRef memberRef,
                                           ICorDebugFunction **ppFunction)
{
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);
    FAIL_IF_NEUTERED(this);
    COV_VALIDATE_OBJECT();

    return E_NOTIMPL;
}

HRESULT CordbObjectValue::GetVirtualMethodAndType(mdMemberRef memberRef,
                                                  ICorDebugFunction **ppFunction,
                                                  ICorDebugType **ppType)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugType **);

    COV_VALIDATE_OBJECT();

    return E_NOTIMPL;
}

HRESULT CordbObjectValue::GetContext(ICorDebugContext **ppContext)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppContext, ICorDebugContext **);

    COV_VALIDATE_OBJECT();

    return E_NOTIMPL;
}

HRESULT CordbObjectValue::IsValueClass(BOOL *pbIsValueClass)
{
    FAIL_IF_NEUTERED(this);
    COV_VALIDATE_OBJECT();

    if (pbIsValueClass)
        *pbIsValueClass = FALSE;

    return S_OK;
}

HRESULT CordbObjectValue::GetManagedCopy(IUnknown **ppObject)
{
    // GetManagedCopy() is deprecated. In the case where the version of
    // the debugger doesn't match the version of the debuggee, the two processes
    // might have dangerously different notions of the layout of an object.

    // This function is deprecated
    return E_NOTIMPL;
}

HRESULT CordbObjectValue::SetFromManagedCopy(IUnknown *pObject)
{
    // Deprecated for the same reason as GetManagedCopy()
    return E_NOTIMPL;
}

HRESULT CordbObjectValue::GetValue(void *pTo)
{
    FAIL_IF_NEUTERED(this);
    COV_VALIDATE_OBJECT();

    VALIDATE_POINTER_TO_OBJECT_ARRAY(pTo, BYTE, m_size, false, true);

   // Copy out the value, which is the whole object.
    memcpy(pTo, m_objectCopy, m_size);

    return S_OK;
}

HRESULT CordbObjectValue::SetValue(void *pFrom)
{
    // You're not allowed to set a whole object at once.
    return E_INVALIDARG;
}

HRESULT CordbObjectValue::GetLength(ULONG32 *pcchString)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pcchString, SIZE_T *);
    FAIL_IF_NEUTERED(this);
    
    _ASSERTE(m_info.objTypeData.elementType == ELEMENT_TYPE_STRING);

    COV_VALIDATE_OBJECT();

    *pcchString = (ULONG32)m_info.stringInfo.length;
    return S_OK;
}

HRESULT CordbObjectValue::GetString(ULONG32 cchString,
                                    ULONG32 *pcchString,
                                    WCHAR szString[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(szString, WCHAR, cchString, true, true);
    VALIDATE_POINTER_TO_OBJECT(pcchString, SIZE_T *);

    _ASSERTE(m_info.objTypeData.elementType == ELEMENT_TYPE_STRING);

    COV_VALIDATE_OBJECT();

    if ((szString == NULL) || (cchString == 0))
        return E_INVALIDARG;

    // Add 1 to include null terminator
    SIZE_T len = m_info.stringInfo.length + 1;

    if (cchString < len)
        len = cchString;

    memcpy(szString, m_stringBuffer, len * 2);
    *pcchString = (ULONG32)m_info.stringInfo.length;

    return S_OK;
}

HRESULT CordbObjectValue::Init(void)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    LOG((LF_CORDB,LL_INFO1000,"Invoking COV::Init\n"));

    HRESULT hr = S_OK;

    hr = CordbValue::Init();

    if (FAILED(hr))
        return hr;

    SIZE_T nstructSize = 0;

    _ASSERTE (m_info.objTypeData.elementType != ELEMENT_TYPE_GENERICINST);
    _ASSERTE (m_info.objTypeData.elementType != ELEMENT_TYPE_VAR);
    _ASSERTE (m_info.objTypeData.elementType != ELEMENT_TYPE_MVAR);

    if (m_info.objTypeData.elementType == ELEMENT_TYPE_CLASS)
        nstructSize = m_info.nstructInfo.size;

    // Copy the entire object over to this process.
    m_objectCopy = new (nothrow) BYTE[m_size + nstructSize];

    if (m_objectCopy == NULL)
        return E_OUTOFMEMORY;

    BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                  (const void*) m_id,
                                  m_objectCopy,
                                  m_size,
                                  NULL);

    if (!succ)
        return HRESULT_FROM_GetLastError();

    // If this is an NStruct, copy the seperated NStruct fields and
    // append them onto the object. NOTE: the field offsets are
    // automatically adjusted by the left side in GetAndSendClassInfo.
    if (nstructSize != 0)
    {
        succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                 (const void*) m_info.nstructInfo.ptr,
                                 m_objectCopy + m_size,
                                 nstructSize,
                                 NULL);

        if (!succ)
            return HRESULT_FROM_GetLastError();
    }


    // Compute offsets to the locals and to a string if this is a
    // string object.
    m_objectLocalVars = m_objectCopy + m_info.objOffsetToVars;

    if (m_info.objTypeData.elementType == ELEMENT_TYPE_STRING)
        m_stringBuffer = m_objectCopy + m_info.stringInfo.offsetToStringBase;

    return hr;
}

/* ------------------------------------------------------------------------- *
 * Valuce Class Object Value class
 * ------------------------------------------------------------------------- */

CordbVCObjectValue::CordbVCObjectValue(CordbAppDomain *appdomain,
                                       CordbType *type,
                                       REMOTE_PTR remoteAddress,
                                       void *localAddress,
                                       RemoteAddress *remoteRegAddr)

    : CordbValue(appdomain, type, remoteAddress, localAddress, remoteRegAddr, false, appdomain->GetSweepableExitNeuterList()),
      m_objectCopy(NULL)
{
}

CordbVCObjectValue::~CordbVCObjectValue()
{
    DTOR_ENTRY(this);

    _ASSERTE(IsNeutered());

    // Destroy the copy of the object.
    if (m_objectCopy != NULL)
    {
        delete [] m_objectCopy;
    }
}

HRESULT CordbVCObjectValue::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugValue)
    {
        *pInterface = static_cast<ICorDebugValue*>(static_cast<ICorDebugObjectValue*>(this));
    }
    else if (id == IID_ICorDebugValue2)
    {
        *pInterface = static_cast<ICorDebugValue2*>(this);
    }
    else if (id == IID_ICorDebugObjectValue)
    {
        *pInterface = static_cast<ICorDebugObjectValue*>(this);
    }
    else if (id == IID_ICorDebugObjectValue2)

    {
        *pInterface = static_cast<ICorDebugObjectValue2*>(this);
    }
    else if (id == IID_ICorDebugGenericValue)
    {
        *pInterface = static_cast<ICorDebugGenericValue*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugObjectValue*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbVCObjectValue::GetType(CorElementType *pType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pType, CorElementType *);

    *pType = ELEMENT_TYPE_VALUETYPE;
    return S_OK;
}


HRESULT CordbVCObjectValue::GetClass(ICorDebugClass **ppClass)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    *ppClass = (ICorDebugClass*) GetClass();

    if (*ppClass != NULL)
        (*ppClass)->AddRef();

    return S_OK;
}

CordbClass *CordbVCObjectValue::GetClass()
{
    CordbClass *tycon;
    Instantiation inst;
    m_type->DestConstructedType(&tycon, &inst);
    return tycon;
}

HRESULT CordbVCObjectValue::GetFieldValueForType(ICorDebugType *pType,
                                          mdFieldDef fieldDef,
                                          ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    // Validate the token.
    if (m_type->m_class == NULL || !m_type->m_class->GetModule()->m_pIMImport->IsValidToken(fieldDef))
        return E_INVALIDARG;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    CordbType *type;

    //
    //
    if (pType == NULL)
    {
        type = m_type;
    }
    else
    {
        type = static_cast<CordbType*> (pType);
    }

    DebuggerIPCE_FieldData *pFieldData;

#ifdef _DEBUG
    pFieldData = NULL;
#endif

    HRESULT hr = type->GetFieldInfo(fieldDef, &pFieldData);

    _ASSERTE(hr != CORDBG_E_ENC_HANGING_FIELD);
    // If we get back CORDBG_E_ENC_HANGING_FIELD we'll just fail -
    // value classes should not be able to add fields once they're loaded,
    // since the new fields _can't_ be contiguous with the old fields,
    // and having all the fields contiguous is kinda the point of a V.C.

    if (FAILED(hr))
    {
        return hr;
    }
    _ASSERTE(pFieldData != NULL);

    // Compute the remote address, too, so that SetValue will work.
    BYTE *ra = NULL;
    RemoteAddress *pra = NULL;

    if (m_remoteRegAddr.kind == RAK_NONE)
    {
        _ASSERTE(NULL != m_id);
        ra = (BYTE *)(m_id + pFieldData->GetInstanceOffset() );
    }
    else
    {
        // Remote register address is the same as the parent....
        /*
         *       
         */
        pra = &m_remoteRegAddr;
    }

    CordbModule *pModule = type->m_class->GetModule();
    
    SigParser sigParser;
    IfFailRet( pFieldData->GetFieldSignature(pModule, &sigParser) );
        
    CordbType *fldtyp;
    IfFailRet( CordbType::SigToType(pModule, &sigParser, type->m_inst, &fldtyp) );

    // create a value for the member field.
    hr = CordbValue::CreateValueByType(m_appdomain,
                                       fldtyp,
                                       false,
                                       (void*)ra,
                                       &(m_objectCopy[pFieldData->GetInstanceOffset()]),
                                       pra,
                                       NULL,
                                       ppValue);

    return hr;
}

HRESULT CordbVCObjectValue::GetFieldValue(ICorDebugClass *pClass,
                                        mdFieldDef fieldDef,
                                        ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(pClass, ICorDebugClass *);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);

    HRESULT hr;
    _ASSERTE(m_type);

    if (m_type->m_elementType != ELEMENT_TYPE_CLASS &&
        m_type->m_elementType != ELEMENT_TYPE_VALUETYPE)
    {
        return E_INVALIDARG;
    }

    RSExtSmartPtr<CordbType> relevantType;

    if (FAILED (hr= m_type->GetParentType((CordbClass *) pClass, &relevantType)))
    {
        return hr;
    }
    // Upon exit relevantType will either be the appropriate type for the
    // class we're looking for.

    hr = GetFieldValueForType(relevantType, fieldDef, ppValue);
    // GetParentType ands one reference to relevantType, holder dtor releases that.
    return hr;

}

HRESULT CordbVCObjectValue::GetValue(void *pTo)
{
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pTo, BYTE, m_size, false, true);
    FAIL_IF_NEUTERED(this);
    
    // Copy out the value, which is the whole object.
    memcpy(pTo, m_objectCopy, m_size);

    return S_OK;
}

HRESULT CordbVCObjectValue::SetValue(void *pFrom)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    HRESULT hr = S_OK;

    VALIDATE_POINTER_TO_OBJECT_ARRAY(pFrom, BYTE, m_size, true, false);

    // Can't change literals...
    if (m_isLiteral)
        return E_INVALIDARG;

    // We had better have a remote address.
    _ASSERTE((m_id != NULL) || (m_remoteRegAddr.kind != RAK_NONE));

    if (m_type)
    {
        IfFailRet(m_type->Init(FALSE));
    }        

    // If not enregistered, send a Set Value Class message to the right side with the address of this value class, the
    // address of the new data, and the class of the value class that we're setting.
    if (m_id != NULL)
    {
        DebuggerIPCEvent event;

        // First, we have to make room on the Left Side for the new data for the value class. We allocate memory on the
        // Left Side for this, then write the new data across. The Set Value Class message will free the buffer when its
        // done.
        void *buffer = NULL;
        IfFailRet(GetProcess()->GetAndWriteRemoteBuffer(m_appdomain, m_size, pFrom, &buffer));

        // Finally, send over the Set Value Class message.
        GetProcess()->InitIPCEvent(&event, DB_IPCE_SET_VALUE_CLASS, true, m_appdomain->GetADToken());
        event.SetValueClass.oldData = (void*)m_id;
        event.SetValueClass.newData = buffer;
        IfFailRet(m_type->TypeToBasicTypeData(&event.SetValueClass.type));

        // Note: two-way event here...
        IfFailRet(GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event, sizeof(DebuggerIPCEvent)));

        _ASSERTE(event.type == DB_IPCE_SET_VALUE_CLASS_RESULT);

        hr = event.hr;
    }
    else
    {
        // The value class is enregistered, so we don't have to go through the
        // Left Side. Simply update the proper register.
        if (m_size > sizeof(DWORD))
            return E_INVALIDARG;

        DWORD newValue = *((DWORD*)pFrom);
        hr = SetEnregisteredValue((void*)&newValue);
    }

    // That worked, so update the copy of the value we have over here.
    if (SUCCEEDED(hr))
        memcpy(m_objectCopy, pFrom, m_size);

    return hr;
}

HRESULT CordbVCObjectValue::GetVirtualMethod(mdMemberRef memberRef,
                                           ICorDebugFunction **ppFunction)
{
    return E_NOTIMPL;
}

HRESULT CordbVCObjectValue::GetVirtualMethodAndType(mdMemberRef memberRef,
                                                    ICorDebugFunction **ppFunction,
                                                    ICorDebugType **ppType)
{
    return E_NOTIMPL;
}

HRESULT CordbVCObjectValue::GetContext(ICorDebugContext **ppContext)
{
    return E_NOTIMPL;
}

HRESULT CordbVCObjectValue::IsValueClass(BOOL *pbIsValueClass)
{
    if (pbIsValueClass)
        *pbIsValueClass = TRUE;

    return S_OK;
}

HRESULT CordbVCObjectValue::GetManagedCopy(IUnknown **ppObject)
{
    // This function is deprecated
    return E_NOTIMPL;
}

HRESULT CordbVCObjectValue::SetFromManagedCopy(IUnknown *pObject)
{
    // This function is deprecated
    return E_NOTIMPL;
}

HRESULT
CordbValue::GetEnregisteredValue(BYTE* pTo, SIZE_T size)
{
#ifndef _X86_
    _ASSERTE(!"@TODO IA64/AMD64 -- Not Yet Implemented");
    return E_NOTIMPL;
#else // !_X86_
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    _ASSERTE(RAK_NONE != m_remoteRegAddr.kind);

    HANDLE hProcess = GetProcess()->m_handle;

    HRESULT hr = S_OK;
    ICorDebugNativeFrame* pINativeFrame;

    // The parent of an enregistered value must be a native frame.
    hr = m_pParent->QueryInterface(IID_ICorDebugNativeFrame,
                                   reinterpret_cast< void ** >(&pINativeFrame));
    // nickbe 03/06/2003 03:29:1
    //
    // Failing this QI indicates that this value is an enregistered subobject.
    // We currently don't have enregistered subobjects, so we can assert that
    // it will just always succeed.
    //
    _ASSERTE(SUCCEEDED(hr));
    _ASSERTE(NULL != pINativeFrame);
    CordbNativeFrame* pNativeFrame =
        static_cast< CordbNativeFrame* >(pINativeFrame);
    _ASSERTE(RAK_NONE != m_remoteRegAddr.kind);
    switch (m_remoteRegAddr.kind)
    {
        case RAK_REG:
            {
                UINT_PTR* reg =
                    pNativeFrame->GetAddressOfRegister(m_remoteRegAddr.reg1);
                PREFIX_ASSUME(reg != NULL);
                _ASSERTE(sizeof(*reg) == size);

                memcpy(pTo, reg, sizeof(*reg));
            }
            break;
        case RAK_REGREG:
            {
                UINT_PTR* highWordAddr =
                    pNativeFrame->GetAddressOfRegister(m_remoteRegAddr.reg1);
                PREFIX_ASSUME(highWordAddr != NULL);
                UINT_PTR* lowWordAddr =
                    pNativeFrame->GetAddressOfRegister(m_remoteRegAddr.u.reg2);
                PREFIX_ASSUME(lowWordAddr != NULL);
                _ASSERTE(sizeof(*highWordAddr) + sizeof(*lowWordAddr) == size);

                memcpy(pTo, lowWordAddr, sizeof(*lowWordAddr));
                memcpy(&pTo[sizeof(*lowWordAddr)],
                       highWordAddr, sizeof(*highWordAddr));
            }
            break;
        case RAK_REGMEM:
            {
                // Read the high bits from the register...

                UINT_PTR* highBitsAddr =
                    pNativeFrame->GetAddressOfRegister(m_remoteRegAddr.reg1);
                PREFIX_ASSUME(highBitsAddr != NULL);

                // ... and the low bits from the remote process
                DWORD lowBits;
                /*
                 *       
                 */
                BOOL success =
                    ReadProcessMemoryI(hProcess,
                                       reinterpret_cast< void* >(static_cast< ULONG >(m_remoteRegAddr.addr)),
                                       &lowBits,
                                       sizeof(lowBits),
                                       NULL);
                if (!success)
                {
                    hr = HRESULT_FROM_GetLastError();
                    break;
                }

                _ASSERTE(sizeof(lowBits)+sizeof(*highBitsAddr) == size);

                memcpy(pTo, &lowBits, sizeof(lowBits));
                memcpy(&pTo[sizeof(lowBits)],
                       highBitsAddr, sizeof(*highBitsAddr));
            }
            break;
        case RAK_MEMREG:
            {
                // Read the high bits from the remote process' memory
                DWORD highBits;
                BOOL success = ReadProcessMemoryI(hProcess,
                                                  reinterpret_cast< void* >(static_cast< ULONG >(m_remoteRegAddr.addr)),
                                                  &highBits,
                                                  sizeof(highBits),
                                                  NULL);
                if (!success)
                {
                    hr = HRESULT_FROM_GetLastError();
                    break;
                }

                // and the low bits from a register
                UINT_PTR* lowBitsAddr =
                    pNativeFrame->GetAddressOfRegister(m_remoteRegAddr.reg1);
                PREFIX_ASSUME(lowBitsAddr != NULL);

                _ASSERTE(sizeof(*lowBitsAddr)+sizeof(highBits) == size);

                memcpy(pTo, lowBitsAddr, sizeof(*lowBitsAddr));
                memcpy(&pTo[sizeof(*lowBitsAddr)],
                       &highBits, sizeof(highBits));
            }
            break;
        default:
            _ASSERTE(!"invalid variable home");
            hr = E_NOTIMPL;
            break;

    }

    pINativeFrame->Release();
    return hr;
#endif // !_X86_
}

//
// CordbVCObjectValue::Init
//
// Description
//      Initializes the Right-Side's representation of a Value Class object.
// Parameters
//      None
// Returns
//      HRESULT
//          S_OK if the function completed normally
//          failing HR otherwise
// Exceptions
//      None
//
HRESULT CordbVCObjectValue::Init(void)
{
#if !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_)
    PORTABILITY_ASSERT("NYI: CordbVCObjectValue::Init for this platform");
    return E_NOTIMPL;
#endif // !_X86_ && !_IA64_ && !_AMD64_
    HRESULT hr = S_OK;

    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); // 
    IfFailRet( CordbValue::Init() );

    // Get the object size from the class
    IfFailRet( m_type->GetUnboxedObjectSize(&m_size) );

    // Copy the entire object over to this process.
    m_objectCopy = new (nothrow) BYTE[m_size];

    if (m_objectCopy == NULL)
    {
        return E_OUTOFMEMORY;
    }

    if (m_localAddress != NULL)
    {
        // The data is already in the local address space. Go ahead and copy it
        // from there.
        memcpy(m_objectCopy, m_localAddress, m_size);
        return S_OK;
    }

    if (NULL != m_id)
    {
        // Data isn't the the local address space, but we can still get it from
        // the remote process' address space. Copy it from there.
        BOOL success =
            ReadProcessMemoryI(GetProcess()->m_handle,
                               reinterpret_cast< void* >(m_id),
                               m_objectCopy, m_size, NULL);
        if (!success)
        {
            return HRESULT_FROM_GetLastError();
        }
        return S_OK;
    }

    // Okay, the data isn't just in the remote process' address space; it's
    // enregistered (or at least partially enregistered). Go fetch it from it's
    // home
    ICorDebugNativeFrame* pINativeFrame = NULL;
    hr = m_pParent->QueryInterface(IID_ICorDebugNativeFrame,
                                   reinterpret_cast< void** >(&pINativeFrame));
    if (FAILED(hr))
    {
        return hr;
    }
    CordbNativeFrame* pNativeFrame =
        static_cast< CordbNativeFrame* >(pINativeFrame);
    hr = this->GetEnregisteredValue(m_objectCopy, m_size);
    pNativeFrame->Release();

    return hr;
}

/* ------------------------------------------------------------------------- *
 * Box Value class
 * ------------------------------------------------------------------------- */

CordbBoxValue::CordbBoxValue(CordbAppDomain *appdomain,
                             CordbType *type,
                             REMOTE_PTR remoteAddress,
                             SIZE_T objectSize,
                             SIZE_T offsetToVars)
    : CordbValue(appdomain, type, remoteAddress, NULL, NULL, false, appdomain->GetProcess()->GetContinueNeuterList()),
       m_offsetToVars(offsetToVars)
{
    m_size = (ULONG32)objectSize;
}

CordbBoxValue::~CordbBoxValue()
{
    DTOR_ENTRY(this);
    _ASSERTE(IsNeutered());
}

HRESULT CordbBoxValue::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugValue)
    {
        *pInterface = static_cast<ICorDebugValue*>(static_cast<ICorDebugBoxValue*>(this));
    }
    else if (id == IID_ICorDebugValue2)

    {
        *pInterface = static_cast<ICorDebugValue2*>(this);
    }
    else if (id == IID_ICorDebugBoxValue)
    {
        *pInterface = static_cast<ICorDebugBoxValue*>(this);
    }
    else if (id == IID_ICorDebugGenericValue)
    {
        *pInterface = static_cast<ICorDebugGenericValue*>(this);
    }
    else if (id == IID_ICorDebugHeapValue)
    {
        *pInterface = static_cast<ICorDebugHeapValue*>(this);
    }
    else if (id == IID_ICorDebugHeapValue2)
    {
        *pInterface = static_cast<ICorDebugHeapValue2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugBoxValue*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbBoxValue::GetType(CorElementType *pType)
{
    VALIDATE_POINTER_TO_OBJECT(pType, CorElementType *);

    *pType = ELEMENT_TYPE_CLASS;

    return (S_OK);
}

HRESULT CordbBoxValue::IsValid(BOOL *pbValid)
{
    VALIDATE_POINTER_TO_OBJECT(pbValid, BOOL *);


    return E_NOTIMPL;
}

HRESULT CordbBoxValue::CreateRelocBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
{
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoint, ICorDebugValueBreakpoint **);

    return E_NOTIMPL;
}

/*
* Creates a handle of the given type for this heap value.
*
* Not Implemented In-Proc.
*/
HRESULT CordbBoxValue::CreateHandle(
    CorDebugHandleType handleType,
    ICorDebugHandleValue ** ppHandle)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    
    return CordbValue::InternalCreateHandle(handleType, ppHandle);
}   // CreateHandle

HRESULT CordbBoxValue::GetValue(void *pTo)
{
    // Can't get a whole copy of a box.
    return E_INVALIDARG;
}

HRESULT CordbBoxValue::SetValue(void *pFrom)
{
    // You're not allowed to set a box value.
    return E_INVALIDARG;
}

HRESULT CordbBoxValue::GetObject(ICorDebugObjectValue **ppObject)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppObject, ICorDebugObjectValue **);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    RSSmartPtr<CordbVCObjectValue> pVCValue(
        new (nothrow) CordbVCObjectValue(m_appdomain, m_type,
                               (REMOTE_PTR)((BYTE*)m_id + m_offsetToVars), NULL, NULL) );

    if (pVCValue != NULL)
    {
        hr = pVCValue->Init();

        if (SUCCEEDED(hr))
        {
            pVCValue->AddRef();
            *ppObject = (ICorDebugObjectValue*)pVCValue;
        }
    }
    else
        hr = E_OUTOFMEMORY;

    return hr;
}

HRESULT CordbBoxValue::Init(void)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    HRESULT hr = CordbValue::Init();

    if (FAILED(hr))
        return hr;

    // Box values only really remember the info needed to unbox and
    // create a value class value.
    return S_OK;
}


/* ------------------------------------------------------------------------- *
 * Array Value class
 * ------------------------------------------------------------------------- */

// How large of a buffer do we allocate to hold array elements.
// Note that since we must be able to hold at least one element, we may
// allocate larger than the cache size here.
// Also, this cache doesn't include a small header used to store the rank vectors
#ifdef _DEBUG
// For debug, use a small size to cause more churn
    #define ARRAY_CACHE_SIZE (1000)
#else
// For release, guess 4 pages should be enough. Subtract some bytes to store
// the header so that that doesn't push us onto another page. (We guess a reasonable
// header size, but it's ok if it's larger).
    #define ARRAY_CACHE_SIZE (4 * 4096 - 24)
#endif


CordbArrayValue::CordbArrayValue(CordbAppDomain *appdomain,
                                 CordbType *type,
                                 DebuggerIPCE_ObjectData *pObjectInfo)
    : CordbValue(appdomain, type, pObjectInfo->objRef, NULL, NULL, false, appdomain->GetProcess()->GetContinueNeuterList()),
      m_info(*pObjectInfo),
      m_objectCopy(NULL)
{
    m_size = (ULONG32)m_info.objSize;

    type->DestUnaryType(&m_elemtype);

// Set range to illegal values to force a load on first access
    m_idxLower = m_idxUpper = (unsigned int) -1;
}

CordbArrayValue::~CordbArrayValue()
{
    DTOR_ENTRY(this);
    _ASSERTE(IsNeutered());    

    // Destroy the copy of the object.
    if (m_objectCopy != NULL)
        delete [] m_objectCopy;
}

HRESULT CordbArrayValue::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugValue)
    {
        *pInterface = static_cast<ICorDebugValue*>(static_cast<ICorDebugArrayValue*>(this));
    }
    else if (id == IID_ICorDebugValue2)

    {
        *pInterface = static_cast<ICorDebugValue2*>(this);
    }
    else if (id == IID_ICorDebugArrayValue)
    {
        *pInterface = static_cast<ICorDebugArrayValue*>(this);
    }
    else if (id == IID_ICorDebugGenericValue)
    {
        *pInterface = static_cast<ICorDebugGenericValue*>(this);
    }
    else if (id == IID_ICorDebugHeapValue)
    {
        *pInterface = static_cast<ICorDebugHeapValue*>(this);
    }
    else if (id == IID_ICorDebugHeapValue2)
    {
        *pInterface = static_cast<ICorDebugHeapValue2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugArrayValue*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbArrayValue::GetElementType(CorElementType *pType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pType, CorElementType *);

    *pType = m_elemtype->m_elementType;
    return S_OK;
}


HRESULT CordbArrayValue::GetRank(ULONG32 *pnRank)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pnRank, SIZE_T *);

    // Rank info is duplicated for sanity checking - double check it here.
    _ASSERTE(m_info.arrayInfo.rank == m_type->m_rank);
    *pnRank = m_type->m_rank;
    return S_OK;
}

HRESULT CordbArrayValue::GetCount(ULONG32 *pnCount)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);    
    VALIDATE_POINTER_TO_OBJECT(pnCount, SIZE_T *);

    *pnCount = m_info.arrayInfo.componentCount;
    return S_OK;
}

HRESULT CordbArrayValue::GetDimensions(ULONG32 cdim, ULONG32 dims[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(dims, SIZE_T, cdim, true, true);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Rank info is duplicated for sanity checking - double check it here.
    _ASSERTE(m_info.arrayInfo.rank == m_type->m_rank);
    if (cdim != m_type->m_rank)
        return E_INVALIDARG;

    // SDArrays don't have bounds info, so return the component count.
    if (cdim == 1)
        dims[0] = m_info.arrayInfo.componentCount;
    else
    {
        _ASSERTE(m_info.arrayInfo.offsetToUpperBounds != 0);
        _ASSERTE(m_arrayUpperBase != NULL);

        // The upper bounds info in the array is the true size of each
        // dimension.
        for (unsigned int i = 0; i < cdim; i++)
            dims[i] = m_arrayUpperBase[i];
    }

    return S_OK;
}

HRESULT CordbArrayValue::HasBaseIndicies(BOOL *pbHasBaseIndicies)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);    
    VALIDATE_POINTER_TO_OBJECT(pbHasBaseIndicies, BOOL *);

    *pbHasBaseIndicies = m_info.arrayInfo.offsetToLowerBounds != 0;
    return S_OK;
}

HRESULT CordbArrayValue::GetBaseIndicies(ULONG32 cdim, ULONG32 indicies[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);    
    VALIDATE_POINTER_TO_OBJECT_ARRAY(indicies, SIZE_T, cdim, true, true);

    // Rank info is duplicated for sanity checking - double check it here.
    _ASSERTE(m_info.arrayInfo.rank == m_type->m_rank);
    if ((cdim != m_type->m_rank) ||
        (m_info.arrayInfo.offsetToLowerBounds == 0))
        return E_INVALIDARG;

    _ASSERTE(m_arrayLowerBase != NULL);

    for (unsigned int i = 0; i < cdim; i++)
        indicies[i] = m_arrayLowerBase[i];

    return S_OK;
}

HRESULT CordbArrayValue::CreateElementValue(void *remoteElementPtr,
                                            void *localElementPtr,
                                            ICorDebugValue **ppValue)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    return CordbValue::CreateValueByType(m_appdomain,
        m_elemtype,
        false,
        remoteElementPtr,
        localElementPtr,
        NULL,
        NULL,
        ppValue);

}

HRESULT CordbArrayValue::GetElement(ULONG32 cdim, ULONG32 indicies[],
                                    ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(indicies, SIZE_T, cdim, true, true);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    *ppValue = NULL;

    // Rank info is duplicated for sanity checking - double check it here.
    _ASSERTE(m_info.arrayInfo.rank == m_type->m_rank);
    if ((cdim != m_type->m_rank) || (indicies == NULL))
        return E_INVALIDARG;

    // If the array has lower bounds, adjust the indicies.
    if (m_info.arrayInfo.offsetToLowerBounds != 0)
    {
        _ASSERTE(m_arrayLowerBase != NULL);

        for (unsigned int i = 0; i < cdim; i++)
            indicies[i] -= m_arrayLowerBase[i];
    }

    SIZE_T offset = 0;

    // SDArrays don't have upper bounds
    if (cdim == 1)
    {
        offset = indicies[0];

        // Bounds check
        if (offset >= m_info.arrayInfo.componentCount)
            return E_INVALIDARG;
    }
    else
    {
        _ASSERTE(m_info.arrayInfo.offsetToUpperBounds != 0);
        _ASSERTE(m_arrayUpperBase != NULL);

        // Calculate the offset for all dimensions.
        DWORD multiplier = 1;

        for (int i = cdim - 1; i >= 0; i--)
        {
            // Bounds check
            if (indicies[i] >= m_arrayUpperBase[i])
                return E_INVALIDARG;

            offset += indicies[i] * multiplier;
            multiplier *= m_arrayUpperBase[i];
        }

        _ASSERTE(offset < m_info.arrayInfo.componentCount);
    }

    return GetElementAtPosition((ULONG32)offset, ppValue);
}

HRESULT CordbArrayValue::GetElementAtPosition(ULONG32 nPosition,
                                              ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    if (nPosition >= m_info.arrayInfo.componentCount)
    {
        *ppValue = NULL;
        return E_INVALIDARG;
    }

    // Rank info is duplicated for sanity checking - double check it here.
    _ASSERTE(m_info.arrayInfo.rank == m_type->m_rank);
    const int cbHeader = 2 * m_type->m_rank * sizeof(DWORD);

    // Ensure that the proper subset is in the cache
    if (nPosition < m_idxLower || nPosition >= m_idxUpper)
    {
        const int cbElemSize = (int)m_info.arrayInfo.elementSize;
        int len = max(ARRAY_CACHE_SIZE / cbElemSize, 1);
        m_idxLower = nPosition;
        m_idxUpper = min(m_idxLower + len, m_info.arrayInfo.componentCount);
        _ASSERTE(m_idxLower < m_idxUpper);

        int cbOffsetFrom = (int)(m_info.arrayInfo.offsetToArrayBase + m_idxLower * cbElemSize);

        int cbSize = (m_idxUpper - m_idxLower) * cbElemSize;

    // Copy the proper subrange of the array over
        BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                      ((const BYTE*) m_id) + cbOffsetFrom,
                                      m_objectCopy + cbHeader,
                                      cbSize,
                                      NULL);

        if (!succ)
            return HRESULT_FROM_GetLastError();
    }

    // calculate local address
    void *localElementPtr = m_objectCopy + cbHeader +
        ((nPosition - m_idxLower) * m_info.arrayInfo.elementSize);

    REMOTE_PTR remoteElementPtr = (REMOTE_PTR)(m_id +
        m_info.arrayInfo.offsetToArrayBase +
        (nPosition * m_info.arrayInfo.elementSize));

    return CreateElementValue(remoteElementPtr, localElementPtr, ppValue);
}

HRESULT CordbArrayValue::IsValid(BOOL *pbValid)
{
    VALIDATE_POINTER_TO_OBJECT(pbValid, BOOL *);


    return E_NOTIMPL;
}

HRESULT CordbArrayValue::CreateRelocBreakpoint(
                                      ICorDebugValueBreakpoint **ppBreakpoint)
{
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoint, ICorDebugValueBreakpoint **);

    return E_NOTIMPL;
}

/*
* Creates a handle of the given type for this heap value.
*
* Not Implemented In-Proc.
*/
HRESULT CordbArrayValue::CreateHandle(
    CorDebugHandleType handleType,
    ICorDebugHandleValue ** ppHandle)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    
    return CordbValue::InternalCreateHandle(handleType, ppHandle);
}   // CreateHandle


HRESULT CordbArrayValue::GetValue(void *pTo)
{
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pTo, void *, 1, false, true);
    FAIL_IF_NEUTERED(this);
    
    // Copy out the value, which is the whole array.
    // There's no lazy-evaluation here, so this could be rather large
    BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                  (const void*) m_id,
                                  pTo,
                                  m_size,
                                  NULL);

    if (!succ)
        return HRESULT_FROM_GetLastError();

    return S_OK;
}

HRESULT CordbArrayValue::SetValue(void *pFrom)
{
    // You're not allowed to set a whole array at once.
    return E_INVALIDARG;
}


HRESULT CordbArrayValue::Init(void)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    HRESULT hr = S_OK;

    hr = CordbValue::Init();

    if (FAILED(hr))
        return hr;


    int cbVector = (int)(m_info.arrayInfo.rank * sizeof(DWORD));
    int cbHeader = 2 * cbVector;

    // Find largest data size that will fit in cache
    unsigned int cbData = (unsigned int)(m_info.arrayInfo.componentCount * m_info.arrayInfo.elementSize);
    if (cbData > ARRAY_CACHE_SIZE)
    {
        cbData = (unsigned int)(ARRAY_CACHE_SIZE / m_info.arrayInfo.elementSize)
            * (unsigned int)m_info.arrayInfo.elementSize;
    }

    if (cbData < m_info.arrayInfo.elementSize)
    {
        cbData = (unsigned int)m_info.arrayInfo.elementSize;
    }

    // Allocate memory
    m_objectCopy = new (nothrow) BYTE[cbHeader + cbData];
    if (m_objectCopy == NULL)
        return E_OUTOFMEMORY;


    m_arrayLowerBase  = NULL;
    m_arrayUpperBase  = NULL;

    // Copy base vectors into header. (Offsets are 0 if the vectors aren't used)
    if (m_info.arrayInfo.offsetToLowerBounds != 0)
    {
        m_arrayLowerBase  = (DWORD*)(m_objectCopy);

        BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                      ((const BYTE*) m_id) + m_info.arrayInfo.offsetToLowerBounds,
                                      m_arrayLowerBase,
                                      cbVector,
                                      NULL);

        if (!succ)
            return HRESULT_FROM_GetLastError();
    }


    if (m_info.arrayInfo.offsetToUpperBounds != 0)
    {
        m_arrayUpperBase  = (DWORD*)(m_objectCopy + cbVector);
        BOOL succ = ReadProcessMemoryI(GetProcess()->m_handle,
                                      ((const BYTE*) m_id) + m_info.arrayInfo.offsetToUpperBounds,
                                      m_arrayUpperBase,
                                      cbVector,
                                      NULL);

        if (!succ)
            return HRESULT_FROM_GetLastError();
    }

    // That's all for now. We'll do lazy-evaluation for the array contents.

    return hr;
}

/* ------------------------------------------------------------------------- *
 * Handle Value
 * ------------------------------------------------------------------------- */
CordbHandleValue::CordbHandleValue(
    CordbAppDomain *appdomain,
    CordbType           *type,                        // The type of object that we create handle on
    CorDebugHandleType handleType)         // strong or weak handle
    : CordbValue(appdomain, type, NULL, NULL, NULL, false, 
                    appdomain->GetSweepableExitNeuterList()
                )
{
    // In this case, m_id will be the object handle.

    m_fCanBeValid = TRUE;

    m_handleType = handleType;
    m_size = sizeof(void*);
}

HRESULT CordbHandleValue::Init(LSPTR_OBJECTHANDLE pHandle)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); 
    HRESULT hr = S_OK;

    // If it is a strong handle, m_pHandle will not be NULL unless Dispose method is called.
    // If it is a weak handle, m_pHandle can be NULL when Dispose is called.
    m_pHandle = pHandle;
    
    // This will init m_info.
    IfFailRet(RefreshHandleValue());

    // objRefBad is currently overloaded to mean that 1) the object ref is invalid, or 2) the object ref is NULL.
    // NULL is clearly not a bad object reference, but in either case we have no more type data to work with,
    // so don't attempt to assign more specific type information to the reference.
    if (!m_info.objRefBad)
    {
        // We need to get the type info from the left side.
        CordbType *newtype;

        IfFailRet(CordbType::TypeDataToType(m_appdomain, &m_info.objTypeData, &newtype));

        m_type.Assign(newtype);
    }

    return hr;
}

CordbHandleValue::~CordbHandleValue()
{
    DTOR_ENTRY(this);

    _ASSERTE(IsNeutered());
}


void CordbHandleValue::Neuter(NeuterTicket ticket)
{
    // recycle the handle back to EE.

    // Need to recycle the handle. Don't need to check error.
    // Dispose() fails if we're neutered, but we're not marked neuter yet.
    Dispose();

    CordbValue::Neuter(ticket);
}

//***************************************************************
//
// Helper: Refresh the handle value object.
// return BOOL indicate if pObjectToken is pointed to a valid object or not.
//
//***************************************************************

HRESULT CordbHandleValue::RefreshHandleValue()
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    // If Dispose has been called, don't bother to refresh handle value.
    if (m_pHandle == NULL)
    {
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }

    // If weak handle and the object was dead, no point to refresh the handle value
    if (m_fCanBeValid == FALSE)
    {
        return CORDBG_E_BAD_REFERENCE_VALUE;
    }

    DebuggerIPCEvent event;
    CorElementType type = m_type->m_elementType;
    HRESULT     hr = S_OK;

    _ASSERTE (type != ELEMENT_TYPE_GENERICINST);
    _ASSERTE (type != ELEMENT_TYPE_VAR);
    _ASSERTE (type != ELEMENT_TYPE_MVAR);

    GetProcess()->InitIPCEvent(&event,
                            DB_IPCE_GET_OBJECT_INFO,
                            true,
                            m_appdomain->GetADToken());

    event.GetObjectInfo.objectRefAddress = NULL; // ignored
    event.GetObjectInfo.objectRefIsValue = TRUE; // ignored
    event.GetObjectInfo.objectHandle = m_pHandle;
    event.GetObjectInfo.objectType = (CorElementType)type;

    // Note: two-way event here...
    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event,
                                          sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        return hr;

    _ASSERTE(event.type == DB_IPCE_GET_OBJECT_INFO_RESULT);
    // Save the results for later.
    m_info = event.GetObjectInfoResult;

    // If reference is already gone bad or reference is NULL,
    // don't bother to refetch in the future.
    //
    if ((m_info.objRefBad) || (m_info.objRef == NULL))
    {
        m_fCanBeValid = FALSE;
    }        

    return event.hr;
}

HRESULT CordbHandleValue::QueryInterface(REFIID id, void **pInterface)
{
    VALIDATE_POINTER_TO_OBJECT(pInterface, void **);

    if (id == IID_ICorDebugValue)
    {
        *pInterface = static_cast<ICorDebugValue*>(this);
    }
    else if ((id == IID_ICorDebugValue2))
    {
        *pInterface = static_cast<ICorDebugValue2*>(this);
    }
    else if (id == IID_ICorDebugReferenceValue)
    {
        *pInterface = static_cast<ICorDebugReferenceValue*>(this);
    }
    else if (id == IID_ICorDebugHandleValue)
    {
        *pInterface = static_cast<ICorDebugHandleValue*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugHandleValue*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}


// return handle type. Currently we have strong and weak.
HRESULT CordbHandleValue::GetHandleType(CorDebugHandleType *pType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pType, CorDebugHandleType *);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    if (m_pHandle == NULL)
    {
        // handle has been disposed!
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }
    *pType = m_handleType;
    return S_OK;
}

// Dispose will cause handle to be recycled.
HRESULT CordbHandleValue::Dispose()
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    HRESULT             hr = S_OK;
    DebuggerIPCEvent    event;
    CordbProcess        *process;

    process = GetProcess();

    // Process should still be alive because it would have neutered us if it became invalid.
    _ASSERTE(process != NULL);

    if (m_pHandle == NULL)
    {
        // handle has been disposed!
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }

    LSPTR_OBJECTHANDLE objHandle = m_pHandle;
    m_pHandle = LSPTR_OBJECTHANDLE::NullPtr();

    if (process->m_exiting)
    {
        // process is exiting. Don't do anything
        return S_OK;
    }

    // recycle the handle to EE
    process->InitIPCEvent(&event,
                          DB_IPCE_DISPOSE_HANDLE,
                          false,
                          m_appdomain->GetADToken());

    event.DisposeHandle.objectHandle = objHandle;
    if (m_handleType == HANDLE_STRONG)
    {
        event.DisposeHandle.fStrong = TRUE;
    }
    else
    {
        event.DisposeHandle.fStrong = FALSE;
    }

    // Note: one-way event here...
    hr = process->m_cordb->SendIPCEvent(process, &event,
                                        sizeof(DebuggerIPCEvent));

    hr = WORST_HR(hr, event.hr);
    
    return hr;
}   // Dispose

HRESULT CordbHandleValue::GetType(CorElementType *pType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pType, CorElementType *);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );
        
    HRESULT     hr = S_OK;

    if (m_pHandle == NULL)
    {
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }

    bool isBoxedVCObject = false;
    if ((m_type->m_class != NULL) && (m_type->m_elementType != ELEMENT_TYPE_STRING))
    {
        hr = m_type->m_class->IsValueClass(&isBoxedVCObject);

        if (FAILED(hr))
            return hr;
    }

    if (isBoxedVCObject)
    {
        // if we create the handle to a boxed value type, then the type is
        // E_T_CLASS. m_type is the underlying value type. That is incorrect to
        // return.
        //
        *pType = ELEMENT_TYPE_CLASS;
        return S_OK;
    }

    return m_type->GetType(pType);
}   // GetType

HRESULT CordbHandleValue::GetSize(ULONG32 *pSize)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pSize, ULONG32 *);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    if (m_pHandle == NULL)
    {
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }

    //return the size of reference
    *pSize = m_size;
    return S_OK;
}   // GetSize


HRESULT CordbHandleValue::GetAddress(CORDB_ADDRESS *pAddress)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pAddress, CORDB_ADDRESS *);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    if (m_pHandle == NULL)
    {
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }

    *pAddress = LsPtrToCookie(m_pHandle);
    return S_OK;
}   // GetAddress


HRESULT CordbHandleValue::CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
{
    return E_NOTIMPL;
}   // CreateBreakpoint

HRESULT CordbHandleValue::IsNull(BOOL *pbNull)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pbNull, BOOL *);
    FAIL_IF_NEUTERED(this);    
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    HRESULT         hr = S_OK;

    *pbNull = FALSE;

    if (m_pHandle == NULL)
    {
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }


    // Only return true if handle is long weak handle and is disposed.
    if (m_handleType == HANDLE_WEAK_TRACK_RESURRECTION)
    {
        hr = RefreshHandleValue();
        if (FAILED(hr))
        {
            return hr;
        }

        if (m_info.objRef == NULL)
        {
            *pbNull = TRUE;
        }
    }
    else if (m_info.objRef == NULL)
    {
        *pbNull = TRUE;
    }

    // strong handle always return false for IsNull

    return S_OK;
}   // IsNull

HRESULT CordbHandleValue::GetValue(CORDB_ADDRESS *pValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pValue, CORDB_ADDRESS *);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    if (m_pHandle == NULL)
    {
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }

    RefreshHandleValue();
    *pValue = PTR_TO_CORDB_ADDRESS(m_info.objRef);
    return S_OK;
}   // CreateBreakpoint

HRESULT CordbHandleValue::SetValue(CORDB_ADDRESS value)
{
    // do not support SetValue on Handle
    return E_FAIL;
}   // GetValue

HRESULT CordbHandleValue::Dereference(ICorDebugValue **ppValue)
{
    HRESULT hr = S_OK;
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    FAIL_IF_NEUTERED(this);    
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());    
    _ASSERTE( m_appdomain != NULL && !m_appdomain->IsNeutered() );

    *ppValue = NULL;

    if (m_pHandle == NULL)
    {
        return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
    }

    hr = RefreshHandleValue();
    if (FAILED(hr))
    {
        return hr;
    }        

    if ((m_info.objRefBad) || (m_info.objRef == NULL))
    {
        return CORDBG_E_BAD_REFERENCE_VALUE;
    }

    return CordbReferenceValue::DereferenceCommon(
        m_appdomain, 
        m_type, 
        NULL, // don't support typed-by-refs
        &m_info, 
        NULL, // don't support caching
        ppValue);
    
}   // Dereference

HRESULT CordbHandleValue::DereferenceStrong(ICorDebugValue **ppValue)
{
    return E_NOTIMPL;
}


