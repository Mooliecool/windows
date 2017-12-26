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
public class Co5666get_Root
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Root";
	public static String s_strTFName        = "Co5666get_Root.cs";
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
			DirectoryInfo dir2;
			String dirName = s_strTFAbbrev+"TestDir";
			String str2;
			iCountTestcases++; 
#if PLATFORM_UNIX
			strLoc = "Loc_49b78";
			dir2 = new DirectoryInfo("/").Root;
			str2 = dir2.FullName;
		  	iCountTestcases++;
			if(!str2.Equals("/")) {
				iCountErrors++;
				printerr("Error_69y7b! Unexpected parent=="+dir2.FullName);
			}
			strLoc = "Loc_9876b";
			dir2 = new DirectoryInfo("/a/b/c/d").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("/")) {
				iCountErrors++;
				printerr( "Error_298yg! Unexpected parent=="+str2);
			}
			strLoc = "Loc_75y7b";
			dir2 = new DirectoryInfo("/a/b/c").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("/")) {
				iCountErrors++;
				printerr( "Error_9887b! Unexpected parent=="+str2);
			}
			strLoc = "Loc_2984y";
			dir2 = new DirectoryInfo("/Test/.././Test/Test").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("/")) {
				iCountErrors++;
				printerr( "Error_20928! Unexpected parent=="+str2);
			}
			strLoc = "Loc_8y76y";
			dir2 = new DirectoryInfo("/My Samples/Hello To The World/Test").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("/")) {
				iCountErrors++;
				printerr( "Error_9019c! Unexpected parent=="+str2);
			} 
#else   // PLATFORM_UNIX
			strLoc = "Loc_49b78";
			dir2 = new DirectoryInfo("C:\\").Root;
			str2 = dir2.FullName;
		  	iCountTestcases++;
			if(!str2.Equals("C:\\")) {
				iCountErrors++;
				printerr("Error_69y7b! Unexpected parent=="+dir2.FullName);
			}
			strLoc = "Loc_98ygg";
			dir2 = new DirectoryInfo("\\Machine\\Test").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			String root = Environment.CurrentDirectory.Substring(0, Environment.CurrentDirectory.IndexOf('\\')+1);
			if(!str2.Equals(root)) {
				iCountErrors++;
				printerr( "Error_91y7b! Unexpected parent=="+str2);
			}
			strLoc = "Loc_yg7bk";
			dir2 = new DirectoryInfo("\\\\Machine\\Test").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("\\\\Machine\\Test")) {
				iCountErrors++;
				printerr( "Error_4y7gb! Unexpected parent=="+dir2.FullName);
			}
			strLoc = "Loc_9876b";
			dir2 = new DirectoryInfo("X:\\a\\b\\c\\d\\").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("X:\\")) {
				iCountErrors++;
				printerr( "Error_298yg! Unexpected parent=="+str2);
			}
			strLoc = "Loc_75y7b";
			dir2 = new DirectoryInfo("X:\\a\\b\\c").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("X:\\")) {
				iCountErrors++;
				printerr( "Error_9887b! Unexpected parent=="+str2);
			}
			strLoc = "Loc_y7t98";
			dir2 = new DirectoryInfo("\\\\Machine\\Test1\\Test2\\Test3").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("\\\\Machine\\Test1")) {
				iCountErrors++;
				printerr( "Error_69929! Unexpected parent=="+str2);
			} 
			strLoc = "Loc_2984y";
			dir2 = new DirectoryInfo("X:\\Test\\..\\.\\Test\\Test").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("X:\\")) {
				iCountErrors++;
				printerr( "Error_20928! Unexpected parent=="+str2);
			}
			strLoc = "Loc_8y76y";
			dir2 = new DirectoryInfo("X:\\My Samples\\Hello To The World\\Test").Root;
			str2 = dir2.FullName;
			iCountTestcases++;
			if(!str2.Equals("X:\\")) {
				iCountErrors++;
				printerr( "Error_9019c! Unexpected parent=="+str2);
			} 
#endif  // PLATFORM_UNIX
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
		Co5666get_Root cbA = new Co5666get_Root();
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
