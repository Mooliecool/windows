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
public class Co8782ctor
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ListDictionary.ctor";
    public static String s_strTFName        = "Co8782ctor.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        ListDictionary ld; 
        try
        {
            Console.WriteLine("--- default ctor ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            ld = new ListDictionary();
            Console.WriteLine("1. compare to null");
            iCountTestcases++;
            if (ld == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, dictionary is null after default ctor");
            } 
            Console.WriteLine("2. check Count");
            iCountTestcases++;
            if (ld.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Count = {0} after default ctor", ld.Count);
            }
            Console.WriteLine("3. check Item(some_key)");
            iCountTestcases++;
            if (ld["key"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, Item(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("4. check ToString()");
            iCountTestcases++;
            string temp = ld.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("ListDictionary") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ToString() doesn't contain \"ListDictionary\"");
            }
            Console.WriteLine("5. check returned Type");
            iCountTestcases++;
            temp = ld.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("ListDictionary") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0005: returned type doesn't contain \"ListDictionary\"");
            }
            Console.WriteLine("6. compare returned Type of two dictionary");
            iCountTestcases++;
            string temp1 = (new ListDictionary()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: returned types of two collections differ");
            }
            Console.WriteLine("7. check Keys collection");
            iCountTestcases++;
            System.Collections.ICollection keys = ld.Keys;
            if ( keys.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007: Keys contains {0} keys after default ctor", keys.Count);
            }
            Console.WriteLine("8. check Values collection");
            iCountTestcases++;
            System.Collections.ICollection values = ld.Values;
            if ( values.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0008: Values contains {0} items after default ctor", values.Count);
            }
            Console.WriteLine("9. Add(name, value)");
            iCountTestcases++;
            ld.Add("Name", "Value");
            if (ld.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009a: Count returned {0} instead of 1", ld.Count);
            }
            if (String.Compare(ld["Name"].ToString(), "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009b: Item() returned unexpected value");
            }
            Console.WriteLine("10. Clear()");
            iCountTestcases++;
            ld.Clear();
            if (ld.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010a: Count returned {0} instead of 0 after Clear()", ld.Count);
            }
            if (ld["Name"] != null)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010b: Item() returned non-null value after Clear()");
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
        Co8782ctor cbA = new Co8782ctor();
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
