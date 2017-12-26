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
public class Co8701ctor_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "HybridDictionary.ctor(int)";
    public static String s_strTFName        = "Co8701ctor_int.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public const int MAX_LEN = 50;          
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        HybridDictionary hd; 
        const int BIG_LENGTH = 100;
        string [] valuesLong = new string[BIG_LENGTH];
        string [] keysLong = new string[BIG_LENGTH];
        int len;
        try
        {
            for (int i = 0; i < BIG_LENGTH; i++) 
            {
                valuesLong[i] = "Item" + i;
                keysLong[i] = "keY" + i;
            } 
            Console.WriteLine("--- create dictionary ---");
            Console.WriteLine("");
            strLoc = "Loc_001oo-0"; 
            iCountTestcases++;
            Console.WriteLine(" A. ctor(0)");
            hd = new HybridDictionary(0);
            Console.WriteLine("1. compare to null");
            iCountTestcases++;
            if (hd == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, dictionary is null after default ctor");
            } 
            Console.WriteLine("2. check Count");
            iCountTestcases++;
            if (hd.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Count = {0} after default ctor", hd.Count);
            }
            Console.WriteLine("3. check Item(some_key)");
            iCountTestcases++;
            if (hd["key"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, Item(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("4. check ToString()");
            iCountTestcases++;
            string temp = hd.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("HybridDictionary") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ToString() doesn't contain \"HybridDictionary\"");
            }
            Console.WriteLine("5. check returned Type");
            iCountTestcases++;
            temp = hd.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("HybridDictionary") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0005: returned type doesn't contain \"HybridDictionary\"");
            }
            Console.WriteLine("6. compare returned Type of two dictionary");
            iCountTestcases++;
            string temp1 = (new HybridDictionary()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: returned types of two collections differ");
            }
            Console.WriteLine("7. check Keys collection");
            iCountTestcases++;
            System.Collections.ICollection keys = hd.Keys;
            if ( keys.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007: Keys contains {0} keys after default ctor", keys.Count);
            }
            Console.WriteLine("8. check Values collection");
            iCountTestcases++;
            System.Collections.ICollection values = hd.Values;
            if ( values.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0008: Values contains {0} items after default ctor", values.Count);
            }
            Console.WriteLine("9. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009b: Item() returned unexpected value");
            }
            if (hd["NAME"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009c: Item() returned non-null value for uppercase key");
            }
            Console.WriteLine("10. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("11. multiple Add(name, value)");
            iCountTestcases++;
            len = valuesLong.Length;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            iCountTestcases++;
            if (hd.Count != len) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0011a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_0011b_{0}: Item() returned unexpected value", i);
                }
                if (hd[keysLong[i].ToUpper()] != null)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_0011c_{0}: Item() returned non-null for uppercase key", i);
                }
            }
            Console.WriteLine("12. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00012: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine("");
            strLoc = "Loc_001ooB-10"; 
            iCountTestcases++;
            Console.WriteLine(" B. ctor(10)");
            hd = new HybridDictionary(10);
            Console.WriteLine("1. compare to null");
            iCountTestcases++;
            if (hd == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, dictionary is null after default ctor");
            } 
            Console.WriteLine("2. check Count");
            iCountTestcases++;
            if (hd.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Count = {0} after default ctor", hd.Count);
            }
            Console.WriteLine("3. check Item(some_key)");
            iCountTestcases++;
            if (hd["key"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, Item(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("4. check ToString()");
            iCountTestcases++;
            temp = hd.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("HybridDictionary") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ToString() doesn't contain \"HybridDictionary\"");
            }
            Console.WriteLine("5. check returned Type");
            iCountTestcases++;
            temp = hd.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("HybridDictionary") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0005: returned type doesn't contain \"HybridDictionary\"");
            }
            Console.WriteLine("6. compare returned Type of two dictionary");
            iCountTestcases++;
            temp1 = (new HybridDictionary()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: returned types of two collections differ");
            }
            Console.WriteLine("7. check Keys collection");
            iCountTestcases++;
            keys = hd.Keys;
            if ( keys.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007: Keys contains {0} keys after default ctor", keys.Count);
            }
            Console.WriteLine("8. check Values collection");
            iCountTestcases++;
            values = hd.Values;
            if ( values.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0008: Values contains {0} items after default ctor", values.Count);
            }
            Console.WriteLine("9. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009b: Item() returned unexpected value");
            }
            if (hd["NAME"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009c: Item() returned non-null value for uppercase key");
            }
            Console.WriteLine("10. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("11. multiple Add(name, value)");
            iCountTestcases++;
            len = valuesLong.Length;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            iCountTestcases++;
            if (hd.Count != len) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0011a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_0011b_{0}: Item() returned unexpected value", i);
                }
                if (hd[keysLong[i].ToUpper()] != null)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_0011c_{0}: Item() returned non-null for uppercase key", i);
                }
            }
            Console.WriteLine("12. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00012: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine("");
            strLoc = "Loc_001ooC-100"; 
            iCountTestcases++;
            Console.WriteLine(" C. ctor(100)");
            hd = new HybridDictionary(100);
            Console.WriteLine("1. compare to null");
            iCountTestcases++;
            if (hd == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, dictionary is null after default ctor");
            } 
            Console.WriteLine("2. check Count");
            iCountTestcases++;
            if (hd.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Count = {0} after default ctor", hd.Count);
            }
            Console.WriteLine("3. check Item(some_key)");
            iCountTestcases++;
            if (hd["key"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, Item(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("4. check ToString()");
            iCountTestcases++;
            temp = hd.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("HybridDictionary") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ToString() doesn't contain \"HybridDictionary\"");
            }
            Console.WriteLine("5. check returned Type");
            iCountTestcases++;
            temp = hd.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("HybridDictionary") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0005: returned type doesn't contain \"HybridDictionary\"");
            }
            Console.WriteLine("6. compare returned Type of two dictionary");
            iCountTestcases++;
            temp1 = (new HybridDictionary()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: returned types of two collections differ");
            }
            Console.WriteLine("7. check Keys collection");
            iCountTestcases++;
            keys = hd.Keys;
            if ( keys.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007: Keys contains {0} keys after default ctor", keys.Count);
            }
            Console.WriteLine("8. check Values collection");
            iCountTestcases++;
            values = hd.Values;
            if ( values.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0008: Values contains {0} items after default ctor", values.Count);
            }
            Console.WriteLine("9. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009b: Item() returned unexpected value");
            }
            if (hd["NAME"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009c: Item() returned non-null value for uppercase key");
            }
            Console.WriteLine("10. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("11. multiple Add(name, value)");
            iCountTestcases++;
            len = valuesLong.Length;
            hd.Clear();
            for (int i = 0; i < len; i++) 
            {
                hd.Add(keysLong[i], valuesLong[i]);
            }
            iCountTestcases++;
            if (hd.Count != len) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0011a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_0011b_{0}: Item() returned unexpected value", i);
                }
                if (hd[keysLong[i].ToUpper()] != null)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_0011c_{0}: Item() returned non-null for uppercase key", i);
                }
            }
            Console.WriteLine("12. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00012: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine("");
            Console.WriteLine("15. ctor(-1) - no exception");
            try 
            {
                hd = new HybridDictionary(-1);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_00015a: unexpected exception: {0}", e.ToString());
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
        Co8701ctor_int cbA = new Co8701ctor_int();
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
