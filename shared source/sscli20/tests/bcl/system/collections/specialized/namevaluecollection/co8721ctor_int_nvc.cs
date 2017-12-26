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
public class Co8721ctor_int_nvc
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.ctor(int, NameValueCollection)";
    public static String s_strTFName        = "Co8721ctor_int_nvc.cs";
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
            Console.WriteLine("--- create extra collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc1 = new NameValueCollection(10);
            Console.WriteLine("1. Create w/o capacity from empty w capacity");
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
            Console.WriteLine("1.3. check Capacity ");
            Console.WriteLine("2. Create w capacity from empty w same capacity");
            nvc = new NameValueCollection(10, nvc1);
            Console.WriteLine("2.1 compare to null");
            iCountTestcases++;
            if (nvc == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.1, collection is null");
            } 
            Console.WriteLine("2.2. check Count");
            iCountTestcases++;
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.2, Count = {0} ", nvc.Count);
            }
            Console.WriteLine("2.3. check Capacity ");
            Console.WriteLine("3. Create w capacity from empty w greater capacity");
            nvc = new NameValueCollection(5, nvc1);
            Console.WriteLine("3.1 compare to null");
            iCountTestcases++;
            if (nvc == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003.1, collection is null");
            } 
            Console.WriteLine("3.2. check Count");
            iCountTestcases++;
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003.2, Count = {0} ", nvc.Count);
            }
            Console.WriteLine("3.3. check Capacity ");
            Console.WriteLine("4. Create w capacity from empty w smaller capacity");
            nvc = new NameValueCollection(50, nvc1);
            Console.WriteLine("4.1 compare to null");
            iCountTestcases++;
            if (nvc == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004.1, collection is null");
            } 
            Console.WriteLine("4.2. check Count");
            iCountTestcases++;
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004.2, Count = {0} ", nvc.Count);
            }
            Console.WriteLine("4.3. check Capacity ");
            Console.WriteLine("5. Create from filled collection - smaller capacity ");
            int len = values.Length;
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            for (int i = 0 ; i < len; i ++) 
            {
                nvc1.Add(names[i], values[i]);
            }
            if (nvc1.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, Count = {0} after instead of {1}", nvc.Count, len);
            }
            Console.WriteLine(" - new ({0}, other_collection_Count_{1})", (int)len/2, nvc1.Count );
            nvc = new NameValueCollection(len/2, nvc1);
            Console.WriteLine("5.1. check Count");
            iCountTestcases++;
            if (nvc.Count != nvc1.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005.1, Count = {0} instead of {1}", nvc.Count, nvc1.Count);
            }
            Console.WriteLine("5.2. check Capacity");
            string [] keys1 = nvc1.AllKeys;
            string [] keys = nvc.AllKeys; 
            Console.WriteLine("5.3. check Keys");
            iCountTestcases++;
            if (keys1.Length != keys.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005.3a, new collection  Keys.Length is {0} instead of {1}", keys.Length, keys1.Length);
            }
            else 
            {
                for (int i = 0; i < keys1.Length; i++) 
                {
                    iCountTestcases++;
                    if ( Array.IndexOf(keys, keys1[i]) < 0 )  
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0005.3b_{0}, no key \"{1}\" in AllKeys", i, keys1[i] );
                    }
                }
            }
            Console.WriteLine("5.4. check Values");
            iCountTestcases++;
            for (int i = 0; i < keys.Length; i++) 
            {
                iCountTestcases++;
                string [] val = nvc.GetValues(keys[i]);
                if ( (val.Length != 1) || String.Compare(val[0], (nvc1.GetValues(keys[i]))[0], false) != 0  )  
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005.4_{0}, unexpected value at key \"{1}\"", i, keys1[i] );
                }
            }
            Console.WriteLine("6. Create from filled collection - count capacity ");
            len = values.Length;
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            Console.WriteLine(" - new ({0}, other_collection_Count_{1})", len, nvc1.Count );
            nvc = new NameValueCollection(len, nvc1);
            Console.WriteLine("6.1. check Count");
            iCountTestcases++;
            if (nvc.Count != nvc1.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006.1, Count = {0} instead of {1}", nvc.Count, nvc1.Count);
            }
            Console.WriteLine("6.2. check Capacity");
            keys1 = nvc1.AllKeys;
            keys = nvc.AllKeys; 
            Console.WriteLine("6.3. check Keys");
            iCountTestcases++;
            if (keys1.Length != keys.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006.3a, new collection  Keys.Length is {0} instead of {1}", keys.Length, keys1.Length);
            }
            else 
            {
                for (int i = 0; i < keys1.Length; i++) 
                {
                    iCountTestcases++;
                    if ( Array.IndexOf(keys, keys1[i]) < 0 )  
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0006.3b_{0}, no key \"{1}\" in AllKeys", i, keys1[i] );
                    }
                }
            }
            Console.WriteLine("6.4. check Values");
            iCountTestcases++;
            for (int i = 0; i < keys.Length; i++) 
            {
                iCountTestcases++;
                string [] val = nvc.GetValues(keys[i]);
                if ( (val.Length != 1) || String.Compare(val[0], (nvc1.GetValues(keys[i]))[0], false) != 0  )  
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006.4_{0}, unexpected value at key \"{1}\"", i, keys1[i] );
                }
            }
            Console.WriteLine("7. Create from filled collection - greater capacity capacity ");
            len = values.Length;
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            Console.WriteLine(" - new ({0}, other_collection_Count_{1})", len*2, nvc1.Count );
            nvc = new NameValueCollection(len*2, nvc1);
            Console.WriteLine("7.1. check Count");
            iCountTestcases++;
            if (nvc.Count != nvc1.Count) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007.1, Count = {0} instead of {1}", nvc.Count, nvc1.Count);
            }
            Console.WriteLine("7.2. check Capacity");
            keys1 = nvc1.AllKeys;
            keys = nvc.AllKeys; 
            Console.WriteLine("7.3. check Keys");
            iCountTestcases++;
            if (keys1.Length != keys.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007.3a, new collection  Keys.Length is {0} instead of {1}", keys.Length, keys1.Length);
            }
            else 
            {
                for (int i = 0; i < keys1.Length; i++) 
                {
                    iCountTestcases++;
                    if ( Array.IndexOf(keys, keys1[i]) < 0 )  
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0007.3b_{0}, no key \"{1}\" in AllKeys", i, keys1[i] );
                    }
                }
            }
            Console.WriteLine("7.4. check Values");
            iCountTestcases++;
            for (int i = 0; i < keys.Length; i++) 
            {
                iCountTestcases++;
                string [] val = nvc.GetValues(keys[i]);
                if ( (val.Length != 1) || String.Compare(val[0], (nvc1.GetValues(keys[i]))[0], false) != 0  )  
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0007.4_{0}, unexpected value at key \"{1}\"", i, keys1[i] );
                }
            }
            Console.WriteLine("8. change argument collection");
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            string toChange = keys1[0];
            string init = nvc1[toChange];
            Console.WriteLine(" - change element in argument");
            iCountTestcases++;
            nvc1[toChange] = "new Value";
            if (String.Compare(nvc1[toChange], "new Value", false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008a, failed to change element");
            }
            iCountTestcases++;
            if (String.Compare(nvc[toChange], init, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008b, changed element in new collection");
            }
            Console.WriteLine(" - remove element from argument");
            nvc1.Remove(toChange);
            iCountTestcases++;
            if (nvc1.Count != len - 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008c, failed to remove element");
            }
            iCountTestcases++;
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008d, collection changed after argument change - removed element");
            }
            iCountTestcases++;
            keys = nvc.AllKeys;
            if ( Array.IndexOf(keys, toChange) < 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008e, collection changed after argument change - no key");
            }
            Console.WriteLine("9. negative capacity");
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            try 
            {
                nvc = new NameValueCollection(-1, nvc1);
                iCountErrors++;
                Console.WriteLine("Err_0009a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("     expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("10. null argument collection");
            strLoc = "Loc_0010oo"; 
            iCountTestcases++;
            try 
            {
                nvc = new NameValueCollection(10, (NameValueCollection)null);
                iCountErrors++;
                Console.WriteLine("Err_00010a, no exception");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine("     expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_00010b, unexpected exception: {0}", e.ToString());
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
        Co8721ctor_int_nvc cbA = new Co8721ctor_int_nvc();
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
