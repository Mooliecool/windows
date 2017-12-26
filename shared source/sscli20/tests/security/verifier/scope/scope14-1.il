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
//access public field of another value class in another il file, Should not get VE 

.module 'scope14-1.exe'

.assembly 'scope14-1' // as "scope14-1.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.assembly extern anotherstruct
{
  .hash = (0D F9 74 AF 52 47 BD E4 52 9A 46 FC BB 33 F9 31   // ..t.RG..R.F..3.1
           56 BB 42 FF )                                     // V.B.
  .ver 0:0:0:0
}
.class auto ansi TestClass extends [mscorlib]System.Object
{
  .method public hidebysig static void Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       9 (0x9)
    .maxstack  1
    .locals init (value class [anotherstruct]anotherStruct.CompilerSource[] V_0)

	ldc.i4.7
	call       void [mscorlib]System.Console::WriteLine(int32)

    IL_0000:  ldc.i4.s   32
    IL_0002:  newarr     [anotherstruct]anotherStruct.CompilerSource
    IL_0007:  stloc.0
    IL_0008:  ret
  } // end of method 'M::Main'

  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'M::.ctor'

} // end of class 'M'

//*********** DISASSEMBLY COMPLETE ***********************
