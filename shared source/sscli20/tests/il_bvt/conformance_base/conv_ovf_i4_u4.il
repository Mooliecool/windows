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
.class public		conv_ovf_u4 {
.method public static		int32 conv(int32,int32) {
.locals init		(class [mscorlib]System.OverflowException,int32)
.maxstack	2
try_start:
	ldarg		0
	conv.ovf.u4
	ldarg		1
	ceq
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	brfalse			FAIL
	ldc.i4			0x11111111
	br				END
aHandler:
	isinst			[mscorlib]System.OverflowException
	stloc	0
	leave			HEnd
HEnd:
	ldloc	0
	brfalse			FAIL
	ldc.i4			0xEEEEEEEE
	br				END
FAIL:
	ldc.i4	0x0001
	br				END
END:
	ret
.try try_start to try_end catch [mscorlib]System.OverflowException handler aHandler to HEnd
}
.method public static		int32 conv_un(int32,int32) {
.locals init		(class [mscorlib]System.OverflowException,unsigned int32)
.maxstack	2
try_start2:
	ldarg		0
	conv.ovf.u4.un
	stloc.1
	leave.s	try_end2
try_end2:
	ldloc.1
	ldarg		1
	ceq
	brfalse			FAIL2
	ldc.i4			0x11111111
	br				END2
aHandler2:
	isinst			[mscorlib]System.OverflowException
	stloc	0
	leave			HEnd2
HEnd2:
	ldloc	0
	brfalse			FAIL2
	ldc.i4			0xEEEEEEEE
	br				END2
FAIL2:
	ldc.i4			0x00000000
	br				END2
END2:
	ret
.try try_start2 to try_end2 catch [mscorlib]System.OverflowException handler aHandler2 to HEnd2
}
.method public		void conv_ovf_u4() {
.maxstack	1
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
	ldc.i4			0x80000000
	ldc.i4			0x00000000
	call		int32 conv_ovf_u4::conv(int32,int32)
	ldc.i4			0xEEEEEEEE
	ceq
	brfalse			FAIL
	ldc.i4			0xFFFFFFFF
	ldc.i4			0x00000000
	call		int32 conv_ovf_u4::conv(int32,int32)
	ldc.i4			0xEEEEEEEE
	ceq
	brfalse			FAIL
	ldc.i4			0x00000000
	ldc.i4			0x00000000
	call		int32 conv_ovf_u4::conv(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i4			0x00000001
	ldc.i4			0x00000001
	call		int32 conv_ovf_u4::conv(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i4			0x7FFFFFFF
	ldc.i4			0x7FFFFFFF
	call		int32 conv_ovf_u4::conv(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i4			0x55555555
	ldc.i4			0x55555555
	call		int32 conv_ovf_u4::conv(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i4			0xAAAAAAAA
	ldc.i4			0x00000000
	call		int32 conv_ovf_u4::conv(int32,int32)
	ldc.i4			0xEEEEEEEE
	ceq
	brfalse			FAIL
	ldc.i4			0x00000000
	ldc.i4			0x00000000
	call		int32 conv_ovf_u4::conv_un(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i4			0x00000001
	ldc.i4			0x00000001
	call		int32 conv_ovf_u4::conv_un(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i4			0x7FFFFFFF
	ldc.i4			0x7FFFFFFF
	call		int32 conv_ovf_u4::conv_un(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i4			0x55555555
	ldc.i4			0x55555555
	call		int32 conv_ovf_u4::conv_un(int32,int32)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
}
.assembly conv_ovf_i4_u4exe{}
