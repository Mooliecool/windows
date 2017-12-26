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
public class Co5513CreateSubdirectory_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.CreateSubdirectory(String)";
	public static String s_strTFName        = "Co5516CreateSubdirectory_str.cs";
	public static String s_strTFAbbrev      = "Co5516";
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
			DirectoryInfo dir = null;
			strLoc = "Loc_099u8";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory(null);
				iCountErrors++;
				printerr( "Error_298dy! Expected exception not thrown, dir=="+dir.FullName);
				dir.Delete(true);
			} catch (ArgumentNullException aexc) {
				printinfo( "Info_298yt! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_209xj! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_098gt";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory(String.Empty);
				iCountErrors++;
				printerr( "Error_3987c! Expected exception not thrown, dir=="+dir.FullName);
				dir.Delete(true);
			} catch ( ArgumentException aexc ) {
				printinfo( "Info_89t7g! Caught expected exception, aexc=="+aexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_9092c! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_498vy";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory("         ");
				if ( dir.FullName.IndexOf( s_strTFPath ) == -1  )
                {
                    iCountErrors++;
                    printerr("Error_0010! Unexpected directory name :: " + dir.FullName );
                }    
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_28829! Unexpected exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_98399";
            iCountTestcases++;
			try {
                dir = new DirectoryInfo(".").CreateSubdirectory("\t");
                    iCountErrors++;
                    printerr("Error_2091c! Unexpected directory name :: " + dir.FullName );
                }  
		catch (ArgumentException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_8374v! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_209xc";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory(".");
				if ( dir.FullName != s_strTFPath )
                                {
                                        iCountErrors++;
                                        printerr("Error_0012! Unexpected directory name :: " + dir.FullName );
                                } 
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_12898! Unexpected exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_09t83";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory("..");
				if ( s_strTFPath.IndexOf( dir.FullName ) == -1 )
                {
                        iCountErrors++;
                        printerr("Error_0010! Unexpected directory name :: " + dir.FullName );
                } 	 
            }catch (ArgumentException e){
				printinfo( "Error_9788! Expected exception occured, exc==" +e.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0199xh! Incorrect exception thrown, exc=="+exc.ToString());
			} 
			strLoc = "Loc_2908y";
			StringBuilder sb = new StringBuilder();
			int current = Environment.CurrentDirectory.Length;
			int templength = 260-current-1;
			for(int i = 0 ; i < templength ; i++)
				sb.Append("a");
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory(sb.ToString());
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
			for(int i = 0 ; i < templength; i++)
				sb.Append("a");
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory(sb.ToString());
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
			dir = new DirectoryInfo(".").CreateSubdirectory("!@#$%^&");
			if(!dir.FullName.Equals(Environment.CurrentDirectory+Path.DirectorySeparatorChar+"!@#$%^&")) {
				iCountErrors++;
				printerr( "Error_0109x! Incorrect directory name, dir=="+dir.FullName);
			}
			dir.Delete(true);
			strLoc = "Loc_8t9dy";
			dir = new DirectoryInfo(".").CreateSubdirectory("TestDirectory");
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(Environment.CurrentDirectory+"\\TestDirectory");
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2019u! Correct directory not created, exc=="+exc);
			}
			dir.Delete(true);
			strLoc = "Loc_209ud";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory("Test\\Test\\Test");
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
				dir = new DirectoryInfo(".").CreateSubdirectory(":");
				iCountErrors++;
				printerr( "Error_19883! Expected exception not thrown, dir=="+dir.FullName);
				dir.Delete(true);
			} catch ( NotSupportedException ) {
			} catch ( Exception exc ) {
				iCountErrors++;
				printerr( "Error_0198xu! Incorrect exception thrown, exc=="+exc.ToString());
			}
#endif //!PLATFORM_UNIX
			strLoc = "Loc_38yc9";
			iCountTestcases++;
			try {
				dir = new DirectoryInfo(".").CreateSubdirectory("..\\TestDir");
				iCountErrors++;
				printerr( "Error_38yf8! Expected exception not thrown, dir=="+dir.FullName);
				dir.Delete(true);
			} catch (ArgumentException aexc ) {
				printinfo( "Info_38fy8! Caught expected exception, aexc=="+aexc.Message);
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
		Co5513CreateSubdirectory_str cbA = new Co5513CreateSubdirectory_str();
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
