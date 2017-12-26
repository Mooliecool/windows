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
public class Co8736set_Item_str_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.Item(string)";
    public static String s_strTFName        = "Co8736set_Item_str_str.cs";
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
        string itm;         
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection();
            Console.WriteLine("1. set Item(string) on empty collection");
            iCountTestcases++;
            nvc.Clear();
            Console.WriteLine("     - Item(null)");
            nvc[null] = "nullItem";
            iCountTestcases++;
            if ( nvc.Count != 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, failed to add item");
            }     		
            iCountTestcases++;
            if ( nvc[null] == null ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, returned null");
            }
            else 
            {
                iCountTestcases++;
                if ( String.Compare(nvc[null], "nullItem", false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001c, wrong value");
                }
            }
            nvc.Clear();
            Console.WriteLine("     - Item(some_string)");
            nvc["some_string"] = "someItem";
            iCountTestcases++;
            if ( nvc.Count != 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001d, failed to add item");
            }     		
            iCountTestcases++;
            if ( nvc["some_string"] == null ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001e, returned null");
            }
            else 
            {
                iCountTestcases++;
                if ( String.Compare(nvc["some_string"], "someItem", false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001f, wrong value");
                }
            }
            Console.WriteLine("2. add simple strings, set them via Item(string)");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            nvc.Clear();
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
                iCountTestcases++;
                nvc[keys[i]] = "Item" + i;
                if ( String.Compare(nvc[keys[i]], "Item"+i, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned: \"{1}\", expected \"{2}\"", i, nvc[keys[i]], "Item"+i);
                }  
            }
            Console.WriteLine("3. add intl strings and access via Item(string)");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            string [] intlValues = new string [len * 3];
            for (int i = 0; i < len*3; i++) 
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
            nvc.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( nvc.Count != (len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", nvc.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                nvc[intlValues[i+len]] = intlValues[i + len*2];
                if ( String.Compare(nvc[intlValues[i+len]], intlValues[i + len*2], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, returned \"{1}\" instead of \"{2}\"", i, nvc[intlValues[i+len]], intlValues[i + len*2]);
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
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc[intlValues[i+len]], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, returned \"{1}\" instead of \"{2}\"", i, nvc[intlValues[i+len]], intlValues[i]);
                }
                iCountTestcases++;
                if ( String.Compare(nvc[intlValuesLower[i+len]], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}b, returned \"{1}\" instead of \"{2}\"", i, nvc[intlValuesLower[i+len]], intlValues[i]);
                }
                iCountTestcases++;
                if ( !caseInsensitive && String.Compare(nvc[intlValues[i+len]], intlValuesLower[i], false) == 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}c, returned lowercase when added uppercase", i);
                }
                nvc[intlValues[i+len]] = intlValuesLower[i]; 
                iCountTestcases++;
                if ( !caseInsensitive && String.Compare(nvc[intlValues[i+len]], intlValues[i], false) == 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}d, failed to set to uppercase value", i);
                }
                iCountTestcases++;
                if ( !caseInsensitive && String.Compare(nvc[intlValuesLower[i+len]], intlValues[i], false) == 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}e, failed to set to lowercase value", i);
                }
                iCountTestcases++;
                if ( String.Compare(nvc[intlValues[i+len]], intlValuesLower[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}f, returned uppercase when set to lowercase", i);
                }
            }
            Console.WriteLine("5. multiple items with same key and set Item(string)");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            nvc.Clear();
            len = values.Length;
            string k = "keykey";
            string k1 = "hm1";
            string exp = "";
            string exp1 = "";
            string newVal = "nEw1,nEw2";
            string newVal1 = "Hello,hello,hELLo";
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(k, "Value"+i);
                nvc.Add(k1, "iTem"+i);
                if (i < len-1) 
                {
                    exp += "Value"+i+",";
                    exp1 += "iTem"+i+",";
                }
                else  
                {
                    exp += "Value"+i;
                    exp1 += "iTem"+i;
                }
            }
            if (nvc.Count != 2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", nvc.Count, 2);
            } 
            Console.WriteLine(" - item with 0-th key");
            iCountTestcases++;
            if ( String.Compare(nvc[k], exp, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, returned \"{0}\" instead of \"{1}\"", nvc[k], exp);
            } 
            iCountTestcases++;
            nvc[k] = newVal;
            if ( String.Compare(nvc[k], newVal, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005c, returned \"{0}\" instead of \"{1}\"", nvc[k], newVal);
            }  
            if ( nvc.GetValues(k).Length != 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005d, number of values is {0} instead of 1", nvc.GetValues(k).Length);
            }  
            Console.WriteLine(" - item with 1-st key");
            if ( String.Compare(nvc[k1], exp1, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005e, returned \"{0}\" instead of \"{1}\"", nvc[k1], exp1);
            }  
            iCountTestcases++;
            nvc[k1] = newVal1;
            if ( String.Compare(nvc[k1], newVal1, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005f, returned \"{0}\" instead of \"{1}\"", nvc[k1], newVal1);
            }  
            if ( nvc.GetValues(k1).Length != 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005g, number of values is {0} instead of 1", nvc.GetValues(k).Length);
            }  
            Console.WriteLine("6. set Item(null) when there is item with null key");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            nvc.Add(null, "nullValue");
            if ( String.Compare(nvc[null], "nullValue", false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a: returned \"{}\" instead of \"{}\"", nvc[null], "nullValue");
            }
            nvc[null] = "newnewValue";
            if ( String.Compare(nvc[null], "newnewValue", false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b: returned \"{}\" instead of \"{}\"", nvc[null], "newnewValue");
            }
            Console.WriteLine("7. set Item(null) when no such key");
            strLoc = "Loc_07oo"; 
            iCountTestcases++;
            nvc.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(keys[i], values[i]);
            }
            nvc[null] = "newNullValue";
            try 
            {
                itm = nvc[null];
                if ( String.Compare(itm, "newNullValue",false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007a: returned unexpected value ");
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
        Co8736set_Item_str_str cbA = new Co8736set_Item_str_str();
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
