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
public class Co5751GetParent_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Directory.GetParent(String)";
	public static String s_strTFName        = "Co5751GetParent_str.cs";
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
			DirectoryInfo dirInfo= null;
			String dirName = s_strTFAbbrev+"TestDir";
			String str2;
			strLoc = "Loc_49b78";
#if PLATFORM_UNIX
                        dirInfo = Directory.GetParent("/");
                        iCountTestcases++;
                        if(dirInfo != null) {
                                iCountErrors++;
                                printerr("Error_69y7b! Unexpected parent=="+dirInfo.ToString());
                        }
                        strLoc = "Loc_9876b";
                        dirInfo = Directory.GetParent("\\a\\b\\c\\d");
                        str2 = dirInfo.ToString();
                        iCountTestcases++;
                        if(!str2.Equals("/a/b/c")) {
                                iCountErrors++;
                                printerr( "Error_298yg! Unexpected parent=="+str2);
                        }
                        strLoc = "Loc_75y7b";
                        dirInfo = Directory.GetParent("\\a\\b\\c");
                        str2 = dirInfo.ToString();
                        iCountTestcases++;
                        if(!str2.Equals("/a/b")) {
                                iCountErrors++;
                                printerr( "Error_9887b! Unexpected parent=="+str2);
                        }
                        strLoc = "Loc_2984y";
                        dirInfo = Directory.GetParent("\\Test\\..\\.\\Test");
                        str2 = dirInfo.ToString();
                        iCountTestcases++;
                        if(!str2.Equals("/")) {
                                iCountErrors++;
                                printerr( "Error_20928! Unexpected parent=="+str2);
                        }
                        strLoc = "Loc_8y76y";
                        dirInfo = Directory.GetParent("\\My Samples\\Hello To The World\\Test");
                        str2 = dirInfo.ToString();
                        iCountTestcases++;
                        if(!str2.Equals("/My Samples/Hello To The World")) {
                                iCountErrors++;
                                printerr( "Error_9019c! Unexpected parent=="+str2);
                        }
#else
			dirInfo = Directory.GetParent("C:\\");
		  	iCountTestcases++;
			if(dirInfo != null) {
				iCountErrors++;
				printerr("Error_69y7b! Unexpected parent=="+dirInfo.ToString());
			}
			strLoc = "Loc_98ygg";
			dirInfo = Directory.GetParent(@"\\Vinayr4\test\test");
			str2 = dirInfo.ToString();
			iCountTestcases++;
			if(!str2.Equals(@"\\Vinayr4\test")) {
				iCountErrors++;
				printerr( "Error_91y7b! Unexpected parent=="+str2);
			}
			strLoc = "Loc_yg7bk";
			dirInfo = Directory.GetParent("\\\\Machine\\Test");
			iCountTestcases++;
			if(dirInfo != null) {
				iCountErrors++;
				printerr( "Error_4y7gb! Unexpected parent=="+dirInfo.ToString());
			}
			strLoc = "Loc_9876b";
			dirInfo = Directory.GetParent("X:\\a\\b\\c\\d");
			str2 = dirInfo.ToString();
			iCountTestcases++;
			if(!str2.Equals("X:\\a\\b\\c")) {
				iCountErrors++;
				printerr( "Error_298yg! Unexpected parent=="+str2);
			}
			strLoc = "Loc_75y7b";
			dirInfo = Directory.GetParent("X:\\a\\b\\c");
			str2 = dirInfo.ToString();
			iCountTestcases++;
			if(!str2.Equals("X:\\a\\b")) {
				iCountErrors++;
				printerr( "Error_9887b! Unexpected parent=="+str2);
			}
			strLoc = "Loc_y7t98";
			dirInfo = Directory.GetParent("\\\\Machine\\Test1\\Test2");
			str2 = dirInfo.ToString();
			iCountTestcases++;
			if(!str2.Equals("\\\\Machine\\Test1")) {
				iCountErrors++;
				printerr( "Error_69929! Unexpected parent=="+str2);
			} 
			strLoc = "Loc_2984y";
			dirInfo = Directory.GetParent("X:\\Test\\..\\.\\Test");
			str2 = dirInfo.ToString();
			iCountTestcases++;
			if(!str2.Equals("X:\\")) {
				iCountErrors++;
				printerr( "Error_20928! Unexpected parent=="+str2);
			}
			strLoc = "Loc_8y76y";
			dirInfo = Directory.GetParent("X:\\My Samples\\Hello To The World\\Test");
			str2 = dirInfo.ToString();
			iCountTestcases++;
			if(!str2.Equals("X:\\My Samples\\Hello To The World")) {
				iCountErrors++;
				printerr( "Error_9019c! Unexpected parent=="+str2);
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
		Co5751GetParent_str cbA = new Co5751GetParent_str();
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
