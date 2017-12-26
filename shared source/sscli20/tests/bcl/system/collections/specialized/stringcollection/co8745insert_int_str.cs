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
public class Co8745Insert_int_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.Insert(int, string)";
    public static String s_strTFName        = "Co8745Insert_int_str.cs";
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
        StringCollection sc; 
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
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            Console.WriteLine("1. Insert into empty collection");
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if (sc.Count > 0)
                    sc.Clear();
                sc.Insert(0, values[i]);
                if (sc.Count != 1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}a, Count {1} instead of 1", i, sc.Count);
                }
                if (! sc.Contains(values[i])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}b, doesn't contain just inserted item", i);
                }
            } 
            Console.WriteLine("2. Insert into filled collection");
            strLoc = "Loc_002oo"; 
            Console.WriteLine(" - at the beginning");
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            string val = intl.GetString(MAX_LEN, true, true, true);
            iCountTestcases++;
            sc.Insert(0, val);
            if (sc.Count != values.Length + 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, Count returned {0} instead of {1}", sc.Count, values.Length + 1);
            } 
            iCountTestcases++;
            if (sc.IndexOf(val) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002c, IndexOf returned {0} instead of {1}", sc.IndexOf(val), 0);
            } 
            for (int i = 0; i < values.Length; i++) 
            { 
                iCountTestcases++;
                if (sc.IndexOf(values[i]) != i+1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}d, IndexOf returned {1} instead of {2}", i, sc.IndexOf(values[i]), i+1);
                } 
            }
            Console.WriteLine(" - at the end");
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002e, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            iCountTestcases++;
            sc.Insert(values.Length, val);
            if (sc.Count != values.Length + 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002f, Count returned {0} instead of {1}", sc.Count, values.Length + 1);
            } 
            iCountTestcases++;
            if (sc.IndexOf(val) != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002g, IndexOf returned {0} instead of {1}", sc.IndexOf(val), values.Length);
            } 
            for (int i = 0; i < values.Length; i++) 
            { 
                iCountTestcases++;
                if (sc.IndexOf(values[i]) != i) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}h, IndexOf returned {1} instead of {2}", i, sc.IndexOf(values[i]), i);
                } 
            }
            Console.WriteLine(" - into the middle");
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002i, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            iCountTestcases++;
            sc.Insert(values.Length/2, val);
            if (sc.Count != values.Length + 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002j, Count returned {0} instead of {1}", sc.Count, values.Length + 1);
            } 
            iCountTestcases++;
            if (sc.IndexOf(val) != values.Length/2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002g, IndexOf returned {0} instead of {1}", sc.IndexOf(val), values.Length/2);
            } 
            for (int i = 0; i < values.Length; i++) 
            { 
                iCountTestcases++;
                int expected = i;
                if (i >= values.Length / 2)
                    expected = i+1;
                if (sc.IndexOf(values[i]) != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}k, IndexOf returned {1} instead of {2}", i, sc.IndexOf(values[i]), expected);
                } 
            }
            Console.WriteLine("3. Insert(-1, string)");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            try 
            {
                sc.Insert(-1, val);
                iCountErrors++;
                Console.WriteLine("Err_0003a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("4. Insert(Count + 1, string)");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            try 
            {
                sc.Insert(sc.Count + 1, val);
                iCountErrors++;
                Console.WriteLine("Err_0004a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("5. Insert(Count + 2, string)");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            try 
            {
                sc.Insert(sc.Count + 2, val);
                iCountErrors++;
                Console.WriteLine("Err_0005a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, unexpected exception: {0}", e.ToString());
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
        Co8745Insert_int_str cbA = new Co8745Insert_int_str();
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
