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
public class Co5594ctor_str_b
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamWriter.GetBaseStream";
	public static String s_strTFName        = "Co5594ctor_str_b.cs";
	public static String s_strTFAbbrev      = "Co5594";
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
        String strTemp = "" ;
		try
		{
			StreamWriter sw2;
			StreamReader sr2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);
			strLoc = "Loc_98yv7";
			iCountTestcases++;
			try {
				sw2 = new StreamWriter(null, false);
				iCountErrors++;
				printerr( "Error_2yc83! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_287c7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_984yv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_487bh";
			sw2 = new StreamWriter(filName, false);
            strTemp = "123456" ;
			sw2.Write(strTemp);
			sw2.Flush();
			iCountTestcases++;
			if(sw2.BaseStream.Length != strTemp.Length) {
				iCountErrors++;
				printerr( "Error_298yx! Incorrect stream length=="+sw2.BaseStream.Length);
			}
			sw2.Close();
			sw2 = new StreamWriter(filName, false);
            strTemp = "1234" ;
			sw2.Write(strTemp);
			sw2.Flush();
			iCountTestcases++;
			if(sw2.BaseStream.Length != strTemp.Length) {
				iCountErrors++;
				printerr( "Error_298yv! Incorrect stream length=="+sw2.BaseStream.Length);
			}
			sw2.Close();
			sw2 = new StreamWriter(filName, true);
            strTemp = "ABCDE" ;
			sw2.Write(strTemp);
			sw2.Flush();
			iCountTestcases++;
			if(sw2.BaseStream.Length != (strTemp.Length + "ABCD".Length) ) { 
				iCountErrors++;
				printerr( "Error_2908c! Incorrect stream length=="+sw2.BaseStream.Length);
			}
			sw2.Close();
            sr2 = new StreamReader(filName);
			String tmp;
			tmp = sr2.ReadToEnd();
			sr2.Close();
			if(!tmp.Equals("1234ABCDE")) {
				iCountErrors++;
				printerr( "Error_98ygy! Incorrect string=="+tmp);
			}
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
		Co5594ctor_str_b cbA = new Co5594ctor_str_b();
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
