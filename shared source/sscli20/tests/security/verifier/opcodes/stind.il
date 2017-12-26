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
.assembly 'stind' { }
.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {
.method public hidebysig instance void Positive_I4_i1_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_0() cil managed
{
	.maxstack 8
	.locals init (int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Positive_I4_i1_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_2() cil managed
{
	.maxstack 8
	.locals init (unsigned int8)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Positive_I4_i2_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_4() cil managed
{
	.maxstack 8
	.locals init (int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Positive_I4_i2_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_6() cil managed
{
	.maxstack 8
	.locals init (unsigned int16)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Positive_I4_i4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void PevNegative_I4_i_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_8() cil managed
{
	.maxstack 8
	.locals init (int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Positive_I4_i4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void PevNegative_I4_i_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_10() cil managed
{
	.maxstack 8
	.locals init (unsigned int32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void PevNegative_I4_i4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Positive_I4_i_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_12() cil managed
{
	.maxstack 8
	.locals init (native int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void PevNegative_I4_i4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Positive_I4_i_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_14() cil managed
{
	.maxstack 8
	.locals init (native unsigned int)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Positive_I8_i8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_16() cil managed
{
	.maxstack 8
	.locals init (int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Positive_I8_i8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_18() cil managed
{
	.maxstack 8
	.locals init (unsigned int64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Positive_R_r4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_20() cil managed
{
	.maxstack 8
	.locals init (float32)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Positive_R_r8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Negative_REF_ref_22() cil managed
{
	.maxstack 8
	.locals init (float64)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I4_i1_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I4_i2_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I4_i4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I4_i_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I4_i8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I4_r4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I4_r8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I4_ref_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i4.0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_I8_i1_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_I8_i2_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_I8_i4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_I8_i_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_I8_i8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_I8_r4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_I8_r8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_I8_ref_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.i8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_R_i1_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i1
	ret
}
.method public hidebysig instance void Negative_R_i2_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i2
	ret
}
.method public hidebysig instance void Negative_R_i4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i4
	ret
}
.method public hidebysig instance void Negative_R_i_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i
	ret
}
.method public hidebysig instance void Negative_R_i8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.i8
	ret
}
.method public hidebysig instance void Negative_R_r4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r4
	ret
}
.method public hidebysig instance void Negative_R_r8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.r8
	ret
}
.method public hidebysig instance void Negative_R_ref_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldc.r8 0
	stind.ref
	ret
}
.method public hidebysig instance void Negative_REF_i1_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i1
	ret
}
.method public hidebysig instance void Negative_REF_i2_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i2
	ret
}
.method public hidebysig instance void Negative_REF_i4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i4
	ret
}
.method public hidebysig instance void Negative_REF_i_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i
	ret
}
.method public hidebysig instance void Negative_REF_i8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.i8
	ret
}
.method public hidebysig instance void Negative_REF_r4_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r4
	ret
}
.method public hidebysig instance void Negative_REF_r8_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.r8
	ret
}
.method public hidebysig instance void Positive_REF_ref_24() cil managed
{
	.maxstack 8
	.locals init (string)
	.zeroinit
	ldloca.s 0
	ldstr "Hi"
	stind.ref
	ret
}.method public hidebysig static int32 Main() cil managed
{
	.entrypoint
	.maxstack 8
	.locals init (class Test V_0, int32)
	.zeroinit
	newobj     instance void Test::.ctor()
	stloc.0
	ldloc.0
	callvirt instance void Test::Positive_I4_i1_0()
IL_NEXT_1:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_0()
		ldstr "Negative_I4_i2_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_2
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_2
	}
IL_NEXT_2:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_0()
		ldstr "Negative_I4_i4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_3
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_3
	}
IL_NEXT_3:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_0()
		ldstr "Negative_I4_i_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_4
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_4
	}
IL_NEXT_4:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_0()
		ldstr "Negative_I4_i8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_5
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_5
	}
IL_NEXT_5:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_0()
		ldstr "Negative_I4_r4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_6
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_6
	}
IL_NEXT_6:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_0()
		ldstr "Negative_I4_r8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_7
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_7
	}
IL_NEXT_7:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_0()
		ldstr "Negative_I4_ref_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_8
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_8
	}
IL_NEXT_8:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_0()
		ldstr "Negative_I8_i1_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_9
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_9
	}
IL_NEXT_9:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_0()
		ldstr "Negative_I8_i2_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_10
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_10
	}
IL_NEXT_10:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_0()
		ldstr "Negative_I8_i4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_11
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_11
	}
IL_NEXT_11:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_0()
		ldstr "Negative_I8_i_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_12
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_12
	}
IL_NEXT_12:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_0()
		ldstr "Negative_I8_i8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_13
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_13
	}
IL_NEXT_13:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_0()
		ldstr "Negative_I8_r4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_14
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_14
	}
IL_NEXT_14:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_0()
		ldstr "Negative_I8_r8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_15
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_15
	}
IL_NEXT_15:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_0()
		ldstr "Negative_I8_ref_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_16
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_16
	}
IL_NEXT_16:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_0()
		ldstr "Negative_R_i1_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_17
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_17
	}
IL_NEXT_17:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_0()
		ldstr "Negative_R_i2_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_18
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_18
	}
IL_NEXT_18:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_0()
		ldstr "Negative_R_i4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_19
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_19
	}
IL_NEXT_19:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_0()
		ldstr "Negative_R_i_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_20
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_20
	}
IL_NEXT_20:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_0()
		ldstr "Negative_R_i8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_21
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_21
	}
IL_NEXT_21:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_0()
		ldstr "Negative_R_r4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_22
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_22
	}
IL_NEXT_22:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_0()
		ldstr "Negative_R_r8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_23
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_23
	}
IL_NEXT_23:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_0()
		ldstr "Negative_R_ref_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_24
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_24
	}
IL_NEXT_24:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_0()
		ldstr "Negative_REF_i1_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_25
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_25
	}
IL_NEXT_25:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_0()
		ldstr "Negative_REF_i2_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_26
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_26
	}
IL_NEXT_26:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_0()
		ldstr "Negative_REF_i4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_27
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_27
	}
IL_NEXT_27:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_0()
		ldstr "Negative_REF_i_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_28
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_28
	}
IL_NEXT_28:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_0()
		ldstr "Negative_REF_i8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_29
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_29
	}
IL_NEXT_29:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_0()
		ldstr "Negative_REF_r4_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_30
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_30
	}
IL_NEXT_30:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_0()
		ldstr "Negative_REF_r8_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_31
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_31
	}
IL_NEXT_31:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_0()
		ldstr "Negative_REF_ref_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_32
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_32
	}
IL_NEXT_32:
	ldloc.0
	callvirt instance void Test::Positive_I4_i1_2()
IL_NEXT_33:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_2()
		ldstr "Negative_I4_i2_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_34
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_34
	}
IL_NEXT_34:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_2()
		ldstr "Negative_I4_i4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_35
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_35
	}
IL_NEXT_35:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_2()
		ldstr "Negative_I4_i_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_36
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_36
	}
IL_NEXT_36:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_2()
		ldstr "Negative_I4_i8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_37
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_37
	}
IL_NEXT_37:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_2()
		ldstr "Negative_I4_r4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_38
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_38
	}
IL_NEXT_38:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_2()
		ldstr "Negative_I4_r8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_39
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_39
	}
IL_NEXT_39:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_2()
		ldstr "Negative_I4_ref_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_40
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_40
	}
IL_NEXT_40:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_2()
		ldstr "Negative_I8_i1_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_41
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_41
	}
IL_NEXT_41:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_2()
		ldstr "Negative_I8_i2_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_42
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_42
	}
IL_NEXT_42:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_2()
		ldstr "Negative_I8_i4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_43
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_43
	}
IL_NEXT_43:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_2()
		ldstr "Negative_I8_i_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_44
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_44
	}
IL_NEXT_44:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_2()
		ldstr "Negative_I8_i8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_45
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_45
	}
IL_NEXT_45:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_2()
		ldstr "Negative_I8_r4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_46
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_46
	}
IL_NEXT_46:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_2()
		ldstr "Negative_I8_r8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_47
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_47
	}
IL_NEXT_47:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_2()
		ldstr "Negative_I8_ref_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_48
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_48
	}
IL_NEXT_48:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_2()
		ldstr "Negative_R_i1_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_49
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_49
	}
IL_NEXT_49:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_2()
		ldstr "Negative_R_i2_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_50
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_50
	}
IL_NEXT_50:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_2()
		ldstr "Negative_R_i4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_51
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_51
	}
IL_NEXT_51:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_2()
		ldstr "Negative_R_i_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_52
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_52
	}
IL_NEXT_52:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_2()
		ldstr "Negative_R_i8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_53
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_53
	}
IL_NEXT_53:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_2()
		ldstr "Negative_R_r4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_54
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_54
	}
IL_NEXT_54:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_2()
		ldstr "Negative_R_r8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_55
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_55
	}
IL_NEXT_55:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_2()
		ldstr "Negative_R_ref_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_56
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_56
	}
IL_NEXT_56:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_2()
		ldstr "Negative_REF_i1_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_57
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_57
	}
IL_NEXT_57:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_2()
		ldstr "Negative_REF_i2_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_58
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_58
	}
IL_NEXT_58:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_2()
		ldstr "Negative_REF_i4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_59
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_59
	}
IL_NEXT_59:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_2()
		ldstr "Negative_REF_i_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_60
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_60
	}
IL_NEXT_60:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_2()
		ldstr "Negative_REF_i8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_61
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_61
	}
IL_NEXT_61:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_2()
		ldstr "Negative_REF_r4_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_62
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_62
	}
IL_NEXT_62:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_2()
		ldstr "Negative_REF_r8_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_63
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_63
	}
IL_NEXT_63:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_2()
		ldstr "Negative_REF_ref_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_64
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_64
	}
IL_NEXT_64:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_4()
		ldstr "Negative_I4_i1_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_65
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_65
	}
IL_NEXT_65:
	ldloc.0
	callvirt instance void Test::Positive_I4_i2_4()
IL_NEXT_66:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_4()
		ldstr "Negative_I4_i4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_67
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_67
	}
IL_NEXT_67:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_4()
		ldstr "Negative_I4_i_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_68
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_68
	}
IL_NEXT_68:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_4()
		ldstr "Negative_I4_i8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_69
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_69
	}
IL_NEXT_69:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_4()
		ldstr "Negative_I4_r4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_70
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_70
	}
IL_NEXT_70:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_4()
		ldstr "Negative_I4_r8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_71
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_71
	}
IL_NEXT_71:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_4()
		ldstr "Negative_I4_ref_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_72
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_72
	}
IL_NEXT_72:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_4()
		ldstr "Negative_I8_i1_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_73
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_73
	}
IL_NEXT_73:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_4()
		ldstr "Negative_I8_i2_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_74
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_74
	}
IL_NEXT_74:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_4()
		ldstr "Negative_I8_i4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_75
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_75
	}
IL_NEXT_75:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_4()
		ldstr "Negative_I8_i_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_76
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_76
	}
IL_NEXT_76:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_4()
		ldstr "Negative_I8_i8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_77
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_77
	}
IL_NEXT_77:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_4()
		ldstr "Negative_I8_r4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_78
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_78
	}
IL_NEXT_78:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_4()
		ldstr "Negative_I8_r8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_79
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_79
	}
IL_NEXT_79:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_4()
		ldstr "Negative_I8_ref_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_80
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_80
	}
IL_NEXT_80:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_4()
		ldstr "Negative_R_i1_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_81
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_81
	}
IL_NEXT_81:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_4()
		ldstr "Negative_R_i2_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_82
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_82
	}
IL_NEXT_82:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_4()
		ldstr "Negative_R_i4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_83
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_83
	}
IL_NEXT_83:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_4()
		ldstr "Negative_R_i_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_84
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_84
	}
IL_NEXT_84:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_4()
		ldstr "Negative_R_i8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_85
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_85
	}
IL_NEXT_85:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_4()
		ldstr "Negative_R_r4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_86
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_86
	}
IL_NEXT_86:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_4()
		ldstr "Negative_R_r8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_87
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_87
	}
IL_NEXT_87:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_4()
		ldstr "Negative_R_ref_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_88
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_88
	}
IL_NEXT_88:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_4()
		ldstr "Negative_REF_i1_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_89
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_89
	}
IL_NEXT_89:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_4()
		ldstr "Negative_REF_i2_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_90
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_90
	}
IL_NEXT_90:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_4()
		ldstr "Negative_REF_i4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_91
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_91
	}
IL_NEXT_91:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_4()
		ldstr "Negative_REF_i_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_92
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_92
	}
IL_NEXT_92:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_4()
		ldstr "Negative_REF_i8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_93
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_93
	}
IL_NEXT_93:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_4()
		ldstr "Negative_REF_r4_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_94
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_94
	}
IL_NEXT_94:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_4()
		ldstr "Negative_REF_r8_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_95
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_95
	}
IL_NEXT_95:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_4()
		ldstr "Negative_REF_ref_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_96
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_96
	}
IL_NEXT_96:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_6()
		ldstr "Negative_I4_i1_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_97
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_97
	}
IL_NEXT_97:
	ldloc.0
	callvirt instance void Test::Positive_I4_i2_6()
IL_NEXT_98:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_6()
		ldstr "Negative_I4_i4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_99
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_99
	}
IL_NEXT_99:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_6()
		ldstr "Negative_I4_i_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_100
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_100
	}
IL_NEXT_100:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_6()
		ldstr "Negative_I4_i8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_101
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_101
	}
IL_NEXT_101:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_6()
		ldstr "Negative_I4_r4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_102
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_102
	}
IL_NEXT_102:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_6()
		ldstr "Negative_I4_r8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_103
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_103
	}
IL_NEXT_103:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_6()
		ldstr "Negative_I4_ref_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_104
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_104
	}
IL_NEXT_104:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_6()
		ldstr "Negative_I8_i1_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_105
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_105
	}
IL_NEXT_105:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_6()
		ldstr "Negative_I8_i2_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_106
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_106
	}
IL_NEXT_106:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_6()
		ldstr "Negative_I8_i4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_107
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_107
	}
IL_NEXT_107:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_6()
		ldstr "Negative_I8_i_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_108
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_108
	}
IL_NEXT_108:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_6()
		ldstr "Negative_I8_i8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_109
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_109
	}
IL_NEXT_109:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_6()
		ldstr "Negative_I8_r4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_110
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_110
	}
IL_NEXT_110:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_6()
		ldstr "Negative_I8_r8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_111
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_111
	}
IL_NEXT_111:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_6()
		ldstr "Negative_I8_ref_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_112
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_112
	}
IL_NEXT_112:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_6()
		ldstr "Negative_R_i1_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_113
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_113
	}
IL_NEXT_113:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_6()
		ldstr "Negative_R_i2_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_114
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_114
	}
IL_NEXT_114:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_6()
		ldstr "Negative_R_i4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_115
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_115
	}
IL_NEXT_115:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_6()
		ldstr "Negative_R_i_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_116
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_116
	}
IL_NEXT_116:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_6()
		ldstr "Negative_R_i8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_117
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_117
	}
IL_NEXT_117:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_6()
		ldstr "Negative_R_r4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_118
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_118
	}
IL_NEXT_118:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_6()
		ldstr "Negative_R_r8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_119
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_119
	}
IL_NEXT_119:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_6()
		ldstr "Negative_R_ref_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_120
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_120
	}
IL_NEXT_120:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_6()
		ldstr "Negative_REF_i1_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_121
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_121
	}
IL_NEXT_121:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_6()
		ldstr "Negative_REF_i2_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_122
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_122
	}
IL_NEXT_122:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_6()
		ldstr "Negative_REF_i4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_123
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_123
	}
IL_NEXT_123:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_6()
		ldstr "Negative_REF_i_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_124
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_124
	}
IL_NEXT_124:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_6()
		ldstr "Negative_REF_i8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_125
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_125
	}
IL_NEXT_125:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_6()
		ldstr "Negative_REF_r4_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_126
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_126
	}
IL_NEXT_126:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_6()
		ldstr "Negative_REF_r8_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_127
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_127
	}
IL_NEXT_127:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_6()
		ldstr "Negative_REF_ref_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_128
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_128
	}
IL_NEXT_128:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_8()
		ldstr "Negative_I4_i1_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_129
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_129
	}
IL_NEXT_129:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_8()
		ldstr "Negative_I4_i2_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_130
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_130
	}
IL_NEXT_130:
	ldloc.0
	callvirt instance void Test::Positive_I4_i4_8()
IL_NEXT_131:
	ldloc.0
	callvirt instance void Test::PevNegative_I4_i_8()
IL_NEXT_132:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_8()
		ldstr "Negative_I4_i8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_133
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_133
	}
IL_NEXT_133:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_8()
		ldstr "Negative_I4_r4_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_134
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_134
	}
IL_NEXT_134:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_8()
		ldstr "Negative_I4_r8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_135
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_135
	}
IL_NEXT_135:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_8()
		ldstr "Negative_I4_ref_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_136
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_136
	}
IL_NEXT_136:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_8()
		ldstr "Negative_I8_i1_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_137
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_137
	}
IL_NEXT_137:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_8()
		ldstr "Negative_I8_i2_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_138
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_138
	}
IL_NEXT_138:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_8()
		ldstr "Negative_I8_i4_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_139
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_139
	}
IL_NEXT_139:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_8()
		ldstr "Negative_I8_i_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_140
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_140
	}
IL_NEXT_140:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_8()
		ldstr "Negative_I8_i8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_141
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_141
	}
IL_NEXT_141:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_8()
		ldstr "Negative_I8_r4_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_142
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_142
	}
IL_NEXT_142:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_8()
		ldstr "Negative_I8_r8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_143
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_143
	}
IL_NEXT_143:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_8()
		ldstr "Negative_I8_ref_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_144
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_144
	}
IL_NEXT_144:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_8()
		ldstr "Negative_R_i1_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_145
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_145
	}
IL_NEXT_145:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_8()
		ldstr "Negative_R_i2_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_146
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_146
	}
IL_NEXT_146:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_8()
		ldstr "Negative_R_i4_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_147
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_147
	}
IL_NEXT_147:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_8()
		ldstr "Negative_R_i_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_148
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_148
	}
IL_NEXT_148:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_8()
		ldstr "Negative_R_i8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_149
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_149
	}
IL_NEXT_149:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_8()
		ldstr "Negative_R_r4_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_150
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_150
	}
IL_NEXT_150:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_8()
		ldstr "Negative_R_r8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_151
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_151
	}
IL_NEXT_151:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_8()
		ldstr "Negative_R_ref_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_152
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_152
	}
IL_NEXT_152:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_8()
		ldstr "Negative_REF_i1_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_153
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_153
	}
IL_NEXT_153:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_8()
		ldstr "Negative_REF_i2_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_154
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_154
	}
IL_NEXT_154:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_8()
		ldstr "Negative_REF_i4_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_155
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_155
	}
IL_NEXT_155:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_8()
		ldstr "Negative_REF_i_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_156
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_156
	}
IL_NEXT_156:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_8()
		ldstr "Negative_REF_i8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_157
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_157
	}
IL_NEXT_157:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_8()
		ldstr "Negative_REF_r4_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_158
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_158
	}
IL_NEXT_158:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_8()
		ldstr "Negative_REF_r8_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_159
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_159
	}
IL_NEXT_159:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_8()
		ldstr "Negative_REF_ref_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_160
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_160
	}
IL_NEXT_160:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_10()
		ldstr "Negative_I4_i1_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_161
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_161
	}
IL_NEXT_161:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_10()
		ldstr "Negative_I4_i2_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_162
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_162
	}
IL_NEXT_162:
	ldloc.0
	callvirt instance void Test::Positive_I4_i4_10()
IL_NEXT_163:
	ldloc.0
	callvirt instance void Test::PevNegative_I4_i_10()
IL_NEXT_164:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_10()
		ldstr "Negative_I4_i8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_165
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_165
	}
IL_NEXT_165:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_10()
		ldstr "Negative_I4_r4_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_166
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_166
	}
IL_NEXT_166:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_10()
		ldstr "Negative_I4_r8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_167
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_167
	}
IL_NEXT_167:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_10()
		ldstr "Negative_I4_ref_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_168
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_168
	}
IL_NEXT_168:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_10()
		ldstr "Negative_I8_i1_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_169
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_169
	}
IL_NEXT_169:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_10()
		ldstr "Negative_I8_i2_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_170
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_170
	}
IL_NEXT_170:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_10()
		ldstr "Negative_I8_i4_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_171
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_171
	}
IL_NEXT_171:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_10()
		ldstr "Negative_I8_i_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_172
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_172
	}
IL_NEXT_172:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_10()
		ldstr "Negative_I8_i8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_173
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_173
	}
IL_NEXT_173:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_10()
		ldstr "Negative_I8_r4_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_174
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_174
	}
IL_NEXT_174:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_10()
		ldstr "Negative_I8_r8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_175
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_175
	}
IL_NEXT_175:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_10()
		ldstr "Negative_I8_ref_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_176
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_176
	}
IL_NEXT_176:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_10()
		ldstr "Negative_R_i1_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_177
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_177
	}
IL_NEXT_177:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_10()
		ldstr "Negative_R_i2_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_178
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_178
	}
IL_NEXT_178:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_10()
		ldstr "Negative_R_i4_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_179
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_179
	}
IL_NEXT_179:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_10()
		ldstr "Negative_R_i_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_180
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_180
	}
IL_NEXT_180:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_10()
		ldstr "Negative_R_i8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_181
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_181
	}
IL_NEXT_181:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_10()
		ldstr "Negative_R_r4_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_182
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_182
	}
IL_NEXT_182:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_10()
		ldstr "Negative_R_r8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_183
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_183
	}
IL_NEXT_183:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_10()
		ldstr "Negative_R_ref_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_184
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_184
	}
IL_NEXT_184:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_10()
		ldstr "Negative_REF_i1_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_185
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_185
	}
IL_NEXT_185:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_10()
		ldstr "Negative_REF_i2_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_186
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_186
	}
IL_NEXT_186:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_10()
		ldstr "Negative_REF_i4_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_187
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_187
	}
IL_NEXT_187:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_10()
		ldstr "Negative_REF_i_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_188
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_188
	}
IL_NEXT_188:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_10()
		ldstr "Negative_REF_i8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_189
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_189
	}
IL_NEXT_189:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_10()
		ldstr "Negative_REF_r4_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_190
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_190
	}
IL_NEXT_190:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_10()
		ldstr "Negative_REF_r8_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_191
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_191
	}
IL_NEXT_191:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_10()
		ldstr "Negative_REF_ref_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_192
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_192
	}
IL_NEXT_192:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_12()
		ldstr "Negative_I4_i1_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_193
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_193
	}
IL_NEXT_193:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_12()
		ldstr "Negative_I4_i2_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_194
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_194
	}
IL_NEXT_194:
	ldloc.0
	callvirt instance void Test::PevNegative_I4_i4_12()
IL_NEXT_195:
	ldloc.0
	callvirt instance void Test::Positive_I4_i_12()
IL_NEXT_196:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_12()
		ldstr "Negative_I4_i8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_197
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_197
	}
IL_NEXT_197:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_12()
		ldstr "Negative_I4_r4_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_198
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_198
	}
IL_NEXT_198:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_12()
		ldstr "Negative_I4_r8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_199
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_199
	}
IL_NEXT_199:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_12()
		ldstr "Negative_I4_ref_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_200
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_200
	}
IL_NEXT_200:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_12()
		ldstr "Negative_I8_i1_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_201
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_201
	}
IL_NEXT_201:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_12()
		ldstr "Negative_I8_i2_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_202
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_202
	}
IL_NEXT_202:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_12()
		ldstr "Negative_I8_i4_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_203
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_203
	}
IL_NEXT_203:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_12()
		ldstr "Negative_I8_i_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_204
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_204
	}
IL_NEXT_204:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_12()
		ldstr "Negative_I8_i8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_205
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_205
	}
IL_NEXT_205:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_12()
		ldstr "Negative_I8_r4_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_206
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_206
	}
IL_NEXT_206:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_12()
		ldstr "Negative_I8_r8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_207
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_207
	}
IL_NEXT_207:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_12()
		ldstr "Negative_I8_ref_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_208
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_208
	}
IL_NEXT_208:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_12()
		ldstr "Negative_R_i1_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_209
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_209
	}
IL_NEXT_209:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_12()
		ldstr "Negative_R_i2_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_210
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_210
	}
IL_NEXT_210:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_12()
		ldstr "Negative_R_i4_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_211
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_211
	}
IL_NEXT_211:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_12()
		ldstr "Negative_R_i_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_212
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_212
	}
IL_NEXT_212:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_12()
		ldstr "Negative_R_i8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_213
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_213
	}
IL_NEXT_213:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_12()
		ldstr "Negative_R_r4_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_214
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_214
	}
IL_NEXT_214:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_12()
		ldstr "Negative_R_r8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_215
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_215
	}
IL_NEXT_215:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_12()
		ldstr "Negative_R_ref_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_216
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_216
	}
IL_NEXT_216:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_12()
		ldstr "Negative_REF_i1_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_217
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_217
	}
IL_NEXT_217:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_12()
		ldstr "Negative_REF_i2_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_218
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_218
	}
IL_NEXT_218:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_12()
		ldstr "Negative_REF_i4_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_219
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_219
	}
IL_NEXT_219:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_12()
		ldstr "Negative_REF_i_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_220
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_220
	}
IL_NEXT_220:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_12()
		ldstr "Negative_REF_i8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_221
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_221
	}
IL_NEXT_221:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_12()
		ldstr "Negative_REF_r4_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_222
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_222
	}
IL_NEXT_222:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_12()
		ldstr "Negative_REF_r8_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_223
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_223
	}
IL_NEXT_223:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_12()
		ldstr "Negative_REF_ref_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_224
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_224
	}
IL_NEXT_224:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_14()
		ldstr "Negative_I4_i1_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_225
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_225
	}
IL_NEXT_225:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_14()
		ldstr "Negative_I4_i2_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_226
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_226
	}
IL_NEXT_226:
	ldloc.0
	callvirt instance void Test::PevNegative_I4_i4_14()
IL_NEXT_227:
	ldloc.0
	callvirt instance void Test::Positive_I4_i_14()
IL_NEXT_228:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_14()
		ldstr "Negative_I4_i8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_229
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_229
	}
IL_NEXT_229:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_14()
		ldstr "Negative_I4_r4_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_230
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_230
	}
IL_NEXT_230:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_14()
		ldstr "Negative_I4_r8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_231
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_231
	}
IL_NEXT_231:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_14()
		ldstr "Negative_I4_ref_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_232
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_232
	}
IL_NEXT_232:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_14()
		ldstr "Negative_I8_i1_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_233
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_233
	}
IL_NEXT_233:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_14()
		ldstr "Negative_I8_i2_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_234
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_234
	}
IL_NEXT_234:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_14()
		ldstr "Negative_I8_i4_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_235
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_235
	}
IL_NEXT_235:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_14()
		ldstr "Negative_I8_i_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_236
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_236
	}
IL_NEXT_236:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_14()
		ldstr "Negative_I8_i8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_237
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_237
	}
IL_NEXT_237:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_14()
		ldstr "Negative_I8_r4_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_238
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_238
	}
IL_NEXT_238:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_14()
		ldstr "Negative_I8_r8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_239
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_239
	}
IL_NEXT_239:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_14()
		ldstr "Negative_I8_ref_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_240
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_240
	}
IL_NEXT_240:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_14()
		ldstr "Negative_R_i1_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_241
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_241
	}
IL_NEXT_241:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_14()
		ldstr "Negative_R_i2_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_242
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_242
	}
IL_NEXT_242:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_14()
		ldstr "Negative_R_i4_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_243
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_243
	}
IL_NEXT_243:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_14()
		ldstr "Negative_R_i_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_244
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_244
	}
IL_NEXT_244:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_14()
		ldstr "Negative_R_i8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_245
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_245
	}
IL_NEXT_245:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_14()
		ldstr "Negative_R_r4_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_246
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_246
	}
IL_NEXT_246:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_14()
		ldstr "Negative_R_r8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_247
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_247
	}
IL_NEXT_247:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_14()
		ldstr "Negative_R_ref_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_248
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_248
	}
IL_NEXT_248:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_14()
		ldstr "Negative_REF_i1_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_249
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_249
	}
IL_NEXT_249:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_14()
		ldstr "Negative_REF_i2_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_250
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_250
	}
IL_NEXT_250:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_14()
		ldstr "Negative_REF_i4_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_251
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_251
	}
IL_NEXT_251:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_14()
		ldstr "Negative_REF_i_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_252
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_252
	}
IL_NEXT_252:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_14()
		ldstr "Negative_REF_i8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_253
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_253
	}
IL_NEXT_253:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_14()
		ldstr "Negative_REF_r4_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_254
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_254
	}
IL_NEXT_254:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_14()
		ldstr "Negative_REF_r8_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_255
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_255
	}
IL_NEXT_255:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_14()
		ldstr "Negative_REF_ref_14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_256
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_256
	}
IL_NEXT_256:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_16()
		ldstr "Negative_I4_i1_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_257
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_257
	}
IL_NEXT_257:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_16()
		ldstr "Negative_I4_i2_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_258
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_258
	}
IL_NEXT_258:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_16()
		ldstr "Negative_I4_i4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_259
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_259
	}
IL_NEXT_259:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_16()
		ldstr "Negative_I4_i_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_260
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_260
	}
IL_NEXT_260:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_16()
		ldstr "Negative_I4_i8_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_261
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_261
	}
IL_NEXT_261:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_16()
		ldstr "Negative_I4_r4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_262
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_262
	}
IL_NEXT_262:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_16()
		ldstr "Negative_I4_r8_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_263
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_263
	}
IL_NEXT_263:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_16()
		ldstr "Negative_I4_ref_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_264
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_264
	}
IL_NEXT_264:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_16()
		ldstr "Negative_I8_i1_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_265
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_265
	}
IL_NEXT_265:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_16()
		ldstr "Negative_I8_i2_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_266
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_266
	}
IL_NEXT_266:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_16()
		ldstr "Negative_I8_i4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_267
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_267
	}
IL_NEXT_267:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_16()
		ldstr "Negative_I8_i_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_268
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_268
	}
IL_NEXT_268:
	ldloc.0
	callvirt instance void Test::Positive_I8_i8_16()
IL_NEXT_269:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_16()
		ldstr "Negative_I8_r4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_270
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_270
	}
IL_NEXT_270:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_16()
		ldstr "Negative_I8_r8_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_271
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_271
	}
IL_NEXT_271:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_16()
		ldstr "Negative_I8_ref_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_272
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_272
	}
IL_NEXT_272:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_16()
		ldstr "Negative_R_i1_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_273
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_273
	}
IL_NEXT_273:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_16()
		ldstr "Negative_R_i2_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_274
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_274
	}
IL_NEXT_274:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_16()
		ldstr "Negative_R_i4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_275
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_275
	}
IL_NEXT_275:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_16()
		ldstr "Negative_R_i_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_276
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_276
	}
IL_NEXT_276:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_16()
		ldstr "Negative_R_i8_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_277
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_277
	}
IL_NEXT_277:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_16()
		ldstr "Negative_R_r4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_278
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_278
	}
IL_NEXT_278:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_16()
		ldstr "Negative_R_r8_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_279
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_279
	}
IL_NEXT_279:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_16()
		ldstr "Negative_R_ref_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_280
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_280
	}
IL_NEXT_280:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_16()
		ldstr "Negative_REF_i1_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_281
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_281
	}
IL_NEXT_281:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_16()
		ldstr "Negative_REF_i2_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_282
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_282
	}
IL_NEXT_282:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_16()
		ldstr "Negative_REF_i4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_283
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_283
	}
IL_NEXT_283:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_16()
		ldstr "Negative_REF_i_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_284
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_284
	}
IL_NEXT_284:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_16()
		ldstr "Negative_REF_i8_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_285
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_285
	}
IL_NEXT_285:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_16()
		ldstr "Negative_REF_r4_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_286
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_286
	}
IL_NEXT_286:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_16()
		ldstr "Negative_REF_r8_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_287
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_287
	}
IL_NEXT_287:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_16()
		ldstr "Negative_REF_ref_16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_288
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_288
	}
IL_NEXT_288:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_18()
		ldstr "Negative_I4_i1_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_289
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_289
	}
IL_NEXT_289:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_18()
		ldstr "Negative_I4_i2_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_290
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_290
	}
IL_NEXT_290:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_18()
		ldstr "Negative_I4_i4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_291
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_291
	}
IL_NEXT_291:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_18()
		ldstr "Negative_I4_i_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_292
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_292
	}
IL_NEXT_292:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_18()
		ldstr "Negative_I4_i8_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_293
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_293
	}
IL_NEXT_293:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_18()
		ldstr "Negative_I4_r4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_294
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_294
	}
IL_NEXT_294:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_18()
		ldstr "Negative_I4_r8_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_295
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_295
	}
IL_NEXT_295:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_18()
		ldstr "Negative_I4_ref_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_296
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_296
	}
IL_NEXT_296:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_18()
		ldstr "Negative_I8_i1_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_297
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_297
	}
IL_NEXT_297:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_18()
		ldstr "Negative_I8_i2_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_298
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_298
	}
IL_NEXT_298:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_18()
		ldstr "Negative_I8_i4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_299
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_299
	}
IL_NEXT_299:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_18()
		ldstr "Negative_I8_i_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_300
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_300
	}
IL_NEXT_300:
	ldloc.0
	callvirt instance void Test::Positive_I8_i8_18()
IL_NEXT_301:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_18()
		ldstr "Negative_I8_r4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_302
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_302
	}
IL_NEXT_302:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_18()
		ldstr "Negative_I8_r8_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_303
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_303
	}
IL_NEXT_303:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_18()
		ldstr "Negative_I8_ref_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_304
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_304
	}
IL_NEXT_304:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_18()
		ldstr "Negative_R_i1_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_305
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_305
	}
IL_NEXT_305:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_18()
		ldstr "Negative_R_i2_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_306
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_306
	}
IL_NEXT_306:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_18()
		ldstr "Negative_R_i4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_307
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_307
	}
IL_NEXT_307:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_18()
		ldstr "Negative_R_i_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_308
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_308
	}
IL_NEXT_308:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_18()
		ldstr "Negative_R_i8_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_309
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_309
	}
IL_NEXT_309:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_18()
		ldstr "Negative_R_r4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_310
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_310
	}
IL_NEXT_310:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_18()
		ldstr "Negative_R_r8_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_311
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_311
	}
IL_NEXT_311:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_18()
		ldstr "Negative_R_ref_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_312
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_312
	}
IL_NEXT_312:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_18()
		ldstr "Negative_REF_i1_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_313
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_313
	}
IL_NEXT_313:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_18()
		ldstr "Negative_REF_i2_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_314
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_314
	}
IL_NEXT_314:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_18()
		ldstr "Negative_REF_i4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_315
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_315
	}
IL_NEXT_315:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_18()
		ldstr "Negative_REF_i_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_316
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_316
	}
IL_NEXT_316:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_18()
		ldstr "Negative_REF_i8_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_317
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_317
	}
IL_NEXT_317:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_18()
		ldstr "Negative_REF_r4_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_318
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_318
	}
IL_NEXT_318:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_18()
		ldstr "Negative_REF_r8_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_319
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_319
	}
IL_NEXT_319:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_18()
		ldstr "Negative_REF_ref_18() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_320
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_320
	}
IL_NEXT_320:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_20()
		ldstr "Negative_I4_i1_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_321
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_321
	}
IL_NEXT_321:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_20()
		ldstr "Negative_I4_i2_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_322
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_322
	}
IL_NEXT_322:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_20()
		ldstr "Negative_I4_i4_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_323
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_323
	}
IL_NEXT_323:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_20()
		ldstr "Negative_I4_i_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_324
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_324
	}
IL_NEXT_324:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_20()
		ldstr "Negative_I4_i8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_325
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_325
	}
IL_NEXT_325:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_20()
		ldstr "Negative_I4_r4_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_326
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_326
	}
IL_NEXT_326:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_20()
		ldstr "Negative_I4_r8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_327
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_327
	}
IL_NEXT_327:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_20()
		ldstr "Negative_I4_ref_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_328
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_328
	}
IL_NEXT_328:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_20()
		ldstr "Negative_I8_i1_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_329
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_329
	}
IL_NEXT_329:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_20()
		ldstr "Negative_I8_i2_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_330
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_330
	}
IL_NEXT_330:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_20()
		ldstr "Negative_I8_i4_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_331
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_331
	}
IL_NEXT_331:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_20()
		ldstr "Negative_I8_i_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_332
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_332
	}
IL_NEXT_332:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_20()
		ldstr "Negative_I8_i8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_333
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_333
	}
IL_NEXT_333:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_20()
		ldstr "Negative_I8_r4_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_334
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_334
	}
IL_NEXT_334:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_20()
		ldstr "Negative_I8_r8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_335
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_335
	}
IL_NEXT_335:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_20()
		ldstr "Negative_I8_ref_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_336
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_336
	}
IL_NEXT_336:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_20()
		ldstr "Negative_R_i1_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_337
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_337
	}
IL_NEXT_337:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_20()
		ldstr "Negative_R_i2_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_338
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_338
	}
IL_NEXT_338:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_20()
		ldstr "Negative_R_i4_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_339
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_339
	}
IL_NEXT_339:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_20()
		ldstr "Negative_R_i_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_340
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_340
	}
IL_NEXT_340:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_20()
		ldstr "Negative_R_i8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_341
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_341
	}
IL_NEXT_341:
	ldloc.0
	callvirt instance void Test::Positive_R_r4_20()
IL_NEXT_342:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_20()
		ldstr "Negative_R_r8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_343
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_343
	}
IL_NEXT_343:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_20()
		ldstr "Negative_R_ref_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_344
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_344
	}
IL_NEXT_344:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_20()
		ldstr "Negative_REF_i1_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_345
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_345
	}
IL_NEXT_345:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_20()
		ldstr "Negative_REF_i2_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_346
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_346
	}
IL_NEXT_346:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_20()
		ldstr "Negative_REF_i4_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_347
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_347
	}
IL_NEXT_347:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_20()
		ldstr "Negative_REF_i_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_348
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_348
	}
IL_NEXT_348:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_20()
		ldstr "Negative_REF_i8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_349
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_349
	}
IL_NEXT_349:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_20()
		ldstr "Negative_REF_r4_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_350
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_350
	}
IL_NEXT_350:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_20()
		ldstr "Negative_REF_r8_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_351
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_351
	}
IL_NEXT_351:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_20()
		ldstr "Negative_REF_ref_20() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_352
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_352
	}
IL_NEXT_352:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_22()
		ldstr "Negative_I4_i1_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_353
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_353
	}
IL_NEXT_353:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_22()
		ldstr "Negative_I4_i2_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_354
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_354
	}
IL_NEXT_354:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_22()
		ldstr "Negative_I4_i4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_355
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_355
	}
IL_NEXT_355:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_22()
		ldstr "Negative_I4_i_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_356
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_356
	}
IL_NEXT_356:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_22()
		ldstr "Negative_I4_i8_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_357
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_357
	}
IL_NEXT_357:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_22()
		ldstr "Negative_I4_r4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_358
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_358
	}
IL_NEXT_358:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_22()
		ldstr "Negative_I4_r8_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_359
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_359
	}
IL_NEXT_359:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_22()
		ldstr "Negative_I4_ref_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_360
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_360
	}
IL_NEXT_360:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_22()
		ldstr "Negative_I8_i1_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_361
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_361
	}
IL_NEXT_361:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_22()
		ldstr "Negative_I8_i2_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_362
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_362
	}
IL_NEXT_362:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_22()
		ldstr "Negative_I8_i4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_363
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_363
	}
IL_NEXT_363:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_22()
		ldstr "Negative_I8_i_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_364
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_364
	}
IL_NEXT_364:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_22()
		ldstr "Negative_I8_i8_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_365
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_365
	}
IL_NEXT_365:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_22()
		ldstr "Negative_I8_r4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_366
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_366
	}
IL_NEXT_366:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_22()
		ldstr "Negative_I8_r8_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_367
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_367
	}
IL_NEXT_367:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_22()
		ldstr "Negative_I8_ref_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_368
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_368
	}
IL_NEXT_368:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_22()
		ldstr "Negative_R_i1_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_369
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_369
	}
IL_NEXT_369:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_22()
		ldstr "Negative_R_i2_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_370
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_370
	}
IL_NEXT_370:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_22()
		ldstr "Negative_R_i4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_371
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_371
	}
IL_NEXT_371:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_22()
		ldstr "Negative_R_i_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_372
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_372
	}
IL_NEXT_372:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_22()
		ldstr "Negative_R_i8_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_373
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_373
	}
IL_NEXT_373:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_22()
		ldstr "Negative_R_r4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_374
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_374
	}
IL_NEXT_374:
	ldloc.0
	callvirt instance void Test::Positive_R_r8_22()
IL_NEXT_375:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_22()
		ldstr "Negative_R_ref_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_376
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_376
	}
IL_NEXT_376:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_22()
		ldstr "Negative_REF_i1_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_377
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_377
	}
IL_NEXT_377:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_22()
		ldstr "Negative_REF_i2_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_378
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_378
	}
IL_NEXT_378:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_22()
		ldstr "Negative_REF_i4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_379
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_379
	}
IL_NEXT_379:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_22()
		ldstr "Negative_REF_i_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_380
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_380
	}
IL_NEXT_380:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_22()
		ldstr "Negative_REF_i8_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_381
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_381
	}
IL_NEXT_381:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_22()
		ldstr "Negative_REF_r4_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_382
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_382
	}
IL_NEXT_382:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_22()
		ldstr "Negative_REF_r8_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_383
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_383
	}
IL_NEXT_383:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_ref_22()
		ldstr "Negative_REF_ref_22() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_384
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_384
	}
IL_NEXT_384:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i1_24()
		ldstr "Negative_I4_i1_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_385
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_385
	}
IL_NEXT_385:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i2_24()
		ldstr "Negative_I4_i2_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_386
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_386
	}
IL_NEXT_386:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i4_24()
		ldstr "Negative_I4_i4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_387
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_387
	}
IL_NEXT_387:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i_24()
		ldstr "Negative_I4_i_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_388
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_388
	}
IL_NEXT_388:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_i8_24()
		ldstr "Negative_I4_i8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_389
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_389
	}
IL_NEXT_389:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r4_24()
		ldstr "Negative_I4_r4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_390
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_390
	}
IL_NEXT_390:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_r8_24()
		ldstr "Negative_I4_r8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_391
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_391
	}
IL_NEXT_391:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I4_ref_24()
		ldstr "Negative_I4_ref_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_392
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_392
	}
IL_NEXT_392:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i1_24()
		ldstr "Negative_I8_i1_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_393
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_393
	}
IL_NEXT_393:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i2_24()
		ldstr "Negative_I8_i2_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_394
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_394
	}
IL_NEXT_394:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i4_24()
		ldstr "Negative_I8_i4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_395
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_395
	}
IL_NEXT_395:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i_24()
		ldstr "Negative_I8_i_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_396
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_396
	}
IL_NEXT_396:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_i8_24()
		ldstr "Negative_I8_i8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_397
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_397
	}
IL_NEXT_397:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r4_24()
		ldstr "Negative_I8_r4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_398
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_398
	}
IL_NEXT_398:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_r8_24()
		ldstr "Negative_I8_r8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_399
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_399
	}
IL_NEXT_399:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_I8_ref_24()
		ldstr "Negative_I8_ref_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_400
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_400
	}
IL_NEXT_400:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i1_24()
		ldstr "Negative_R_i1_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_401
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_401
	}
IL_NEXT_401:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i2_24()
		ldstr "Negative_R_i2_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_402
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_402
	}
IL_NEXT_402:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i4_24()
		ldstr "Negative_R_i4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_403
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_403
	}
IL_NEXT_403:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i_24()
		ldstr "Negative_R_i_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_404
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_404
	}
IL_NEXT_404:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_i8_24()
		ldstr "Negative_R_i8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_405
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_405
	}
IL_NEXT_405:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r4_24()
		ldstr "Negative_R_r4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_406
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_406
	}
IL_NEXT_406:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_r8_24()
		ldstr "Negative_R_r8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_407
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_407
	}
IL_NEXT_407:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_R_ref_24()
		ldstr "Negative_R_ref_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_408
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_408
	}
IL_NEXT_408:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i1_24()
		ldstr "Negative_REF_i1_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_409
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_409
	}
IL_NEXT_409:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i2_24()
		ldstr "Negative_REF_i2_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_410
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_410
	}
IL_NEXT_410:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i4_24()
		ldstr "Negative_REF_i4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_411
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_411
	}
IL_NEXT_411:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i_24()
		ldstr "Negative_REF_i_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_412
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_412
	}
IL_NEXT_412:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_i8_24()
		ldstr "Negative_REF_i8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_413
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_413
	}
IL_NEXT_413:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r4_24()
		ldstr "Negative_REF_r4_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_414
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_414
	}
IL_NEXT_414:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_REF_r8_24()
		ldstr "Negative_REF_r8_24() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_415
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_415
	}
IL_NEXT_415:
	ldloc.0
	callvirt instance void Test::Positive_REF_ref_24()
IL_NEXT_416:
	ldloc.1
	brtrue IL_FAIL
	ldstr "PASSED"
	br.s IL_OUT
IL_FAIL:
	ldstr "FAILED!"
IL_OUT:
	call void [mscorlib]System.Console::WriteLine(string)
	ldloc.1
	ldc.i4 0
	add
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
