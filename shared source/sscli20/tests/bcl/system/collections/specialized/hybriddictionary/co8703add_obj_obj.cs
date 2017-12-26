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
public class Co8703Add_obj_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "HybridDictionary.Add(object, object)";
    public static String s_strTFName        = "Co8703Add_obj_obj.cs";
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
        HybridDictionary hd; 
        const int BIG_LENGTH = 100;
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
        int cnt = 0;            
        try
        {
            intl = new IntlStrings(); 
            for (int i = 0; i < BIG_LENGTH; i++) 
            {
                valuesLong[i] = "Item" + i;
                keysLong[i] = "keY" + i;
            } 
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            hd = new HybridDictionary();
            Console.WriteLine("1. add few simple strings");
            for (int i = 0; i < valuesShort.Length; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(keysShort[i], valuesShort[i]);
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}a, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (String.Compare(hd[keysShort[i]].ToString(), valuesShort[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}b, returned item \"{1}\" instead of \"{2}\"", i, hd[keysShort[i]], valuesShort[i]);
                } 
            }
            Console.WriteLine(" .. increase number of items");
            for (int i = 0; i < valuesLong.Length; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(keysLong[i], valuesLong[i]);
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}c, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}d, returned item \"{1}\" instead of \"{2}\"", i, hd[keysLong[i]], valuesLong[i]);
                } 
            }
            Console.WriteLine("2. add few intl strings");
            int len = valuesShort.Length;
            hd.Clear();
            string [] intlValues = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
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
            Console.WriteLine(" initial number of items: " + hd.Count);
            strLoc = "Loc_002oo"; 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(intlValues[i+len], intlValues[i]);
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}a, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (String.Compare(hd[intlValues[i+len]].ToString(), intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned item \"{1}\" instead of \"{2}\"", i, hd[intlValues[i+len]], intlValues[i]);
                } 
            }
            Console.WriteLine(" .. increase number of items");
            for (int i = 0; i < valuesLong.Length; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(keysLong[i], intlValues[1]);
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), intlValues[1], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}d, returned item \"{1}\" instead of \"{2}\"", i, hd[keysLong[i]], intlValues[1]);
                } 
            }
            Console.WriteLine("3. Case sensitivity");
            string [] intlValuesLower = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
            {
                intlValues[i] = intlValues[i].ToUpper();
            }
            for (int i = 0; i < len * 2; i++) 
            {
                intlValuesLower[i] = intlValues[i].ToLower();
            } 
            hd.Clear();
            Console.WriteLine(" initial number of items: " + hd.Count);
            strLoc = "Loc_003oo"; 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(intlValues[i+len], intlValues[i]);
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}a, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if ( hd[intlValues[i+len]] == null ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned null", i);
                } 
                else 
                {
                    if ( ! hd[intlValues[i+len]].Equals(intlValues[i]) ) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0002_{0}c, returned item \"{1}\" instead of \"{2}\"", i, hd[intlValues[i+len]], intlValues[i]);
                    } 
                }
                if ( !caseInsensitive && hd[intlValuesLower[i+len]] != null ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}d, returned non-null", i);
                } 
            }
            Console.WriteLine("4. multiple string with the same key");
            hd.Clear();
            len = valuesShort.Length;
            string k = "keykey";
            hd.Add(k, "value");
            try 
            {
                hd.Add(k, "newvalue");
                iCountErrors++;
                Console.WriteLine("Err_0004a, no exception");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("5. Add (string, null) ");
            Console.WriteLine(" initial number of items: " + hd.Count);
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            cnt = hd.Count;
            k = "kk";
            hd.Add(k, null);
            iCountTestcases++;
            if (hd.Count != cnt+1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", hd.Count, cnt+1);
            } 
            iCountTestcases++;
            if (hd[k] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005c, returned non-null on place of null");
            }
            Console.WriteLine("6. Add (null, string) ");
            Console.WriteLine(" initial number of items: " + hd.Count);
            strLoc = "Loc_006oo"; 
            cnt = hd.Count;
            iCountTestcases++;
            try 
            {
                hd.Add(null, "item");
                iCountErrors++;
                Console.WriteLine("Err_0006a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("7. add duplicate values");
            hd.Clear();
            for (int i = 0; i < valuesShort.Length; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(keysShort[i], "value");
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007_{0}a, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (! hd[keysShort[i]].Equals("value") ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007_{0}b, returned item \"{1}\" instead of \"{2}\"", i, hd[keysShort[i]], "value");
                } 
            }
            iCountTestcases++;
            if ( hd.Keys.Count != valuesShort.Length ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007_c, Keys contains {0} instead of {1}", hd.Keys.Count, valuesShort.Length);
            } 
            iCountTestcases++;
            if ( hd.Values.Count != valuesShort.Length ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007_c, Values contains {0} instead of {1}", hd.Values.Count, valuesShort.Length);
            } 
            Console.WriteLine("8. add many simple strings");
            hd = new HybridDictionary();
            for (int i = 0; i < valuesLong.Length; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(keysLong[i], valuesLong[i]);
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0008_{0}a, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0008_{0}b, returned item \"{1}\" instead of \"{2}\"", i, hd[keysLong[i]], valuesLong[i]);
                } 
            }
            Console.WriteLine(" .. add more items");
            for (int i = 0; i < valuesLong.Length; i++) 
            {
                iCountTestcases++;
                cnt = hd.Count;
                hd.Add(keysLong[i]+"_", valuesLong[i]+i);
                if (hd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0008_{0}c, count is {1} instead of {2}", i, hd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]+"_"].ToString(), valuesLong[i]+i, false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0008_{0}d, returned item \"{1}\" instead of \"{2}\"", i, hd[keysLong[i]+"_"], valuesLong[i]+i);
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
        Co8703Add_obj_obj cbA = new Co8703Add_obj_obj();
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
