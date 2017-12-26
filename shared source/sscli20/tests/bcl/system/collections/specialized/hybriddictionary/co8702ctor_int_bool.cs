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
public class Co8702ctor_int_bool
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "HybridDictionary.ctor(int, bool)";
    public static String s_strTFName        = "Co8702ctor_int_bool.cs";
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
            Console.WriteLine("--- ctor (false) ---");
            strLoc = "Loc_001ooA-false"; 
            iCountTestcases++;
            Console.WriteLine(" A. ctor(0, false)");
            strLoc = "Loc_000ooA";
            hd = new HybridDictionary(0, false);
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
            Console.WriteLine("6. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value");
            }
            hd.Add("NaMe", "Value");
            if (hd.Count != 2) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006c: Count returned {0} instead of 2", hd.Count);
            }
            if (String.Compare(hd["NaMe"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006d: Item() returned unexpected value");
            }
            if (hd["NAME"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006e: Item() returned non-null value for uppercase key");
            }
            Console.WriteLine("7. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("8. multiple Add(name, value)");
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
                Console.WriteLine("Err_008a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008b_{0}: Item() returned unexpected value", i);
                }
                if (hd[keysLong[i].ToUpper()] != null)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008c_{0}: Item() returned non-null for uppercase key", i);
                }
            }
            Console.WriteLine("9. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine(" B. ctor(10, false)");
            strLoc = "Loc_000ooB-false";
            hd = new HybridDictionary(10, false);
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
            Console.WriteLine("6. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value");
            }
            hd.Add("NaMe", "Value");
            if (hd.Count != 2) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006c: Count returned {0} instead of 2", hd.Count);
            }
            if (String.Compare(hd["NaMe"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006d: Item() returned unexpected value");
            }
            if (hd["NAME"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006e: Item() returned non-null value for uppercase key");
            }
            Console.WriteLine("7. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("8. multiple Add(name, value)");
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
                Console.WriteLine("Err_008a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008b_{0}: Item() returned unexpected value", i);
                }
                if (hd[keysLong[i].ToUpper()] != null)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008c_{0}: Item() returned non-null for uppercase key", i);
                }
            }
            Console.WriteLine("9. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine(" C. ctor(100, false)");
            strLoc = "Loc_000ooC-false";
            hd = new HybridDictionary(100, false);
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
            Console.WriteLine("6. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value");
            }
            hd.Add("NaMe", "Value");
            if (hd.Count != 2) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006c: Count returned {0} instead of 2", hd.Count);
            }
            if (String.Compare(hd["NaMe"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006d: Item() returned unexpected value");
            }
            if (hd["NAME"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006e: Item() returned non-null value for uppercase key");
            }
            Console.WriteLine("7. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("8. multiple Add(name, value)");
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
                Console.WriteLine("Err_008a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008b_{0}: Item() returned unexpected value", i);
                }
                if (hd[keysLong[i].ToUpper()] != null)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008c_{0}: Item() returned non-null for uppercase key", i);
                }
            }
            Console.WriteLine("9. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine("");
            Console.WriteLine("--- ctor (true) ---");
            strLoc = "Loc_000ooA-true";
            iCountTestcases++;
            Console.WriteLine(" A. ctor(0, true)");
            hd = new HybridDictionary(0, true);
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
            Console.WriteLine("6. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value");
            }
            try 
            {
                hd.Add("NaMe", "vl");
                iCountErrors++;
                Console.WriteLine("Err_0006Noex: no exception");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006ex: unexpected exception: {0}", e.ToString());
            }
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["NAME"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value for uppercase key");
            }
            Console.WriteLine("7. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("8. multiple Add(name, value)");
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
                Console.WriteLine("Err_008a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008b_{0}: Item() returned unexpected value", i);
                }
                if (String.Compare(hd[keysLong[i].ToUpper()].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008c_{0}: Item() returned unexpected value for uppercase key", i);
                }
            }
            Console.WriteLine("9. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine("--- ctor (true) ---");
            strLoc = "Loc_000ooB-true";
            iCountTestcases++;
            Console.WriteLine(" B. ctor(10, true)");
            hd = new HybridDictionary(10, true);
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
            Console.WriteLine("6. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value");
            }
            try 
            {
                hd.Add("NaMe", "vl");
                iCountErrors++;
                Console.WriteLine("Err_0006Noex: no exception");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006ex: unexpected exception: {0}", e.ToString());
            }
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["NAME"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value for uppercase key");
            }
            Console.WriteLine("7. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("8. multiple Add(name, value)");
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
                Console.WriteLine("Err_008a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008b_{0}: Item() returned unexpected value", i);
                }
                if (String.Compare(hd[keysLong[i].ToUpper()].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008c_{0}: Item() returned unexpected value for uppercase key", i);
                }
            }
            Console.WriteLine("9. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            Console.WriteLine("--- ctor (true) ---");
            strLoc = "Loc_000ooC-true";
            iCountTestcases++;
            Console.WriteLine(" C. ctor(100, true)");
            hd = new HybridDictionary(100, true);
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
            Console.WriteLine("6. Add(name, value)");
            iCountTestcases++;
            hd.Add("Name", "Value");
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value");
            }
            try 
            {
                hd.Add("NaMe", "vl");
                iCountErrors++;
                Console.WriteLine("Err_0006Noex: no exception");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006ex: unexpected exception: {0}", e.ToString());
            }
            if (hd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006a: Count returned {0} instead of 1", hd.Count);
            }
            if (String.Compare(hd["NAME"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006b: Item() returned unexpected value for uppercase key");
            }
            Console.WriteLine("7. Clear() short dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007a: Count returned {0} instead of 0 after Clear()", hd.Count);
            }
            if (hd["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007b: Item() returned non-null value after Clear()");
            }
            Console.WriteLine("8. multiple Add(name, value)");
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
                Console.WriteLine("Err_008a: Count returned {0} instead of {1}", hd.Count, len);
            }
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if (String.Compare(hd[keysLong[i]].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008b_{0}: Item() returned unexpected value", i);
                }
                if (String.Compare(hd[keysLong[i].ToUpper()].ToString(), valuesLong[i], false) != 0)  
                {  
                    iCountErrors++;
                    Console.WriteLine("Err_008c_{0}: Item() returned unexpected value for uppercase key", i);
                }
            }
            Console.WriteLine("9. Clear() long dictionary");
            iCountTestcases++;
            hd.Clear();
            if (hd.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009: Count returned {0} instead of 0 after Clear()", hd.Count);
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
        Co8702ctor_int_bool cbA = new Co8702ctor_int_bool();
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
