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
public class Co8761Remove_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringDictionary.Remove(string)";
    public static String s_strTFName        = "Co8761Remove_str.cs";
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
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sd = new StringDictionary();
            Console.WriteLine("1. Remove() from empty dictionary");
            iCountTestcases++;
            if (sd.Count > 0)
                sd.Clear();
            for (int i = 0; i < keys.Length; i++) 
            {
                sd.Remove(keys[0]);
            }
            Console.WriteLine("2. Remove() on filled dictionary");  
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
                cnt = sd.Count;
                sd.Remove(keys[i]);
                if (sd.Count != cnt - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002b_{0}, didn't remove element with {0} key", i);
                } 
                iCountTestcases++;
                if ( sd.ContainsValue(values[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002c_{0}, removed wrong value", i);
                } 
                iCountTestcases++;
                if ( sd.ContainsKey(keys[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002d_{0}, removed wrong value", i);
                } 
            }
            Console.WriteLine("3. Remove() on dictionary with duplicate values ");
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
            sd.Remove("keykey2");
            if (!sd.ContainsValue(intlStr)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, removed both duplicates");
            }
            if ( sd.ContainsKey("keykey2") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, removed not given instance");
            }
            if (! sd.ContainsKey("keykey1") ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003d, removed wrong instance");
            }
            Console.WriteLine("4. Remove() from dictionary with intl strings");
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
                cnt = sd.Count;
                sd.Remove(intlValues[i+len]);
                if (sd.Count != cnt - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004b_{0}, didn't remove element with {0} key", i+len);
                } 
                iCountTestcases++;
                if ( sd.ContainsValue(intlValues[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004c_{0}, removed wrong value", i);
                } 
                iCountTestcases++;
                if ( sd.ContainsKey(intlValues[i+len]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0004d_{0}, removed wrong key", i);
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
            Console.WriteLine(" ... remove Uppercased ..."); 
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                cnt = sd.Count;
                sd.Remove(intlValuesUpper[i]);
                if (sd.Count != cnt - 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005b_{0}, didn't remove element with {0} lower key", i+len);
                } 
                iCountTestcases++;
                if ( sd.ContainsValue(intlValues[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005c_{0}, removed wrong value", i);
                } 
                iCountTestcases++;
                if ( sd.ContainsKey(intlValuesUpper[i]) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005d_{0}, removed wrong key", i);
                } 
            }
            Console.WriteLine("6. Remove(null)");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            try 
            {
                sd.Remove(null);
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
        Co8761Remove_str cbA = new Co8761Remove_str();
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
