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
public class Co9007GetLastWriteTime_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.GetLastWriteTime()";
	public static String s_strTFName        = "co9007getlastwritetime_str.cs";
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
			String fileName = s_strTFAbbrev+"TestFile";			
			FileInfo file2;
			Stream stream;
			if(File.Exists(fileName))
				File.Delete(fileName);
                        iCountTestcases++;
			try {
				DateTime dt = File.GetLastWriteTime(null);
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
				DateTime dt = File.GetLastWriteTime("");
				iCountErrors++;
				printerr( "Error_0004! Expected exception not thrown");
			} catch (ArgumentException exc){
                                printinfo("Expected exception thrown :: " + exc.Message);      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0005! Unexpected exceptiont thrown: "+exc.ToString());
			}
			strLoc = "Loc_0006";
			file2 = new FileInfo(fileName);
            FileStream fs = file2.Create() ;
			fs.Write(new Byte[]{10}, 0, 1);
			fs.Close();
            double lMilliSecs = (DateTime.Now-File.GetLastWriteTime( fileName )).TotalMilliseconds ;
            Console.WriteLine("Millis.. " + lMilliSecs );
			iCountTestcases++;
            Console.WriteLine("FileName ... " + (DateTime.Now - File.GetLastWriteTime( fileName )).Minutes );
            Console.WriteLine( DateTime.Now );
            Console.WriteLine( File.GetLastWriteTime( fileName ) );
			if((DateTime.Now - File.GetLastWriteTime( fileName )).Minutes > 6 ) {
				iCountErrors++;
				Console.WriteLine((DateTime.Now-File.GetLastWriteTime( fileName )).TotalMilliseconds);
				printerr( "Error_0007! Last Write Time time cannot be correct");
			}
			strLoc = "Loc_0008";
			stream = file2.Open(FileMode.Open, FileAccess.Read);
			stream.Read(new Byte[1], 0, 1);
			stream.Close();		
			file2.Refresh();
			iCountTestcases++;
			if((DateTime.Now-File.GetLastWriteTime( fileName )).TotalMinutes >6 ) {
				iCountErrors++;
				Console.WriteLine((DateTime.Now-File.GetLastWriteTime( fileName )).TotalMilliseconds);
				printerr( "Eror_0009! LastWriteTime is way off");
			}
			file2.Delete();
			strLoc = "Loc_0010";
			iCountTestcases++;
			try {
                                Console.WriteLine( File.GetLastWriteTime(s_strTFName) );
                                if( (DateTime.Today - File.GetLastWriteTime(s_strTFName) ).Seconds > 60 ) {
					iCountErrors++;
					printerr( "Eror_0011! LastWrite time is not correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0012! Unexpected exception thrown: "+exc.ToString());
			} 
			if(File.Exists(fileName))
				File.Delete(fileName);
		} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_0013!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
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
		Co9007GetLastWriteTime_str cbA = new Co9007GetLastWriteTime_str();
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
