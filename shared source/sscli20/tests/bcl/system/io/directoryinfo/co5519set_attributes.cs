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
public class Co5519set_Attributes
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.CreationTime";
	public static String s_strTFName        = "Co5519set_Attributes.cs";
	public static String s_strTFAbbrev      = "Co5519";
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
                        DirectoryInfo dir = null;
			FileAttributes diratt;
			new DirectoryInfo(".").Attributes = new FileAttributes();
			strLoc = "Loc_20hx9";
			dir = new DirectoryInfo(".");
			diratt = dir.Attributes;
			iCountTestcases++;
			if((diratt & FileAttributes.Directory) != FileAttributes.Directory) {
				iCountErrors++;
			} 
			strLoc = "Loc_039ux";
			dir = new DirectoryInfo(".");
		        dir.Attributes = FileAttributes.ReadOnly;
			diratt = dir.Attributes;
			iCountTestcases++;
			if((diratt & ((FileAttributes.ReadOnly|FileAttributes.Directory))) != (FileAttributes.ReadOnly|FileAttributes.Directory)) {
				iCountErrors++;
				printerr( "Error_2989d! Incorrect directory attribute set");
			}
			dir.Attributes = new FileAttributes();
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
		Co5519set_Attributes cbA = new Co5519set_Attributes();
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
