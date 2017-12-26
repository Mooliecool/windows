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
public class Co9305ctor_str_enc_b
{
	public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader(String, Encoding, boolean)";
	public static String s_strTFName        = "Co9305ctor_str_enc_b.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		try
		{
			StreamReader sr2;
			StreamWriter sw2;
			String str2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);			
			strLoc = "Loc_98yv7";
			sw2 = new StreamWriter(filName);
			sw2.Close();
			iCountTestcases++;
			try {
				sr2 = new StreamReader((String)null, Encoding.UTF8, false);
				iCountErrors++;
				printerr( "Error_2yc83! Expected exception not thrown");
				sr2.Close();
			} catch (ArgumentNullException aexc) {
				printinfo("Info_287c7! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_984yv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98g77";
			iCountTestcases++;
			try {
				sr2 = new StreamReader(filName, null, false);
				iCountErrors++;
				printerr( "Error_t8786! Expected exception not thrown");
				sr2.Close();
			} catch (ArgumentNullException aexc) {
				printinfo("Info_2989x! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_27gy7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98y8x";
			sw2 = new StreamWriter(filName, false, Encoding.ASCII, 11);
			sw2.Write("HelloThere\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.ASCII, false);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("HelloThere?")) {
				iCountErrors++;
				printerr( "Error_o973! Incorrect String interpreted");
			}
			sr2.Close();
			strLoc = "Loc_98y8x";
			sw2 = new StreamWriter(filName, false, Encoding.ASCII, 11);
			sw2.Write("HelloThere\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.ASCII, true);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("HelloThere?")) {
				iCountErrors++;
				printerr( "Error_4534! Incorrect String interpreted");
			}
			sr2.Close();
			strLoc = "Loc_4747u";
			sw2 = new StreamWriter(filName, false, Encoding.UTF8, 20);
			sw2.Write("This is UTF8\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.UTF8, false);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF8\u00FF")) {
				iCountErrors++;
				printerr( "Error_1y8xx! Incorrect string on stream");
			}
			sr2.Close();
			strLoc = "Loc_4747u";
			sw2 = new StreamWriter(filName, false, Encoding.UTF8);
			sw2.Write("This is UTF8\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.UTF8, true);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF8\u00FF")) {
				iCountErrors++;
				printerr( "Error_4358! Incorrect string on stream");
			}
			sr2.Close();
			strLoc = "Loc_28y7c";
			sw2 = new StreamWriter(filName, false, Encoding.UTF7);
			sw2.Write("This is UTF7\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.UTF7, false);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF7\u00FF")) {
				iCountErrors++;
				printerr( "Error_2091x! Incorrect string on stream=="+str2);
			}
			sr2.Close();    
            strLoc = "Loc_28y7c";
			sw2 = new StreamWriter(filName, false, Encoding.UTF7);
			sw2.Write("This is UTF7\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.UTF7, true);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is UTF7\u00FF")) {
				iCountErrors++;
				printerr( "Error_2091x! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_98hcf";
			sw2 = new StreamWriter(filName, false, Encoding.BigEndianUnicode);
			sw2.Write("This is BigEndianUnicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.BigEndianUnicode, false);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is BigEndianUnicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_2g88t! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_98hcf";
			sw2 = new StreamWriter(filName, false, Encoding.BigEndianUnicode);
			sw2.Write("This is BigEndianUnicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.BigEndianUnicode, true);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is BigEndianUnicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_5545! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_48y8d";
			sw2 = new StreamWriter(filName, false, Encoding.Unicode);
			sw2.Write("This is Unicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.Unicode, false);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is Unicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_5345! Incorrect string on stream=="+str2);
			}
			sr2.Close();  
			strLoc = "Loc_48y8d";
			sw2 = new StreamWriter(filName, false, Encoding.Unicode);
			sw2.Write("This is Unicode\u00FF");
			sw2.Close();
			sr2 = new StreamReader(filName, Encoding.Unicode, true);
			str2 = sr2.ReadToEnd();
			iCountTestcases++;
			if(!str2.Equals("This is Unicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_5453! Incorrect string on stream=="+str2);
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
			Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
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
		Co9305ctor_str_enc_b cbA = new Co9305ctor_str_enc_b();
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
