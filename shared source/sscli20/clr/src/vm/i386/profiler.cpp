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

#include "common.h"
#include "proftoeeinterfaceimpl.h"

//
// The following structure is the format on x86 builds of the data
// being passed in plaformSpecificHandle for ProfileEnter/Leave/Tailcall
//
typedef struct _PROFILE_PLATFORM_SPECIFIC_DATA
{
    FunctionID functionId;
    DWORD    doubleBuffer1;
    DWORD    doubleBuffer2;
    DWORD    floatBuffer;
    DWORD    floatingPointValuePresent;
    UINT_PTR eax; // eax and edx must be continuous in this structure to make getting 64 bit return values easier.
    UINT_PTR edx;
    UINT_PTR ecx;
    UINT_PTR esp;
    UINT_PTR ip;
} PROFILE_PLATFORM_SPECIFIC_DATA, *PPROFILE_PLATFORM_SPECIFIC_DATA;


/*
 * ProfileGetIPFromPlatformSpecificHandle
 *
 * This routine takes the platformSpecificHandle and retrieves from it the
 * IP value.
 *
 * Parameters:
 *    handle - the platformSpecificHandle passed to ProfileEnter/Leave/Tailcall
 *
 * Returns:
 *    The IP value stored in the handle.
 */
UINT_PTR ProfileGetIPFromPlatformSpecificHandle(void *handle)
{
    LEAF_CONTRACT;

    return ((PROFILE_PLATFORM_SPECIFIC_DATA *)handle)->ip;
}

#define PROFILE_FAKE_BASE_OFFSET sizeof(INT64)

/*
 * ProfileArgIterator::ProfileArgIterator
 *
 * Constructor.  Does almost nothing.  Init must be called after construction.
 *
 */
ProfileArgIterator::ProfileArgIterator(MetaSig *pMetaSig, void *platformSpecificHandle):
    //
    // We are doing something that works, but is using ArgIterators a little differently.
    //
    // ArgIterator expects the first parameter to the constructor to be the address on the stack
    // of a FramedMethodFrame.  It then uses the third parameter as the offset beyond the base
    // address of the FramedMethodFrame of where the stack begins.
    //
    // Then the final parameter is the offset of the register set within the FramedMethodFrame.
    //
    // Now we dont have a FramedMethodFrame, so we will fool the ArgIterator as follows:
    //   - For the address of FramedMethodFrame, we will use the *ending* esp stored in the platformSpecificHandle
    //   - Will will give a zero-offset for the stack
    //
    // This will allow us to get all the stack variables as follows:
    //   - Stack variables come out of the iterator normally.
    //
    // To get enregistered arguments, we have to do something even more bizarre.
    //   - When we are enumerating arguments, if the regStackOffset is not -1, then we know it is
    //     enregistered and we then compare that offset with the ones in the struct
    //     ArgumentRegisters.ECX or EDX.  We completely ignore the total offset.
    //
    //
    // One last twist.  Since the ArgIterator returns zero to indicate that there are no more arguments, we
    // cannot use zero as the base offset, otw the last parameter always returns an offset of zero, which is
    // indistinquishable from the ending case. So, we put a fake offset and subtract that off later.
    //
    m_argIterator((LPBYTE)(((PROFILE_PLATFORM_SPECIFIC_DATA *)platformSpecificHandle)->esp),
                   pMetaSig,
                   PROFILE_FAKE_BASE_OFFSET,
                   0xFFFFFFFF)

{
    LEAF_CONTRACT;

    m_handle = platformSpecificHandle;
    m_count = 0;
}

/*
 * ProfileArgIterator::~ProfileArgIterator
 *
 * Destructor, releases all resources.
 *
 */
ProfileArgIterator::~ProfileArgIterator()
{
    LEAF_CONTRACT;
}


/*
 * ProfileArgIterator::Init
 *
 * This routine initializes for arg iteration.  It is not expected to be called
 * more than once at this time, but could me made to do so later.
 *
 * Parameters:
 *    pMetaSig - The signature of the method we are going iterate over
 *    platformSpecificHandle - the value passed to ProfileEnter/Leave/Tailcall
 *
 * Returns:
 *    None.
 */
void ProfileArgIterator::Init(MetaSig *pMetaSig, void *platformSpecificHandle)
{
    //
    // It would be really nice to contract this, but the underlying functions are convolutedly
    // contracted.  Basically everything should be loaded by the time the profiler gets a call
    // back, so everything is NOTHROW/NOTRIGGER, but there is not mechanism for saying that the
    // contracts in called functions should be for the best case, not the worst case, now.
    //
    WRAPPER_CONTRACT;

    if (pMetaSig == NULL)
    {
        _ASSERTE(!"Null is not a valid parameter");
        return;
    }

    //
    // Replace the handle value if necessary
    //
    if (m_handle != platformSpecificHandle)
    {
        _ASSERTE(platformSpecificHandle != NULL);
        m_handle = platformSpecificHandle;
    }

    PROFILE_PLATFORM_SPECIFIC_DATA *pData = (PROFILE_PLATFORM_SPECIFIC_DATA *)m_handle;

    m_argIterator = ArgIterator((LPBYTE)(pData->esp), pMetaSig, PROFILE_FAKE_BASE_OFFSET, 0xFFFFFFFF);
    m_count = 0;
}

/*
 * ProfileArgIterator::GetNextArgAddr
 *
 * After initialization, this method is called repeatedly until it
 * returns NULL to get the address of each arg.  Note: this address
 * could be anywhere on the stack.
 *
 * Parameters:
 *    pType is the IMAGE_CEE_CS_* type.
 *    pStructSize is the struct size, if appropriate.
 *
 * Returns:
 *    Address of the argument, or NULL if iteration is complete.
 *    Fills in pType and pStructSize if non-NULL return value.
 */
LPVOID ProfileArgIterator::GetNextArgAddr(BYTE *pType, UINT32 *pStructSize)
{
    //
    // It would be really nice to contract this, but the underlying functions are convolutedly
    // contracted.  Basically everything should be loaded by the time the profiler gets a call
    // back, so everything is NOTHROW/NOTRIGGER, but there is not mechanism for saying that the
    // contracts in called functions should be for the best case, not the worst case, now.
    //
    WRAPPER_CONTRACT;

    UINT regStructOfs = (UINT)-1;

    int argOffset = m_argIterator.GetNextOffset(pType, pStructSize, &regStructOfs);

    //
    // Value is enregistered, figure out where and return that.
    //
    PROFILE_PLATFORM_SPECIFIC_DATA *pData = (PROFILE_PLATFORM_SPECIFIC_DATA *)m_handle;

    //
    // Zero indicates the end of the args.
    //
    if (argOffset == 0)
    {
        return NULL;
    }

    if (pData == NULL)
    {
        _ASSERTE(!"Why do we have a NULL data pointer here?");
        return NULL;
    }

    //
    // If this is not enregistered, return the value
    //
    if (regStructOfs == (UINT)-1)
    {
        m_count++;
        return ((LPBYTE)pData->esp) + argOffset - PROFILE_FAKE_BASE_OFFSET;
    }

    switch (regStructOfs)
    {
    case offsetof(ArgumentRegisters, ECX):
        m_count++;
        return &(pData->ecx);
    case offsetof(ArgumentRegisters, EDX):
        m_count++;
        return &(pData->edx);
    }

    _ASSERTE(!"Arg is an unsaved register!");
    return NULL;
}

/*
 * ProfileArgIterator::GetHiddenArgValue
 *
 * Called after initialization, any number of times, to retrieve any
 * hidden argument, so that resolution for Generics can be done.
 *
 * Parameters:
 *    None.
 *
 * Returns:
 *    Value of the hidden parameter, or NULL if none exists.
 */
LPVOID ProfileArgIterator::GetHiddenArgValue(void)
{
    //
    // It would be really nice to contract this, but the underlying functions are convolutedly
    // contracted.  Basically everything should be loaded by the time the profiler gets a call
    // back, so everything is NOTHROW/NOTRIGGER, but there is not mechanism for saying that the
    // contracts in called functions should be for the best case, not the worst case, now.
    //
    WRAPPER_CONTRACT;

    PROFILE_PLATFORM_SPECIFIC_DATA *pData = (PROFILE_PLATFORM_SPECIFIC_DATA *)m_handle;

    MethodDesc *pMethodDesc = FunctionIdToMethodDesc(pData->functionId);

    if (!pMethodDesc->IsSharedByGenericInstantiations())
    {
        return NULL;
    }

    if (!pMethodDesc->RequiresInstMethodTableArg() &&
        !pMethodDesc->RequiresInstMethodDescArg() &&
        !pMethodDesc->AcquiresInstMethodTableFromThis())
    {
        return NULL;
    }

    //
    // The ArgIterator::GetParamTypeOffset() can only be called after calling GetNextOffset until the
    // entire signature has been walked, but *before* GetNextOffset returns 0 - indicating the end.
    //

    //
    // Start over from the beginning.
    //
    m_argIterator = ArgIterator((LPBYTE)(pData->esp), m_argIterator.GetSig(), PROFILE_FAKE_BASE_OFFSET, 0xFFFFFFFF);

    BYTE dummyType;
    UINT32 dummySize;
    UINT32 count = 0;

    UINT32 cArguments = m_argIterator.GetSig()->NumFixedArgs();

    for (count = 0; count < cArguments; count++)
    {
        m_argIterator.GetNextArgAddr();
    }

    //
    // Now get the offset of the hidden arg
    //
    INT regStructOfs = -1;
    int argOffset = m_argIterator.GetParamTypeArgOffset(&regStructOfs);

    //
    // Now restore where we were
    //
    m_argIterator = ArgIterator((LPBYTE)(pData->esp), m_argIterator.GetSig(), PROFILE_FAKE_BASE_OFFSET, 0xFFFFFFFF);

    count = 0;
    while (count < m_count)
    {
        m_argIterator.GetNextOffset(&dummyType, &dummySize, NULL);
        count++;
    }

    if (argOffset == 0)
    {
        return NULL;
    }

    //
    // If this is not enregistered, return the value
    //
    if (regStructOfs == -1)
    {
        return *(LPVOID *)(((LPBYTE)pData->esp) + argOffset - PROFILE_FAKE_BASE_OFFSET);
    }

    switch (regStructOfs)
    {
    case offsetof(ArgumentRegisters, ECX):
        return (LPVOID)(pData->ecx);
    case offsetof(ArgumentRegisters, EDX):
        return (LPVOID)(pData->edx);
    }

    _ASSERTE(!"Arg is an unsaved register!");
    return NULL;
}

/*
 * ProfileArgIterator::GetThis
 *
 * Called after initialization, any number of times, to retrieve the
 * value of 'this'.
 *
 * Parameters:
 *    None.
 *
 * Returns:
 *    value of the 'this' parameter, or NULL if none exists.
 */
LPVOID ProfileArgIterator::GetThis(void)
{
    //
    // It would be really nice to contract this, but the underlying functions are convolutedly
    // contracted.  Basically everything should be loaded by the time the profiler gets a call
    // back, so everything is NOTHROW/NOTRIGGER, but there is not mechanism for saying that the
    // contracts in called functions should be for the best case, not the worst case, now.
    //
    WRAPPER_CONTRACT;

    PROFILE_PLATFORM_SPECIFIC_DATA *pData = (PROFILE_PLATFORM_SPECIFIC_DATA *)m_handle;

    if (pData->ip == 0)
    {
        return NULL;
    }

    _ASSERTE(NULL != m_argIterator.GetSig()); // requires pSig

    if (!m_argIterator.GetSig()->HasThis())
    {
        return NULL;
    }

    MethodDesc *pMethodDesc = FunctionIdToMethodDesc(pData->functionId);

    if ((pMethodDesc == NULL) || pMethodDesc->IsStatic())
    {
        return NULL;
    }

    LPBYTE loc = (BYTE *)(pData->esp);

#if defined(_X86_)

    switch (offsetof(ArgumentRegisters, THIS_REG))
    {
    case offsetof(ArgumentRegisters, ECX):
        loc = (BYTE *)&(pData->ecx);
        break;

    case offsetof(ArgumentRegisters, EDX):
        loc = (BYTE *)&(pData->edx);
        break;

    default:
        _ASSERTE(!"This is an unsaved register!");
        return NULL;
    }

#endif

#if THISPTR_LOCATION > 0
    if (m_argIterator.GetSig()->HasRetBuffArg())
    {
        // AMD64 always has the return buffer argument first, even before the this pointer
#if defined(_X86_)
        switch (offsetof(ArgumentRegisters, THIS_REG))
        {
         case offsetof(ArgumentRegisters, ECX):
             loc = (BYTE *)&(pData->edx);
             break;
         default:
             _ASSERTE(!"This is an unsaved register!");
             return NULL;
         }
#else
        loc += sizeof(void *);
#endif
    }
#endif

    return *(LPVOID*) loc;

}



/*
 * ProfileArgIterator::GetReturnBufferAddr
 *
 * Called after initialization, any number of times, to retrieve the
 * address of the return buffer.  NULL indicates no return value.
 *
 * Parameters:
 *    None.
 *
 * Returns:
 *    Address of the return buffer, or NULL if none exists.
 */
LPVOID ProfileArgIterator::GetReturnBufferAddr(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    PROFILE_PLATFORM_SPECIFIC_DATA *pData = (PROFILE_PLATFORM_SPECIFIC_DATA *)m_handle;

    switch (m_argIterator.GetSig()->GetReturnType())
    {
    case ELEMENT_TYPE_R8:
        _ASSERTE(pData->floatingPointValuePresent);
        return (void *)(&(pData->doubleBuffer1));

    case ELEMENT_TYPE_R4:
        _ASSERTE(pData->floatingPointValuePresent);
        return (void *)(&(pData->floatBuffer));

    default:
        if (m_argIterator.GetSig()->GetReturnTypeNormalized() == ELEMENT_TYPE_VALUETYPE)
        {
            return (void *)(pData->esp);
        }
        return &(pData->eax);
    }

    _ASSERTE(!"SHOULD NOT REACH HERE!");
}


