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
public class Co8753set_Item
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.Item set";
    public static String s_strTFName        = "Co8753set_Item.cs";
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
            Console.WriteLine("1. set Item on empty collection");
            Console.WriteLine(" (-1)th");
            strLoc = "Loc_001oo"; 
            itm = intl.GetString(MAX_LEN, true, true, true);
            iCountTestcases++;
            try 
            {
                sc[-1] = itm;
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
            Console.WriteLine(" 0th to string");
            iCountTestcases++;
            try 
            {
                sc[0] = itm;
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
            Console.WriteLine(" 0th to null");
            iCountTestcases++;
            try 
            {
                sc[0] = null;
                iCountErrors++;
                Console.WriteLine("Err_0001e, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001f, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("2. set Item on collection with simple strings");
            strLoc = "Loc_002oo";
            sc.Clear(); 
            iCountTestcases++;
            sc.AddRange(values);
            int cnt = values.Length;
            if (sc.Count != cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sc.Count, cnt);
            } 
            for (int i = 0; i < cnt; i++) 
            {
                iCountTestcases++;
                sc[i] = values[cnt-i-1];
                if (String.Compare(sc[i], values[cnt-i-1], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, value is {1} instead of {2}", i, sc[i], values[cnt-i-1]);
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
            sc.Clear();
            cnt = intlValues.Length;
            sc.AddRange(intlValues);
            if ( sc.Count != intlValues.Length ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sc.Count, intlValues.Length);
            } 
            for (int i = cnt; i < cnt; i++) 
            {
                iCountTestcases++;
                sc[i] = intlValues[cnt-i-1];
                iCountTestcases++;
                if (String.Compare(sc[i], intlValues[cnt-i-1], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, actual item is {1} instead of {2}", i, sc[i], intlValues[cnt-i-1]);
                } 
            }
            Console.WriteLine("4. case sensitivity");
            strLoc = "Loc_004oo"; 
            string intlStrUpper = intlValues[0];
            intlStrUpper = intlStrUpper.ToUpper();
            string intlStrLower = intlStrUpper.ToLower();
            sc.Clear();
            sc.AddRange(values);
            sc.Add(intlStrUpper);
            iCountTestcases++;
            if ( sc.Count != values.Length + 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", sc.Count, values.Length + 1);
            } 
            sc[0] = intlStrLower;
            iCountTestcases++;
            if (!caseInsensitive && (String.Compare(sc[0], intlStrUpper, false) == 0)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, set to uppercase when should have to lower");
            } 
            iCountTestcases++;
            if (String.Compare(sc[0], intlStrLower, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004c, disn't set to lower");
            } 
            sc[sc.Count - 1] = intlStrLower;
            iCountTestcases++;
            if (!caseInsensitive && (String.Compare(sc[sc.Count - 1], intlStrUpper, false) == 0)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, didn't set from uppercase to lowercase ");
            } 
            iCountTestcases++;
            if (String.Compare(sc[sc.Count - 1], intlStrLower, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004c, disn't set to lower");
            } 
            Console.WriteLine("5. set to null");
            strLoc = "Loc_005oo"; 
            if (sc.Count < 1)
                sc.AddRange(values); 
            int ind = sc.Count / 2;
            sc[ind] = null;
            iCountTestcases++;
            if (sc[ind] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, failed to set to null");
            } 
            iCountTestcases++;
            if (!sc.Contains(null)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, Contains() didn't return truw for null");
            } 
            iCountTestcases++;
            if (sc.IndexOf(null) != ind) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005c, IndexOf() returned {0} instead of {1}", sc.IndexOf(null), ind);
            } 
            Console.WriteLine("6. set [-1] to string");
            sc.Clear();
            sc.AddRange(intlValues);
            Console.WriteLine(" collection contains {0} items", sc.Count);
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            try 
            {
                sc[-1] = intlStrUpper;
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
            Console.WriteLine("7. set [Count] to string");
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            try 
            {
                sc[sc.Count] = intlStrUpper;
                iCountErrors++;
                Console.WriteLine("Err_0007a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("8. set (Count + 1) to string");
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            try 
            {
                sc[sc.Count + 1] = intlStrUpper;
                iCountErrors++;
                Console.WriteLine("Err_0008a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("9. set [Count] to null");
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            try 
            {
                sc[sc.Count] = null;
                iCountErrors++;
                Console.WriteLine("Err_0009a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009b, unexpected exception: {0}", e.ToString());
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
        Co8753set_Item cbA = new Co8753set_Item();
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
