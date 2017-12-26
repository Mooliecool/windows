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
.class public		sub_ovf_u2 {
.method public static int32 u2(int32,int32,int32) {
.locals init		(class [mscorlib]System.OverflowException,unsigned int16)
.maxstack	2
try_start:
	ldarg	0
	ldarg	1
	sub
	conv.ovf.u2
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	ldarg	2
	ceq
	br			END
aHandler:
	isinst	[mscorlib]System.OverflowException
	stloc	0
	leave			HEnd
HEnd:
	ldloc	0
	brfalse		FAIL
	ldarg	2
	ldc.i4		0xEEEEEEEE
	ceq
	br			END
FAIL:
	ldc.i4	0x0001
END:
	ret
.try try_start to try_end catch [mscorlib]System.OverflowException handler aHandler to HEnd
}
.method public		void sub_ovf_u2() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x0000FFFE
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x0000FFFF
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00005555
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x0000AAAA
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x0000FFFE
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x0000FFFF
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00005555
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x0000AAAA
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFE
	ldc.i4		0x00000000
	ldc.i4		0x0000FFFE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFE
	ldc.i4		0x00000001
	ldc.i4		0x0000FFFD
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFE
	ldc.i4		0x0000FFFE
	ldc.i4		0x00000000
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFE
	ldc.i4		0x0000FFFF
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFE
	ldc.i4		0x00005555
	ldc.i4		0x0000AAA9
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFE
	ldc.i4		0x0000AAAA
	ldc.i4		0x00005554
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFF
	ldc.i4		0x00000000
	ldc.i4		0x0000FFFF
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFF
	ldc.i4		0x00000001
	ldc.i4		0x0000FFFE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFF
	ldc.i4		0x0000FFFE
	ldc.i4		0x00000001
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFF
	ldc.i4		0x0000FFFF
	ldc.i4		0x00000000
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFF
	ldc.i4		0x00005555
	ldc.i4		0x0000AAAA
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000FFFF
	ldc.i4		0x0000AAAA
	ldc.i4		0x00005555
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x00000000
	ldc.i4		0x00005555
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x00000001
	ldc.i4		0x00005554
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x0000FFFE
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x0000FFFF
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x00005555
	ldc.i4		0x00000000
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x00005555
	ldc.i4		0x0000AAAA
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000AAAA
	ldc.i4		0x00000000
	ldc.i4		0x0000AAAA
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000AAAA
	ldc.i4		0x00000001
	ldc.i4		0x0000AAA9
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000AAAA
	ldc.i4		0x0000FFFE
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000AAAA
	ldc.i4		0x0000FFFF
	ldc.i4		0xEEEEEEEE
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000AAAA
	ldc.i4		0x00005555
	ldc.i4		0x00005555
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
	brfalse		FAIL
	ldc.i4		0x0000AAAA
	ldc.i4		0x0000AAAA
	ldc.i4		0x00000000
	call	int32 sub_ovf_u2::u2(int32,int32,int32)
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
.assembly sub_ovf_u2exe{}
