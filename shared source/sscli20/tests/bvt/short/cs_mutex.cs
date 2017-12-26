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
public class CO_Mutex {
 static Mutex gMutex;
 const int ITERS=100;
 static AutoResetEvent Event1 = new AutoResetEvent(false);
 static AutoResetEvent Event2 = new AutoResetEvent(false);
 public void T1start() {
 Console.WriteLine("In T1 start"); 
 for (int i = 0; i< ITERS; i++) {
 gMutex.WaitOne();
 Console.WriteLine("In one:"+i); 
 gMutex.ReleaseMutex();
 }
 Event1.Set();
 }
 public void T2start() {
 Console.WriteLine("In T2 start"); 
 for (int i = 0; i< ITERS; i++) {
 gMutex.WaitOne();
 Console.WriteLine("In two:"+i); 
 gMutex.ReleaseMutex();
 }
 Event2.Set();
 }
 public static int Main(String[] args) {
 Console.WriteLine("CO_Mutex.cs ...");
 gMutex = new Mutex(false,"MyMutex");
 try								{	gMutex.ReleaseMutex( );				}
 catch (ApplicationException)	{	Console.WriteLine("o.k. AppEx");	}
 CO_Mutex tm = new CO_Mutex();
 Thread	t1	= new Thread(new ThreadStart(tm.T1start));
 Thread	t2	= new Thread(new ThreadStart(tm.T2start));
 t1.Start();
 t2.Start();
 AutoResetEvent[] evs = new AutoResetEvent[2];
 evs[0] = Event1;
 evs[1] = Event2;
 WaitHandle.WaitAll(evs);
 Console.WriteLine("... CO_Mutex.cs");
 return 0;
 }
}
