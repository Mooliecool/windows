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
public class Co5560ctor_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamWriter(String)";
	public static String s_strTFName        = "Co5560ctor_str.cs";
	public static String s_strTFAbbrev      = "Co5560";
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strValue = String.Empty;
		try {
			iCountTestcases++;
			StreamWriter sw = new StreamWriter((String)null);
			iCountErrors++;
			printerr( "Error_298vy! Expected exception not thrown");
		} catch (ArgumentNullException) {
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_2918x! Incorrect exceptiont thrown, exc=="+exc.ToString());
		}
		try {
			iCountTestcases++;
			StreamWriter sw = new StreamWriter("..");
			iCountErrors++;
			printerr( "Error_1908x! Expected exception not thrown");
		} catch (UnauthorizedAccessException ) {
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_18y77! Incorrect exception thrown, exc=="+exc.ToString());
		}
		try {
			StreamWriter sw = new StreamWriter("TestFile.tmp");
			sw.Write(5);
			sw.Flush();
			sw.Close();
			StreamReader sr = new StreamReader("TestFile.tmp");
			iCountTestcases++;
			Int32 tmp;
			if((tmp = sr.Read())  != (Int32)'5') {
				iCountErrors++;
				printerr( "Error_298yx! Error_4y7gg! Incorrect value read, tmp=="+tmp);
			} 
			sr.Close();
			new FileInfo("TestFile.tmp").Delete();
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_019x7! Unexpected exception thrown, exc=="+exc.ToString());
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
		Co5560ctor_str cbA = new Co5560ctor_str();
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
