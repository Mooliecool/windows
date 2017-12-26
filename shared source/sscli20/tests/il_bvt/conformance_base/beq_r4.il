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
.assembly extern convdll{}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack 2
.locals init	(class [convdll]ConvDLL)
newobj instance void [convdll]ConvDLL::.ctor()
stloc 0
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq R4INFMAX
	br FAIL
R4INFMAX:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldc.r4		float32(0x80000000)
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq R4MAXONE
	br FAIL
R4MAXONE:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldc.r4		float32(0x80000000)
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq R4ONEZERO
	br FAIL
R4ONEZERO:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldc.r4		float32(0x80000000)
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq R4ZERONEGZERO
	br FAIL
R4ZERONEGZERO:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldc.r4		float32(0x80000000)
	beq R4ZERONEGONE
	br FAIL
R4ZERONEGONE:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq R4NEGZERONEGZERO
	br FAIL
R4NEGZERONEGZERO:
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x80000000)
	beq R4NEGZEROONE
	br FAIL
R4NEGZEROONE:
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldc.r4		float32(0x80000000)
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldc.r4		float32(0x80000000)
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq R4NEGONEMIN
	br FAIL
R4NEGONEMIN:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldc.r4		float32(0x80000000)
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq R4MINNEGINF
	br FAIL
R4MINNEGINF:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldc.r4		float32(0x80000000)
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq R4NEGINFNAN
	br FAIL
R4NEGINFNAN:
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::max_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::zero_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldc.r4		float32(0x80000000)
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_one_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::min_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::_inf_r4
	beq FAIL
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	ldloc 0
	ldfld float32 [convdll]ConvDLL::NaN_r4
	beq FAIL
	br BACKCHECK
TOPASS:
	br PASS
BACKCHECK:
	ldc.r4 0x0
	dup
	beq TOPASS
	br FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
.assembly beq_r4exe{}
