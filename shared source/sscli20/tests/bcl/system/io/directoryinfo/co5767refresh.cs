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
public class Co5767Refresh
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Refresh()";
	public static String s_strTFName        = "Co5767Refresh.cs";
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
			String dirName = s_strTFAbbrev+"Dir";
			if(Directory.Exists(dirName))
			   Directory.Delete(dirName);
			strLoc = "Loc_00001";
			Directory.CreateDirectory(dirName);
			dir2 = new DirectoryInfo(dirName);
			Directory.Delete(dirName);
			iCountTestcases++;
			dir2.Refresh();
			strLoc = "Loc_00005";
			if(Directory.Exists("Temp001"))
				Directory.Delete("Temp001");
			iCountTestcases++;
			Directory.CreateDirectory(dirName);
			dir2 = new DirectoryInfo(dirName);
			dir2.MoveTo("Temp001");
			dir2.Refresh();
			Directory.Delete("Temp001");
			strLoc = "Loc_00006";
			iCountTestcases++;
			Directory.CreateDirectory(dirName);
			dir2 = new DirectoryInfo(dirName);
			Console.WriteLine(dir2.Attributes);
   			if(((Int32)dir2.Attributes & (Int32)FileAttributes.ReadOnly) != 0) {
				iCountErrors++;
				printerr( "Error_00007! Attribute set before refresh");
			}
			dir2.Attributes = FileAttributes.ReadOnly;                        
			dir2.Refresh();
			iCountTestcases++;
			if(((Int32)dir2.Attributes & (Int32)FileAttributes.ReadOnly) <= 0) {
				iCountErrors++;
				printerr( "Error_00008! Object not refreshed after setting readonly");
			}
			dir2.Attributes = new FileAttributes();
			dir2.Refresh();
   			if(((Int32)dir2.Attributes & (Int32)FileAttributes.ReadOnly) != 0) {
				iCountErrors++;
				printerr( "Error_00009! Object not refreshed after removing readonly");
			}
			if(Directory.Exists(dirName))
			   Directory.Delete(dirName);
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
		Co5767Refresh cbA = new Co5767Refresh();
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
