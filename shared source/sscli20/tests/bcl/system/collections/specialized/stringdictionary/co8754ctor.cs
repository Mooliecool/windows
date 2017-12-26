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
public class Co8754ctor
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringDictionary.ctor";
    public static String s_strTFName        = "Co8754ctor.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        StringDictionary sd; 
        try
        {
            Console.WriteLine("--- default ctor ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sd = new StringDictionary();
            Console.WriteLine("1. compare to null");
            iCountTestcases++;
            if (sd == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, collection is null after default ctor");
            } 
            Console.WriteLine("2. check Count");
            iCountTestcases++;
            if (sd.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Count = {0} after default ctor", sd.Count);
            }
            Console.WriteLine("3. check ContainsValue()");
            iCountTestcases++;
            if (sd.ContainsValue("string")) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, ContainsValue() returned true after default ctor");
            }
            Console.WriteLine("4. check ContainsKey()");
            iCountTestcases++;
            if (sd.ContainsKey("string")) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ContainsKey() returned true after default ctor");
            }
            Console.WriteLine("5. check ToString()");
            iCountTestcases++;
            string temp = sd.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("StringDictionary") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005, ToString() doesn't contain \"StringDictionary\"");
            }
            Console.WriteLine("6. check returned Type");
            iCountTestcases++;
            temp = sd.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("StringDictionary") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: returned type doesn't contain \"StringDictionary\"");
            }
            Console.WriteLine("7. compare returned Type of two Dictionaries");
            iCountTestcases++;
            string temp1 = (new StringDictionary()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007: returned types of two collections differ");
            }
            Console.WriteLine("8. check IsSynchronized");
            iCountTestcases++;
            Console.WriteLine(" IsSynchronized: " + sd.IsSynchronized);
            if (sd.IsSynchronized) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0008: IsSynchronized returned {0}", sd.IsSynchronized);
            }
            Console.WriteLine("9. add item and verify");
            iCountTestcases++;
            sd.Add("key", "value");
            iCountTestcases++;
            if (sd.Count != 1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009a: Count returned {0}", sd.Count);
            }
            iCountTestcases++;
            if ( !sd.ContainsKey("key") ) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009b: ContainsKey() returned false");
            } 
            iCountTestcases++;
            if ( !sd.ContainsValue("value") ) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0009c: ContainsValue() returned false");
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
        Co8754ctor cbA = new Co8754ctor();
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
