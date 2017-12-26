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
public class Co8737ctor
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.ctor";
    public static String s_strTFName        = "Co8737ctor.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        StringCollection sc; 
        try
        {
            Console.WriteLine("--- default ctor ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            Console.WriteLine("1. compare to null");
            iCountTestcases++;
            if (sc == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, collection is null after default ctor");
            } 
            Console.WriteLine("2. check Count");
            iCountTestcases++;
            if (sc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Count = {0} after default ctor", sc.Count);
            }
            Console.WriteLine("3. check Contains()");
            iCountTestcases++;
            if (sc.Contains("string")) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, Contains() returned true after default ctor");
            }
            Console.WriteLine("4. check ToString()");
            iCountTestcases++;
            string temp = sc.ToString();
            Console.WriteLine(" ToString(): " + temp);
            if (temp.IndexOf("StringCollection") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ToString() doesn't contain \"StringCollection\"");
            }
            Console.WriteLine("5. check returned Type");
            iCountTestcases++;
            temp = sc.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + temp);
            if (temp.IndexOf("StringCollection") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0005: returned type doesn't contain \"StringCollection\"");
            }
            Console.WriteLine("6. compare returned Type of two collection");
            iCountTestcases++;
            string temp1 = (new StringCollection()).GetType().ToString().Trim();
            if (String.Compare(temp, temp1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: returned types of two collections differ");
            }
            Console.WriteLine("7. check IsReadOnly");
            iCountTestcases++;
            Console.WriteLine(" IsReadOnly: " + sc.IsReadOnly);
            if (sc.IsReadOnly) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0007: IsReadOnly returned {0}", sc.IsReadOnly);
            }
            Console.WriteLine("8. check IsSynchronized");
            iCountTestcases++;
            Console.WriteLine(" IsSynchronized: " + sc.IsSynchronized);
            if (sc.IsSynchronized) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0008: IsSynchronized returned {0}", sc.IsSynchronized);
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
        Co8737ctor cbA = new Co8737ctor();
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
