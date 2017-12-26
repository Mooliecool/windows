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
.assembly tailcallexe{}
.method public static int32 main(class [mscorlib]System.String[]) {
.locals     (class [mscorlib]System.IO.TextWriter,class _tailcall)
.entrypoint
	call	    class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Out()
    stloc       0
    newobj      instance void _tailcall::.ctor()
    stloc       1
    ldloc       1
    callvirt    instance int32 _tailcall::method1()
    ldc.i4      0x1234
    bne.un      fail
pass:
    ldc.i4      0x0000
    br          end
fail:
    ldc.i4      0x00001
    br          end
end:
    ret
}
.class public          _tailcall {
.method public void .ctor() {
.maxstack   10
    ldarg.0
    call instance void [mscorlib]System.Object::.ctor()
    ret
}
.method public         int32 method1() {
.locals     (int32,class [mscorlib]System.String)
    ldarg       0  
    tail.
    callvirt    instance int32 _tailcall::method2()
    ret
}
.method public         int32 method2() {
.locals     ()
    ldc.i4      0x1234
    ret 
}
}
