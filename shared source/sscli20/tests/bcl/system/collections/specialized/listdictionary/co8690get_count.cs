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
public class Co8690get_Count
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ListDictionary.Count";
    public static String s_strTFName        = "Co8690get_Count.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public const int MAX_LEN = 50;          
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        ListDictionary ld; 
        string [] values = 
        {
            "",
            " ",
            "a",
            "aA",
            "text",
            "     SPaces",
            "1",
            "$%^#",
            "2222222222222222222222222",
            System.DateTime.Today.ToString(),
            Int32.MaxValue.ToString()
        };
        string [] keys = 
        {
            "zero",
            "oNe",
            " ",
            "",
            "aa",
            "1",
            System.DateTime.Today.ToString(),
            "$%^#",
            Int32.MaxValue.ToString(),
            "     spaces",
            "2222222222222222222222222"
        };
        int cnt = 0;            
        try
        {
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001_oo"; 
            iCountTestcases++;
            ld = new ListDictionary();
            cnt = ld.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001_, count is {0} instead of {1} after default ctor", ld.Count, 0);
            }
            Console.WriteLine("1. call Clear() on empty dictionary and check Count");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            ld.Clear();
            cnt = ld.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, count is {0} instead of {1} after Clear()", ld.Count, 0);
            }
            iCountTestcases++;
            cnt = ld.Keys.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, Keys.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            iCountTestcases++;
            cnt = ld.Values.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001c, Values.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            Console.WriteLine("2. add simple strings and get Count");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = ld.Count;
            for (int i = 0; i < values.Length; i++) 
            {    
                ld.Add(keys[i], values[i]);
            }
            if (ld.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", ld.Count, values.Length);
            }
            iCountTestcases++;
            ld.Clear();
            cnt = ld.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, count is {0} instead of {1} after Clear()", ld.Count, 0);
            }
            iCountTestcases++;
            cnt = ld.Keys.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002c, Keys.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            iCountTestcases++;
            cnt = ld.Values.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002d, Values.Count is {0} instead of {1} after Clear()", cnt, 0);
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
        Co8690get_Count cbA = new Co8690get_Count();
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
