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
using System.Reflection;
using System.Runtime.Serialization;
using System.Security;
using System.Security.Permissions; 
[assembly:SecurityPermission(SecurityAction.RequestRefuse, SerializationFormatter=true)]
public class Co8649FormatterServices
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Security - FormatterServices";
	public static String s_strTFName        = "Co8649FormatterServices.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	MemberInfo[] members;
	Test t;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		StreamingContext sc1 = new StreamingContext(StreamingContextStates.All);
		try
		{
			strLoc = "Loc_9347sg";
			iCountTestcases++;			
			try{
				Call_FS_1();
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_290437sg";
			iCountTestcases++;			
			try{
				Call_FS_2();
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_290437sg";
			iCountTestcases++;			
			members = typeof(Test).GetFields(BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance);
			t = new Test();
			try{
				Call_FS_3();
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_290437sg";
			iCountTestcases++;			
			try{
				Call_FS_4();
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_290437sg";
			iCountTestcases++;			
			try{
				Call_FS_5();
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_290437sg";
			iCountTestcases++;			
			try{
				Call_FS_6();
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
		} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
		}
		if ( iCountErrors == 0 )
		{
			Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
			return true;
		}
		else
		{
			Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
			return false;
		}
	}
	void Call_FS_1()
	{
		MemberInfo[] members = FormatterServices.GetSerializableMembers(typeof(Test));
	}
	void Call_FS_2()
	{
		MemberInfo[] members = FormatterServices.GetSerializableMembers(typeof(Test), new StreamingContext(StreamingContextStates.All));
	}
	void Call_FS_3()
	{
		Object[] os = FormatterServices.GetObjectData(t, members);
	}
	void Call_FS_4()
	{
		Type tp = FormatterServices.GetTypeFromAssembly(typeof(string).Assembly, "System.Int32");
	}
	void Call_FS_5()
	{
		Object o = FormatterServices.GetUninitializedObject(typeof(Test));
	}
	void Call_FS_6()
	{
		Object o = FormatterServices.PopulateObjectMembers(t, members, new Object[]{1,"hello"});
	}	
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co8649FormatterServices cbA = new Co8649FormatterServices();
		try {
			bResult = cbA.runTest();
		} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
		}
		if (!bResult)
		{
			Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
			Console.WriteLine( " " );
			Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
			Console.WriteLine( " " );
      }
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
[Serializable]
public class Test
{
	private int i;
	private string s;
	internal Test(){
		i=5;
		s="yep";
	}
	public int I{
		get {return i;}
	}
	public string S{
		get {return s;}
	}
}
