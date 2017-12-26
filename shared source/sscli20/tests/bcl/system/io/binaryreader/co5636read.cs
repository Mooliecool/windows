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
using System.Threading;
public class Co5636Read
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BinaryReader.Read()";
	public static String s_strTFName        = "Co5636Read.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
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
			BinaryReader sr2;
			StreamWriter sw2;
			MemoryStream ms2;
			FileStream fs2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);			
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
				,'\n'
				,'\v'
				,'\r'
				,'5'
				,'T'
				,'-'
				,';'
				,'\u00E6'
			};
			strLoc = "Loc_2698b";
			try {
				ms2 = new MemoryStream();
				sw2 = new StreamWriter(ms2);
				for(int i = 0 ; i < chArr.Length ; i++)
					sw2.Write(chArr[i]);
				sw2.Flush();
				ms2.Position = 0;
				sr2 = new BinaryReader(ms2);
				Int32 tmp = 0;
				for(int i = 0 ; i < chArr.Length ; i++) {
					iCountTestcases++;
					if((tmp = sr2.Read()) != (Int32)chArr[i]) {
						iCountErrors++;
						printerr( "Error_298vc_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
					}
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298yg! Unexpected exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_9848v";
			try {
				fs2 = new FileStream(filName, FileMode.Create);
				sw2 = new StreamWriter(fs2);
				for(int i = 0 ; i < chArr.Length ; i++)
					sw2.Write(chArr[i]);
				sw2.Flush();
				sw2.Close();
				fs2 = new FileStream(filName, FileMode.Open);
				sr2 = new BinaryReader(fs2);
				Int32 tmp = 0;
				for(int i = 0 ; i < chArr.Length ; i++) {
					iCountTestcases++;
					if((tmp = sr2.Read()) != (Int32)chArr[i]) {
						iCountErrors++;
						printerr( "Error_98yv8!_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
					}
				}
				sr2.Close();
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_698y7! Unexpected exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_958hb";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new StreamWriter(fs2, Encoding.UTF8, 4);
			sw2.Write("\u00FF\u00FF");
			sw2.Close();
			fs2 = new FileStream(filName, FileMode.Open);
			sr2 = new BinaryReader(fs2);
			iCountTestcases++;
            sr2.Read();  
			if(sr2.Read() != 0xFF) {
				iCountErrors++;
				printerr( "Error_t8yc! Incorrect character read");
			}
			if(sr2.Read() != 0xFF) {
				iCountErrors++;
				printerr("Error_8yb78! Incorrect character read");
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
		Co5636Read cbA = new Co5636Read();
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
