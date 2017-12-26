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
public class Co8751get_Item
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.Item get";
    public static String s_strTFName        = "Co8751get_Item.cs";
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
        string itm;         
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
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            Console.WriteLine("1. get Item from empty collection");
            Console.WriteLine(" (-1)th");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            try 
            {
                itm = sc[-1];
                iCountErrors++;
                Console.WriteLine("Err_0001a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine(" 0th");
            iCountTestcases++;
            try 
            {
                itm = sc[0];
                iCountErrors++;
                Console.WriteLine("Err_0001c, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001d, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("2. Get Item on collection with simple strings");
            strLoc = "Loc_002oo";
            sc.Clear(); 
            iCountTestcases++;
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if (String.Compare(sc[i], values[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned {1} instead of {2}", i, sc[i], values[i]);
                } 
            }
            Console.WriteLine("3. get Item on collection with intl strings");
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
            int cnt = sc.Count;
            sc.AddRange(intlValues);
            if ( sc.Count != (cnt + intlValues.Length) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sc.Count, cnt + intlValues.Length);
            } 
            cnt = values.Length;
            for (int i = cnt; i < cnt + intlValues.Length; i++) 
            {
                iCountTestcases++;
                if (String.Compare(sc[i], intlValues[i-cnt], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, returned {1} instead of {2}", i, sc[i], intlValues[i-cnt]);
                } 
            }
            Console.WriteLine("4. case sensitivity");
            strLoc = "Loc_004oo"; 
            string intlStr = intlValues[0];
            intlStr = intlStr.ToUpper();
            sc.Clear();
            sc.Add(intlStr);            
            iCountTestcases++;
            if ( sc.Count != 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of 1", sc.Count);
            } 
            iCountTestcases++;
            if (!caseInsensitive && (String.Compare(sc[0], intlValues[0].ToLower(), false) == 0)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, returned unexpected result: {0} when should have return all upper", sc[0]);
            } 
            iCountTestcases++;
            if (String.Compare(sc[0], intlStr, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004c, returned {0} instead of {1}", sc[0], intlStr);
            } 
            Console.WriteLine("4. get [-1]");
            sc.Clear();
            sc.AddRange(intlValues);
            Console.WriteLine(" collection contains {0} items", sc.Count);
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            try 
            {
                itm = sc[-1];
                iCountErrors++;
                Console.WriteLine("Err_0004a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("5. get [Count]");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            try 
            {
                itm = sc[sc.Count];
                iCountErrors++;
                Console.WriteLine("Err_0005a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("6. get [Count + 1]");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            try 
            {
                itm = sc[sc.Count + 1];
                iCountErrors++;
                Console.WriteLine("Err_0006a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b, unexpected exception: {0}", e.ToString());
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
        Co8751get_Item cbA = new Co8751get_Item();
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
