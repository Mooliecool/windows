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
public class Co8719ctor_nvc
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.ctor(NameValueCollection)";
    public static String s_strTFName        = "Co8719ctor_nvc.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        NameValueCollection nvc; 
        NameValueCollection nvc1;         
        string [] values = 
        {
            "",
            " ",
            "a",
            "aa",
            "tExt",
            "     SPaces",
            "1",
            "$%^#",
            "2222222222222222222222222",
            System.DateTime.Today.ToString(),
            Int32.MaxValue.ToString()
        };
        string [] names = 
        {
            "zero",
            "oNe",
            " ",
            "",
            "aA",
            "1",
            System.DateTime.Today.ToString(),
            "$%^#",
            Int32.MaxValue.ToString(),
            "     spaces",
            "2222222222222222222222222"
        };
        try
        {
            Console.WriteLine("--- default ctor ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc1 = new NameValueCollection();
            Console.WriteLine("1. Create from another empty collection");
            nvc = new NameValueCollection(nvc1);
            Console.WriteLine("1.1 compare to null");
            iCountTestcases++;
            if (nvc == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.1, collection is null");
            } 
            Console.WriteLine("1.2. check Count");
            iCountTestcases++;
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.2, Count = {0} ", nvc.Count);
            }
            Console.WriteLine("1.3. check Get(some_key)");
            iCountTestcases++;
            if (nvc.Get("key") != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.3, Get(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("1.4. check ToString()");
            iCountTestcases++;
            string temp = nvc.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("NameValueCollection") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.4, ToString() doesn't contain \"NameValueCollection\"");
            }
            Console.WriteLine("1.5. check returned Type");
            iCountTestcases++;
            temp = nvc.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("NameValueCollection") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.5: returned type doesn't contain \"NameValueCollection\"");
            }
            Console.WriteLine("1.6. compare returned Type of two collection");
            iCountTestcases++;
            string temp1 = (new NameValueCollection()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.6: returned types of two collections differ");
            }
            Console.WriteLine("1.7. check AllKeys array");
            iCountTestcases++;
            string [] keys = nvc.AllKeys;
            if ( keys.Length != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.7: AllKeys contains {0} keys after default ctor", keys.Length);
            }
            Console.WriteLine("1.8. check Item(some_key)");
            iCountTestcases++;
            if (nvc["key"] != null) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.8: Item(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("1.9. Add(name, value)");
            iCountTestcases++;
            nvc.Add("Name", "Value");
            if (nvc.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.9a: Count returned {0} instead of 1", nvc.Count);
            }
            if (String.Compare(nvc["Name"], "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.9b: Item() returned unexpected value");
            }
            Console.WriteLine("1.10. Clear()");
            iCountTestcases++;
            nvc.Clear();
            if (nvc.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.10a: Count returned {0} instead of 0 after Clear()", nvc.Count);
            }
            if (nvc["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0001.10b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("2. Create from filled collection");
            int len = values.Length;
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            for (int i = 0 ; i < len; i ++) 
            {
                nvc1.Add(names[i], values[i]);
            }
            if (nvc1.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, Count = {0} after instead of {1}", nvc.Count, len);
            }
            nvc = new NameValueCollection(nvc1);
            Console.WriteLine("2.1. check Count");
            iCountTestcases++;
            if (nvc.Count != nvc1.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.1, Count = {0} instead of {1}", nvc.Count, nvc1.Count);
            }
            string [] keys1 = nvc1.AllKeys;
            keys = nvc.AllKeys; 
            Console.WriteLine("2.2. check Keys");
            iCountTestcases++;
            if (keys1.Length != keys.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.2a, new collection  Keys.Length is {0} instead of {1}", keys.Length, keys1.Length);
            }
            else 
            {
                for (int i = 0; i < keys1.Length; i++) 
                {
                    iCountTestcases++;
                    if ( Array.IndexOf(keys, keys1[i]) < 0 )  
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0002.2b_{0}, no key \"{1}\" in AllKeys", i, keys1[i] );
                    }
                }
            }
            Console.WriteLine("2.3. check Values");
            iCountTestcases++;
            for (int i = 0; i < keys.Length; i++) 
            {
                iCountTestcases++;
                string [] val = nvc.GetValues(keys[i]);
                if ( (val.Length != 1) || String.Compare(val[0], (nvc1.GetValues(keys[i]))[0], false) != 0  )  
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002.3_{0}, unexpected value at key \"{1}\"", i, keys1[i] );
                }
            }
            Console.WriteLine("3. change argument collection");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            string toChange = keys1[0];
            string init = nvc1[toChange];
            Console.WriteLine(" - change element in argument");
            iCountTestcases++;
            nvc1[toChange] = "new Value";
            if (String.Compare(nvc1[toChange], "new Value", false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, failed to change element");
            }
            iCountTestcases++;
            if (String.Compare(nvc[toChange], init, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, changed element in new collection");
            }
            Console.WriteLine(" - remove element from argument");
            nvc1.Remove(toChange);
            iCountTestcases++;
            if (nvc1.Count != len - 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, failed to remove element");
            }
            iCountTestcases++;
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003d, collection changed after argument change - removed element");
            }
            iCountTestcases++;
            keys = nvc.AllKeys;
            if ( Array.IndexOf(keys, toChange) < 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003e, collection changed after argument change - no key");
            }
            Console.WriteLine("4. invalid parameter");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            try 
            {
                nvc = new NameValueCollection((NameValueCollection)null);
                iCountErrors++;
                Console.WriteLine("Err_0004a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine("     expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, unexpected exception: {0}", e.ToString());
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
        Co8719ctor_nvc cbA = new Co8719ctor_nvc();
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
