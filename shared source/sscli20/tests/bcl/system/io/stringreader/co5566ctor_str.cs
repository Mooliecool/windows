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
public class Co5566ctor_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StringReader(String)";
	public static String s_strTFName        = "Co5566ctor_str.cs";
	public static String s_strTFAbbrev      = "Co5566";
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
			StringReader sr;
			strLoc = "Loc_98yg7";
			iCountTestcases++;
			try {		
                sr = new StringReader(null);
                String strTemp = sr.ReadToEnd();
				iCountErrors++;
				printerr( "Error_18syx! StringReader should only hold null");
				sr.ReadLine();
				sr.Peek();
				sr.Read();
				sr.Close();
			} catch (ArgumentNullException exc) {
				printinfo("Expected exception thrown, exc=="+exc.Message);
			}catch (Exception exc) {
				iCountErrors++;
				printerr("Error_109xu! Unexpected exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_4790s";
			sr = new StringReader(String.Empty);
			iCountTestcases++;
			if(!sr.ReadToEnd().Equals(String.Empty)) {
				iCountErrors++;
				printerr( "Error_099xa! Incorrect construction");
			}		   		 
			strLoc = "Loc_8388x";
			sr = new StringReader("Hello\0World");
			iCountTestcases++;
			if(!sr.ReadToEnd().Equals("Hello\0World")) {
				iCountErrors++;
				printerr( "Error_1099f! Incorrect construction");
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
		Co5566ctor_str cbA = new Co5566ctor_str();
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
