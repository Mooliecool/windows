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
public class Co5675GetDirectories_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetDirectories()";
	public static String s_strTFName        = "Co5675GetDirectories_str.cs";
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
			dir2 = new DirectoryInfo(".");
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
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				Directory.GetDirectories("\n");
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
			strArr = Directory.GetDirectories(dirName);
			iCountTestcases++;
			if(strArr.Length != 0) {
				iCountErrors++;
				printerr("Error_207v7! Incorrect number of directories returned");
			}
			strLoc = "Loc_2398c";
			dir2.CreateSubdirectory("TestDir1");
			dir2.CreateSubdirectory("TestDir2");
			dir2.CreateSubdirectory("TestDir3");
			dir2.CreateSubdirectory("Test1Dir1");
			dir2.CreateSubdirectory("Test1Dir2");
            FileStream[] fs = new FileStream[2];
			fs[0] = new FileInfo(dir2.FullName + "\\"  +"TestFile1").Create();
			fs[1] = new FileInfo(dir2.FullName + "\\"  +"TestFile2").Create();
			for( int iLoop = 0 ; iLoop < 2 ; iLoop++ )
                fs[iLoop].Close();			
			strLoc = "Loc_249yv";
			strArr = Directory.GetDirectories(Environment.CurrentDirectory+"\\"+dirName);
			iCountTestcases++;
			if(strArr.Length != 5) {
				iCountErrors++;
				printerr( "Error_t5792! Incorrect number of directories=="+strArr.Length);
			}
			String[] names = new String[strArr.Length];
			int i = 0;
			foreach ( String f in strArr ) {
                String file = f.Replace("/", "\\");
				names[i++] =  file.Substring( file.LastIndexOf("\\" ) + 1);
            }
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir1") < 0) {
				iCountErrors++;
				printerr( "Error_4898v! Incorrect name=="+strArr[0].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_4598c! Incorrect name=="+strArr[1].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir1") < 0) {
				iCountErrors++;
				printerr( "Error_209d8! Incorrect name=="+strArr[2].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_10vtu! Incorrect name=="+strArr[3].ToString());
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir3") < 0) {
				iCountErrors++;
				printerr( "Error_190vh! Incorrect name=="+strArr[4].ToString());
			}
			Directory.Delete(dirName+"\\TestDir2");
			Directory.Delete(dirName+"\\TestDir3");
			strArr = Directory.GetDirectories(".\\"+dirName);
			iCountTestcases++;
			if(strArr.Length != 3) {
				iCountErrors++;
				printerr( "Error_0989b! Incorrec tnumber of directories=="+strArr.Length);
			} 
			names = new String[strArr.Length];
			i = 0;
			foreach( String f in strArr ) {
                String file = f.Replace("/", "\\");
				names[i++] = file.Substring( file.LastIndexOf("\\" ) + 1);
            }
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir1") < 0) {
				iCountErrors++;
				printerr( "Error_10938! Incorrect name=="+strArr[0]);
			} 
			iCountTestcases++;
			if(Array.IndexOf(names, "Test1Dir2")  < 0) {
				iCountErrors++;
				printerr( "Error_01c99! Incorrect name=="+strArr[1].ToString());
			}
			iCountTestcases++;
			if(Array.IndexOf(names, "TestDir1") < 0) {
				iCountErrors++;
				printerr( "Error_176y7! Incorrect name=="+strArr[2].ToString());
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
		Co5675GetDirectories_str cbA = new Co5675GetDirectories_str();
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
