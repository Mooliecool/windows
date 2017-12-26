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
public class Co5759Move_str_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Move(String,String)";
	public static String s_strTFName        = "Co5759Move_str_str.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
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
			String dirName = s_strTFAbbrev+"Dir";
			DirectoryInfo dir2 = null ;
			if(Directory.Exists("\\TempDirectory"))
				Directory.Delete("\\TempDirectory", true);
			if(Directory.Exists(dirName))
				Directory.Delete(dirName,true);
			strLoc = "Loc_00001";
			iCountTestcases++;
			try {
				Directory.Move(null, dirName);
				iCountErrors++;
				printerr( "Error_00002! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_00003! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00004! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				Directory.Move(dirName, null);
				iCountErrors++;
				printerr( "Error_00005! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_00006! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00007! Incorrect exception thrown, exc=="+exc.ToString());
			} 			
			strLoc = "Loc_00008";
			iCountTestcases++;
			try {
				Directory.Move(String.Empty, dirName);
				iCountErrors++;
				printerr( "Error_00008! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo( "Info_00009! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00010! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				Directory.Move(dirName, String.Empty);
				iCountErrors++;
				printerr( "Error_00011! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo( "Info_00012! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00013! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_00014";
			iCountTestcases++;
			try {
				Directory.Move("NonExistentDirectory", dirName);
				iCountErrors++;
				printerr( "Error_00015! Expected exception not thrown");
			} catch (DirectoryNotFoundException dexc) {
				printinfo( "Info_00016! Caught expected exception, dexc=="+dexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00017! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_00018";
			iCountTestcases++;
			try {
				Directory.Move(".", ".");
				iCountErrors++;
				printerr( "Error_00019! Expected exception not thrown");
			} catch (IOException aexc) {
				printinfo( "Info_00020! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00021! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_00025";
			Directory.CreateDirectory(dirName);
			iCountTestcases++;
			try {
				Console.WriteLine( "Root directory..." + Directory.GetCurrentDirectory().Substring( 0,3 ) );
				if( Directory.GetCurrentDirectory().Substring( 0,3 ) == @"d:\" || Directory.GetCurrentDirectory().Substring( 0,3 ) == @"D:\")
					Directory.Move(Directory.GetCurrentDirectory() +"\\"+dirName, "C:\\TempDirectory");
				else
					Directory.Move(Directory.GetCurrentDirectory() +"\\"+dirName, "D:\\TempDirectory");
				iCountErrors++;
				printerr( "Error_00026! Expected exception not thrown");
			} catch (IOException aexc) {
				printinfo( "Info_00027! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00000! Incorrect exception thrown, exc=="+exc.ToString());
			}
			if( Directory.Exists( dirName ) )
				Directory.Delete(dirName, true);
			strLoc = "Loc_00036";
			iCountTestcases++;
			try {
				Directory.Move(".", "Temp*");
				iCountErrors++;
				printerr( "Error_00037! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo("Info_00038! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00039! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_00040";
			iCountTestcases++;
			try {
				Directory.Move(".", "<MyDirectory>");
				iCountErrors++;
				printerr( "Error_00041! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo("Info_00042! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00043 Incorret exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_00044";
			String str = "";
			for(int i = 0 ; i < 250 ; i++)
				str += "a";
			iCountTestcases++;
			try {
				Directory.Move(".", str);
				iCountErrors++;
				printerr( "Error_00045! Expected exception not thrown");
			} catch (PathTooLongException pexc) {
				printinfo( "Info_00046! Caught expected exception, pexc=="+pexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00047! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_00048";
			iCountTestcases++;
			Directory.CreateDirectory(dirName);
			try {
				Directory.Move(dirName, "X:\\Temp");
				iCountErrors++;
				printerr( "Error_00049! Expected exception not thrown");
			} catch (IOException aexc) {
				printinfo( "Info_00050! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00051! Incorrect exception thrown, exc=="+exc.ToString());
			}
			Directory.Delete(dirName);
			strLoc = "Loc_00052";
			Directory.CreateDirectory(dirName);
#if PLATFORM_UNIX
            String targetDirectory = "This is my directory";
#else
            String targetDirectory = "\\This is my directory";
#endif
			Directory.Move(dirName, targetDirectory);
			iCountTestcases++;
			if(!Directory.Exists(targetDirectory)) {
				iCountErrors++;
				printerr( "Error_00053! Destination directory missing");
			}
			Directory.Delete(targetDirectory);
			if(Directory.Exists(dirName)) 
				Directory.Delete(dirName, true);
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
		Co5759Move_str_str cbA = new Co5759Move_str_str();
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
