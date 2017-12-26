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
public class Co3741Close
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BinaryWriter.Close()";
	public static String s_strTFName        = "Co3741Close.cs";
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
			BinaryWriter sw2;
			FileStream fs2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);			
			strLoc = "Loc_9577b";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new BinaryWriter(fs2);
			sw2.Write("Dette er sjef");
			sw2.Close();
			strLoc = "Loc_49t8e";
			fs2 = new FileStream(filName, FileMode.Open);
			sw2 = new BinaryWriter(fs2);
			sw2.Close();
			sw2.Close();
			sw2.Close();
			iCountTestcases++;
			try {
				sw2.Seek(1, SeekOrigin.Begin);
				iCountErrors++;
				printerr( "Error_687yv! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_y67y9! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_9y7g7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write(new Byte[2], 0, 2);
				iCountErrors++;
				printerr( "Error_9177g! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_80093! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_7100b! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write(new Char[2], 0, 2);
				iCountErrors++;
				printerr( "Error_2398j! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_t877b! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_787yb! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write(true);
				iCountErrors++;
				printerr( "Error_10408! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_1098t! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_178t5! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write((Byte)4);
				iCountErrors++;
				printerr( "Error_17985! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_0984s! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_019g8! Incorrect exception thrown, excc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write(new Byte[]{1,2});
				iCountErrors++;
				printerr( "Error_10989! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_01099! Caught expectede exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_16794! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			iCountTestcases++;
			try {
				sw2.Write('a');
				iCountErrors++;
				printerr( "ERror_1980f! Expected Exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_1099g! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1098g! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			iCountTestcases++;
			try {
				sw2.Write(new Char[]{'a','b'});
				iCountErrors++;
				printerr( "Error_19876! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_0198c! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1869g! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write(5.3);
				iCountErrors++;
				printerr( "Error_1089g! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_t5999! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1900v! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			iCountTestcases++;
			try {
				sw2.Write((Int16)3);
				iCountErrors++;
				printerr( "Error_92889! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo("Info_768g8! Caught expected exception, exc=="+iexc.Message);
			} catch(Exception exc) {
				iCountErrors++;
				printerr( "Error_0160t! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write(33);
				iCountErrors++;
				printerr( "Error_109g7! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_0198v! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_10796! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			iCountTestcases++;
			try {
				sw2.Write((Int64)42);
				iCountErrors++;
				printerr( "Error_967gb! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_10860! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1067b! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			iCountTestcases++;
			try {
				sw2.Write((SByte)4);
				iCountErrors++;
				printerr( "Error_16908! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_1768b! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1769g! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write("Hello There");
				iCountErrors++;
				printerr( "Error_t5087! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_35987! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_t87bb! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write((Single)4.3);
				iCountErrors++;
				printerr( "Error_5698v! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_6987y! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298xa! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write((UInt16)3);
				iCountErrors++;
				printerr( "Error_58743! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_287cc! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_80954! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write((UInt32)4);
				iCountErrors++;
				printerr( "Error_58074! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_1509t! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298cc! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write((UInt64)5);
				iCountErrors++;
				printerr( "Error_78884! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_208cx! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_28908! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				sw2.Write("Bah");
				iCountErrors++;
				printerr( "Error_3498v! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo( "Info_298uv! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_3209v! Incorrect exception thrown, exc=="+exc.ToString());
			}
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
		Co3741Close cbA = new Co3741Close();
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
