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
public class Co9027GetLastAccessTime_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetLastAccessTime()";
	public static String s_strTFName        = "Co9027GetLastAccessTime_str.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		try
		{
			String dirName = s_strTFAbbrev+"TestDir";			
			DirectoryInfo dir2;
			if(Directory.Exists(dirName))
				Directory.Delete(dirName, true);
			strLoc = "Loc_r8r7j";
			dir2 = Directory.CreateDirectory(dirName);
			dir2.GetFileSystemInfos (dirName);
			dir2.Refresh();
			iCountTestcases++;
			try {
				Console.WriteLine(Directory.GetLastAccessTime(dirName));
				if((DateTime.Now-Directory.GetLastAccessTime(dirName)).Days != 0) {
					iCountErrors++;
					Console.WriteLine((DateTime.Now-dir2.LastAccessTime).Days);
					printerr( "Error_20hjx! Access time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_20fhd! Unexpected exceptiont thrown: "+exc.ToString());
				printinfo( "");
			}
			strLoc = "Loc_20yxc";
			dir2.GetFiles();
			dir2.Refresh();
			Thread.Sleep(1000);
			iCountTestcases++;
			try {
				if((DateTime.Now-Directory.GetLastAccessTime(dirName)).Days != 0) {
					iCountErrors++;
					Console.WriteLine((DateTime.Now-Directory.GetLastAccessTime(dirName)).Days);
					printerr( "Eror_209x9! LastAccessTime is way off");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_209jx! Unexpected exception thrown: "+exc.ToString());
			} 
			dir2.Delete();
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
		Co9027GetLastAccessTime_str cbA = new Co9027GetLastAccessTime_str();
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
