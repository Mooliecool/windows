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
.assembly cltexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public _clt {
.field public static   int32 i4
.field public static   int32 u4
.field public static   int64 i8
.field public static   int64 u8
.field public static   float32 r4
.field public static   float64 r8
.field public static   int32 u
.method public static void initialize() {
        ldc.i4  0x7FFFFFFF
        stsfld  int32 _clt::i4
        ldc.i4  0xFFFFFFFF
        stsfld  int32 _clt::u4
        ldc.i8  0x7FFFFFFFFFFFFFFF
        stsfld  int64 _clt::i8
        ldc.i8  0xFFFFFFFFFFFFFFFF
        stsfld  int64 _clt::u8
        ldc.r4  float32(0x7F7FFFFF)
        stsfld  float32 _clt::r4
        ldc.r8  float64(0x7FEFFFFFFFFFFFFF)
        stsfld  float64 _clt::r8
        ldc.i4  0x000000FF              
        stsfld  int32 _clt::u
        ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	call	void _clt::initialize()
        ldc.i4  0x00000001
        ldsfld  int32 _clt::i4
        clt
        brfalse FAIL    
        ldc.i4  0x00000001
        ldsfld  int32 _clt::u4
        clt.un
        brfalse FAIL    
        ldc.i8  0x0000000000000001
        ldsfld  int64 _clt::i8
        clt
        brfalse FAIL    
        ldc.i8  0x0000000000000001
        ldsfld  int64 _clt::u8
        clt.un
        brfalse FAIL    
        ldc.r4  float32(0x3F800000)
        ldsfld  float32 _clt::r4
        clt
        brfalse FAIL    
        ldc.r4  float32(0x7FC00000)
        ldsfld  float32 _clt::r4
        clt.un
        brfalse FAIL    
        ldc.r8  float64(0x3FF0000000000000)
        ldsfld  float64 _clt::r8
        clt
        brfalse FAIL    
        ldc.r8  float64(0x7FF8000000000000)
        ldsfld  float64 _clt::r8
        clt.un
        brfalse FAIL    
        ldc.i4  0x00000001
        ldsfld  int32 _clt::u
        clt.un
        brfalse FAIL    
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
