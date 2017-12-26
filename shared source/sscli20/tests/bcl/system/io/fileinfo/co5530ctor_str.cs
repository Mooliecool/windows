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
public class Co5530ctor_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Directory.ToString";
    public static String s_strTFName        = "Co5530ctor_str.cs";
    public static String s_strTFAbbrev      = "Co5530";
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
            strLoc = "Loc_498yg";
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo(null);
                iCountErrors++;
                printerr( "Error_209uz! Expected exception not thrown, fil2=="+fil2.FullName);
            } 
            catch (ArgumentNullException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_21x99! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_7h7g7";
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo("ThisFileDoesNotExist");
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_20g9u! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_289vy";
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo(Environment.CurrentDirectory);
                fil2.Open(FileMode.Open);
                iCountErrors++;
                printerr( "Error_301ju! Expected exception not thrown, fil2=="+fil2.FullName);
            } 
            catch (UnauthorizedAccessException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_209us! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_fd348";
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo(".");
                fil2.Open(FileMode.Open);
                iCountErrors++;
                printerr( "Error_398vh! Expected exception not thrown, fil2=="+fil2.FullName);
            } 
            catch (UnauthorizedAccessException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_27h72! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_r7yd9";
            File.Create("MyTestFile").Close();
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo(Environment.CurrentDirectory+"\\MyTestFile");
                fil2.Delete();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_f588y! Unexpected exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_f548y";
            File.Create("MyTestFile").Close();
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo("MyTestFile");
                fil2.Delete();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_298gv! Unexpected exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_298dy";
            File.Create("Hello.there.you.have.an.extension").Close();
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo("Hello.there.you.have.an.extension");
                fil2.Delete();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2901s! Unexpected exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_984hg";
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo("**");
                iCountErrors++;
                printerr( "Error_298xh! Expected exception not thrown, fil2=="+fil2.FullName);
            } 
            catch (ArgumentException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2091s! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_948jk";
            iCountTestcases++;
            try 
            {
                fil2 = new FileInfo(String.Empty);
                iCountErrors++;
                printerr( "Error_0199z! Expected exception not thrown, fil2=="+fil2.FullName);
            } 
            catch (ArgumentException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_20109! Incorrect exception thrown, exc=="+exc.ToString());
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
        Co5530ctor_str cbA = new Co5530ctor_str();
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
