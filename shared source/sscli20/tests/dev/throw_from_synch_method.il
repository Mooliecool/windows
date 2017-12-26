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
.assembly test {}

.class private auto ansi beforefieldinit Test
       extends [mscorlib]System.Object
{
  .method public hidebysig newslot virtual
          instance void  foo() cil managed synchronized
  {
    newobj     instance void ['mscorlib']System.Exception::.ctor()
    throw
    ret
  }

  .method public hidebysig newslot virtual
          instance void  foo_enter() cil managed 
  {
     .try {
      ldarg 0
      callvirt   instance void Test::foo()
      leave.s   Fail
    }
    catch ['mscorlib']System.Exception 
    {
        pop
        ldstr      "Inside catch [EXPECTED]"
        call       void ['mscorlib']System.Console::WriteLine(string)
        leave.s   Pass
    }  // end handler
    Fail:
     newobj     instance void ['mscorlib']System.Exception::.ctor()
     throw
    Pass:
     ret
  }
  .method private hidebysig static void  Main() cil managed
  {
    .entrypoint
    .locals init (class Test V_1, class [mscorlib]System.Threading.Thread V_2, int32 V_3)
    .try {
      newobj     instance void Test::.ctor()
      stloc.0
      ldloc.0
      callvirt   instance void Test::foo_enter()
      leave.s   SecondCall
    }
    catch ['mscorlib']System.Exception 
    {
        pop
        ldstr      "Inside catch [UNEXPECTED]"
        call       void ['mscorlib']System.Console::WriteLine(string)
        ldc.i4.1
        stloc      V_3
        leave.s   SecondCall
    }  // end handler

SecondCall:
    ldloc V_3
    brtrue EndIL
    .try {
        ldloc.0
        ldftn   instance void Test::foo_enter()
        newobj  instance void [mscorlib]System.Threading.ThreadStart::.ctor(object, native int)
        newobj     instance void [mscorlib]System.Threading.Thread::.ctor(class [mscorlib]System.Threading.ThreadStart)
        stloc.1
        ldloc.1
        callvirt   instance void [mscorlib]System.Threading.Thread::Start()
        ldloc.1
        callvirt   instance void [mscorlib]System.Threading.Thread::Join()
        leave.s    EndIL
      }
      catch ['mscorlib']System.Exception 
      {  
        pop
        ldstr      "Inside catch [UNEXPECTED]"
        call       void ['mscorlib']System.Console::WriteLine(string)
        ldc.i4.1
        stloc      V_3
        leave.s    EndIL
      }  // end handler
EndIL:
    ldloc  V_3 
    call       void [mscorlib]System.Environment::set_ExitCode(int32)
    ret
  }

  .method public hidebysig specialname rtspecialname
          instance void  .ctor() cil managed
  {
    ldarg.0
    call       instance void [mscorlib]System.Object::.ctor()
    ret
  }
}

