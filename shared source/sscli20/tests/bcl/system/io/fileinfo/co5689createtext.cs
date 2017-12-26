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
public class Co5689CreateText
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.CreateText";
    public static String s_strTFName        = "Co5689CreateText.cs";
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
            String filName = s_strTFAbbrev+"TestFile";
            FileInfo fil2;
            StreamWriter sw2;
            StreamReader sr2;
            String str2;
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_2y78b";
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            sw2 = fil2.CreateText();
            sw2.Write("HelloWorld");
            sw2.Close();
            sr2 = new StreamReader(filName);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("HelloWorld")) 
            {
                iCountErrors++;
                printerr( "Error_21y77! Incorrect string written, str2=="+str2);
            }
            sr2.Close();
            strLoc = "Loc_2gy7b";
            sw2 = fil2.CreateText();
            sw2.Write("You Big Globe");
            sw2.Close();
            sr2 = new StreamReader(filName);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("You Big Globe")) 
            {
                iCountErrors++;
                printerr( "Error_12ytb! Incorrect string written, str2=="+str2);
            }
            sr2.Close();
#if !PLATFORM_UNIX  // delete of a readonly file fails only on Windows
            strLoc = "Loc_498yv";
            fil2.Attributes = FileAttributes.ReadOnly;
            iCountTestcases++;
            try 
            {
                sw2 = fil2.CreateText();
                iCountErrors++;
                printerr( "Error_398fy! Expected exception not thrown");
            } 
            catch (UnauthorizedAccessException aexc) 
            {
                printinfo( "Info_39c88! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_8gy8g! Incorrect exception thrown, exc=="+exc.ToString());
            }
#endif //!PLATFORM_UNIX
            fil2.Attributes = new FileAttributes();
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
        Co5689CreateText cbA = new Co5689CreateText();
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
