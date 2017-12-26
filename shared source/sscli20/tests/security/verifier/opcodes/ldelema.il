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
.assembly 'ldelema' { }
.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {
.method public hidebysig instance void Positive_i1_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i1_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i1_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr int8
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u1_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u1_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u1_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr unsigned int8
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i2_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i2_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i2_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr int16
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u2_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u2_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u2_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr unsigned int16
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i4_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i4_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_i4_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_i4_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i4_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr int32
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u4_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u4_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_u4_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_u4_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u4_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr unsigned int32
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_i_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_i_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr native int
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_u_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void PevNegative_u_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr native unsigned int
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i8_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Positive_i8_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_i8_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr int64
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u8_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Positive_u8_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_u8_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr unsigned int64
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Positive_r4_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r4_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr float32
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Positive_r8_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_r8_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr float64
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_i1() cil managed
{
	.maxstack  8
	.locals init (int8&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_u1() cil managed
{
	.maxstack  8
	.locals init (unsigned int8&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema unsigned int8
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_i2() cil managed
{
	.maxstack  8
	.locals init (int16&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_u2() cil managed
{
	.maxstack  8
	.locals init (unsigned int16&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema unsigned int16
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_i4() cil managed
{
	.maxstack  8
	.locals init (int32&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_u4() cil managed
{
	.maxstack  8
	.locals init (unsigned int32&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema unsigned int32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_i() cil managed
{
	.maxstack  8
	.locals init (native int&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema native int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_u() cil managed
{
	.maxstack  8
	.locals init (native unsigned int&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema native unsigned int
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_i8() cil managed
{
	.maxstack  8
	.locals init (int64&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_u8() cil managed
{
	.maxstack  8
	.locals init (unsigned int64&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema unsigned int64
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_r4() cil managed
{
	.maxstack  8
	.locals init (float32&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema float32
	stloc.0
	ret
}
.method public hidebysig instance void Negative_ref_r8() cil managed
{
	.maxstack  8
	.locals init (float64&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema float64
	stloc.0
	ret
}
.method public hidebysig instance void Positive_ref_ref() cil managed
{
	.maxstack  8
	.locals init (string&)
	ldc.i4 100
	newarr string
	ldc.i4 10
	ldelema string
	stloc.0
	ret
}
.method public hidebysig static int32 Main() cil managed
{
	.entrypoint
	.maxstack 8
	.locals init (class Test V_0)
	newobj     instance void Test::.ctor()
	stloc.0
	ldloc.0
	callvirt instance void Test::Positive_i1_i1()
IL_NEXT_1:
	ldloc.0
	callvirt instance void Test::Positive_i1_u1()
IL_NEXT_2:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_i2()
		ldstr "Negative_i1_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_3
	}
IL_NEXT_3:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_u2()
		ldstr "Negative_i1_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_4
	}
IL_NEXT_4:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_i4()
		ldstr "Negative_i1_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_5
	}
IL_NEXT_5:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_u4()
		ldstr "Negative_i1_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_6
	}
IL_NEXT_6:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_i()
		ldstr "Negative_i1_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_7
	}
IL_NEXT_7:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_u()
		ldstr "Negative_i1_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_8
	}
IL_NEXT_8:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_i8()
		ldstr "Negative_i1_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_9
	}
IL_NEXT_9:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_u8()
		ldstr "Negative_i1_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_10
	}
IL_NEXT_10:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_r4()
		ldstr "Negative_i1_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_11
	}
IL_NEXT_11:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_r8()
		ldstr "Negative_i1_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_12
	}
IL_NEXT_12:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i1_ref()
		ldstr "Negative_i1_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_13
	}
IL_NEXT_13:
	ldloc.0
	callvirt instance void Test::Positive_u1_i1()
IL_NEXT_14:
	ldloc.0
	callvirt instance void Test::Positive_u1_u1()
IL_NEXT_15:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_i2()
		ldstr "Negative_u1_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_16
	}
IL_NEXT_16:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_u2()
		ldstr "Negative_u1_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_17
	}
IL_NEXT_17:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_i4()
		ldstr "Negative_u1_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_18
	}
IL_NEXT_18:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_u4()
		ldstr "Negative_u1_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_19
	}
IL_NEXT_19:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_i()
		ldstr "Negative_u1_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_20
	}
IL_NEXT_20:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_u()
		ldstr "Negative_u1_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_21
	}
IL_NEXT_21:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_i8()
		ldstr "Negative_u1_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_22
	}
IL_NEXT_22:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_u8()
		ldstr "Negative_u1_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_23
	}
IL_NEXT_23:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_r4()
		ldstr "Negative_u1_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_24
	}
IL_NEXT_24:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_r8()
		ldstr "Negative_u1_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_25
	}
IL_NEXT_25:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u1_ref()
		ldstr "Negative_u1_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_26
	}
IL_NEXT_26:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_i1()
		ldstr "Negative_i2_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_27
	}
IL_NEXT_27:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_u1()
		ldstr "Negative_i2_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_28
	}
IL_NEXT_28:
	ldloc.0
	callvirt instance void Test::Positive_i2_i2()
IL_NEXT_29:
	ldloc.0
	callvirt instance void Test::Positive_i2_u2()
IL_NEXT_30:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_i4()
		ldstr "Negative_i2_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_31
	}
IL_NEXT_31:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_u4()
		ldstr "Negative_i2_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_32
	}
IL_NEXT_32:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_i()
		ldstr "Negative_i2_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_33
	}
IL_NEXT_33:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_u()
		ldstr "Negative_i2_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_34
	}
IL_NEXT_34:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_i8()
		ldstr "Negative_i2_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_35
	}
IL_NEXT_35:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_u8()
		ldstr "Negative_i2_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_36
	}
IL_NEXT_36:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_r4()
		ldstr "Negative_i2_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_37
	}
IL_NEXT_37:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_r8()
		ldstr "Negative_i2_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_38
	}
IL_NEXT_38:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i2_ref()
		ldstr "Negative_i2_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_39
	}
IL_NEXT_39:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_i1()
		ldstr "Negative_u2_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_40
	}
IL_NEXT_40:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_u1()
		ldstr "Negative_u2_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_41
	}
IL_NEXT_41:
	ldloc.0
	callvirt instance void Test::Positive_u2_i2()
IL_NEXT_42:
	ldloc.0
	callvirt instance void Test::Positive_u2_u2()
IL_NEXT_43:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_i4()
		ldstr "Negative_u2_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_44
	}
IL_NEXT_44:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_u4()
		ldstr "Negative_u2_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_45
	}
IL_NEXT_45:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_i()
		ldstr "Negative_u2_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_46
	}
IL_NEXT_46:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_u()
		ldstr "Negative_u2_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_47
	}
IL_NEXT_47:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_i8()
		ldstr "Negative_u2_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_48
	}
IL_NEXT_48:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_u8()
		ldstr "Negative_u2_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_49
	}
IL_NEXT_49:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_r4()
		ldstr "Negative_u2_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_50
	}
IL_NEXT_50:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_r8()
		ldstr "Negative_u2_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_51
	}
IL_NEXT_51:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u2_ref()
		ldstr "Negative_u2_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_52
	}
IL_NEXT_52:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_i1()
		ldstr "Negative_i4_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_53
	}
IL_NEXT_53:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_u1()
		ldstr "Negative_i4_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_54
	}
IL_NEXT_54:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_i2()
		ldstr "Negative_i4_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_55
	}
IL_NEXT_55:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_u2()
		ldstr "Negative_i4_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_56
	}
IL_NEXT_56:
	ldloc.0
	callvirt instance void Test::Positive_i4_i4()
IL_NEXT_57:
	ldloc.0
	callvirt instance void Test::Positive_i4_u4()
IL_NEXT_58:
	ldloc.0
	callvirt instance void Test::PevNegative_i4_i()
IL_NEXT_59:
	ldloc.0
	callvirt instance void Test::PevNegative_i4_u()
IL_NEXT_60:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_i8()
		ldstr "Negative_i4_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_61
	}
IL_NEXT_61:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_u8()
		ldstr "Negative_i4_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_62
	}
IL_NEXT_62:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_r4()
		ldstr "Negative_i4_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_63
	}
IL_NEXT_63:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_r8()
		ldstr "Negative_i4_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_64
	}
IL_NEXT_64:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i4_ref()
		ldstr "Negative_i4_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_65
	}
IL_NEXT_65:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_i1()
		ldstr "Negative_u4_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_66
	}
IL_NEXT_66:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_u1()
		ldstr "Negative_u4_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_67
	}
IL_NEXT_67:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_i2()
		ldstr "Negative_u4_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_68
	}
IL_NEXT_68:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_u2()
		ldstr "Negative_u4_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_69
	}
IL_NEXT_69:
	ldloc.0
	callvirt instance void Test::Positive_u4_i4()
IL_NEXT_70:
	ldloc.0
	callvirt instance void Test::Positive_u4_u4()
IL_NEXT_71:
	ldloc.0
	callvirt instance void Test::PevNegative_u4_i()
IL_NEXT_72:
	ldloc.0
	callvirt instance void Test::PevNegative_u4_u()
IL_NEXT_73:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_i8()
		ldstr "Negative_u4_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_74
	}
IL_NEXT_74:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_u8()
		ldstr "Negative_u4_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_75
	}
IL_NEXT_75:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_r4()
		ldstr "Negative_u4_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_76
	}
IL_NEXT_76:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_r8()
		ldstr "Negative_u4_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_77
	}
IL_NEXT_77:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u4_ref()
		ldstr "Negative_u4_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_78
	}
IL_NEXT_78:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_i1()
		ldstr "Negative_i_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_79
	}
IL_NEXT_79:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_u1()
		ldstr "Negative_i_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_80
	}
IL_NEXT_80:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_i2()
		ldstr "Negative_i_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_81
	}
IL_NEXT_81:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_u2()
		ldstr "Negative_i_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_82
	}
IL_NEXT_82:
	ldloc.0
	callvirt instance void Test::PevNegative_i_i4()
IL_NEXT_83:
	ldloc.0
	callvirt instance void Test::PevNegative_i_u4()
IL_NEXT_84:
	ldloc.0
	callvirt instance void Test::Positive_i_i()
IL_NEXT_85:
	ldloc.0
	callvirt instance void Test::Positive_i_u()
IL_NEXT_86:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_i8()
		ldstr "Negative_i_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_87
	}
IL_NEXT_87:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_u8()
		ldstr "Negative_i_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_88
	}
IL_NEXT_88:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_r4()
		ldstr "Negative_i_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_89
	}
IL_NEXT_89:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_r8()
		ldstr "Negative_i_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_90
	}
IL_NEXT_90:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i_ref()
		ldstr "Negative_i_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_91
	}
IL_NEXT_91:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_i1()
		ldstr "Negative_u_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_92
	}
IL_NEXT_92:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_u1()
		ldstr "Negative_u_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_93
	}
IL_NEXT_93:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_i2()
		ldstr "Negative_u_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_94
	}
IL_NEXT_94:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_u2()
		ldstr "Negative_u_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_95
	}
IL_NEXT_95:
	ldloc.0
	callvirt instance void Test::PevNegative_u_i4()
IL_NEXT_96:
	ldloc.0
	callvirt instance void Test::PevNegative_u_u4()
IL_NEXT_97:
	ldloc.0
	callvirt instance void Test::Positive_u_i()
IL_NEXT_98:
	ldloc.0
	callvirt instance void Test::Positive_u_u()
IL_NEXT_99:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_i8()
		ldstr "Negative_u_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_100
	}
IL_NEXT_100:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_u8()
		ldstr "Negative_u_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_101
	}
IL_NEXT_101:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_r4()
		ldstr "Negative_u_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_102
	}
IL_NEXT_102:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_r8()
		ldstr "Negative_u_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_103
	}
IL_NEXT_103:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u_ref()
		ldstr "Negative_u_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_104
	}
IL_NEXT_104:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_i1()
		ldstr "Negative_i8_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_105
	}
IL_NEXT_105:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_u1()
		ldstr "Negative_i8_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_106
	}
IL_NEXT_106:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_i2()
		ldstr "Negative_i8_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_107
	}
IL_NEXT_107:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_u2()
		ldstr "Negative_i8_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_108
	}
IL_NEXT_108:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_i4()
		ldstr "Negative_i8_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_109
	}
IL_NEXT_109:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_u4()
		ldstr "Negative_i8_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_110
	}
IL_NEXT_110:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_i()
		ldstr "Negative_i8_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_111
	}
IL_NEXT_111:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_u()
		ldstr "Negative_i8_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_112
	}
IL_NEXT_112:
	ldloc.0
	callvirt instance void Test::Positive_i8_i8()
IL_NEXT_113:
	ldloc.0
	callvirt instance void Test::Positive_i8_u8()
IL_NEXT_114:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_r4()
		ldstr "Negative_i8_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_115
	}
IL_NEXT_115:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_r8()
		ldstr "Negative_i8_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_116
	}
IL_NEXT_116:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_i8_ref()
		ldstr "Negative_i8_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_117
	}
IL_NEXT_117:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_i1()
		ldstr "Negative_u8_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_118
	}
IL_NEXT_118:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_u1()
		ldstr "Negative_u8_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_119
	}
IL_NEXT_119:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_i2()
		ldstr "Negative_u8_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_120
	}
IL_NEXT_120:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_u2()
		ldstr "Negative_u8_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_121
	}
IL_NEXT_121:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_i4()
		ldstr "Negative_u8_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_122
	}
IL_NEXT_122:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_u4()
		ldstr "Negative_u8_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_123
	}
IL_NEXT_123:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_i()
		ldstr "Negative_u8_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_124
	}
IL_NEXT_124:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_u()
		ldstr "Negative_u8_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_125
	}
IL_NEXT_125:
	ldloc.0
	callvirt instance void Test::Positive_u8_i8()
IL_NEXT_126:
	ldloc.0
	callvirt instance void Test::Positive_u8_u8()
IL_NEXT_127:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_r4()
		ldstr "Negative_u8_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_128
	}
IL_NEXT_128:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_r8()
		ldstr "Negative_u8_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_129
	}
IL_NEXT_129:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_u8_ref()
		ldstr "Negative_u8_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_130
	}
IL_NEXT_130:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_i1()
		ldstr "Negative_r4_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_131
	}
IL_NEXT_131:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_u1()
		ldstr "Negative_r4_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_132
	}
IL_NEXT_132:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_i2()
		ldstr "Negative_r4_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_133
	}
IL_NEXT_133:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_u2()
		ldstr "Negative_r4_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_134
	}
IL_NEXT_134:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_i4()
		ldstr "Negative_r4_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_135
	}
IL_NEXT_135:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_u4()
		ldstr "Negative_r4_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_136
	}
IL_NEXT_136:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_i()
		ldstr "Negative_r4_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_137
	}
IL_NEXT_137:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_u()
		ldstr "Negative_r4_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_138
	}
IL_NEXT_138:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_i8()
		ldstr "Negative_r4_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_139
	}
IL_NEXT_139:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_u8()
		ldstr "Negative_r4_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_140
	}
IL_NEXT_140:
	ldloc.0
	callvirt instance void Test::Positive_r4_r4()
IL_NEXT_141:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_r8()
		ldstr "Negative_r4_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_142
	}
IL_NEXT_142:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r4_ref()
		ldstr "Negative_r4_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_143
	}
IL_NEXT_143:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_i1()
		ldstr "Negative_r8_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_144
	}
IL_NEXT_144:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_u1()
		ldstr "Negative_r8_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_145
	}
IL_NEXT_145:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_i2()
		ldstr "Negative_r8_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_146
	}
IL_NEXT_146:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_u2()
		ldstr "Negative_r8_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_147
	}
IL_NEXT_147:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_i4()
		ldstr "Negative_r8_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_148
	}
IL_NEXT_148:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_u4()
		ldstr "Negative_r8_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_149
	}
IL_NEXT_149:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_i()
		ldstr "Negative_r8_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_150
	}
IL_NEXT_150:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_u()
		ldstr "Negative_r8_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_151
	}
IL_NEXT_151:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_i8()
		ldstr "Negative_r8_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_152
	}
IL_NEXT_152:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_u8()
		ldstr "Negative_r8_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_153
	}
IL_NEXT_153:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_r4()
		ldstr "Negative_r8_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_154
	}
IL_NEXT_154:
	ldloc.0
	callvirt instance void Test::Positive_r8_r8()
IL_NEXT_155:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_r8_ref()
		ldstr "Negative_r8_ref() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_156
	}
IL_NEXT_156:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_i1()
		ldstr "Negative_ref_i1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_157
	}
IL_NEXT_157:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_u1()
		ldstr "Negative_ref_u1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_158
	}
IL_NEXT_158:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_i2()
		ldstr "Negative_ref_i2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_159
	}
IL_NEXT_159:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_u2()
		ldstr "Negative_ref_u2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_160
	}
IL_NEXT_160:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_i4()
		ldstr "Negative_ref_i4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_161
	}
IL_NEXT_161:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_u4()
		ldstr "Negative_ref_u4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_162
	}
IL_NEXT_162:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_i()
		ldstr "Negative_ref_i() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_163
	}
IL_NEXT_163:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_u()
		ldstr "Negative_ref_u() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_164
	}
IL_NEXT_164:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_i8()
		ldstr "Negative_ref_i8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_165
	}
IL_NEXT_165:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_u8()
		ldstr "Negative_ref_u8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_166
	}
IL_NEXT_166:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_r4()
		ldstr "Negative_ref_r4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_167
	}
IL_NEXT_167:
	.try {
		ldloc.0
		callvirt instance void Test::Negative_ref_r8()
		ldstr "Negative_ref_r8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_168
	}
IL_NEXT_168:
	ldloc.0
	callvirt instance void Test::Positive_ref_ref()
IL_NEXT_169:
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
