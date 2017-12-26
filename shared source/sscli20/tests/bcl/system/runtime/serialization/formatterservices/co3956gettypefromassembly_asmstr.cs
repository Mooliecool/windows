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
using System.Text;
public class Co3956GetTypeFromAssembly_AsmStr
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "FormatterServices.GetTypeFromString(Assembly, String)";
	public static String s_strTFName        = "Co3956GetTypeFromAssembly_AsmStr.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		Console.WriteLine();
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		Assembly asm1;
		Type tpValue;
		Type[] arrTpClassLib = null;
		try {
			do
			{
				strLoc = "Loc_435cd";
				tpValue = typeof(String);
				asm1 = tpValue.Module.Assembly;
				iCountTestcases++;
				if(!tpValue.Equals(FormatterServices.GetTypeFromAssembly(asm1, "System.String"))){
					iCountErrors++;
					Console.WriteLine("Err_743cd! Unexpected value reutned");
				}
				strLoc = "Loc_02547cs";
				arrTpClassLib = typeof(String).Module.GetTypes();
				for(int i=0; i<arrTpClassLib.Length;i++){
					if(!arrTpClassLib[i].IsPublic || !arrTpClassLib[i].IsSerializable)
					continue;
					try {
						tpValue = FormatterServices.GetTypeFromAssembly(asm1, arrTpClassLib[i].FullName);
						iCountTestcases++;
						if(!tpValue.Equals(arrTpClassLib[i])){
							iCountErrors++;
							Console.WriteLine("Err_743cd! Unexpected value reutned");
						}
						}catch(Exception ex){
						iCountErrors++;
						Console.WriteLine("Err_846qm! Unexpected exception thrown for the type, " + arrTpClassLib[i].FullName + ", " + ex.ToString());
					}
				}
				iCountTestcases++;
				if(FormatterServices.GetTypeFromAssembly(asm1, "No such string")!=null){
					iCountErrors++;
					Console.WriteLine("Err_743cd! Unexpected value reutned");
				}
				tpValue = this.GetType();
				asm1 = tpValue.Module.Assembly;
				iCountTestcases++;
				if(!tpValue.Equals(FormatterServices.GetTypeFromAssembly(asm1, "Co3956GetTypeFromAssembly_AsmStr"))){
					iCountErrors++;
					Console.WriteLine("Err_743cd! Unexpected value reutned");
				}
				iCountTestcases++;
				if(FormatterServices.GetTypeFromAssembly(asm1, "No such string")!=null){
					iCountErrors++;
					Console.WriteLine("Err_743cd! Unexpected value reutned");
				}
				try
				{
					iCountTestcases++;
					FormatterServices.GetTypeFromAssembly(null, "Any String");
					iCountErrors++;
					Console.WriteLine("Err_935vd! We were expecting this to throw!");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_935fd! Unexpected exception thrown, " + ex.ToString());
				}
				try
				{
					iCountTestcases++;
					FormatterServices.GetTypeFromAssembly(asm1, null);
					iCountErrors++;
					Console.WriteLine("Err_106vs! We were expecting this to throw!");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_720cxa! Unexpected exception thrown, " + ex.ToString());
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general);
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
		Co3956GetTypeFromAssembly_AsmStr cbA = new Co3956GetTypeFromAssembly_AsmStr();
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
	internal String str_Humpty;
	public int i_spy;
	public baloney()
	{
		str_Humpty = "Hello World";
		i_spy = 20;
	}
}
