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

// test interaction of exceptions and garbage collector

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:3300:0
}
.assembly excepgc1
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module excepgc1.exe
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001

// ============== CLASS STRUCTURE DECLARATION ==================
//
.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.MarshalByRefObject
{
} // end of class MainApp


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.MarshalByRefObject
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.MarshalByRefObject::.ctor()
    IL_0006:  ret
  } // end of method MainApp::.ctor

  .method public hidebysig instance void 
          Action() cil managed
  {
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
  } // end of method MainApp::Action

  .method public hidebysig static int32  Main() cil managed
  {
    .entrypoint
    .maxstack  2
    .locals init (object V_0)
    
    newobj     instance void [mscorlib]System.Object::.ctor()
    stloc.0    

_try:
     newobj     instance void MainApp::.ctor()
     call       instance void MainApp::Action()
     leave.s    _ret

_try_end:
_filter:
     dup
     callvirt   instance class [mscorlib]System.String [mscorlib]System.Object::ToString()     
     call       void [mscorlib]System.GC::Collect()
     call       void [mscorlib]System.Console::WriteLine(object)
     call       void [mscorlib]System.Console::WriteLine(object)
     ldc.i4      1
     endfilter

_handler:
     dup
     callvirt   instance class [mscorlib]System.String [mscorlib]System.Object::ToString()          
     call       void [mscorlib]System.GC::Collect()
     call       void [mscorlib]System.Console::WriteLine(object)
     call       void [mscorlib]System.Console::WriteLine(object)
     leave.s    _ret

_handler_end:
_ret:
     ldloc.0
     call       void [mscorlib]System.Console::WriteLine(object)     
     ldstr      "Passed"
     call       void [mscorlib]System.Console::WriteLine(string)     
     ldc.i4      0
     ret

.try _try to _try_end filter _filter handler _handler to _handler_end
  } // end of method MainApp::Main

} // end of class MainApp
