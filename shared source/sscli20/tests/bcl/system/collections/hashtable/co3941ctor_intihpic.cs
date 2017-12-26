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
using System;
using System.IO;
using System.Collections;
using System.Globalization;
internal class Co3941_Comparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2);
    }
}
internal class Co3941_CaseInsensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, true);
    }
}
internal class Co3941_Hash:IHashCodeProvider 
{
    public Int32 GetHashCode(Object obj1)
    {
        if(!(obj1 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return ((String)obj1).GetHashCode();
    }
}
internal class Co3941_CaseInsensitiveHash:IHashCodeProvider 
{
    private static Int32 iVal = 0;
    public Int32 GetHashCode(Object obj1)
    {
        if(!(obj1 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return iVal++;
    }
}
public class Co3941ctor_IntIHPIC
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Hashtable.ctor(Int32, IHashCodeProvider, IComparer)";
    public static String s_strTFName        = "Co3941ctor_IntIHPIC.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        Hashtable hsh1;
        String[] strKeys = new String[]{"key", "keY", "kEy", "kEY", "Key", "KeY", "KEy", "KEY"};
        try 
        {
            do
            {
                try 
                {
                    iCountTestcases++;
                    hsh1 = new Hashtable(-1, new CaseInsensitiveHashCodeProvider(), new CaseInsensitiveComparer());
                    iCountErrors++;
                    Console.WriteLine("Err_8452vs! exception not thrown");
                }
                catch(ArgumentException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
                try 
                {
                    iCountTestcases++;
                    hsh1 = new Hashtable(Int32.MaxValue, new CaseInsensitiveHashCodeProvider(), new CaseInsensitiveComparer());
                    iCountErrors++;
                    Console.WriteLine("Err_8452vs! exception not thrown");
                }
                catch(ArgumentException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
                iCountTestcases++;
                hsh1 = new Hashtable(32, new CaseInsensitiveHashCodeProvider(), null);
                for(int i=0; i<strKeys.Length; i++)
                {
                    try 
                    {
                        hsh1.Add(strKeys[i], null);
                    }
                    catch(Exception ex)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                    }
                }
                if(hsh1.Count != strKeys.Length)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54243fs! Unexpeceted value returned");
                }
                for(int i=0; i<hsh1.Count; i++)
                {
                    if(!hsh1.ContainsKey(strKeys[i]))
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                    }
                }
                iCountTestcases++;
                hsh1 = new Hashtable(32, null, new CaseInsensitiveComparer());
                for(int i=0; i<strKeys.Length; i++)
                {
                    try 
                    {
                        hsh1.Add(strKeys[i], null);
                    }
                    catch(Exception ex)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                    }
                }
                if(hsh1.Count != strKeys.Length)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54243fs! Unexpeceted value returned");
                }
                for(int i=0; i<hsh1.Count; i++)
                {
                    if(!hsh1.ContainsKey(strKeys[i]))
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                    }
                }
                iCountTestcases++;
                hsh1 = new Hashtable(0, new CaseInsensitiveHashCodeProvider(), new CaseInsensitiveComparer());
                hsh1.Add(strKeys[0], null);
                for(int i=1; i<strKeys.Length; i++)
                {
                    try 
                    {
                        hsh1.Add(strKeys[i], null);
                        iCountErrors++;
                        Console.WriteLine("Err_534fsdf! Did not throw an exception");
                    }
                    catch(ArgumentException)
                    {
                    }
                    catch(Exception ex)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                    }
                }
                if(hsh1.Count != 1)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54243fs! Unexpeceted value returned");
                }
                if(!hsh1.ContainsKey(strKeys[0]))
                {
                    iCountErrors++;
                    Console.WriteLine("Err_43f4fsd! Unexpeceted value returned");
                }
                for(int i=1; i<hsh1.Count; i++)
                {
                    if(hsh1.ContainsKey(strKeys[i]))
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                    }
                }
                iCountTestcases++;
                hsh1 = new Hashtable(32, new CaseInsensitiveHashCodeProvider(), new Co3941_Comparer());
                for(int i=0; i<strKeys.Length; i++)
                {
                    try 
                    {
                        hsh1.Add(strKeys[i], null);
                    }
                    catch(Exception ex)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                    }
                }
                if(hsh1.Count != strKeys.Length)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54243fs! Unexpeceted value returned");
                }
                for(int i=0; i<hsh1.Count; i++)
                {
                    if(!hsh1.ContainsKey(strKeys[i]))
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                    }
                }
                iCountTestcases++;
                hsh1 = new Hashtable(32, new CaseInsensitiveHashCodeProvider(), new Co3941_CaseInsensitiveComparer());
                hsh1.Add(strKeys[0], null);
                for(int i=1; i<strKeys.Length; i++)
                {
                    try 
                    {
                        hsh1.Add(strKeys[i], null);
                        iCountErrors++;
                        Console.WriteLine("Err_534fsdf! Did not throw an exception");
                    }
                    catch(ArgumentException)
                    {
                    }
                    catch(Exception ex)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                    }
                }
                if(hsh1.Count != 1)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54243fs! Unexpeceted value returned");
                }
                if(!hsh1.ContainsKey(strKeys[0]))
                {
                    iCountErrors++;
                    Console.WriteLine("Err_43f4fsd! Unexpeceted value returned");
                }
                for(int i=1; i<hsh1.Count; i++)
                {
                    if(hsh1.ContainsKey(strKeys[i]))
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                    }
                }
                iCountTestcases++;
                hsh1 = new Hashtable(32, new Co3941_Hash(), new CaseInsensitiveComparer());
                for(int i=0; i<strKeys.Length; i++)
                {
                    try 
                    {
                        hsh1.Add(strKeys[i], null);
                    }
                    catch(Exception ex)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                    }
                }
                if(hsh1.Count != strKeys.Length)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54243fs! Unexpeceted value returned");
                }
                for(int i=0; i<hsh1.Count; i++)
                {
                    if(!hsh1.ContainsKey(strKeys[i]))
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                    }
                }
                iCountTestcases++;
                hsh1 = new Hashtable(32, new CaseInsensitiveHashCodeProvider(), new CaseInsensitiveComparer());
                hsh1.Add(strKeys[0], null);
                for(int i=1; i<strKeys.Length; i++)
                {
                    try 
                    {
                        hsh1.Add(strKeys[i], null);
                        iCountErrors++;
                        Console.WriteLine("Err_534fsdf! Did not throw an exception");
                    }
                    catch(ArgumentException)
                    {
                    }
                    catch(Exception ex)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                    }
                }
                if(hsh1.Count != 1)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54243fs! Unexpeceted value returned");
                }
                if(!hsh1.ContainsKey(strKeys[0]))
                {
                    iCountErrors++;
                    Console.WriteLine("Err_43f4fsd! Unexpeceted value returned");
                }
                for(int i=1; i<hsh1.Count; i++)
                {
                    if(hsh1.ContainsKey(strKeys[i]))
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                    }
                }
            } while (false);
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
            return true;
        }
        else
        {
            Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co3941ctor_IntIHPIC cbA = new Co3941ctor_IntIHPIC();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
    }
}
