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
public class Co5570ReadLine
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StringReader.ReadLine()";
	public static String s_strTFName        = "Co5570ReadLine.cs";
	public static String s_strTFAbbrev      = "Co5570";
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
			String str1;
			strLoc = "Loc_98yg7";
            try{
    			sr = new StringReader(null);
    			iCountTestcases++;
    			if(sr.ReadLine() != null) {
    				iCountErrors++;
    				printerr( "Error_198yz! Incorrect value returned");
    			}
            } catch (ArgumentNullException e){
                printinfo("Error_5783!!! Expected exception occured..." + e.Message );
            } catch (Exception e){
                iCountErrors++ ;
                printinfo("Error_5783!!! UnExpected exception occured..." + e.ToString() );
            }
			strLoc = "Loc_4790s";
			sr = new StringReader(String.Empty);
			iCountTestcases++;
			if(sr.ReadLine() != null) {
				iCountErrors++;
				printerr( "Error_099xa! Incorrect value returned");
			} 
			strLoc = "Loc_8388x";
			str1 = "Hello\0\t\r\v   \\ World";
			sr = new StringReader(str1);
			iCountTestcases++;
			str1 = sr.ReadLine();
			if(!str1.Equals("Hello\0\t")) {
				iCountErrors++;
				printerr( "Error_198x9! Incorrect String read, str1=="+str1);
			} 
			strLoc = "Loc_2908x";
			str1 = "procrastination is the art of keeping up with yesterday";
			sr = new StringReader(str1);
			iCountTestcases++;
			if(!sr.ReadLine().Equals(str1)) {
				iCountErrors++;
				printerr( "Error_783yv! Incorrect string returned");
			}
			iCountTestcases++;
			if(sr.ReadLine() != null) {
				iCountErrors++;
				printerr( "Error_1908e! Null value expected");
			}
			strLoc = "Loc_398yc";
			str1 = "procrastination\n is the\r art of keeping up with yesterday";
			sr = new StringReader(str1);
			iCountTestcases++;
			str1 = sr.ReadLine();
			if(!str1.Equals("procrastination")) {
				iCountErrors++;
				printerr( "Error_3988f! Inocrrect string read, str1=="+str1);
			}
			iCountTestcases++;
			str1 = sr.ReadLine();
			if(!str1.Equals(" is the")) {
				iCountErrors++;
				printerr( "Error_988xa Incorrect string read, str1=="+str1);
			}
			iCountTestcases++;
			str1 = sr.ReadLine();
			if(!str1.Equals(" art of keeping up with yesterday")) {
				iCountErrors++;
				printerr( "Error_899sa! Incorrect string read, str1=="+str1);
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
		Co5570ReadLine cbA = new Co5570ReadLine();
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
