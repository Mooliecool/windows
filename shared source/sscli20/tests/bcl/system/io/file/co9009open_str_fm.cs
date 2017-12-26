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
public class Co9009Open_str_fm
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.Open(String,FileMode)";
	public static String s_strTFName        = "Co9009Open_str_fm.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	String strLoc = "Loc_0001";
	String strValue = String.Empty;
	int iCountErrors = 0;
	int iCountTestcases = 0;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		try
		{
			TestMethod(FileMode.CreateNew);
			TestMethod(FileMode.Create);
			TestMethod(FileMode.Open);
			TestMethod(FileMode.OpenOrCreate);
			TestMethod(FileMode.Truncate);
			TestMethod(FileMode.Append);
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
	public void TestMethod(FileMode fm)
	{
			String fileName = s_strTFAbbrev+"TestFile";
			FileInfo fil2;
			StreamWriter sw2;
			Stream fs2 = null;
			String str2;
			if(File.Exists(fileName))
				File.Delete(fileName);
			strLoc = "Loc_0001";
			fil2 = new FileInfo(fileName);
			switch(fm) {
			case FileMode.CreateNew:
			case FileMode.Create:
			case FileMode.OpenOrCreate:
				iCountTestcases++;
				try{
					fs2 = File.Open( null , fm);
					if(!File.Exists(fileName)) {
						iCountErrors++;
						printerr( "Error_0002! File not created, FileMode=="+ fm.ToString());
					}
				} catch(ArgumentNullException ex ){
					printinfo("Expected exception thrown :: " + ex.Message );
				} catch ( Exception ex ){
					iCountErrors++ ;
					printerr("Error_0003! Unexpected exception thrown :: " + ex.ToString() );
				}
				iCountTestcases++;
				try{
					fs2 = File.Open( "", fm);
					if(!File.Exists(fileName)) {
						iCountErrors++;
						printerr( "Error_0004! File not created, FileMode=="+ fm.ToString());
					}
				} catch(ArgumentException ex ){
					printinfo("Expected exception thrown :: " + ex.Message );
				} catch ( Exception ex ){
					iCountErrors++ ;
					printerr("Error_0005! Unexpected exception thrown :: " + ex.ToString() );
				}
				fs2 = File.Open(fileName, fm);
				iCountTestcases++;
				if(!File.Exists(fileName)) {
					iCountErrors++;
					printerr( "Error_0006! File not created, FileMode=="+ fm.ToString());
				}
				fs2.Close();
				break;
			case FileMode.Open:
			case FileMode.Truncate:
				iCountTestcases++;
				try {
					fs2 = File.Open(fileName, fm);
					iCountErrors++;
					printerr( "Error_0007! Expected exception not thrown");
					fs2.Close();
				} catch (FileNotFoundException fexc) {
					printinfo( "Info_0008! Caught expected exception, fexc=="+fexc.Message);
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_0009! Incorrect exception thrown, exc=="+exc.ToString());
				}
				break;
			case FileMode.Append:
				iCountTestcases++;
				try {
					fs2 = File.Open(fileName , fm);
                    fs2.Write(new Byte[]{54,65,54,90}, 0 , 4 );
                    if ( fs2.Length != 4) {
                        iCountErrors++ ;
                        Console.WriteLine( "Unexpected file length .... " + fs2.Length );
                    }
					fs2.Close();
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_0012! Incorrect exception thrown, exc=="+exc.ToString());
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
					fs2 = File.Open(fileName , fm);
					iCountErrors++;
					printerr( "Error_27b98! Expected exception not thrown");
					fs2.Close();
				} catch (IOException aexc) {
					printinfo( "Info_2399c! Caught expected exception, aexc=="+aexc.Message);
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_g8782! Incorrect exception thrown, exc=="+exc.ToString());
				}
				break;
			case FileMode.Create:
				fs2 = File.Open(fileName , fm);
				if(fs2.Length != 0) {
					iCountErrors++;
					printerr( "Error_287vb! Incorrect length of file=="+fil2.Length);
				}
				fs2.Close();
				break;
			case FileMode.OpenOrCreate:
			case FileMode.Open:
				fs2 = File.Open(fileName , fm);
				if(fs2.Length != str2.Length ) {
					iCountErrors++;
					printerr( "Error_2gy78! Incorrect length on file=="+fil2.Length);
				}
				fs2.Close();
				break;
			case FileMode.Truncate:
				fs2 = File.Open(fileName , fm);
				if(fs2.Length != 0) {
					iCountErrors++;
					printerr( "Error_29gv9! Incorrect length on file=="+fil2.Length);
				}
				fs2.Close();
				break;
			case FileMode.Append:
				iCountTestcases++;
				try {
					fs2 = File.Open(fileName , fm);
                    fs2.Write(new Byte[]{54,65,54,90}, 0 , 4 );
                    if ( fs2.Length != 16) {  
                        iCountErrors++ ;
                        Console.WriteLine( "Unexpected file length .... " + fs2.Length );
                    }
					fs2.Close();
				} catch (Exception exc) {
					iCountErrors++;
					printerr( "Error_27878! Incorrect exception thrown, exc=="+exc.ToString());
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
		Co9009Open_str_fm cbA = new Co9009Open_str_fm();
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
