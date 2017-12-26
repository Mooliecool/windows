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
internal class Co8686_InsensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_insensitiveComparer! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, true);
    }
}
internal class Co8686_SpecialStruct 
{
    public Int32 Num;
    public String Wrd;
}
public class Co8686Contains_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ListDictionary.Contains(obj)";
    public static String s_strTFName        = "Co8686Contains_obj.cs";
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
            Console.WriteLine("1. Contains() on empty dictionary");
            iCountTestcases++;
            Console.WriteLine("     - Contains(null)");
            try 
            {
                ld.Contains(null);
                iCountErrors++;
                Console.WriteLine("Err_0001a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("     - Contains(some_object)");
            if ( ld.Contains("some_string") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001c, empty dictionary contains some_object");
            }
            Console.WriteLine("2. add simple strings and check Contains()");
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
                if ( ! ld.Contains(keys[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, doesn't contain \"{1}\"", i, keys[i]);
                }  
            }
            Console.WriteLine("3. add intl strings check Contains()");
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
            ld.Clear();
            for (int i = 0; i < len; i++) 
            {
                ld.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( ld.Count != (len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", ld.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( ! ld.Contains(intlValues[i+len]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, doesn't contain \"{1}\"", i, intlValues[i+len]);
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
            for (int i = 0; i < len; i++) 
            {
                ld.Add(intlValues[i+len], intlValues[i]);     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( ! ld.Contains(intlValues[i+len]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, doesn't contain added uppercase \"{1}\"", i, intlValues[i+len]);
                }
                iCountTestcases++;
                if ( !caseInsensitive && ld.Contains(intlValuesLower[i+len]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, contains lowercase \"{1}\" - should not", i, intlValuesLower[i+len]);
                }
            }
            Console.WriteLine("5. similar_but_different_in_casing keys and Contains()");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            ld.Clear();
            string [] ks = {"Key", "kEy", "keY"};
            len = ks.Length;
            for (int i = 0; i < len; i++) 
            {
                ld.Add(ks[i], "Value"+i);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", ld.Count, len);
            } 
            iCountTestcases++;
            if ( ld.Contains("Value0") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, returned true when should not");
            }  
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( !ld.Contains(ks[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005c_{0}, returned false when true expected", i);
                }  
            }
            Console.WriteLine("6. Contains(null) for filled dictionary");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            ld.Clear();
            for (int i = 0; i < len; i++) 
            {
                ld.Add(keys[i], values[i]);
            }
            try 
            {
                ld.Contains(null);
                iCountErrors++;
                Console.WriteLine("Err_0006a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("7. Contains() for case-insensitive comparer");
            ld = new ListDictionary(new Co8686_InsensitiveComparer());
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            ld.Clear();
            len = ks.Length;
            ld.Add(ks[0], "Value0");
            for (int i = 1; i < len; i++) 
            {
                try 
                {
                    ld.Add(ks[i], "Value"+i);
                    iCountErrors++;
                    Console.WriteLine("Err_0007a_{0}, no exception", i);
                }
                catch (ArgumentException e) 
                {
                    Console.WriteLine("_{0}, Expected exception: {1}", i, e.Message);
                }
                catch (Exception ex) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007b_{0}, unexpected exception: {1}", i, ex.ToString());
                }
            }
            if (ld.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007c, count is {0} instead of {1}", ld.Count, 1);
            } 
            iCountTestcases++;
            if ( ld.Contains("Value0") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007d, returned true when should not");
            }  
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( !ld.Contains(ks[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007e_{0}, returned false when true expected", i);
                }  
            }
            iCountTestcases++;
            if ( !ld.Contains("KEY") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007f, returned false non-existing-cased key");
            } 
            Console.WriteLine("10. Contains() and SpecialStructs_not_overriding_Equals");
            ld = new ListDictionary();
            strLoc = "Loc_010oo"; 
            iCountTestcases++;
            Co8686_SpecialStruct s = new Co8686_SpecialStruct();
            s.Num = 1;
            s.Wrd = "one";
            Co8686_SpecialStruct s1 = new Co8686_SpecialStruct();
            s.Num = 1;
            s.Wrd = "one";
            ld.Add(s, "first");
            ld.Add(s1, "second");
            if (ld.Count != 2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010a, count is {0} instead of {1}", ld.Count, 2);
            } 
            iCountTestcases++;
            if ( !ld.Contains(s) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010b, returned false when true expected");
            }  
            iCountTestcases++;
            if ( !ld.Contains(s1) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010c, returned false when true expected");
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
        Co8686Contains_obj cbA = new Co8686Contains_obj();
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
