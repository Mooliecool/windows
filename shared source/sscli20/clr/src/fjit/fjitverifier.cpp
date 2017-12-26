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

// Pre-compiler header
#include "jitpch.h"

// Project header files
#include "fjit.h"

// Class header file
#include "fjitverifier.h"

SplitOffsetStack::SplitOffsetStack(int size )
{
  stack = new int[size];
  if (stack == NULL)
        RaiseException(SEH_NO_MEMORY,EXCEPTION_NONCONTINUABLE,0,NULL); 
  stack_size = size;
  num_offsets = 0;
}

SplitOffsetStack:: ~SplitOffsetStack()
{
  // The array must have been allocated
  _ASSERTE( stack != NULL );
  // Delete the array
  delete [] stack;
}

void SplitOffsetStack::pushOffset( unsigned int offset )
{
  if ( num_offsets+1 >= stack_size )
  {
    // Allocate a new array which is twice the size of the old one
    stack_size = 2*stack_size;
    int *  new_stack = new int[stack_size];
    if (new_stack == NULL)
        RaiseException(SEH_NO_MEMORY,EXCEPTION_NONCONTINUABLE,0,NULL); 
    // Copy stack entries into the new array
    for (int i = 0; i <num_offsets; i++ )
      new_stack[i] = stack[i];
    // Delete the old array and set the pointer to the new one 
    delete [] stack;
    stack = new_stack;
  }

  stack[num_offsets++] = offset;
}

int SplitOffsetStack::popOffset( )
{
  _ASSERTE( !isEmpty() && stack != NULL );
  num_offsets--;
  return stack[num_offsets];
}

void SplitOffsetStack::reset()
{
  // Reset the state
  num_offsets = 0; 
}

void SplitOffsetStack::putAt( unsigned int offset, int index  )
{
  // Make sure that the index is valid; throw an exception if it is not
  _ASSERTE( index <= num_offsets );
  if (index > num_offsets )
     { RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL); }

  pushOffset( offset );
  for (int i = num_offsets-1; i > index; i-- )
    stack[i] = stack[i-1];
  stack[index] = offset;
}

void SplitOffsetStack::putInFront( unsigned int offset, int index  )
{
  if ( index == num_offsets - 1)
    pushOffset( offset );
  else
    putAt(offset, index + 1 );
}

#if defined(_DEBUG)
void SplitOffsetStack::dumpStack()
{
  printf("Number Offsets %d  Size of Array Allocated %d\n", num_offsets, stack_size );
  for (int i = 0; i <num_offsets; i++ )
    printf("\tOffset %d: %x\n", i, stack[i] );
}
#endif
