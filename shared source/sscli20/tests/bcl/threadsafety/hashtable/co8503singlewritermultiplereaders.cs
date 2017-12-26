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
using System.Collections;
using System.Threading;
public class Co8503SingleWriterMultipleReaders
{
	const Int32 numberOfArrayListReads = 1000;
	const Int32 numberOfTableElements = 100;
	public static readonly String s_strActiveBugNums = "";
	public static readonly String s_strDtTmVer       = "";
	public static readonly String s_strClassMethod   = "Hashtable: Testing multiple Readers with a single writer";
	public static readonly String s_strTFName        = "Co8503SingleWriterMultipleReaders.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	private Hashtable table;
	public virtual bool runTest()
	{
		const Int32 iNumberOfThreads = 50;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc="123_er";
		Console.Out.Write( s_strClassMethod );
		Console.Out.Write( ": " );
		Console.Out.Write( s_strTFPath + s_strTFName );
		Console.Out.Write( ": " );
		Console.Out.Write( s_strDtTmVer );
		Console.Out.WriteLine( " runTest started..." );
		ThreadStart tdst1;				
		Thread[] thdPool;			
		try{
			strLoc="loc_375df";
			iCountTestcases++;
			table = new Hashtable();
			table.Add(0, 0);
			thdPool = new Thread[iNumberOfThreads];
			for(int i=0; i<iNumberOfThreads; i++){
				tdst1 = new ThreadStart(this.DoTheValueReading);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<iNumberOfThreads; i++)
				thdPool[i].Start();
			Thread.Sleep(200);
			for(int i=1; i<numberOfTableElements; i++)
				table.Add(i, i);
			for(int i=0; i<iNumberOfThreads; i++)
				thdPool[i].Join();
			strLoc="loc_3475sdg";
			iCountTestcases++;
			table = new Hashtable();
			table.Add(0, new Graph_Object(0));
			thdPool = new Thread[iNumberOfThreads];
			for(int i=0; i<iNumberOfThreads; i++){
				tdst1 = new ThreadStart(this.DoTheObjectReading);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<iNumberOfThreads; i++)
				thdPool[i].Start();
			Thread.Sleep(200);
			for(int i=1; i<numberOfTableElements; i++){
				table.Add(i, new Graph_Object(i));
			}
			for(int i=0; i<iNumberOfThreads; i++)
				thdPool[i].Join();
		}
		catch (Exception exc_general){
			++iCountErrors;
			Console.WriteLine( s_strTFAbbrev + "Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
		}
		if ( iCountErrors == 0 )
		{
			Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
			return true;
		}
		else
		{
			Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
			return false;
		}
	}
	private void DoTheValueReading(){
		Random random = new Random();
		Int32 iValue;
		while(table.Count < (numberOfTableElements - 2)){
			iValue = random.Next(table.Count - 1);
			if((Int32)table[iValue] != iValue){
				throw new Exception("Err_83gen! Sorry! wrong value read" );
			}
		}
	}
	private void DoTheObjectReading(){
		Random random = new Random();
		Int32 counter;
		while(table.Count < (numberOfTableElements - 2)){
			counter = random.Next(table.Count - 1);
			if(((Graph_Object)table[counter]).iValue != counter){
				throw new Exception("Err_9734tsg! Sorry! wrong value read" );
			}
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
		Co8503SingleWriterMultipleReaders oCbTest = new Co8503SingleWriterMultipleReaders();
		try
		{
			bResult = oCbTest.runTest();
		}
		catch ( Exception exc_main )
		{
			bResult = false;
			Console.WriteLine( s_strTFAbbrev + "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
		}
		if ( ! bResult )
		{
			Console.WriteLine( s_strTFAbbrev + s_strTFPath );
			Console.Error.WriteLine( " " );
			Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
			Console.Error.WriteLine( " " );
		}
		if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
	}
}
[Serializable]
internal class Graph_Object
{
	public int iValue;
	public String strValue;
	internal Graph_Object(int value){
		iValue = value;
		strValue = (((Int32)value)).ToString();
	}
}
