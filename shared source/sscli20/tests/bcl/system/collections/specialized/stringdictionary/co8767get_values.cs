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
public class Co8767get_Values
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringDictionary.Values";
    public static String s_strTFName        = "Co8767get_Values.cs";
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
            sd = new StringDictionary();
            Console.WriteLine("1. get Values for empty dictionary");
            iCountTestcases++;
            if (sd.Count > 0)
                sd.Clear();
            if (sd.Values.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, returned Values.Count = {0}", sd.Values.Count);
            }
            Console.WriteLine("2. get Values on filled dictionary");  
            strLoc = "Loc_002oo"; 
            int len = values.Length;
            iCountTestcases++;
            sd.Clear();
            for (int i = 0; i < len; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            if (sd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sd.Count, len);
            } 
            vs = sd.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(string), len);
            vs.CopyTo(arr, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                ind = Array.IndexOf(arr, values[i]);
                if (ind < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002b_{0}, Values doesn't contain \"{1}\" value. Search result: {2}", i, keys[i], ind);
                } 
            }
            Console.WriteLine("3. get Values on dictionary with duplicate values ");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            sd.Clear();
            string intlStr = intl.GetString(MAX_LEN, true, true, true);
            sd.Add("keykey1", intlStr);        
            for (int i = 0; i < len; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            sd.Add("keykey2", intlStr);        
            if (sd.Count != len+2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sd.Count, len+2);
            } 
            iCountTestcases++;
            vs = sd.Values;
            if (vs.Count != sd.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(string), len+2);
            vs.CopyTo(arr, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                ind = Array.IndexOf(arr, values[i]);
                if (ind < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003c_{0}, Values doesn't contain \"{1}\" value", i, keys[i]);
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
            sd.Clear();
            for (int i = 0; i < len; i++) 
            {
                sd.Add(intlValues[i+len], intlValues[i]);
            }
            if (sd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", sd.Count, len);
            }
            vs = sd.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(string), len);
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
            Console.WriteLine("5. Change dictinary and verify case sensitivity");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            sd.Clear();
            for (int i = 0; i < len; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            if (sd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", sd.Count, len);
            } 
            vs = sd.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, returned Values.Count = {0}", vs.Count);
            }
            Console.WriteLine("     - remove element from the dictionary");
            sd.Remove(keys[0]);
            if (sd.Count != len-1) 
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
            arr = Array.CreateInstance(typeof(string), sd.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, values[0]);
            if (ind >= 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005d, Values still contains removed value " + ind);
            } 
            Console.WriteLine("     - add element to the dictionary");
            sd.Add(keys[0], "new item");
            if (sd.Count != len) 
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
            arr = Array.CreateInstance(typeof(string), sd.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, "new item");
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005g, Values doesn't contain added value ");
            } 
            Console.WriteLine("     - set to uppercase");
            sd[keys[0]] = "UPPERCASE";
            if (sd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005h, Count changed after set");
            } 
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005j, Values.Count changed after set");
            }
            iCountTestcases++;
            arr = Array.CreateInstance(typeof(string), sd.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, "UPPERCASE");
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005k, Values doesn't contain value we just set ");
            } 
            ind = Array.IndexOf(arr, "uppercase");
            if (ind >= 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005m, Values contains lowercase version of value we just set ");
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
        Co8767get_Values cbA = new Co8767get_Values();
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
