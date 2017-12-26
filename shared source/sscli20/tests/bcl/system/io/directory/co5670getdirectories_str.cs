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
public class Co5670GetDirectories_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetDirectories()";
	public static String s_strTFName        = "Co5670GetDirectories_str.cs";
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
			DirectoryInfo[] dirArr;
			if(Directory.Exists(dirName))
				Directory.Delete(dirName, true);
			strLoc = "Loc_477g8";
			iCountTestcases++;
			try {
				Directory.GetDirectories(null);
				iCountErrors++;
				printerr( "Error_2988b! Expected exception not thrown");
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_29087! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0707t! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_4yg7b";
			iCountTestcases++;
			try {
				Directory.GetDirectories(String.Empty);
				iCountErrors++;
				printerr( "Error_8ytbm! Expected exception not thrown");
			} catch (ArgumentException aexc) {
				printinfo( "Info_y687d! Caught expected, exc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2908y! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_1190x";
			iCountTestcases++;
			try {
				Directory.GetDirectories("*");
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
			dirArr = dir2.GetDirectories();
			iCountTestcases++;
			if(dirArr.Length != 0) {
				iCountErrors++;
				printerr("Error_207v7! Incorrect number of directories returned");
			}
			strLoc = "Loc_2398c";
			dir2.CreateSubdirectory("TestDir1");
			dir2.CreateSubdirectory("TestDir2");
			dir2.CreateSubdirectory("TestDir3");
			dir2.CreateSubdirectory("Test1Dir1");
			dir2.CreateSubdirectory("Test1Dir2");
			new FileInfo(dir2.ToString() + "TestFile1");
			new FileInfo(dir2.ToString() + "TestFile2");
			iCountTestcases++;
			dirArr = dir2.GetDirectories("TestDir*");
			iCountTestcases++;
			if(dirArr.Length != 3) {
				iCountErrors++;
				printerr( "Error_1yt75! Incorrect number of directories returned");
			}
			String[] names = new String[3];
			int i = 0 ;
			foreach (DirectoryInfo d in dirArr)
				names[i++] = d.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir1") < 0) {
				iCountErrors++;
				printerr( "Error_3y775! Incorrect name=="+dirArr[0].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_90885! Incorrect name=="+dirArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir3") < 0) {
				iCountErrors++;
				printerr( "Error_879by! Incorrect name=="+dirArr[2].Name);
			}
			dirArr = dir2.GetDirectories("*");
			iCountTestcases++;
			if(dirArr.Length != 5) {
				iCountErrors++;
				printerr( "Error_t5792! Incorrect number of directories=="+dirArr.Length);
			}
			names = new String[5];
			i = 0;
			foreach( DirectoryInfo d in dirArr)
				names[i++] = d.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir1") < 0) {
				iCountErrors++;
				printerr( "Error_4898v! Incorrect name=="+dirArr[0].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_4598c! Incorrect name=="+dirArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir1") < 0) {
				iCountErrors++;
				printerr( "Error_209d8! Incorrect name=="+dirArr[2].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_10vtu! Incorrect name=="+dirArr[3].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir3") < 0) {
				iCountErrors++;
				printerr( "Error_190vh! Incorrect name=="+dirArr[4].Name);
			}
			dirArr = dir2.GetDirectories("*Dir2");
			iCountTestcases++;
			if(dirArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_8019x! Incorrect number of directories=="+dirArr.Length);
			}
			names = new String[2];
			i = 0; 
			foreach ( DirectoryInfo d in dirArr) 
				names[i++] = d.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_167yb! Incorrect name=="+dirArr[0].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_49yb7! Incorrect name=="+dirArr[1].Name);
			}
			strLoc = "Loc_fy87e";
			dirArr = dir2.GetDirectories("Test1Dir2");
			iCountTestcases++;
			if(dirArr.Length != 1) {
				iCountErrors++;
				printerr( "Error_2398v! Incorrect number of directories returned, expected==1, got=="+dirArr.Length);
			}
			iCountTestcases++;
			if(!dirArr[0].Name.Equals("Test1Dir2")) {			
				iCountErrors++;
				printerr( "Error_88gbb! Incorrect directory returned=="+dirArr[0]);
			}
			strLoc = "Loc_48yg7";
			dirArr = dir2.GetDirectories("T*st*D*2");
			iCountTestcases++;
			if(dirArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_39g8b! Incorrect number of directories returned, expected==2, got=="+dirArr.Length);
			}
			dir2.CreateSubdirectory("AAABB");
			dir2.CreateSubdirectory("aaabbcc");
			dirArr = dir2.GetDirectories("*BB*");
			iCountTestcases++;
#if PLATFORM_UNIX // case-sensitive filesystem
                        if(dirArr.Length != 1) {
#else
			if(dirArr.Length != 2) {
#endif
				iCountErrors++;
				printerr( "Error_4y190! Incorrect number of directories=="+dirArr.Length);
			} 
			names = new String[2];
			i = 0;
			foreach ( DirectoryInfo d in dirArr )
				names[i++] = d.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "AAABB") < 0) {
				iCountErrors++;
				printerr( "Error_956yb! Incorrect name=="+dirArr[0]);
			} 
#if !PLATFORM_UNIX // case-sensitive filesystem
			iCountTestcases++;
			if(Array.IndexOf(names, "aaabbcc") < 0) {
				iCountErrors++;
				printerr( "Error_48yg7! Incorrect name=="+dirArr[1]);
			}
#endif //!PLATFORM_UNIX
			dirArr = dir2.GetDirectories("Directory");
			iCountTestcases++;
			if(dirArr.Length != 0) {
				iCountErrors++;
				printerr( "Error_209v7! Incorrect number of directories=="+dirArr.Length);
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
		Co5670GetDirectories_str cbA = new Co5670GetDirectories_str();
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
