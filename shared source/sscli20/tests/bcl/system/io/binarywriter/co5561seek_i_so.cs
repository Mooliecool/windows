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
public class Co5561Seek_i_so
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BinaryWriter.Seek(Int32, SeekOrigin)";
	public static String s_strTFName        = "Co5561Seek_i_so.cs";
	public static String s_strTFAbbrev      = "Co5561";
	public static String s_strTFPath        = Environment.CurrentDirectory.ToString();
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
        int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue, Int16.MinValue};
        int[] iArrLargeValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/200 , Int32.MaxValue/1000, Int16.MaxValue, Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
		try
		{		
			BinaryWriter dw2 = null;
			MemoryStream mstr = null;
			Byte[] bArr = null;
			StringBuilder sb = new StringBuilder();
			Int64 lReturn = 0;
			strLoc = "Loc_278yg";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("Hello, this is my string".ToCharArray());
			iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
    			try {
    				lReturn = dw2.Seek(iArrInvalidValues[iLoop], SeekOrigin.Begin);
    				iCountErrors++;
    				printerr( "Error_17dy7! Expected exception not thrown");
    			} catch (IOException ) {
    			} catch (Exception exc) {
    				iCountErrors++;
    				printerr( "Error_918yc! Incorrect exception thrown, exc=="+exc.ToString());
    			}
    			if(lReturn != 0) {
    				iCountErrors++;
    				printerr( "Error_2t8gy! Incorrect return value, lReturn=="+lReturn);
    			}
            }
            dw2.Close(); 
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("Hello, this is my string".ToCharArray());
			iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
    			try {
    				lReturn = dw2.Seek(iArrLargeValues[iLoop], SeekOrigin.Begin);
                    if(lReturn != iArrLargeValues[iLoop]) {
                        iCountErrors++;
                        printerr( "Error_43434! Incorrect return value, lReturn=="+lReturn);
                    }
    			} catch (Exception exc) {
    				iCountErrors++;
    				printerr( "Error_9880! Incorrect exception thrown, exc=="+exc.ToString());
    			}
            }
            dw2.Close(); 
			strLoc = "Loc_093uc";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("012345789".ToCharArray());
			iCountTestcases++;
			try {
				lReturn = dw2.Seek(3, ~SeekOrigin.Begin);
				iCountErrors++;
				printerr( "Error_109ux! Expected exception not thrown");
			} catch (ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_190x9! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dw2.Close();
			strLoc = "Loc_98yct";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("0123456789".ToCharArray());
			lReturn = dw2.Seek(0, SeekOrigin.Begin);
			iCountTestcases++;
			if(lReturn != 0) { 
				iCountErrors++;
				printerr( "Error_9t8vb! Incorrect return value, lReturn=="+lReturn);
			} 
			dw2.Write("lki".ToCharArray());
			dw2.Flush();
			bArr = mstr.ToArray();
			sb = new StringBuilder();
			for(int i = 0 ; i < bArr.Length ; i++)
				sb.Append((Char)bArr[i]);
			iCountTestcases++;
			if(!sb.ToString().Equals("lki3456789")) {
				iCountErrors++;
				printerr( "Error_837sy! Incorrect sequence in stream, sb=="+sb.ToString());
			}
			dw2.Close();
			strLoc = "Loc_29hxs";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("0123456789".ToCharArray());
			lReturn = dw2.Seek(3, SeekOrigin.Begin);
			iCountTestcases++;
			if(lReturn != 3) {
				iCountErrors++;
				printerr( "Error_209gu! Incorrect return value, lReturn=="+lReturn);
			}
			dw2.Write("lk".ToCharArray());
			dw2.Flush();
			bArr = mstr.ToArray();
			sb = new StringBuilder();
			for(int i = 0 ; i < bArr.Length ; i++)
				sb.Append((Char)bArr[i]);
			iCountTestcases++;
			if(!sb.ToString().Equals("012lk56789")) {
				iCountErrors++;
				printerr( "Error_87vhe! Incorrect sequence in stream, sb=="+sb.ToString());
			}
			dw2.Close();
			strLoc = "Loc_487yx";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("0123456789".ToCharArray());
			lReturn = dw2.Seek(-3, SeekOrigin.End);
			iCountTestcases++;
			if(lReturn != 7) {
				iCountErrors++;
				printerr("Error_20r45! Incorrec return value, lReturn=="+lReturn);
			} 
			dw2.Write("ll".ToCharArray());
			dw2.Flush();
			bArr = mstr.ToArray();
			sb = new StringBuilder();
			for(int i = 0 ; i < bArr.Length ; i++)
				sb.Append((Char)bArr[i]);
			iCountTestcases++;
			if(!sb.ToString().Equals("0123456ll9")) {
				iCountErrors++;
				printerr( "Error_874ty! Incorrect sequence in stream, sb=="+sb.ToString());
			}
			dw2.Close();
			strLoc = "Loc_183us";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("0123456789".ToCharArray());
			mstr.Position = 2;
			lReturn = dw2.Seek(2, SeekOrigin.Current);
			iCountTestcases++;
			if(lReturn != 4) {
				iCountErrors++;
				printerr("Error_23118! Incorrect return value, lReturn=="+lReturn);
			} 
			dw2.Write("ll".ToCharArray());
			dw2.Flush();
			bArr = mstr.ToArray();
			sb = new StringBuilder();
			for(int i = 0 ; i < bArr.Length ; i++)
				sb.Append((Char)bArr[i]);
			iCountTestcases++;
			if(!sb.ToString().Equals("0123ll6789")) {
				iCountErrors++;
				printerr( "Error_0198u! Incorrect sequence in stream, sb=="+sb.ToString());
			} 
			dw2.Close();
			strLoc = "Loc_948vj";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("0123456789".ToCharArray());
			iCountTestcases++;
			try {
				lReturn = dw2.Seek(4, SeekOrigin.End); 
				long iPosition = mstr.Position ;
                if( iPosition != 14 ){
                    iCountErrors++ ;
                    printerr( "Err_1234!! Unexpected position value .." + iPosition);
                }
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_398yg! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dw2.Close();
			strLoc = "Loc_039cu";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("0123456789".ToCharArray());
			iCountTestcases++;
			try {
				lReturn = dw2.Seek(11, SeekOrigin.Begin);
				long iPosition = mstr.Position ;
                if( iPosition != 11 ){
                    iCountErrors++ ;
                    printerr( "Err_4321!! Unexpected position value .." + iPosition);
                }
			} catch (IOException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_98yfx! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dw2.Close();
			strLoc = "Loc_298yv";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			dw2.Write("0123456789".ToCharArray());
			iCountTestcases++;
			lReturn = dw2.Seek(10, SeekOrigin.Begin);
			iCountTestcases++;
			if( lReturn != 10) {
				iCountErrors++;
				printerr( "Error_938tv! Incorrect return value, lReturn=="+lReturn);
			}
			dw2.Write("ll".ToCharArray());
			bArr = mstr.ToArray();
			sb = new StringBuilder();
			for(int i = 0 ; i < bArr.Length ; i++)
				sb.Append((Char)bArr[i]);
			iCountTestcases++;
			if(!sb.ToString().Equals("0123456789ll")) {
				iCountErrors++;
				printerr( "Error_27g8d! Incorrect stream, sb=="+sb.ToString());
			}
			dw2.Close();
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
		Co5561Seek_i_so cbA = new Co5561Seek_i_so();
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
