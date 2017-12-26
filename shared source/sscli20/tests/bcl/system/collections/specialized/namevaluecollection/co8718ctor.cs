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
public class Co8718ctor
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.ctor";
    public static String s_strTFName        = "Co8718ctor.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        NameValueCollection nvc; 
        try
        {
            Console.WriteLine("--- default ctor ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection();
            Console.WriteLine("1. compare to null");
            iCountTestcases++;
            if (nvc == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, collection is null after default ctor");
            } 
            Console.WriteLine("2. check Count");
            iCountTestcases++;
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Count = {0} after default ctor", nvc.Count);
            }
            Console.WriteLine("3. check Get(some_key)");
            iCountTestcases++;
            if (nvc.Get("key") != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, Get(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("4. check ToString()");
            iCountTestcases++;
            string temp = nvc.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("NameValueCollection") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ToString() doesn't contain \"NameValueCollection\"");
            }
            Console.WriteLine("5. check returned Type");
            iCountTestcases++;
            temp = nvc.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("NameValueCollection") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0005: returned type doesn't contain \"NameValueCollection\"");
            }
            Console.WriteLine("6. compare returned Type of two collection");
            iCountTestcases++;
            string temp1 = (new NameValueCollection()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: returned types of two collections differ");
            }
            Console.WriteLine("7. check AllKeys array");
            iCountTestcases++;
            string [] keys = nvc.AllKeys;
            if ( keys.Length != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007: AllKeys contains {0} keys after default ctor", keys.Length);
            }
            Console.WriteLine("8. check Item(some_key)");
            iCountTestcases++;
            if (nvc["key"] != null) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0008: Item(some_key) returned non-null after default ctor");
            }
            Console.WriteLine("9. Add(name, value)");
            iCountTestcases++;
            nvc.Add("Name", "Value");
            if (nvc.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009a: Count returned {0} instead of 1", nvc.Count);
            }
            if (String.Compare(nvc["Name"], "Value", false) != 0)  
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009b: Item() returned unexpected value");
            }
            Console.WriteLine("10. Clear()");
            iCountTestcases++;
            nvc.Clear();
            if (nvc.Count != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_00010a: Count returned {0} instead of 0 after Clear()", nvc.Count);
            }
            if (nvc["Name"] != null)  
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
        Co8718ctor cbA = new Co8718ctor();
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
