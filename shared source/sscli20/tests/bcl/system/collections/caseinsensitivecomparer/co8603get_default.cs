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
using System.Collections;
using System.Globalization;
using System.IO;
public class Co8603get_Default
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "CaseInsensitiveComparer.Default";
    public static String s_strTFName        = "Co8603get_Default.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        CaseInsensitiveComparer comparer;
        CaseInsensitiveComparer defaultComparer;
        CultureInfo[] cultures;
        try
        {
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            iCountTestcases++;
            cultures = CultureInfo.GetCultures(CultureTypes.AllCultures);
            for(int i=0; i<cultures.Length; i++)
            {
                comparer = new CaseInsensitiveComparer(cultures[i]);
                defaultComparer = CaseInsensitiveComparer.Default;
                if(defaultComparer.Compare("hello", "HELLO")!=0)
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_93745sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", 0, defaultComparer.Compare("hello", "HELLO"));
                }
                if(defaultComparer.Compare("hello", "hello")!=0)
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_93745sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", 0, defaultComparer.Compare("hello", "hello"));
                }
                if(defaultComparer.Compare("hello", "mello")==0)
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_3846tdfsg! wrong value returned. Expected - <{0}>, Returned - <{1}>", 0, defaultComparer.Compare("hello", "mello"));
                }
                iCountTestcases++;
                if(defaultComparer.Compare(5, 5)!=0)
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_347tsfg! wrong value returned");
                }
                if(defaultComparer.Compare(5, 10)==0)
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_973425sdg! wrong value returned");
                }
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co8603get_Default cbA = new Co8603get_Default();
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
