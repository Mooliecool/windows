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
public class Co5660get_Name
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Exists(String)";
	public static String s_strTFName        = "Co5660get_Name.cs";
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
			DirectoryInfo dir2=null;
            String curDirectory = Environment.CurrentDirectory.Replace("/", "\\");
            String dirName = null;
			strLoc = "Loc_276t8";
			iCountTestcases++;
			dir2 = new DirectoryInfo(".");
            dirName = dir2.Name.Replace("/", "\\");
			if(!dirName.Equals(curDirectory.Substring(curDirectory.LastIndexOf("\\")+1))) {
				iCountErrors++;
				printerr( "Error_69v8j! Incorrect Name on directory, dir2.Name=="+dir2.Name);
			}
			iCountTestcases++;
			dir2 = new DirectoryInfo(Environment.CurrentDirectory);
            dirName = dir2.Name.Replace("/", "\\");
			if(!dirName.Equals(curDirectory.Substring(curDirectory.LastIndexOf("\\")+1))) {
				iCountErrors++;
				printerr( "Error_97t67! Incorrect Name on directory, dir2.Name=="+dir2.Name);
			}
#if !PLATFORM_UNIX
			strLoc = "Loc_99084";
			dir2 = new DirectoryInfo("\\\\fxqasql\\TestDrivers\\FxBcl\\IO");
			if(!dir2.Name.Equals("IO")) {
				iCountErrors++;
				printerr( "Error_02099! Incorrect name=="+dir2.Name);
			}
			iCountTestcases++;
			dir2 = new DirectoryInfo("C:\\");
			Console.WriteLine(dir2.Name);
			if(!dir2.Name.Equals("C:\\")) {
				iCountErrors++;
				printerr( "Error_50210! Incorrect name=="+dir2.Name);
			}
			strLoc = "Loc_t97g8";
			dir2 = new DirectoryInfo(Environment.CurrentDirectory.ToUpper());		   
			iCountTestcases++;
			Console.WriteLine(dir2.Name);
			if(!dir2.Name.Equals(Environment.CurrentDirectory.Substring(Environment.CurrentDirectory.LastIndexOf("\\")+1).ToUpper())) {
				iCountErrors++;
				printerr( "Error_15787! Incorrect return");
			}
			dir2 = new DirectoryInfo(Environment.CurrentDirectory.ToLower());
			Console.WriteLine(dir2.Name);
			iCountTestcases++;
			if(!dir2.Name.Equals(Environment.CurrentDirectory.Substring(Environment.CurrentDirectory.LastIndexOf("\\")+1).ToLower())) {
				iCountErrors++;
				printerr( "Error_2yg77! Incorrect return");
			}
#endif
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
		Co5660get_Name cbA = new Co5660get_Name();
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
