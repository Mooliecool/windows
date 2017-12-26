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
public class Co5589Create_str_i
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.Create(String,Int32)";
	public static String s_strTFName        = "Co5589Create_str_i.cs";
	public static String s_strTFAbbrev      = "Co5589";
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		String filName = s_strTFAbbrev+"Test.tmp";
		try
		{
			Stream fs2;
			strLoc = "Loc_89y8b";
			iCountTestcases++;
			try {
				fs2 = File.Create(null, 1);
				iCountErrors++;
				printerr( "Error_298yr! Expected exception not thrown");
				fs2.Close();
			} catch (ArgumentNullException aex) {
				printinfo("Info_8t8yb! Caught expected exception, aex=="+aex.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_987b7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_t7y84";
			iCountTestcases++;
			try {
				fs2 = File.Create(".", 1);
				iCountErrors++;
				printerr( "Error_7858c! Expected exception not thrown");
				fs2.Close();
			} catch (UnauthorizedAccessException aexc) {
				printinfo("Info_7t865! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_958vy! Incorrect exception thrown, ex=="+exc.ToString());
			}
			strLoc = "Loc_8t87b";
			iCountTestcases++;
			try {
				fs2 = File.Create(String.Empty, 1);
				iCountErrors++;
				printerr( "Error_598yb! Expected exception not thrown");
				fs2.Close();
			} catch (ArgumentException aexc) {
				printinfo("Info_87yg7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_9t5y! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_t98x9";
			iCountTestcases++;
			try {
				fs2 = File.Create(filName, -10);
				iCountErrors++;
				printerr( "Error_598cy! Expected exception not thrown");
				fs2.Close();
			} catch (ArgumentOutOfRangeException aexc) {
				printinfo("Info_287yv! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_928xy! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_887th";
			fs2 = File.Create(filName, 1000);
            Console.WriteLine( filName );
            Console.WriteLine( File.Exists(filName) );
			iCountTestcases++;
			if(!File.Exists(filName)) {
				iCountErrors++;
				printerr( "Error_2876g! File not created, file=="+filName);
			}
			iCountTestcases++;
			if(fs2.Length != 0) {
				iCountErrors++;
				printerr( "Error_t598v! Incorrect file length=="+fs2.Length);
			}
			if(fs2.Position != 0) {
				iCountErrors++;
				printerr( "Error_958bh! Incorrect file position=="+fs2.Position);
			}
			fs2.Close();
			strLoc = "Loc_89ytb";
			String currentdir = Environment.CurrentDirectory;
            filName = currentdir.Substring(0, currentdir.IndexOf('\\')+1)+filName ;
			fs2 = File.Create(filName, 1000);
			iCountTestcases++;
			if(!File.Exists( filName)) {
				iCountErrors++;
				printerr( "Error_t78g7! File not created, file=="+filName);
			}
			fs2.Close();
			strLoc = "loc_89tbh";
            filName = Environment.CurrentDirectory+"\\TestFile.tmp" ;
			fs2 = File.Create( filName , 100);
			iCountTestcases++;
			if(!File.Exists( filName )) {
				iCountErrors++;
				printerr( "Error_t87gy! File not created, file=="+filName);
			} 
			fs2.Close();
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
		Co5589Create_str_i cbA = new Co5589Create_str_i();
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
