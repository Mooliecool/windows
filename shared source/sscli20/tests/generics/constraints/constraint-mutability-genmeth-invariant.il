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


// This test is equivalent to the following C# code

/*
using System;

interface GIComparable { int CompareTo<T>(T x); }

class Finder2<T> where T : GIComparable<T>
{
    public int Find(T x, T[] arr) 
        {      
            for (int i = 0; i < arr.Length; i++)
                if (arr[i].CompareTo(x) == 0) return i;
            return -1;
        }
}

class StaticFind2
{
    public static int Find<T>(T x, T[] arr) where T : GIComparable<T>
        {      
            for (int i = 0; i < arr.Length; i++)
                if (arr[i].CompareTo(x) == 0) return i;
            return -1;
        }
}

struct S2 : GIComparable
{
    int x;
    int y;
    public S2(int _x, int _y) { x = _x; y = _y; }
    public int CompareTo<T>(T that) 
        { 
            if (this.x < ((S2) that).x) return (-1);
            if (this.x >  ((S2) that).x) return (1);
            if (this.y <  ((S2) that).y) return (-1);
            if (this.y >  ((S2) that).y) return (1);
            return 0;
        }
}

}

class R2 : GIComparable
{
    int x;
    int y;
    public R2(int _x, int _y) { x = _x; y = _y; }
    public int CompareTo<T>(T that) 
        { 
            if (this.x < ((R2) that).x) return (-1);
            if (this.x > ((R2) that).x) return (1);
            if (this.y < ((R2) that).y) return (-1);
            if (this.y > ((R2) that).y) return (1);
            return 0;
        }
}


struct OS2 : GIComparable
{
    int x;
    public OS2(int _x) { x = _x; }
    public int CompareTo<T>(T that) 
        { 
            if (this.x < ((OS2) that).x) return (-1);
            if (this.x > ((OS2) that).x) return (1);
            return 0;
        }
}


class OR2 : GIComparable
{
    int x;
    public OR2(int _x) { x = _x; }
    public int CompareTo<T>(T that) 
        { 
            if (this.x < ((OR2) that).x) return (-1);
            if (this.x > ((OR2) that).x) return (1);
            return 0;
        }
}

class Maine {
    static public int Main(string[] args)
    {
        if (args.Length < 1)
            {
                System.Console.Error.WriteLine("Usage: perf <N>");
                return 1;
            }
        
        int N = System.Int32.Parse(args[0]);
        {

           
            int[] a = new int[N];
            for (int i = 0; i<N; i++) 
            {
                a[i] = i;
            }
            Finder<int> f = new Finder<int>();
            for (int i = 0; i<N; i++) 
            {
                if (f.Find(i, a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using Finder<int>.Find", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (int,1)");
            for (int i = 0; i<N; i++) 
            {
                if (StaticFind.Find<int>(i, a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using StaticFind.Find<int>", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (int,2)");
        }
        {
            S2[] a = new S2[N];
            for (int i = 0; i<N; i++) 
            {
                a[i] = new S2(i,i);
            }
            Finder2<S2> f = new Finder2<S2>();
            for (int i = 0; i<N; i++) 
            {
                if (f.Find(new S2(i,i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using Finder<S2>.Find", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (S2,1)");
            for (int i = 0; i<N; i++) 
            {
                if (StaticFind2.Find<S2>(new S2(i,i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using StaticFind2.Find<S2>", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (S2,2)");
        }
        {
            R2[] a = new R2[N];
            for (int i = 0; i<N; i++) 
            {
                a[i] = new R2(i,i);
            }
            Finder2<R2> f = new Finder2<R2>();
            for (int i = 0; i<N; i++) 
            {
                if (f.Find(new R2(i,i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using Finder<R2>.Find", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (R2,1)");
            for (int i = 0; i<N; i++) 
            {
                if (StaticFind2.Find<R2>(new R2(i,i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using StaticFind2.Find<R2>", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (R2,2)");
        }
        {
            OS2[] a = new OS2[N];
            for (int i = 0; i<N; i++) 
            {
                a[i] = new OS2(i);
            }
            Finder2<OS2> f = new Finder2<OS2>();
            for (int i = 0; i<N; i++) 
            {
                if (f.Find(new OS2(i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using Finder<OS2>.Find", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (OS2,1)");
            for (int i = 0; i<N; i++) 
            {
                if (StaticFind2.Find<OS2>(new OS2(i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using StaticFind2.Find<OS2>", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (OS2,2)");
        }
        {
            OR2[] a = new OR2[N];
            for (int i = 0; i<N; i++) 
            {
                a[i] = new OR2(i);
            }
            Finder2<OR2> f = new Finder2<OR2>();
            for (int i = 0; i<N; i++) 
            {
                if (f.Find(new OR2(i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using Finder<OR2>.Find", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (OR2,1)");
            for (int i = 0; i<N; i++) 
            {
                if (StaticFind2.Find<OR2>(new OR2(i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using StaticFind2.Find<OR2>", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (OR2,2)");
        }
        {
            OR2[] a = new OR2[N];
            for (int i = 0; i<N; i++) 
            {
                a[i] = new OR2(i);
            }
            Finder2<OR2> f = new Finder2<OR2>();
            for (int i = 0; i<N; i++) 
            {
                if (f.Find(new OR2(i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using Finder<GIComparable<OR2>>.Find", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (OR2,1)");
            for (int i = 0; i<N; i++) 
            {
                if (StaticFind2.Find<GIComparable<OR2>>(new OR2(i), a) != i)
                    System.Console.Error.WriteLine("Error finding {0} using StaticFind2.Find<GIComparable<OR2>>", i);
            }
            System.Console.Error.WriteLine("Test PaSSeD (OR2,2)");
        }
            return 100;
    }
}

                
*/



// Metadata version: v1.2.21230
.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .hash = (77 44 54 E4 D3 61 56 02 14 E9 88 CD D8 31 EB C9   // wDT..aV......1..
           2B DF E4 40 )                                     // +..@
  .ver 1:2:3300:0
}
.assembly test
{

  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 

  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module test.exe
// MVID: {B1E5359B-2F69-470A-9D5A-3CFCAC471629}
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x04190000

// =============== CLASS MEMBERS DECLARATION ===================

.class interface private abstract auto ansi GIComparable
{
  .method public hidebysig newslot abstract virtual 
          instance int32  CompareTo<T>(!!0 x) cil managed
  {
  } // end of method GIComparable::CompareTo

} // end of class GIComparable

.class private auto ansi beforefieldinit Finder2<(class GIComparable) T>
       extends [mscorlib]System.Object
{
  .method public hidebysig instance int32 
          Find(!0 x,
               !0[] arr) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0,
             int32 V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0020

    IL_0004:  ldarg.2
    IL_0005:  ldloc.0
    IL_0006:  ldelema     !0
    IL_0010:  ldarg.1
    IL_000b:  constrained.        !0
    IL_0011:  callvirt   instance int32 class GIComparable::CompareTo<!0>(!!0)
    IL_0016:  brtrue.s   IL_001c

    IL_0018:  ldloc.0
    IL_0019:  stloc.1
    IL_001a:  br.s       IL_002a

    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.1
    IL_001e:  add
    IL_001f:  stloc.0
    IL_0020:  ldloc.0
    IL_0021:  ldarg.2
    IL_0022:  ldlen
    IL_0023:  conv.i4
    IL_0024:  blt.s      IL_0004

    IL_0026:  ldc.i4.m1
    IL_0027:  stloc.1
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.1
    IL_002b:  ret
  } // end of method Finder2::Find

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method Finder2::.ctor

} // end of class Finder2

.class private auto ansi beforefieldinit StaticFind2
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  Find<(class GIComparable) T>(!!0 x,
                                                                           !!0[] arr) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0,
             int32 V_1)
    IL_0000:  ldc.i4.0
    IL_0001:  stloc.0
    IL_0002:  br.s       IL_0020

    IL_0004:  ldarg.1
    IL_0005:  ldloc.0
    IL_0006:  ldelema     !!0
    IL_0010:  ldarg.0
    IL_000b:  constrained.        !!0
    IL_0011:  callvirt   instance int32 class GIComparable::CompareTo<!!0>(!!0)
    IL_0016:  brtrue.s   IL_001c

    IL_0018:  ldloc.0
    IL_0019:  stloc.1
    IL_001a:  br.s       IL_002a

    IL_001c:  ldloc.0
    IL_001d:  ldc.i4.1
    IL_001e:  add
    IL_001f:  stloc.0
    IL_0020:  ldloc.0
    IL_0021:  ldarg.1
    IL_0022:  ldlen
    IL_0023:  conv.i4
    IL_0024:  blt.s      IL_0004

    IL_0026:  ldc.i4.m1
    IL_0027:  stloc.1
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.1
    IL_002b:  ret
  } // end of method StaticFind2::Find

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method StaticFind2::.ctor

} // end of class StaticFind2


.class private sequential ansi sealed beforefieldinit S2
       extends [mscorlib]System.ValueType
       implements GIComparable
{
  .field private int32 x
  .field private int32 y
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x,
                               int32 _y) cil managed
  {
    // Code size       15 (0xf)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 S2::x
    IL_0007:  ldarg.0
    IL_0008:  ldarg.2
    IL_0009:  stfld      int32 S2::y
    IL_000e:  ret
  } // end of method S2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo<T>(!!0 that) cil managed
  {
    // Code size       82 (0x52)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 S2::x
    IL_0006:  ldarg   that
      box !!0
      unbox S2
    IL_0008:  ldfld      int32 S2::x
    IL_000d:  bge.s      IL_0013

    IL_000f:  ldc.i4.m1
    IL_0010:  stloc.0
    IL_0011:  br.s       IL_0050

    IL_0013:  ldarg.0
    IL_0014:  ldfld      int32 S2::x
    IL_0019:  ldarg   that
      box !!0
      unbox S2
    IL_001b:  ldfld      int32 S2::x
    IL_0020:  ble.s      IL_0026

    IL_0022:  ldc.i4.1
    IL_0023:  stloc.0
    IL_0024:  br.s       IL_0050

    IL_0026:  ldarg.0
    IL_0027:  ldfld      int32 S2::y
    IL_002c:  ldarg   that
      box !!0
      unbox S2
    IL_002e:  ldfld      int32 S2::y
    IL_0033:  bge.s      IL_0039

    IL_0035:  ldc.i4.m1
    IL_0036:  stloc.0
    IL_0037:  br.s       IL_0050

    IL_0039:  ldarg.0
    IL_003a:  ldfld      int32 S2::y
    IL_003f:  ldarg  that
      box !!0
      unbox S2
    IL_0041:  ldfld      int32 S2::y
    IL_0046:  ble.s      IL_004c

    IL_0048:  ldc.i4.1
    IL_0049:  stloc.0
    IL_004a:  br.s       IL_0050

    IL_004c:  ldc.i4.0
    IL_004d:  stloc.0
    IL_004e:  br.s       IL_0050

    IL_0050:  ldloc.0
    IL_0051:  ret
  } // end of method S2::CompareTo

} // end of class S2


.class private auto ansi beforefieldinit R2
       extends [mscorlib]System.Object
       implements GIComparable
{
  .field private int32 x
  .field private int32 y
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x,
                               int32 _y) cil managed
  {
    // Code size       21 (0x15)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 R2::x
    IL_000d:  ldarg.0
    IL_000e:  ldarg.2
    IL_000f:  stfld      int32 R2::y
    IL_0014:  ret
  } // end of method R2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo<T>(!!0 that) cil managed
  {
    // Code size       78 (0x4e)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 R2::x
    IL_0006:  ldarg.1
box  !!0
unbox.any  class R2

    IL_0007:  ldfld      int32 R2::x
    IL_000c:  bge.s      IL_0012

    IL_000e:  ldc.i4.m1
    IL_000f:  stloc.0
    IL_0010:  br.s       IL_004c

    IL_0012:  ldarg.0
    IL_0013:  ldfld      int32 R2::x
    IL_0018:  ldarg.1
box  !!0
unbox.any  class R2
    IL_0019:  ldfld      int32 R2::x
    IL_001e:  ble.s      IL_0024

    IL_0020:  ldc.i4.1
    IL_0021:  stloc.0
    IL_0022:  br.s       IL_004c

    IL_0024:  ldarg.0
    IL_0025:  ldfld      int32 R2::y
    IL_002a:  ldarg.1
box  !!0
unbox.any  class R2
    IL_002b:  ldfld      int32 R2::y
    IL_0030:  bge.s      IL_0036

    IL_0032:  ldc.i4.m1
    IL_0033:  stloc.0
    IL_0034:  br.s       IL_004c

    IL_0036:  ldarg.0
    IL_0037:  ldfld      int32 R2::y
    IL_003c:  ldarg.1
box  !!0
unbox.any  class R2
    IL_003d:  ldfld      int32 R2::y
    IL_0042:  ble.s      IL_0048

    IL_0044:  ldc.i4.1
    IL_0045:  stloc.0
    IL_0046:  br.s       IL_004c

    IL_0048:  ldc.i4.0
    IL_0049:  stloc.0
    IL_004a:  br.s       IL_004c

    IL_004c:  ldloc.0
    IL_004d:  ret
  } // end of method R2::CompareTo

} // end of class R2

.class private sequential ansi sealed beforefieldinit OS2
       extends [mscorlib]System.ValueType
       implements GIComparable
{
  .field private int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      int32 OS2::x
    IL_0007:  ret
  } // end of method OS2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo<T>(!!0 that) cil managed
  {
    // Code size       44 (0x2c)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 OS2::x
    IL_0006:  ldarg   that
box  !!0
unbox.any  OS2
    IL_0008:  ldfld      int32 OS2::x
    IL_000d:  bge.s      IL_0013

    IL_000f:  ldc.i4.m1
    IL_0010:  stloc.0
    IL_0011:  br.s       IL_002a

    IL_0013:  ldarg.0
    IL_0014:  ldfld      int32 OS2::x
    IL_0019:  ldarg   that
box  !!0
unbox.any  OS2
    IL_001b:  ldfld      int32 OS2::x
    IL_0020:  ble.s      IL_0026

    IL_0022:  ldc.i4.1
    IL_0023:  stloc.0
    IL_0024:  br.s       IL_002a

    IL_0026:  ldc.i4.0
    IL_0027:  stloc.0
    IL_0028:  br.s       IL_002a

    IL_002a:  ldloc.0
    IL_002b:  ret
  } // end of method OS2::CompareTo

} // end of class OS2

.class private auto ansi beforefieldinit OR2
       extends [mscorlib]System.Object
       implements GIComparable
{
  .field private int32 x
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(int32 _x) cil managed
  {
    // Code size       14 (0xe)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  stfld      int32 OR2::x
    IL_000d:  ret
  } // end of method OR2::.ctor

  .method public hidebysig newslot virtual final 
          instance int32  CompareTo<T>(!!0 that) cil managed
  {
    // Code size       42 (0x2a)
    .maxstack  2
    .locals init (int32 V_0)
    IL_0000:  ldarg.0
    IL_0001:  ldfld      int32 OR2::x
    IL_0006:  ldarg.1
box  !!0
unbox.any  class OR2
    IL_0007:  ldfld      int32 OR2::x
    IL_000c:  bge.s      IL_0012

    IL_000e:  ldc.i4.m1
    IL_000f:  stloc.0
    IL_0010:  br.s       IL_0028

    IL_0012:  ldarg.0
    IL_0013:  ldfld      int32 OR2::x
    IL_0018:  ldarg.1
box  !!0
unbox.any  class OR2
    IL_0019:  ldfld      int32 OR2::x
    IL_001e:  ble.s      IL_0024

    IL_0020:  ldc.i4.1
    IL_0021:  stloc.0
    IL_0022:  br.s       IL_0028

    IL_0024:  ldc.i4.0
    IL_0025:  stloc.0
    IL_0026:  br.s       IL_0028

    IL_0028:  ldloc.0
    IL_0029:  ret
  } // end of method OR2::CompareTo

} // end of class OR2

.class private auto ansi beforefieldinit Maine
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  Main(string[] args) cil managed
  {
    .entrypoint
    // Code size       2147 (0x863)
    .maxstack  5
    .locals init (int32 V_0,
             int32[] V_1,
             int32 V_2,
             int32 dummy3,
             int32 V_4,
             int32 V_5,
             int32 dummy6,
             int32 V_7,
             int32 dummy8,
             int32 V_9,
             int32 V_10,
             valuetype S2[] V_11,
             int32 V_12,
             class Finder2<valuetype S2> V_13,
             int32 V_14,
             int32 V_15,
             int32 dummy16,
             int32 V_17,
             int32 dummy18,
             int32 V_19,
             int32 V_20,
             class R2[] V_21,
             int32 V_22,
             class Finder2<class R2> V_23,
             int32 V_24,
             int32 V_25,
             int32 dummy26,
             int32 V_27,
             int32 dummy28,
             int32 V_29,
             int32 V_30,
             valuetype OS2[] V_31,
             int32 V_32,
             class Finder2<valuetype OS2> V_33,
             int32 V_34,
             int32 V_35,
             int32 dummy36,
             int32 V_37,
             int32 dummy38,
             int32 V_39,
             int32 V_40,
             class OR2[] V_41,
             int32 V_42,
             class Finder2<class OR2> V_43,
             int32 V_44,
             int32 V_45,
             int32 dummy46,
             int32 V_47,
             int32 dummy48,
             int32 V_49,
             int32 V_50,
             class OR2[] V_51,
             int32 V_52,
             class Finder2<class OR2> V_53,
             int32 V_54,
             int32 V_55,
             int32 V_56)
    IL_0000:  ldarg.0
    IL_0001:  ldlen
    IL_0002:  conv.i4
    IL_0003:  ldc.i4.1
    IL_0004:  bge.s      IL_001d

    IL_0006:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_000b:  ldstr      "Usage: perf <N>"
    IL_0010:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0015:  ldc.i4.1
    IL_0016:  stloc.s    V_56
    IL_0018:  br         IL_0860

    IL_001d:  ldarg.0
    IL_001e:  ldc.i4.0
    IL_001f:  ldelem.ref
    IL_0020:  call       int32 [mscorlib]System.Int32::Parse(string)
    IL_0025:  stloc.0

    IL_019f:  ldloc.0
    IL_01a0:  newarr     S2
    IL_01a5:  stloc.s    V_11
    IL_01a7:  ldc.i4.0
    IL_01a8:  stloc.s    V_12
    IL_01aa:  br.s       IL_01c4

    IL_01ac:  ldloc.s    V_11
    IL_01ae:  ldloc.s    V_12
    IL_01b0:  ldelema    S2
    IL_01b5:  ldloc.s    V_12
    IL_01b7:  ldloc.s    V_12
    IL_01b9:  call       instance void S2::.ctor(int32,
                                                 int32)
    IL_01be:  ldloc.s    V_12
    IL_01c0:  ldc.i4.1
    IL_01c1:  add
    IL_01c2:  stloc.s    V_12
    IL_01c4:  ldloc.s    V_12
    IL_01c6:  ldloc.0
    IL_01c7:  blt.s      IL_01ac

    IL_01c9:  newobj     instance void class Finder2<valuetype S2>::.ctor()
    IL_01ce:  stloc.s    V_13
    IL_01d0:  ldc.i4.0
    IL_01d1:  stloc.s    V_14
    IL_01d3:  br.s       IL_0207

    IL_01d5:  ldloc.s    V_13
    IL_01d7:  ldloc.s    V_14
    IL_01d9:  ldloc.s    V_14
    IL_01db:  newobj     instance void S2::.ctor(int32,
                                                 int32)
    IL_01e0:  ldloc.s    V_11
    IL_01e2:  callvirt   instance int32 class Finder2<valuetype S2>::Find(!0,
                                                                          !0[])
    IL_01e7:  ldloc.s    V_14
    IL_01e9:  beq.s      IL_0201

    IL_01eb:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_01f0:  ldstr      "Error finding {0} using Finder2<S2>.Find"
    IL_01f5:  ldloc.s    V_14
    IL_01f7:  box        [mscorlib]System.Int32
    IL_01fc:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0201:  ldloc.s    V_14
    IL_0203:  ldc.i4.1
    IL_0204:  add
    IL_0205:  stloc.s    V_14
    IL_0207:  ldloc.s    V_14
    IL_0209:  ldloc.0
    IL_020a:  blt.s      IL_01d5

    IL_020c:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0211:  ldstr      "Test PaSSeD (S2,1)"
    IL_0216:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_021b:  ldc.i4.0
    IL_021c:  stloc.s    V_15
    IL_021e:  br.s       IL_0250

    IL_0220:  ldloc.s    V_15
    IL_0222:  ldloc.s    V_15
    IL_0224:  newobj     instance void S2::.ctor(int32,
                                                 int32)
    IL_0229:  ldloc.s    V_11
    IL_022b:  call       int32 StaticFind2::Find<valuetype S2>(!!0,
                                                               !!0[])
    IL_0230:  ldloc.s    V_15
    IL_0232:  beq.s      IL_024a

    IL_0234:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0239:  ldstr      "Error finding {0} using StaticFind2.Find<S2>"
    IL_023e:  ldloc.s    V_15
    IL_0240:  box        [mscorlib]System.Int32
    IL_0245:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_024a:  ldloc.s    V_15
    IL_024c:  ldc.i4.1
    IL_024d:  add
    IL_024e:  stloc.s    V_15
    IL_0250:  ldloc.s    V_15
    IL_0252:  ldloc.0
    IL_0253:  blt.s      IL_0220

    IL_0255:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_025a:  ldstr      "Test PaSSeD (S2,2)"
    IL_025f:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0325:  ldloc.0
    IL_0326:  newarr     R2
    IL_032b:  stloc.s    V_21
    IL_032d:  ldc.i4.0
    IL_032e:  stloc.s    V_22
    IL_0330:  br.s       IL_0346

    IL_0332:  ldloc.s    V_21
    IL_0334:  ldloc.s    V_22
    IL_0336:  ldloc.s    V_22
    IL_0338:  ldloc.s    V_22
    IL_033a:  newobj     instance void R2::.ctor(int32,
                                                 int32)
    IL_033f:  stelem.ref
    IL_0340:  ldloc.s    V_22
    IL_0342:  ldc.i4.1
    IL_0343:  add
    IL_0344:  stloc.s    V_22
    IL_0346:  ldloc.s    V_22
    IL_0348:  ldloc.0
    IL_0349:  blt.s      IL_0332

    IL_034b:  newobj     instance void class Finder2<class R2>::.ctor()
    IL_0350:  stloc.s    V_23
    IL_0352:  ldc.i4.0
    IL_0353:  stloc.s    V_24
    IL_0355:  br.s       IL_0389

    IL_0357:  ldloc.s    V_23
    IL_0359:  ldloc.s    V_24
    IL_035b:  ldloc.s    V_24
    IL_035d:  newobj     instance void R2::.ctor(int32,
                                                 int32)
    IL_0362:  ldloc.s    V_21
    IL_0364:  callvirt   instance int32 class Finder2<class R2>::Find(!0,
                                                                      !0[])
    IL_0369:  ldloc.s    V_24
    IL_036b:  beq.s      IL_0383

    IL_036d:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0372:  ldstr      "Error finding {0} using Finder2<R2>.Find"
    IL_0377:  ldloc.s    V_24
    IL_0379:  box        [mscorlib]System.Int32
    IL_037e:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0383:  ldloc.s    V_24
    IL_0385:  ldc.i4.1
    IL_0386:  add
    IL_0387:  stloc.s    V_24
    IL_0389:  ldloc.s    V_24
    IL_038b:  ldloc.0
    IL_038c:  blt.s      IL_0357

    IL_038e:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0393:  ldstr      "Test PaSSeD (R2,1)"
    IL_0398:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_039d:  ldc.i4.0
    IL_039e:  stloc.s    V_25
    IL_03a0:  br.s       IL_03d2

    IL_03a2:  ldloc.s    V_25
    IL_03a4:  ldloc.s    V_25
    IL_03a6:  newobj     instance void R2::.ctor(int32,
                                                 int32)
    IL_03ab:  ldloc.s    V_21
    IL_03ad:  call       int32 StaticFind2::Find<class R2>(!!0,
                                                           !!0[])
    IL_03b2:  ldloc.s    V_25
    IL_03b4:  beq.s      IL_03cc

    IL_03b6:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_03bb:  ldstr      "Error finding {0} using StaticFind2.Find<R2>"
    IL_03c0:  ldloc.s    V_25
    IL_03c2:  box        [mscorlib]System.Int32
    IL_03c7:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_03cc:  ldloc.s    V_25
    IL_03ce:  ldc.i4.1
    IL_03cf:  add
    IL_03d0:  stloc.s    V_25
    IL_03d2:  ldloc.s    V_25
    IL_03d4:  ldloc.0
    IL_03d5:  blt.s      IL_03a2

    IL_03d7:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_03dc:  ldstr      "Test PaSSeD (R2,2)"
    IL_03e1:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)

    IL_04af:  ldloc.0
    IL_04b0:  newarr     OS2
    IL_04b5:  stloc.s    V_31
    IL_04b7:  ldc.i4.0
    IL_04b8:  stloc.s    V_32
    IL_04ba:  br.s       IL_04d2

    IL_04bc:  ldloc.s    V_31
    IL_04be:  ldloc.s    V_32
    IL_04c0:  ldelema    OS2
    IL_04c5:  ldloc.s    V_32
    IL_04c7:  call       instance void OS2::.ctor(int32)
    IL_04cc:  ldloc.s    V_32
    IL_04ce:  ldc.i4.1
    IL_04cf:  add
    IL_04d0:  stloc.s    V_32
    IL_04d2:  ldloc.s    V_32
    IL_04d4:  ldloc.0
    IL_04d5:  blt.s      IL_04bc

    IL_04d7:  newobj     instance void class Finder2<valuetype OS2>::.ctor()
    IL_04dc:  stloc.s    V_33
    IL_04de:  ldc.i4.0
    IL_04df:  stloc.s    V_34
    IL_04e1:  br.s       IL_0513

    IL_04e3:  ldloc.s    V_33
    IL_04e5:  ldloc.s    V_34
    IL_04e7:  newobj     instance void OS2::.ctor(int32)
    IL_04ec:  ldloc.s    V_31
    IL_04ee:  callvirt   instance int32 class Finder2<valuetype OS2>::Find(!0,
                                                                           !0[])
    IL_04f3:  ldloc.s    V_34
    IL_04f5:  beq.s      IL_050d

    IL_04f7:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_04fc:  ldstr      "Error finding {0} using Finder2<OS2>.Find"
    IL_0501:  ldloc.s    V_34
    IL_0503:  box        [mscorlib]System.Int32
    IL_0508:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_050d:  ldloc.s    V_34
    IL_050f:  ldc.i4.1
    IL_0510:  add
    IL_0511:  stloc.s    V_34
    IL_0513:  ldloc.s    V_34
    IL_0515:  ldloc.0
    IL_0516:  blt.s      IL_04e3

    IL_0518:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_051d:  ldstr      "Test PaSSeD (OS2,1)"
    IL_0522:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_0527:  ldc.i4.0
    IL_0528:  stloc.s    V_35
    IL_052a:  br.s       IL_055a

    IL_052c:  ldloc.s    V_35
    IL_052e:  newobj     instance void OS2::.ctor(int32)
    IL_0533:  ldloc.s    V_31
    IL_0535:  call       int32 StaticFind2::Find<valuetype OS2>(!!0,
                                                                !!0[])
    IL_053a:  ldloc.s    V_35
    IL_053c:  beq.s      IL_0554

    IL_053e:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0543:  ldstr      "Error finding {0} using StaticFind2.Find<OS2>"
    IL_0548:  ldloc.s    V_35
    IL_054a:  box        [mscorlib]System.Int32
    IL_054f:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0554:  ldloc.s    V_35
    IL_0556:  ldc.i4.1
    IL_0557:  add
    IL_0558:  stloc.s    V_35
    IL_055a:  ldloc.s    V_35
    IL_055c:  ldloc.0
    IL_055d:  blt.s      IL_052c

    IL_055f:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0564:  ldstr      "Test PaSSeD (OS2,2)"
    IL_0569:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)

    IL_0629:  ldloc.0
    IL_062a:  newarr     OR2
    IL_062f:  stloc.s    V_41
    IL_0631:  ldc.i4.0
    IL_0632:  stloc.s    V_42
    IL_0634:  br.s       IL_0648

    IL_0636:  ldloc.s    V_41
    IL_0638:  ldloc.s    V_42
    IL_063a:  ldloc.s    V_42
    IL_063c:  newobj     instance void OR2::.ctor(int32)
    IL_0641:  stelem.ref
    IL_0642:  ldloc.s    V_42
    IL_0644:  ldc.i4.1
    IL_0645:  add
    IL_0646:  stloc.s    V_42
    IL_0648:  ldloc.s    V_42
    IL_064a:  ldloc.0
    IL_064b:  blt.s      IL_0636

    IL_064d:  newobj     instance void class Finder2<class OR2>::.ctor()
    IL_0652:  stloc.s    V_43
    IL_0654:  ldc.i4.0
    IL_0655:  stloc.s    V_44
    IL_0657:  br.s       IL_0689

    IL_0659:  ldloc.s    V_43
    IL_065b:  ldloc.s    V_44
    IL_065d:  newobj     instance void OR2::.ctor(int32)
    IL_0662:  ldloc.s    V_41
    IL_0664:  callvirt   instance int32 class Finder2<class OR2>::Find(!0,
                                                                       !0[])
    IL_0669:  ldloc.s    V_44
    IL_066b:  beq.s      IL_0683

    IL_066d:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0672:  ldstr      "Error finding {0} using Finder2<OR2>.Find"
    IL_0677:  ldloc.s    V_44
    IL_0679:  box        [mscorlib]System.Int32
    IL_067e:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_0683:  ldloc.s    V_44
    IL_0685:  ldc.i4.1
    IL_0686:  add
    IL_0687:  stloc.s    V_44
    IL_0689:  ldloc.s    V_44
    IL_068b:  ldloc.0
    IL_068c:  blt.s      IL_0659

    IL_068e:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_0693:  ldstr      "Test PaSSeD (OR2,1)"
    IL_0698:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_069d:  ldc.i4.0
    IL_069e:  stloc.s    V_45
    IL_06a0:  br.s       IL_06d0

    IL_06a2:  ldloc.s    V_45
    IL_06a4:  newobj     instance void OR2::.ctor(int32)
    IL_06a9:  ldloc.s    V_41
    IL_06ab:  call       int32 StaticFind2::Find<class OR2>(!!0,
                                                            !!0[])
    IL_06b0:  ldloc.s    V_45
    IL_06b2:  beq.s      IL_06ca

    IL_06b4:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_06b9:  ldstr      "Error finding {0} using StaticFind2.Find<OR2>"
    IL_06be:  ldloc.s    V_45
    IL_06c0:  box        [mscorlib]System.Int32
    IL_06c5:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string,
                                                                                 object)
    IL_06ca:  ldloc.s    V_45
    IL_06cc:  ldc.i4.1
    IL_06cd:  add
    IL_06ce:  stloc.s    V_45
    IL_06d0:  ldloc.s    V_45
    IL_06d2:  ldloc.0
    IL_06d3:  blt.s      IL_06a2

    IL_06d5:  call       class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Error()
    IL_06da:  ldstr      "Test PaSSeD (OR2,2)"
    IL_06df:  callvirt   instance void [mscorlib]System.IO.TextWriter::WriteLine(string)
    IL_085a:  ldc.i4.s   0        // Pass
    IL_085c:  stloc.s    V_56
    IL_085e:  br.s       IL_0864

    IL_0860:  ldc.i4.s   1        // Fail
    IL_0862:  stloc.s    V_56
    
    IL_0864:  ldloc.s    V_56
    IL_0866:  ret
  } // end of method Maine::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method Maine::.ctor

} // end of class Maine


// =============================================================

//*********** DISASSEMBLY COMPLETE ***********************
// WARNING: Created Win32 resource file constrained-il.res
