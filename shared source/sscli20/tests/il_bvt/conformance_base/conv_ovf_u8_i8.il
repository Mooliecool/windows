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
.class public		conv_ovf_i8 {
.method public static		int32 conv(int64,int64) {
.locals init		(class [mscorlib]System.OverflowException,int32)
.maxstack	2
try_start:
	ldarg		0
	conv.ovf.i8
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
.method public static		int32 conv_un(int64,int64) {
.locals init		(class [mscorlib]System.OverflowException,int64)
.maxstack	2
try_start:
	ldarg		0
	conv.ovf.i8.un
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	ldarg		1
	ceq
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
.method public		void conv_ovf_i8() {
.maxstack	1
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
	ldc.i8			0x8000000000000000
	ldc.i8			0x8000000000000000
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0xFFFFFFFFFFFFFFFF
	ldc.i8			0xFFFFFFFFFFFFFFFF
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x0000000000000000
	ldc.i8			0x0000000000000000
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x0000000000000001
	ldc.i8			0x0000000000000001
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x7FFFFFFFFFFFFFFE
	ldc.i8			0x7FFFFFFFFFFFFFFE
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x7FFFFFFFFFFFFFFF
	ldc.i8			0x7FFFFFFFFFFFFFFF
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x5555555555555555
	ldc.i8			0x5555555555555555
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0xAAAAAAAAAAAAAAAA
	ldc.i8			0xAAAAAAAAAAAAAAAA
	call		int32 conv_ovf_i8::conv(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x0000000000000000
	ldc.i8			0x0000000000000000
	call		int32 conv_ovf_i8::conv_un(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x0000000000000001
	ldc.i8			0x0000000000000001
	call		int32 conv_ovf_i8::conv_un(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x7FFFFFFFFFFFFFFE
	ldc.i8			0x7FFFFFFFFFFFFFFE
	call		int32 conv_ovf_i8::conv_un(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x7FFFFFFFFFFFFFFF
	ldc.i8			0x7FFFFFFFFFFFFFFF
	call		int32 conv_ovf_i8::conv_un(int64,int64)
	ldc.i4			0x11111111
	ceq
	brfalse			FAIL
	ldc.i8			0x5555555555555555
	ldc.i8			0x5555555555555555
	call		int32 conv_ovf_i8::conv_un(int64,int64)
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
.assembly conv_ovf_u8_i8exe{}
