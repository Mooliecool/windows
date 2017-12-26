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
public class Co5735SetLength_i64
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "MemoryStream.SetLength(Int64)";
    public static String s_strTFName        = "Co5735SetLength_i64.cs";
    public static String s_strTFAbbrev      = "Co5735";
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
            MemoryStream ms2;
            StreamWriter sw2;
            strLoc = "Loc_98yc8";
            ms2 = new MemoryStream();
            iCountTestcases++;
            try 
            {
                ms2.SetLength(-2);
                iCountErrors++;
                printerr( "Error_23890! Expected exception not thrown, position=="+ms2.Position);
            } 
            catch (ArgumentOutOfRangeException aexc) 
            {
                printinfo( "Info_328xj! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_289ux! Incorrect exception thrown, exc=="+exc.ToString());
            }
            ms2.Close();
            strLoc = "Loc_27yxc";
            ms2 = new MemoryStream();
            ms2.SetLength(50);
            ms2.Position = 50;
            sw2 = new StreamWriter(ms2);
            for(char c = 'a' ; c < 'f' ; c++) 
                sw2.Write(c);
            sw2.Flush();
            iCountTestcases++;
            if(ms2.Length != 55) 
            {
                iCountErrors++;
                printerr( "Error_389xd! Incorrect stream length=="+ms2.Length);
            }
            ms2.SetLength(30);
            iCountTestcases++;
            if(ms2.Length != 30) 
            {
                iCountErrors++;
                printerr( "Error_28xye! Incorrect length=="+ms2.Length);
            }
            iCountTestcases++;
            if(ms2.Position != 30) 
            {
                iCountErrors++;
                printerr( "Error_3989a! Incorrect position=="+ms2.Position);
            }
            ms2.SetLength(100);
            iCountTestcases++;
            if(ms2.Length != 100) 
            {
                iCountErrors++;
                printerr( "Error_2090x! Incorrect length=="+ms2.Length);
            }
            ms2.Close();
            strLoc = "Loc_99189";
            ms2 = new MemoryStream();
            iCountTestcases++;
            try 
            {
                ms2.SetLength(Int64.MaxValue);
                iCountErrors++;
                printerr( "Error_209xu! Expected exception not thrown, ms2.Length=="+ms2.Length);
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_0999d! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (IOException iexc) 
            {
                printinfo( "Info_24889! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200fu! Incorrect exception thrown, exc=="+exc.ToString());
            }
            ms2.Close();
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
        Co5735SetLength_i64 cbA = new Co5735SetLength_i64();
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
