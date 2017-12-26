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
public class Co9003GetCreationTime_str
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "File.GetCreationTime()";
	public static String s_strTFName        = "co9003getcreationtime_str.cs";
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
                        iCountTestcases++;
			try {
				DateTime dt = File.GetCreationTime(null);
				iCountErrors++;
				printerr( "Error_0009! Expected exception not thrown");
			} catch (ArgumentNullException exc){
                                printinfo("Expected exception thrown");      
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0010! Unexpected exceptiont thrown: "+exc.ToString());
			}
			strLoc = "Loc_0002";
                        FileInfo file1 = new FileInfo(fileName);
			FileStream fs = file1.Create();
			fs.Close();                        
                        iCountTestcases++;
			try {
				Console.WriteLine( File.GetCreationTime(fileName) );
				if((File.GetCreationTime(fileName) - DateTime.Now).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0003! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0004! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        file1.Delete();
			strLoc = "Loc_0005";
                        fileName = fileName + "Testing" ;
                        FileInfo file2 = new FileInfo(fileName);
                        FileStream fs2 = file2.Create();
			fs2.Close();			
                        Thread.Sleep(2000);
			iCountTestcases++;
			try {
				Console.WriteLine( DateTime.Now );
                                Console.WriteLine( (DateTime.Now-File.GetCreationTime(fileName)).Seconds.ToString());
                                Console.WriteLine( file2.CreationTime );
                                if((DateTime.Now-File.GetCreationTime(fileName)).Seconds > 3) {
					iCountErrors++;
					printerr( "Eror_0006! Creation time is off");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0007! Unexpected exception thrown: "+exc.ToString());
			} 
                        file2.Delete();
			strLoc = "Loc_0012";
			iCountTestcases++;
			try {
				Console.WriteLine( "File date :: " + File.GetCreationTime(s_strTFName));
                                Console.WriteLine( "Today :: " + DateTime.Today );
                                if( (DateTime.Today - File.GetCreationTime(s_strTFName) ).Seconds > 60 ) {
					iCountErrors++;
					printerr( "Eror_0013! Creation time is off");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0014! Unexpected exception thrown: "+exc.ToString());
			} 
                } catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_0008!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
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
		Co9003GetCreationTime_str cbA = new Co9003GetCreationTime_str();
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
