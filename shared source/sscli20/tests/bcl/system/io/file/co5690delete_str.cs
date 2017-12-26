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
public class Co5690Delete_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.CreateText";
	public static String s_strTFName        = "Co5690Delete_str.cs";
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
			FileInfo fil2;
			StreamWriter sw2;
			if(File.Exists(filName))
				File.Delete(filName);
			strLoc = "Loc_2yc81";
			iCountTestcases++;
			try {
				File.Delete(null);
				iCountErrors++;
				printerr( "Error_1u9by! Expected exception not thrown");
			} catch ( ArgumentNullException aexc) {
				printinfo ( "Info_g98yb! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_19d4b! Incorrect exception thrown, exc=="+exc.ToString());
			}	 
			strLoc = "Loc_32453";
			iCountTestcases++;
			try {
				File.Delete("*.*");
				iCountErrors++;
				printerr( "Error_4342! Expected exception not thrown");
			} catch ( ArgumentException aexc) {
				printinfo ( "Info_43432!!!! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_7777! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_90187";
			iCountTestcases++;
			try {
				File.Delete(".");
				iCountErrors++;
				printerr( "Error_19yb7! Expected exception not thrown");
			} catch (UnauthorizedAccessException aexc) {
				printinfo( "Info_28gy7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_19gyb! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_2g79b";
			new FileStream(filName, FileMode.Create).Close();
			File.Delete(filName);
			iCountTestcases++;
			if(File.Exists(filName)) {
				iCountErrors++;
				printerr( "Error_y7gbs! File not deleted");
			}
			new FileStream(filName, FileMode.Create).Close();
			File.Delete(Environment.CurrentDirectory+"\\"+filName);
			iCountTestcases++;
			if(File.Exists(filName)) {
				iCountErrors++;
				printerr( "Error_94t7b! File not deleted");
			}
			strLoc = "Loc_278yb";
			iCountTestcases++;
			try {
				File.Delete("FileDoesNotExist");
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_218b9! Unexpected exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_298c8";
			sw2 = new StreamWriter(filName);
			sw2.Write("HelloWorld");
			sw2.Close();
			File.Delete(filName);
			iCountTestcases++;
			if(File.Exists(filName)) {
				iCountErrors++;
				printerr( "Error_4017v! File not deleted");
			}
#if !PLATFORM_UNIX // Unix systems allow readonly files to be deleted
			strLoc = "Loc_298b7";
			fil2 = new FileInfo(filName);
			new FileStream(filName, FileMode.Create).Close();
			fil2.Attributes = FileAttributes.ReadOnly;
			iCountTestcases++;
			try {
				File.Delete(filName);
				iCountErrors++;
				printerr( "Error_487bg! Expected exception not thrown");
			} catch (UnauthorizedAccessException aexc) {
				printinfo ( "Info_24y7b! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2467y! Incorrect exception thrown, exc=="+exc.ToString());
			}
			fil2.Attributes = new FileAttributes();
			fil2.Delete();
#endif
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
		Co5690Delete_str cbA = new Co5690Delete_str();
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
