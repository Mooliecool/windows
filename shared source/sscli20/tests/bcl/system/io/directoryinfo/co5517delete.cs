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
public class Co5517Delete
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Delete()";
	public static String s_strTFName        = "Co5517Delete";
	public static String s_strTFAbbrev      = "Co5514";
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
			DirectoryInfo dir = null;
			strLoc = "Loc_238yc";
			dir = Directory.CreateDirectory("TestDir");
			dir.Delete();
			iCountTestcases++;
			if(dir.Exists) {
				iCountErrors++;
				printerr( "Error_298sh! Directory should have been Deleted");
			}
			strLoc = "Loc_398ch";
			Directory.CreateDirectory("TestDir\\TestDir");
			dir = new DirectoryInfo("TestDir");
			iCountTestcases++;
			try {
				dir.Delete();
				iCountErrors++;
				printerr( "Error_209uw! Should not been able to Delete an unempty directory");
			} catch (IOException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2908g! Incorrect exception thrown, exc=="+exc.ToString());
			}
			new DirectoryInfo("TestDir\\TestDir").Delete();
			new DirectoryInfo("TestDir").Delete();
			iCountTestcases++;
			dir = new DirectoryInfo("TestDir");
			if(dir.Exists) {
				iCountErrors++;
				printerr( "Error_298hx! Directory should have been Deleted, dir=="+dir.FullName);
			} 
			strLoc = "Loc_98hf8";
			dir = new DirectoryInfo("C:\\");
			iCountTestcases++;
			try {
				dir.Delete();
				iCountErrors++;
				printerr( "Error_28ytg7! Should not been able to Delete root directory");
			} catch ( IOException exc ) {
                printinfo( "Error_4423!!! Expected exception occured... " + exc.Message );
			} catch ( Exception exc ) {
				iCountErrors++;
				printerr( "Error_109xu! Incorrect exception thrown, exc=="+exc.ToString());
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
		Co5517Delete cbA = new Co5517Delete();
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
