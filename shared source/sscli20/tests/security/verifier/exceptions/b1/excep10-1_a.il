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
.module 'excep10-1.exe'
.assembly 'excep10-1' 
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    .entrypoint
    .locals init (class ['mscorlib']System.Exception e)
_try:
    ldstr       "Before throw"
    call        void ['mscorlib']System.Console::WriteLine(class ['mscorlib']System.String)
    newobj      instance void ['mscorlib']System.Exception::.ctor()
    throw

_try_end:
_filter:
    ldstr       "filter"
    call        void ['mscorlib']System.Console::WriteLine(class ['mscorlib']System.String)
    stloc       e
    ldc.i4      1
    endfilter
_handler:
    ldstr       "handler"
    call        void ['mscorlib']System.Console::WriteLine(class ['mscorlib']System.String)
    stloc       e
    leave.s     _ret
_handler_end:
_ret:
    ret
.try _filter to _try_end filter _filter handler _handler to _handler_end
}
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
}
