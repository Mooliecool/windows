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
public class Co8770Reset
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringEnumerator.Reset()";
    public static String s_strTFName        = "Co8770Reset.cs";
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
            Console.WriteLine("1. Reset() on empty collection");
            Console.WriteLine("     - Reset()");
            iCountTestcases++;
            en = sc.GetEnumerator();
            try 
            {
                en.Reset();
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, unexpected exception: " + e.ToString());
            }
            Console.WriteLine("     - Current");
            iCountTestcases++;
            try 
            {
                curr = en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0001b, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("     - Add item to the collection");
            iCountTestcases++;
            int cnt = sc.Count;
            sc.Add(values[0]);
            if ( sc.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001d, failed to add item");
            }
            Console.WriteLine("     - Reset() on modified collection");
            try 
            {
                en.Reset();
                iCountErrors++;
                Console.WriteLine("Err_0001e, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001f, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("2. Reset() on filled collection");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            sc.AddRange(values);
            en = sc.GetEnumerator();
            Console.WriteLine("     - Reset() after Reset()");
            iCountTestcases++;
            try 
            {
                en.Reset();
                en.Reset();
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, unexpected exception: " + e.ToString());
            }
            Console.WriteLine("     - Reset() after 0th item ");
            iCountTestcases++;
            if (!en.MoveNext()) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, MoveNext() returned false");
            }
            iCountTestcases++;
            curr = en.Current;
            if (String.Compare(curr, values[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002c, Current returned wrong value");
            }
            en.Reset();
            if (!en.MoveNext()) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002d, MoveNext() returned false");
            }
            iCountTestcases++;
            if (String.Compare(en.Current, curr, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002e, Current returned wrong value");
            }
            int ind = sc.Count / 2;
            Console.WriteLine("     - Reset() after {0} item ", ind);
            en.Reset();
            for (int i = 0; i < ind + 1; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
                if ( !res ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002f_{0}, MoveNext returned false", i);
                }
                iCountTestcases++;
                curr = en.Current;
                if (String.Compare(curr, sc[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002g_{0}, Current returned \"{1}\" instead of \"{2}\"", i, curr, sc[i]);
                }
                iCountTestcases++;
                string curr1 = en.Current;
                if (String.Compare(curr, curr1, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002h_{0}, second call of Current returned different result", i);
                }
            }
            en.Reset();
            if (!en.MoveNext()) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002i, MoveNext() returned false");
            }
            iCountTestcases++;
            if (String.Compare(en.Current, sc[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002j, Current returned wrong value");
            }
            ind = sc.Count;
            Console.WriteLine("     - Reset() after {0} item ", ind-1);
            en.Reset();
            for (int i = 0; i < ind; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
                if ( !res ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002k_{0}, MoveNext returned false", i);
                }
                iCountTestcases++;
                curr = en.Current;
                if (String.Compare(curr, sc[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002l_{0}, Current returned \"{1}\" instead of \"{2}\"", i, curr, sc[i]);
                }
                iCountTestcases++;
                string curr1 = en.Current;
                if (String.Compare(curr, curr1, false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002m_{0}, second call of Current returned different result", i);
                }
            }
            en.Reset();
            if (!en.MoveNext()) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002n, MoveNext() returned false");
            }
            iCountTestcases++;
            if (String.Compare(en.Current, sc[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002o, Current returned wrong value");
            }
            en.Reset();
            Console.WriteLine("     - Reset() after enumerated beyond the last item");
            for (int i = 0; i < ind; i++) 
            {
                res = en.MoveNext();
            }
            res = en.MoveNext();
            iCountTestcases++;
            res = en.MoveNext();
            if ( res ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002p, MoveNext returned true");
            }
            en.Reset();
            if (!en.MoveNext()) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002q, MoveNext() returned false");
            }
            iCountTestcases++;
            if (String.Compare(en.Current, sc[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002r, Current returned wrong value");
            }
            Console.WriteLine("     - Current after Reset()");
            iCountTestcases++;
            en.Reset();
            try 
            {
                curr = en.Current;
                iCountErrors++;
                Console.WriteLine("Err_0002s, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002t, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("3. Reset() modified collection in process of enumeration");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            if (sc.Count < 1)
                sc.AddRange(values);
            iCountTestcases++;
            en = sc.GetEnumerator();
            Console.WriteLine("     - Reset() for init position of the enumerator");
            sc.RemoveAt(0);
            iCountTestcases++;
            try 
            {
                en.Reset();
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
            Console.WriteLine("     - Enumerate to the middle of the collection and Reset()");
            for (int i = 0; i < sc.Count / 2; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
            }
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
            Console.WriteLine("     - call Reset()");
            iCountTestcases++;
            try 
            {
                en.Reset();
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
            en = sc.GetEnumerator();
            Console.WriteLine("     - Enumerate to end of the collection and Reset()");
            for (int i = 0; i < sc.Count; i++) 
            {
                iCountTestcases++;
                res = en.MoveNext();
            }
            Console.WriteLine("     - modify collection");
            iCountTestcases++;
            sc.RemoveAt(0);
            Console.WriteLine("     - call Reset()");
            iCountTestcases++;
            try 
            {
                en.Reset();
                iCountErrors++;
                Console.WriteLine("Err_0003f, no exception");
            }
            catch (InvalidOperationException ex) 
            {
                Console.WriteLine("  expected exception: " + ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003g, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("4. Reset() after enumerating beyond the end");
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
            cnt = sc.Count;
            iCountTestcases++;
            sc.RemoveAt(0);
            if ( sc.Count != cnt - 1 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, didn't remove 0-item");
            }   
            Console.WriteLine("     - call Reset()");
            iCountTestcases++;
            try 
            {
                en.Reset();
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
        Co8770Reset cbA = new Co8770Reset();
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
