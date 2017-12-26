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
public class Co8760GetEnumerator
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringDictionary.GetEnumerator()";
    public static String s_strTFName        = "Co8760GetEnumerator.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public const int MAX_LEN = 50;          
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        StringDictionary sd; 
        IEnumerator en; 
        DictionaryEntry curr;        
        string [] values = 
        {
            "a",
            "aa",
            "",
            " ",
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
        try
        {
            Console.WriteLine("--- create dictionary ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sd = new StringDictionary();
            Console.WriteLine("1. Enumerator for empty dictionary");
            Console.WriteLine("     - get type");
            iCountTestcases++;
            en = sd.GetEnumerator();
            string type = en.GetType().ToString();
            if ( type.IndexOf("Enumerator", 0) == 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, type is not Enumerator");
            }
            Console.WriteLine("     - MoveNext");
            iCountTestcases++;
            bool res = en.MoveNext();
            if ( res ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, MoveNext returned true");
            }
            Console.WriteLine("     - Current");
            iCountTestcases++;
            try 
            {
                curr = (DictionaryEntry)en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0001c, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001d, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("2. GetEnumerator for filled collection");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            for (int i = 0; i < values.Length; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            Console.WriteLine("     - get type");
            iCountTestcases++;
            en = sd.GetEnumerator();
            type = en.GetType().ToString();
            if ( type.IndexOf("Enumerator", 0) == 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, type is not Enumerator");
            }
            Console.WriteLine("     - MoveNext and Current within collection");
            for (int i = 0; i < sd.Count; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
                if ( !res ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002b_{0}, MoveNext returned false", i);
                }
                iCountTestcases++;
                curr = (DictionaryEntry)en.Current;
                if (! sd.ContainsValue(curr.Value.ToString()) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002c_{0}, Current dictionary doesn't contain value from enumerator", i);
                }
                iCountTestcases++;
                if (! sd.ContainsKey(curr.Key.ToString()) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002d_{0}, Current dictionary doesn't contain key from enumerator", i);
                }
                iCountTestcases++;
                if ( String.Compare(sd[curr.Key.ToString()], curr.Value.ToString(), false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002e_{0}, Value for current Key is different in dictionary", i);
                }
                iCountTestcases++;
                DictionaryEntry curr1 = (DictionaryEntry)en.Current;
                if (! curr.Equals(curr1) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002f_{0}, second call of Current returned different result", i);
                }
            }
            res = en.MoveNext();
            Console.WriteLine("     - MoveNext outside of the collection");
            iCountTestcases++;
            res = en.MoveNext();
            if ( res ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002g, MoveNext returned true");
            }
            Console.WriteLine("     - Current outside of the collection");
            iCountTestcases++;
            try 
            {
                curr = (DictionaryEntry)en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0002h, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002i, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("     - Reset");
            iCountTestcases++;
            en.Reset();
            Console.WriteLine("     - get Current after Reset");
            iCountTestcases++;
            try 
            {
                curr = (DictionaryEntry)en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0002j, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002k, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("3. Enumerator and modified dictionary");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            if (sd.Count < 1) 
            {
                for (int i = 0; i < values.Length; i++) 
                {
                    sd.Add(keys[i], values[i]);
                }
            }
            iCountTestcases++;
            en = sd.GetEnumerator();
            Console.WriteLine("     - MoveNext");
            res = en.MoveNext();
            if (!res) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, MoveNext returned false");
            }
            Console.WriteLine("     - modify collection");
            curr = (DictionaryEntry)en.Current;
            int cnt = sd.Count;
            iCountTestcases++;
            sd.Remove(keys[0]);
            if ( sd.Count != cnt - 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, didn't remove item with 0th key");
            }
            Console.WriteLine("     - get Current");
            iCountTestcases++;
            DictionaryEntry curr2 = (DictionaryEntry)en.Current;
            if (! curr.Equals(curr2) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, current returned different value after midification");
            }
            Console.WriteLine("     - call MoveNext");
            iCountTestcases++;
            try 
            {
                res = en.MoveNext();
                iCountErrors++;
                Console.WriteLine("Err_0003d, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003e, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("4. Modify dictionary after enumerated beyond the end");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            sd.Clear();
            for (int i = 0; i < values.Length; i++) 
            {
                sd.Add(keys[i], values[i]);
            }
            iCountTestcases++;
            en = sd.GetEnumerator();
            for (int i = 0; i < sd.Count; i ++) 
            {
                en.MoveNext();
            }
            Console.WriteLine("     - get Current at the end of the dictionary");
            curr = (DictionaryEntry)en.Current;
            Console.WriteLine("     - modify collection");
            curr = (DictionaryEntry)en.Current;
            cnt = sd.Count;
            iCountTestcases++;
            sd.Remove(keys[0]);
            if ( sd.Count != cnt - 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, didn't remove item with 0th key");
            }
            Console.WriteLine("     - get Current after modifying");
            iCountTestcases++;
            curr2 = (DictionaryEntry)en.Current;
            if (! curr.Equals(curr2) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004c, current returned different value after midification");
            }
            Console.WriteLine("     - call MoveNext after modifying");
            iCountTestcases++;
            try 
            {
                res = en.MoveNext();
                iCountErrors++;
                Console.WriteLine("Err_0004d, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004e, unexpected exception: {0}", e.ToString());
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
        Co8760GetEnumerator cbA = new Co8760GetEnumerator();
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
