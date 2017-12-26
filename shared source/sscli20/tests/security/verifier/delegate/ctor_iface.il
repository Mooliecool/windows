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
// ctor_iface.il
.assembly extern mscorlib { }
.assembly ctor_iface { }
.class private auto ansi sealed TestDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance int32
          Invoke() runtime managed { }
}
.class interface public abstract auto ansi TestInterface
{
  .method public hidebysig newslot virtual abstract 
          instance int32 Method() il managed { }
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
       implements TestInterface
{
  .method public hidebysig newslot final virtual 
          instance int32 Method() il managed
  {
    .maxstack  1
    .locals init (int32 V_0)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0004
    IL_0004:  ldloc.0
    IL_0005:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  3
    IL_0000:  ldc.i4.1
    IL_0001:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
    IL_0006:  newobj     instance void TestClass::.ctor()
    		  castclass TestInterface
    IL_000d:  dup
    IL_000e:  ldvirtftn  instance int32 TestInterface::Method()
    IL_0014:  newobj     instance void TestDelegate::.ctor(object, native unsigned int)
    IL_001b:  callvirt   instance int32 TestDelegate::Invoke()
    IL_0020:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
    IL_0025:  ret
  }
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  }
}
