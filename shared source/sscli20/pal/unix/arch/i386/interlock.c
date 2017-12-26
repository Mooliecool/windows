/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    interlock.c

Abstract:

    Implementation of Interlocked functions for the Intel x86
    platform. These functions are processor dependent.

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"


SET_DEFAULT_DEBUG_CHANNEL(SYNC);



/*++
Function:
  InterlockedIncrement

The InterlockedIncrement function increments (increases by one) the
value of the specified variable and checks the resulting value. The
function prevents more than one thread from using the same variable
simultaneously.

Parameters

lpAddend 
       [in/out] Pointer to the variable to increment. 

Return Values

The return value is the resulting incremented value. 

--*/
LONG
PALAPI
InterlockedIncrement(
             IN OUT LONG volatile *lpAddend)
{
    LONG RetValue;

    __asm__ __volatile__(
             "lock; xaddl %0,(%1)"
             : "=r" (RetValue)
             : "r" (lpAddend), "0" (1)
             : "memory"
             );

    return RetValue + 1;
}


/*++
Function:
  InterlockedDecrement

The InterlockedDecrement function decrements (decreases by one) the
value of the specified variable and checks the resulting value. The
function prevents more than one thread from using the same variable
simultaneously.

Parameters

lpAddend 
       [in/out] Pointer to the variable to decrement. 

Return Values

The return value is the resulting decremented value.

--*/
LONG
PALAPI
InterlockedDecrement(
             IN OUT LONG volatile *lpAddend)
{
    LONG RetValue;

    __asm__ __volatile__(
             "lock; xaddl %0,(%1)"
             : "=r" (RetValue)
             : "r" (lpAddend), "0" (-1)
             : "memory"
             );

    return RetValue - 1;
}


/*++
Function:
  InterlockedExchange

The InterlockedExchange function atomically exchanges a pair of
values. The function prevents more than one thread from using the same
variable simultaneously.

Parameters

Target 
       [in/out] Pointer to the value to exchange. The function sets
       this variable to Value, and returns its prior value.
Value 
       [in] Specifies a new value for the variable pointed to by Target. 

Return Values

The function returns the initial value pointed to by Target. 

--*/
LONG
PALAPI
InterlockedExchange(
            IN OUT LONG volatile *Target,
            IN LONG Value)
{
    LONG result;

    __asm__ __volatile__(
             "lock; xchgl %0,(%1)"
             : "=r" (result)
             : "r" (Target), "0" (Value)
             : "memory" 
             );
    return result;
}


/*++
Function:
  InterlockedCompareExchange

The InterlockedCompareExchange function performs an atomic comparison
of the specified values and exchanges the values, based on the outcome
of the comparison. The function prevents more than one thread from
using the same variable simultaneously.

If you are exchanging pointer values, this function has been
superseded by the InterlockedCompareExchangePointer function.

Parameters

Destination     [in/out] Specifies the address of the destination value. The sign is ignored. 
Exchange        [in]     Specifies the exchange value. The sign is ignored. 
Comperand       [in]     Specifies the value to compare to Destination. The sign is ignored. 

Return Values

The return value is the initial value of the destination.

--*/
LONG
PALAPI
InterlockedCompareExchange(
               IN OUT LONG volatile *Destination,
               IN LONG Exchange,
               IN LONG Comperand)
{
    LONG result;
 
    __asm__ __volatile__(
             "lock; cmpxchgl %2,(%1)"
             : "=a" (result)
             : "r" (Destination), "r" (Exchange), "0" (Comperand)
             : "memory"
             );
 
    return result;
}

void
PALAPI
MemoryBarrier(
    VOID)
{
    LONG Barrier;

    __asm__ __volatile__(
        "xchg %%eax, %0"
        :
        : "m" (Barrier)
        : "memory", "eax");
}

void
PALAPI
YieldProcessor(
    VOID)
{
    __asm__ __volatile__ (
        "rep\n"
        "nop"
    );
}
