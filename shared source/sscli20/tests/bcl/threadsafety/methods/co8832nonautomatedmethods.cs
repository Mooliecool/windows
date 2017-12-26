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
using System.Runtime.Serialization;
using System.Reflection;
using System.Globalization;
public class Co8832NonAutomatedMethods
{
	public static readonly String s_strActiveBugNums = "";
	public static readonly String s_strDtTmVer       = "";
	public static readonly String s_strClassMethod   = "public static - threadsafety test";
	public static readonly String s_strTFName        = "Co8832NonAutomatedMethods.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	public virtual bool runTest()
	{
		const Int32 numberOfThreads = 200;
		Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ThreadStart tdst1;				
		Thread[] thdPool;
		ThreadStart tdst2;
		Thread[] thdPool2;
		try {
			strLoc = "Loc_001aa";
			iCountTestcases++;
			strLoc = "Loc_002ab";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.PublicStatics);
				thdPool[i] = new Thread(tdst1);
			}							
			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(this.PublicStatics);
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
           Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
           return false;
        }
    }
	private void PublicStatics()
	{
		MemberInfo[] ms = FormatterServices.GetSerializableMembers(typeof(A), new StreamingContext(StreamingContextStates.All));
		foreach(MemberInfo m in ms){
			if(!m.DeclaringType.Equals(typeof(A)))
				throw new Exception("Err_358sfg! error");
		}
		A a = new A();
		Object[] os = FormatterServices.GetObjectData(a, ms);
		if((int)os[0]!=5)
			throw new Exception("Err_293457tsgd! error");
		if((String)os[1]!="Hello World")
			throw new Exception("Err_394ts7g! error");
		Object o = FormatterServices.GetUninitializedObject(typeof(A));
		Object o1 = FormatterServices.PopulateObjectMembers(o, ms, os);
		if(((A)o1).I!=5)
			throw new Exception("Err_23947tsg! 2347sg");
		if(((A)o1).S!="Hello World")
			throw new Exception("Err_w347sg! 2347sg");
		Type tp1 = FormatterServices.GetTypeFromAssembly(typeof(String).Assembly, "System.Int32");
		if(!tp1.Equals(typeof(Int32)))
			throw new Exception("Err_3497tsg! ");
		Int32[] iS = {1,2,3};
		Int32[] iD = new Int32[3];
		Buffer.BlockCopy(iS, 0, iD, 0, 12);
		if(iD[0]!=1)
			throw new Exception("Err_3w497gb! ");
		if(iD[1]!=2)
			throw new Exception("Err_3w497gb! ");
		if(iD[2]!=3)
			throw new Exception("Err_3w497gb! ");
		Buffer.SetByte(iD, 4, 2);
		Double d1 = 0;
		Boolean parse = Double.TryParse("2.1", NumberStyles.Any, null, out d1);
		if(!parse)
			throw new Exception("Err_29457sg! ");
		if(d1!=2.1)
			throw new Exception("Err_23497tgz! ");
		parse = TimeZone.IsDaylightSavingTime(DateTime.Now, new DaylightTime(new DateTime(2001, 1, 1), new DateTime(2001, 12, 1), new TimeSpan(TimeSpan.TicksPerHour)));
	}
	private void WorkOnInstanceAndLocal_2()
	{
		MemberInfo[] ms = FormatterServices.GetSerializableMembers(typeof(int), new StreamingContext(StreamingContextStates.All));
		foreach(MemberInfo m in ms){
			if(!m.DeclaringType.Equals(typeof(Int32)))
				throw new Exception("Err_23947tsg! error");
		}
		int i=5;
		Object[] os = FormatterServices.GetObjectData(i, ms);
		if((int)os[0]!=5)
			throw new Exception("Err_32497g! error");
		Object o = FormatterServices.GetUninitializedObject(typeof(int));
		Object o1 = FormatterServices.PopulateObjectMembers(o, ms, os);
		if((Int32)o1!=5)
			throw new Exception("Err_23947tsg! 2347sg");
		Int32[] iS = {1,2,3};
		if(Buffer.ByteLength(iS)!=12)
			throw new Exception("Err_10974! wrong result");
		if(Buffer.GetByte(iS, 4)!=2)
			throw new Exception("Err_23408! wrong result");
	}
	static void MyHandler(object sender, UnhandledExceptionEventArgs args) {
	   Exception e = (Exception) args.ExceptionObject;
	   Console.WriteLine("MyHandler caught : " + e.Message);
	   Environment.Exit(1);
	}
	public static void Main(String[] args)
	{
		AppDomain currentDomain = AppDomain.CurrentDomain;
		currentDomain.UnhandledException += new UnhandledExceptionEventHandler(MyHandler);
	   bool bResult = false;
	   Co8832NonAutomatedMethods cbA = new Co8832NonAutomatedMethods();
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
[Serializable]
class A
{
	int i;
	String str;
	internal A(){
		i=5;
		str = "Hello World";
	}
	internal Int32 I{
		get{return i;}
	}
	internal String S{
		get{return str;}
	}
}
