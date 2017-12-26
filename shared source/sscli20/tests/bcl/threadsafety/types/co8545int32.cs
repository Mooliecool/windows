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
public class Co8545Int32
{
	public static readonly String s_strActiveBugNums = "";
	public static readonly String s_strDtTmVer       = "";
	public static readonly String s_strClassMethod   = "Int32 - threadsafety test";
	public static readonly String s_strTFName        = "Co8545Int32.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	private Int32 instanceInt_1;
	private static Int32 staticInt_1;
        private static Random randomNumGen;
        private static Int32[] ValueArray = { -1, 0, 1, 303, 10, 2, 1024, 16, 7, 32 };
        const Int32 ValueArraySize = 10;
	public virtual bool runTest()
	{
		const Int32 numberOfThreads = 200;
		Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ThreadStart tdst1;				
		Thread[] thdPool;
                randomNumGen = new Random();
		try {
			strLoc = "Loc_001";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.WorkOnInstanceAndLocal);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<numberOfThreads; i++)
			        thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
			strLoc = "Loc_002";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(Co8545Int32.WorkOnStatic);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
		} catch (Exception exc_general ) {
		   ++iCountErrors;
		   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
		}
        if ( iCountErrors == 0 ){
           Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
           return true;
        }
        else{
           Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" ,BugNums?: "+s_strActiveBugNums );
           return false;
        }
    }
	private void WorkOnInstanceAndLocal()
	{
                int localInt, i, j;
                for ( i = 0; i < 100; i++ )
                {
                   int index = randomNumGen.Next(0, ValueArraySize); 
		   instanceInt_1 = ValueArray[index];
                   Thread.Sleep(index);  
                   localInt = instanceInt_1; 
                   for ( j = 0; j < ValueArraySize; j++ )
                     if ( ValueArray[j] == localInt )
                          break;
                   if (j == ValueArraySize )
                     throw new Exception("WorkOnInstanceAndLocal: Atomicity of Read/Write violated - " + localInt);
                }	
	}
	private static void WorkOnStatic()
	{
		int localInt, j;
                for ( int i = 0; i < 10; i++ )
                {
                   int index = randomNumGen.Next(0, ValueArraySize); 
		   staticInt_1 = ValueArray[index];
                   Thread.Sleep(index); 
                   localInt = staticInt_1;
                   for ( j = 0; j < ValueArraySize; j++ )
                     if ( ValueArray[j] == localInt )
                          break;
                   if (j == ValueArraySize )
                      throw new Exception("WorkOnStatic: Atomicity of Read/Write violated");
                }		
	}	
	static void MyHandler(object sender, UnhandledExceptionEventArgs args) 
	{
		Exception e = (Exception) args.ExceptionObject;
		Console.WriteLine("Unhandled exception caught : " + e.Message);
		Environment.Exit(1);
	}
	public static void Main(String[] args)
	{
		AppDomain currentDomain = AppDomain.CurrentDomain;
		currentDomain.UnhandledException += new UnhandledExceptionEventHandler(MyHandler);
		bool bResult = false;
	   Co8545Int32 cbA = new Co8545Int32();
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
