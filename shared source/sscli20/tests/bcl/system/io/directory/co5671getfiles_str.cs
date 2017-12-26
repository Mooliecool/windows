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
public class Co5671GetFiles_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetFiles()";
	public static String s_strTFName        = "Co5671GetFiles_str.cs";
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
			FileInfo[] filArr;
			if(Directory.Exists(dirName))
				Directory.Delete(dirName, true);
			strLoc = "Loc_477g8";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				dir2.GetFiles(null);
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
				FileInfo[] fInfos = dir2.GetFiles(String.Empty);
				if( fInfos.Length != 0){ 
                    iCountErrors++;
    				printerr( "Error_8ytbm! Invalid number of file infos are returned");
                }
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2908y! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_1190x";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
                FileInfo[] strFiles = dir2.GetFiles("\n");
                if( strFiles.Length != 0){
    				iCountErrors++;
    				printerr( "Error_2198y!Unexpected files retrieved..");
                }
            } catch ( IOException e) {
                printinfo("Expected exception occured... " + e.Message );
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_17888! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_4y982";
			dir2 = Directory.CreateDirectory(dirName);
			filArr = dir2.GetFiles();
			iCountTestcases++;
			if(filArr.Length != 0) {
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
			new FileInfo(dir2.FullName +"\\" + "TestFile2").Create();
			new FileInfo(dir2.FullName +"\\" + "TestFile3").Create();
			new FileInfo(dir2.FullName +"\\" + "Test1File1").Create();
			new FileInfo(dir2.FullName +"\\" + "Test1File2").Create();
			iCountTestcases++;
			filArr = dir2.GetFiles("TestFile*");
			iCountTestcases++;
			if(filArr.Length != 3) {
				iCountErrors++;
				printerr( "Error_1yt75! Incorrect number of files returned" + filArr.Length);
			}
			String[] names = new String[3];
			int i = 0;
			foreach ( FileInfo f in filArr)
				names[i++] = f.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile1") < 0) {
				iCountErrors++;
				printerr( "Error_3y775! Incorrect name=="+filArr[0].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_90885! Incorrect name=="+filArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile3") < 0) {
				iCountErrors++;
				printerr( "Error_879by! Incorrect name=="+filArr[2].Name);
			}
			filArr = dir2.GetFiles("*");
			iCountTestcases++;
			if(filArr.Length != 5) {
				iCountErrors++;
				printerr( "Error_t5792! Incorrect number of files=="+filArr.Length);
			}
			names = new String[5];
			i = 0;
			foreach( FileInfo f in filArr)
				names[i++] = f.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File1") < 0) {
				iCountErrors++;
				printerr( "Error_4898v! Incorrect name=="+filArr[0].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_4598c! Incorrect name=="+filArr[1].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile1") < 0) {
				iCountErrors++;
				printerr( "Error_209d8! Incorrect name=="+filArr[2].Name);
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_10vtu! Incorrect name=="+filArr[3].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile3") < 0) {
				iCountErrors++;
				printerr( "Error_190vh! Incorrect name=="+filArr[4].Name);
			}
			filArr = dir2.GetFiles("*File2");
			iCountTestcases++;
			if(filArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_8019x! Incorrect number of files=="+filArr.Length);
			}
			names = new String[2];
			i = 0;
			foreach (FileInfo f in filArr)
				names[i++] = f.Name;
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_167yb! Incorrect name=="+filArr[0].Name);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_49yb7! Incorrect name=="+filArr[1].Name);
			}
			strLoc = "Loc_9438y";
			filArr = dir2.GetFiles("*es*F*l*");
			iCountTestcases++;
			if(filArr.Length != 5) {
				iCountErrors++;
				printerr( "Error_38fy3! Incorrect number of files returned, expected==5, got=="+filArr.Length);
			}
            dir2 = new DirectoryInfo(".");
			FileInfo fi1 = new FileInfo("AAABB");
			FileInfo fi2 = new FileInfo("aaabbcc");
            FileStream fs1 = fi1.Create();
            FileStream fs2 = fi2.Create();
			fs1.Close();
            fs2.Close();
			filArr = dir2.GetFiles("*BB*");
			iCountTestcases++;
#if PLATFORM_UNIX // case-sensitive filesystem
                        if(filArr.Length != 1) {
#else
			if(filArr.Length != 2) {
#endif
				iCountErrors++;
				printerr( "Error_4y190! Incorrect number of files=="+filArr.Length);
			} 
			names = new String[2];
			i = 0;
			foreach (FileInfo f in filArr) {
				names[i++] = f.Name;   
            }
			iCountTestcases++;
			if(Array.IndexOf(names, "AAABB") < 0) {
				iCountErrors++;
				printerr( "Error_956yb! Incorrect name=="+filArr[0].Name);
			} 
#if !PLATFORM_UNIX // case-sensitive filesystem
			iCountTestcases++;
			if(Array.IndexOf(names, "aaabbcc") < 0) {
				iCountErrors++;
				printerr( "Error_48yg7! Incorrect name=="+filArr[1].Name);
			}
#endif
			strLoc = "Loc_3y8cc";
			filArr = dir2.GetFiles("AAABB");
			iCountTestcases++;
			if(filArr.Length != 1) {
				iCountErrors++;
				printerr( "Error_398s1! Incorrect number of files=="+filArr.Length);
			}
			iCountTestcases++;
			if(!filArr[0].Name.Equals("AAABB")) {
				iCountErrors++;
				printerr( "Error_3298y! Incorrect file name=="+filArr[0].Name);
			}
            strLoc = "Loc_5435";
			filArr = dir2.GetFiles("Directory");
			iCountTestcases++;
			if(filArr.Length != 0) {
				iCountErrors++;
				printerr( "Error_209v7! Incorrect number of files=="+filArr.Length);
			}
			new DirectoryInfo("TestDir1" ).Create();
            FileStream fs = new FileInfo("TestDir1\\Test.tmp").Create();
			fs.Close();
            filArr = dir2.GetFiles("TestDir1\\*");
			iCountTestcases++;
			if(filArr.Length != 1) {
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
		Co5671GetFiles_str cbA = new Co5671GetFiles_str();
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
