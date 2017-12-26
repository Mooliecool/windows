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
public class Co8551Byte
{
	public static readonly String s_strClassMethod   = "Byte - threadsafety test";
	public static readonly String s_strTFName        = "Co8551Byte.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	private Byte instanceInt_1;
	private static Byte staticInt_1;
	
        private static Random randomNumGen;
        private static Byte[] ValueArray = { 96, 0, 1, 127, 10, 2, 102, 16, 7, 32 };
        const Byte ValueArraySize = 10;

	public virtual bool runTest()
	{
		const Byte numberOfThreads = 200;
		Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod);
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
			strLoc = "Loc_003df";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(Co8551Byte.WorkOnStatic);
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
           Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors);
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

		Byte localInt_1;
		localInt_1 = 1 + 1;
		if(localInt_1 != 2)
			throw new Exception("Loc_7453fg! Major Error here");
		localInt_1 = Byte.MaxValue + Byte.MinValue;
		if(localInt_1 != Byte.MaxValue)
			throw new Exception("Loc_98745sg! Major Error here");
		localInt_1 = 100 * 2;
		if(localInt_1 != 200)
			throw new Exception("Loc_3975sg! Major Error here, " + localInt_1);
		localInt_1 = Byte.Parse("32");
		if(localInt_1 != 32)
			throw new Exception("Loc_975esg! Major Error here, " + localInt_1);
		localInt_1 = Byte.MaxValue;
		if(localInt_1.GetHashCode() != 255)
			throw new Exception("Loc_874325dg! Major Error here, " + localInt_1.GetHashCode());
		if(localInt_1.ToString() != "255")
			throw new Exception("Loc_8754s! Major Error here, " + localInt_1.ToString());
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
		Console.WriteLine("MyHandler caught : " + e.Message);
		Environment.Exit(1);
	}
	public static void Main(String[] args)
	{
		AppDomain currentDomain = AppDomain.CurrentDomain;
		currentDomain.UnhandledException += new UnhandledExceptionEventHandler(MyHandler);
		bool bResult = false;
	   Co8551Byte cbA = new Co8551Byte();
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
