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
public class Co8698get_Values
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ListDictionary.Values";
    public static String s_strTFName        = "Co8698get_Values.cs";
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
        ListDictionary ld; 
        string [] values = 
        {
            "",
            " ",
            "a",
            "aa",
            "tExt",
            "     spAces",
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
        Array arr;
        ICollection vs;         
        int ind;
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            ld = new ListDictionary();
            Console.WriteLine("1. get Values for empty dictionary");
            iCountTestcases++;
            if (ld.Count > 0)
                ld.Clear();
            if (ld.Values.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, returned Values.Count = {0}", ld.Values.Count);
            }
            Console.WriteLine("2. get Values on filled dictionary");  
            strLoc = "Loc_002oo"; 
            int len = values.Length;
            iCountTestcases++;
            ld.Clear();
            for (int i = 0; i < len; i++) 
            {
                ld.Add(keys[i], values[i]);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", ld.Count, len);
            } 
            vs = ld.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(Object), len);
            vs.CopyTo(arr, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                ind = Array.IndexOf(arr, values[i]);
                if (ind < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002b_{0}, Values doesn't contain \"{1}\" value. Search result: {2}", i, values[i], ind);
                } 
            }
            Console.WriteLine("3. get Values on dictionary with different_in_casing_only keys ");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            ld.Clear();
            string intlStr = intl.GetString(MAX_LEN, true, true, true);
            ld.Add("keykey", intlStr);        
            for (int i = 0; i < len; i++) 
            {
                ld.Add(keys[i], values[i]);
            }
            ld.Add("keyKey", intlStr);        
            if (ld.Count != len+2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", ld.Count, len+2);
            } 
            iCountTestcases++;
            vs = ld.Values;
            if (vs.Count != ld.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(Object), len+2);
            vs.CopyTo(arr, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                ind = Array.IndexOf(arr, values[i]);
                if (ind < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003c_{0}, Values doesn't contain \"{1}\" value", i, values[i]);
                } 
            }
            iCountTestcases++;
            ind = Array.IndexOf(arr, intlStr);
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, Values doesn't contain {0} value", intlStr);
            } 
            Console.WriteLine("4. get Values for dictionary with intl strings");
            strLoc = "Loc_004oo"; 
            string [] intlValues = new string [len*2];
            for (int i = 0; i < len*2; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            ld.Clear();
            for (int i = 0; i < len; i++) 
            {
                ld.Add(intlValues[i+len], intlValues[i]);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", ld.Count, len);
            }
            vs = ld.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(Object), len);
            vs.CopyTo(arr, 0);
            for (int i = 0; i < arr.Length; i++) 
            {
                iCountTestcases++;
                ind = Array.IndexOf(arr, intlValues[i]);
                if (ind < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004c_{0}, Values doesn't contain \"{1}\" value", i, intlValues[i]);
                } 
            }
            Console.WriteLine("5. Change dictinary");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            ld.Clear();
            for (int i = 0; i < len; i++) 
            {
                ld.Add(keys[i], values[i]);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", ld.Count, len);
            } 
            vs = ld.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, returned Values.Count = {0}", vs.Count);
            }
            Console.WriteLine("     - remove element from the dictionary");
            ld.Remove(keys[0]);
            if (ld.Count != len-1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, didn't remove element");
            } 
            if (vs.Count != len-1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005c, Values were not updated after removal");
            }
            iCountTestcases++;
            arr = Array.CreateInstance(typeof(Object), ld.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, values[0]);
            if (ind >= 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005d, Values still contains removed value " + ind);
            } 
            Console.WriteLine("     - add element to the dictionary");
            ld.Add(keys[0], "new item");
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005e, didn't add element");
            } 
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005f, Values were not updated after addition");
            }
            iCountTestcases++;
            arr = Array.CreateInstance(typeof(Object), ld.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, "new item");
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005g, Values doesn't contain added value ");
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
    void printArr(Array ar) 
    {
        string temp = "";
        for (int i = 0; i < ar.Length; i++) 
        {
            temp += System.Environment.NewLine + ar.GetValue(i).ToString();
        }
        Console.WriteLine(temp);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co8698get_Values cbA = new Co8698get_Values();
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
