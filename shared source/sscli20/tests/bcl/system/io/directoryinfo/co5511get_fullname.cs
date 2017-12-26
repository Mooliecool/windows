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
public class Co5511get_FullName
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.FullName";
	public static String s_strTFName        = "Co5511get_FullName.cs";
	public static String s_strTFAbbrev      = "Co5511";
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
			DirectoryInfo dir=null;
                        strLoc = "Loc_2908x";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(null);
				iCountErrors++;
				printerr( "Error_2908x! Expected exception not thrown, dir=="+dir.ToString());
			} catch ( ArgumentNullException aexc ) {
				Console.WriteLine("Info_298hx! Caught expected exception=="+aexc.Message);
			} catch ( Exception exc ) {
				iCountErrors++;
				printerr( "Error_209ux! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_909d9";
			dir = new DirectoryInfo(Environment.CurrentDirectory);
			iCountTestcases++;
			if(!dir.FullName.Equals(Environment.CurrentDirectory)) {
				iCountErrors++;
				printerr( "Error_209xu! Incorrect Directory returned , dir=="+dir.FullName);
			}
			strLoc = "Loc_2999s";
			iCountTestcases++;
			dir = new DirectoryInfo("This directory does not exist");
            String curDirectory = Environment.CurrentDirectory.Replace("/", "\\");
            String dirName = dir.FullName.Replace("/", "\\");
			if(!dirName.Equals(curDirectory+"\\This directory does not exist")) {
				iCountErrors++;
				printerr( "Error_109z9! Incorrect directory name, dir=="+dir.ToString());
			}
#if !PLATFORM_UNIX
			strLoc = "Loc_20u9x";
			dir = new DirectoryInfo("c:\\");
			iCountTestcases++;
			if(!dir.FullName.Equals("c:\\")) {
				iCountErrors++;
				printerr( "Error_2098x! Incorrect dir returned=="+dir.FullName);
			}
			strLoc = "Loc_099s8";
			dir = new DirectoryInfo("C:\\");
			DirectoryInfo dir2 = new DirectoryInfo("C:\\");
			iCountTestcases++;
			if(!dir.FullName.Equals(dir2.FullName)) {
				iCountErrors++;
				printerr( "Error_2982y! dir=="+dir.FullName+" , dir2=="+dir2.FullName);
				printinfo("");
			}
#endif  // !PLATFORM_UNIX
			Directory.CreateDirectory("TestDir");
			dir = new DirectoryInfo("TestDir");
			iCountTestcases++;
            dirName = dir.FullName.Replace("/", "\\");
			if(!dirName.Equals(curDirectory+"\\TestDir")) {
				iCountErrors++;
				printerr( "Error_298yx! Incorrect dir constructed, dir=="+dir.FullName);
			}
			dir.Delete();
			strLoc = "Loc_298yb";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo("      ");
                                dir.Create();
				iCountErrors++;
				printerr( "Error_0919x! Expected exception not thrown, dir=="+dir.FullName);
			} catch ( ArgumentException ) {
			} catch ( Exception exc) {
				iCountErrors++;
				printerr( "Error_4577c! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_949gg";
			dir = new DirectoryInfo(".");
			iCountTestcases++;
			if(!dir.FullName.Equals(Environment.CurrentDirectory)) {
				iCountErrors++;
				printerr("Error_299xu! Incorrect dir constructed, dir=="+dir.FullName);
			} 
			strLoc = "Loc_9937a";
			dir = new DirectoryInfo("..");
			iCountTestcases++;
            String strParent = curDirectory.Substring(0, curDirectory.LastIndexOf("\\"));
            if ( strParent.IndexOf("\\") == -1 && strParent.IndexOf("/") == -1 ) 
                strParent = strParent + "\\" ;
            Console.WriteLine( strParent );
            dirName = dir.FullName.Replace("/", "\\");
            strParent = strParent.Replace("/", "\\");
			if(!dirName.Equals( strParent )) {
				iCountErrors++;
				printerr( "Error_298xy! Incorrect dir returned, dir=="+dir.FullName);
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
		Co5511get_FullName cbA = new Co5511get_FullName();
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
