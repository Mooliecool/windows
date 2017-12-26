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
using System.Text;
public class Co8788StringBuilder
{
	private const Int32 numberOfThreads = 200;
	public static readonly String s_strClassMethod   = "StringBuilder - threadsafety test";
	public static readonly String s_strTFName        = "Co8788StringBuilder.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	private StringBuilder instanceInt_1;
	private static StringBuilder staticInt_1;
	private static Boolean debug = false;
	private static Boolean state_Append = false;
	private static Boolean state_AppendFormat = false;
	private static Boolean state_Insert = false;
	private static Boolean state_Replace = false;
	private static Boolean state_OtherMethods = false;
	private static Int32 threadCount = 0;
	public virtual bool runTest()
	{
		Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod );
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ThreadStart tdst1;				
		Thread[] thdPool1;
		ThreadStart tdst2;
		Thread[] thdPool2;
		Co8788StringBuilder[] targets;
		String instanceValue;
		try {
			strLoc = "Loc_001";
			iCountTestcases++;
			instanceInt_1 = new StringBuilder();
			thdPool1 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.WorkOnInstance);
				thdPool1[i] = new Thread(tdst1);
			}

			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(this.WorkOnInstance_2);
				thdPool2[i] = new Thread(tdst2);
			}

			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool1[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool1[i].Join();
			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Join();

			instanceValue = instanceInt_1.ToString();
			strLoc = "Loc_002";
			iCountTestcases++;
			staticInt_1 = new StringBuilder();
			targets = new Co8788StringBuilder[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++)
				targets[i] = new Co8788StringBuilder();
			thdPool1 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(targets[i].WorkOnStatic);
				thdPool1[i] = new Thread(tdst1);
			}
			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(targets[i].WorkOnStatic_2);
				thdPool2[i] = new Thread(tdst2);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool1[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool1[i].Join();
			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Join();
			instanceValue = staticInt_1.ToString();
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
	private void WorkOnInstance()
	{
		String strLoc = "Loc_000oo";
		String temp;
		Int32 len;
		Int32 iTmp;
		Char chTmp;
		Boolean fTmp;
		try{
			strLoc = "Loc_8734sg!";
			instanceInt_1.Append(Int16.MinValue);
			instanceInt_1.Append(Int32.MaxValue);
			instanceInt_1.Append(Int64.MaxValue);
			instanceInt_1.Append(true);
			instanceInt_1.Append('a');
			instanceInt_1.Append('a', 1000);
			instanceInt_1.Append("Hello World");
			instanceInt_1.Append("Hello World", 2, 5);
			instanceInt_1.Append(Byte.MaxValue);
			temp = instanceInt_1.ToString();
			instanceInt_1.Append(SByte.MaxValue);
			instanceInt_1.Append(UInt16.MaxValue);
			instanceInt_1.Append(UInt32.MaxValue);
			instanceInt_1.Append(UInt64.MaxValue);
			instanceInt_1.Append(Single.MaxValue);
			instanceInt_1.Append(Double.MaxValue);
			instanceInt_1.Append(new Char[]{'a', 'b', 'c', 'd'});
			instanceInt_1.Append(new Char[]{'a', 'b', 'c', 'd', 'e'}, 1, 2);
			instanceInt_1.Append(new Object());
			instanceInt_1.Append(Decimal.MaxValue);
			if(!state_Append && debug){
				state_Append = true;
				Console.WriteLine("Completed Append");
			}
			strLoc = "Loc_9347sg!";
			instanceInt_1.AppendFormat("This is {0} the value", Int32.MaxValue);
			instanceInt_1.AppendFormat("This is {0} the value followed by {1}", Int32.MaxValue, Int64.MaxValue);
			temp = instanceInt_1.ToString();
			instanceInt_1.AppendFormat("This is {0} the value {1} {2}", Int32.MaxValue, "Nooo", 'a');
			instanceInt_1.AppendFormat("This is {0} the value {1} {2} {3} {4}", Int32.MaxValue, 'a', 'b', 'c', 'd');
			instanceInt_1.AppendFormat(null, "This is {0} the value {1} {2} {3} {4}", Int32.MaxValue, 'a', 'b', 'c', 'd');
			if(!state_AppendFormat && debug){
				state_AppendFormat = true;
				Console.WriteLine("Completed AppendFormat");
			}
			strLoc = "Loc_2075tsfg!";
			instanceInt_1.Insert(0, Int16.MinValue);
			instanceInt_1.Insert(1, Int32.MaxValue);
			instanceInt_1.Insert(2, Int64.MaxValue);
			instanceInt_1.Insert(3, true);
			instanceInt_1.Insert(4, 'a');
			instanceInt_1.Insert(6, "Hello World");
			instanceInt_1.Insert(7, "Hello World", 2);
			instanceInt_1.Insert(8, Byte.MaxValue);
			instanceInt_1.Insert(9, SByte.MaxValue);
			instanceInt_1.Insert(10, UInt16.MaxValue);
			temp = instanceInt_1.ToString();
			instanceInt_1.Insert(11, UInt32.MaxValue);
			instanceInt_1.Insert(12, UInt64.MaxValue);
			instanceInt_1.Insert(13, Single.MaxValue);
			instanceInt_1.Insert(14, Double.MaxValue);
			instanceInt_1.Insert(15, new Char[]{'a', 'b', 'c', 'd'});
			instanceInt_1.Insert(16, new Char[]{'a', 'b', 'c', 'd', 'e'}, 1, 2);
			instanceInt_1.Insert(17, new Object());
			instanceInt_1.Insert(18, Decimal.MaxValue);
			if(!state_Insert && debug){
				state_Insert = true;
				Console.WriteLine("Completed Insert");
			}
			strLoc = "Loc_29375sdg!";
			instanceInt_1.Replace('a', 'b');
			instanceInt_1.Replace('b', 'c', 10, 100);
			temp = instanceInt_1.ToString();
			instanceInt_1.Replace("Hello World", "Godbyte earth");
			instanceInt_1.Replace("Godbyte earth", "Hello Universe", 10, 100);
			if(!state_Replace && debug){
				state_Replace = true;
				Console.WriteLine("Completed Replace");
			}
			strLoc = "Loc_0347sdg!";
			instanceInt_1.Remove(1, 10);
			temp = instanceInt_1.ToString();
			len = instanceInt_1.Length;
			try{
				temp = instanceInt_1.ToString(0, len);
			}catch{
			}
			try{
				instanceInt_1.Length = len;
			}catch{
			}
			len = instanceInt_1.EnsureCapacity(len);
			fTmp = instanceInt_1.Equals(new Object());
			fTmp = instanceInt_1.Equals(instanceInt_1);
			if(!state_OtherMethods && debug){
				state_OtherMethods = true;
				Console.WriteLine("Completed Other methods");
			}
			if(debug && (++threadCount>(numberOfThreads-1)))
				Console.WriteLine("Last thread completed other methods");
			strLoc = "Loc_0213sdg!";
			instanceInt_1.Capacity = instanceInt_1.Capacity + 1000;
			instanceInt_1[0] = 'a';			
			chTmp = instanceInt_1[0];
			iTmp = instanceInt_1.MaxCapacity;
			instanceInt_1.Length = instanceInt_1.Length + 1000;
		}catch(Exception ex){
			Console.WriteLine("Err_874325s! Location: {0} Excepion thrown, need investigation! {1}", strLoc, ex);
		}						
	}
	private void WorkOnInstance_2()
	{
		String strLoc = "Loc_000oo";
		try{
			strLoc = "Loc_2075tsfg!";
			instanceInt_1.Length=0;
		}catch(Exception ex){
			Console.WriteLine("Err_0324sg! Location: {0} Excepion thrown, need investigation! {1}", strLoc, ex);
		}
	}
	private void WorkOnStatic()
	{
		String strLoc = "Loc_000oo";
		String temp;
		Int32 len;
		Int32 iTmp;
		Char chTmp;
		Boolean fTmp;
		try{
			strLoc = "Loc_8734sg!";
			staticInt_1.Append(Int16.MinValue);
			staticInt_1.Append(Int32.MaxValue);
			staticInt_1.Append(Int64.MaxValue);
			staticInt_1.Append(true);
			staticInt_1.Append('a');
			staticInt_1.Append('a', 1000);
			staticInt_1.Append("Hello World");
			staticInt_1.Append("Hello World", 2, 5);
			staticInt_1.Append(Byte.MaxValue);
			temp = staticInt_1.ToString();
			staticInt_1.Append(SByte.MaxValue);
			staticInt_1.Append(UInt16.MaxValue);
			staticInt_1.Append(UInt32.MaxValue);
			staticInt_1.Append(UInt64.MaxValue);
			staticInt_1.Append(Single.MaxValue);
			staticInt_1.Append(Double.MaxValue);
			staticInt_1.Append(new Char[]{'a', 'b', 'c', 'd'});
			staticInt_1.Append(new Char[]{'a', 'b', 'c', 'd', 'e'}, 1, 2);
			staticInt_1.Append(new Object());
			staticInt_1.Append(Decimal.MaxValue);
			if(!state_Append && debug){
				state_Append = true;
				Console.WriteLine("Completed Append");
			}
			strLoc = "Loc_9347sg!";
			staticInt_1.AppendFormat("This is {0} the value", Int32.MaxValue);
			staticInt_1.AppendFormat("This is {0} the value followed by {1}", Int32.MaxValue, Int64.MaxValue);
			temp = staticInt_1.ToString();
			staticInt_1.AppendFormat("This is {0} the value {1} {2}", Int32.MaxValue, "Nooo", 'a');
			staticInt_1.AppendFormat("This is {0} the value {1} {2} {3} {4}", Int32.MaxValue, 'a', 'b', 'c', 'd');
			staticInt_1.AppendFormat(null, "This is {0} the value {1} {2} {3} {4}", Int32.MaxValue, 'a', 'b', 'c', 'd');
			if(!state_AppendFormat && debug){
				state_AppendFormat = true;
				Console.WriteLine("Completed AppendFormat");
			}
			strLoc = "Loc_2075tsfg!";
			staticInt_1.Insert(0, Int16.MinValue);
			staticInt_1.Insert(1, Int32.MaxValue);
			staticInt_1.Insert(2, Int64.MaxValue);
			staticInt_1.Insert(3, true);
			staticInt_1.Insert(4, 'a');
			staticInt_1.Insert(6, "Hello World");
			staticInt_1.Insert(7, "Hello World", 2);
			staticInt_1.Insert(8, Byte.MaxValue);
			staticInt_1.Insert(9, SByte.MaxValue);
			staticInt_1.Insert(10, UInt16.MaxValue);
			temp = staticInt_1.ToString();
			staticInt_1.Insert(11, UInt32.MaxValue);
			staticInt_1.Insert(12, UInt64.MaxValue);
			staticInt_1.Insert(13, Single.MaxValue);
			staticInt_1.Insert(14, Double.MaxValue);
			staticInt_1.Insert(15, new Char[]{'a', 'b', 'c', 'd'});
			staticInt_1.Insert(16, new Char[]{'a', 'b', 'c', 'd', 'e'}, 1, 2);
			staticInt_1.Insert(17, new Object());
			staticInt_1.Insert(18, Decimal.MaxValue);
			if(!state_Insert && debug){
				state_Insert = true;
				Console.WriteLine("Completed Insert");
			}
			strLoc = "Loc_932745tsfg!";
			staticInt_1.Replace('a', 'b');
			staticInt_1.Replace('b', 'c', 10, 100);
			temp = staticInt_1.ToString();
			staticInt_1.Replace("Hello World", "Godbyte earth");
			staticInt_1.Replace("Godbyte earth", "Hello Universe", 10, 100);
			if(!state_Replace && debug){
				state_Replace = true;
				Console.WriteLine("Completed Replace");
			}
			strLoc = "Loc_0347sdg!";
			staticInt_1.Remove(1, 10);
			temp = staticInt_1.ToString();
			len = staticInt_1.Length;
			try{
				temp = staticInt_1.ToString(0, len);
			}catch{
			}
			try{
				staticInt_1.Length = len;
			}catch{
			}
			len = staticInt_1.EnsureCapacity(len);
			fTmp = staticInt_1.Equals(new Object());
			fTmp = staticInt_1.Equals(staticInt_1);
			if(!state_OtherMethods && debug){
				state_OtherMethods = true;
				Console.WriteLine("Completed Other methods");
			}
			if(debug && (++threadCount>(numberOfThreads-1)))
				Console.WriteLine("Last thread completed other methods");
			strLoc = "Loc_0213sdg!";
			staticInt_1.Capacity = staticInt_1.Capacity + 1000;
			staticInt_1[0] = 'a';			
			chTmp = staticInt_1[0];
			iTmp = staticInt_1.MaxCapacity;
			staticInt_1.Length = staticInt_1.Length + 1000;
		}catch(Exception ex){
			Console.WriteLine("Err_874325s! Location: {0} Excepion thrown, need investigation! {1}", strLoc, ex);
		}						
	}	
	private void WorkOnStatic_2()
	{
		String strLoc = "Loc_000oo";
		try{
			strLoc = "Loc_2075tsfg!";
			staticInt_1.Length=0;
		}catch(Exception ex){
			Console.WriteLine("Err_9275sdg! Location: {0} Excepion thrown, need investigation! {1}", strLoc, ex);
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
	   Co8788StringBuilder cbA = new Co8788StringBuilder();
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
