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
public class Co8776Add_NVC
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.Add(NVC)";
    public static String s_strTFName        = "Co8776Add_NVC.cs";
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
        NameValueCollection nvc1; 
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
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection();
            nvc1 = new NameValueCollection();
            Console.WriteLine("1. Add(empty_coll) to empty collection");
            iCountTestcases++;
            nvc.Clear();
            nvc1.Clear();
            nvc.Add(nvc1);
            iCountTestcases++;
            if ( nvc.Count != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, count is {0} instead of 0", nvc.Count);
            }     		
            Console.WriteLine("2. Add(simple_strings_coll) to empty collection");
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
            iCountTestcases++;
            nvc1.Clear();
            nvc1.Add(nvc);
            if (nvc1.Count != nvc.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, count is {0} instead of {1}", nvc1.Count, nvc.Count);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc1[keys[i]], values[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, returned: \"{1}\", expected \"{2}\"", i, nvc[keys[i]], values[i]);
                }  
            }
            Console.WriteLine("3. Add(simple_strings_coll) to simple_string_collection");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            len = values.Length;
            if (nvc.Count < len) 
            {
                nvc.Clear();
                for (int i = 0; i < len; i++) 
                {
                    nvc.Add(keys[i], values[i]);
                }
            }
            nvc1.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc1.Add("k"+i, "v" + i);
            }
            cnt = nvc1.Count;
            iCountTestcases++;
            nvc1.Add(nvc);
            if (nvc1.Count != cnt+nvc.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", nvc1.Count, cnt+nvc.Count);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc1[keys[i]], values[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, returned: \"{1}\", expected \"{2}\"", i, nvc[keys[i]], values[i]);
                }  
                if ( String.Compare(nvc1["k"+i], "v"+i, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}c, returned: \"{1}\", expected \"{2}\"", i, nvc["k"+i], "v"+i);
                }  
            }
            Console.WriteLine("4. Add(intl_strings_coll) to empty collection");
            strLoc = "Loc_004oo"; 
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
            nvc.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( nvc.Count != (len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", nvc.Count, len);
            } 
            iCountTestcases++;
            nvc1.Clear();
            nvc1.Add(nvc);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc1[intlValues[i+len]], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}b, returned \"{1}\" instead of \"{2}\"", i, nvc1[intlValues[i+len]], intlValues[i]);
                }  
            }
            Console.WriteLine("5. case sensitivity");
            strLoc = "Loc_005oo"; 
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
            nvc1.Clear();
            nvc1.Add(nvc);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc1[intlValues[i+len]], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}a, returned \"{1}\" instead of \"{2}\"", i, nvc1[intlValues[i+len]], intlValues[i]);
                }
                iCountTestcases++;
                if ( String.Compare(nvc1[intlValuesLower[i+len]], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}b, returned \"{1}\" instead of \"{2}\"", i, nvc1[intlValuesLower[i+len]], intlValues[i]);
                }
                iCountTestcases++;
                if ( !caseInsensitive && (String.Compare(nvc1[intlValues[i+len]], intlValuesLower[i], false) == 0) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}c, returned lowercase when added uppercase", i);
                }
            }
            Console.WriteLine("6. Add(NVC) with already existing keys");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            nvc.Clear();
            cnt = nvc.Count;
            len = values.Length;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(keys[i], values[i]);
            }
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a, count is {0} instead of {1}", nvc.Count, values.Length);
            } 
            iCountTestcases++;
            nvc1.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc1.Add(keys[i], values[i]);
            }
            if (nvc1.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b, count is {0} instead of {1}", nvc1.Count, values.Length);
            } 
            cnt = nvc1.Count;
            iCountTestcases++;
            nvc1.Add(nvc);
            if (nvc1.Count != cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006c, count is {0} instead of {1}", nvc1.Count, cnt);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc1[keys[i]], values[i] + "," + values[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}d, returned: \"{1}\", expected \"{2}\"", i, nvc[keys[i]], values[i]+","+values[i]);
                }  
            }
            Console.WriteLine("7. multiple items with same key");
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            nvc.Clear();
            len = values.Length;
            string k = "keykey";
            string k1 = "hm1";
            string exp = "";
            string exp1 = "";
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
                Console.WriteLine("Err_0007a, count is {0} instead of {1}", nvc.Count, 2);
            }
            nvc1.Clear();
            nvc1.Add(nvc); 
            Console.WriteLine(" - item with 0-th key");
            iCountTestcases++;
            if ( String.Compare(nvc1[k], exp, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007b, returned \"{0}\" instead of \"{1}\"", nvc1[k], exp);
            } 
            if ( nvc1.GetValues(k).Length != len ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007c, number of values is {0} instead of {1}", nvc1.GetValues(k).Length, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                if ( String.Compare(nvc1.GetValues(k)[i], "Value"+i, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007_{0}_d, returned {1} instead of {2}", i, nvc1.GetValues(k)[i], "Value"+i);
                }  
            } 
            Console.WriteLine(" - item with 1-st key");
            if ( String.Compare(nvc1[k1], exp1, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007e, returned \"{0}\" instead of \"{1}\"", nvc1[k1], exp1);
            }  
            iCountTestcases++;
            if ( nvc1.GetValues(k1).Length != len ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007f, number of values is {0} instead of {1}", nvc1.GetValues(k1).Length, len);
            }  
            for (int i = 0; i < len; i++) 
            {
                if ( String.Compare(nvc1.GetValues(k1)[i], "iTem"+i, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007_{0}_g, returned {1} instead of {2}", i, nvc1.GetValues(k1)[i], "iTem"+i);
                }  
            } 
            Console.WriteLine("8. Add(with_null_key) when there is item with null key");
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            nvc.Add(null, "nullValue");   
            nvc1[null] = "nullValue1";
            nvc1.Add(nvc);
            if ( String.Compare(nvc1[null], "nullValue1," + nvc[null], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008a: returned \"{0}\" instead of \"{1}\"", nvc1[null], "nullValue1," + nvc[null]);
            }
            Console.WriteLine("9. Add(with_null_key) when no such key");
            strLoc = "Loc_07oo"; 
            iCountTestcases++;
            nvc.Clear();
            nvc1.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc1.Add(keys[i], values[i]);
            }
            nvc[null] = "newNullValue";
            nvc1.Add(nvc);
            if ( String.Compare(nvc1[null], "newNullValue",false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009: returned unexpected value ");
            }
            Console.WriteLine("10. Add(null_collection)");
            strLoc = "Loc_10oo"; 
            iCountTestcases++;
            try 
            {
                nvc1.Add(null);
                iCountErrors++;
                Console.WriteLine("Err_0010a: no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine("Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010b: unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("11. Add(empty_coll) to filled collection");
            strLoc = "Loc_011oo"; 
            iCountTestcases++;
            nvc.Clear();
            if (nvc1.Count < len) 
            {
                nvc1.Clear();
                for (int i = 0; i < len; i++) 
                {
                    nvc1.Add(keys[i], values[i]);
                }
                if (nvc1.Count != len) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0011a, count is {0} instead of {1}", nvc1.Count, values.Length);
                } 
            }
            cnt = nvc1.Count;
            iCountTestcases++;
            nvc1.Add(nvc);
            if (nvc1.Count != cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0011b, count has changed: {0} instead of {1}", nvc1.Count, cnt);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc1[keys[i]], values[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0011_{0}c, returned: \"{1}\", expected \"{2}\"", i, nvc[keys[i]], values[i]);
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
        Co8776Add_NVC cbA = new Co8776Add_NVC();
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
