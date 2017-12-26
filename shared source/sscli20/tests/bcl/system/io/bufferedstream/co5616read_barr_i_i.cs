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
public class Co5616Read_bArr_i_i
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BufferedStream.Write(Byte[], Int, Int";
	public static String s_strTFName        = "Co5616Read_bArr_i_i.cs";
	public static String s_strTFAbbrev      = "Co5616";
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
			BufferedStream bs2;
			MemoryStream memstr2;
			FileStream fs2;
			Int32 count;
			Byte[] bWriteArr, bReadArr;
			if(File.Exists("Co5616Test.tmp"))
				File.Delete("Co5616Test.tmp");			
			strLoc = "Loc_984yv";
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			iCountTestcases++;
			try {
				bs2.Read(null, 0, 0);
				iCountErrors++;
				printerr( "Error_298yv! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo("Info_g98b7! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_t8y78! Incorrect exception thrown, exc=="+exc.ToString());
			}
			bs2.Close();
			strLoc = "Loc_9875g";
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, -1, 0);
				iCountErrors++;
				printerr("Error_988bb! Expected exception not thrown");
			} catch (ArgumentOutOfRangeException aexc) {
				printinfo("Info_98yby! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_099hy! Incorrect exception thrown, exc=="+exc.ToString());
			}
			bs2.Close();
			strLoc = "Loc_5g8ys";
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, 0, -1);
				iCountErrors++;
				printerr( "Error_9t8yj! Expected exception not thrown");
			} catch (ArgumentOutOfRangeException aexc) {
				printinfo("Info_9488b! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_7687b! Incorrect exception thrown, exc=="+exc.ToString());
			}
			bs2.Close();
			strLoc = "Loc_1228x";
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			bs2.Write(new Byte[]{1}, 0, 1);
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, 2, 0);
				iCountErrors++;
				printerr( "Error_3444j! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo("Info_g8777! CAught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_t77gg! Incorrect exception thrown, exc=="+exc.ToString());
			}
			bs2.Close();
			strLoc = "Loc_897yg";
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			bs2.Write(new Byte[]{1}, 0, 1);
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, 0, 2);
				iCountErrors++;
				printerr( "Error_98y8x! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo("Info_g58yb! Caught expected exception, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_ytg78! Incorrect exception thrown, exc=="+exc.ToString());
			}			
			strLoc = "Loc_8g7yb";
			count = 0;
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			bWriteArr = new Byte[100];
			for(int i = 0 ; i < 100 ; i++) 
				bWriteArr[i] = (Byte)i;
			bs2.Write(bWriteArr, 0, 100);
			iCountTestcases++;   
                        bs2.Flush();
			bReadArr = memstr2.ToArray();
			for(int i = 0 ; i < bReadArr.Length ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr( "Error_98ybb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			bs2.Position = 0;
			count = bs2.Read(bReadArr, 0, 100);
			iCountTestcases++;
			if(count != 100) {
				iCountErrors++;
				printerr( "Error_958yv! Incorrect count=="+count);
			}
			for(int i = 0 ; i < bReadArr.Length ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr( "Error_98gy8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}			
			bs2.Close();
			strLoc = "Loc_g85y7";
			count = 0;
			fs2 = new FileStream("Co5616Test.tmp", FileMode.Create);
			bs2 = new BufferedStream(fs2);
			bWriteArr = new Byte[100];
			for(int i = 0 ; i < 100 ; i++) 
				bWriteArr[i] = (Byte)i;
			bs2.Write(bWriteArr, 0, 100);
			iCountTestcases++;   
                        bs2.Flush();
			fs2.Position = 0;
			fs2.Read(bReadArr, 0, 100);
			for(int i = 0 ; i < bReadArr.Length ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr( "Error_98ybb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			bs2.Position = 0;
			count = bs2.Read(bReadArr, 0, 100);
			iCountTestcases++;
			if(count != 100) {
				iCountErrors++;
				printerr( "Error_11789! Incorrect count=="+count);
			}
			for(int i = 0 ; i < bReadArr.Length ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr( "Error_98gy8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}			
			fs2.Close();
			strLoc = "Loc_r78g8";
			count = 0;
			memstr2 = new MemoryStream(11);
			bs2 = new BufferedStream(memstr2);
			bWriteArr = new Byte[100];
			for(int i = 0 ; i < 100 ; i++) 
				bWriteArr[i] = (Byte)i;
			bs2.Write(bWriteArr, 10, 12);
			iCountTestcases++;   
                        bs2.Flush();
			memstr2.Position = 0;
			bReadArr = memstr2.ToArray();
			for(int i = 0 ; i < 12 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i+10]) {
					iCountErrors++;
					printerr( "Error_y88gb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			bs2.Position = 0;
			bReadArr = new Byte[100];
			count = bs2.Read(bReadArr, 10, 12);
			iCountTestcases++;
			if(count != 12) {
				iCountErrors++;
				printerr( "Error_29908! Incorrect count=="+count);
			}
			for(int i = 10 ; i < 22 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr("Error_98yg8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			strLoc = "Loc_98fyf";
			bs2.Write(bWriteArr, 80, 19);
			iCountTestcases++;  
                        bs2.Flush();
			memstr2.Position = 0;
			bReadArr = memstr2.ToArray();
			for(int i = 0 ; i < 12 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i+10]) {
					iCountErrors++;
					printerr( "Error_87vyb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			for(int i = 12 ; i < 30 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i+68]) {
					iCountErrors++;
					printerr( "Error_t8yb8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			bs2.Position = 12;
			bReadArr = new Byte[100];
			count = bs2.Read(bReadArr, 80, 19);
			iCountTestcases++;
			if(count != 19) {
				iCountErrors++;
				printerr( "Error_f7878! Incorrect count=="+count);
			}
			for(int i = 80; i < 99 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr( "Error_87gyb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			bs2.Close();
			strLoc = "Loc_r78g8";
			count = 0;
			fs2 = new FileStream("Co5616Test.tmp", FileMode.Create);
			bs2 = new BufferedStream(fs2);
			bWriteArr = new Byte[100];
			for(int i = 0 ; i < 100 ; i++) 
				bWriteArr[i] = (Byte)i;
			bs2.Write(bWriteArr, 10, 12);
			iCountTestcases++;    
                        bs2.Flush();
			fs2.Position = 0;
			fs2.Read(bReadArr, 0, 12);
			for(int i = 0 ; i < 12 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i+10]) {
					iCountErrors++;
					printerr( "Error_y88gb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			bs2.Position = 0;
			bReadArr = new Byte[100];
			count = bs2.Read(bReadArr, 10, 12);
			iCountTestcases++;
			if(count != 12) {
				iCountErrors++;
				printerr( "Error_29908! Incorrect count=="+count);
			}
			for(int i = 10 ; i < 22 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr("Error_98yg8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			strLoc = "Loc_98fyf";
			bs2.Write(bWriteArr, 80, 19);
			iCountTestcases++;  
			bs2.Flush();
			fs2.Position = 0;
			fs2.Read(bReadArr, 0, 30);
			for(int i = 0 ; i < 12 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i+10]) {
					iCountErrors++;
					printerr( "Error_87vyb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			for(int i = 12 ; i < 30 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i+68]) {
					iCountErrors++;
					printerr( "Error_t8yb8! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			bs2.Position = 12;
			bReadArr = new Byte[100];
			count = bs2.Read(bReadArr, 80, 19);
			iCountTestcases++;
			if(count != 19) {
				iCountErrors++;
				printerr( "Error_f7878! Incorrect count=="+count);
			}
			for(int i = 80; i < 99 ; i++) {
				iCountTestcases++;
				if(bReadArr[i] != bWriteArr[i]) {
					iCountErrors++;
					printerr( "Error_87gyb! Expected=="+bWriteArr[i]+", got=="+bReadArr[i]);
				}
			}
			fs2.Close();
			strLoc = "Loc_8975y";
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			memstr2.Close();
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, 0, 1);
				iCountErrors++;
				printerr( "Error_2228k! Expected exception not thrown");
			} catch (NotSupportedException iexc) {
				printinfo("Info_87y7b! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1289x! Incorrect exception thrown, exc=="+exc.ToString());
			}
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			bs2.Close();
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, 0, 1);
				iCountErrors++;
				printerr( "Error_t98yb! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo("Info_8y88b! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_98b8a! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_985yb";
			fs2 = new FileStream("Co5616Test.tmp", FileMode.Create);
			bs2 = new BufferedStream(fs2);
			fs2.Close();
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, 0, 1);
				iCountErrors++;
				printerr( "Error_f487h! Expected exception not thrown");
			} catch (NotSupportedException iexc) {
				printinfo("Info_899bb! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298yb! Incorrect exception thrown, exc=="+exc.ToString());
			}
			fs2 = new FileStream("Co5616Test.tmp", FileMode.Create);
			bs2 = new BufferedStream(fs2);
			bs2.Close();
			iCountTestcases++;
			try {
				bs2.Read(new Byte[]{1}, 0, 1);
				iCountErrors++;
				printerr( "Error_0199vf! Expected exception not thrown");
			} catch (ObjectDisposedException iexc) {
				printinfo("Info_998yv! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_10tbm! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_f487y";
			memstr2 = new MemoryStream();
			bs2 = new BufferedStream(memstr2);
			bs2.Write(new Byte[3]{1,2,3}, 0, 3);
			bs2.Flush();
			bs2.Position = 0;
			count = bs2.Read(new Byte[0], 0, 0);
			iCountTestcases++;
			if(count != 0) {
				iCountErrors++;
				printerr("Error_298yb! Incorrect count=="+count);
			}
			iCountTestcases++;
			if(bs2.Position != 0) {
				iCountErrors++;
				printerr( "Error_2yg7b! Incorrect position=="+bs2.Position);
			} 
			iCountTestcases++;
			if(bs2.Length != 3) {
				iCountErrors++;
				printerr( "Error_2000d! Incorrect length=="+bs2.Length);
			}
			if(File.Exists("Co5616Test.tmp"))
				File.Delete("Co5616Test.tmp");
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
		Co5616Read_bArr_i_i cbA = new Co5616Read_bArr_i_i();
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
