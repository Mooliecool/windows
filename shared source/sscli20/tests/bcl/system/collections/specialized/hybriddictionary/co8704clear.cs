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
public class Co8704Clear
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "HybridDictionary.Clear()";
    public static String s_strTFName        = "Co8704Clear.cs";
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
            strLoc = "Loc_001_oo"; 
            iCountTestcases++;
            hd = new HybridDictionary();
            cnt = hd.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001_, count is {0} instead of {1} after default ctor", hd.Count, 0);
            }
            Console.WriteLine("1. call Clear() on empty dictionary");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            hd.Clear();
            cnt = hd.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, count is {0} instead of {1} after Clear()", hd.Count, 0);
            }
            iCountTestcases++;
            cnt = hd.Keys.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, Keys.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            iCountTestcases++;
            cnt = hd.Values.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001c, Values.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            Console.WriteLine("2. add few simple strings and Clear()");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = hd.Count;
            for (int i = 0; i < valuesShort.Length; i++) 
            {    
                hd.Add(keysShort[i], valuesShort[i]);
            }
            if (hd.Count != valuesShort.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", hd.Count, valuesShort.Length);
            }
            iCountTestcases++;
            hd.Clear();
            cnt = hd.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, count is {0} instead of {1} after Clear()", hd.Count, 0);
            }
            iCountTestcases++;
            cnt = hd.Keys.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002c, Keys.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            iCountTestcases++;
            cnt = hd.Values.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002d, Values.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            Console.WriteLine("3. add intl strings and Clear()");
            strLoc = "Loc_003oo"; 
            int len = valuesShort.Length;
            string [] intlValues = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            cnt = hd.Count;
            iCountTestcases++;
            for (int i = 0; i < len; i++) 
            {    
                hd.Add(intlValues[i+len], intlValues[i]);
            }
            if (hd.Count != (cnt + len)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", hd.Count, cnt + len);
            }
            iCountTestcases++;
            hd.Clear();
            cnt = hd.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, count is {0} instead of {1} after Clear()", hd.Count, 0);
            }
            iCountTestcases++;
            cnt = hd.Keys.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, Keys.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            iCountTestcases++;
            cnt = hd.Values.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003d, Values.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            Console.WriteLine("4. add many simple strings and Clear()");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            hd.Clear();
            for (int i = 0; i < valuesLong.Length; i++) 
            {    
                hd.Add(keysLong[i], valuesLong[i]);
            }
            if (hd.Count != valuesLong.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", hd.Count, valuesLong.Length);
            }
            iCountTestcases++;
            hd.Clear();
            cnt = hd.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, count is {0} instead of {1} after Clear()", hd.Count, 0);
            }
            iCountTestcases++;
            cnt = hd.Keys.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004c, Keys.Count is {0} instead of {1} after Clear()", cnt, 0);
            }
            iCountTestcases++;
            cnt = hd.Values.Count;
            if ( cnt != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004d, Values.Count is {0} instead of {1} after Clear()", cnt, 0);
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
        Co8704Clear cbA = new Co8704Clear();
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
