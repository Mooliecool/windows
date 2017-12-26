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
public class Co5546Write_ui64
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BinaryWriter.Write(UInt64)";
	public static String s_strTFName        = "Co5546Write_ui64.cs";
	public static String s_strTFAbbrev      = "Co5546";
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
			MemoryStream mstr = null;
			UInt64 ui64a = 0;
			UInt64[] ui64Arr = new UInt64[0];
			int ii = 0;
			String filName = s_strTFAbbrev+"Test.tmp";
			ui64Arr = new UInt64[] {
				UInt64.MinValue
				,UInt64.MaxValue
				,0
				,100
				,1000
				,10000
				,UInt64.MaxValue-100
			};
			strLoc = "Loc_8yfv7";
			fil2 = new FileInfo(filName);
			fs2 = fil2.Open(FileMode.Create);
			dw2 = new BinaryWriter(fs2);
			try {
				for(ii = 0 ; ii < ui64Arr.Length ; ii++)
					dw2.Write(ui64Arr[ii]);		   
				dw2.Flush();
				fs2.Close();
				strLoc = "Loc_987hg";
				fs2 = fil2.Open(FileMode.Open);
				dr2 = new BinaryReader(fs2);
				for(ii = 0 ; ii < ui64Arr.Length ;ii++) {
					iCountTestcases++;
					if((ui64a = dr2.ReadUInt64()) != ui64Arr[ii]) {
						iCountErrors++;
						printerr( "Error_298hg_"+ii+"! Expected=="+ui64Arr[ii]+" , got=="+ui64a);
					}
				}
				iCountTestcases++;
				try {
					ui64a = dr2.ReadUInt64();
					iCountErrors++;
					printerr( "Error_2389! Expected exception not thrown, ui64a=="+ui64a);
				} catch (EndOfStreamException) {
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_3298h! Unexpected exception thrown, exc=="+exc.ToString());
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_278gy! Unexpected exception, exc=="+exc.ToString());
			}
			fs2.Close();
	  		fil2.Delete();
			strLoc = "Loc_98yss";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			try {
				for(ii = 0 ; ii < ui64Arr.Length ; ii++) 
					dw2.Write(ui64Arr[ii]);
				dw2.Flush();
				mstr.Position = 0;
				strLoc = "Loc_287y5";
				dr2 = new BinaryReader(mstr);
				for(ii = 0 ; ii < ui64Arr.Length ;ii++) {
					iCountTestcases++;
					if((ui64a = dr2.ReadUInt64()) != ui64Arr[ii]) {
						iCountErrors++;
						printerr( "Error_48yf4_"+ii+"! Expected=="+ui64Arr[ii]+" , got=="+ui64a);
					}
				}
				iCountTestcases++;
				try {
					ui64a = dr2.ReadUInt64();
					iCountErrors++;
					printerr( "Error_2d847! Expected exception not thrown, ui64a=="+ui64a);
				} catch (EndOfStreamException) {
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_238gy! Unexpected exception thrown, exc=="+exc.ToString());
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_38f85! Unexpected exception, exc=="+exc.ToString());
			}
			mstr.Close();
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
		Co5546Write_ui64 cbA = new Co5546Write_ui64();
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
