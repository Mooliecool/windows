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
using System.Runtime.Serialization;
using System.IO;
public class Co3884ctor_tp_IF
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SerializationInfo.Ctor(Type, IFormatterConverter)";
	public static String s_strTFName        = "Co3884ctor_tp_IF.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		SerializationInfo serinfo1 = null;
		try {
			do
			{
				serinfo1 = new SerializationInfo(typeof(Int32), new FormatterConverter());
				iCountTestcases++;
				if(serinfo1.MemberCount != 0)
				{
					iCountErrors++;
					Console.WriteLine("Err_0256csd! Wrong number of members, " + serinfo1.MemberCount.ToString());
				}
				serinfo1.AddValue("MyOwnField", 5, typeof(Int32));
				iCountTestcases++;
				if(serinfo1.MemberCount != 1)
				{
					iCountErrors++;
					Console.WriteLine("Err_8564cfdg! Wrong number of members, " + serinfo1.MemberCount.ToString());
				}
				iCountTestcases++;
				if(serinfo1.GetInt16("MyOwnField") != 5)
				{
					iCountErrors++;
					Console.WriteLine("Err_675349fd! Wrong value returned, " + serinfo1.GetInt16("MyOwnField"));
				}
				serinfo1 = new SerializationInfo(typeof(MyOwnClass), new FormatterConverter());
				serinfo1.AddValue("MyOwnField_1", "5", typeof(Int32));
				serinfo1.AddValue("MyOwnField_2", "AnyValue", typeof(String));
				serinfo1.AddValue("MyOwnField_WhichIsNotInThisClass", "AnyValue", typeof(String));
				iCountTestcases++;
				if(serinfo1.MemberCount != 3)
				{
					iCountErrors++;
					Console.WriteLine("Err_6538fcd! Wrong number of members, " + serinfo1.MemberCount.ToString());
				}
				try {
					iCountTestcases++;
					serinfo1 = new SerializationInfo(null, new FormatterConverter());
					iCountErrors++;
					Console.WriteLine("Err_653fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_6439cd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					serinfo1 = new SerializationInfo(typeof(Int32), null);
					iCountErrors++;
					Console.WriteLine("Err_1065753cd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_5739cd! Wrong exception thrown, " + ex);
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.StackTrace);
		}
		if ( iCountErrors == 0 )
		{
			Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
			return true;
		}
		else
		{
			Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
			return false;
		}
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co3884ctor_tp_IF cbA = new Co3884ctor_tp_IF();
		try {
			bResult = cbA.runTest();
			} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
		}
		if (!bResult)
		{
			Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
			Console.WriteLine( " " );
			Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
			Console.WriteLine( " " );
		}
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
class MyOwnClass
{
	Int32 MyOwnField_1;
	String MyOwnField_2;
	MyOwnClass()
	{
		MyOwnField_1 = 5;
		MyOwnField_2 = "Dunno";
	}
}
