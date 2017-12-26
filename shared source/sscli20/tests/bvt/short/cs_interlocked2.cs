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
using System;
using System.Threading;
using System.Runtime.Remoting.Contexts;
public class Fred {
 public static long		L;
 public static int		I;
 public static long		L2;
 public static int		I2;
 public Fred( ) {
 L = 2147483647L;
 I = 2147483647;
 L2 = 0L;
 I2 = 0;
 }
}
public class CO_Interlocked {
 public static void Main(String[] args) {
 int		ec	= 0;
 Console.WriteLine("CO_Interlocked2 ...");
 Console.WriteLine("--------------------------------");
 Fred	f	= new Fred( );
 int		i	= 0;
 long	l	= 0L;
 for (int ii=0;ii<2;ii++) {
 i = Interlocked.Decrement(ref Fred.I);
 l = Interlocked.Decrement(ref Fred.L);
 Console.WriteLine("   Int32 {0}  {1}",i,Fred.I);
 Console.WriteLine("   Int64 {0}  {1}",l,Fred.L);
 }
 Console.WriteLine( );
 for (int ii=0;ii<4;ii++) {
 i = Interlocked.Increment(ref Fred.I);
 l = Interlocked.Increment(ref Fred.L);
 Console.WriteLine("   Int32 {0}  {1}",i,Fred.I);
 if (l!=Fred.L) {
 Console.Write("** ");
 ec = 99;
 }
 else
   Console.Write("   ");
 Console.WriteLine("Int64 {0}  {1}",l,Fred.L);
 }
 Console.WriteLine("********************************");
 for (int ii=0;ii<2;ii++) {
 i = Interlocked.Increment(ref Fred.I2);
 l = Interlocked.Increment(ref Fred.L2);
 Console.WriteLine("   Int32 {0}  {1}",i,Fred.I2);
 Console.WriteLine("   Int64 {0}  {1}",l,Fred.L2);
 }
 Console.WriteLine( );
 for (int ii=0;ii<4;ii++) {
 i = Interlocked.Decrement(ref Fred.I2);
 l = Interlocked.Decrement(ref Fred.L2);
 Console.WriteLine("   Int32 {0}  {1}",i,Fred.I2);
 if (l!=Fred.L2) {
 Console.Write("** ");
 ec = 99;
 }
 else
   Console.Write("   ");
 Console.WriteLine("Int64 {0}  {1}",l,Fred.L2);
 }
 Console.WriteLine("++++++++++++++++++++++++++++++++");
 Console.WriteLine("... CO_Interlocked2 ExitCode({0})",ec);
 Environment.ExitCode = ec;
 return;
 }
}
