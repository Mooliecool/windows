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
public class Co8747RemoveAt_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.RemoveAt(int)";
    public static String s_strTFName        = "Co8747RemoveAt_int.cs";
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
            Console.WriteLine("1. RemoveAt() from empty collection");
            iCountTestcases++;
            if (sc.Count > 0)
                sc.Clear();
            try 
            {
                sc.RemoveAt(0);
                iCountErrors++;
                Console.WriteLine("Err_0001_{0}a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine(" expected exception: " + ex.Message);
            }    
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001_{0}b, unexpected exception: " + e.ToString());
            }    
            Console.WriteLine("2. RemoveAt() on filled collection");
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
            iCountTestcases++;
            sc.RemoveAt(0);
            if (sc.Count != values.Length - 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, Count returned {0} instead of {1}", sc.Count, values.Length - 1);
            } 
            iCountTestcases++;
            if (sc.Contains(values[0])) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002c, removed wrong item");
            } 
            for (int i = 0; i < values.Length; i++) 
            { 
                iCountTestcases++;
                if (sc.IndexOf(values[i]) != i-1) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}d, IndexOf returned {1} instead of {2}", i, sc.IndexOf(values[i]), i-1);
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
            sc.RemoveAt(values.Length-1);
            if (sc.Count != values.Length - 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002f, Count returned {0} instead of {1}", sc.Count, values.Length - 1);
            } 
            iCountTestcases++;
            if (sc.Contains(values[values.Length - 1])) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002g, removed wrong item");
            } 
            for (int i = 0; i < values.Length-1; i++) 
            { 
                iCountTestcases++;
                if (sc.IndexOf(values[i]) != i) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}h, IndexOf returned {1} instead of {2}", i, sc.IndexOf(values[i]), i);
                } 
            }
            Console.WriteLine(" - at the middle");
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002i, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            iCountTestcases++;
            sc.RemoveAt(values.Length/2);
            if (sc.Count != values.Length - 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002j, Count returned {0} instead of {1}", sc.Count, values.Length - 1);
            } 
            iCountTestcases++;
            if (sc.Contains(values[values.Length/2])) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002g, removed wrong item");
            } 
            for (int i = 0; i < values.Length; i++) 
            { 
                iCountTestcases++;
                int expected = i;
                if (i == values.Length / 2)
                    expected = -1;
                else
                    if (i > values.Length / 2)
                    expected = i-1;
                if (sc.IndexOf(values[i]) != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}k, IndexOf returned {1} instead of {2}", i, sc.IndexOf(values[i]), expected);
                } 
            }
            Console.WriteLine("3. RemoveAt() on collection with duplicate strings ");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            sc.Clear();
            string intlStr = intl.GetString(MAX_LEN, true, true, true);
            sc.Add(intlStr);        
            sc.AddRange(values);
            sc.Add(intlStr);        
            if (sc.Count != values.Length + 2) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {1} instead of {2}", sc.Count, values.Length + 2);
            } 
            iCountTestcases++;
            sc.RemoveAt(values.Length + 1);
            if (!sc.Contains(intlStr)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, removed both duplicates");
            }
            if (sc.IndexOf(intlStr) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, removed 1st instance");
            }
            Console.WriteLine("4. RemoveAt(-1)");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            try 
            {
                sc.RemoveAt(-1);
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
            Console.WriteLine("5. RemoveAt(Count)");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            try 
            {
                sc.RemoveAt(sc.Count);
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
            Console.WriteLine("6. RemoveAt(Count+1)");
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            try 
            {
                sc.RemoveAt(sc.Count+1);
                iCountErrors++;
                Console.WriteLine("Err_0006a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
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
        Co8747RemoveAt_int cbA = new Co8747RemoveAt_int();
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
