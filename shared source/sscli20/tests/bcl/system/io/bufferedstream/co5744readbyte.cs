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
public class Co5744ReadByte
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BufferedStream.ReadByte";
	public static String s_strTFName        = "Co5744ReadByte.cs";
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
			FileStream fs2;
			MemoryStream ms2;
			BufferedStream bs2;
			String filName = s_strTFAbbrev + "TestFile.tmp";
			Int32 ii = 0;
			Byte[] bytArr;
			Int32 i32;
			bytArr = new Byte[] {
				Byte.MinValue
				,Byte.MaxValue
				,100
				,Byte.MaxValue-100
			};
			if(File.Exists(filName))
				File.Delete(filName);
			strLoc = "Loc_8yfv7";
			fs2 = new FileStream(filName, FileMode.Create);
			bs2 = new BufferedStream(fs2);
			for(ii = 0 ; ii < bytArr.Length ; ii++)
				bs2.WriteByte(bytArr[ii]);		   
			bs2.Flush();
			bs2.Close();
			strLoc = "Loc_987hg";
			fs2 = new FileStream(filName, FileMode.Open);
			bs2 = new BufferedStream(fs2);
			for(ii = 0 ; ii < bytArr.Length ;ii++) {
				iCountTestcases++;
				if((i32 = bs2.ReadByte()) != bytArr[ii]) {
					iCountErrors++;
					printerr( "Error_298hg_"+ii+"! Expected=="+bytArr[ii]+" , got=="+i32);
				}
			}
			i32 = bs2.ReadByte();
			if(i32 != -1) {
				iCountErrors++;
				printerr( "Error_2389! -1 return expected, i32=="+i32);
			} 
			fs2.Close();
			strLoc = "Loc_398yc";
			ms2 = new MemoryStream();
			bs2 = new BufferedStream(ms2);
			for(ii = 0 ; ii < bytArr.Length ; ii++)
				bs2.WriteByte(bytArr[ii]);
			bs2.Flush();
			bs2.Position = 0;
			bs2.Flush();
			for(ii = 0 ; ii < bytArr.Length ; ii++) {
				iCountTestcases++;
				if((i32 = bs2.ReadByte()) != bytArr[ii]) {
					iCountErrors++;
					printerr( "Error_38yv8_"+ii+"! Expected=="+bytArr[ii]+", got=="+i32);
				}
			}
			i32 = bs2.ReadByte();
			if(i32 != -1) {
				iCountErrors++;
				printerr( "Error_238v8! -1 return expected, i32=="+i32);
			}
			bs2.Position = 0;
			for(ii = 0 ; ii < bytArr.Length; ii++)
				bs2.WriteByte(bytArr[ii]);
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
		Co5744ReadByte cbA = new Co5744ReadByte();
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
