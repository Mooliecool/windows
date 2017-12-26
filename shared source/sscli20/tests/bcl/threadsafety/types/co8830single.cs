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
using System.IO;
using System.Threading;
public class Co8830Single
{
	public static readonly String s_strClassMethod   = "Single - threadsafety test";
	public static readonly String s_strTFName        = "Co8830Single.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	private Single instanceInt_1;
	
        private static Random randomNumGen;
        private static Single[] ValueArray = { -1.111f, 0.0f, 1.111f, 3.03f, 10.0e10f, 2.5f, 10.24f, 1.6f, 7e20f, 3.2e15f };
        const Int32 ValueArraySize = 10;

	public virtual bool runTest()
	{
		const Int32 numberOfThreads = 200;
		Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ThreadStart tdst1;				
		Thread[] thdPool;
		ThreadStart tdst2;
		Thread[] thdPool2;
                randomNumGen = new Random();
		try {
			strLoc = "Loc_001";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.WorkOnInstanceAndLocal_1);
				thdPool[i] = new Thread(tdst1);
			}							
			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(this.WorkOnInstanceAndLocal_2);
				thdPool2[i] = new Thread(tdst2);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Join();
			
		} catch (Exception exc_general ) {
		   ++iCountErrors;
		   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
		}
        if ( iCountErrors == 0 ){
           Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
           return true;
        }
        else{
           Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors );
           return false;
        }
    }
	private void WorkOnInstanceAndLocal_1()
	{
		Single localDouble; int i, j;
                for ( i = 0; i < 100; i++ )
                {
                   int index = randomNumGen.Next(0, ValueArraySize); 
		   instanceInt_1 = ValueArray[index];
                   Thread.Sleep(index);  
                   localDouble = instanceInt_1; 
                   for ( j = 0; j < ValueArraySize; j++ )
                     if ( ValueArray[j].CompareTo(localDouble) == 0 )
                          break;
                   if (j == ValueArraySize )
                     throw new Exception("WorkOnInstanceAndLocal: Atomicity of Read/Write violated - " + localDouble);
                }	
	}
	private void WorkOnInstanceAndLocal_2()
	{
		Single localDouble; int i, j;
                for ( i = 0; i < 100; i++ )
                {
                   int index = randomNumGen.Next(0, ValueArraySize); 
		   instanceInt_1 = ValueArray[index];
                   Thread.Sleep(index);  
                   localDouble = instanceInt_1; 
                   for ( j = 0; j < ValueArraySize; j++ )
                     if ( ValueArray[j].CompareTo(localDouble) == 0 )
                          break;
                   if (j == ValueArraySize )
                     throw new Exception("WorkOnInstanceAndLocal: Atomicity of Read/Write violated - " + localDouble);
                }	
	}
	
	
	static void MyHandler(object sender, UnhandledExceptionEventArgs args) 
	{
		Exception e = (Exception) args.ExceptionObject;
		Console.WriteLine("MyHandler caught : " + e.Message);
		Environment.Exit(1);
	}
	public static void Main(String[] args)
	{
		AppDomain currentDomain = AppDomain.CurrentDomain;
		currentDomain.UnhandledException += new UnhandledExceptionEventHandler(MyHandler);
		bool bResult = false;
	   Co8830Single cbA = new Co8830Single();
	   try {
	      bResult = cbA.runTest();
	   } catch (Exception exc_main){
	      bResult = false;
	      Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
	   }
	   if (!bResult)
	   {
	      Console.WriteLine(s_strTFName+ s_strTFPath);
	      Console.Error.WriteLine( " " );
	      Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
	      Console.Error.WriteLine( " " );
	   }
	   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
