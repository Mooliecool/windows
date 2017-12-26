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
public class Co5701get_Attributes
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Directory()";
    public static String s_strTFName        = "co5701get_attributes.cs";
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
            if(File.Exists(filName)) 
            {
                fil2 = new FileInfo(filName);
                fil2.Attributes = new FileAttributes();
                File.Delete(filName);
            }
            strLoc = "loc_2yg8c";
            fil2 = new FileInfo("FileDoesNotExist");
            iCountTestcases++;
            try 
            {
                fil2.Attributes = new FileAttributes();
                iCountErrors++;
                printerr( "Error_27t8b! Expected exception not thrown");
            } 
            catch ( FileNotFoundException fexc) 
            {
                printinfo( "Info_2109x! Caught expected exception, fexc=="+fexc.Message);
            } 
            catch ( Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_21409! Incorrect exception thrown, exc=="+exc.ToString());
            }
            File.Create(filName).Close();
            try 
            {
#if !PLATFORM_UNIX
                strLoc = "Loc_48yx9";
                fil2 = new FileInfo(filName);
                fil2.Attributes = FileAttributes.Hidden;
                iCountTestcases++;
                if((fil2.Attributes & FileAttributes.Hidden) != FileAttributes.Hidden) 
                {
                    iCountErrors++;
                    printerr( "ERror_2g985! Hidden not set");
                }
                fil2.Refresh();
                fil2.Attributes = FileAttributes.System ;
                iCountTestcases++;
                fil2.Refresh();
                if((fil2.Attributes & FileAttributes.System) != FileAttributes.System) 
                {
                    iCountErrors++;
                    printerr( "Error_298g7! System not set");
                }
#endif // PLATFORM_UNIX
                strLoc = "Loc_29gy7";
                fil2 = new FileInfo(filName);
                fil2.Attributes = FileAttributes.Archive;
                fil2.Refresh();
                fil2.Attributes = FileAttributes.ReadOnly | fil2.Attributes;
                fil2.Refresh();
                iCountTestcases++;
                if((fil2.Attributes & FileAttributes.ReadOnly) != FileAttributes.ReadOnly) 
                {
                    iCountErrors++;
                    printerr( "Error_g58y8! ReadOnly attribute not set");
                }
#if !PLATFORM_UNIX
                iCountTestcases++;
                if((fil2.Attributes & FileAttributes.Archive) != FileAttributes.Archive) 
                {
                    iCountErrors++;
                    printerr( "Error_2g78b! Archive attribute not set");
                }
#endif // PLATFORM_UNIX
                fil2.Attributes = new FileAttributes();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_284y8! Unexpected exception thrown, , exc=="+exc.ToString());
            }
            if(File.Exists(filName)) 
            {
                fil2 = new FileInfo(filName);
                fil2.Attributes = new FileAttributes();
                File.Delete(filName);
            }
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
        Co5701get_Attributes cbA = new Co5701get_Attributes();
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
