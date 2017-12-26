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
public class Co5571Read_chArr_ii
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StringReader.ReadLine()";
	public static String s_strTFName        = "Co5571Read_chArr_ii.cs";
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
			String chArrString = String.Empty;
			Int32 i32;
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
			for(int i = 0 ; i < chArr.Length ; i++)
				chArrString += chArr[i];
			Char[] cArrWrite;
			strLoc = "Loc_209xu";
			sr = new StringReader(String.Empty);
			iCountTestcases++;
			try {
				sr.Read(null, 0, 0);
				iCountErrors++;
				printerr( "Error_109ux! Expected exception not thrown");
			} catch (ArgumentNullException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0199x! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_09uv8";
			sr = new StringReader(String.Empty);
			cArrWrite = new Char[0];
			iCountTestcases++;
			try {
				sr.Read(cArrWrite, -1, 0);
				iCountErrors++;
				printerr( "Error_3498h! Expecerted exception not thornw");
			} catch (ArgumentOutOfRangeException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298yy! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_884y3";
			sr = new StringReader(String.Empty);
			cArrWrite = new Char[0];
			iCountTestcases++;
			try {
				sr.Read(cArrWrite, 0, -1);
				iCountErrors++;
				printerr( "Error_287yg! Expected exception not thrown");
			} catch (ArgumentOutOfRangeException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_89ty7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_8857v";
			sr = new StringReader(chArrString);
			cArrWrite = new Char[0];
			iCountTestcases++;
			try {
				sr.Read(cArrWrite, 0, 1);
				iCountErrors++;
				printerr( "Error_3989x! Expected exception not thrown");
			} catch (ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_18t7v! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_48y8v";
			sr = new StringReader(chArrString);
			cArrWrite = new Char[chArr.Length];
			i32 = sr.Read(cArrWrite, 0, chArr.Length);
			iCountTestcases++;
			if(i32 != chArr.Length) {
				iCountErrors++;
				printerr( "Error_398yx! Expected=="+chArr.Length+", i32=="+i32);
			}
			for(int i = 0 ; i < cArrWrite.Length ; i++) {
				iCountTestcases++;
				if(cArrWrite[i] != chArr[i]) {
					iCountErrors++;
					printerr( "Error_298vy_"+i+"! Expected=="+chArr[i]+", got=="+cArrWrite[i]);
				}
			}
			strLoc = "Loc_847yv";
			sr = new StringReader("procrastination is the art of keeping up with yesterday");
			chArr = new Char[]{'A', 'B', 'C', 'p', 'r', 'o', 'G', 'H', 'I', 'J'};
			cArrWrite = new Char[]{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
			i32 = sr.Read(cArrWrite, 3, 3);
			iCountTestcases++;
			if(i32 != 3) {
				iCountErrors++;
				printerr( "Error_289bb! Expected==3, i32=="+i32);
			} 
			for(int i = 0 ; i < cArrWrite.Length ; i++) {
				iCountTestcases++;
				if(cArrWrite[i] != chArr[i]) {
					iCountErrors++;
					printerr( "Error_298yg_"+i+"! Expected=="+chArr[i]+", got=="+cArrWrite[i]);
				}
			}
			strLoc = "Loc_498yg";
			sr = new StringReader(String.Empty);
			cArrWrite = new Char[0];
			i32 = sr.Read(cArrWrite, 0, 0);
			iCountTestcases++;
			if(i32 != 0) { 
				iCountErrors++;
				printerr( "Error_298yg! Expected==0, i32=="+i32);
			}
			strLoc = "Loc_4hvcy";
			sr = new StringReader("ab");
			sr.Read();
			cArrWrite = new Char[2];
			i32 = sr.Read(cArrWrite, 0, 2);
			iCountTestcases++;
			if(i32 != 1) {
				iCountErrors++;
				printerr( "Error_298yc! Incorrect return value, i32=="+i32);
			}
			iCountTestcases++;
			if(cArrWrite[0] != 'b') {
				iCountErrors++;
				printerr( "Error_98gcn! Incorrect value");
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
		Co5571Read_chArr_ii cbA = new Co5571Read_chArr_ii();
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
