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
public class Co5580get_CanWrite
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.SetLength(Int64)";
    public static String s_strTFName        = "Co5580get_CanWrite.cs";
    public static String s_strTFAbbrev      = "Co5580";
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
            FileStream fs2;
            String filName = s_strTFAbbrev+"Test.tmp";
            try 
            {
                new FileInfo(filName).Delete();
            } 
            catch (Exception) {} 
            strLoc = "Loc_4988i";
            new FileStream(filName, FileMode.Create).Close();
            fs2 = new FileStream(filName, FileMode.Open, FileAccess.Read);
            iCountTestcases++;
            if(fs2.CanWrite) 
            {
                iCountErrors++;
                printerr( "Error_889wj! CanWrite returned false for readable stream");
            }
            fs2.Close();
            strLoc = "Loc_88gyo";
            fs2 = new FileStream(filName, FileMode.Create, FileAccess.Write);
            iCountTestcases++;
            if(!fs2.CanWrite) 
            {
                iCountErrors++;
                printerr( "Error_20v87! CanWrite returned true for writeable stream");
            }
            fs2.Close();
            strLoc = "Loc_877yp";
            fs2 = new FileStream(filName, FileMode.Create, FileAccess.ReadWrite);
            iCountTestcases++;
            if(!fs2.CanWrite) 
            {
                iCountErrors++;
                printerr( "Error_86877! CanWrite returned false for ReadWrite access");
            }
            fs2.Close();
            new FileInfo(filName).Delete();
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
        Co5580get_CanWrite cbA = new Co5580get_CanWrite();
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
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
