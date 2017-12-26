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
public class MutexSample
{
 static Mutex			gM1;
 static Mutex			gM2;
 const int				ITERS	= 100;
 static AutoResetEvent	Event1	= new AutoResetEvent(false);
 static AutoResetEvent	Event2	= new AutoResetEvent(false);
 static AutoResetEvent	Event3	= new AutoResetEvent(false);
 static AutoResetEvent	Event4	= new AutoResetEvent(false);
 public static int Main(String[] args)
   {
   Console.WriteLine("MutexSample.cs ...");
   gM1 = new Mutex(true,"MyMutex");			
   gM2 = new Mutex(true);						
   Console.WriteLine(" - Main Owns gM1 and gM2");
   AutoResetEvent[]	evs	= new AutoResetEvent[4];
   evs[0] = Event1;			
   evs[1] = Event2;			
   evs[2] = Event3;			
   evs[3] = Event4;			
   MutexSample			tm	= new MutexSample( );
   Thread				t1	= new Thread(new ThreadStart(tm.t1Start));
   Thread				t2	= new Thread(new ThreadStart(tm.t2Start));
   Thread				t3	= new Thread(new ThreadStart(tm.t3Start));
   Thread				t4	= new Thread(new ThreadStart(tm.t4Start));
   t1.Start( );				
   t2.Start( );				
   t3.Start( );				
   t4.Start( );				
   Thread.Sleep(2000);
   Console.WriteLine(" - Main releases gM1");
   gM1.ReleaseMutex( );		
   Thread.Sleep(1000);
   Console.WriteLine(" - Main releases gM2");
   gM2.ReleaseMutex( );		
   WaitHandle.WaitAll(evs);	
   Console.WriteLine("... MutexSample.cs");
   return 0;
   }
 public void t1Start( )
   {
   Console.WriteLine("t1Start started,  Mutex.WaitAll(Mutex[])");
   Mutex[]				gMs	= new Mutex[2];
   gMs[0] = gM1;				
   gMs[1] = gM2;
   Mutex.WaitAll(gMs);			
   Thread.Sleep(2000);
   Console.WriteLine("t1Start finished, Mutex.WaitAll(Mutex[])");
   Event1.Set( );
   gMs[0].ReleaseMutex();
   gMs[1].ReleaseMutex();				
   }
 public void t2Start( )
   {
   Console.WriteLine("t2Start started,  gM1.WaitOne( )");
   gM1.WaitOne( );				
   Console.WriteLine("t2Start finished, gM1.WaitOne( )");
   Event2.Set( );
   gM1.ReleaseMutex();				
   }
 public void t3Start( )
   {
   Console.WriteLine("t3Start started,  Mutex.WaitAny(Mutex[])");
   Mutex[]				gMs	= new Mutex[2];
   gMs[0] = gM1;				
   gMs[1] = gM2;
   Mutex.WaitAny(gMs);			
   Console.WriteLine("t3Start finished, Mutex.WaitAny(Mutex[])");
   Event3.Set( );
   gMs[0].ReleaseMutex();				
   }
 public void t4Start( )
   {
   Console.WriteLine("t4Start started,  gM2.WaitOne( )");
   gM2.WaitOne( );				
   Console.WriteLine("t4Start finished, gM2.WaitOne( )");
   Event4.Set( );
   gM2.ReleaseMutex();				
   }
}
