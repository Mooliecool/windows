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
public class Co5620ctor_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamReader(String)";
	public static String s_strTFName        = "Co5620ctor_str.cs";
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
			StreamReader sr2;
			StreamWriter sw2;
			String str2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);			
			strLoc = "Loc_t98hb";
			iCountTestcases++;
			try {
				sr2 = new StreamReader((String)null);
				iCountErrors++;
				printerr( "Error_987yg! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_288cj! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_86t7b! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98hgb";
			iCountTestcases++;
			try {
				sr2 = new StreamReader(Environment.CurrentDirectory);
				iCountErrors++;
				printerr("Error_t7987! Expected exception not thrown");
			} catch (UnauthorizedAccessException aexc) {
				printinfo( "Info_98b8b! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_958yv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sr2 = new StreamReader(".");
				iCountErrors++;
				printerr( "Error_298yb! Expected exception not thrown");
			} catch(UnauthorizedAccessException aexc) {
				printinfo( "Info_0299x! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_10b8h! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sr2 = new StreamReader("..");
				iCountErrors++;
				printerr( "Error_1238c! Expected exception not thrown");
			} catch (UnauthorizedAccessException aexc) {
				printinfo("Info_0199bj! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_938gh! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_27ygb";
			iCountTestcases++;
			try {
				sr2 = new StreamReader("HHHHHHHHHHHHHHHHHHHHHHHHH");
				iCountErrors++;
				printerr( "Error_981t8! Expected exception not thrown");
			} catch (FileNotFoundException fexc) {
				printinfo("Info_7t8yb! Caught expected exception, exc=="+fexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_t876y! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98y29";
			sw2 = new StreamWriter(filName);
			sw2.Write("Hello\u00FFWorld");
			sw2.Close();
			sr2 = new StreamReader(filName);
			iCountTestcases++;
			str2 = sr2.ReadToEnd();
			if(!str2.Equals("Hello\u00FFWorld")) {
				iCountTestcases++;
				printerr( "Error_896yb! Incorrect string read=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_948yv";
			sw2 = new StreamWriter("\u00FF\u00FF");
			sw2.Write("Hello\u00FFWorld");
			sw2.Close();
			sr2 = new StreamReader("\u00FF\u00FF");
			iCountTestcases++;
			str2 = sr2.ReadToEnd();
			if(!str2.Equals("Hello\u00FFWorld")) {
				iCountErrors++;
				printerr( "Error_01u9b! Incorrect string read=="+str2);
			}
			sr2.Close();
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
		Co5620ctor_str cbA = new Co5620ctor_str();
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
