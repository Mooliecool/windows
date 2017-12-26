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
 public static int		Point;
 public static int		lightSwitch1;
 public static int		lightSwitch2;
 public static float		Bounce1;
 public static float		Bounce2;
 public static Object	Obj1;
 public static Object	Obj2;
 public static int		Off			= 0;
 public static int		On			= 1;
 public static float		Down		= (float)9.0;
 public static float		Up			= (float)9.1;
 public static Thread	T;
 public static Context	C;
 public Fred( ) {
 T = Thread.CurrentThread;
 C = Thread.CurrentContext;
 Point = 10000;
 lightSwitch1 = Off;
 lightSwitch2 = Off;
 Bounce1 = Down;
 Bounce2 = Down;
 Obj1 = this;
 Obj2 = (Object)T;
 }
}
public class Dec {
 int			ID	= 0;
 Object		Obj;
 public Dec(Object obj)	{Obj=obj;}
 public void ThreadStart( ) {
 int		localID		= Interlocked.Increment(ref ID);
 for (int i=0;i<=700;i++) {
 Interlocked.CompareExchange(ref Fred.lightSwitch2,Fred.Off,Fred.On);
 Interlocked.CompareExchange(ref Fred.Bounce2,Fred.Down,Fred.Up);
 Interlocked.CompareExchange(ref Fred.Obj1,(Object)Fred.T,(Object)Fred.C);
 Interlocked.Exchange(ref Fred.lightSwitch1, Fred.Off);
 Interlocked.Exchange(ref Fred.Bounce1, Fred.Down);
 Interlocked.Exchange(ref Fred.Obj1, this);
 Interlocked.Decrement(ref Fred.Point);
 Thread.Sleep(0);
 }
 }
}
public class Inc {
 int			ID	= 0;
 Object		Obj;
 public Inc(Object obj)	{Obj=obj;}
 public void ThreadStart( ) {
 int		localID		= Interlocked.Increment(ref ID);
 for (int i=0;i<=700;i++) {
 Interlocked.CompareExchange(ref Fred.lightSwitch2,Fred.On,Fred.Off);
 Interlocked.CompareExchange(ref Fred.Bounce2,Fred.Up,Fred.Down);
 Interlocked.CompareExchange(ref Fred.Obj1,(Object)Fred.C,(Object)Fred.T);
 Interlocked.Exchange(ref Fred.lightSwitch1, Fred.On);
 Interlocked.Exchange(ref Fred.Bounce1, Fred.Up);
 Interlocked.Exchange(ref Fred.Obj1, this);
 Interlocked.Increment(ref Fred.Point);
 Thread.Sleep(0);
 }
 }
}
public class CO_Interlocked {
 public static void Main(String[] args) {
 Console.WriteLine("CO_Interlocked ...");
 Fred	f = new Fred( );
 Inc				inc				= new Inc(f);
 Dec				dec				= new Dec(f);
 Thread			Incer			= new Thread(new ThreadStart(inc.ThreadStart));
 Thread			Incer2			= new Thread(new ThreadStart(inc.ThreadStart));
 Thread			Decer			= new Thread(new ThreadStart(dec.ThreadStart));
 Thread			Decer2			= new Thread(new ThreadStart(dec.ThreadStart));
 Thread			Decer3			= new Thread(new ThreadStart(dec.ThreadStart));
 try	{
 Incer.Start( );
 Incer2.Start( );
 Decer.Start( );
 Decer2.Start( );
 Decer3.Start( );
 Incer.Join( );
 Incer2.Join( );
 Decer.Join( );
 Decer2.Join( );
 Decer3.Join( );
 }
 catch (ThreadStateException e)			{Console.WriteLine(e);}
 catch (ThreadInterruptedException e)	{Console.WriteLine(e);}
 Console.WriteLine("... CO_Interlocked");
 Environment.ExitCode = 0;
 return;
 }
}
