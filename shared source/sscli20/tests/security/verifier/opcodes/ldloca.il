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
.assembly extern mscorlib { }
.assembly 'ldloca' { }
.class public auto ansi beforefieldinit Test
       extends [mscorlib]System.Object
{
  .method public hidebysig static void Negative1() cil managed
  {
    .maxstack  8
    ldloca.s 0
    pop
    ret
  }
  .method public hidebysig static void Negative2() cil managed
  {
    .maxstack  8
    ldloca.s 0
    pop
    ret
  }
  .method public hidebysig static void Negative3() cil managed
  {
    .maxstack  8
    ldloca 0
    pop
    ret
  }
  .method public hidebysig static void Negative4() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca.s 3
    pop
    ret
  }
  .method public hidebysig static void Negative5() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca.s 3
    pop
    ret
  }
  .method public hidebysig static void Negative6() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca 3
    pop
    ret
  }
  .method public hidebysig static void Negative7() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca 0xffffffff
    pop
    ret
  }
  .method public hidebysig static void Negative8() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca 0x7fffffff
    pop
    ret
  }
  .method public hidebysig static void Negative9() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca 0x000ffff
    pop
    ret
  }
  .method public hidebysig static void Negative10() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca -1
    pop
    ret
  }
  .method public hidebysig static void Negative11() cil managed
  {
    .maxstack  8
    .locals init (int32 arg1, object arg2, object arg3)
    .zeroinit
    ldloca.s 255
    pop
    ret
  }
  .method public hidebysig static void Negative12() cil managed
  {
    .maxstack  8
    .locals init (
			int32 arg1, int32 arg2, int32 arg3, int32 arg4, int32 arg5, int32 arg6, int32 arg7, int32 arg8, int32 arg9, int32 arg10, int32 arg11, int32 arg12, int32 arg13, int32 arg14, int32 arg15, int32 arg16, 
			int32 arg17, int32 arg18, int32 arg19, int32 arg20, int32 arg21, int32 arg22, int32 arg23, int32 arg24, int32 arg25, int32 arg26, int32 arg27, int32 arg28, int32 arg29, int32 arg30, int32 arg31, int32 arg32, 
			int32 arg33, int32 arg34, int32 arg35, int32 arg36, int32 arg37, int32 arg38, int32 arg39, int32 arg40, int32 arg41, int32 arg42, int32 arg43, int32 arg44, int32 arg45, int32 arg46, int32 arg47, int32 arg48, 
			int32 arg49, int32 arg50, int32 arg51, int32 arg52, int32 arg53, int32 arg54, int32 arg55, int32 arg56, int32 arg57, int32 arg58, int32 arg59, int32 arg60, int32 arg61, int32 arg62, int32 arg63, int32 arg64, 
			int32 arg65, int32 arg66, int32 arg67, int32 arg68, int32 arg69, int32 arg70, int32 arg71, int32 arg72, int32 arg73, int32 arg74, int32 arg75, int32 arg76, int32 arg77, int32 arg78, int32 arg79, int32 arg80, 
			int32 arg81, int32 arg82, int32 arg83, int32 arg84, int32 arg85, int32 arg86, int32 arg87, int32 arg88, int32 arg89, int32 arg90, int32 arg91, int32 arg92, int32 arg93, int32 arg94, int32 arg95, int32 arg96, 
			int32 arg97, int32 arg98, int32 arg99, int32 arg100, int32 arg101, int32 arg102, int32 arg103, int32 arg104, int32 arg105, int32 arg106, int32 arg107, int32 arg108, int32 arg109, int32 arg110, int32 arg111, int32 arg112, 
			int32 arg113, int32 arg114, int32 arg115, int32 arg116, int32 arg117, int32 arg118, int32 arg119, int32 arg120, int32 arg121, int32 arg122, int32 arg123, int32 arg124, int32 arg125, int32 arg126, int32 arg127, int32 arg128, 
			int32 arg129, int32 arg130, int32 arg131, int32 arg132, int32 arg133, int32 arg134, int32 arg135, int32 arg136, int32 arg137, int32 arg138, int32 arg139, int32 arg140, int32 arg141, int32 arg142, int32 arg143, int32 arg144, 
			int32 arg145, int32 arg146, int32 arg147, int32 arg148, int32 arg149, int32 arg150, int32 arg151, int32 arg152, int32 arg153, int32 arg154, int32 arg155, int32 arg156, int32 arg157, int32 arg158, int32 arg159, int32 arg160, 
			int32 arg161, int32 arg162, int32 arg163, int32 arg164, int32 arg165, int32 arg166, int32 arg167, int32 arg168, int32 arg169, int32 arg170, int32 arg171, int32 arg172, int32 arg173, int32 arg174, int32 arg175, int32 arg176, 
			int32 arg177, int32 arg178, int32 arg179, int32 arg180, int32 arg181, int32 arg182, int32 arg183, int32 arg184, int32 arg185, int32 arg186, int32 arg187, int32 arg188, int32 arg189, int32 arg190, int32 arg191, int32 arg192, 
			int32 arg193, int32 arg194, int32 arg195, int32 arg196, int32 arg197, int32 arg198, int32 arg199, int32 arg200, int32 arg201, int32 arg202, int32 arg203, int32 arg204, int32 arg205, int32 arg206, int32 arg207, int32 arg208, 
			int32 arg209, int32 arg210, int32 arg211, int32 arg212, int32 arg213, int32 arg214, int32 arg215, int32 arg216, int32 arg217, int32 arg218, int32 arg219, int32 arg220, int32 arg221, int32 arg222, int32 arg223, int32 arg224, 
			int32 arg225, int32 arg226, int32 arg227, int32 arg228, int32 arg229, int32 arg230, int32 arg231, int32 arg232, int32 arg233, int32 arg234, int32 arg235, int32 arg236, int32 arg237, int32 arg238, int32 arg239, int32 arg240, 
			int32 arg241, int32 arg242, int32 arg243, int32 arg244, int32 arg245, int32 arg246, int32 arg247, int32 arg248, int32 arg249, int32 arg250, int32 arg251, int32 arg252, int32 arg253, int32 arg254, int32 arg255, int32 arg256
  			)
  	.zeroinit
    ldloca 256
    pop
    ret
  }
  .method public hidebysig static int32 Main() cil managed
  {
    .entrypoint
    .maxstack  4
    .try {
      		call       void Test::Negative1()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_1
    }
NEXT_1:
    .try {
      		call       void Test::Negative2()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_2
    }
NEXT_2:
    .try {
      		call       void Test::Negative3()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_3
    }
NEXT_3:
    .try {
    		ldc.i4.0
    		ldnull
    		dup
      		call       void Test::Negative4()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_4
    }
NEXT_4:
    .try {
    		ldc.i4.0
    		ldnull
    		dup
      		call       void Test::Negative5()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_5
    }
NEXT_5:
    .try {
    		ldnull		
    		ldc.i4.0
    		ldnull
    		dup
      		call       void Test::Negative6()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		ldnull		
      		leave.s    NEXT_6
    }
NEXT_6:
    .try {
    		ldnull
    		ldc.i4.0
    		ldnull
      		call       void Test::Negative7()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException{
      		pop
      		leave.s    NEXT_7
    }
NEXT_7:
    .try {
    		ldnull
    		ldc.i4.0
    		ldnull
      		call       void Test::Negative8()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_8
    }
NEXT_8:
    .try {
    		ldnull
    		ldc.i4.0
    		ldnull
      		call       void Test::Negative9()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_9
    }
NEXT_9:
    .try {
    		ldnull
    		ldc.i4.0
    		ldnull
      		call       void Test::Negative10()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_10
    }
NEXT_10:
    .try {
    		ldnull
    		ldc.i4.0
    		ldnull
      		call       void Test::Negative11()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_11
    }
NEXT_11:
    .try {
      		call       void Test::Negative12()
      		leave    FAIL
    } catch [mscorlib]System.InvalidProgramException {
      		pop
      		leave.s    NEXT_12
    }
NEXT_12:
	ldc.i4.s   0
	ldstr      "PASSED"
	br.s RET
FAIL:
	ldc.i4.s   1
	ldstr      "FAILED"
RET:
	call       void [mscorlib]System.Console::WriteLine(string)
	ret
  } 
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() cil managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } 
} 
