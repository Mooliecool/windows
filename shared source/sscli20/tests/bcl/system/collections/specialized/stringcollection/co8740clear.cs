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
using System.IO;
using System.Text;  
using System;
using System.Collections;
using System.Collections.Specialized;
using GenStrings;
public class Co8740Clear
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.Clear()";
    public static String s_strTFName        = "Co8740Clear.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public const int MAX_LEN = 50;          
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        IntlStrings intl;
        String strLoc = "Loc_000oo";
        StringCollection sc; 
        string [] values = 
        {
            "",
            " ",
            "a",
            "aa",
            "text",
            "     spaces",
            "1",
            "$%^#",
            "2222222222222222222222222",
            System.DateTime.Today.ToString(),
            Int32.MaxValue.ToString()
        };
        int cnt = 0;            
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001_oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            cnt = sc.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001_, count is {0} instead of {1} after default ctor", sc.Count, 0);
            }
            Console.WriteLine("1. call Clear() on empty collection");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc.Clear();
            cnt = sc.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, count is {0} instead of {1} after Clear()", sc.Count, 0);
            }
            Console.WriteLine("2. add simple strings and Clear()");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = sc.Count;
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sc.Count, values.Length);
            }
            iCountTestcases++;
            sc.Clear();
            cnt = sc.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, count is {0} instead of {1} after Clear()", sc.Count, 0);
            }
            Console.WriteLine("3. add intl strings and Clear()");
            strLoc = "Loc_003oo"; 
            string [] intlValues = new string [values.Length];
            for (int i = 0; i < values.Length; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            cnt = sc.Count;
            iCountTestcases++;
            sc.AddRange(intlValues);
            if (sc.Count != (cnt + intlValues.Length)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sc.Count, cnt + intlValues.Length);
            }
            iCountTestcases++;
            sc.Clear();
            cnt = sc.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, count is {0} instead of {1} after Clear()", sc.Count, 0);
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_general!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "Pass.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
            return true;
        }
        else
        {
            Console.WriteLine("Fail!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co8740Clear cbA = new Co8740Clear();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : Fail! Error Err_main! Uncaught Exception in main(), exc_main=="+exc_main);
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "Fail!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
    }
}
