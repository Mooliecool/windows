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
public class Co8758ContainsValue_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringDictionary.ContainsValue(string)";
    public static String s_strTFName        = "Co8758ContainsValue_str.cs";
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
        StringDictionary sd; 
        string ind;
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
        string [] keys = 
        {
            "zero",
            "one",
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
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sd = new StringDictionary();
            Console.WriteLine("1. Check for empty dictionary");
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if (sd.ContainsValue(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}, returned true for empty dictionary", i);
                }
            } 
            Console.WriteLine("2. add simple strings and verify ContainsValue()");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = values.Length;
            for (int i = 0; i < cnt; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            if (sd.Count != cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sd.Count, cnt);
            } 
            for (int i = 0; i < cnt; i++) 
            {
                iCountTestcases++;
                if (!sd.ContainsValue(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, collection doesn't contain value \"{1}\"", i, values[i]);
                } 
                iCountTestcases++;
                if (!sd.ContainsKey(keys[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, collection doesn't contain key \"{1}\"", i, keys[i]);
                } 
            }
            Console.WriteLine("3. add intl strings and verify ContainsValue()");
            strLoc = "Loc_003oo"; 
            int len = values.Length;
            string [] intlValues = new string [len*2];
            for (int i = 0; i < len*2; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            Boolean caseInsensitive = false;
            for (int i = 0; i < len * 2; i++) 
            {
                if(intlValues[i].Length!=0 && intlValues[i].ToLower()==intlValues[i].ToUpper())
                    caseInsensitive = true;
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                cnt = sd.Count;
                sd.Add(intlValues[i+len], intlValues[i]);
                if (sd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}a, count is {1} instead of {2}", i, sd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (!sd.ContainsValue(intlValues[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, collection doesn't contain value of new item", i);
                } 
                iCountTestcases++;
                if (!sd.ContainsKey(intlValues[i+len])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}c, collection doesn't contain key of new item", i);
                } 
                ind = intlValues[i+len];
                iCountTestcases++;
                if (String.Compare(sd[ind], intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}d, returned item \"{1}\" instead of \"{2}\"", i, sd[ind], intlValues[i]);
                } 
            }
            Console.WriteLine("4. add null string with non-null key and verify ContainsValue()");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            cnt = sd.Count;
            string k = "keykey";
            sd.Add(k, null);
            if (sd.Count != cnt+1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {1} instead of {2}", sd.Count, cnt+1);
            } 
            iCountTestcases++;
            if (!sd.ContainsValue(null)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, dictionary doesn't contain value null");
            }
            Console.WriteLine("5. Case sensitivity");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            sd.Clear();
            if (sd.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005, count is {1} instead of {2} after Clear()", sd.Count, 0);
            } 
            string [] intlValuesLower = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
            {
                intlValues[i] = intlValues[i].ToUpper();
            }
            for (int i = 0; i < len * 2; i++) 
            {
                intlValuesLower[i] = intlValues[i].ToLower();
            } 
            sd.Clear();
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                cnt = sd.Count;
                sd.Add(intlValues[i+len], intlValues[i]);     
                if (sd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}a, count is {1} instead of {2}", i, sd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (!sd.ContainsValue(intlValues[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}b, collection doesn't contain value of new item", i);
                } 
                iCountTestcases++;
                if (!sd.ContainsKey(intlValues[i+len])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}c, collection doesn't contain key of new item", i);
                } 
                iCountTestcases++;
                if (!caseInsensitive && sd.ContainsValue(intlValuesLower[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}d, collection contains lowercase value of new item", i);
                } 
                iCountTestcases++;
                if ( !sd.ContainsKey(intlValuesLower[i+len])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}e, collection doesn't contain lowercase key of new item", i);
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
        Co8758ContainsValue_str cbA = new Co8758ContainsValue_str();
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
