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
public class Co5638get_BaseStream
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BinaryReader.BaseStream";
	public static String s_strTFName        = "Co5638get_BaseStream.cs";
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
			BinaryWriter sr2;
		    MemoryStream ms2;
			FileStream fs2;
			BufferedStream bs2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);			
			strLoc = "Loc_98hg7";
			ms2 = new MemoryStream();
			sr2 = new BinaryWriter(ms2);
			iCountTestcases++;
			if(sr2.BaseStream != ms2) {
				iCountErrors++;
				printerr( "Error_g57hb! Incorrect basestream");
			}
			sr2.Close();
			ms2 = new MemoryStream();
			bs2 = new BufferedStream(ms2);
			sr2 = new BinaryWriter(bs2);
			iCountTestcases++;
			if(sr2.BaseStream != bs2) {
				iCountErrors++;
				printerr( "Error_988hu! Incorrect basestream");
			}
			sr2.Close();
			fs2 = new FileStream(filName, FileMode.Create);
			sr2 = new BinaryWriter(fs2);
			iCountTestcases++;
			if(sr2.BaseStream != fs2) {
				iCountErrors++;
				printerr( "Error_29g87! Incorrect baseStream");
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
		Co5638get_BaseStream cbA = new Co5638get_BaseStream();
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
