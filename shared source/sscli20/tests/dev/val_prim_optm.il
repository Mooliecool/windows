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
.assembly 'ldelem' { }

.class private sequential ansi sealed beforefieldinit A
       extends [mscorlib]System.ValueType
{
  .field public int32 a
} // end of class A

.class private sequential ansi sealed beforefieldinit B
       extends [mscorlib]System.ValueType
{
  .field public unsigned int32 a
} // end of class B

.class private sequential ansi sealed beforefieldinit C
       extends [mscorlib]System.ValueType
{
  .field public valuetype A a
} // end of class C

.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {
.method public hidebysig instance void Positive_ldloc_stloc() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloc.0
        stloc.0
        ldloc.1
        stloc.1 
	ret
}
.method public hidebysig instance void Positive_ldloc_stobj() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldloc.0
        stobj      A
        ldloca.s   V_1
        ldloc.1
        stobj      B 
	ret
}
.method public hidebysig instance void Negative_ldloca_initobj() cil managed
{
        .maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    A
	ret
}
.method public hidebysig instance void Negative_ldloc_stloc() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloc.1
        stloc.0
	ret
}
.method public hidebysig instance void Negative_ldloc_stobj_1() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldloc.1
        stobj      A
	ret
}
.method public hidebysig instance void Negative_ldloc_stobj_2() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldloc.0
        stobj      B
	ret
}
.method public hidebysig instance void Negative_ldloc_stobj_3() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldloc.1
        stobj      A
	ret
}
.method public hidebysig instance void Negative_ldloc_stobj_4() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_1
        ldloc.0
        stobj      A
	ret
}
.method public hidebysig instance void Negative_ldobj_stloc_1() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldobj      A
        stloc.1
	ret
}
.method public hidebysig instance void Negative_ldobj_stloc_2() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_1
        ldobj      A
        stloc.0
	ret
}
.method public hidebysig instance void Negative_ldobj_stloc_3() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldobj      B
        stloc.0
	ret
}
.method public hidebysig instance void Negative_ldarg_stloc(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldarg.1
        stloc.0
	ret
}
.method public hidebysig instance void Negative_ldloc_starg(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldloc.0
        starg.s    1
	ret
}
.method public hidebysig instance void Positive_ldloc_starg(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype B V_0)
	ldloca.s   V_0
        initobj    B
        ldloc.0
        starg.s    1
	ret
}
.method public hidebysig instance void Positive_ldarg_stloc(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype B V_0)
	ldloca.s   V_0
        initobj    B
        ldarg.1
        stloc.0
	ret
}
.method public hidebysig instance void Negative_ldobj_starg(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_0
        ldobj      A
        starg.s    1
	ret
}
.method public hidebysig instance void Negative_ldarg_stobj(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_0
        ldarg.1
        stobj      A
	ret
}
.method public hidebysig instance void Negative_ldarga_stobj_1(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldarga.s   1
        ldloc.0
        stobj      A
	ret
}
.method public hidebysig instance void Negative_ldarga_stobj_2(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldarga.s   1
        ldloc.0
        stobj      B
	ret
}
.method public hidebysig instance void Negative_ldarga_ldobj_stloc_1(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldarga.s   1
        ldobj      A
        stloc.0
	ret
}
.method public hidebysig instance void Negative_ldarga_ldobj_stloc_2(valuetype B A_0) cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldarga.s   1
        ldobj      B
        stloc.0
	ret
}
.method public hidebysig instance void Negative_ldarga_ldind(valuetype B A_0) cil managed
{
	.maxstack  8
        ldarga.s   1
        ldind.i4
        pop
	ret
}
.method public hidebysig instance void Negative_ldarga_stind(valuetype B A_0) cil managed
{
	.maxstack  8
        ldarga.s   1
        ldc.i4     10
        stind.i4
	ret
}
.method public hidebysig instance void Negative_ldelem_stloc_1() cil managed
{
	.maxstack  8
        .locals init (valuetype A V_0)
	ldc.i4 100
	newarr valuetype B
	ldc.i4 10
	ldelem A
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ldelem_stloc_2() cil managed
{
	.maxstack  8
        .locals init (int32 V_0)
	ldc.i4 100
	newarr valuetype A
	ldc.i4 10
	ldelem A
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ldloc_stelem_1() cil managed
{
	.maxstack  8
        .locals init (valuetype B V_0)
	ldc.i4 100
	newarr valuetype A
	ldc.i4 10
        ldloc.0
	stelem A
	ret
}
.method public hidebysig instance void Negative_ldloc_stelem_2() cil managed
{
	.maxstack  8
        .locals init (int32 V_0)
	ldc.i4 100
	newarr valuetype A
	ldc.i4 10
        ldloc.0
	stelem A
	ret
}
.method public hidebysig instance void Negative_ldelem_stelem_1() cil managed
{
	.maxstack  8
        ldc.i4 100
	newarr valuetype B
        ldc.i4 10
        ldc.i4 100
	newarr valuetype A
	ldc.i4 10
	ldelem A
	stelem A
	ret
}
.method public hidebysig instance void Positive_ldelem_stloc() cil managed
{
	.maxstack  8
        .locals init (valuetype A V_0)
	ldc.i4 100
	newarr valuetype A
	ldc.i4 10
	ldelem A
	stloc.0
	ret
}
.method public hidebysig instance void Positive_ldloc_stelem() cil managed
{
	.maxstack  8
        .locals init (valuetype A V_0)
	ldc.i4 100
	newarr valuetype A
	ldc.i4 10
        ldloc.0
	stelem A
	ret
}
.method public hidebysig instance void Negative_ldelem_stelem_2() cil managed
{
	.maxstack  8
        ldc.i4 100
	newarr valuetype B
        ldc.i4 10
        ldc.i4 100
	newarr valuetype A
	ldc.i4 10
	ldelem A
	stelem B
	ret
}
.method public hidebysig instance void Negative_ldelema_stloc_1() cil managed
{
        .maxstack  8
        .locals init (valuetype A V_0)
	ldc.i4 100
	newarr valuetype B
        ldc.i4 10
        ldelema A
        ldobj A
        stloc.0
        ret
}
.method public hidebysig instance void Negative_ldelema_stloc_2() cil managed
{
	.maxstack  8
        .locals init (valuetype A V_0)
	ldc.i4 100
	newarr valuetype B
        ldc.i4 10
        ldelema B
        ldobj A
        stloc.0
        ret
}
.method public hidebysig instance void Negative_ldelema_stloc_3() cil managed
{
	.maxstack  8
        .locals init (int32 V_0)
	ldc.i4 100
	newarr valuetype B
        ldc.i4 10
        ldelema int32
        ldind.i4
        stloc.0
        ret
}
.method public hidebysig instance void Negative_ldelema_stobj_1() cil managed
{
	.maxstack  8
	ldc.i4 100
	newarr valuetype B
        ldc.i4 10
        ldelema B
        ldc.i4 10
        stobj B
        ret
}
.method public hidebysig instance void Negative_ldelema_stobj_2() cil managed
{
	.maxstack  8
	ldc.i4 100
	newarr valuetype B
        ldc.i4 10
        ldelema B
        ldc.i4 10
        stobj int32
        ret
}
.method public hidebysig instance void Negative_ldflda_stloc_1() cil managed
{
	
        .maxstack  8
	.locals init (valuetype C V_0,
                      valuetype B V_1,
                      valuetype A V_2)
	ldloca.s   V_0
        initobj    C
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_2
        initobj    A
        ldloca.s   V_0
        ldflda     valuetype A C::a
        ldloc.1    
        stobj      A
        ret
}
.method public hidebysig instance void Negative_ldflda_stloc_2() cil managed
{
	.maxstack  8
	.locals init (valuetype C V_0,
                      valuetype B V_1,
                      valuetype A V_2)
	ldloca.s   V_0
        initobj    C
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_2
        initobj    A
        ldloca.s   V_0
        ldflda     valuetype A C::a
        ldc.i4     10    
        stobj      A
        ret
}
.method public hidebysig instance void Negative_ldfld_stloc_1() cil managed
{
	.maxstack  8
	.locals init (valuetype C V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    C
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldfld      valuetype A C::a
        stloc.1
        ret
}
.method public hidebysig instance void Negative_ldfld_stloc_2() cil managed
{
	.maxstack  8
	.locals init (valuetype C V_0,
                      int32 V_1)
	ldloca.s   V_0
        initobj    C       
        ldloca.s   V_0
        ldfld      valuetype A C::a
        stloc.1
        ret
}
.method public hidebysig instance void Negative_ldloc_stfld_1() cil managed
{
	.maxstack  8
	.locals init (valuetype C V_0,
                      valuetype B V_1)
	ldloca.s   V_0
        initobj    C
        ldloca.s   V_1
        initobj    B
        ldloca.s   V_0
        ldloc.1
        stfld      valuetype A C::a
        ret
}
.method public hidebysig instance void Positive_ldflda_stind() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldloca.s   V_0
        ldflda     int32 A::a
        ldc.i4     10
        stind.i4
        ret
}
.method public hidebysig instance void Positive_ldfld_stloc() cil managed
{
	.maxstack  8
	.locals init (valuetype C V_0,
                      valuetype A V_1)
	ldloca.s   V_0
        initobj    C
        ldloca.s   V_1
        initobj    A
        ldloca.s   V_0
        ldfld      valuetype A C::a
        stloc.1
        ret
}
.method public hidebysig instance void Negative_ldloc_stfld_2() cil managed
{
	.maxstack  8
	.locals init (valuetype C V_0,
                      int32 V_1)
	ldloca.s   V_0
        initobj    C
        ldloca.s   V_0
        ldloc.1
        stfld      valuetype A C::a
        ret
}
.method public hidebysig instance void Negative_ldloc_stfld_3() cil managed
{
	.maxstack  8
	.locals init (valuetype C V_0,
                      int32 V_1)
	ldloca.s   V_0
        initobj    C
        ldloca.s   V_0
        ldloc.1
        stfld      int32 C::a
        ret
}

.method public hidebysig instance void Negative_ldloc_mul() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldloc.0
        ldloc.0
        mul
        pop
        ret
}

.method public hidebysig instance void Negative_ldloc_neg() cil managed
{
	.maxstack  8
	.locals init (valuetype A V_0)
	ldloca.s   V_0
        initobj    A
        ldloc.0
        neg
        pop
        ret
}

.method public hidebysig static int32 Main() cil managed
{
	.entrypoint
	.maxstack 8
	.locals init (class Test V_0, 
                      valuetype A V_1,
                      valuetype B V_2)
	newobj     instance void Test::.ctor()
	stloc.0
	ldloc.0
	callvirt instance void Test::Positive_ldloc_stloc()
IL_NEXT_1:
	ldloc.0
	callvirt instance void Test::Positive_ldloc_stobj()
IL_NEXT_2:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloca_initobj()
		ldstr "Negative_ldloca_initobj() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_3
	}
IL_NEXT_3:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stloc()
		ldstr "Negative_ldloc_stloc() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_4
	}
IL_NEXT_4:
        .try {
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stobj_1()
		ldstr "Negative_ldloc_stobj_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_5
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_5
	}
IL_NEXT_5:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stobj_2()
		ldstr "Negative_ldloc_stobj_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_6
	}
IL_NEXT_6:
        .try {
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stobj_3()
		ldstr "Negative__ldloc_stobj_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_7
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_7
	}
IL_NEXT_7:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stobj_4()
		ldstr "Negative__ldloc_stobj_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_8
	}
IL_NEXT_8:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldobj_stloc_1()
		ldstr "Negative_ldobj_stloc_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_9
	}
IL_NEXT_9:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldobj_stloc_2()
		ldstr "Negative_ldobj_stloc_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_10
	}
IL_NEXT_10:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldobj_stloc_3()
		ldstr "Negative_ldobj_stloc_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_11
	}
IL_NEXT_11:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarg_stloc(valuetype B)
		ldstr "Negative_ldarg_stloc() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_12
	}
IL_NEXT_12:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldloc_starg(valuetype B)
		ldstr "Negative_i1_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_13
	}
IL_NEXT_13:
	ldloc.0
        ldloc.2
	callvirt instance void Test::Positive_ldloc_starg(valuetype B)
IL_NEXT_14:
	ldloc.0
        ldloc.2
	callvirt instance void Test::Positive_ldarg_stloc(valuetype B)
IL_NEXT_15:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldobj_starg(valuetype B)
		ldstr "Negative_ldobj_starg() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_16
	}
IL_NEXT_16:
        .try {
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarg_stobj(valuetype B)
		ldstr "Negative_ldarg_stobj() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_17
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_17
	}
IL_NEXT_17:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarga_stobj_1(valuetype B)
		ldstr "Negative_ldarga_stobj_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_18
	}
IL_NEXT_18:
        .try {
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarga_stobj_2(valuetype B)
		ldstr "Negative_ldarga_stobj_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_19
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_19
	}
IL_NEXT_19:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarga_ldobj_stloc_1(valuetype B)
		ldstr "Negative_ldarga_ldobj_stloc_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_20
	}
IL_NEXT_20:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarga_ldobj_stloc_2(valuetype B)
		ldstr "Negative_ldarga_ldobj_stloc_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_21
	}
IL_NEXT_21:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarga_ldind(valuetype B)
		ldstr "Negative_ldarga_ldind() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_22
	}
IL_NEXT_22:
	.try {
		ldloc.0
                ldloc.2
		callvirt instance void Test::Negative_ldarga_stind(valuetype B)
		ldstr "Negative_u1_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_23
	}
IL_NEXT_23:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelem_stloc_1()
		ldstr "Negative_ldelem_stloc_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_24
	}
IL_NEXT_24:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelem_stloc_1()
		ldstr "Negative_ldelem_stloc_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_25
	}
IL_NEXT_25:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stelem_1()
		ldstr "Negative_ldloc_stelem_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_26
	}
IL_NEXT_26:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stelem_2()
		ldstr "Negative_ldloc_stelem_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_27
	}
IL_NEXT_27:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelem_stelem_1()
		ldstr "Negative_ldelem_stelem_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_28
	}
IL_NEXT_28:
	ldloc.0
	callvirt instance void Test::Positive_ldelem_stloc()
IL_NEXT_29:
	ldloc.0
	callvirt instance void Test::Positive_ldloc_stelem()
IL_NEXT_30:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelem_stelem_2()
		ldstr "Negative_ldelem_stelem_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_31
	}
IL_NEXT_31:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelema_stloc_1()
		ldstr "Negative_ldelema_stloc_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_32
	}
IL_NEXT_32:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelema_stloc_2()
		ldstr "Negative_ldelema_stloc_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_33
	}
IL_NEXT_33:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelema_stloc_3()
		ldstr "Negative_ldelema_stloc_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_34
	}
IL_NEXT_34:
        .try {
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelema_stobj_1()
		ldstr "Negative_ldelema_stobj_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_35
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_35
	}
IL_NEXT_35:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldelema_stobj_2()
		ldstr "Negative_ldelema_stobj_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_36
	}
IL_NEXT_36:
        .try {
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldflda_stloc_1()
		ldstr "Negative_ldflda_stloc_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_37
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_37
	}
IL_NEXT_37:
        .try {
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldflda_stloc_2()
		ldstr "Negative_ldflda_stloc_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_38
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_38
	}
IL_NEXT_38:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldfld_stloc_1()
		ldstr "Negative_ldfld_stloc_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_39
	}
IL_NEXT_39:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldfld_stloc_2()
		ldstr "Negative_ldfld_stloc_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_40
	}
IL_NEXT_40:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stfld_1()
		ldstr "Negative_ldloc_stfld_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_41
	}
IL_NEXT_41:
	ldloc.0
	callvirt instance void Test::Positive_ldflda_stind()
IL_NEXT_42:
	ldloc.0
	callvirt instance void Test::Positive_ldfld_stloc()
IL_NEXT_43:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stfld_2()
		ldstr "Negative_ldloc_stfld_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_44
	}
IL_NEXT_44:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_stfld_3()
		ldstr "Negative_ldloc_stfld_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.MissingFieldException {
		leave.s IL_NEXT_45
	}
IL_NEXT_45:
        .try {
        .try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_mul()
		ldstr "Negative_ldloc_mul_stloc() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_46
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_46
	}
IL_NEXT_46:
        .try {
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ldloc_neg()
		ldstr "Negative_ldloc_neg_stloc() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_47
	}
        } catch [mscorlib]System.InvalidProgramException {
		leave.s IL_NEXT_47
	}
IL_NEXT_47:
	ldc.i4 0
	ldstr "PASSED"
	br.s IL_OUT
IL_FAIL:
	ldc.i4 1
	ldstr "FAILED!"
IL_OUT:
	call void [mscorlib]System.Console::WriteLine(string)
	ret
}
.method public hidebysig specialname rtspecialname instance void .ctor() cil managed
{
	.maxstack  8
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor()
	ret
}
}
