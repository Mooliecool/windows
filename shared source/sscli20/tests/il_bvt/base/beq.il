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
.assembly beqexe{}
.method public	static int32 main(class [mscorlib]System.String[]) {
.locals		(class [mscorlib]System.String)
.entrypoint
	ldc.i4		0xFFAAFFAA
	ldc.i4		0xFFAAFFAA
	beq			next0
	br			fail
next0:
        ldc.i4          0xDDDD0000
        ldc.i4          0x0000DDDD
        beq                     fail
        br                      next1
next1:
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x3F800000)
	beq			next2
	br			fail
next2:
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x7F800000)
	beq			fail
	br			next3
next3:
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x3F800000)
	beq			next4
	br			fail
next4:
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0xBF800000)
	beq			fail
	br			next5
next5:
        ldc.r4          float32(0xBF800000)
        ldc.r4          float32(0x3F800000)
        beq                     fail
        br                      pass
pass:
	ldc.i4		0x0000
	br			end
fail:
	ldc.i4		0x0001
	br			end
end:
	ret
}
