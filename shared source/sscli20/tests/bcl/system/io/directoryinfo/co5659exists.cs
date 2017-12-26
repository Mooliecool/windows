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
public class Co5659Exists
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Exists(String)";
	public static String s_strTFName        = "Co5659Exists.cs";
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
			DirectoryInfo dir2= null;
			strLoc = "Loc_199gb";
			iCountTestcases++;
			try {
				Console.WriteLine(new DirectoryInfo("").Exists);
				iCountErrors++;
				printerr( "Error_109tg! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo( "Info_t9763! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_6t69b! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_276t8";
			iCountTestcases++;
			dir2 = new DirectoryInfo(".");
			if(!dir2.Exists) {
				iCountErrors++;
				printerr( "Error_95428! Incorrect return value");
			}
			iCountTestcases++;
			dir2 = new DirectoryInfo(Environment.CurrentDirectory);
			if(!dir2.Exists) {
				iCountErrors++;
				printerr( "Error_97t67! Incorrect return value");
			}
			strLoc = "Loc_y7t03";
			iCountTestcases++;
			dir2 = new DirectoryInfo("..");
			if(!dir2.Exists) {
				iCountErrors++;
				printerr( "Error_290bb! Incorrect return value");
			}
			String tmpDir = Environment.CurrentDirectory;
#if !PLATFORM_UNIX
			Environment.CurrentDirectory = "C:\\";
			dir2 = new DirectoryInfo("..");
			iCountTestcases++;			
			if(!dir2.Exists) {
				iCountErrors++;
				printerr( "Error_t987y! Incorrect return value");
			}
#endif
			Environment.CurrentDirectory = tmpDir;
			strLoc = "Loc_t993c";
			try{
    			iCountTestcases++;
    			dir2 = new DirectoryInfo("Da drar vi til fjells");
    			if(dir2.Exists) {
    				iCountErrors++;
    				printerr( "Error_6895b! Incorrect return value");
    			}
            } catch (Exception exc) {
            	iCountErrors++;
            	printerr( "Error_2947c! Unexpected exception thrown, exc=="+exc.ToString());
            }
#if !PLATFORM_UNIX
            try{
    			iCountTestcases++;
    			dir2 = new DirectoryInfo("xx:\\");
                iCountErrors++;
                printerr( "Error_29867! Expected exception not occured");
            } catch (NotSupportedException ) {
            } catch (Exception exc) {
            	iCountErrors++;
            	printerr( "Error_rewrew! Unexpected exception thrown, exc=="+exc.ToString());
            }
#endif
			strLoc = "Loc_t899t";
			StringBuilder sb = new StringBuilder();
			for(int i = 0 ; i < 500 ; i++) 
				sb.Append(i);
			iCountTestcases++;
			try {			   
				dir2 = new DirectoryInfo(sb.ToString());
				iCountErrors++;
				printerr( "Error_20937! Expected exception not thrown");
			} catch (PathTooLongException pexc) {
				printinfo( "Info_59831! Caught expected exception, pexc=="+pexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_7159s! Incorrect exception thrown, exc=="+exc.ToString());
			}
#if !PLATFORM_UNIX
			strLoc = "Loc_t97g8";
			dir2 = new DirectoryInfo(Environment.CurrentDirectory.ToUpper());
			iCountTestcases++;
			if(!dir2.Exists) {
				iCountErrors++;
				printerr( "Error_15787! Incorrect return");
			}
			dir2 = new DirectoryInfo(Environment.CurrentDirectory.ToLower());
			iCountTestcases++;
			if(!dir2.Exists) {
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
		Co5659Exists cbA = new Co5659Exists();
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
