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
public class Co5523ToString
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.ToString";
	public static String s_strTFName        = "Co5523ToString.cs";
	public static String s_strTFAbbrev      = "Co5523";
	public static String s_strTFPath        = Environment.CurrentDirectory.ToString();
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
            String curDirectory = Environment.CurrentDirectory.ToString().Replace("/", "\\");
            String dirName = null;
			strLoc = "Loc_909d9";
			dir = new DirectoryInfo(curDirectory);
			iCountTestcases++;
			if(!dir.ToString().Equals(curDirectory)) {
				iCountErrors++;
				printerr( "Error_209xu! Incorrect Directory returned , dir=="+dir.ToString());
			}
			strLoc = "Loc_20u9x";
			dir = new DirectoryInfo("c:\\");
			iCountTestcases++;
			if(!dir.ToString().Equals("c:\\")) {
				iCountErrors++;
				printerr( "Error_2098x! Incorrect dir returned=="+dir.ToString());
			}
			Directory.CreateDirectory("TestDir");
			dir = new DirectoryInfo("TestDir");
			iCountTestcases++;
                        Console.WriteLine(dir.FullName );
                        Console.WriteLine( curDirectory+"\\TestDir" );
            String testDir = (curDirectory + "\\TestDir").Replace("/", "\\");
            dirName = dir.FullName.Replace("/", "\\");
			if(!dirName.Equals(testDir)) {
				iCountErrors++;
				printerr( "Error_298yx! Incorrect dir constructed, dir=="+dir.ToString());
			}
			dir.Delete();
			strLoc = "Loc_298yb";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo("      ");
                                dir.Create();
				iCountErrors++;
				printerr( "Error_09rux! Expected exception not thrown, dir=="+dir.ToString());
			} catch ( ArgumentException ) {
			} catch ( Exception exc) {
				iCountErrors++;
				printerr( "Error_4577c! Incorrect exception thrown, exc=="+exc.ToString());
				printinfo( "");
			}
			strLoc = "Loc_949gg";
			dir = new DirectoryInfo(".");
			iCountTestcases++;
            dirName = dir.FullName.Replace("/", "\\");
			if(!dirName.Equals(curDirectory)) {
				iCountErrors++;
				printerr("Error_299xu! Incorrect dir constructed, dir=="+dir.ToString());
			} 
			strLoc = "Loc_9937a";
			dir = new DirectoryInfo("..");
			iCountTestcases++;
            dirName = dir.FullName.Replace("/", "\\");
			if(!dirName.Equals(curDirectory.Substring(0, curDirectory.LastIndexOf("\\")))) {
				iCountErrors++;
				printerr( "Error_298xy! Incorrect dir returned, dir=="+dir.ToString());
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
		Co5523ToString cbA = new Co5523ToString();
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
