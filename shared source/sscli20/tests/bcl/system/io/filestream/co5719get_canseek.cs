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
public class Co5719get_CanSeek
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileStream.GetHandle()";
    public static String s_strTFName        = "Co5719get_CanSeek.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
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
            String filName = s_strTFAbbrev + "TestFile.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_20987";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            iCountTestcases++;
            if(fs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_898c8! True when underlying stream is closed");
            }
            strLoc = "Loc_029uc";
            fs2 = new FileStream("Co5608Test.tmp", FileMode.Create, FileAccess.Write);
            iCountTestcases++;
            if(!fs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_0uyt4! True when stream is write only");
            }
            fs2.Close();
            strLoc = "Loc_091uc";
            fs2 = new FileStream("Co5608Test.tmp", FileMode.Open, FileAccess.ReadWrite);
            iCountTestcases++;
            if(!fs2.CanSeek) 
            {
                iCountErrors++;
                printerr("Error_298xj! False when stream is ReadWrite");
            } 
            fs2.Close();
            strLoc = "Loc_09u9v";
            fs2 = new FileStream("Co5608Test.tmp", FileMode.Open, FileAccess.Read);
            iCountTestcases++;
            if(!fs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_f8u89! False when stream is Read");
            }
            fs2.Close();
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
        Co5719get_CanSeek cbA = new Co5719get_CanSeek();
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
