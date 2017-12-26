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
public class co8634settype_type
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SerializationInfo.SetType(Type)";
	public static String s_strTFName        = "co8634settype_type.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		SerializationInfo serinfo1 = null;
		Type type = null;
		String strValue;
		try {
			type = typeof(Int32);
			serinfo1 = new SerializationInfo(type, new FormatterConverter());
			iCountTestcases++;
			if(!type.FullName.Equals(serinfo1.FullTypeName)){
				iCountErrors++;
				Console.WriteLine("Err_0256csd! Wrong type long name, " + serinfo1.FullTypeName + " " + type.FullName);
			}
			type = typeof(String);
			serinfo1.SetType(type);
			if(!type.FullName.Equals(serinfo1.FullTypeName)){
				iCountErrors++;
				Console.WriteLine("Err_39745sg! Wrong type long name, " + serinfo1.FullTypeName + " " + type.FullName);
			}
			type = typeof(System.Runtime.Serialization.SerializationInfo);
			serinfo1.SetType(type);
			iCountTestcases++;
			if(!type.FullName.Equals(serinfo1.FullTypeName)){
				iCountErrors++;
				Console.WriteLine("Err_02346fsd! Wrong type name, " + serinfo1.FullTypeName + " " + type.FullName);
			}
			type = typeof(UserDefined);
			serinfo1.SetType(type);
			iCountTestcases++;
			if(!serinfo1.FullTypeName.Equals("UserDefined")){
				iCountErrors++;
				Console.WriteLine("Err_02346fsd! Wrong type name, " + serinfo1.FullTypeName + " " + type.FullName);
			}
			if(serinfo1.AssemblyName.IndexOf("co8634settype_type")<0){
				iCountErrors++;
				Console.WriteLine("Err_02346fsd! Wrong type name, " + serinfo1.FullTypeName + " " + serinfo1.AssemblyName);
			}
			iCountTestcases++;
			try{
				serinfo1.SetType(null);
				iCountErrors++;
				Console.WriteLine("Err_39475sdg! Exception not thrown");
			}catch(ArgumentException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_39475sdg! Wrong Exception thrown" + ex.GetType().Name);
			}
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
		co8634settype_type cbA = new co8634settype_type();
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
[Serializable]
internal class UserDefined{
}
