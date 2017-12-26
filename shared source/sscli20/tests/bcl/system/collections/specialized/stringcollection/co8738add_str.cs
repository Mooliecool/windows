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
public class Co8738Add_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.Add(string)";
    public static String s_strTFName        = "Co8738Add_str.cs";
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
        int ind = 0;            
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            Console.WriteLine("1. add simple strings");
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                cnt = sc.Count;
                sc.Add(values[i]);
                if (sc.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}a, count is {1} instead of {2}", i, sc.Count, cnt+1);
                } 
                iCountTestcases++;
                if (!sc.Contains(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}b, collection doesn't contain new item", i);
                } 
                iCountTestcases++;
                ind = sc.IndexOf(values[i]);
                if (ind != sc.Count - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}c, returned index {1} instead of {2}", i, ind, sc.Count - 1);
                } 
                if (ind != -1) 
                {
                    iCountTestcases++;
                    if (String.Compare(sc[ind], values[i], false) != 0) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0001_{0}d, returned item \"{1}\" instead of \"{2}\"", i, sc[ind], values[i]);
                    } 
                }
            }
            Console.WriteLine("2. add intl strings");
            string [] intlValues = new string [values.Length];
            for (int i = 0; i < values.Length; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            Console.WriteLine(" initial number of items: " + sc.Count);
            strLoc = "Loc_002oo"; 
            for (int i = 0; i < intlValues.Length; i++) 
            {
                iCountTestcases++;
                cnt = sc.Count;
                sc.Add(intlValues[i]);
                if (sc.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}a, count is {1} instead of {2}", i, sc.Count, cnt+1);
                } 
                iCountTestcases++;
                if (!sc.Contains(intlValues[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, collection doesn't contain new item", i);
                } 
                iCountTestcases++;
                ind = sc.IndexOf(intlValues[i]);
                if (ind != sc.Count - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, returned index {1} instead of {2}", i, ind, sc.Count - 1);
                } 
                if (ind != -1) 
                {
                    iCountTestcases++;
                    if (String.Compare(sc[ind], intlValues[i], false) != 0) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0002_{0}d, returned item \"{1}\" instead of \"{2}\"", i, sc[ind], intlValues[i]);
                    } 
                }
            }
            Console.WriteLine("3. Add a very long string");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            cnt = sc.Count;
            string intlStr = intlValues[0];
            while (intlStr.Length < 10000)
                intlStr += intlStr;
            Console.WriteLine("  - add string of Length " + intlStr.Length);
            sc.Add(intlStr);
            if (sc.Count != cnt+1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {1} instead of {2}", sc.Count, cnt+1);
            } 
            iCountTestcases++;
            if (!sc.Contains(intlStr)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, collection doesn't contain new item");
            } 
            iCountTestcases++;
            ind = sc.IndexOf(intlStr);
            if (ind != sc.Count - 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, returned index {1} instead of {2}", ind, sc.Count - 1);
            } 
            if (ind != -1) 
            {
                iCountTestcases++;
                if (String.Compare(sc[ind], intlStr, false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003d, returned item \"{1}\" instead of \"{2}\"", sc[ind], intlStr);
                } 
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
        Co8738Add_str cbA = new Co8738Add_str();
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
