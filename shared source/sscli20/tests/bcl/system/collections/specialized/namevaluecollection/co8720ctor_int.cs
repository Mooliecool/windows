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
public class Co8720ctor_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.ctor(int)";
    public static String s_strTFName        = "Co8720ctor_int.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        NameValueCollection nvc; 
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
            Console.WriteLine("1. Create with capacity 10");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection(10);
            Console.WriteLine("1.1. check Count");
            iCountTestcases++;
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.1, Count = {0} for empty with capacity 10", nvc.Count);
            }
            Console.WriteLine("1.3. add 10 elements");
            int len = values.Length;
            for (int i = 0; i < 10; i++ ) 
            {
                nvc.Add( names[i], values[i]);
            }
            iCountTestcases++;
            if (nvc.Count != 10) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.3, Count = {0} instead of 10", nvc.Count);
            }
            Console.WriteLine("1.5. add more elements");
            for (int i = 0; i < 5; i++ ) 
            {
                nvc.Add("key"+i, "item"+i);
            }
            iCountTestcases++;
            if (nvc.Count != 15) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.5, Count = {0} instead of 15", nvc.Count);
            }
            Console.WriteLine("2. Create with capacity 100");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection(100);
            Console.WriteLine("2.1. check Count");
            iCountTestcases++;
            if (nvc.Count != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.1, Count = {0} for empty with capacity 100", nvc.Count);
            }
            Console.WriteLine("2.3. add 100 elements");
            for (int i = 0; i < 100; i++ ) 
            {
                nvc.Add( names[i/10]+i, values[i/10]+i);
            }
            iCountTestcases++;
            if (nvc.Count != 100) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.3, Count = {0} instead of 100", nvc.Count);
            }
            Console.WriteLine("2.5. add more elements");
            for (int i = 0; i < 70; i++ ) 
            {
                nvc.Add("key"+i, "item"+i);
            }
            iCountTestcases++;
            if (nvc.Count != 170) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.5, Count = {0} instead of 170", nvc.Count);
            }
            Console.WriteLine("3. invalid parameter");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            try 
            {
                nvc = new NameValueCollection(-1);
                iCountErrors++;
                Console.WriteLine("Err_0003a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("     expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, unexpected exception: {0}", e.ToString());
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
        Co8720ctor_int cbA = new Co8720ctor_int();
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
