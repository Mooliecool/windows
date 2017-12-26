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
public class Co9005GetLastAccessTime_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.GetLastAccessTime()";
	public static String s_strTFName        = "co9005getlastaccesstime_str.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_0001";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		try
		{
			String filName = s_strTFAbbrev+"TestFile";			
			FileInfo fil2;
			Stream stream;
			if(File.Exists(filName))
				File.Delete(filName);
                        iCountTestcases++;
			try {
				DateTime dt = File.GetLastAccessTime(null);
				iCountErrors++;
				printerr( "Error_0002! Expected exception not thrown");
			} catch (ArgumentNullException exc){
                                printinfo("Expected exception thrown" + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0003! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        iCountTestcases++;
			try {
				DateTime dt = File.GetLastAccessTime("");
				iCountErrors++;
				printerr( "Error_0004! Expected exception not thrown");
			} catch (ArgumentException exc){
                                printinfo("Expected exception thrown" + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0005! Unexpected exceptiont thrown: "+exc.ToString());
			}
			strLoc = "Loc_0006";
			FileStream fs = new FileStream(filName, FileMode.Create);
                        fs.Close();
			fil2 = new FileInfo(filName);
			stream = fil2.OpenWrite();
			stream.Write(new Byte[]{10}, 0, 1);
			stream.Close();
			Thread.Sleep(4000);
			fil2.Refresh();
			iCountTestcases++;
			try {
				DateTime d1 = fil2.LastAccessTime ;
                DateTime d2 = DateTime.Today ; 
                if( d1.Year != d2.Year || d1.Month != d2.Month || d1.Day != d2.Day) {
					iCountErrors++;
					printerr( "Error_0007! Creation time cannot be correct.. Expected:::" + DateTime.Today.ToString() + ", Actual:::" + fil2.LastAccessTime.ToString());
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0008! Unexpected exceptiont thrown: "+exc.ToString());
			}
			strLoc = "Loc_0009";
			iCountTestcases++;
			try {
                                if( (DateTime.Today - File.GetLastAccessTime(s_strTFName) ).Seconds > 60 ) {
					iCountErrors++;
					printerr( "Eror_0010! LastAccess time is not correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0011! Unexpected exception thrown: "+exc.ToString());
			} 
			fil2.Delete();
			if(File.Exists(filName))
				File.Delete(filName);
		} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_0012!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
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
		Co9005GetLastAccessTime_str cbA = new Co9005GetLastAccessTime_str();
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
