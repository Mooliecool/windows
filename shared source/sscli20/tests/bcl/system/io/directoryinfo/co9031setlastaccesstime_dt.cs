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
public class Co9031set_LastAccessTime_dt
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "DirectoryInfo.LastAccessTime";
	public static String s_strTFName        = "Co9031set_LastAccessTime_dt.cs";
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
			String fileName = s_strTFAbbrev+"TestDirectory";                         
			strLoc = "Loc_0006" ;
                        DirectoryInfo dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				dir2.LastAccessTime = DateTime.Today ;
				if((dir2.LastAccessTime - DateTime.Now).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0007! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0008! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0009";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				dir2.LastAccessTime = DateTime.Now.AddYears(1) ;
				if((dir2.LastAccessTime - DateTime.Now.AddYears(1)).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0010! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0011! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0012";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				DateTime dt = DateTime.Now.AddYears(-1) ;
                dir2.LastAccessTime =  dt ;
				if(! CompareDates( dt, dir2.LastAccessTime) ) {
					iCountErrors++;
					printerr( "Error_0013! Creation time cannot be correct" + CompareDates( dt , dir2.LastAccessTime) );
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0014! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0015";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				dir2.LastAccessTime = DateTime.Now.AddMonths(1) ;
				if((dir2.LastAccessTime - DateTime.Now.AddMonths(1)).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0016! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0017! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0018";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				dir2.LastAccessTime = DateTime.Now.AddMonths(-1) ;
				if((dir2.LastAccessTime - DateTime.Now.AddMonths(-1)).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0019! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0020! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0021";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				dir2.LastAccessTime =  DateTime.Now.AddDays(1) ;
				if((dir2.LastAccessTime - DateTime.Now.AddDays(1)).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0022! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0023! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0024";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				dir2.LastAccessTime =  DateTime.Now.AddDays(-1) ;
				if((dir2.LastAccessTime - DateTime.Now.AddDays(-1)).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0025! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0026! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0025";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				dir2.LastAccessTime =  new DateTime(2001,332,20,50,50,50) ;
                                iCountErrors++;
                                printerr( "Error_0026! Creation time cannot be correct");
			} catch (ArgumentOutOfRangeException aoore){
                                printinfo("Unexpected exception occured :: " + aoore.Message );
                        } catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0027! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
			strLoc = "Loc_0028";
                        dir2 = new DirectoryInfo(fileName);
			dir2.Create();
                        iCountTestcases++;
			try {
				DateTime dt =  new DateTime( 2001,2,2,20,20,20) ;
                                dir2.LastAccessTime =  dt  ;
				if((dir2.LastAccessTime - dt ).Seconds > 0) {
					iCountErrors++;
					printerr( "Error_0029! Creation time cannot be correct");
				}
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_0030! Unexpected exceptiont thrown: "+exc.ToString());
			}
                        dir2.Delete();
                } catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_0100!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
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
    private bool CompareDates( DateTime dt1, DateTime dt2 ){
        if ( (dt1.Year == dt2.Year) && (dt1.Month == dt2.Month) && (dt1.Day == dt2.Day) ) 
            return true ;
        else
            return false ;
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
		Co9031set_LastAccessTime_dt cbA = new Co9031set_LastAccessTime_dt();
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
