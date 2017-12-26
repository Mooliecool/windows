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
public class Co5658Exists_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Exists(String)";
	public static String s_strTFName        = "Co5658Exists_str.cs";
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
			String str1;
			DirectoryInfo dir2;
			strLoc = "Loc_t987b";
			iCountTestcases++;
			try {
				if(Directory.Exists(null)){
					iCountErrors++;
					printerr( "Error_49939! Expected exception not thrown");
				}
			} catch (ArgumentNullException aexc) {
				printinfo("Info_0193v! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_6019b! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_199gb";
			iCountTestcases++;
			try {
				if(Directory.Exists(String.Empty)){
					iCountErrors++;
					printerr( "Error_109tg! Expected exception not thrown");
				}
			} catch (ArgumentException aexc) {
				printinfo( "Info_t9763! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_6t69b! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_276t8";
			iCountTestcases++;
			if(!Directory.Exists(".")) {
				iCountErrors++;
				printerr( "Error_95428! Incorrect return value");
			}
			iCountTestcases++;
			if(!Directory.Exists(Environment.CurrentDirectory)) {
				iCountErrors++;
				printerr( "Error_97t67! Incorrect return value");
			}
			strLoc = "Loc_y7t03";
			iCountTestcases++;
			if(!Directory.Exists("..")) {
				iCountErrors++;
				printerr( "Error_290bb! Incorrect return value");
			}
			String tmpDir = Environment.CurrentDirectory;
#if PLATFORM_UNIX
                        Environment.CurrentDirectory = "/";
#else
			Environment.CurrentDirectory = "C:\\";
#endif
			iCountTestcases++;
			if(!Directory.Exists("..")) {
				iCountErrors++;
				printerr( "Error_t987y! Incorrect return value");
			}
			Environment.CurrentDirectory = tmpDir;
			strLoc = "Loc_t993c";
			iCountTestcases++;
			if(Directory.Exists("Da drar vi til fjells")) {
				iCountErrors++;
				printerr( "Error_6895b! Incorrect return value");
			}
#if !PLATFORM_UNIX
                        // "\\\\\\" is an invalid path on Win32 (not a valid UNC path), but
                        // is valid on Unix systems.
			iCountTestcases++;
			if(Directory.Exists("\\\\\\\\\\\\")) {
				iCountErrors++;
				printerr( "Error_67903! Incorrect return value");
			}
#endif //!PLATFORM_UNIX
			iCountTestcases++;
			if(Directory.Exists("XX:\\")) {
				iCountErrors++;
				printerr( "Error_29867! Incorrect return");
			}
			strLoc = "Loc_t899t";
			StringBuilder sb = new StringBuilder();
			for(int i = 0 ; i < 500 ; i++) 
				sb.Append(i);
			iCountTestcases++;
			try {
				if(Directory.Exists(sb.ToString())){
					iCountErrors++;
					printerr( "Error_20937! Expected exception not thrown");
				}
			} catch (PathTooLongException pexc) {
				printinfo( "Info_59831! Caught expected exception, pexc=="+pexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_7159s! Incorrect exception thrown, exc=="+exc.ToString());
			}
#if !PLATFORM_UNIX // Tests case-insensitivity in the filesystem
			strLoc = "Loc_2498c";
			str1 = "Co5658TestDir\\Test";
			dir2 = Directory.CreateDirectory(str1);
			iCountTestcases++;
			if(!Directory.Exists(str1.ToUpper(CultureInfo.InvariantCulture))) {
				iCountErrors++;
				printerr( "Error_219yg! Incorrect return");
			} 
			iCountTestcases++;
			if(!Directory.Exists(str1.ToLower(CultureInfo.InvariantCulture))) {
				iCountErrors++;
				printerr( "Error_6y5y7! Incorrect return");
			}
			dir2 = new DirectoryInfo("Co5658TestDir");
			dir2.Delete(true);
#endif //!PLATFORM_UNIX
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
		Co5658Exists_str cbA = new Co5658Exists_str();
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
