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
public class Co5557Write_ch
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StringWriter.Write(Char)";
	public static String s_strTFName        = "Co5557Write_ch.cs";
	public static String s_strTFAbbrev      = "Co5557";
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strValue = String.Empty;
		Char[] chArr = new Char[]{
			Char.MinValue
			,Char.MaxValue
			,'\t'
			,' '
			,'$'
			,'@'
			,'#'
			,'\0'
			,'\v'
			,'\''
			,'\u3190'
			,'\uC3A0'
			,'A'
			,'5'
			,'\uFE70' 
			,'-'
			,';'
			,'\u00E6'
			,'\n'
			,'\v'
		};
		try {
			StringBuilder sb = new StringBuilder(40);
			StringWriter sw = new StringWriter(sb);
			StringReader sr;
			for(int i = 0 ; i < chArr.Length ; i++)
				sw.Write(chArr[i]);
			sr = new StringReader(sw.GetStringBuilder().ToString());
			Int32 tmp = 0;
			for(int i = 0 ; i < chArr.Length ; i++) {
				iCountTestcases++;
				if((tmp = sr.Read()) != (Int32)chArr[i]) {
					iCountErrors++;
					printerr( "Error_298vc_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
				}
			}
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_298yg! Unexpected exception thrown, exc=="+exc.ToString());
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
		Co5557Write_ch cbA = new Co5557Write_ch();
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
