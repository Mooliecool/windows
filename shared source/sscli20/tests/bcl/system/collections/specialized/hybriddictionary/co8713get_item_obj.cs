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
public class Co8713get_Item_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "HybridDictionary.Item(object)";
    public static String s_strTFName        = "Co8713get_Item_obj.cs";
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
        Object itm;         
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
            Console.WriteLine("1. get Item() on empty dictionary");
            iCountTestcases++;
            cnt = hd.Count;
            Console.WriteLine("     - Item(null)");
            try 
            {
                itm = hd[null];
                iCountErrors++;
                Console.WriteLine("Err_0001a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, unexpected exception: {0}", e.ToString());
            }
            iCountTestcases++;
            cnt = hd.Count;
            Console.WriteLine("     - Item(some_string)");
            itm = hd["some_string"];
            if (itm != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001c, returned non-null for Item(some_string)");
            }
            Console.WriteLine("2. add few simple strings, get Item(Object)");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = hd.Count;
            int len = valuesShort.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysShort[i], valuesShort[i]);
            }
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", hd.Count, valuesShort.Length);
            } 
            for (int i = 0; i < len; i++) 
            {
                cnt = hd.Count;
                iCountTestcases++;
                itm = hd[keysShort[i]];
                if (String.Compare(itm.ToString(), valuesShort[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned wrong item", i);
                } 
            }
            Console.WriteLine("3. add many simple strings, get Item(Object)");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            hd.Clear();
            cnt = hd.Count;
            len = valuesLong.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", hd.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                cnt = hd.Count;
                iCountTestcases++;
                itm = hd[keysLong[i]];
                if (String.Compare(itm.ToString(), valuesLong[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, returned wrong item", i);
                } 
            }
            Console.WriteLine("4. add many intl strings and get Item()");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            len = valuesLong.Length;
            string [] intlValues = new string [len * 2];
            for (int i = 0; i < len*2; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            iCountTestcases++;
            cnt = hd.Count;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( hd.Count != (cnt+len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", hd.Count, cnt+len);
            } 
            for (int i = 0; i < len; i++) 
            {
                cnt = hd.Count;
                iCountTestcases++;
                itm = hd[intlValues[i+len]];
                if (string.Compare(itm.ToString(), intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}b, returned wrong item", i);
                } 
            }
            Console.WriteLine("5. add few intl strings and get Item()");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            len = valuesShort.Length;
            iCountTestcases++;
            hd.Clear();
            cnt = hd.Count;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( hd.Count != (cnt+len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", hd.Count, cnt+len);
            } 
            for (int i = 0; i < len; i++) 
            {
                cnt = hd.Count;
                iCountTestcases++;
                itm = hd[intlValues[i+len]];
                if (String.Compare(itm.ToString(), intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}b, returned wrong item", i);
                } 
            }
            Console.WriteLine("6. case sensitivity - hashtable");
            strLoc = "Loc_006oo"; 
            len = valuesLong.Length;
            hd.Clear();
            Console.WriteLine("   - add uppercase and access via uppercase");
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i].ToUpper(), valuesLong[i].ToUpper());     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                itm = hd[keysLong[i].ToUpper()];
                if (String.Compare(itm.ToString(), valuesLong[i].ToUpper(), false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}b, returned wrong item", i);
                } 
                iCountTestcases++;
                if (String.Compare(itm.ToString(), valuesLong[i].ToLower(), false) == 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}c, returned lowercase when added uppercase", i);
                } 
            } 
            hd.Clear();
            Console.WriteLine("   - add uppercase but access via lowercase");
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i].ToUpper(), valuesLong[i].ToUpper());     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                itm = hd[keysLong[i].ToLower()];
                if (itm != null ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}d, returned non-null for lowercase key", i);
                } 
            } 
            Console.WriteLine("7. case sensitivity - list");
            strLoc = "Loc_007oo";
            len = valuesShort.Length;
            Console.WriteLine("   ** fill with {0} elements", len); 
            hd.Clear();
            Console.WriteLine("   - add uppercase and access via uppercase");
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i].ToUpper(), valuesLong[i].ToUpper());     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                itm = hd[keysLong[i].ToUpper()];
                if (String.Compare(itm.ToString(), valuesLong[i].ToUpper(), false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007_{0}b, returned wrong item", i);
                } 
                iCountTestcases++;
                if (String.Compare(itm.ToString(), valuesLong[i].ToLower(), false) == 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007_{0}c, returned lowercase when added uppercase", i);
                } 
            } 
            hd.Clear();
            Console.WriteLine("   - add uppercase but access via lowercase");
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i].ToUpper(), valuesLong[i].ToUpper());     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                itm = hd[keysLong[i].ToLower()];
                if (itm != null ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007_{0}d, returned non-null for lowercase key", i);
                } 
            } 
            Console.WriteLine("8. get Item() on case-insensitive HD - list");
            hd = new HybridDictionary(true);
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            len = valuesShort.Length;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i].ToLower(), valuesLong[i].ToLower());
            }
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008a, count is {0} instead of {1}", hd.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                itm = hd[keysLong[i].ToUpper()];
                if (String.Compare(itm.ToString(), valuesLong[i].ToLower(), false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0008b_{0}, returned wrong item for uppercase key", i);
                } 
                iCountTestcases++;
                itm = hd[keysLong[i].ToLower()];
                if (String.Compare(itm.ToString(), valuesLong[i].ToLower(), false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0008b_{0}, returned wrong item - for lowercase key", i);
                } 
            }
            Console.WriteLine("9. get Item() on case-insensitive HD - hashtable");
            hd = new HybridDictionary(true);
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            len = valuesLong.Length;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i].ToLower(), valuesLong[i].ToLower());
            }
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009a, count is {0} instead of {1}", hd.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                itm = hd[keysLong[i].ToUpper()];
                if (String.Compare(itm.ToString(), valuesLong[i].ToLower(), false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0009b_{0}, returned wrong item for uppercase key", i);
                } 
                iCountTestcases++;
                itm = hd[keysLong[i].ToLower()];
                if (String.Compare(itm.ToString(), valuesLong[i].ToLower(), false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0009c_{0}, returned wrong item for lowercase key", i);
                } 
            }
            Console.WriteLine("10. get Item(null) for filled LD - list");
            hd = new HybridDictionary();
            strLoc = "Loc_0010oo"; 
            iCountTestcases++;
            len = valuesShort.Length;
            hd.Clear();
            for (int i = 0; i < len; i ++) 
            {
                hd.Add(keysShort[i], valuesShort[i]);
            }
            iCountTestcases++;
            try 
            {
                itm = hd[null];
                iCountErrors++;
                Console.WriteLine("Err_0010a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("11. get Item(null) for filled LD - hashtable");
            hd = new HybridDictionary();
            strLoc = "Loc_0011oo"; 
            iCountTestcases++;
            len = valuesLong.Length;
            hd.Clear();
            for (int i = 0; i < len; i ++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            iCountTestcases++;
            try 
            {
                itm = hd[null];
                iCountErrors++;
                Console.WriteLine("Err_0011a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0011b, unexpected exception: {0}", e.ToString());
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
        Co8713get_Item_obj cbA = new Co8713get_Item_obj();
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
