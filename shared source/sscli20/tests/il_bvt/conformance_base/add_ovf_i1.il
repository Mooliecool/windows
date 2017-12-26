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
.class public		add_ovf_i1 {
.method public static int32 i1(int32,int32,int32) {
.locals init	(class [mscorlib]System.OverflowException, int8)
.maxstack	2
try_start:
	ldarg	0
	ldarg	1
	add
	conv.ovf.i1
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	ldarg	2
	ceq
	brfalse	FAIL
	ldc.i4	0x11111111
	br		END
aHandler:
	isinst	[mscorlib]System.OverflowException
	stloc	0
	leave			aeEnd
aeEnd:
	ldloc	0
	brfalse	FAIL
	ldc.i4	0xEEEEEEEE
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
.try try_start to try_end catch [mscorlib]System.OverflowException handler aHandler to aeEnd
}
.method public		void add_ovf_i1() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
	ldc.i4		0xFFFFFF80
	ldc.i4		0xFFFFFF80
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFF80
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFF80
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFF80
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFF80
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFF81
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFF80
	ldc.i4		0x0000007F
	ldc.i4		0xFFFFFFFF
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFF80
	ldc.i4		0x00000055
	ldc.i4		0xFFFFFFD5
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFF80
	ldc.i4		0xFFFFFFAA
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFF80
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFE
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFFF
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x0000007F
	ldc.i4		0x0000007E
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000055
	ldc.i4		0x00000054
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFAA
	ldc.i4		0xFFFFFFA9
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFF80
	ldc.i4		0xFFFFFF80
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFFF
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	ldc.i4		0x00000001
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x0000007F
	ldc.i4		0x0000007F
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0x00000055
	ldc.i4		0x00000055
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFAA
	ldc.i4		0xFFFFFFAA
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFF80
	ldc.i4		0xFFFFFF81
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	ldc.i4		0x00000001
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000001
	ldc.i4		0x00000002
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x0000007F
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0x00000055
	ldc.i4		0x00000056
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFAA
	ldc.i4		0xFFFFFFAB
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x0000007F
	ldc.i4		0xFFFFFF80
	ldc.i4		0xFFFFFFFF
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x0000007F
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x0000007E
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x0000007F
	ldc.i4		0x00000000
	ldc.i4		0x0000007F
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x0000007F
	ldc.i4		0x00000001
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0x0000007F
	ldc.i4		0x0000007F
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0x0000007F
	ldc.i4		0x00000055
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0x0000007F
	ldc.i4		0xFFFFFFAA
	ldc.i4		0x00000029
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000055
	ldc.i4		0xFFFFFF80
	ldc.i4		0xFFFFFFD5
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000055
	ldc.i4		0xFFFFFFFF
	ldc.i4		0x00000054
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000055
	ldc.i4		0x00000000
	ldc.i4		0x00000055
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000055
	ldc.i4		0x00000001
	ldc.i4		0x00000056
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0x00000055
	ldc.i4		0x0000007F
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0x00000055
	ldc.i4		0x00000055
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0x00000055
	ldc.i4		0xFFFFFFAA
	ldc.i4		0xFFFFFFFF
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFAA
	ldc.i4		0xFFFFFF80
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFAA
	ldc.i4		0xFFFFFFFF
	ldc.i4		0xFFFFFFA9
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFAA
	ldc.i4		0x00000000
	ldc.i4		0xFFFFFFAA
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFAA
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFAB
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFAA
	ldc.i4		0x0000007F
	ldc.i4		0x00000029
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFAA
	ldc.i4		0x00000055
	ldc.i4		0xFFFFFFFF
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFFAA
	ldc.i4		0xFFFFFFAA
	ldc.i4		0x00000000
	call	int32 add_ovf_i1::i1(int32,int32,int32)
	ldc.i4		0xEEEEEEEE
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
.assembly add_ovf_i1exe{}
