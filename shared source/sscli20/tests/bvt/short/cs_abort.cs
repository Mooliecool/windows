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
public class AbortTest {
 public AbortTest( ) { }
 void Abortee1( ) {
 while(true) ;
 }
 void Abortee2( ) {
 try					{	while(true) ;	}
 catch (Exception e) {
 Console.WriteLine("__02:Catch attempting to swallow all exceptions");
 Console.WriteLine(e.ToString( ));
 }
 }
 void Abortee3( ) {
 try					{	while(true) ;	}
 finally {
 Console.WriteLine("__03:in finally doing a long computation ...");
 for (int i=0;i<3;i++) {
 Delay(1000);
 Console.Write("3");
 }
 Console.WriteLine( );
 Console.WriteLine("__03:Done");
 }
 }
 void Abortee4( ) {
 try					{	while(true) ;	}
 catch (Exception e) {
 Console.WriteLine("__04:in catch attempting to swallow all exceptions");
 Console.WriteLine(e.ToString( ));
 }
 finally {
 Console.WriteLine("__04:in finally doing a Delay(1000) ...");
 Delay(1000);
 Console.WriteLine("__04:Done");
 }
 }
 void Abortee5( ) {
 try				{	int		a	= gNum/gDenom;	}
 catch (ArithmeticException) {
 Console.WriteLine("__05:Got arithmetic exception");
 try {
 AbortStart.Set( );
 Console.WriteLine("__05:waiting for someone to abort");
 AbortSent.WaitOne( );
 Console.WriteLine("__05:waiting some more to make sure we are not interrupted");
 for (int i=0;i<3;i++) {
 Delay(5000);
 Console.Write("5");
 }
 Console.WriteLine( );
 Console.WriteLine("__05:Done");
 }
 catch (Exception) {
 Console.WriteLine("__05:Should get here");
 }
 Console.WriteLine("__05:ResetingAbort to defeat the abort");
 Thread.ResetAbort( );
 }			
 Console.WriteLine("__05:Should see this after calling Thread.ResetAbort( )");
 }
 void Abortee6( ) {
 try				{	int		a	= gNum/gDenom;	}
 catch (ArithmeticException) {
 Console.WriteLine("__06:Got arithmetic exception");
 try {
 AbortStart.Set( );
 Console.WriteLine("__06:waiting for someone to abort");
 AbortSent.WaitOne( );
 Console.WriteLine("__06:waiting some more to make sure we are not interrupted");
 for (int i=0;i<3;i++) {
 Delay(1000);
 Console.Write("6");
 }
 Console.WriteLine( );
 Console.WriteLine("__06:Done");
 }
 catch (ArithmeticException) {
 Console.WriteLine("%_06:Should not get here");
 retCode = 6;
 }
 }
 Console.WriteLine("%_06:Should not see this ");
 retCode = 6;
 }
 void Abortee7( ) {
 try {
 try			{	int		a	= gNum/gDenom;	}
 finally {
 Console.WriteLine("__07:in finally doing a long computation ...");
 for (int i=0;i<5;i++) {
 Delay(1000);
 Console.Write("7");
 }
 Console.WriteLine( );
 Console.WriteLine("__07:done");
 }
 }
 catch (ArithmeticException) {
 Console.WriteLine("__07:Caught arithmetic exception");
 }
 Console.WriteLine("%_07:Should not see this ");
 retCode = 7;
 }
 void Abortee8( ) {
 try {
 try			{	int		a	= gNum/gDenom;	}
 finally {
 Console.WriteLine("__08:in finally doing a DoLongComputation(5) ...");
 try {
 DoLongComputation(5);
 int		a	= gNum/gDenom;
 }
 catch (ArithmeticException) {
 Console.WriteLine("__08:Caught arithmetic exception 2");
 }
 }
 }
 catch (ArithmeticException) {
 Console.WriteLine("__08:Caught arithmetic exception");
 }
 Console.WriteLine("%_08:Should not see this ");
 retCode = 8;
 }
 void Abortee9( ) {
 try					{	Abortee8( );	}
 catch(Exception) {
 Console.WriteLine("__09:Attempt to catch all exception");
 }
 while (true) ;
 }	
 void Abortee10( ) {
 try {
 try		{	while(true) ;				}
 finally {	int		a	= gNum/gDenom;	}
 }
 catch (Exception) {
 Console.WriteLine("__10:Attempt to catch all exception");
 }
 }
 void Abortee11( ) {
 Console.WriteLine("%_11:should not see this");
 retCode = 11;
 }
 void Abortee12( ) {
 Event1.WaitOne( );
 }
 public static void Aborter1(ThreadStart abortee) {
 Thread			t	= new Thread(abortee);
 t.Start( );
 Delay(500);
 Console.WriteLine("A__1:Aborting t1");
 t.Abort( );
 t.Join( );
 Console.WriteLine("A__1:Test passed");
 Console.WriteLine("    :***********************************");
 }
 public static void Aborter2(ThreadStart abortee) {
 Thread			t	= new Thread(abortee);
 t.Start( );
 AbortStart.WaitOne( );
 Console.WriteLine("A__2:Aborting t1");
 t.Abort(t);
 t.Abort(t);
 t.Abort(t);
 AbortSent.Set( );
 t.Join( );
 Console.WriteLine("A__2:Test passed");
 Console.WriteLine("    :***********************************");
 }
 public static void Aborter3(ThreadStart abortee) {
 try		{
 Thread			t	= new Thread(abortee);
 t.Abort( );
 t.Start( );
 t.Join( );
 }
 catch (Exception) {
 }
 Console.WriteLine("A__3:Test passed");
 Console.WriteLine("    :***********************************");
 }
 public static void Aborter4(ThreadStart abortee) {
 Thread			t	= new Thread(abortee);
 t.Start( );
 t.Abort( );
 t.Join( );
 t.Abort( );
 Console.WriteLine("A__4:Test passed");
 Console.WriteLine("    :***********************************");
 }
 private void AbortTest1( ) {
 Console.WriteLine("AT01:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee1);
 Aborter1(ts);
 }	
 private void AbortTest2( ) {
 Console.WriteLine("AT02:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee2);
 Aborter1(ts);
 }	
 private void AbortTest3( ) {
 Console.WriteLine("AT03:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee3);
 Aborter1(ts);
 }
 private void AbortTest4( ) {
 Console.WriteLine("AT04:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee4);
 Aborter1(ts);
 }	
 private void AbortTest5( ) {
 Console.WriteLine("AT05:Start Test (A__2)");
 ThreadStart		ts	= new ThreadStart(this.Abortee5);
 Aborter2(ts);
 }	
 private void AbortTest6( ) {
 Console.WriteLine("AT06:Start Test (A__2)");
 ThreadStart		ts	= new ThreadStart(this.Abortee6);
 Aborter2(ts);
 }	
 private void AbortTest7( ) {
 Console.WriteLine("AT07:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee7);
 Aborter1(ts);
 }	
 private void AbortTest8( ) {
 Console.WriteLine("AT08:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee8);
 Aborter1(ts);
 }	
 private void AbortTest9( ) {
 Console.WriteLine("AT09:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee9);
 Aborter1(ts);
 }	
 private void AbortTest10( ) {
 Console.WriteLine("AT10:Start Test (A__1)");
 ThreadStart		ts	= new ThreadStart(this.Abortee10);
 Aborter1(ts);
 }	
 private void AbortTest11( ) {
 Console.WriteLine("AT11:Start Test (A__3)");
 ThreadStart		ts	= new ThreadStart(this.Abortee11);
 Aborter3(ts);
 }	
 private void AbortTest12( ) {
 Console.WriteLine("AT12:Start Test");
 ThreadStart		ts	= new ThreadStart(this.Abortee12);
 Thread			t	= new Thread(ts);
 t.Start( );
 Delay(500);
 Console.WriteLine("AT12:suspending t1");
 t.Suspend( );
 Delay(50000);
 }	
 private void AbortTest13( ) {
 Console.WriteLine("AT13:Start Test (A__4)");
 ThreadStart		ts	= new ThreadStart(this.Abortee1);
 Aborter4(ts);
 }	
 private static int				retCode		= 0;
 private static int				gNum		= 1;
 private static int				gDenom		= 0;
 private static AutoResetEvent	Event1		= new AutoResetEvent(false);
 private static AutoResetEvent	AbortStart	= new AutoResetEvent(false);
 private static AutoResetEvent	AbortSent	= new AutoResetEvent(false);
 public static void Delay(int milliseconds) {
 Event1.WaitOne(milliseconds,false);
 }
 private static void DoLongComputation(int n) {
 Console.WriteLine("_DLC:long compute inside finally");
 for (int i=0;i<n;i++) {
 Delay(1000);
 Console.Write("d");
 }
 Console.WriteLine( );
 Console.WriteLine("_DLC:done");
 }
 public static void Main(String[] args) {
 AbortTest	g	= new AbortTest();
 g.AbortTest1();
 g.AbortTest2();
 g.AbortTest3();
 g.AbortTest4();
 g.AbortTest5();
 g.AbortTest6();
 g.AbortTest7();
 g.AbortTest8();
 g.AbortTest9();
 g.AbortTest10();
 g.AbortTest11();
 g.AbortTest13();
 Environment.ExitCode = retCode;
 Console.WriteLine("retCode=={0}",retCode);
 }
}
