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
public class Co5598ctor_str_b_env_i
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamWriter(String, boolean, encoding)";
	public static String s_strTFName        = "Co5598ctor_str_b_env_i.cs";
	public static String s_strTFAbbrev      = "Co5598";
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
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);
			strLoc = "Loc_98yv7";
			iCountTestcases++;
			try {
				sw2 = new StreamWriter(null, false, Encoding.UTF8, 1);
				iCountErrors++;
				printerr( "Error_2yc83! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_287c7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_984yv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98g77";
			iCountTestcases++;
			try {
				sw2 = new StreamWriter(filName, false, null, 1);
				iCountErrors++;
				printerr( "Error_t8786! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_2989x! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_27gy7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_299xj";
			iCountTestcases++;
			try {
				sw2 = new StreamWriter(filName, false, Encoding.UTF8, -2);
				iCountErrors++;
				printerr( "Error_1908x! Expected exception not thrown");
			} catch (ArgumentOutOfRangeException aexc) {
				printinfo( "Info_199xu! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_56y8v! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98y8x";
			sw2 = new StreamWriter(filName, false, Encoding.ASCII, 9);
			sw2.Write("HelloThere\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.ASCII);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("HelloThere?")) {
				iCountErrors++;
				printerr( "Error_298xw! Incorrect String interpreted");
			}
			sr2.Close();
			strLoc = "Loc_4747u";
			sw2 = new StreamWriter(filName, false, Encoding.UTF8, 8888);
			sw2.Write("This is UTF8\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.UTF8);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF8\u00FF")) {
				iCountErrors++;
				printerr( "Error_1y8xx! Incorrect string on stream");
			}
			sr2.Close();
			strLoc = "Loc_28y7c";
			sw2 = new StreamWriter(filName, false, Encoding.UTF7, 11);
			sw2.Write("This is UTF7\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.UTF7);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF7\u00FF")) {
				iCountErrors++;
				printerr( "Error_2091x! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_98hcf";
			sw2 = new StreamWriter(filName, false, Encoding.BigEndianUnicode, 20);
			sw2.Write("This is BigEndianUnicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.BigEndianUnicode);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is BigEndianUnicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_2g88t! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_48y8d";
			sw2 = new StreamWriter(filName, false, Encoding.Unicode, 50000);
			sw2.Write("This is Unicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.Unicode);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is Unicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_2g88t! Incorrect string on stream=="+str2);
			}
			sr2.Close();			
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
		Co5598ctor_str_b_env_i cbA = new Co5598ctor_str_b_env_i();
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
