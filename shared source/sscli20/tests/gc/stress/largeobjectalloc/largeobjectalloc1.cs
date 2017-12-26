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

// LargeObjectAlloc1.cs
// Allocate arrays from 20KB to 25MB, 200 times
// If memory is low, after every loop, the large objects should be collected
// and committed from the LargeObjectHeap


using System;
public class Test
{
	public static void Main(string[] args)
	{
		int loop = 0;
		byte []junk;
		
		Console.WriteLine("Test should return ExitCode 100\n");
		while(loop<=200)
		{
		  	Console.Write("LOOP: {0}",loop);
			for(int size=20000;size<=5242880*5;size+=1024*1024) {
			  try
			  {
			   junk = new byte[size];
			   //Console.WriteLine("Allocated Size: {0}",size);
			   Console.Write(".");
			  }
			  catch (Exception e)
			  {
			    Console.WriteLine("Failure to allocate "+size+" at loop "+loop);
			    Console.WriteLine("Caught Exception: {0}",e);
			    Environment.ExitCode=1;
			    return;
			  }
			}
		       loop++;
		       Console.WriteLine("\n");
		}
		Environment.ExitCode=0;
		Console.WriteLine("Test Passed");
	
	}

}
