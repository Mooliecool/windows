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
public class Co9034GetFileSystemEntries_str_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.GetFileSystemEntries()";
	public static String s_strTFName        = "Co9034GetFileSystemEntries_str_str.cs";
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
                        strLoc = "Loc_4y982";
                        iCountTestcases++;
			try {
				Directory.GetFileSystemEntries(null, "*");
				iCountErrors++;
				printerr( "Error_0002! Expected exception not thrown");
			} catch (ArgumentNullException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0003! Unexpected exceptiont thrown: "+exc.ToString());
			}
            iCountTestcases++;
            try {
                Directory.GetFileSystemEntries("", "*");
                iCountErrors++;
                printerr( "Error_0004! Expected exception not thrown");
            } catch (ArgumentException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
                iCountErrors++;
                printerr( "Error_0005! Unexpected exceptiont thrown: "+exc.ToString());
            }
            iCountTestcases++;
			try {
				Directory.GetFileSystemEntries(dirName , "*");
				iCountErrors++;
				printerr( "Error_1001! Expected exception not thrown");
			} catch (DirectoryNotFoundException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1002! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        iCountTestcases++;
			try {
				String strTempDir = "c:\\dls;d\\442349-0\\v443094(*)(+*$#$*\\\\\\";
                                Directory.GetFileSystemEntries(strTempDir , "*");
				iCountErrors++;
				printerr( "Error_1003! Expected exception not thrown");
			} catch (ArgumentException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1004! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        iCountTestcases++;
			try {
				String strTempDir = "             ";
                                Directory.GetFileSystemEntries(strTempDir , "*");
				iCountErrors++;
				printerr( "Error_1044! Expected exception not thrown");
			} catch (ArgumentException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_1023! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        dir2 = new DirectoryInfo( dirName );
                        dir2.Create();
                        iCountTestcases++;
			try {
				Directory.GetFileSystemEntries(dirName, null);
				iCountErrors++;
				printerr( "Error_02002! Expected exception not thrown");
			} catch (ArgumentNullException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_00023! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        iCountTestcases++;
			try {
				String[] strInfos = Directory.GetFileSystemEntries(dirName, "");
				if( strInfos.Length != 0){ 
                    iCountErrors++;
    				printerr( "Error_9004! Invalid number of directories returned");
                }
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_9005! Unexpected exceptiont thrown: "+exc.ToString());
			}                         
                        iCountTestcases++;
			try {
				String strTempDir = "c:\\dls;d\\442349-0\\v443094(*)(+*$#$*\\\\\\";
                                Directory.GetFileSystemEntries(dirName , strTempDir);
				iCountErrors++;
				printerr( "Error_3003! Expected exception not thrown");
			} catch (ArgumentException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_3004! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        iCountTestcases++;
			try {
				String strTempDir = "a..b abc..d";
                                Directory.GetFileSystemEntries(dirName , strTempDir);  
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_4004! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        iCountTestcases++;
			try {
				String strTempDir = "..ab ab.. .. abc..d\abc.." ;
                                Directory.GetFileSystemEntries(dirName , strTempDir);
				iCountErrors++;
				printerr( "Error_144003! Expected exception not thrown");
			} catch (ArgumentException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;     
				printerr( "Error_10404! Unexpected exceptiont thrown: "+exc.ToString());
			} 
 			strLoc = "Loc_0001";
                        iCountTestcases++;
			try {
                                strArr = Directory.GetFileSystemEntries ("Directory" , "*");
				iCountErrors++;
				printerr( "Error_14443! Expected exception not thrown");
			} catch (IOException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;     
				printerr( "Error_10433! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        iCountTestcases++;
			try {
#if PLATFORM_UNIX
                                String strTempDir = "///////";
#else
				String strTempDir = "c:\\\\\\\\\\";
#endif
                                strArr = Directory.GetFileSystemEntries(strTempDir , "*");
                                if ( strArr == null || strArr.Length == 0 )
                                {
                                    printerr("Error_1234!!! INvalid number of file system entries count :: " + strArr.Length );
                                    iCountErrors++ ;    
                                }
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_10406! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        iCountTestcases++;
			try {
                                strArr = Directory.GetFileSystemEntries(s_strTFPath , "*");
                                if ( strArr == null || strArr.Length == 0 )
                                {
                                    printerr("Error_2434!!! INvalid number of file system entries count :: " + strArr.Length );
                                    iCountErrors++ ;    
                                }    
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_12321!!! Unexpected exceptiont thrown: "+exc.ToString());
			} 
                        strArr = Directory.GetFileSystemEntries ( dirName , "*");
			iCountTestcases++;
			if(strArr.Length != 0) {
				iCountErrors++;
				printerr("Error_207v7! Incorrect number of directories returned");
			}
			strLoc = "Loc_2398c";
			dir2.CreateSubdirectory("TestDir1");
			dir2.CreateSubdirectory("TestDir2");
			dir2.CreateSubdirectory("TestDir3");
                        Console.WriteLine( dir2.ToString() );
			FileStream fs1 = new FileInfo(dir2.ToString() + "\\" + "TestFile1").Create();
			FileStream fs2 = new FileInfo(dir2.ToString() + "\\" + "TestFile2").Create();
			FileStream fs3 = new FileInfo(dir2.ToString() + "\\" + "Test1File2").Create();
			FileStream fs4 = new FileInfo(dir2.ToString() + "\\" + "Test1Dir2").Create();
                        strArr = Directory.GetFileSystemEntries(dirName , "           ");
			iCountTestcases++;
			if(strArr.Length != 0) {
				iCountErrors++;
				printerr( "Error_24324! Incorrect number of directories returned" + strArr.Length);
			}
                        strArr = Directory.GetFileSystemEntries(dirName , "Test1*");
			iCountTestcases++;
			if(strArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_24343! Incorrect number of directories returned" + strArr.Length);
			}
			strArr = Directory.GetFileSystemEntries ( dir2.Name , "*");
			iCountTestcases++;
			if(strArr.Length != 7) {
				iCountErrors++;
				printerr( "Error_1yt75! Incorrect number of directories returned" + strArr.Length);
			}
                        for(int iLoop = 0 ; iLoop < strArr.Length ; iLoop++)
                                strArr[iLoop] = strArr[iLoop].Substring( strArr[iLoop].IndexOf(Path.DirectorySeparatorChar) + 1 ); 
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestDir1") < 0) {
				iCountErrors++;
				printerr( "Error_4yg76! Incorrect name=="+strArr[0]);
			}
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_1987y! Incorrect name=="+strArr[1]);
			}
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestDir3") < 0) {
				iCountErrors++;
				printerr( "Error_4yt76! Incorrect name=="+strArr[2]);
			}
			iCountTestcases++;
			if(Array.IndexOf(strArr, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_3y775! Incorrect name=="+strArr[3]);
			}
			iCountTestcases++;
			if(Array.IndexOf(strArr, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_90885! Incorrect name=="+strArr[4]);
			}
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestFile1") < 0) {
				iCountErrors++;
				printerr( "Error_879by! Incorrect name=="+strArr[5]);
			}
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_29894! Incorrect name=="+strArr[6]);
			}
                        fs1.Close();
                        fs2.Close();
                        fs3.Close();
                        fs4.Close();                                
			strArr = Directory.GetFileSystemEntries (dir2.Name , "*2");
			iCountTestcases++;
			if(strArr.Length != 4) {
				iCountErrors++;
				printerr( "Error_8019x! Incorrect number of files=="+strArr.Length);
			}
                        for(int iLoop = 0 ; iLoop < strArr.Length ; iLoop++)
                                strArr[iLoop] = strArr[iLoop].Substring( strArr[iLoop].IndexOf(Path.DirectorySeparatorChar) + 1 ); 
			iCountTestcases++;
			if(Array.IndexOf(strArr, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_247yg! Incorrect name=="+strArr[0]);
			} 
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_24gy7! Incorrect name=="+strArr[1]);
			}
			iCountTestcases++;
			if(Array.IndexOf(strArr, "Test1File2") < 0) {
				iCountErrors++;
				printerr( "Error_167yb! Incorrect name=="+strArr[2]);
			} 
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestFile2") < 0) {
				iCountErrors++;
				printerr( "Error_49yb7! Incorrect name=="+strArr[3]);
			}
			strArr = Directory.GetFileSystemEntries (dir2.Name , "*Dir2");
			iCountTestcases++;
			if(strArr.Length != 2) {
				iCountErrors++;
				printerr( "Error_948yv! Incorrect number of files=="+strArr.Length);
			}
                        for(int iLoop = 0 ; iLoop < strArr.Length ; iLoop++)
                                strArr[iLoop] = strArr[iLoop].Substring( strArr[iLoop].IndexOf(Path.DirectorySeparatorChar) + 1 ); 
                        iCountTestcases++;
			if(Array.IndexOf(strArr, "Test1Dir2") < 0) {
				iCountErrors++;
				printerr( "Error_247yg! Incorrect name=="+strArr[0]);
			} 
			iCountTestcases++;
			if(Array.IndexOf(strArr, "TestDir2") < 0) {
				iCountErrors++;
				printerr( "Error_24gy7! Incorrect name=="+strArr[1]);
			}
			new FileInfo(dir2.FullName + "\\"  +"AAABB").Create();
			Directory.CreateDirectory(dir2.FullName + "\\" +"aaabbcc");
			strArr = Directory.GetFileSystemEntries (dir2.Name , "*BB*");
                        iCountTestcases++;
#if PLATFORM_UNIX // case-sensitive filesystem
                        if(strArr.Length != 1) {
#else
			if(strArr.Length != 2) {
#endif
				iCountErrors++;
				printerr( "Error_4y190! Incorrect number of files=="+strArr.Length);
			} 
                        for(int iLoop = 0 ; iLoop < strArr.Length ; iLoop++)
                                strArr[iLoop] = strArr[iLoop].Substring( strArr[iLoop].IndexOf(Path.DirectorySeparatorChar) + 1 ); 
#if !PLATFORM_UNIX // case-sensitive filesystem
                        iCountTestcases++;
			if(Array.IndexOf(strArr, "aaabbcc") < 0) {
				iCountErrors++;
				printerr( "Error_956yb! Incorrect name=="+strArr[0]);
			} 
#endif
			iCountTestcases++;
			if(Array.IndexOf(strArr, "AAABB") < 0) {
				iCountErrors++;
				printerr( "Error_48yg7! Incorrect name=="+strArr[1]);
			}
 			strLoc = "Loc_2301";
			FileInfo f = new FileInfo(dir2.Name + "\\"  +"TestDir1\\Test.tmp");
                        FileStream fs6 = f.Create();
			strArr = Directory.GetFileSystemEntries (dir2.Name, "TestDir1\\*");
			iCountTestcases++;
			if(strArr.Length != 1) {
				iCountErrors++;
				printerr( "Error_28gyb! Incorrect number of files");
			} 
                        fs6.Close();
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
		Co9034GetFileSystemEntries_str_str cbA = new Co9034GetFileSystemEntries_str_str();
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
