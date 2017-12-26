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
.assembly addexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public _add {
.field public static int32 global0
.field public static int32 global1
.method public void '<init>'() {
.maxstack	10
	ldarg.0
	call instance void [mscorlib]System.Object::'<init>'()
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	5
.locals	()
        ldc.i4  0xAABB
        conv.i2
        ldc.i4  0x1111
        add
        ldc.i4  0xFFFFBBCC
        ceq
        brfalse FAIL
        ldc.i4  0x11223344
        ldc.i4  0x11111111
        add
        ldc.i4  0x22334455
        ceq
        brfalse FAIL
        ldsflda	int32 _add::global0
        ldc.i4  0xAAAAAAAA
        stind.i4        
        ldsflda	int32 _add::global1
        ldc.i4  0x44444444
        stind.i4
        ldsflda	int32 _add::global0
        ldc.i4  0x4
        add
        ldind.i4
        ldc.i4  0x44444444
        ceq
        brfalse FAIL
        ldc.i8  0x0011223344556677
        ldc.i8  0x7766554433221100
        add
        ldc.i8  0x7777777777777777
        ceq
        brfalse FAIL
        ldc.r4  float32(0x3F800000)
        ldc.r4  float32(0x00000000)
        add
        ldc.r4  float32(0x3F800000)
        ceq
        brfalse FAIL
        ldc.r8  float64(0xBFF0000000000000)
        ldc.r8  float64(0x0000000000000000)
        add
        ldc.r8  float64(0xBFF0000000000000)
        ceq
        brfalse FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
