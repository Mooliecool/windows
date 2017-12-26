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
public class Co5672GetFiles_str_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetFiles()";
	public static String s_strTFName        = "Co5672GetFiles_str_str.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
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
			DirectoryInfo dir2;
			String dirName = s_strTFAbbrev+"TestDir";
			String[] strArr;
			if(Directory.Exists(dirName))
				Directory.Delete(dirName, true);
			strLoc = "Loc_477g8";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				Directory.GetFiles(null, "*");
				iCountErrors++;
				printerr( "Error_2988b! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_29087! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0707t! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				Directory.GetFiles(".", null);
				iCountErrors++;
				printerr( "Error_2y867! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_2875b! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_39yb7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_4yg7b";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				Directory.GetFiles(String.Empty, "*");
				iCountErrors++;
				printerr( "Error_8ytbm! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo( "Info_y687d! Caught expected, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2908y! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				String[] strFiles = Directory.GetFiles(".", String.Empty);
				if ( strFiles.Length != 0 ) {
                    iCountErrors++;
                    printerr( "Error_478b8! Incorrect number of files");
                }
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_21999! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_1190x";
			iCountTestcases++;
			try {
				String[] str = Directory.GetFiles(".", "       ");
                Console.WriteLine("Number of files :: " + str.Length );
				if ( str.Length != 0 ) {
                    iCountErrors++;
                    printerr( "Error_43432! Incorrect number of files");
                }                
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_17888! Incorrect exception thrown, exc=="+exc.ToString());
			}
			iCountTestcases++;
			try {
				Directory.GetFiles("     " , ".");
				iCountErrors++;
				printerr( "Error_29019! Expected exception not thrown" );
			} catch(ArgumentException iexc) {
				printinfo( "Info_6867b! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_9678g! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_4y982";
			dir2 = Directory.CreateDirectory(dirName);
			strArr = Directory.GetFiles(Environment.CurrentDirectory+"\\"+dirName, "*");
			iCountTestcases++;
			if(strArr.Length != 0) {
				iCountErrors++;
				printerr("Error_207v7! Incorrect number of files returned");
			}
			strLoc = "Loc_2398c";
			dir2.CreateSubdirectory("TestDir1");
			dir2.CreateSubdirectory("TestDir2");
			dir2.CreateSubdirectory("TestDir3");
			dir2.CreateSubdirectory("Test1Dir1");
			dir2.CreateSubdirectory("Test1Dir2");
			new FileInfo(dir2.FullName + "\\" + "TestFile1").Create();
			new FileInfo(dir2.FullName + "\\" +"TestFile2").Create();
			new FileInfo(dir2.FullName + "\\" +"TestFile3").Create();
			new FileInfo(dir2.FullName + "\\" +"Test1File1").Create();
			new FileInfo(dir2.FullName + "\\" +"Test1File2").Create();
			iCountTestcases++;
			strArr = Directory.GetFiles(Environment.CurrentDirectory+"\\"+dirName, "TestFile*");
			iCountTestcases++;
			if(strArr.Length != 3) {
				iCountErrors++;
				printerr( "Error_1yt75 Incorrect number of files returned=="+strArr.Length);
			}
			String[] names = new String[strArr.Length];
			int i = 0;
			foreach (String f in strArr)
				names[i++] = f.Substring( f.ToString().LastIndexOf(Path.DirectorySeparatorChar) + 1 );
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile1") < 0) {
				iCountErrors++;
				printerr( "Error_3y775! Incorrect name=="+strArr[0].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_90885! Incorrect name=="+strArr[1].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile3") < 0) {
				iCountErrors++;
				printerr( "Error_879by! Incorrect name=="+strArr[2].ToString());
			}
			strLoc = "Loc_4y7gb";
			strArr = Directory.GetFiles(".\\"+dirName, "*");
			iCountTestcases++;
			if(strArr.Length != 5) {
				iCountErrors++;
				printerr( "Error_t5792! Incorrect number of files=="+strArr.Length);
			}
			names = new String[strArr.Length];
			i = 0;
			foreach (String f in strArr)
				names[i++] = f.Substring( f.ToString().LastIndexOf(Path.DirectorySeparatorChar) + 1 );
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File1") < 0) {
				iCountErrors++;
				printerr( "Error_4898v! Incorrect name=="+strArr[0].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_4598c! Incorrect name=="+strArr[1].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile1") < 0 ) {
				iCountErrors++;
				printerr( "Error_209d8! Incorrect name=="+strArr[2].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_10vtu! Incorrect name=="+strArr[3].ToString());
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile3") < 0) {
				iCountErrors++;
				printerr( "Error_190vh! Incorrect name=="+strArr[4].ToString());
			}
			strLoc = "Loc_4yg7b";
			strArr = Directory.GetFiles(Environment.CurrentDirectory+"\\"+dirName, "*File2");
			iCountTestcases++;
			if(strArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_8019x! Incorrect number of files=="+strArr.Length);
			}
			names = new String[strArr.Length];
			i = 0;
			foreach ( String f in strArr)
				names[i++] = f.Substring( f.ToString().LastIndexOf(Path.DirectorySeparatorChar) + 1 );
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_167yb! Incorrect name=="+strArr[0].ToString());
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_49yb7! Incorrect name=="+strArr[1].ToString());
			}
			strLoc = "Loc_767b7";
			new FileInfo(dir2.FullName + "\\" +"AAABB").Create();
			new FileInfo(dir2.FullName + "\\" +"aaabbcc").Create();
			strArr = Directory.GetFiles(Environment.CurrentDirectory+"\\"+dirName, "*BB*");
			iCountTestcases++;
#if PLATFORM_UNIX // case-sensitive filesystem
                        if(strArr.Length != 1) {
#else
			if(strArr.Length != 2) {
#endif
				iCountErrors++;
				printerr( "Error_4y190! Incorrect number of files=="+strArr.Length);
			} 
			names = new String[strArr.Length];
			i = 0;
			foreach(String f in strArr)
				names[i++] = f.Substring( f.ToString().LastIndexOf(Path.DirectorySeparatorChar) + 1 );
			iCountTestcases++;
			if(Array.IndexOf(names, "AAABB") < 0) {
				iCountErrors++;
				printerr( "Error_956yb! Incorrect name=="+strArr[0].ToString());
			} 
#if !PLATFORM_UNIX // case-sensitive filesystem
			iCountTestcases++;
			if(Array.IndexOf(names, "aaabbcc") < 0) {
				iCountErrors++;
				printerr( "Error_48yg7! Incorrect name=="+strArr[1].ToString());
			}
#endif
			strLoc = "Loc_38yf8";
			strArr = Directory.GetFiles(Environment.CurrentDirectory+"\\"+dirName, "AAABB");
			iCountTestcases++;
			if(strArr.Length != 1) {
				iCountErrors++;
				printerr( "Error_20989! Incorrect number of files=="+strArr.Length);
			}
			if((strArr[0].ToString().IndexOf("AAABB")) == -1 ) {
				iCountErrors++;
				printerr( "Error_4y8v8! Incorrect name=="+strArr[0].ToString());
			}
			strLoc = "Loc_yg78b";
			strArr = Directory.GetFiles(".\\"+dirName, "Directory");
			iCountTestcases++;
			if(strArr.Length != 0) {
				iCountErrors++;
				printerr( "Error_209v7! Incorrect number of files=="+strArr.Length);
			}
			new FileInfo(dir2.FullName + "\\" +"TestDir1\\Test.tmp").Create();
			strArr = Directory.GetFiles(Environment.CurrentDirectory+"\\"+dirName, "TestDir1\\*");
			iCountTestcases++;
			if(strArr.Length != 1) {
				iCountErrors++;
				printerr( "Error_28gyb! Incorrect number of files");
			}
			strLoc = "Loc_98yg5";
			iCountTestcases++;
			try {
				Directory.GetFiles(",", "*");
				iCountErrors++;
				printerr( "Error_2y675! Expected exception not thrown");
			} catch (DirectoryNotFoundException dexc) {
				printinfo( "Info_58y76! Caught expected exception, dexc=="+dexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_249y6! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_98001";
			iCountTestcases++;
			try {
				Directory.GetFiles("DoesNotExist", "*");
				iCountErrors++;
				printerr( "Error_2y76b! Expected exception not thrown");
			} catch (DirectoryNotFoundException dexc) {
				printinfo( "Info_666yb! Caught expected exception, dexc=="+dexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_24y7g! Incorrect exception thrown, exc=="+exc.ToString());
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
		Co5672GetFiles_str_str cbA = new Co5672GetFiles_str_str();
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
