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
public class Co8741Contains_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.Contains(string)";
    public static String s_strTFName        = "Co8741Contains_str.cs";
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
            Console.WriteLine("1. Check for empty collection");
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if (sc.Contains(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}, returned true for empty collection", i);
                }
            } 
            Console.WriteLine("2. add simple strings and verify Contains()");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = sc.Count;
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if (!sc.Contains(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, collection doesn't contain item \"{1}\"", i, values[i]);
                } 
            }
            Console.WriteLine("3. add intl strings and verify Contains()");
            strLoc = "Loc_003oo"; 
            string [] intlValues = new string [values.Length];
            for (int i = 0; i < values.Length; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            int len = values.Length;
            Boolean caseInsensitive = false;
            for (int i = 0; i < len; i++) 
            {
                if(intlValues[i].Length!=0 && intlValues[i].ToLower()==intlValues[i].ToUpper())
                    caseInsensitive = true;
            }
            iCountTestcases++;
            cnt = sc.Count;
            sc.AddRange(intlValues);
            if ( sc.Count != (cnt + intlValues.Length) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sc.Count, cnt + intlValues.Length);
            } 
            for (int i = 0; i < intlValues.Length; i++) 
            {
                iCountTestcases++;
                if (!sc.Contains(intlValues[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, collection doesn't contain item \"{1}\"", i, intlValues[i]);
                } 
            }
            Console.WriteLine("4. Add a very long string and verify Contains()");
            strLoc = "Loc_004oo"; 
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
                Console.WriteLine("Err_0004a, count is {1} instead of {2}", sc.Count, cnt+1);
            } 
            iCountTestcases++;
            if (!sc.Contains(intlStr)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, collection doesn't contain new item");
            }
            Console.WriteLine("5. Case sensitivity");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            sc.Clear();
            if (sc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005, count is {1} instead of {2} after Clear()", sc.Count, 0);
            } 
            intlStr = intlValues[0].ToUpper();
            iCountTestcases++;
            sc.Add(intlStr);
            if (sc.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {1} instead of {2}", sc.Count, 1);
            } 
            iCountTestcases++;
            if (!sc.Contains(intlStr)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, Contains() returned false ");
            }
            intlStr = intlValues[0].ToLower();
            iCountTestcases++;
            if (!caseInsensitive && sc.Contains(intlStr)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005c, Contains() returned true for lowercase version");
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
        Co8741Contains_str cbA = new Co8741Contains_str();
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
