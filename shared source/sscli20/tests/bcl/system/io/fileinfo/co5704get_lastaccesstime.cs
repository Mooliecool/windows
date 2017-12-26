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
public class Co5704get_LastAccessTime
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Directory()";
    public static String s_strTFName        = "Co5704get_LastAccessTime.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
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
            strLoc = "Loc_r8r7j";
            new FileStream(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            stream = fil2.OpenWrite();
            stream.Write(new Byte[]{10}, 0, 1);
            stream.Close();
            Thread.Sleep(4000);
            fil2.Refresh();
            iCountTestcases++;
            try 
            {
                DateTime d1 = DateTime.Now ;
                DateTime d2 = fil2.LastAccessTime;
                if(d1.Year != d2.Year || d1.Month != d2.Month || d1.Month != d2.Month) 
                {
                    iCountErrors++;
                    printerr( "Error_20hjx! Creation time cannot be correct");
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_20fhd! Unexpected exceptiont thrown: "+exc.ToString());
                printinfo( "");
            }
            strLoc = "Loc_20yxc";
            stream = fil2.Open(FileMode.Open);
            stream.Read(new Byte[1], 0, 1);
            stream.Close();		
            fil2.Refresh();
            Thread.Sleep(1000);
            iCountTestcases++;
            try 
            {
                DateTime d1 = DateTime.Now ;
                DateTime d2 = fil2.LastAccessTime;
                if(d1.Year != d2.Year || d1.Month != d2.Month || d1.Month != d2.Month) 
                {
                    iCountErrors++;
                    Console.WriteLine((DateTime.Now-fil2.LastAccessTime).TotalMilliseconds);
                    printerr( "Eror_209x9! LastAccessTime is way off");
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_209jx! Unexpected exception thrown: "+exc.ToString());
            } 
            fil2.Delete();
            if(File.Exists(filName))
                File.Delete(filName);
        } 
        catch (Exception exc_general ) 
        {
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
        Co5704get_LastAccessTime cbA = new Co5704get_LastAccessTime();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
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
