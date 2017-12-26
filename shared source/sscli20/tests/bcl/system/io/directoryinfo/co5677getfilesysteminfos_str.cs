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
public class Co5677GetFileSystemInfos_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetFiles()";
	public static String s_strTFName        = "Co5677GetFileSystemInfos _str.cs";
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
			FileSystemInfo[] fsArr;
			if(Directory.Exists(dirName))
				Directory.Delete(dirName, true);
			strLoc = "Loc_477g8";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				dir2.GetFileSystemInfos (null);
				iCountErrors++;
				printerr( "Error_2988b! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_29087! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0707t! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_4yg7b";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				FileSystemInfo[] strInfos = dir2.GetFileSystemInfos (String.Empty);
				if( strInfos.Length != 0) {                
                    iCountErrors++;
                    printerr( "Error_8ytbm! Unexpected number of file infos returned" + strInfos.Length);
                }
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2908y! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_1190x";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				dir2.GetFileSystemInfos ("..ab ab.. .. abc..d\abc.." );
				iCountErrors++;
				printerr( "Error_2198y! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo( "Info_94170! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_17888! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_4y982";
			dir2 = Directory.CreateDirectory(dirName);
			fsArr = dir2.GetFileSystemInfos ();
			iCountTestcases++;
			if(fsArr.Length != 0) {
				iCountErrors++;
				printerr("Error_207v7! Incorrect number of files returned");
			}
			strLoc = "Loc_2398c";
			dir2.CreateSubdirectory("TestDir1");
			dir2.CreateSubdirectory("TestDir2");
			dir2.CreateSubdirectory("TestDir3");
			dir2.CreateSubdirectory("Test1Dir1");
			dir2.CreateSubdirectory("Test1Dir2");
			new FileInfo(dir2.FullName + "\\" +"TestFile1").Create();
			new FileInfo(dir2.FullName + "\\" +"TestFile2").Create();
			new FileInfo(dir2.FullName + "\\" +"TestFile3").Create();
			new FileInfo(dir2.FullName + "\\" +"Test1File1").Create();
			new FileInfo(dir2.FullName + "\\" +"Test1File2").Create();
			iCountTestcases++;
			fsArr = dir2.GetFileSystemInfos ("TestFile*");
			iCountTestcases++;
			if(fsArr.Length != 3) {
				iCountErrors++;
				printerr( "Error_1yt75! Incorrect number of files returned");
			}
			String[] names = new String[fsArr.Length];
			int i = 0;
			foreach ( FileSystemInfo f in fsArr) 
				names[i++] = f.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile1") < 0) {
				iCountErrors++;
				printerr( "Error_3y775! Incorrect name=="+fsArr[0].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_90885! Incorrect name=="+fsArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile3") < 0) {
				iCountErrors++;
				printerr( "Error_879by! Incorrect name=="+fsArr[2].Name);
			}
			fsArr = dir2.GetFileSystemInfos ("*");
			iCountTestcases++;
			if(fsArr.Length != 10) {
				iCountErrors++;
				printerr( "Error_t5792! Incorrect number of files=="+fsArr.Length);
			}
			names = new String[fsArr.Length];
			i = 0;
			foreach ( FileSystemInfo f in fsArr) 
				names[i++] = f.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir1") < 0) {
				iCountErrors++;
				printerr( "Error_4898v! Incorrect name=="+fsArr[0].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_4598c! Incorrect name=="+fsArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir1") < 0) {
				iCountErrors++;
				printerr( "Error_209d8! Incorrect name=="+fsArr[2].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_10vtu! Incorrect name=="+fsArr[3].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir3") < 0) {
				iCountErrors++;
				printerr( "Error_190vh! Incorrect name=="+fsArr[4].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File1") < 0) {
				iCountErrors++;
				printerr( "Error_4898v! Incorrect name=="+fsArr[5].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_4598c! Incorrect name=="+fsArr[6].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile1") < 0) {
				iCountErrors++;
				printerr( "Error_209d8! Incorrect name=="+fsArr[7].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_10vtu! Incorrect name=="+fsArr[8].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile3") < 0) {
				iCountErrors++;
				printerr( "Error_190vh! Incorrect name=="+fsArr[9].Name);
			}
			fsArr = dir2.GetFileSystemInfos ("*2");
			iCountTestcases++;
			if(fsArr.Length != 4) {
				iCountErrors++;
				printerr( "Error_8019x! Incorrect number of files=="+fsArr.Length);
			}
			names = new String[fsArr.Length];
			i = 0;
			foreach ( FileSystemInfo fs in fsArr ) 
				names[i++] = fs.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_247yg! Incorrect name=="+fsArr[0].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_24gy7! Incorrect name=="+fsArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_167yb! Incorrect name=="+fsArr[2].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_49yb7! Incorrect name=="+fsArr[3].Name);
			}
			fsArr = dir2.GetFileSystemInfos ("*Dir2");
			iCountTestcases++;
			if(fsArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_948yv! Incorrect number of files=="+fsArr.Length);
			}
			names = new String[fsArr.Length];
			i = 0;
			foreach ( FileSystemInfo fs in fsArr)
				names[i++] = fs.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_247yg! Incorrect name=="+fsArr[0].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_24gy7! Incorrect name=="+fsArr[1].Name);
			}
			new FileInfo(dir2.FullName + "\\"  +"AAABB").Create();
			Directory.CreateDirectory(dir2.FullName + "\\" +"AAABBCC");
			fsArr = dir2.GetFileSystemInfos ("*BB*");
			iCountTestcases++;
			if(fsArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_4y190! Incorrect number of files=="+fsArr.Length);
			} 
			names = new String[fsArr.Length];
			i = 0;
			foreach ( FileSystemInfo fs in fsArr)
				names[i++] = fs.Name;
			foreach (FileSystemInfo s in fsArr) 
				Console.WriteLine(s.Name);
			iCountTestcases++;
			if(Array.IndexOf(names, "AAABBCC") < 0) {
				iCountErrors++;
				printerr( "Error_956yb! Incorrect name=="+fsArr[0]);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "AAABB") < 0) {
				iCountErrors++;
				printerr( "Error_48yg7! Incorrect name=="+fsArr[1]);
			}
 			strLoc = "Loc_0001";
			fsArr = dir2.GetFileSystemInfos ("Directory");
			iCountTestcases++;
			if(fsArr.Length != 0) {
				iCountErrors++;
				printerr( "Error_209v7! Incorrect number of files=="+fsArr.Length);
			}
			new FileInfo(dir2.FullName + "\\"  +"TestDir1\\Test.tmp").Create();
			fsArr = dir2.GetFileSystemInfos ("TestDir1\\*");
			iCountTestcases++;
			if(fsArr.Length != 1) {
				iCountErrors++;
				printerr( "Error_28gyb! Incorrect number of files");
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
		Co5677GetFileSystemInfos_str cbA = new Co5677GetFileSystemInfos_str();
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
