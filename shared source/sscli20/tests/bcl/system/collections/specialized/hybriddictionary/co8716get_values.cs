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
public class Co8716get_Values
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "HybridDictionary.Values";
    public static String s_strTFName        = "Co8716get_Values.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public const int MAX_LEN = 50;          
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        const int BIG_LENGTH = 100;
        String strLoc = "Loc_000oo";
        HybridDictionary hd; 
        string [] valuesShort = 
        {
            "",
            " ",
            "$%^#",
            System.DateTime.Today.ToString(),
            Int32.MaxValue.ToString()
        };
        string [] keysShort = 
        {
            Int32.MaxValue.ToString(),
            " ",
            System.DateTime.Today.ToString(),
            "",
            "$%^#"
        };
        string [] valuesLong = new string[BIG_LENGTH];
        string [] keysLong = new string[BIG_LENGTH];
        Array arr;
        ICollection vs;         
        int ind;
        try
        {
            for (int i = 0; i < BIG_LENGTH; i++) 
            {
                valuesLong[i] = "Item" + i;
                keysLong[i] = "keY" + i;
            } 
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            hd = new HybridDictionary();
            Console.WriteLine("1. get Values for empty dictionary");
            iCountTestcases++;
            if (hd.Count > 0)
                hd.Clear();
            if (hd.Values.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, returned Values.Count = {0}", hd.Values.Count);
            }
            Console.WriteLine("2. get Values on short filled dictionary");  
            strLoc = "Loc_002oo"; 
            int len = valuesShort.Length;
            iCountTestcases++;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysShort[i], valuesShort[i]);
            }
            Console.WriteLine("   Count = " + hd.Count);
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", hd.Count, len);
            } 
            vs = hd.Values;
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
                ind = Array.IndexOf(arr, valuesShort[i]);
                if (ind < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002b_{0}, Values doesn't contain \"{1}\" value. Search result: {2}", i, valuesShort[i], ind);
                } 
            }
            Console.WriteLine("3. get Values on long filled dictionary");  
            strLoc = "Loc_003oo"; 
            len = valuesLong.Length;
            iCountTestcases++;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            Console.WriteLine("   Count = " + hd.Count);
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", hd.Count, len);
            } 
            vs = hd.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(Object), len);
            vs.CopyTo(arr, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                ind = Array.IndexOf(arr, valuesLong[i]);
                if (ind < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003b_{0}, Values doesn't contain \"{1}\" value. Search result: {2}", i, valuesLong[i], ind);
                } 
            }
            Console.WriteLine("4. get Values on short dictionary with different_in_casing_only keys ");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            hd.Clear();
            string intlStr = "intlStr";
            hd.Add("keykey", intlStr);        
            hd.Add("keyKey", intlStr);        
            if (hd.Count != 2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", hd.Count, 2);
            } 
            Console.WriteLine("    Count = " + hd.Count);
            iCountTestcases++;
            vs = hd.Values;
            if (vs.Count != hd.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(Object), 2);
            vs.CopyTo(arr, 0);
            iCountTestcases++;
            ind = Array.IndexOf(arr, intlStr);
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, Values doesn't contain {0} value", intlStr);
            } 
            Console.WriteLine("5. get Values on long dictionary with different_in_casing_only keys ");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            hd.Clear();
            hd.Add("keykey", intlStr);        
            for (int i = 0; i < BIG_LENGTH; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            hd.Add("keyKey", intlStr);        
            if (hd.Count != BIG_LENGTH + 2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", hd.Count, BIG_LENGTH + 2);
            } 
            Console.WriteLine("    Count = " + hd.Count);
            iCountTestcases++;
            vs = hd.Values;
            if (vs.Count != hd.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, returned Values.Count = {0}", vs.Count);
            }
            arr = Array.CreateInstance(typeof(Object), BIG_LENGTH + 2);
            vs.CopyTo(arr, 0);
            for (int i = 0 ; i < BIG_LENGTH; i++) 
            {
                iCountTestcases++;
                if ( Array.IndexOf(arr, valuesLong[i]) < 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005c_{1}, Values doesn't contain {0} value", valuesLong[i], i);
                }
            }
            iCountTestcases++;
            ind = Array.IndexOf(arr, intlStr);
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005d, Values doesn't contain {0} value", intlStr);
            } 
            Console.WriteLine("6. Change long dictinary");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            hd.Clear();
            len = valuesLong.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            Console.WriteLine("    Count = " + hd.Count);
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a, count is {0} instead of {1}", hd.Count, len);
            } 
            vs = hd.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a, returned Values.Count = {0}", vs.Count);
            }
            Console.WriteLine("     - remove element from the dictionary");
            hd.Remove(keysLong[0]);
            if (hd.Count != len-1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b, didn't remove element");
            } 
            if (vs.Count != len-1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006c, Values were not updated after removal");
            }
            iCountTestcases++;
            arr = Array.CreateInstance(typeof(Object), hd.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, valuesLong[0]);
            if (ind >= 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006d, Values still contains removed value " + ind);
            } 
            Console.WriteLine("     - add element to the dictionary");
            hd.Add(keysLong[0], "new item");
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006e, didn't add element");
            } 
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006f, Values were not updated after addition");
            }
            iCountTestcases++;
            arr = Array.CreateInstance(typeof(Object), hd.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, "new item");
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006g, Values doesn't contain added value ");
            } 
            Console.WriteLine("7. Change short dictinary");
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            hd.Clear();
            len = valuesShort.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysShort[i], valuesShort[i]);
            }
            Console.WriteLine("    Count = " + hd.Count);
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007a, count is {0} instead of {1}", hd.Count, len);
            } 
            vs = hd.Values;
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007a, returned Values.Count = {0}", vs.Count);
            }
            Console.WriteLine("     - remove element from the dictionary");
            hd.Remove(keysShort[0]);
            if (hd.Count != len-1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007b, didn't remove element");
            } 
            if (vs.Count != len-1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007c, Values were not updated after removal");
            }
            iCountTestcases++;
            arr = Array.CreateInstance(typeof(Object), hd.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, valuesShort[0]);
            if (ind >= 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007d, Values still contains removed value " + ind);
            } 
            Console.WriteLine("     - add element to the dictionary");
            hd.Add(keysShort[0], "new item");
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007e, didn't add element");
            } 
            if (vs.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007f, Values were not updated after addition");
            }
            iCountTestcases++;
            arr = Array.CreateInstance(typeof(Object), hd.Count);
            vs.CopyTo(arr, 0);
            ind = Array.IndexOf(arr, "new item");
            if (ind < 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007g, Values doesn't contain added value ");
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
        Co8716get_Values cbA = new Co8716get_Values();
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
