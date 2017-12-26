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
public class Co5736SetCurrentDirectory_dir
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.SetCurrentDirectory(Directory)";
	public static String s_strTFName        = "Co5736set_SetCurrentDirectory_dir.cs";
	public static String s_strTFAbbrev      = "Co5736";
	public static String s_strTFPath        = Directory.GetCurrentDirectory();
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
			DirectoryInfo currentdir = new DirectoryInfo(Directory.GetCurrentDirectory());
			strLoc = "Loc_23849";
			iCountTestcases++;
			try {
				Directory.SetCurrentDirectory(null) ;
				iCountErrors++;
				printerr( "Error_388rf! Expected exception not thrown, dir=="+ Directory.GetCurrentDirectory());
			} catch (ArgumentNullException aexc ) {
				printinfo ( "Info_2v9cc! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr("Error_28t7b! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_2g77b";
			Directory.SetCurrentDirectory("..");
			dir = new DirectoryInfo(".");
			iCountTestcases++;
			if(!Directory.GetCurrentDirectory().Equals(dir.FullName)) {
				iCountErrors++;
				printerr( "Error_38g8b! Directory Not set correctly");
			}
			strLoc = "Loc_9t8gt";
#if PLATFORM_UNIX
                        dir = new DirectoryInfo("/");
                        Directory.SetCurrentDirectory("/");
#else
			dir = new DirectoryInfo("c:\\");
			Directory.SetCurrentDirectory("c:\\");
#endif
			iCountTestcases++;
			if(!Directory.GetCurrentDirectory().Equals(dir.FullName)) {
				iCountErrors++;
				printerr( "Error_238g7! Directory not set correctly, dir=="+Directory.GetCurrentDirectory());
			}
			strLoc = "Loc_87ygv";
			Directory.SetCurrentDirectory(currentdir.FullName);
			iCountTestcases++;
			if(!Directory.GetCurrentDirectory().Equals(currentdir.FullName)) {
				iCountErrors++;
				printerr( "Error_2g7b7! Directory not set correctly, dir=="+Directory.GetCurrentDirectory());
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
		Co5736SetCurrentDirectory_dir cbA = new Co5736SetCurrentDirectory_dir();
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
