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
public class Co9012Exists
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileInfo.Exists()";
    public static String s_strTFName        = "Co9012Exists.cs";
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
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_276t8";
            iCountTestcases++;
            fil2 = new FileInfo(".");
            if(fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_95428! Incorrect return value");
            }
            iCountTestcases++;
            fil2 = new FileInfo(Environment.CurrentDirectory);
            if(fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_97t67! Incorrect return value");
            }
            strLoc = "Loc_99084";
            String strPath = "" ; 
            new FileStream( strPath +filName, FileMode.Create).Close();
            fil2 = new FileInfo(strPath +filName);
            if(!fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_02099! Incorrect return");
            }
            File.Delete( strPath +filName);
            iCountTestcases++;
            try 
            {
                fil2.Refresh();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2547b! Unexpected exception, exc=="+exc.ToString());
            }
            if(fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_50210! Incorrect return");
            }
            strLoc = "Loc_t993c";
            try 
            {			
                iCountTestcases++;
                fil2 = new FileInfo("Da drar vi til fjells");
                if(fil2.Exists) 
                {
                    iCountErrors++;
                    printerr( "Error_6895b! Incorrect return value");
                }			
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_1y908! Unexpected exception, exc=="+exc.ToString());
            }
            strLoc = "Loc_2y78g";
            new FileStream(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            if(!fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_9t821! Returned false for existing file");
            }
            iCountTestcases++;
            fil2 = new FileInfo(Environment.CurrentDirectory+"\\"+filName);
            if(!fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_9198v! Returned false for existing file");
            }			
            File.Delete(filName);
            strLoc = "Loc_298g7";
            String tmp = filName+"   "+filName;
            new FileStream(tmp, FileMode.Create).Close();
            fil2 = new FileInfo(tmp);
            iCountTestcases++;
            if(!fil2.Exists) 
            {
                iCountErrors++;
                printerr( "Error_01y8v! Returned incorrect value");
            }
            fil2.Delete();
            strLoc = "Loc_398vy8";
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo("*");
                if(fil2.Exists) 
                {
                    iCountErrors++;
                    printerr( "Error_4979c! File with wildcard exists: "+fil2.FullName);
                }
            }
            catch(ArgumentException)
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_498u9! Unexpected exception thrown, exc=="+exc.ToString());
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
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
        Co9012Exists cbA = new Co9012Exists();
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
