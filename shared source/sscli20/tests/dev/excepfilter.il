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
//try start >= try end, should get VE
.module 'excepfilter.exe'
.assembly 'excepfilter' 
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
.method private hidebysig static void check(object o) cil managed
{
    .maxstack  2
    ldarg.0
    call       void [mscorlib]System.Console::WriteLine(object)
    ldarg.0
    callvirt   instance class [mscorlib]System.Type [mscorlib]System.Object::GetType()
    ldtoken    [mscorlib]System.Exception
    call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    beq.s      good
    ldc.i4.1
    call       void [mscorlib]System.Environment::Exit(int32)
good:  ret
}

.method public hidebysig static void Main() cil managed
{
    .entrypoint
_try:
    ldstr       "Before throw"
    call        void ['mscorlib']System.Console::WriteLine(class ['mscorlib']System.String)
    newobj      instance void ['mscorlib']System.Exception::.ctor()
    throw

_try_end:
_filter:
    call       void TestClass::check(object)
    ldc.i4      1
    endfilter
_handler:
    call       void TestClass::check(object)
    leave.s     _ret
_handler_end:
_ret:
    ret
.try _try to _try_end filter _filter handler _handler to _handler_end
}
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
}
