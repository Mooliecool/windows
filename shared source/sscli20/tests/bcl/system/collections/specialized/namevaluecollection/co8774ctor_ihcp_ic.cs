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
internal class Co8774_SensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, false);
    }
}
internal class Co8774_CaseInsensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, true);
    }
}
internal class Co8774_SensitiveHashCodeProvider:IHashCodeProvider 
{
    public Int32 GetHashCode(Object obj1)
    {
        return obj1.GetHashCode();
    }
}
internal class Co8774_InsensitiveHashCodeProvider:IHashCodeProvider 
{
    public Int32 GetHashCode(Object obj1)
    {
        if(!(obj1 is String))
            throw new Exception("Err_InsProvider! object needs to be String");
        return (obj1.ToString().ToLower()).GetHashCode();
    }
}
public class Co8774ctor_IHCP_IC
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.ctor(IHashCodeProvider, IComparer)";
    public static String s_strTFName        = "Co8774ctor_IHCP_IC.cs";
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
            "item",
            "Item",
            "iTem"
        };
        string exp = "";
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
            for (int i = 0; i < values.Length; i++) 
            {
                if (i == 0)
                    exp += values[i];
                else
                    exp += "," + values[i];
            }
            Console.WriteLine("1. ctor(sensitive_IHashCodeProvider, case_insensitive_comparer)");
            nvc = new NameValueCollection(new Co8774_SensitiveHashCodeProvider(), new Co8774_CaseInsensitiveComparer());
            iCountTestcases++;
            int len = values.Length;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(names[i], values[i]);
            }
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.1a, Count is {0} instead of {1}", nvc.Count, len);
            } 
            iCountTestcases++;
            if (String.Compare(nvc[names[0]], values[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.1b, returned {0} instead of {1}", nvc[names[0]], values[0]);
            }
            iCountTestcases++;
            if (nvc["KEY"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001.1c, returned {0} instead of null", nvc.Get("KEY"));
            }
            Console.WriteLine("2. ctor(sensitive_IHashCodeProvider, case_sensitive_comparer)");
            nvc = new NameValueCollection(new Co8774_SensitiveHashCodeProvider(), new Co8774_SensitiveComparer());
            iCountTestcases++;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(names[i], values[i]);
            }
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.1a, Count is {0} instead of {1}", nvc.Count, len);
            } 
            iCountTestcases++;
            if (String.Compare(nvc[names[0]], values[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.1b, returned {0} instead of {1}", nvc[names[0]], values[0]);
            }
            iCountTestcases++;
            if (nvc["KEY"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002.1c, returned {0} instead of null", nvc.Get("KEY"));
            }
            Console.WriteLine("3. ctor(insensitive_IHashCodeProvider, case_insensitive_comparer)");
            nvc = new NameValueCollection(new Co8774_InsensitiveHashCodeProvider(), new Co8774_CaseInsensitiveComparer());
            iCountTestcases++;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(names[i], values[i]);
            }
            if (nvc.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003.1a, Count is {0} instead of {1}", nvc.Count, 1);
            } 
            iCountTestcases++;
            if (String.Compare(nvc[names[0]], exp, false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003.1b, returned {0} instead of {1}", nvc[names[0]], exp);
            }
            iCountTestcases++;
            if (nvc["KEY"] == null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003.1c, returned null instead of {0} ", exp);
            }
            else 
            {
                if (String.Compare(nvc["KEY"], exp, false) != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003.1d, returned {0} instead of {1}", nvc["KEY"], exp);
                }
            }
            Console.WriteLine("4. ctor(insensitive_IHashCodeProvider, case_sensitive_comparer)");
            nvc = new NameValueCollection(new Co8774_InsensitiveHashCodeProvider(), new Co8774_SensitiveComparer());
            iCountTestcases++;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(names[i], values[i]);
            }
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004.1a, Count is {0} instead of {1}", nvc.Count, len);
            } 
            iCountTestcases++;
            if (String.Compare(nvc[names[0]], values[0], false) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004.1b, returned {0} instead of {1}", nvc[names[0]], values[0]);
            }
            iCountTestcases++;
            if (nvc["KEY"] != null) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004.1c, returned {0} instead of null", nvc.Get("KEY"));
            }
            Console.WriteLine("5. null parameter");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            try 
            {
                nvc = new NameValueCollection(null, null);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, unexpected exception: {0}", e.ToString());
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
        Co8774ctor_IHCP_IC cbA = new Co8774ctor_IHCP_IC();
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
