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

    return NULL;
}

/*
 * ProfileArgIterator::ProfileArgIterator
 *
 * Constructor.  Does almost nothing.  Init must be called after construction.
 *
 */
ProfileArgIterator::ProfileArgIterator(MetaSig *pMetaSig, void *platformSpecificHandle):
    m_argIterator(NULL,
                  pMetaSig,
                  0,
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
    WRAPPER_CONTRACT;
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
    WRAPPER_CONTRACT;
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
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

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
    return NULL;
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

    return NULL;
}



