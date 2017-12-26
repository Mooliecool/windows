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
.class public		mul_ovf_i2 {
.method public static int32 i2(int32,int32) {
.locals init		(class [mscorlib]System.OverflowException,int16)
.maxstack	2
try_start:
	ldarg	0
	ldarg	1
	mul
	conv.ovf.i2
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	br		END
aHandler:
	isinst	[mscorlib]System.OverflowException
	stloc	0
	leave			HEnd
HEnd:
	ldloc	0
	brfalse	FAIL
	ldc.i4	0xAEAEAEAE
	br		END
FAIL:
	ldc.i4	0xEEEEEEEE
END:
	ret
.try try_start to try_end catch [mscorlib]System.OverflowException handler aHandler to HEnd
}
.method public		void mul_ovf_i2() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
	ldc.i4		0xFFFF8000
	ldc.i4		0xFFFF8000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFF8000
	ldc.i4		0xFFFFFFFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFF8000
	ldc.i4		0x00000000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFF8000
	ldc.i4		0x00000001
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFF8000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFF8000
	ldc.i4		0x00007FFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFF8000
	ldc.i4		0x00005555
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFF8000
	ldc.i4		0xFFFFAAAA
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFF8000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000001
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000001
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00007FFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFF8001
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00005555
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFFAAAB
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFAAAA
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00005556
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFF8000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00007FFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00005555
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFAAAA
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFF8000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFF8000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000001
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000001
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00007FFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00007FFF
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00005555
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00005555
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFAAAA
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFFAAAA
	ceq
	brfalse		FAIL
	ldc.i4		0x00007FFF
	ldc.i4		0xFFFF8000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0x00007FFF
	ldc.i4		0xFFFFFFFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFF8001
	ceq
	brfalse		FAIL
	ldc.i4		0x00007FFF
	ldc.i4		0x00000000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00007FFF
	ldc.i4		0x00000001
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00007FFF
	ceq
	brfalse		FAIL
	ldc.i4		0x00007FFF
	ldc.i4		0x00007FFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0x00007FFF
	ldc.i4		0x00005555
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0x00007FFF
	ldc.i4		0xFFFFAAAA
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0xFFFF8000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0xFFFFFFFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFFAAAB
	ceq
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x00000000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x00000001
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00005555
	ceq
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x00007FFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x00005555
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0xFFFFAAAA
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFAAAA
	ldc.i4		0xFFFF8000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFAAAA
	ldc.i4		0xFFFFFFFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00005556
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFAAAA
	ldc.i4		0x00000000
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0x00000000
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFAAAA
	ldc.i4		0x00000001
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xFFFFAAAA
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFAAAA
	ldc.i4		0x00007FFF
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFAAAA
	ldc.i4		0x00005555
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFAAAA
	ldc.i4		0xFFFFAAAA
	call	int32 mul_ovf_i2::i2(int32,int32)
	ldc.i4		0xAEAEAEAE
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
}
.assembly mul_ovf_i2exe{}
