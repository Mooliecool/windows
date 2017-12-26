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
public class Co5600ctor_stream_enc
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamWriter(Stream, encoding)";
	public static String s_strTFName        = "Co5600ctor_stream_enc.cs";
	public static String s_strTFAbbrev      = "Co5600";
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
			StreamWriter sw2;
			StreamReader sr2;
			String str2;
			if(File.Exists("Co5600Test.tmp"))
				File.Delete("Co5600Test.tmp");
			strLoc = "Loc_98yv7";
			iCountTestcases++;
			try {
				sw2 = new StreamWriter((Stream)null, Encoding.UTF8);
				iCountErrors++;
				printerr( "Error_2yc83! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_287c7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_984yv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_24098";
			iCountTestcases++;
			try {
				sw2 = new StreamWriter(new MemoryStream(), null);
				iCountErrors++;
				printerr( "Error_209x7! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_277v7! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298x7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98y8x";
			FileStream fs = new FileStream("Co5600Test.tmp", FileMode.Create);
			sw2 = new StreamWriter(fs, Encoding.ASCII);
			sw2.Write("HelloThere\u00FF");
			sw2.Close();
			sr2 = new StreamReader("Co5600Test.tmp", Encoding.ASCII);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("HelloThere?")) {
				iCountErrors++;
				printerr( "Error_298xw! Incorrect String interpreted");
			}
			sr2.Close();
			strLoc = "Loc_4747u";
			fs = new FileStream("Co5600Test.tmp", FileMode.Create);
			sw2 = new StreamWriter(fs, Encoding.UTF8);
			sw2.Write("This is UTF8\u00FF");
			sw2.Close();
			sr2 = new StreamReader("Co5600Test.tmp", Encoding.UTF8);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF8\u00FF")) {
				iCountErrors++;
				printerr( "Error_1y8xx! Incorrect string on stream");
			}
			sr2.Close();
			strLoc = "Loc_28y7c";
			fs = new FileStream("Co5600Test.tmp", FileMode.Create);
			sw2 = new StreamWriter(fs, Encoding.UTF7);
			sw2.Write("This is UTF7\u00FF");
			sw2.Close();
			sr2 = new StreamReader("Co5600Test.tmp", Encoding.UTF7);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF7\u00FF")) {
				iCountErrors++;
				printerr( "Error_2091x! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_98hcf";
			fs = new FileStream("Co5600Test.tmp", FileMode.Create);
			sw2 = new StreamWriter(fs, Encoding.BigEndianUnicode);
			sw2.Write("This is BigEndianUnicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader("Co5600Test.tmp", Encoding.BigEndianUnicode);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is BigEndianUnicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_2g88t! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_48y8d";
			fs = new FileStream("Co5600Test.tmp", FileMode.Create);
			sw2 = new StreamWriter(fs, Encoding.Unicode);
			sw2.Write("This is Unicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader("Co5600Test.tmp", Encoding.Unicode);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is Unicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_2g88t! Incorrect string on stream=="+str2);
			}
			sr2.Close();			
			if(File.Exists("Co5600Test.tmp"))
				File.Delete("Co5600Test.tmp");
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
		Co5600ctor_stream_enc cbA = new Co5600ctor_stream_enc();
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
