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

public class CO_Mutex2 {
 static Mutex gM1;
 static Mutex gM2;
 const int ITERS=100;
 static AutoResetEvent Event1 = new AutoResetEvent(false);
 static AutoResetEvent Event2 = new AutoResetEvent(false);
 static AutoResetEvent Event3 = new AutoResetEvent(false);
 static AutoResetEvent Event4 = new AutoResetEvent(false);
	
 public void T1start() {
  Console.WriteLine("In T1 start, Mutex.WaitAll(Mutex[])");
  Mutex[] gMs = new Mutex[2];
  gMs[0] = gM1; gMs[1] = gM2;
  Mutex.WaitAll(gMs);
  Thread.Sleep(2000);
  Console.WriteLine("T1 finished, Mutex.WaitAll(Mutex[])");
  Event1.Set();
  gM1.ReleaseMutex();
  gM2.ReleaseMutex();
 } 
 public void T2start() {
  Console.WriteLine("In T2 start, gM1.WaitOne( )");
  gM1.WaitOne();
  Console.WriteLine("T2 finished, gM1.WaitOne( )");
  Event2.Set();
  gM1.ReleaseMutex();
 }
 public void T3start() {
  Console.WriteLine("In T3 start, Mutex.WaitAny(Mutex[])");
  Mutex[] gMs = new Mutex[2];
  gMs[0] = gM1; gMs[1] = gM2;
  Mutex.WaitAny(gMs);
  Console.WriteLine("T3 finished, Mutex.WaitAny(Mutex[])");
  Event3.Set();
  gM1.ReleaseMutex();
 }
 public void T4start() {
  Console.WriteLine("In T4 start, gM2.WaitOne( )");
  gM2.WaitOne();
  Console.WriteLine("T4 finished, gM2.WaitOne( )");
  Event4.Set();
  gM2.ReleaseMutex();
 }

 public static int Main(String[] args) {
  Console.WriteLine("CO_Mutex2.cs ...");
  gM1 = new Mutex(true,"MyMutex");
  gM2 = new Mutex(true);
  CO_Mutex2 tm = new CO_Mutex2();
  Console.WriteLine(" - Main Owns gM1 and gM2");
  Thread t1= new Thread(new ThreadStart(tm.T1start));
  Thread t2= new Thread(new ThreadStart(tm.T2start));
  Thread t3= new Thread(new ThreadStart(tm.T3start));
  Thread t4= new Thread(new ThreadStart(tm.T4start));
  t1.Start();
  t2.Start();
  t3.Start();
  t4.Start();
  Thread.Sleep(2000);
  Console.WriteLine(" - Main releases gM1");
  gM1.ReleaseMutex();
  Thread.Sleep(1000);
  Console.WriteLine(" - Main releases gM2");
  gM2.ReleaseMutex();
  AutoResetEvent[] evs = new AutoResetEvent[4];
  evs[0] = Event1;
  evs[1] = Event2;
  evs[2] = Event3;
  evs[3] = Event4;
  WaitHandle.WaitAll(evs);
  Console.WriteLine("... CO_Mutex2.cs");
  return 0;
 }
}
