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
public class Co8610set_Value
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "DictionaryEntry.Value";
    public static String s_strTFName        = "Co8610set_Value.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        int iCountErrors = 0;
        int iCountTestcases = 0;
        DictionaryEntry entry;
        String strValue;
        try
        {
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            entry = new DictionaryEntry();
            strValue = "Hello";
            entry.Value = strValue;
            if((String)entry.Value!=strValue)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned");
            }
            strLoc = "Loc_32497fxgb";
            iCountTestcases++;
            entry = new DictionaryEntry("What", "ever");
            strValue = "Hello";
            entry.Value = strValue;
            if((String)entry.Value!=strValue)
            {
                iCountErrors++;
                Console.WriteLine( "Err_89745sg! wrong value returned");
            }
            strLoc = "Loc_32497fxgb";
            iCountTestcases++;
            entry = new DictionaryEntry(5, 6);
            strValue = "Hello";
            entry.Value = strValue;
            if((String)entry.Value!=strValue)
            {
                iCountErrors++;
                Console.WriteLine( "Err_89745sg! wrong value returned");
            }
            entry = new DictionaryEntry("STring", 5);
            entry.Value = 5;
            if((Int32)entry.Value!=5)
            {
                iCountErrors++;
                Console.WriteLine( "Err_89745sg! wrong value returned");
            }
            strLoc = "Loc_dfyusg";
            iCountTestcases++;
            entry.Value = null;
            if(entry.Value!=null)
            {
                iCountErrors++;
                Console.WriteLine( "Err_89745sg! wrong value returned");
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
        Co8610set_Value cbA = new Co8610set_Value();
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
