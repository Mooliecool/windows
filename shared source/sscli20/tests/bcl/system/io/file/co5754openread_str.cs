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
public class Co5754OpenRead_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.OpenRead(String)";
	public static String s_strTFName        = "Co5754OpenRead_str.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		try
		{
			String filName = s_strTFAbbrev+"TestFile";			
			Stream s2;
			if(File.Exists(filName))
				File.Delete(filName);
			strLoc = "Loc_00001";
			iCountTestcases++;
			try {
				File.OpenRead("lllllllllllllllllllll");
				iCountErrors++;
				printerr( "Error_00002! Expected exception not thrown");
			} catch (IOException iexc) {
				printinfo( "Info_00003! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00004! Incorrect exception caught, exc=="+exc.ToString());
			}
			strLoc = "Loc_00005";
			new FileStream(filName, FileMode.Create).Close();
			iCountTestcases++;
			s2 = File.OpenRead(filName);
			iCountTestcases++;
			if(!s2.CanRead) {
				iCountErrors++;
				printerr( "Error_00006! File not opened canread");
			}
			iCountTestcases++;
			if(s2.CanWrite) {
				iCountErrors++;
				printerr( "Error_00007! File was opened canWrite");
			}
			s2.Close();
			strLoc = "Loc_00008";
			iCountTestcases++;
			try {
				s2 = File.OpenRead(null);
				iCountErrors++;
				printerr( "Error_00009! Expected exeption not thrown");
				s2.Close();
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_00010! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00011! Incorrect exception, exc=="+exc.ToString());
			}
			strLoc = "Loc_00012";
			iCountTestcases++;
			try {
				s2 = File.OpenRead(String.Empty);
				iCountErrors++;
				printerr( "Error_10013! Expected exception not thrown");
				s2.Close();
			} catch (ArgumentException aexc) {
				printinfo( "Info_00014! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00015! Incorrect exception, exc=="+exc.ToString());
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
		Co5754OpenRead_str cbA = new Co5754OpenRead_str();
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
