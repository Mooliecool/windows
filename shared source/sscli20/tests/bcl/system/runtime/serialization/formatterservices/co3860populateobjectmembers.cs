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
using System.Reflection;
public class Co3860PopulateObjectMembers
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "FormatterServices.PopulateObjectMembers(Object, MemberInfo[], Object[])";
	public static String s_strTFName        = "Co3860PopulateObjectMembers.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		Console.WriteLine();
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		Int32 iValue = 0;
		Object objValue = null;
		MemberInfo[] members = null;
		Object[] data = null;
		try {
			do
			{
				members = FormatterServices.GetSerializableMembers(iValue.GetType());
				data = new Object[members.Length];
				data[0] = 10;
				objValue = FormatterServices.GetUninitializedObject(iValue.GetType());
				iValue = (Int32)FormatterServices.PopulateObjectMembers(objValue, members, data);
				iCountTestcases++;
				if(iValue != 10)
				{
					iCountErrors++;
					Console.WriteLine("Err_753dw! Wrong Value returned," + iValue.ToString());
				}
				data[0] = "Hello World";
				try
				{
					iCountTestcases++;
					iValue = (Int32)FormatterServices.PopulateObjectMembers(objValue, members, data);
					iCountErrors++;
					Console.WriteLine("Err_045fcd! Excepion not thrown");
				}
				catch(ArgumentException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_369sq! Wrong excepion thrown, " + ex.ToString());
				}
				baloney bal = new baloney();
				members = FormatterServices.GetSerializableMembers(bal.GetType());
				data = new Object[members.Length];
				for(int i=0; i<members.Length; i++) {
					if(members[i].Name.Equals("str_Humpty")) {
						data[i] = "Anything";
					}
					else if(members[i].Name.Equals("i_spy")) {
						data[i] = 3;
					}
					else {
						Console.WriteLine("i scream");
					}
				}
				objValue = FormatterServices.GetUninitializedObject(bal.GetType());
				baloney bal1 = (baloney)FormatterServices.PopulateObjectMembers(objValue, members, data);
				iCountTestcases++;
				if(!bal1.str_Humpty.Equals("Anything"))
				{
					iCountErrors++;
					Console.WriteLine("Err_9564fd! Wrong Value returned," + bal1.str_Humpty);
				}
				iCountTestcases++;
				if(bal1.i_spy!=3)
				{
					iCountErrors++;
					Console.WriteLine("Err_863cd! Wrong Value returned," + bal1.i_spy.ToString());
				}
				try
				{
					iCountTestcases++;
					FormatterServices.PopulateObjectMembers(null, members, data);
					iCountErrors++;
					Console.WriteLine("Err_058fd! Excepion not thrown");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_105ds! Wrong excepion thrown, " + ex.ToString());
				}
				try
				{
					iCountTestcases++;
					FormatterServices.PopulateObjectMembers(objValue, null, data);
					iCountErrors++;
					Console.WriteLine("Err_9662dsd! Excepion not thrown");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_937sd! Wrong excepion thrown, " + ex.ToString());
				}
				try
				{
					iCountTestcases++;
					FormatterServices.PopulateObjectMembers(objValue, members, null);
					iCountErrors++;
					Console.WriteLine("Err_1057ds! Excepion not thrown");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_0572ds! Wrong excepion thrown, " + ex.ToString());
				}
				members = FormatterServices.GetSerializableMembers(iValue.GetType());
				data = new Object[3];
				data[0] = 10;
				objValue = FormatterServices.GetUninitializedObject(iValue.GetType());
				try
				{
					iCountTestcases++;
					iValue = (Int32)FormatterServices.PopulateObjectMembers(objValue, members, data);
					iCountErrors++;
					Console.WriteLine("Err_054xs! Excepion not thrown");
				}
				catch(ArgumentException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_1067fs! Wrong excepion thrown, " + ex.ToString());
				}
				members = FormatterServices.GetSerializableMembers(iValue.GetType());
				members[0] = null;
				data = new Object[1];
				data[0] = 10;
				objValue = FormatterServices.GetUninitializedObject(iValue.GetType());
				try
				{
					iCountTestcases++;
					FormatterServices.PopulateObjectMembers(objValue, members, data);
					iCountErrors++;
					Console.WriteLine("Err_935cd! Excepion not thrown");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_105xs! Wrong excepion thrown, " + ex.ToString());
				}
				members = typeof(Guid).GetConstructors();
				data = new Object[members.Length];
				for(int i=0; i< data.Length; i++){
					data[i] = i + 10;
				}
				objValue = FormatterServices.GetUninitializedObject(typeof(Guid));
				try
				{
					iCountTestcases++;
					FormatterServices.PopulateObjectMembers(objValue, members, data);
					iCountErrors++;
					Console.WriteLine("Err_046fcsd! Excepion not thrown");
				}
				catch(SerializationException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_9453cdf! Wrong excepion thrown, " + ex.ToString());
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
		Co3860PopulateObjectMembers cbA = new Co3860PopulateObjectMembers();
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
		if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
	}
}
[Serializable] public class baloney
{
	public String str_Humpty;
	public int i_spy;
	public baloney()
	{
		str_Humpty = "Hello World";
		i_spy = 20;
	}
}
