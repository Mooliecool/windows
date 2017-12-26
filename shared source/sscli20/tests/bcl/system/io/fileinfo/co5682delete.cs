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
using System.Diagnostics;
public class Co5682Delete
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Directory.Root";
    public static String s_strTFName        = "co5682delete.cs";
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
            FileStream fs2;
            FileInfo fil2;
            strLoc = "Loc_7198c";
            iCountTestcases++;
            fil2 = new FileInfo("AkkarBurger");
            fil2.Delete();
#if !PLATFORM_UNIX // delete of an open file fails only on Windows
            strLoc = "Loc_29yc7";
            fs2 = new FileStream(filName, FileMode.Create);
            fil2 = new FileInfo(filName);
            Console.WriteLine("FileName for fileInfo " + filName);
            iCountTestcases++;
            try 
            {
                fil2.Delete();
                iCountErrors++;
                printerr( "Error_1y678! Expected exception not thrown");
            } 
            catch (IOException iexc) 
            {
                printinfo( "Info_10fuc! Caught expected exception, iexc=="+iexc.Message); 
            } 
            catch (UnauthorizedAccessException iexc) 
            {
                printinfo( "Info_10fuc! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_16709! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();
            iCountTestcases++;
            if(!fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_768bc! File does not exist=="+fil2.FullName);
            }
            fil2.Delete();
            Console.WriteLine(File.Exists(fil2.FullName));
            Console.WriteLine(fil2.Exists);
            fil2.Refresh();
            iCountTestcases++;
            if(fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_810x8! File not deleted=="+fil2.FullName);
            }
#endif //!PLATFORM_UNIX
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
        Co5682Delete cbA = new Co5682Delete();
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
