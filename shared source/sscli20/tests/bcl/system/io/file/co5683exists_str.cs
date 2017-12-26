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
public class Co5683FileExists_str
{
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.Exists(String)";
	public static String s_strTFName        = "Co5683FileExists_str.cs";
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
			String filName = s_strTFAbbrev+"TestFile";
			if(File.Exists(filName))
				File.Delete(filName);
			strLoc = "Loc_t987b";
			iCountTestcases++;
			try {
				if(File.Exists(null)){
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
				if(File.Exists(String.Empty)){
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
			if(File.Exists(".")) {
				iCountErrors++;
				printerr( "Error_95428! Incorrect return value");
			}
			iCountTestcases++;
			if(File.Exists(Environment.CurrentDirectory)) {
				iCountErrors++;
				printerr( "Error_97t67! Incorrect return value");
			}
			strLoc = "Loc_y7t03";
			iCountTestcases++;
			if(File.Exists("..")) {
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
			if(File.Exists("..")) {
				iCountErrors++;
				printerr( "Error_t987y! Incorrect return value");
			}
			Environment.CurrentDirectory = tmpDir;
			strLoc = "Loc_t993c";
			iCountTestcases++;
			if(File.Exists("Da drar vi til fjells")) {
				iCountErrors++;
				printerr( "Error_6895b! Incorrect return value");
			}
			strLoc = "Loc_t899t";
			StringBuilder sb = new StringBuilder();
			for(int i = 0 ; i < 500 ; i++) 
				sb.Append(i);
			iCountTestcases++;
			try {
				if(File.Exists(sb.ToString())){
					iCountErrors++;
					printerr( "Error_20937! Expected exception not thrown");
				}
			} catch (PathTooLongException pexc) {
				printinfo( "Info_59831! Caught expected exception, pexc=="+pexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_7159s! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_2y78g";
			new FileStream(filName, FileMode.Create).Close();
			iCountTestcases++;
			if(!File.Exists(filName)) {
				iCountErrors++;
				printerr( "Error_9t821! Returned false for existing file");
			}
			iCountTestcases++;
			if(!File.Exists(Environment.CurrentDirectory+"\\"+filName)) {
				iCountErrors++;
				printerr( "Error_9198v! Returned false for existing file");
			}			
			File.Delete(filName);
			iCountTestcases++;
			if(File.Exists(filName)) {
				iCountErrors++;
				printerr( "Errro_197bb! Returned true for deleted file");
			}
			strLoc = "Loc_298g7";
			String tmp = filName+"   "+filName;
			new FileStream(tmp, FileMode.Create).Close();
			iCountTestcases++;
			if(!File.Exists(tmp)) {
				iCountErrors++;
				printerr( "Error_01y8v! Returned incorrect value");
			}
			strLoc = "Loc_398vy8";
			iCountTestcases++;
			try {
				if(File.Exists("*")) {
					iCountErrors++;
					printerr( "Error_4979c! File with wildcard exist");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_498u9! Unexpected exception thrown, exc=="+exc.ToString());
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
			Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
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
		Co5683FileExists_str cbA = new Co5683FileExists_str();
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
