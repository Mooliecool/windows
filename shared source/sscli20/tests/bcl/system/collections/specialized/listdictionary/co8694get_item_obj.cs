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
internal class Co8694_InsensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, true);
    }
}
public class Co8694get_Item_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ListDictionary.Item(object)";
    public static String s_strTFName        = "Co8694get_Item_obj.cs";
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
        Object itm;
        string [] values = 
        {
            "",
            " ",
            "a",
            "aA",
            "text",
            "     SPaces",
            "1",
            "$%^#",
            "2222222222222222222222222",
            System.DateTime.Today.ToString(),
            Int32.MaxValue.ToString()
        };
        string [] keys = 
        {
            "zero",
            "oNe",
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
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            ld = new ListDictionary();
            Console.WriteLine("1. Item() on empty dictionary");
            iCountTestcases++;
            cnt = ld.Count;
            Console.WriteLine("     - Item(null)");
            try 
            {
                itm = ld[null];
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
            cnt = ld.Count;
            Console.WriteLine("     - Item(some_string)");
            itm = ld["some_string"];
            if (itm != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001c, returned non=null");
            }
            Console.WriteLine("2. add simple strings, access via Item(Object)");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = ld.Count;
            int len = values.Length;
            for (int i = 0; i < len; i++) 
            {
                ld.Add(keys[i], values[i]);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", ld.Count, values.Length);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( !ld.Contains(keys[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, doesn't contain key", i);
                }  
                iCountTestcases++;
                if (String.Compare(ld[keys[i]].ToString(), values[i], false) != 0)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, returned wrong value", i);
                }
            }
            Console.WriteLine("3. add intl strings and access via Item()");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            string [] intlValues = new string [len * 2];
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
            iCountTestcases++;
            cnt = ld.Count;
            for (int i = 0; i < len; i++) 
            {
                ld.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( ld.Count != (cnt+len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", ld.Count, cnt+len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (! ld.Contains(intlValues[i+len]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, doesn't contain key", i);
                }  
                iCountTestcases++;
                if (String.Compare(ld[intlValues[i+len]].ToString(), intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}c, returned wrong value", i);
                } 
            }
            Console.WriteLine("4. case sensitivity");
            strLoc = "Loc_004oo"; 
            string [] intlValuesLower = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
            {
                intlValues[i] = intlValues[i].ToUpper();
            }
            for (int i = 0; i < len * 2; i++) 
            {
                intlValuesLower[i] = intlValues[i].ToLower();
            } 
            ld.Clear();
            Console.WriteLine("   - add uppercase and access using uppercase");
            for (int i = 0; i < len; i++) 
            {
                ld.Add(intlValues[i+len], intlValues[i]);     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( !ld.Contains( intlValues[i+len])  ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}b, doesn't contain key", i);
                } 
                iCountTestcases++;
                if (String.Compare(ld[intlValues[i+len]].ToString(), intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}c, returned wrong value", i);
                } 
            } 
            ld.Clear();
            Console.WriteLine("   - add uppercase but access using lowercase");
            for (int i = 0; i < len; i++) 
            {
                ld.Add(intlValues[i+len], intlValues[i]);     
            }
            for (int i = 0; i < len; i++) 
            {
                cnt = ld.Count;
                iCountTestcases++;
                if ( !caseInsensitive && ld[intlValuesLower[i+len]] != null ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}d, failed: returned non-null for lowercase key", i);
                } 
            } 
            Console.WriteLine("5. access Item() on LD with insensitive comparer");
            ld = new ListDictionary(new Co8694_InsensitiveComparer());
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            len = values.Length;
            ld.Clear();
            string kk = "key";
            for (int i = 0; i < len; i++) 
            {
                ld.Add(kk+i, values[i]);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", ld.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (ld[kk.ToUpper() + i] == null) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005b_{0}, returned null for differently cased key", i);
                }
                else 
                {
                    if (String.Compare(ld[kk.ToUpper() + i].ToString(), values[i], false) != 0) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0005b_{0}, returned wrong value", i);
                    } 
                }
            }
            Console.WriteLine("6. Item(null) for filled LD");
            ld = new ListDictionary();
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            cnt = ld.Count;
            if (ld.Count < len) 
            {
                ld.Clear();
                for (int i = 0; i < len; i ++) 
                {
                    ld.Add(keys[i], values[i]);
                } 
            }
            iCountTestcases++;
            try 
            {
                itm = ld[null];
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
        Co8694get_Item_obj cbA = new Co8694get_Item_obj();
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
