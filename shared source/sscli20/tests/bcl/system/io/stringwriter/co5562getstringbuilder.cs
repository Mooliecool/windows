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
public class Co5562GetStringBuilder
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StringWriter.ToString()";
	public static String s_strTFName        = "Co5562GetStringBuilder.cs";
	public static String s_strTFAbbrev      = "Co5561";
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
		};
		try {
			StringBuilder sb = new StringBuilder();
			StringWriter sw = new StringWriter(sb);
			for(int i = 0 ; i < chArr.Length ; i++)
				sb.Append(chArr[i]);
			sw.Write(sb.ToString());
			iCountTestcases++;
			if(!sw.GetStringBuilder().ToString().Equals(sb.ToString())) {
				iCountErrors++;
				printerr( "Error_298vc Expected=="+sb.ToString()+", got=="+sw.ToString());
			}
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_298yg! Unexpected exception thrown, exc=="+exc.ToString());
		}
		try {
			StringWriter sw = new StringWriter();
			iCountTestcases++;
			if(!sw.GetStringBuilder().ToString().Equals(String.Empty)) {
				iCountErrors++;
				printerr( "Error_3988h! Incorrect Strign in StringWriter");
			} 
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_198wh! Unexpected exception thrown, exc=="+exc.ToString());
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
		Co5562GetStringBuilder cbA = new Co5562GetStringBuilder();
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
