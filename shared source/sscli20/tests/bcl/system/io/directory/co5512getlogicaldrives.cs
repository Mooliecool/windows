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
public class Co5512GetLogicalDrives
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetLogicalDrives";
	public static String s_strTFName        = "Co5512GetLogicalDrives.cs";
	public static String s_strTFAbbrev      = "Co5512";
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
			String[] drives = new String[0];
			Hashtable ht_drives = new Hashtable();
			drives = Directory.GetLogicalDrives();
			Console.WriteLine("DriveCount: "+drives.Length);
			for(int i = 0 ; i < drives.Length ; i++) {
				iCountTestcases++;
				if(drives[i].Length != 3) {
					iCountErrors++;
					printerr( "Error_209xh! Incorrect logical drive name, drive=="+drives[i]);
				}
				try {
					ht_drives.Add(drives[i], "Drive: "+drives[i]);
				} catch (Exception) {
					iCountErrors++;
					printerr( "Error_29x88! Drive already listed: drive=="+drives[i]);
				}
				if(!Char.IsLetter((drives[i])[0])) {
					iCountErrors++;
					printerr( "Error_2092j! Incorrect drive letter: drive=="+drives[i]);
				}
				if((drives[i])[1] != ':' && (drives[i])[2] != '\\') {
					iCountErrors++;
					printerr( "Error_20109! Incorrect drive format: drive=="+drives[i]);
				}
				Console.WriteLine(drives[i]);
			}
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
		Co5512GetLogicalDrives cbA = new Co5512GetLogicalDrives();
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
