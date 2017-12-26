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
public class Co8771get_Current
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringEnumerator.Current get";
    public static String s_strTFName        = "Co8771get_Current.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public const int MAX_LEN = 50;          
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        StringCollection sc; 
        StringEnumerator en; 
        string curr;        
        bool res;           
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
        try
        {
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            Console.WriteLine("1. Current on empty collection");
            Console.WriteLine("     - Current");
            en = sc.GetEnumerator();
            iCountTestcases++;
            try 
            {
                curr = en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0001a, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("2. Current on filled collection");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            sc.AddRange(values);
            en = sc.GetEnumerator();
            Console.WriteLine("     - Current before fist MoveNext()");
            iCountTestcases++;
            try 
            {
                curr = en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0002a, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, unexpected exception: {0}", e.ToString());
            }
            int ind = sc.Count;
            Console.WriteLine("     - verify Current for all items");
            en.Reset();
            for (int i = 0; i < ind; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
                if ( !res ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002c_{0}, MoveNext returned false", i);
                }
                iCountTestcases++;
                curr = en.Current;
                if (String.Compare(curr, sc[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002d_{0}, Current returned \"{1}\" instead of \"{2}\"", i, curr, sc[i]);
                }
                iCountTestcases++;
                string curr1 = en.Current;
                if (String.Compare(curr, curr1, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002e_{0}, second call of Current returned different result", i);
                }
            }
            res = en.MoveNext();
            iCountTestcases++;
            res = en.MoveNext();
            if ( res ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002f, MoveNext returned true");
            }
            Console.WriteLine("     - Current after moving beyond the end");
            iCountTestcases++;
            try 
            {
                curr = en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0002g, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002h, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("3. get Current when modified collection in process of enumeration");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            if (sc.Count < 1)
                sc.AddRange(values);
            iCountTestcases++;
            en = sc.GetEnumerator();
            Console.WriteLine("     - Current for init position of the enumerator");
            sc.RemoveAt(0);
            iCountTestcases++;
            try 
            {
                curr = en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0003a, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, unexpected exception: {0}", e.ToString());
            }
            en = sc.GetEnumerator();
            Console.WriteLine("     - Enumerate to the middle of the collection and get Current");
            for (int i = 0; i < sc.Count / 2; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
            }
            curr = en.Current;
            Console.WriteLine("     - modify collection");
            curr = en.Current;
            iCountTestcases++;
            sc.RemoveAt(0);
            Console.WriteLine("     - get Current");
            iCountTestcases++;
            if (String.Compare(curr, en.Current) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, current returned {0} instead of {1}", en.Current, curr);
            }
            Console.WriteLine("     - get Current");
            iCountTestcases++;
            if (String.Compare(curr, en.Current) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, current returned {0} instead of {1}", en.Current, curr);
            }
            en = sc.GetEnumerator();
            Console.WriteLine("     - Enumerate to end of the collection and get Current");
            for (int i = 0; i < sc.Count; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
            }
            curr = en.Current;
            Console.WriteLine("     - modify collection");
            iCountTestcases++;
            sc.RemoveAt(0);
            Console.WriteLine("     - get Current");
            iCountTestcases++;
            try 
            {
                if (String.Compare(en.Current, curr, false) != 0)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003d, no exception");
                }
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003e - unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("4. get Current after enumerating beyond the end and modifying");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            if (sc.Count < 1)
                sc.AddRange(values);
            iCountTestcases++;
            en = sc.GetEnumerator();
            Console.WriteLine("     - Enumerate through the collection");
            for (int i = 0; i < sc.Count; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
            }
            Console.WriteLine("     - Enumerate beyond the end");
            res = en.MoveNext();              
            if ( res ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, MoveNext returned true after moving beyond the end");
            }   
            Console.WriteLine("     - modify collection");
            int cnt = sc.Count;
            iCountTestcases++;
            sc.RemoveAt(0);
            if ( sc.Count != cnt - 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, didn't remove 0-item");
            }   
            Console.WriteLine("     - get Current");
            iCountTestcases++;
            try 
            {
                curr = en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0004c, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004d, unexpected exception: {0}", e.ToString());
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
        Co8771get_Current cbA = new Co8771get_Current();
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
