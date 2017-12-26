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
public class Co8725get_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.Get(int)";
    public static String s_strTFName        = "Co8725get_int.cs";
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
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection();
            Console.WriteLine("1. Get() on empty collection");
            iCountTestcases++;
            Console.WriteLine("     - Get(-1)");
            try 
            {
                nvc.Get(-1);
                iCountErrors++;
                Console.WriteLine("Err_0001a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_001b, unexpected exception: {0}", e.ToString());
            }
            iCountTestcases++;
            Console.WriteLine("     - Get(0)");
            try 
            {
                nvc.Get(0);
                iCountErrors++;
                Console.WriteLine("Err_0001c, no exception");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_001d, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("2. add simple strings access them via Get()");
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
                if ( String.Compare(nvc.Get(i), values[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, returned: \"{1}\", expected \"{2}\"", i, nvc.Get(i), values[i]);
                }  
            }
            Console.WriteLine("3. add intl strings Get()");
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
                if ( String.Compare(nvc.Get(i), intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, returned \"{1}\" instead of \"{2}\"", i, nvc.Get(i), intlValues[i]);
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
                if ( String.Compare(nvc.Get(i), intlValues[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, returned \"{1}\" instead of \"{2}\"", i, nvc.Get(i), intlValues[i]);
                }
                iCountTestcases++;
                if ( !caseInsensitive && String.Compare(nvc.Get(i), intlValuesLower[i], false) == 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004_{0}a, returned lowercase when added uppercase", i);
                }
            }
            Console.WriteLine("5. multiple items with same key and Get()");
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
            if ( String.Compare(nvc.Get(0), exp, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, returned \"{0}\" instead of \"{1}\"", nvc.Get(0), exp);
            }  
            if ( String.Compare(nvc.Get(1), exp1, false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005c, returned \"{0}\" instead of \"{1}\"", nvc.Get(1), exp1);
            }  
            Console.WriteLine("6. Get(-1)");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            try 
            {
                nvc.Get(-1);
                iCountErrors++;
                Console.WriteLine("Err_0006b: no exception ");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("7. Get(count)");
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            if (nvc.Count < 1) 
            {
                for (int i = 0; i < len; i++) 
                {
                    nvc.Add(keys[i], values[i]);
                }
            }
            cnt = nvc.Count;
            try 
            {
                nvc.Get(cnt);
                iCountErrors++;
                Console.WriteLine("Err_007b: no exception ");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_007c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("8. Get(count+1)");
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            if (nvc.Count < 1) 
            {
                for (int i = 0; i < len; i++) 
                {
                    nvc.Add(keys[i], values[i]);
                }
            }
            cnt = nvc.Count;
            try 
            {
                nvc.Get(cnt+1);
                iCountErrors++;
                Console.WriteLine("Err_008b: no exception ");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_008c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("9. Get(null)");
            strLoc = "Loc_09oo"; 
            iCountTestcases++;
            try 
            {
                string res = nvc.Get(null);
                if ( res != null) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0009a: returned non-null ");
                }
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
        Co8725get_int cbA = new Co8725get_int();
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
