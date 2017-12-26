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
public class Co8731Remove_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.Remove(string)";
    public static String s_strTFName        = "Co8731Remove_str.cs";
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
        NameValueCollection nvc; 
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
        string [] ks;           
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection();
            Console.WriteLine("1. Remove() on empty collection");
            iCountTestcases++;
            cnt = nvc.Count;
            Console.WriteLine("     - Remove(null)");
            nvc.Remove(null);
            if (nvc.Count != cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, changed collection after Remove(null)");
            }
            iCountTestcases++;
            cnt = nvc.Count;
            Console.WriteLine("     - Remove(some_string)");
            nvc.Remove("some_string");
            if (nvc.Count != cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, changed collection after Remove(some_string)");
            }
            Console.WriteLine("2. add simple strings, remove them via Remove(string)");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            int len = values.Length;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(keys[i], values[i]);
            }
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", nvc.Count, values.Length);
            } 
            for (int i = 0; i < len; i++) 
            {
                cnt = nvc.Count;
                iCountTestcases++;
                nvc.Remove(keys[i]);
                if (nvc.Count != cnt - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned: failed to remove item", i);
                } 
                ks = nvc.AllKeys;
                iCountTestcases++;
                if ( Array.IndexOf(ks, keys[i]) > -1 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, removed wrong item", i);
                }  
            }
            Console.WriteLine("3. add intl strings and Remove()");
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
            iCountTestcases++;
            cnt = nvc.Count;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( nvc.Count != (cnt+len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", nvc.Count, cnt+len);
            } 
            for (int i = 0; i < len; i++) 
            {
                cnt = nvc.Count;
                iCountTestcases++;
                nvc.Remove(intlValues[i+len]);
                if (nvc.Count != cnt - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, returned: failed to remove item", i);
                } 
                ks = nvc.AllKeys;
                iCountTestcases++;
                if ( Array.IndexOf(ks, intlValues[i+len]) > -1 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}c, removed wrong item", i);
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
            nvc.Clear();
            Console.WriteLine("   - add uppercase and remove uppercase");
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);     
            }
            for (int i = 0; i < len; i++) 
            {
                cnt = nvc.Count;
                iCountTestcases++;
                nvc.Remove(intlValues[i+len]);
                if (nvc.Count != cnt - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}b, returned: failed to remove item", i);
                } 
                ks = nvc.AllKeys;
                iCountTestcases++;
                if ( Array.IndexOf(ks, intlValues[i+len]) > -1 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}c, removed wrong item", i);
                } 
            } 
            nvc.Clear();
            Console.WriteLine("   - add uppercase but remove lowercase");
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);     
            }
            for (int i = 0; i < len; i++) 
            {
                cnt = nvc.Count;
                iCountTestcases++;
                nvc.Remove(intlValuesLower[i+len]);
                if (nvc.Count != cnt - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}d, returned: failed to remove item using lowercase key", i);
                } 
                ks = nvc.AllKeys;
                iCountTestcases++;
                if ( Array.IndexOf(ks, intlValues[i+len]) > -1 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}e, removed wrong item using lowercase key", i);
                } 
            } 
            Console.WriteLine("5. multiple items with same key and Remove()");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            nvc.Clear();
            len = values.Length;
            string k = "keykey";
            string k1 = "hm1";
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(k, "Value"+i);
                nvc.Add(k1, "iTem"+i);
            }
            if (nvc.Count != 2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", nvc.Count, 2);
            } 
            iCountTestcases++;
            nvc.Remove(k);
            if (nvc.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, failed to remove item");
            } 
            ks = nvc.AllKeys;
            iCountTestcases++;
            if ( Array.IndexOf(ks, k) > -1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005_c, removed wrong item");
            } 
            iCountTestcases++;
            nvc.Remove(k1);
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005d, failed to remove item");
            } 
            ks = nvc.AllKeys;
            iCountTestcases++;
            if ( Array.IndexOf(ks, k1) > -1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005_e, removed wrong item");
            } 
            Console.WriteLine("6. Remove(null) when there is item with null key");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            nvc.Add(null, "nullValue");
            if (nvc.Count != cnt + 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a, failed to add item with null-key");
            }
            if (nvc[null] == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b, didn't add item with null-key");
            }
            iCountTestcases++;
            cnt = nvc.Count;
            nvc.Remove(null);
            if (nvc.Count != cnt - 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006c, failed to remove item");
            }
            if (nvc[null] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006d, didn't remove item with null-key");
            } 
            Console.WriteLine("7. Remove(null) when no such key");
            strLoc = "Loc_07oo"; 
            iCountTestcases++;
            nvc.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(keys[i], values[i]);
            }
            cnt = nvc.Count;
            try 
            {
                nvc.Remove(null);
                if ( nvc.Count != cnt) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007a: removed something ");
                }
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007b, unexpected exception: {0}", e.ToString());
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
        Co8731Remove_str cbA = new Co8731Remove_str();
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
