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
public class Co8755Add_str_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringDictionary.Add(string, string)";
    public static String s_strTFName        = "Co8755Add_str_str.cs";
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
        StringDictionary sd; 
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
        string [] keys = 
        {
            "zero",
            "one",
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
        string ind;            
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sd = new StringDictionary();
            Console.WriteLine("1. add simple strings");
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                cnt = sd.Count;
                sd.Add(keys[i], values[i]);
                if (sd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}a, count is {1} instead of {2}", i, sd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (!sd.ContainsValue(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}b, collection doesn't contain value of new item", i);
                } 
                iCountTestcases++;
                if (!sd.ContainsKey(keys[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}c, collection doesn't contain key of new item", i);
                } 
                iCountTestcases++;
                if (String.Compare(sd[keys[i]], values[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}d, returned item \"{1}\" instead of \"{2}\"", i, sd[keys[i]], values[i]);
                } 
            }
            Console.WriteLine("2. add intl strings");
            int len = values.Length;
            string [] intlValues = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
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
            Console.WriteLine(" initial number of items: " + sd.Count);
            strLoc = "Loc_002oo"; 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                cnt = sd.Count;
                sd.Add(intlValues[i+len], intlValues[i]);
                if (sd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}a, count is {1} instead of {2}", i, sd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (!sd.ContainsValue(intlValues[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, collection doesn't contain value of new item", i);
                } 
                iCountTestcases++;
                if (!sd.ContainsKey(intlValues[i+len])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, collection doesn't contain key of new item", i);
                } 
                ind = intlValues[i+len];
                iCountTestcases++;
                if (String.Compare(sd[ind], intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}d, returned item \"{1}\" instead of \"{2}\"", i, sd[ind], intlValues[i]);
                } 
            }
            Console.WriteLine("3. Case sensitivity");
            string [] intlValuesLower = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
            {
                intlValues[i] = intlValues[i].ToUpper();
            }
            for (int i = 0; i < len * 2; i++) 
            {
                intlValuesLower[i] = intlValues[i].ToLower();
            } 
            sd.Clear();
            Console.WriteLine(" initial number of items: " + sd.Count);
            strLoc = "Loc_003oo"; 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                cnt = sd.Count;
                sd.Add(intlValues[i+len], intlValues[i]);
                if (sd.Count != cnt+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}a, count is {1} instead of {2}", i, sd.Count, cnt+1);
                } 
                iCountTestcases++;
                if (!sd.ContainsValue(intlValues[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, collection doesn't contain value of new item", i);
                } 
                iCountTestcases++;
                if (!sd.ContainsKey(intlValues[i+len])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}c, collection doesn't contain key of new item", i);
                } 
                iCountTestcases++;
                if (!caseInsensitive && sd.ContainsValue(intlValuesLower[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}d, collection contains lowercase value of new item", i);
                } 
                iCountTestcases++;
                if ( !sd.ContainsKey(intlValuesLower[i+len])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}e, collection doesn't contain lowercase key of new item", i);
                } 
            }
            Console.WriteLine("4. Add (string, null) ");
            Console.WriteLine(" initial number of items: " + sd.Count);
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            cnt = sd.Count;
            sd.Add("keykey", null);
            iCountTestcases++;
            if (sd.Count != cnt+1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", sd.Count, cnt+1);
            } 
            iCountTestcases++;
            if (!sd.ContainsValue(null)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, collection doesn't contain null");
            }
            iCountTestcases++;
            if (sd["keykey"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004c, returned non-null on place of null");
            } 
            Console.WriteLine("5. Add (null, string) ");
            Console.WriteLine(" initial number of items: " + sd.Count);
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            try 
            {
                sd.Add(null, "item");
                iCountErrors++;
                Console.WriteLine("Err_0005a, ArgumentNullException Expected");
            }
			//                                                                                                                                     
			catch (System.ArgumentNullException e)
			{
				Console.WriteLine("expected exception: {0}", e.ToString());
			} 
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, unexpected exception: " + e.ToString());
            }
            Console.WriteLine("6. Add (key, value) with duplicate key ");
            Console.WriteLine(" initial number of items: " + sd.Count);
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            string k = intl.GetString(MAX_LEN, true, true, true);
            if (! sd.ContainsKey(k)) 
            {
                sd.Add(k, "newItem");
            }
            if (! sd.ContainsKey(k)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a,failed to add item");
            }
            else 
            {
                try 
                {
                    sd.Add(k, "itemitemitem");
                    iCountErrors++;
                    Console.WriteLine("Err_0005b, no exception");
                }
                catch (ArgumentException ex) 
                {
                    Console.WriteLine("  expected exception: " + ex.Message);
                }
                catch (Exception e) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005c, unexpected exception: " + e.ToString());
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
        Co8755Add_str_str cbA = new Co8755Add_str_str();
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
