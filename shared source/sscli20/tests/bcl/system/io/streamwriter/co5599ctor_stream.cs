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
public class Co5599ctor_stream
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamWriter(Stream)";
	public static String s_strTFName        = "Co5599ctor_stream.cs";
	public static String s_strTFAbbrev      = "Co5599";
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
			StreamWriter sw2;
			StreamReader sr2;
			String str2;
			MemoryStream memstr2;
			FileStream fs2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);
			strLoc = "Loc_98yv7";
			iCountTestcases++;
			try {
				sw2 = new StreamWriter((Stream)null);
				iCountErrors++;
				printerr( "Error_2yc83! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_287c7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_984yv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_9yf8c";
			memstr2 = new MemoryStream();
			sw2 = new StreamWriter(memstr2);
			sw2.Write("HelloWorld");
			sw2.Flush();
			sr2 = new StreamReader(memstr2);
			memstr2.Position = 0;
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("HelloWorld")) {
				iCountErrors++;
				printerr( "Error_298xh! Incorrect strnig on stream");
			}
			strLoc = "Loc_438c8";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new StreamWriter(fs2);
			sw2.Write("HelloWorld");
			sw2.Flush();
			fs2.Close();
			fs2 = new FileStream(filName, FileMode.Open);
			sr2 = new StreamReader(fs2);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("HelloWorld")) {
				iCountErrors++;
				printerr( "Error_38f8v! Incorrect string on stream");
			}
			fs2.Close();
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
		Co5599ctor_stream cbA = new Co5599ctor_stream();
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
