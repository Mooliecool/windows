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

.assembly extern mscorlib {}

.module 'anotherstruct.dll'

.assembly 'anotherstruct' // as "anotherstruct.dll"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.namespace anotherStruct
{
  .class value public auto ansi sealed CompilerSource extends [mscorlib]System.ValueType
  {
    .field public int32 i
    .field public int32 j
  } // end of class 'CompilerSource'

} // end of namespace 'anotherStruct'

//*********** DISASSEMBLY COMPLETE ***********************
