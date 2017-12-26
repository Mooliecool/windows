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
public class Co5708Open_fm_fa
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.OpenText(String)";
	public static String s_strTFName        = "Co5708Open_fm_fa.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	String strLoc = "Loc_000oo";
	String strValue = String.Empty;
	int iCountErrors = 0;
	int iCountTestcases = 0;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		try
		{
			TestMethod(FileMode.CreateNew, FileAccess.Read);			
			TestMethod(FileMode.CreateNew, FileAccess.Write);			
			TestMethod(FileMode.CreateNew, FileAccess.ReadWrite);			
			TestMethod(FileMode.Create, FileAccess.Read);		
			TestMethod(FileMode.Create, FileAccess.Write);
			TestMethod(FileMode.Create, FileAccess.ReadWrite);
			TestMethod(FileMode.Open, FileAccess.Read);
			TestMethod(FileMode.Open, FileAccess.Write);
			TestMethod(FileMode.Open, FileAccess.ReadWrite);
			TestMethod(FileMode.OpenOrCreate, FileAccess.Read);
			TestMethod(FileMode.OpenOrCreate, FileAccess.Write);
			TestMethod(FileMode.OpenOrCreate, FileAccess.ReadWrite);
			TestMethod(FileMode.Truncate, FileAccess.Read);
			TestMethod(FileMode.Truncate, FileAccess.Write);
			TestMethod(FileMode.Truncate, FileAccess.ReadWrite);
			TestMethod(FileMode.Append, FileAccess.Read);
			TestMethod(FileMode.Append, FileAccess.Write);
			TestMethod(FileMode.Append, FileAccess.ReadWrite);
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
	public void TestMethod(FileMode fm, FileAccess fa)
	{
			String fileName = s_strTFAbbrev+"TestFile";
			StreamWriter sw2;
			FileStream fs2;
			String str2;
			if(File.Exists(fileName))
				File.Delete(fileName);
			strLoc = "Loc_234yg";
			switch(fm) {
			case FileMode.CreateNew:
			case FileMode.Create:
			case FileMode.OpenOrCreate:
			    try {
				fs2 = File.Open( null, fm, fa);
				iCountTestcases++;
				if(!File.Exists(fileName)) {
					iCountErrors++;
					printerr( "Error_0001! File not created, FileMode=="+fm.ToString("G"));
				}
				fs2.Close();
				} catch (ArgumentException aexc) {
					printinfo( "Info_0002! Caught expected exception, aexc=="+aexc.Message);
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_0004! Incorrect exception thrown, exc=="+exc);
				}
			    try {
				fs2 = File.Open( "", fm, fa);
				iCountTestcases++;
				if(!File.Exists(fileName)) {
					iCountErrors++;
					printerr( "Error_0005! File not created, FileMode=="+fm.ToString("G"));
				}
				fs2.Close();
				} catch (ArgumentException aexc) {
						printinfo( "Info_0006! Caught expected exception, aexc=="+aexc.Message);
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_0008! Incorrect exception thrown, exc=="+exc);
				}
			    try {
				fs2 = File.Open( fileName, fm, fa);
				iCountTestcases++;
				if(!File.Exists(fileName)) {
					iCountErrors++;
					printerr( "Error_48gb7! File not created, FileMode=="+fm.ToString("G"));
				}
				fs2.Close();
				} catch (ArgumentException aexc) {
					if((fm == FileMode.Create && fa == FileAccess.Read) || (fm == FileMode.CreateNew && fa == FileAccess.Read)) {
						printinfo( "Info_4987v! Caught expected exception, aexc=="+aexc.Message);
					}
					else {
						iCountErrors++;
						printerr( "Error_478v8! Unexpected exception, aexc=="+aexc);
					}
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_4879v! Incorrect exception thrown, exc=="+exc);
				}
				break;
			case FileMode.Open:
			case FileMode.Truncate:
				iCountTestcases++;
				try {
					fs2 = File.Open(null, fm, fa);
					iCountErrors++;
					printerr( "Error_1001! Expected exception not thrown");
					fs2.Close();
				} catch (IOException fexc) {
					printinfo( "Info_1002! Caught expected exception, fexc=="+fexc.Message);
				} catch (ArgumentException aexc) {
						printinfo( "Info_1003! Caught expected exception, aexc=="+aexc.Message);					
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_1005! Incorrect exception thrown, exc=="+exc.ToString());
				}
				iCountTestcases++;
				try {
					fs2 = File.Open("", fm, fa);
					iCountErrors++;
					printerr( "Error_1006! Expected exception not thrown");
					fs2.Close();
				} catch (IOException fexc) {
					printinfo( "Info_1007! Caught expected exception, fexc=="+fexc.Message);
				} catch (ArgumentException aexc) { 
						printinfo( "Info_1008! Caught expected exception, aexc=="+aexc.Message);				
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_1010! Incorrect exception thrown, exc=="+exc.ToString());
				}
				iCountTestcases++;
				try {
					fs2 = File.Open(fileName, fm,fa);
					iCountErrors++;
					printerr( "Error_2yg8b! Expected exception not thrown");
					fs2.Close();
				} catch (IOException fexc) {
					printinfo( "Info_49y7b! Caught expected exception, fexc=="+fexc.Message);
				} catch (ArgumentException aexc) {
					if(fa == FileAccess.Read) 
						printinfo( "Info_4398v! Caught expected exception, aexc=="+aexc.Message);
					else {
						iCountErrors++;
						printerr( "Error_v48y8! Unexpected exception thrown, aexc=="+aexc);
					}						
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_2y7gf! Incorrect exception thrown, exc=="+exc.ToString());
				}
				break;
			case FileMode.Append:
				if(fa == FileAccess.Write) {
					fs2 = File.Open(fileName, fm,fa);
					iCountTestcases++;
					if(!File.Exists(fileName)) {
						iCountErrors++;
						printerr( "Error_2498y! File not created");
					}
					fs2.Close();
				}
				else {
					iCountTestcases++;
					try {
						fs2 = File.Open(fileName, fm,fa);
						iCountErrors++;
						printerr( "Error_2g78b! Expected exception not thrown");
						fs2.Close();
					} catch (ArgumentException aexc) {
						printinfo( "Info_2g7y7! Caught expected exception, aexc=="+aexc.Message);
					} catch (Exception exc) {
						iCountErrors++;
						printerr( "Error_g77b7! Incorrect exception thrown, exc=="+exc.ToString());
					}
				}
				break;
			default:
				iCountErrors++;
				printerr( "Error_27tbv! This should not be....");
				break;
			}
			if(File.Exists(fileName))
				File.Delete(fileName);
			strLoc = "Loc_4yg7b";
			sw2 = new StreamWriter(fileName);
			str2 = "Du er en ape";
			sw2.Write(str2);
			sw2.Close();
			switch(fm) {
			case FileMode.CreateNew:
				iCountTestcases++;
				try {
					fs2 = File.Open(null, fm,fa);
					iCountErrors++;
					printerr( "Error_2001! Expected exception not thrown");
					fs2.Close();
				} catch (ArgumentException aexc) {
						printinfo( "Info_2002! Caught expected exception, aexc=="+aexc.Message);
				} catch (IOException aexc) {
					printinfo( "Info_2004! Caught expected exception, aexc=="+aexc.Message);
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_2005! Incorrect exception thrown, exc=="+exc.ToString());
				}
				iCountTestcases++;
				try {
					fs2 = File.Open("", fm,fa);
					iCountErrors++;
					printerr( "Error_2006! Expected exception not thrown");
					fs2.Close();
				} catch (ArgumentException aexc) {
						printinfo( "Info_2007! Caught expected exception, aexc=="+aexc.Message);
				} catch (IOException aexc) {
					printinfo( "Info_2009! Caught expected exception, aexc=="+aexc.Message);
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_2010! Incorrect exception thrown, exc=="+exc.ToString());
				}
				iCountTestcases++;
				try {
					fs2 = File.Open(fileName, fm, fa);
					iCountErrors++;
					printerr( "Error_27b98! Expected exception not thrown");
					fs2.Close();
				} catch (ArgumentException aexc) {
					if(fa == FileAccess.Read) {
						printinfo( "Info_348vy! Caught expected exception, aexc=="+aexc.Message);
					}
					else {
						iCountErrors++;
						printerr( "Error_4387v! Unexpected exception, aexc=="+aexc);
					}
				} catch (IOException aexc) {
					printinfo( "Info_2399c! Caught expected exception, aexc=="+aexc.Message);
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_g8782! Incorrect exception thrown, exc=="+exc.ToString());
				}
				break;
			case FileMode.Create:
				try {
				fs2 = File.Open(fileName, fm,fa);
				if(fs2.Length != 0) {
					iCountErrors++;
					printerr( "Error_287vb! Incorrect length of file=="+fs2.Length);
				}
				fs2.Close();
				} catch (ArgumentException aexc) {
					if(fa == FileAccess.Read) {
						printinfo( "Info_4398v! Caught expected exception, aexc=="+aexc.Message);
					}
					else {
						iCountErrors++;
						printerr( "Error_48vy7! Unexpected exception, aexc=="+aexc);
					}
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_47yv3! Incorrect exception thrown, exc=="+exc.ToString());
				}
				break;
			case FileMode.OpenOrCreate:
			case FileMode.Open:
				fs2 = File.Open(fileName, fm,fa);
				if(fs2.Length != str2.Length ) {
					iCountErrors++;
					printerr( "Error_2gy78! Incorrect length on file=="+fs2.Length);
				}
				fs2.Close();
				break;
			case FileMode.Truncate:
				if(fa == FileAccess.Read) {
					iCountTestcases++;
					try {
						fs2 = File.Open(fileName,fm,fa);
						iCountErrors++;
						printerr( "Error_g95y8! Expected exception not thrown");
					} catch (ArgumentException iexc) {
						printinfo( "Info_2988f! Caught expected exception, iexc=="+iexc.Message);
					} catch (Exception exc) {
						iCountErrors++;
						printerr( "Error_98y4v! Incorrect exception thrown, exc=="+exc.ToString());
					}
				}
				else {
					fs2 = File.Open(fileName, fm,fa);
					if(fs2.Length != 0) {
						iCountErrors++;
						printerr( "Error_29gv9! Incorrect length on file=="+fs2.Length);
					}
					fs2.Close();
				}
				break;
			case FileMode.Append:
				if(fa == FileAccess.Write) {
					fs2 = File.Open(fileName, fm,fa);
					iCountTestcases++;
					if(!File.Exists(fileName)) {
						iCountErrors++;
						printerr( "Error_4089v! File not created");
					}
					fs2.Close();
				}
				else {
					iCountTestcases++;
					try {
						fs2 = File.Open(fileName,fm,fa);
						iCountErrors++;
					printerr( "Error_287yb! Expected exception not thrown");
					fs2.Close();
					} catch (ArgumentException aexc) {
						printinfo( "Info_93282! Caught expected exception, aexc=="+aexc.Message);
					} catch (Exception exc) {
						iCountErrors++;
						printerr( "Error_27878! Incorrect exception thrown, exc=="+exc.ToString());
					}
				}
				break;
			default:
				iCountErrors++;
				printerr( "Error_587yb! This should not be...");
				break;
			}
			if(File.Exists(fileName))
				File.Delete(fileName);
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
		Co5708Open_fm_fa cbA = new Co5708Open_fm_fa();
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
