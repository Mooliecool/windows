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
public class Co5676GetFileSystemInfos 
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetFiles()";
	public static String s_strTFName        = "Co5676GetFileSystemInfos .cs";
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
			String str2;
			FileSystemInfo[] fsArr;
			if(Directory.Exists(dirName))
				Directory.Delete(dirName, true);
			strLoc = "Loc_4y982";
			dir2 = Directory.CreateDirectory(dirName);
			fsArr = dir2.GetFileSystemInfos ();
			iCountTestcases++;
			if(fsArr.Length != 0) {
				iCountErrors++;
				printerr("Error_207v7! Incorrect number of directories returned");
			}
			strLoc = "Loc_2398c";
			dir2.CreateSubdirectory("TestDir1");
			dir2.CreateSubdirectory("TestDir2");
			dir2.CreateSubdirectory("TestDir3");
			new FileInfo(dir2.ToString() +"TestFile1");
			new FileInfo(dir2.ToString() +"TestFile2");
			new FileInfo(dir2.ToString() +"Test.bat");
			new FileInfo(dir2.ToString() +"Test.exe");
            FileStream[] fs = new FileStream[4];
			fs[0] = new FileInfo(dir2.FullName + "\\"  +"TestFile1").Create();
			fs[1] = new FileInfo(dir2.FullName + "\\"  +"TestFile2").Create();
			fs[2] = new FileInfo(dir2.FullName + "\\"  +"Test.bat").Create();
			fs[3] = new FileInfo(dir2.FullName + "\\"  +"Test.exe").Create();
			for( int iLoop = 0 ; iLoop < 4 ; iLoop++ )
                fs[iLoop].Close();
			iCountTestcases++;
			fsArr = dir2.GetFileSystemInfos ();
			iCountTestcases++;
			if(fsArr.Length != 7) {
				iCountErrors++;
				printerr( "Error_1yt75! Incorrect number of directories returned");
			}
			String[] names = new String[7];
			int i = 0;
			foreach(FileSystemInfo fse in fsArr)
				names[i++] = fse.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir1") < 0) {
				iCountErrors++;
				printerr( "Error_4yg76! Incorrec tname=="+fsArr[0].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_1987y! Incorrect name=="+fsArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir3") < 0) {
				iCountErrors++;
				printerr( "Error_4yt76! Incorrect name=="+fsArr[2].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test.bat") < 0) {
				iCountErrors++;
				printerr( "Error_3y775! Incorrect name=="+fsArr[3].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test.exe") < 0) {
				iCountErrors++;
				printerr( "Error_90885! Incorrect name=="+fsArr[4].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile1") < 0) {
				iCountErrors++;
				printerr( "Error_879by! Incorrect name=="+fsArr[5].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_29894! Incorrect name=="+fsArr[6].Name);
			}
			if(Directory.Exists(dirName))
				Directory.Delete(dirName, true);
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
		Co5676GetFileSystemInfos  cbA = new Co5676GetFileSystemInfos ();
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
