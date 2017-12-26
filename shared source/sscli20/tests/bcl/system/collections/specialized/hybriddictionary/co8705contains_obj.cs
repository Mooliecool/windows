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
internal class Co8686_SpecialStruct 
{
    public Int32 Num;
    public String Wrd;
}
public class Co8705Contains_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "HybridDictionary.Contains(obj)";
    public static String s_strTFName        = "Co8705Contains_obj.cs";
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
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            hd = new HybridDictionary();
            Console.WriteLine("1. Contains() on empty dictionary");
            iCountTestcases++;
            Console.WriteLine("     - Contains(null)");
            try 
            {
                hd.Contains(null);
				iCountErrors++;
				Console.WriteLine("Err_0001a, ArgumentNullException expected");
            }
			//                                                                                                                                     
			catch (System.ArgumentNullException e)
			{
				Console.WriteLine("expected exception: {0}", e.ToString());
			} 
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("     - Contains(some_object)");
            if ( hd.Contains("some_string") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001c, empty dictionary contains some_object");
            }
            Console.WriteLine("2. add few simple strings and check Contains()");
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
                iCountTestcases++;
                if ( ! hd.Contains(keysShort[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, doesn't contain \"{1}\"", i, keysShort[i]);
                }  
            }
            Console.WriteLine(" .. increase number of elements");
            cnt = hd.Count;
            len = valuesLong.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            if (hd.Count != len+cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002c, count is {0} instead of {1}", hd.Count, len+cnt);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( ! hd.Contains(keysLong[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}d, doesn't contain \"{1}\"", i, keysLong[i]);
                }  
            } 
            for (int i = 0; i < valuesShort.Length; i++) 
            {
                iCountTestcases++;
                if ( ! hd.Contains(keysShort[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}e, doesn't contain \"{1}\"", i, keysShort[i]);
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
            iCountTestcases++;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( hd.Count != (len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", hd.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( ! hd.Contains(intlValues[i+len]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, doesn't contain \"{1}\"", i, intlValues[i+len]);
                }  
            }
            Console.WriteLine("4. case sensitivity");
            strLoc = "Loc_004oo"; 
            hd.Clear();
            len = valuesLong.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);     
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( ! hd.Contains(keysLong[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, doesn't contain added uppercase \"{1}\"", i, keysLong[i]);
                }
                iCountTestcases++;
                if ( hd.Contains(keysLong[i].ToUpper()) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, contains uppercase \"{1}\" - should not", i, keysLong[i].ToUpper());
                }
            }
            Console.WriteLine("5. similar_but_different_in_casing keys and Contains()");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            hd.Clear();
            string [] ks = {"Key", "kEy", "keY"};
            len = ks.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(ks[i], "Value"+i);
            }
            if (hd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", hd.Count, len);
            } 
            iCountTestcases++;
            if ( hd.Contains("Value0") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, returned true when should not");
            }  
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( !hd.Contains(ks[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005c_{0}, returned false when true expected", i);
                }  
            }
            Console.WriteLine(" .. increase number of elements");
            cnt = hd.Count;
            len = valuesLong.Length;
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            if (hd.Count != len+cnt) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005d, count is {0} instead of {1}", hd.Count, len+cnt);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( ! hd.Contains(keysLong[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}e, doesn't contain \"{1}\"", i, keysLong[i]);
                }  
            } 
            for (int i = 0; i < ks.Length; i++) 
            {
                iCountTestcases++;
                if ( ! hd.Contains(ks[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}f, doesn't contain \"{1}\"", i, ks[i]);
                }  
            }
            Console.WriteLine("6. Contains(null) for filled dictionary");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            len = valuesShort.Length;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysShort[i], valuesShort[i]);
            }
            try 
            {
                hd.Contains(null);
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
            Console.WriteLine("7. Contains() for case-insensitive comparer dictionary");
            hd = new HybridDictionary(true);
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            hd.Clear();
            len = ks.Length;
            hd.Add(ks[0], "Value0");
            for (int i = 1; i < len; i++) 
            {
                try 
                {
                    hd.Add(ks[i], "Value"+i);
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
            if (hd.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007c, count is {0} instead of {1}", hd.Count, 1);
            } 
            iCountTestcases++;
            if ( hd.Contains("Value0") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007d, returned true when should not");
            }  
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( !hd.Contains(ks[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007e_{0}, returned false when true expected", i);
                }  
            }
            iCountTestcases++;
            if ( !hd.Contains("KEY") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007f, returned false non-existing-cased key");
            } 
            Console.WriteLine("9. Contains() and few SpecialStructs_not_overriding_Equals");
            hd = new HybridDictionary();
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            Co8686_SpecialStruct s = new Co8686_SpecialStruct();
            s.Num = 1;
            s.Wrd = "one";
            Co8686_SpecialStruct s1 = new Co8686_SpecialStruct();
            s.Num = 1;
            s.Wrd = "one";
            hd.Add(s, "first");
            hd.Add(s1, "second");
            if (hd.Count != 2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009a, count is {0} instead of {1}", hd.Count, 2);
            } 
            iCountTestcases++;
            if ( !hd.Contains(s) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009b, returned false when true expected");
            }  
            iCountTestcases++;
            if ( !hd.Contains(s1) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009c, returned false when true expected");
            }
            Console.WriteLine("10. Contains() and many SpecialStructs_not_overriding_Equals");
            int num = 40;
            hd = new HybridDictionary();
            strLoc = "Loc_010oo"; 
            iCountTestcases++;
            Co8686_SpecialStruct [] ss = new Co8686_SpecialStruct[num];
            for (int i = 0; i < num; i++) 
            {
                ss[i] = new Co8686_SpecialStruct();
                ss[i].Num = i;
                ss[i].Wrd = "value"+i;
                hd.Add(ss[i], "item"+i);
            }
            if (hd.Count != num) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010a, count is {0} instead of {1}", hd.Count, num);
            } 
            for (int i = 0; i < num; i++) 
            {
                iCountTestcases++;
                if ( !hd.Contains(ss[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0010b_{0}, returned false when true expected", i);
                }  
            }
            iCountTestcases++;
            s = new Co8686_SpecialStruct();
            s.Num = 1;
            s.Wrd = "value1";
            if ( hd.Contains(s) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010c, returned true when false expected");
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
        Co8705Contains_obj cbA = new Co8705Contains_obj();
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
