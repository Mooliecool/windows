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
public class Co5572Close
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StringReader.Close()";
	public static String s_strTFName        = "Co5572Close.cs";
	public static String s_strTFAbbrev      = "Co5572";
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
			sr = new StringReader("HelloWorld");
			sr.Close();
            strLoc = "Loc_43423";
			iCountTestcases++;
            try{
    			int iNumCount = sr.Read() ;
                iCountErrors++;
                printerr( "Error_8984c! Expected exception not thrown");
            }catch(ObjectDisposedException e){
                printinfo("Expected exception occured" + e.Message);
            }catch(Exception e ){
                iCountErrors++ ;
                printerr("Error_5895! Unexpected exception occured" + e.ToString());
            }
            strLoc = "Loc_44324";
			iCountTestcases++;
            try{
    			int iNumCount = sr.Peek() ;
                iCountErrors++;
                printerr( "Error_5454! Expected exception not thrown");
            }catch(ObjectDisposedException e){
                printinfo("Expected exception occured" + e.Message);
            }catch(Exception e ){
                iCountErrors++ ;
                printerr("Error_9789! Unexpected exception occured" + e.ToString());
            }
            strLoc = "Loc_45345";
			iCountTestcases++;
            try{
    			String strTemp = sr.ReadLine();
                iCountErrors++;
                printerr( "Error_0988! Expected exception not thrown");
            }catch(ObjectDisposedException e){
                printinfo("Expected exception occured" + e.Message);
            }catch(Exception e ){
                iCountErrors++ ;
                printerr("Error_2158! Unexpected exception occured" + e.ToString());
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
		Co5572Close cbA = new Co5572Close();
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
