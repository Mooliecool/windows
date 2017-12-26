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
using System.Text;
public class CO_TryFinally {
 public int	levelsDeep;
 public int	LEVELS		= 8;
 public CO_TryFinally( )	{	levelsDeep=0;	}
 public void run3( ) {
 try		{	while (true) ;						}
 finally	{	Console.WriteLine("3: finally 1");	}
 }
 public void run2( ) {
 try {
 levelsDeep++;
 if (levelsDeep!=LEVELS)		run2( );
 while (true) ;
 }
 finally {	Console.WriteLine("2: finally {0}",levelsDeep--);	}
 }
 public void run( ) {
 try {
 try{
 try{
 try{
 try{
 try{
 try{
 try{
 while (true)  ;
 }
 finally {	Console.WriteLine("1: finally 1");	}
 }
 finally {	Console.WriteLine("1: finally 2");	}
 }
 finally {	Console.WriteLine("1: finally 3");	}
 }
 finally {	Console.WriteLine("1: finally 4");	}
 }
 finally {	Console.WriteLine("1: finally 5");	}
 }
 finally {	Console.WriteLine("1: finally 6");	}
 }
 finally {	Console.WriteLine("1: finally 7");	}
 }
 finally {	Console.WriteLine("1: finally 8");	}
 }
 public static int Main(String[] args) {
 CO_TryFinally	T	= new CO_TryFinally();
 Thread			t1	= new Thread(new ThreadStart(T.run));
 Thread			t2	= new Thread(new ThreadStart(T.run2));
 Thread			t3	= new Thread(new ThreadStart(T.run3));
 t1.Start();
 t2.Start();
 t3.Start();
 Thread.Sleep(2000);
 Console.WriteLine("1: Abort( )");
 t1.Abort();
 t1.Join();
 Console.WriteLine("2: Abort( )");
 t2.Abort();
 t2.Join();
 Console.WriteLine("3: Abort( )");
 t3.Abort();
 t3.Join();
 return 0;
 }
}
