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
public class Co5768Refresh
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Refresh()";
    public static String s_strTFName        = "Co5768Refresh.cs";
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
            FileInfo fil2;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_00001";
            File.Open(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            File.Delete(filName);
            iCountTestcases++;
            fil2.Refresh();
            strLoc = "Loc_00005";
            if(File.Exists("Temp001"))
                File.Delete("Temp001");
            iCountTestcases++;
            File.Open(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            fil2.MoveTo("Temp001");
            fil2.Refresh();
            File.Delete("Temp001");
            strLoc = "Loc_00006";
            iCountTestcases++;
            File.Open(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            Console.WriteLine(fil2.Attributes);
            if(((Int32)fil2.Attributes & (Int32)FileAttributes.ReadOnly) != 0) 
            {
                iCountErrors++;
                printerr( "Error_00007! Attribute set before refresh");
            }
            fil2.Attributes = FileAttributes.ReadOnly;
            fil2.Refresh();
            iCountTestcases++;
            if(((Int32)fil2.Attributes & (Int32)FileAttributes.ReadOnly) <= 0) 
            {
                iCountErrors++;
                printerr( "Error_00008! Object not refreshed after setting readonly");
            }
            fil2.Attributes = new FileAttributes();
            fil2.Refresh();
            if(((Int32)fil2.Attributes & (Int32)FileAttributes.ReadOnly) != 0) 
            {
                iCountErrors++;
                printerr( "Error_00009! Object not refreshed after removing readonly");
            }
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
        Co5768Refresh cbA = new Co5768Refresh();
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
