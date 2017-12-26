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
public class Co5710Move_str_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.OpenText(String)";
	public static String s_strTFName        = "Co5710Move_str_str.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		try
		{
			String filName = s_strTFAbbrev+"TestFile";
			FileInfo fil1, fil2;
			StreamWriter sw2;
			Char[] cWriteArr, cReadArr;
			StreamReader sr2;
			if(File.Exists(filName)) 
				File.Delete(filName);
			strLoc = "Loc_498yg";
			File.Create(filName).Close();
			fil2 = new FileInfo(filName);
			iCountTestcases++;
			try {
				File.Move(null, fil2.Name);
				iCountErrors++;
				printerr( "Error_209uz! Expected exception not thrown, fil2=="+fil2.FullName);
			} catch (ArgumentNullException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_21x99! Incorrect exception thrown, exc=="+exc.ToString());
			}
			fil2.Delete();
			strLoc = "Loc_898gc";
			File.Create(filName).Close();
			fil2 = new FileInfo(filName);
			iCountTestcases++;
			try {
				File.Move(fil2.Name, null);
				iCountErrors++;
				printerr( "Error_8yt85! Expected exception not thrown");
			} catch (ArgumentNullException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1298s! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_298vy";
			File.Create(filName).Close();
			fil2 = new FileInfo(filName);
			iCountTestcases++;
			try {
				File.Move(fil2.Name, String.Empty);
				iCountErrors++;
				printerr( "Error_092u9! Expected exception not thrown, fil2=="+fil2.FullName);
			} catch (ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_109uc! Incorrect exception thrown, exc=="+exc.ToString());
			}
			fil2.Delete();
			strLoc = "Loc_289vy";
			File.Create(filName).Close();
			fil2 = new FileInfo(filName);
			iCountTestcases++;
			try {
				File.Move(fil2.Name, ".");
				iCountErrors++;
				printerr( "Error_301ju! Expected exception not thrown, fil2=="+fil2.FullName);
			} catch (IOException iexc) {
				printinfo("Info_2389! Caught expected exception, exc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_209us! Incorrect exception thrown, exc=="+exc.ToString());
			}
			fil2.Delete();
			strLoc = "Loc_r7yd9";
			File.Create(filName).Close();
			fil2 = new FileInfo(filName);
			iCountTestcases++;
			File.Move(fil2.Name, fil2.Name);
			if(!File.Exists(fil2.Name)) {
				iCountErrors++;
				printerr( "Error_r8g7b! Expected exception not thrown");
			}
			fil2.Delete();
			strLoc = "Loc_f548y";
			File.Create(filName).Close();
			fil2 = new FileInfo(filName);
			iCountTestcases++;
			File.Move(fil2.Name, "TestFile.tmp1");
			fil1 = new FileInfo("TestFile.tmp1");
			fil2.Refresh();
			if(!File.Exists(fil1.Name)) {
				iCountErrors++;
				printerr( "Error_2978y! File not copied");
			}
			if(File.Exists(fil2.Name)) {
				iCountErrors++;
				printerr( "Error_239vr! Source file still there");
			} 
			fil1.Delete();
			fil2.Delete();
			strLoc = "Loc_984hg";
			File.Create(filName).Close();
			fil2 = new FileInfo(filName);
			iCountTestcases++;
			try {
				File.Move(fil2.Name, "**");
				iCountErrors++;
				printerr( "Error_298xh! Expected exception not thrown, fil2=="+fil2.FullName);
				fil2.Delete();
			} catch (ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2091s! Incorrect exception thrown, exc=="+exc.ToString());
			}
			fil2.Delete();
			strLoc = "Loc_f888m";
			File.Delete("TestTest.tmp");
try {
			sw2 = new StreamWriter(File.Create(filName));
			cWriteArr = new Char[26];
			int j = 0;
			for(Char i = 'A' ; i <= 'Z' ; i++) 
				cWriteArr[j++] = i;
			sw2.Write(cWriteArr, 0, cWriteArr.Length);
			sw2.Flush();
			sw2.Close();
			fil2 = new FileInfo(filName);
			File.Move(fil2.Name, "TestTest.tmp");
			fil1 = new FileInfo("TestTest.tmp");
			fil2.Refresh();
			iCountTestcases++;
			if(!fil1.Exists) {
				iCountErrors++;
				printerr( "Error_9u99s! File not copied correctly: "+fil1.FullName);
			}
			if(File.Exists(fil2.Name)) {
				iCountErrors++;
				printerr( "Error_29h7b! Source file gone");
			}
			sr2 = new StreamReader("TestTest.tmp");
			cReadArr = new Char[cWriteArr.Length];
			sr2.Read(cReadArr, 0, cReadArr.Length);
			iCountTestcases++;
			for(int i = 0 ; i < cReadArr.Length ; i++) {
				iCountTestcases++;
				if( cReadArr[i] != cWriteArr[i]) {
					iCountErrors++;
					printerr( "Error_98yv7! Expected=="+cWriteArr[i]+", got value=="+cReadArr[i]);
				}
			}
			sr2.Close();
			fil1.Delete();
			fil2.Delete();
} catch (Exception exc) {
	iCountErrors++;
	printerr( "Error_28vc8! Unexpected exception, exc=="+exc.ToString());
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
		Co5710Move_str_str cbA = new Co5710Move_str_str();
		try {
			bResult = cbA.runTest();
		} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
		}
		if (!bResult)
		{
			Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
			Console.WriteLine( " " );
			Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
			Console.WriteLine( " " );
      }
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
