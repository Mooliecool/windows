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
public class Co8764get_Item_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringDictionary.Item(string) get";
    public static String s_strTFName        = "Co8764get_Item_str.cs";
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
        string itm;         
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sd = new StringDictionary();
            Console.WriteLine("1. get Item from empty dictionary");
            iCountTestcases++;
            if (sd.Count > 0)
                sd.Clear();
            for (int i = 0; i < keys.Length; i++) 
            {
                itm = sd[keys[i]];
                if (itm != null) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}, returned non-null for {0} key", i);
                }
            }
            Console.WriteLine("2. get Item on filled dictionary");  
            strLoc = "Loc_002oo"; 
            int len = values.Length;
            iCountTestcases++;
            sd.Clear();
            for (int i = 0; i < len; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            if (sd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sd.Count, len);
            } 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(sd[keys[i]], values[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002b_{0}, returned {1} instead of {2}", i, sd[keys[i]], values[i]);
                } 
            }
            Console.WriteLine("3. get Item on dictionary with duplicate values ");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            sd.Clear();
            string intlStr = intl.GetString(MAX_LEN, true, true, true);
            sd.Add("keykey1", intlStr);        
            for (int i = 0; i < len; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            sd.Add("keykey2", intlStr);        
            if (sd.Count != len+2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sd.Count, len+2);
            } 
            iCountTestcases++;
            if (String.Compare(sd["keykey1"], intlStr, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, returned {1} instead of {2}", sd["keykey1"],intlStr);
            } 
            iCountTestcases++;
            if (String.Compare(sd["keykey2"], intlStr, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, returned {1} instead of {2}", sd["keykey2"],intlStr);
            } 
            Console.WriteLine("4. get Item from dictionary with intl strings");
            strLoc = "Loc_004oo"; 
            string [] intlValues = new string [len*2];
            for (int i = 0; i < len*2; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            sd.Clear();
            for (int i = 0; i < len; i++) 
            {
                sd.Add(intlValues[i+len], intlValues[i]);
            }
            if (sd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", sd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(sd[intlValues[i+len]], intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002b_{0}, returned {1} instead of {2}", i, sd[intlValues[i+len]], intlValues[i]);
                } 
            }
            Console.WriteLine("5. Case sensitivity");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            sd.Clear();
            string [] intlValuesUpper = new string [len];
            for (int i = 0; i < len * 2; i++) 
            {
                intlValues[i] = intlValues[i].ToLower();
            }
            for (int i = 0; i < len; i++) 
            {
                intlValuesUpper[i] = intlValues[i+len].ToUpper();
            } 
            sd.Clear();
            Console.WriteLine(" ... add Lowercased ...");
            for (int i = 0; i < len; i++) 
            {
                sd.Add(intlValues[i+len], intlValues[i]);     
            }
            if (sd.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", sd.Count, len);
            } 
            Console.WriteLine(" ... get Uppercased ..."); 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(sd[intlValuesUpper[i]], intlValues[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005b_{0}, returned {1} instead of {2}", i, sd[intlValuesUpper[i]], intlValues[i]);
                } 
            }
            Console.WriteLine("6. Item(null)");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            try 
            {
                itm = sd[null];
                iCountErrors++;
                Console.WriteLine("Err_0006a, ArgumentNullException expected");
            }
			//                                                                                                                   
			catch (System.ArgumentNullException e)
			{
				Console.WriteLine("expected exception: {0}", e.ToString());
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
        Co8764get_Item_str cbA = new Co8764get_Item_str();
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
