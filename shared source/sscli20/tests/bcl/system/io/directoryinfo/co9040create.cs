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
using System.Text;
using System.IO;
using System.Collections;
using System.Globalization; 
public class Co9040Create
{
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "DirectoryInfo.Create()";
	public static String s_strTFName        = "Co9040Create.cs";
	public static String s_strTFAbbrev      = "Co9040";
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
                String strDirName = "" ;
		try
		{
                        DirectoryInfo dir = null;
			strLoc = "Loc_099u8";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".");
                dir.Create();
                if ( dir.FullName != s_strTFPath ){
				    iCountErrors++;
				    printerr( "Error_298dy! Incorrect directory name, dir=="+dir.FullName);
                }    
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_209xj! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_098gt";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo("..\\..\\");
                dir.Create();
                Console.WriteLine(  GetParentDirectory( s_strTFPath , 2) );
                if ( dir.FullName != GetParentDirectory( s_strTFPath , 2) ){
				    iCountErrors++;
				    printerr( "Error_7987! Incorrect directory name, dir=="+dir.FullName);
                }                 
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_9092c! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_098gt";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo("\t\t\t\t");
                                Console.WriteLine( dir.Name );
                                dir.Create();
				iCountErrors++;
				printerr( "Error_rerec! Expected exception not thrown, dir=="+dir.FullName);
				dir.Delete(true);
			} catch ( ArgumentException aexc ) {
				printinfo( "Info_sddd! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_rerc! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_498vy";
			iCountTestcases++;
			try {
                                strDirName =  "Testing";
                                dir = new DirectoryInfo(strDirName);
                                dir.Create();
                                if ( dir.Name != strDirName  )
                                {
                                        iCountErrors++;
                                        printerr("Error_0010! Unexpected directory name :: " + dir.Name );
                                }  
                                dir.Delete();      
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_28829! Unexpected exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_209xc";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(s_strTFPath ) ;
                dir.Create();
                if ( dir.FullName != s_strTFPath ){
				    iCountErrors++;
				    printerr( "Error_6950! Incorrect directory name, dir=="+dir.FullName);
                }
            } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_6566! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_09t83";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo("..\\");
                dir.Create();
                Console.WriteLine("Directory name... " + GetParentDirectory( s_strTFPath , 1 ) );
                String dirName = dir.FullName ;
                if ( dirName  != GetParentDirectory( s_strTFPath , 1 ) ) {
                    iCountErrors++;
                    printerr( "Error_5435! Parent directory name is incorrect, dir=="+dir.FullName);
                }    
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0980! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_2908y";
			StringBuilder sb = new StringBuilder();
			int current = Environment.CurrentDirectory.Length;
			int templength = 260-current-1;
                        sb.Append('a', templength);
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(sb.ToString() );
                                dir.Create();
				dir.Delete(true);
				iCountErrors++;
				printerr( "Error_109ty! Expected exception not thrown, dir=="+dir.FullName);
			} catch (PathTooLongException aexc) {
				printinfo( "Error_29010! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_109dv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_298yg";
			sb = new StringBuilder();
			current = Environment.CurrentDirectory.Length;
			templength = 260-current-5;
                        sb.Append('a', templength);
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(sb.ToString());
                                dir.Create();
				if(!dir.FullName.Equals(Environment.CurrentDirectory+"\\"+sb.ToString())) {
					iCountErrors++;
					printerr( "Error_18rt7! Incorrect directory name");
				} 
			} catch (PathTooLongException pexc) {
				printinfo("Info_92yg7! Caught expected exception, aexc=="+pexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_rt87y! Unexpected exception, exc=="+exc.ToString());
			}
			strLoc = "Loc_87yg7";
			iCountTestcases++;
			dir = new DirectoryInfo("!@#$%^&");
                        dir.Create();
                        if(!dir.FullName.Equals(Environment.CurrentDirectory+Path.DirectorySeparatorChar+"!@#$%^&")) {
				iCountErrors++;
				printerr( "Error_0109x! Incorrect directory name, dir=="+dir.FullName);
			}
			dir.Delete(true);
			strLoc = "Loc_209ud";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(s_strTFPath + "\\" + "Test\\Test\\Test");
                                Console.WriteLine( dir.FullName );
                                dir.Create();
				if ( dir.FullName.IndexOf("Test"+Path.DirectorySeparatorChar+"Test"+Path.DirectorySeparatorChar+"Test")  == -1 )
                                {
                                        iCountErrors++;
                                        printerr("Error_0010! Unexpected directory name :: " + dir.FullName );
                                } 			
			} catch ( Exception exc ) {
				iCountErrors++;
				printerr( "Error_2019u! Incorrect exception thrown, exc=="+exc.ToString());
			}
#if !PLATFORM_UNIX
			strLoc = "Loc_2089x";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(":");
                                dir.Create();
                                iCountErrors++;
				printerr( "Error_19883! Expected exception not thrown, dir=="+dir.FullName);
				dir.Delete(true);
			} catch ( ArgumentException ) {
			} catch ( Exception exc ) {
				iCountErrors++;
				printerr( "Error_0198xu! Incorrect exception thrown, exc=="+exc.ToString());
			}
#endif //!PLATFORM_UNIX
			strLoc = "Loc_38yc9";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo("..\\TestDir");
                                dir.Create();
				dir.Delete(true);
			} catch ( Exception exc) {
				iCountErrors++;
				printerr( "Error_498yv! Incorrect exception thrown, exc=="+exc.ToString());
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
			Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString() );
			return false;
		}
	}
    private String GetParentDirectory(String strDirName , int iNumUpFolder ){
        int iIndex = 0 ;
        while ( iNumUpFolder != 0){          
            if ( strDirName.LastIndexOf(@"\")  > 0 || strDirName.LastIndexOf(@"/") > 0 ){
                  iIndex = Math.Max(strDirName.LastIndexOf(@"\"),
                                    strDirName.LastIndexOf(@"/"));
                  if ( iIndex != -1 )
                    strDirName = strDirName.Substring( 0, iIndex ); 
            }  
            iNumUpFolder-- ;   
        }
            return strDirName + Path.DirectorySeparatorChar;
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
		Co9040Create cbA = new Co9040Create();
		try {
			bResult = cbA.runTest();
		} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
		}
		if (!bResult)
		{
			Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
			Console.WriteLine( " " );
			Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
			Console.WriteLine( " " );
                }
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
