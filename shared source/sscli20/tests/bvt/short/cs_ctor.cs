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
class CtorProc {
 public void ThreadStart() {
 try {
 Thread.Sleep(500);
 }
 catch(ThreadInterruptedException) {
 Console.WriteLine("ThreadInterruptedException");
 }
 }
}
public class CO_Ctor {
 public static int Main(String [] args) {
 int			result		= 0;
 CtorProc	thread_proc	= new CtorProc();
 Console.WriteLine("CS_ctor ...");
 Console.WriteLine("Scenario 1.: Call only Thread constructor");
 try {
 Thread	thread_1	= new Thread(new ThreadStart(thread_proc.ThreadStart));
 }
 catch(ArgumentException) {
 Console.WriteLine("Thread ctor throws ArgumentException in Scenario 1.");
 result = 1;
 }
 Console.WriteLine("Scenario 2.: Construct Thread and run");
 try {
 Thread	thread_2	= new Thread(new ThreadStart(thread_proc.ThreadStart));
 thread_2.Start();
 thread_2.Join();
 }
 catch(ThreadStateException) {
 Console.WriteLine("Thread ctor throws ThreadStateException in Scenario 2.");
 result = 2;
 }
 catch(ArgumentException) {
 Console.WriteLine("Thread ctor throws ArgumentException in Scenario 2.");
 result = 2;
 }
 catch(ThreadInterruptedException) {
 Console.WriteLine("Thread ctor throws ThreadInterruptedException in Scenario 2.");
 result = 2;
 }
 Console.WriteLine("Scenario 3.: Call Thread constructor two times");
 try {
 Thread	thread_3	= new Thread(new ThreadStart(thread_proc.ThreadStart));
 thread_3 = new Thread(new ThreadStart(thread_proc.ThreadStart));
 }
 catch(ArgumentException) {
 Console.WriteLine("Thread ctor throws ArgumentException in Scenario 3.");
 result = 3;
 }
 Console.WriteLine("Scenario 4.: Construct new Thread after Start()");
 try {
 Thread	thread_4	= new Thread(new ThreadStart(thread_proc.ThreadStart));
 thread_4.Start();
 thread_4 = new Thread(new ThreadStart(thread_proc.ThreadStart));
 thread_4.Join();
 Console.WriteLine("Thread didn't throw ThreadStateException in Scenario 4.");
 result = 4;
 }
 catch(ThreadStateException) {
 }
 catch(ThreadInterruptedException) {
 Console.WriteLine("Thread ctor throws ThreadInterruptedException in Scenario 4.");
 result = 4;
 }
 Console.WriteLine("Scenario 5.: ThreadStart == null");
 try {
 Thread	thread_5	= new Thread((ThreadStart)null);
 Console.WriteLine("Thread didn't throw ArgumentException in Scenario 5.");
 result = 5;
 }
 catch(ArgumentException) {
 }
 Console.WriteLine("... CS_ctor");
 Environment.ExitCode = result;
 return result;
 }
}
