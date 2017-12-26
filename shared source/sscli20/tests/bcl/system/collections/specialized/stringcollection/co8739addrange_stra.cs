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
public class Co8739AddRange_strA
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.AddRange(string [])";
    public static String s_strTFName        = "Co8739AddRange_strA.cs";
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
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            Console.WriteLine("1. add simple strings");
            iCountTestcases++;
            cnt = sc.Count;
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            Console.WriteLine("2. verify that collection contains all added items");    
            strLoc = "Loc_002oo"; 
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if (!sc.Contains(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}, collection doesn't contain new item", i);
                } 
            }
            Console.WriteLine("3. add intl strings");
            string [] intlValues = new string [values.Length];
            for (int i = 0; i < values.Length; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            strLoc = "Loc_003oo"; 
            cnt = sc.Count;
            Console.WriteLine(" initial number of items: " + cnt);
            iCountTestcases++;
            sc.AddRange(intlValues);
            if (sc.Count != (cnt + intlValues.Length)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, count is {0} instead of {1}", sc.Count, cnt + intlValues.Length);
            }
            Console.WriteLine("4. verify that collection contains all added items");    
            strLoc = "Loc_004oo"; 
            for (int i = 0; i < intlValues.Length; i++) 
            {
                iCountTestcases++;
                iCountTestcases++;
                if (!sc.Contains(intlValues[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}, collection doesn't contain new item", i);
                } 
            }
            Console.WriteLine("5. add empty range");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            cnt = sc.Count;
            string [] empty = {};
            sc.AddRange(empty);
            if (sc.Count != cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005, count is {0} instead of {1}", sc.Count, cnt);
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
        Co8739AddRange_strA cbA = new Co8739AddRange_strA();
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
