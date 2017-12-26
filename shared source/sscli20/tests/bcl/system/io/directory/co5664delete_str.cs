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
public class Co5664Delete_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Directory.Delete(String)";
	public static String s_strTFName        = "Co5664Delete_str.cs";
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
			strLoc = "Loc_948yg";
			dir2 = new DirectoryInfo(".");
			iCountTestcases++;
			try {
				Directory.Delete(".");
				iCountErrors++;
				printerr( "Error_48y7b! Expected exception not thrown");
			} catch (IOException iexc) {
				    printinfo( "Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_2019c! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_9828v";
		  	dir2 = new DirectoryInfo("ThisDoesNotExist");
			iCountTestcases++;
			try {
				Directory.Delete("ThisDoesNotExist");
				iCountErrors++;
				printerr( "Error_9138v! Expected exception not thrown");
			} catch (DirectoryNotFoundException iexc) {
				printinfo( "Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_799tb! Incorrect exception thrown, exc=="+exc.ToString());
			}
			strLoc = "Loc_498f8";
	   		dir2 = Directory.CreateDirectory(dirName);
			Directory.Delete(dir2.Name);
			if(Directory.Exists(dirName)) {
				iCountErrors++;
				printerr( "Error_987g7! Directory not deleted");
			}
#if !PLATFORM_UNIX // tests case-insensitivity in the filesystem
			strLoc = "Loc_48yv8";
			Directory.CreateDirectory(dirName+"\\Test1");
			dir2 = Directory.CreateDirectory(dirName+"\\Test2");
			Directory.Delete(dir2.FullName.ToUpper(CultureInfo.InvariantCulture));
			iCountTestcases++;
			if(Directory.Exists(dirName+"\\Test2")) {
				iCountErrors++;
				printerr( "Error_49928! Directory not deleted");
			}
			Directory.CreateDirectory(dirName+"\\Test2");
			dir2 = new DirectoryInfo(dirName);
			iCountTestcases++;
			try {
				Directory.Delete(dir2.FullName);
				iCountErrors++;
				printerr( "Error_5y78b! Expected exception not thrown");
			} catch (IOException iexc) {
				printinfo( "Info_29087! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_98yg7! Incorrect exception thrown, exc=="+exc.ToString());
			}
			Directory.Delete(dir2.Name, true);
			iCountTestcases++;
			if(Directory.Exists(dirName)) {
				iCountErrors++;
				printerr( "Error_917ct! Directory not deleted");
			}			   
			strLoc = "Loc_u9uf8";
			Directory.CreateDirectory(dirName+"\\Test1");
			Directory.CreateDirectory(dirName+"\\Test2");
			new FileStream(dirName+"\\Test1\\Hello.tmp", FileMode.Create).Close();
			iCountTestcases++;
			dir2 = new DirectoryInfo(dirName);
			try {
				Directory.Delete(dir2.Name);
				iCountErrors++;
				printerr( "Error_241y7! Expected exception not thrown");
			} catch (IOException iexc) {
				printinfo("Info_9288b! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_07509! Incorrect exception thrown, exc=="+exc.ToString());
			}
			Directory.Delete(dir2.FullName.ToLower(CultureInfo.InvariantCulture), true);
			if(Directory.Exists(dirName)) {
				iCountErrors++;
				printerr( "Error_26y7b! Directory not deleted");
			}
#endif //!PLATFORM_UNIX
			strLoc = "Loc_ty7gy";
			Directory.CreateDirectory(dirName);
			FileStream fs = new FileStream(dirName+"\\Test.tmp", FileMode.Create);
			dir2 = new DirectoryInfo(dirName);
			iCountTestcases++;
			try {
				Directory.Delete(dir2.Name);
				iCountErrors++;
				printerr( "Error_ty7b7! Expected exception not thrown");
			} catch (IOException iexc) {
				printinfo("Info_5918c! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_4919o! Incorrect exception thrown, exc=="+exc.ToString());
			}
			fs.Close();
			Directory.Delete(dir2.Name, true);
			strLoc = "Loc_928yg";
			dir2 = Directory.CreateDirectory(Environment.CurrentDirectory + dirName);
			dir2.Attributes = FileAttributes.ReadOnly;
			iCountTestcases++;
			try {
				Directory.Delete(dir2.Name);
				iCountErrors++;
				printerr( "Error_t01uv! Expected exception not thrown");
			} catch (IOException iexc) {
				printinfo("Info_80399! Caught expected exception, iexc=="+iexc.Message);
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_198yv! Incorrect exception thrown, exc=="+exc.ToString());
			}
			dir2.Attributes = new FileAttributes();
			dir2.Delete(true);
			dir2 = Directory.CreateDirectory(Environment.CurrentDirectory + dirName);
			dir2.Attributes = FileAttributes.Hidden;
			iCountTestcases++;
			Directory.Delete(dir2.FullName);
			if(Directory.Exists(dirName)) {
				iCountErrors++;
				printerr( "Error_948ug Directory not deleted");
			} 
			if(Directory.Exists(dirName)) {
				new DirectoryInfo(dirName).Attributes = new FileAttributes();			   
				Directory.Delete(dirName, true);
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
		Co5664Delete_str cbA = new Co5664Delete_str();
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
