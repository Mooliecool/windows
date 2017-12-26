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
public class Co5693Open_fm
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.OpenText(String)";
    public static String s_strTFName        = "Co5693Open_fm.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    String strLoc = "Loc_000oo";
    String strValue = String.Empty;
    int iCountErrors = 0;
    int iCountTestcases = 0;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        try
        {
            TestMethod(FileMode.CreateNew);
            TestMethod(FileMode.Create);
            TestMethod(FileMode.Open);
            TestMethod(FileMode.OpenOrCreate);
            TestMethod(FileMode.Truncate);
            TestMethod(FileMode.Append);
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
    public void TestMethod(FileMode fm)
    {
        String filName = s_strTFAbbrev+"TestFile";
        FileInfo fil2;
        StreamWriter sw2;
        Stream fs2;
        String str2;
        if(File.Exists(filName))
            File.Delete(filName);
        strLoc = "Loc_234yg";
        fil2 = new FileInfo(filName);
        switch(fm) 
        {
            case FileMode.CreateNew:
            case FileMode.Create:
            case FileMode.OpenOrCreate:
                fs2 = fil2.Open(fm);
                iCountTestcases++;
                if(!File.Exists(filName)) 
                {
                    iCountErrors++;
                }
                fs2.Close();
                break;
            case FileMode.Open:
            case FileMode.Truncate:
                iCountTestcases++;
                try 
                {
                    fs2 = fil2.Open(fm);
                    iCountErrors++;
                    printerr( "Error_2yg8b! Expected exception not thrown");
                    fs2.Close();
                } 
                catch (FileNotFoundException fexc) 
                {
                    printinfo( "Info_49y7b! Caught expected exception, fexc=="+fexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_2y7gf! Incorrect exception thrown, exc=="+exc.ToString());
                }
                break;
            case FileMode.Append:
                iCountTestcases++;
                try 
                {
                    fs2 = fil2.Open(fm);
                    iCountErrors++;
                    printerr( "Error_2g78b! Expected exception not thrown");
                    fs2.Close();
                } 
                catch (ArgumentException aexc) 
                {
                    printinfo( "Info_2g7y7! Caught expected exception, aexc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_g77b7! Incorrect exception thrown, exc=="+exc.ToString());
                }
                break;
            default:
                iCountErrors++;
                printerr( "Error_27tbv! This should not be....");
                break;
        }
        if(File.Exists(filName))
            File.Delete(filName);
        strLoc = "Loc_4yg7b";
        sw2 = new StreamWriter(filName);
        str2 = "Du er en ape";
        sw2.Write(str2);
        sw2.Close();
        fil2 = new FileInfo(filName);
        switch(fm) 
        {
            case FileMode.CreateNew:
                iCountTestcases++;
                try 
                {
                    fs2 = fil2.Open(fm);
                    iCountErrors++;
                    printerr( "Error_27b98! Expected exception not thrown");
                    fs2.Close();
                } 
                catch (IOException aexc) 
                {
                    printinfo( "Info_2399c! Caught expected exception, aexc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_g8782! Incorrect exception thrown, exc=="+exc.ToString());
                }
                break;
            case FileMode.Create:
                fs2 = fil2.Open(fm);
                if(fs2.Length != 0) 
                {
                    iCountErrors++;
                    printerr( "Error_287vb! Incorrect length of file=="+fil2.Length);
                }
                fs2.Close();
                break;
            case FileMode.OpenOrCreate:
            case FileMode.Open:
                fs2 = fil2.Open(fm);
                if(fs2.Length != str2.Length ) 
                {
                    iCountErrors++;
                    printerr( "Error_2gy78! Incorrect length on file=="+fil2.Length);
                }
                fs2.Close();
                break;
            case FileMode.Truncate:
                fs2 = fil2.Open(fm);
                if(fs2.Length != 0) 
                {
                    iCountErrors++;
                    printerr( "Error_29gv9! Incorrect length on file=="+fil2.Length);
                }
                fs2.Close();
                break;
            case FileMode.Append:
                iCountTestcases++;
                try 
                {
                    fs2 = fil2.Open(fm);
                    iCountErrors++;
                    printerr( "Error_287yb! Expected exception not thrown");
                    fs2.Close();
                } 
                catch (ArgumentException aexc) 
                {
                    printinfo( "Info_93282! Caught expected exception, aexc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_27878! Incorrect exception thrown, exc=="+exc.ToString());
                }
                break;
            default:
                iCountErrors++;
                printerr( "Error_587yb! This should not be...");
                break;
        }
        if(File.Exists(filName))
            File.Delete(filName);
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
        Co5693Open_fm cbA = new Co5693Open_fm();
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
