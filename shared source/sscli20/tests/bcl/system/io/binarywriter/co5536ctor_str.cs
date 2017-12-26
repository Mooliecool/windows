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
public class Co5536ctor_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BinaryWriter(Stream)";
	public static String s_strTFName        = "Co5536ctor_str.cs";
	public static String s_strTFAbbrev      = "Co5536";
	public static String s_strTFPath        = Environment.CurrentDirectory.ToString();
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		try
		{
			BinaryWriter dw2 = null;
			Stream fs2 = null;
			BinaryReader dr2 = null;
			FileInfo fil2 = null;
			strLoc = "Loc_98hg8";
			iCountTestcases++;
			try {
				dw2 = new BinaryWriter(null);
				iCountErrors++;
				printerr( "Error_1099x! Expected exception not thrown");
			} catch (ArgumentNullException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1908v! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_g87vj";
			fil2 = new FileInfo("TestFile.tmp");
			iCountTestcases++;
			try {
			fs2 = fil2.Open(FileMode.Create, FileAccess.Read);
				dw2 = new BinaryWriter(fs2);
				iCountErrors++;
				printerr( "Error_1y8g7! Expected exception not thrown");
				fs2.Close();
			} catch (ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_198yx! Incorrect Exception thrown, exc=="+exc.ToString());
			} 
			new FileInfo("TestFile.tmp").Delete();
			strLoc = "Loc_785hv";
			fil2 = new FileInfo("TestFile.tmp");
			fs2 = fil2.Open(FileMode.Create, FileAccess.Write);
			dw2 = new BinaryWriter(fs2);
			dw2.Write(true);
			dw2.Flush();
			fs2.Close();
			strLoc = "Loc_987hg";
			fs2 = fil2.Open(FileMode.Open);
			dr2 = new BinaryReader(fs2);
			iCountTestcases++;
			if(!dr2.ReadBoolean()) {
				iCountErrors++;
				printerr("Error_287gy! Correct value not written");
			}
			fs2.Close();
			fil2.Delete();
			strLoc = "Loc_98yvh";
			MemoryStream mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write(true);
			dw2.Flush();
			mstr.Position = 0;
			dr2 = new BinaryReader(mstr);
			iCountTestcases++;
			if(!dr2.ReadBoolean()) {
				iCountErrors++;
				printerr( "Error_298yx! Incorrect value on stream");
			}
			dr2.Close();
			strLoc = "Loc_09u95";
			mstr = new MemoryStream();
			mstr.Close();
			iCountTestcases++;
			try {
				dw2 = new BinaryWriter(mstr);
				iCountErrors++;
				printerr( "Error_98t4y! Expected exception not thrown");
			} catch ( ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_092xc! Incorrect exception thrown, exc=="+exc.ToString());
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
		Co5536ctor_str cbA = new Co5536ctor_str();
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
