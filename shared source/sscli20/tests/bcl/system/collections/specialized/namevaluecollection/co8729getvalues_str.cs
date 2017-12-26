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
public class Co8729GetValues_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.GetValues(string)";
    public static String s_strTFName        = "Co8729GetValues_str.cs";
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
        string [] vls;          
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
            Console.WriteLine("1. GetValues() on empty collection");
            iCountTestcases++;
            Console.WriteLine("     - GetValues(null)");
            if ( nvc.GetValues(null) != null ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, returned non-null");
            }
            Console.WriteLine("     - GetValues(some_string)");
            if ( nvc.GetValues("some_string") != null ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, returned non-null");
            }
            Console.WriteLine("2. add simple strings access them via GetValues(string)");
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
                iCountTestcases++;
                vls = nvc.GetValues(keys[i]);
                if (vls.Length != 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned number of strings {1} instead of 1", i, vls.Length);
                } 
                iCountTestcases++;
                if ( String.Compare(vls[0], values[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, returned: \"{1}\", expected \"{2}\"", i, vls[0], values[i]);
                }  
            }
            Console.WriteLine("3. add intl strings GetValues()");
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
                vls = nvc.GetValues(intlValues[i+len]);
                if (vls.Length != 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, returned number of strings {1} instead of 1", i, vls.Length);
                } 
                iCountTestcases++;
                if ( String.Compare(vls[0], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}c, returned \"{1}\" instead of \"{2}\"", i, vls[0], intlValues[i]);
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
                vls = nvc.GetValues(intlValues[i+len]);
                if (vls.Length != 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, returned number of strings {1} instead of 1", i, vls.Length);
                } 
                iCountTestcases++;
                if ( String.Compare(vls[0], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}b, returned \"{1}\" instead of \"{2}\"", i, vls[0], intlValues[i]);
                }
                iCountTestcases++;
                vls = nvc.GetValues(intlValuesLower[i+len]);
                if (vls.Length != 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}c, returned number of strings {1} instead of 1", i, vls.Length);
                } 
                iCountTestcases++;
                if ( String.Compare(vls[0], intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}d, returned \"{1}\" instead of \"{2}\"", i, vls[0], intlValues[i]);
                }
                iCountTestcases++;
                if ( !caseInsensitive && String.Compare(vls[0], intlValuesLower[i], false) == 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, returned lowercase when added uppercase", i);
                }
            }
            Console.WriteLine("5. multiple items with same key and GetValues()");
            strLoc = "Loc_005oo"; 
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
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", nvc.Count, 2);
            } 
            iCountTestcases++;
            vls = nvc.GetValues(k);
            if (vls.Length != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, returned number of strings {0} instead of {1}", vls.Length, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(vls[i], "Value"+i, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}b, returned \"{1}\" instead of \"{2}\"", i, vls[i], "Value"+i);
                } 
            } 
            iCountTestcases++;
            vls = nvc.GetValues(k1);
            if (vls.Length != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005c, returned number of strings {0} instead of {1}", vls.Length, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(vls[i], "iTem"+i, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}d, returned \"{1}\" instead of \"{2}\"", i, vls[i], "iTem"+i);
                } 
            } 
            Console.WriteLine("6. GetValues(null) when there is item with null key");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            nvc.Add(null, "nullValue");
            iCountTestcases++;
            vls = nvc.GetValues(null);
            if (vls.Length != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a, returned number of strings {0} instead of {1}", vls.Length, 1);
            } 
            if ( String.Compare(vls[0], "nullValue", false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006: returned \"{}\" instead of \"{}\"", vls[0], "nullValue");
            }
            Console.WriteLine("7. GetValues(null) when no such key");
            strLoc = "Loc_07oo"; 
            iCountTestcases++;
            nvc.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(keys[i], values[i]);
            }
            try 
            {
                vls = nvc.GetValues(null);
                if ( vls != null) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007a: returned non-null ");
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
        Co8729GetValues_str cbA = new Co8729GetValues_str();
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
