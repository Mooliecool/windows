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
using System.Collections;
using System.IO;
using System.Threading;
public class Co8827Console
{
	public static readonly String s_strActiveBugNums = "";
	public static readonly String s_strDtTmVer       = "";
	public static readonly String s_strClassMethod   = "Console - threadsafety test";
	public static readonly String s_strTFName        = "Co8827Console.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	private static Stream stream1;
	private static TextWriter text1;
	private static TextReader text2;
	private static StreamReader reader;
	private static StreamWriter writer1;
	private static StreamWriter writer2;
        private static Random randomNumGen;
	public virtual bool runTest()
	{
                StreamWriter Output = File.CreateText("Output.txt");
		const int numberOfThreads = 200;
                randomNumGen = new Random();
		Output.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ThreadStart tdst1;				
		Thread[] thdPool;
		ThreadStart tdst2;
		Thread[] thdPool2;
		TextWriter tw1;
		TextWriter tw2;
		TextReader tr1;
		ArrayList list;
		Int32 iReader;		
		try {
			strLoc = "Loc_001aa";
			iCountTestcases++;
			if(!File.Exists("Temp.txt")){
				StreamWriter swtemp = File.CreateText("Temp.txt");
				swtemp.Close();
			}
			strLoc = "Loc_002ab";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.WorkOnStdOpen);
				thdPool[i] = new Thread(tdst1);
			}
			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(this.WorkOnStdOpen_Int);
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
			strLoc = "Loc_002ab";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.GetStream);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
			strLoc = "Loc_32497sg_2486tsg";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.WriteConsole_1);
				thdPool[i] = new Thread(tdst1);
			}
			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(this.WriteConsole_2);
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
			tw1 = Console.Out;
			tw2 = Console.Error;
			tr1 = Console.In;
			reader = File.OpenText("Temp.txt");
			FileStream fs1 = new FileStream("temp1.txt", FileMode.Create, FileAccess.Write);
			writer1 = new StreamWriter(fs1);
			fs1 = new FileStream("temp2.txt", FileMode.Create, FileAccess.Write);
			writer2 = new StreamWriter(fs1);
			strLoc = "Loc_349gs";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.SetStream);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
			strLoc = "376sg";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.WriteConsole_1);
				thdPool[i] = new Thread(tdst1);
			}
			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(this.WriteConsole_2);
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
			reader.Close();
			writer1.Close();
			writer2.Close();
			list = new ArrayList();
			reader = File.OpenText("temp1.txt");
			Console.SetIn(reader);
			while((iReader=Console.Read())!=-1){							
				list.Add((Char)iReader);
			}
			reader.Close();
			ConfirmWrite(list, numberOfThreads);
			strLoc = "376sg";
			iCountTestcases++;
			fs1 = new FileStream("temp2.txt", FileMode.Create, FileAccess.Write);
			writer1 = new StreamWriter(fs1);
			Console.SetOut(writer1);
			for(int i=0; i<numberOfThreads; i++){
				Console.Write(5);
			}
			writer1.Close();
			Console.SetOut(tw1);
			reader = File.OpenText("temp2.txt");
			Console.SetIn(reader);
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.ReadConsole);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
            reader.Close();			
			strLoc = "9247sg";
			iCountTestcases++;
			fs1 = new FileStream("temp3.txt", FileMode.Create, FileAccess.Write);
			writer1 = new StreamWriter(fs1);
			Console.SetOut(writer1);
			for(int i=0; i<numberOfThreads; i++){
				Console.WriteLine("Hello World");
			}
			writer1.Close();
			Console.SetOut(tw1);
			reader = File.OpenText("temp3.txt");
			Console.SetIn(reader);
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.ReadLineConsole);
				thdPool[i] = new Thread(tdst1);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
            reader.Close();			
			Console.SetIn(tr1);
			Console.SetOut(tw1);
			Console.SetError(tw2);
		} catch (Exception exc_general ) {
		   ++iCountErrors;
		   Output.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
		}
        if ( iCountErrors == 0 ){
           Output.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
           Output.Close();
           return true;
        }
        else{
           Output.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
           Output.Close();
           return false;
        }
    }
	private void WorkOnStdOpen()
	{
		stream1 = Console.OpenStandardInput(); 
		stream1 = Console.OpenStandardOutput();
		stream1 = Console.OpenStandardError();
	}
	private void WorkOnStdOpen_Int()
	{
		stream1 = Console.OpenStandardInput(256); 
		stream1 = Console.OpenStandardOutput(256);
		stream1 = Console.OpenStandardError(256);
	}	
	private void GetStream()
	{
		text1 = Console.Out;
		text1 = Console.Error;
		text2 = Console.In;
	}	
	private void SetStream()
	{
		Console.SetIn(reader);
		Console.SetOut(writer1);
		Console.SetError(writer2);
	}	
	private void WriteConsole_1()
	{
	        // Test object output 
		Console.Write("{0}", "HHHHHHHH");
		// Test boolean output
                Console.WriteLine(true);
                // Test char
                Console.Write('z');
                // Make sure that the threads are out of sync
                Thread.Sleep(randomNumGen.Next(0, 10)); 
                // Test char[] output
		Char[] chArrValue = new Char[]{'a', 'a', 'a', 'a',};
		Console.WriteLine(chArrValue);
                // Test string output
		Console.WriteLine("Shoo flies dont bother me cause I belong to somebody I feel like a morning start");
	}
	private void WriteConsole_2()
	{
		// Test float32 
		Console.Write(2.222);
                // Test float64
		Console.Write(333333.3333M);
                // Make sure that the threads are out of sync
                Thread.Sleep(randomNumGen.Next(0, 10)); 
		// Test int32
		Console.Write(5555);
                // Test uint32
		Console.Write((uint)9999);
	}	
	private void ReadConsole()
	{
		if((Char)Console.Read()!='5'){
			throw new Exception("Err_2487tsg! Wrong value");
		}
	}
	private void ReadLineConsole()
	{
		if(Console.ReadLine()!="Hello World"){
			throw new Exception("Err_37sfg! Wrong value");
		}
	}
	private void ConfirmWrite(ArrayList list, int threads){
		Char[] cs = new Char[list.Count];
		list.CopyTo(cs, 0);
		String value = new String(cs);
		String seek;
                StreamWriter temp = File.CreateText("Errors.txt");
		for(int i=0; i<threads; i++){
		seek = "HHHHHHHH";
			if(value.IndexOf(seek)<0)
		          { temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception());}
			value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
			seek = "Shoo flies dont bother me cause I belong to somebody I feel like a morning start";
			if(value.IndexOf(seek)<0)
			  { temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception());  }
			value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
			seek = "True";
			if(value.IndexOf(seek)<0)
			  { temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception());}
			value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
			seek = "aaaa";
			if(value.IndexOf(seek)<0)
			   {temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception()); }
			value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
			seek = "z";
			if(value.IndexOf(seek)<0)
			   {temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception()); }
			value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
			seek = "2.222";
			if(value.IndexOf(seek)<0)
			   {temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception()); }
			value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
                        seek = "333333.3333";
			if(value.IndexOf(seek)<0)
			   {temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception()); }
                        value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
			seek = "9999";
			if(value.IndexOf(seek)<0)
			   {temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception()); }
			value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
			seek = "5555";
			if(value.IndexOf(seek)<0)
			   {temp.WriteLine("Seek: "+seek+" Val: " + value); temp.Close(); throw(new System.Exception()); }
                        value = value.Substring(0, value.IndexOf(seek)) + value.Substring(value.IndexOf(seek) + seek.Length);
                      
                        temp.WriteLine("Confirmed values for thread: " + i + " out of " + threads);
                        temp.Flush();	
		}
		temp.WriteLine("Value: " + value );
		temp.Close();
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
	   Co8827Console cbA = new Co8827Console();
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
