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
using System.Text;
using System.IO;
using System.Collections;
using System.Globalization;
public class Co5525Move_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Move(String)";
	public static String s_strTFName        = "Co5525Move_str.cs";
	public static String s_strTFAbbrev      = "Co5524";
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
			FileInfo fil2 = null;
			DirectoryInfo dir2 = null;
			strLoc = "Loc_099u8";
			dir2 = Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			try {
				dir2.MoveTo(null);
				iCountErrors++;
				printerr( "Error_298dy! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_298yt! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_209xj! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_098gt";
			dir2 = Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			try {
				dir2.MoveTo(String.Empty);
				iCountErrors++;
				printerr( "Error_3987c! Expected exception not thrown");
			} catch ( ArgumentException aexc ) {
				printinfo( "Info_89t7g! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_9092c! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			dir2.Delete(true);
			strLoc = "Loc_98hvc";
			dir2= Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			dir2.MoveTo("Test3");
			try {
				dir2 = new DirectoryInfo("Test3");
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2881s! Directory not moved, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_2908x";
			dir2 = Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			try {
				dir2.MoveTo(".");
				iCountErrors++;
				printerr( "Error_2091z! Expected exception not thrown,");
			} catch (IOException ) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2100s! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_1999s";
			dir2 = Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			try {
				dir2.MoveTo("..");
				iCountErrors++;
				printerr( "Error_2091b! Expected exception not thrown");
			} catch (IOException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_01990! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			dir2.Delete(true);
			strLoc = "Loc_498vy";
			dir2 = Directory.CreateDirectory("Test 5525");
			iCountTestcases++;
			try {
				dir2.MoveTo("         ");
				iCountErrors++;
				printerr( "Error_209uc! Expected exception not thrown");
				fil2.Delete();
			} catch (ArgumentException aexc) {
				printinfo("Info_988t7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_28829! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			dir2.Delete(true);
			strLoc = "Loc_98399";
			dir2 = Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			try {
				dir2.MoveTo("\t");
				iCountErrors++;
				printerr( "Error_2091c! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo("Info_9t8g7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_8374v! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_2908y";
			StringBuilder sb = new StringBuilder();
			int current = Environment.CurrentDirectory.Length;
			int templength = 260-current-1;
			for(int i = 0 ; i < templength ; i++)
				sb.Append("a");
			iCountTestcases++;
			dir2 = Directory.CreateDirectory("Test5525");
			try {
				dir2.MoveTo(sb.ToString());
				iCountErrors++;
				printerr( "Error_109ty! Expected exception not thrown");
			} catch (PathTooLongException aexc) { 
				printinfo( "Error_29010! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_109dv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_48fyf";
			sb = new StringBuilder();
			current = Environment.CurrentDirectory.Length;
			templength = 260;
			for(int i = 0 ; i < templength ; i++)
				sb.Append("a");
			iCountTestcases++;
			dir2 = Directory.CreateDirectory("Test5525");
			try {
				dir2.MoveTo(sb.ToString());
				iCountErrors++;
				printerr( "Error_109ty! Expected exception not thrown");
			} catch (PathTooLongException aexc) { 
				printinfo( "Error_29010! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_109dv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_209ud";
			dir2 = Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			try {
				dir2.MoveTo("Test\\Test\\Test");
				iCountErrors++;
				printerr( "Error_1039s! Expected exception not thrown");
				fil2.Delete();
			} catch ( IOException ) {
			} catch ( Exception exc ) {
				iCountErrors++;
				printerr( "Error_2019u! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_2498x";
			iCountTestcases++;
			dir2 = Directory.CreateDirectory("Test5525a");
			dir2 = Directory.CreateDirectory("Test5525");
			try {
				dir2.MoveTo("Test5525a");
				iCountErrors++;
				printerr( "Error_2498h! Expected exception not thrown");
			} catch (IOException ) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_289vt! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			new DirectoryInfo("Test5525a").Delete(true);
			strLoc = "Loc_2798r";
			dir2 = Directory.CreateDirectory("Test5525");
			iCountTestcases++;
			try {
				dir2.MoveTo("******.***");
				iCountErrors++;
				printerr( "Error_298hv! Expected exception not thrown");
			} catch (ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2199d! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Delete(true);
			strLoc = "Loc_209ux";
			dir2 = Directory.CreateDirectory("Test5525");
			DirectoryInfo subdir = dir2.CreateSubdirectory("Test5525");
			dir2.MoveTo("NewTest5525");
			iCountTestcases++;
			try {
				subdir = new DirectoryInfo("NewTest5525\\Test5525");
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_290u1! Failed to move Folder, exc=="+exc.ToString());
			}
			subdir.Delete(true);
			dir2.Delete(true);
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
		Co5525Move_str cbA = new Co5525Move_str();
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
