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
internal class Co8783_SensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, false);
    }
}
internal class Co8783_CaseInsensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, true);
    }
}
public class Co8783ctor_IComp
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ListDictionary.ctor(IComparer)";
    public static String s_strTFName        = "Co8783ctor_IComp.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        ListDictionary ld; 
        string [] values = 
        {
            "item",
            "Item",
            "iTem"
        };
        string [] names = 
        {
            "key",
            "Key",
            "kEy"
        };
        try
        {
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            Console.WriteLine("1. ctor(case_insensitive_comparer)");
            ld = new ListDictionary(new Co8783_CaseInsensitiveComparer());
            iCountTestcases++;
            int len = values.Length;
            ld.Add(names[0], values[0]);
            if (ld.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.1a, Count is {0} instead of {1}", ld.Count, 1);
            } 
            try 
            {
                ld.Add(names[1], values[1]);
                iCountErrors++;
                Console.WriteLine("Err_0001.2a, no exception");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine(" Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.2b, unexpected exception: {0}", e.ToString());
            }
            iCountTestcases++;
            if (String.Compare(ld[names[0]].ToString(), values[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.3, returned {0} instead of {1}", ld[names[0]], values[0]);
            }
            iCountTestcases++;
            if (String.Compare(ld[names[0].ToUpper()].ToString(), values[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.4, failed for uppercase key");
            }
            Console.WriteLine("2. ctor(case_sensitive_comparer)");
            ld = new ListDictionary(new Co8783_SensitiveComparer());
            iCountTestcases++;
            for (int i = 0; i < len; i ++) 
            {
                ld.Add(names[i], values[i]);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.1a, Count is {0} instead of {1}", ld.Count, len);
            } 
            for (int i = 0; i < len; i ++) 
            {
                iCountTestcases++;
                if (String.Compare(ld[names[i]].ToString(), values[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002.3_{2}, returned {0} instead of {1}", ld[names[i]], values[i], i);
                }
            }
            iCountTestcases++;
            if (ld[names[0].ToUpper()] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.4, returned non-null for non-existing uppercase key");
            }
            Console.WriteLine("3. null parameter");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            try 
            {
                ld = new ListDictionary(null);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, unexpected exception: {0}", e.ToString());
            }
            iCountTestcases++;
            for (int i = 0; i < len; i ++) 
            {
                ld.Add(names[i], values[i]);
            }
            if (ld.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003.1a, Count is {0} instead of {1}", ld.Count, len);
            } 
            for (int i = 0; i < len; i ++) 
            {
                iCountTestcases++;
                if (String.Compare(ld[names[i]].ToString(), values[i], false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003.2_{2}, returned {0} instead of {1}", ld[names[i]], values[i], i);
                }
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
        Co8783ctor_IComp cbA = new Co8783ctor_IComp();
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
