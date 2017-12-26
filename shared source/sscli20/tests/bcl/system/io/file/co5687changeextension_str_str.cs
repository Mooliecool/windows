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
using System.Globalization;
using System.Text;
using System.Threading;
public class Co5687ChangeExtension_str_Str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Path.AppendText(String)";
	public static String s_strTFName        = "Co5687ChangeExtension_str_Str.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		try
		{
			String filName = s_strTFAbbrev+"TestFile";
			String str2;
			if(File.Exists(filName))
				File.Delete(filName);
			iCountTestcases++;
			str2 = Path.ChangeExtension(String.Empty,".tmp");
			if(!str2.Equals(String.Empty)) {
				iCountErrors++;
				printerr( "Error_18v88! Expected exception not thrown, str2=="+str2);
			}
			strLoc = "Loc_2y9x8";
			str2 = Path.ChangeExtension("Path.tmp", "...");
			iCountTestcases++;
			if(!str2.Equals("Path...")) {
				iCountErrors++;
				printerr( "Error_98ygf7! Incorrect string returned, str2=="+str2);
			}
			str2 = Path.ChangeExtension("Path.temporary.tmp", ".doc");
			iCountTestcases++;
			if(!str2.Equals("Path.temporary.doc")) {
				iCountErrors++;
				printerr( "Error_187yg! Incorrect string returned, str2=="+str2);
			}
			str2 = Path.ChangeExtension("File.tmp", null);
			iCountTestcases++;
			if(!str2.Equals("File")) {
				iCountErrors++;
				printerr( "Error_198cf! Incorrect string returned, str2=="+str2);
			}
			str2 = Path.ChangeExtension("File.tmp", String.Empty);
			iCountTestcases++;
			if(!str2.Equals("File.")) {
				iCountErrors++;
				printerr( "Error_290wd! Incorrect string returned, str2=="+str2);
			}
			str2 = Path.ChangeExtension("File", ".tmp");
			iCountTestcases++;
			if(!str2.Equals("File.tmp")) {
				iCountErrors++;
				printerr( "Error_19yg7! Incorrect string returned, str2=="+str2);
			}
			if(File.Exists(filName))
				File.Delete(filName);
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
			Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
			return false;
		}
	}
	public void printerr ( String err )
	{
		Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
	}
	public void printinfo ( String info )
	{
		Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co5687ChangeExtension_str_Str cbA = new Co5687ChangeExtension_str_Str();
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
