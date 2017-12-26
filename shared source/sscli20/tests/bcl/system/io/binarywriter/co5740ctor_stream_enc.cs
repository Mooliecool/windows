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
public class Co3740ctor_stream_enc
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamWriter(Stream, Encoding)";
	public static String s_strTFName        = "Co3740ctor_stream_enc.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
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
			MemoryStream ms2;
			FileStream fs2;
			BinaryReader sr2;
			BinaryWriter sw2;
			String str2;
			String filName = s_strTFAbbrev+"Test.tmp";
			if(File.Exists(filName))
				File.Delete(filName);			
			strLoc = "Loc_98yv7";
			iCountTestcases++;
			try {
				sr2 = new BinaryReader((Stream)null, Encoding.UTF8);
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
				sr2 = new BinaryReader(new MemoryStream(), null);
				iCountErrors++;
				printerr( "Error_209x7! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_277v7! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298x7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98y8x";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new BinaryWriter(fs2, Encoding.ASCII);
			sw2.Write("HelloThere\u00FF");
			sw2.Close();
			fs2 = new FileStream(filName, FileMode.Open);
			sr2 = new BinaryReader(fs2, Encoding.ASCII);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("HelloThere?")) {
				iCountErrors++;
				printerr( "Error_298xw! Incorrect String interpreted" + str2);
			}
			sr2.Close();
			strLoc = "Loc_443432";
			ms2 = new MemoryStream();
			sw2 = new BinaryWriter(ms2, Encoding.ASCII);
			sw2.Write("HelloThere\u00FF");
			sw2.Flush();
			ms2.Position = 0;
			sr2 = new BinaryReader(ms2, Encoding.ASCII);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("HelloThere?")) {
				iCountErrors++;
				printerr( "Error_298xw! Incorrect String interpreted");
			}
			sr2.Close();
			strLoc = "Loc_4747u";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new BinaryWriter(fs2, Encoding.UTF8);
			sw2.Write("This is UTF8\u00FF");
			sw2.Close();
			fs2 = new FileStream(filName, FileMode.Open);
			sr2 = new BinaryReader(fs2, Encoding.UTF8);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is UTF8\u00FF")) {
				iCountErrors++;
				printerr( "Error_1y8xx! Incorrect string on stream");
			}
			sr2.Close();
			strLoc = "Loc_878678";
			ms2 = new MemoryStream();
			sw2 = new BinaryWriter(ms2, Encoding.UTF8);
			sw2.Write("This is UTF8\u00FF");
			sw2.Flush();
			ms2.Position = 0;
			sr2 = new BinaryReader(ms2, Encoding.UTF8);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is UTF8\u00FF")) {
				iCountErrors++;
				printerr( "Error_1y8xx! Incorrect string on stream");
			}
			sr2.Close();
			strLoc = "Loc_28y7c";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new BinaryWriter(fs2, Encoding.UTF7);
			sw2.Write("This is UTF7\u00FF");
			sw2.Flush();
			sw2.Close();
			fs2 = new FileStream(filName, FileMode.Open);
			sr2 = new BinaryReader(fs2, Encoding.UTF7);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is UTF7\u00FF")) {
				iCountErrors++;
				printerr( "Error_2091x! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_122334";
			ms2 = new MemoryStream();
			sw2 = new BinaryWriter(ms2, Encoding.UTF7);
			sw2.Write("This is UTF7\u00FF");
			sw2.Flush();
			ms2.Position = 0;
			sr2 = new BinaryReader(ms2, Encoding.UTF7);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is UTF7\u00FF")) {
				iCountErrors++;
				printerr( "Error_5f98y! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_98hcf";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new BinaryWriter(fs2, Encoding.BigEndianUnicode);
			sw2.Write("This is BigEndianUnicode\u00FF");
			sw2.Close();
			fs2 = new FileStream(filName, FileMode.Open);
			sr2 = new BinaryReader(fs2, Encoding.BigEndianUnicode);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is BigEndianUnicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_2g88t! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_32323";
			ms2 = new MemoryStream();
			sw2 = new BinaryWriter(ms2, Encoding.BigEndianUnicode);
			sw2.Write("This is BigEndianUnicode\u00FF");
			sw2.Flush();
			ms2.Position = 0;
			sr2 = new BinaryReader(ms2, Encoding.BigEndianUnicode);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is BigEndianUnicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_8f7yv! Incorrect string on stream=="+str2);
			}
			sr2.Close();
			strLoc = "Loc_48y8d";
			fs2 = new FileStream(filName, FileMode.Create);
			sw2 = new BinaryWriter(fs2, Encoding.Unicode);
			sw2.Write("This is Unicode\u00FF");
			sw2.Close();
			fs2 = new FileStream(filName, FileMode.Open);
			sr2 = new BinaryReader(fs2, Encoding.Unicode);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is Unicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_2g88t! Incorrect string on stream=="+str2);
			}
			sr2.Close();			
			strLoc = "Loc_12232";
			ms2 = new MemoryStream();
			sw2 = new BinaryWriter(ms2, Encoding.Unicode);
			sw2.Write("This is Unicode\u00FF");
			sw2.Flush();
			ms2.Position = 0;
			sr2 = new BinaryReader(ms2, Encoding.Unicode);
			str2 = sr2.ReadString();
			iCountTestcases++;
			if(!str2.Equals("This is Unicode\u00FF")) {
				iCountErrors++;
				printerr( "Error_f897h! Incorrect string on stream=="+str2);
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
		Co3740ctor_stream_enc cbA = new Co3740ctor_stream_enc();
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
