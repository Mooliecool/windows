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
internal class Co8612_Comparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2);
    }
}
internal class Co8612_CaseInsensitiveComparer:IComparer 
{
    public Int32 Compare(Object obj1, Object obj2)
    {
        if(!(obj1 is String) || !(obj2 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return String.Compare((String)obj1, (String)obj2, true);
    }
}
internal class Co8612_BasicHashCodeProvider:IHashCodeProvider 
{
    public Int32 GetHashCode(Object obj1)
    {
        return obj1.GetHashCode();
    }
}
internal class Co8612_Hash:IHashCodeProvider 
{
    public Int32 GetHashCode(Object obj1)
    {
        if(!(obj1 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return ((String)obj1).GetHashCode();
    }
}
internal class Co8612_CaseInsensitiveHash:IHashCodeProvider 
{
    private static Int32 iVal = 0;
    public Int32 GetHashCode(Object obj1)
    {
        if(!(obj1 is String))
            throw new Exception("Err_539gf! object needs to be String");
        return iVal++;
    }
}
public class Co8612ctor_ISII
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Hashtable.ctor(IDictionary, IHashCodeProvider, IComparer)";
    public static String s_strTFName        = "Co8612ctor_ISII.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        Hashtable hsh1;
        Hashtable hshIni;
        IDictionary idic;
        String[] strKeys = new String[]{"key", "keY", "kEy", "kEY", "Key", "KeY", "KEy", "KEY"};
        try 
        {
            idic = null;
            try 
            {
                iCountTestcases++;
                hsh1 = new Hashtable(idic, 0.72f, new CaseInsensitiveHashCodeProvider(), new CaseInsensitiveComparer());
                iCountErrors++;
                Console.WriteLine("Err_8452vs! exception not thrown");
            }
            catch(ArgumentNullException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
            }
            iCountTestcases++;
            hshIni = new Hashtable();
            for(int i=0; i<100; i++)
                hshIni.Add(i, i);				
            idic = hshIni;
            hsh1 = new Hashtable(idic, 0.72f, new Co8612_BasicHashCodeProvider(), Comparer.Default);
            if(hsh1.Count != 100)
            {
                iCountErrors++;
                Console.WriteLine("Err_54243fs! Unexpeceted value returned");
            }
            for(int i=0; i<hsh1.Count; i++)
            {
                if(!hsh1.ContainsKey(i))
                {
                    iCountErrors++;
                    Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                }
                if((int)hsh1[i] != i)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_527345svf! Unexpeceted value returned");
                }
            }
            iCountTestcases++;
            hshIni = new Hashtable();
            for(int i=0; i<strKeys.Length; i++)
            {
                try 
                {
                    hshIni.Add(strKeys[i], null);
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
            }
            hsh1 = new Hashtable(hshIni, 0.72f, new CaseInsensitiveHashCodeProvider(), null);
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
            hshIni = new Hashtable();
            for(int i=0; i<strKeys.Length; i++)
            {
                try 
                {
                    hshIni.Add(strKeys[i], null);
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
            }
            hsh1 = new Hashtable(hshIni, 0.72f, null, new CaseInsensitiveComparer());
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
            hshIni = new Hashtable();
            for(int i=0; i<strKeys.Length; i++)
            {
                try 
                {
                    hshIni.Add(strKeys[i], null);
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
            }
            hsh1 = null;
            try 
            {
                hsh1 = new Hashtable(hshIni, 0.72f, new CaseInsensitiveHashCodeProvider(), new CaseInsensitiveComparer());
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
            if(hsh1 != null)
            {
                iCountErrors++;
                Console.WriteLine("Err_54243fs! Unexpeceted value returned");
            }
            iCountTestcases++;
            hshIni = new Hashtable();
            for(int i=0; i<strKeys.Length; i++)
            {
                try 
                {
                    hshIni.Add(strKeys[i], null);
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
            }
            hsh1 = new Hashtable(hshIni, 0.72f, new CaseInsensitiveHashCodeProvider(), new Co8612_Comparer());
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
            hshIni = new Hashtable();
            for(int i=0; i<strKeys.Length; i++)
            {
                try 
                {
                    hshIni.Add(strKeys[i], null);
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
            }
            hsh1 = null;
            try 
            {
                hsh1 = new Hashtable(hshIni, 0.72f, new CaseInsensitiveHashCodeProvider(), new Co8612_CaseInsensitiveComparer());
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
            if(hsh1 != null)
            {
                iCountErrors++;
                Console.WriteLine("Err_54243fs! Unexpeceted value returned");
            }
            iCountTestcases++;
            hshIni = new Hashtable();
            for(int i=0; i<strKeys.Length; i++)
            {
                try 
                {
                    hshIni.Add(strKeys[i], null);
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
            }
            hsh1 = new Hashtable(hshIni, 0.72f, new Co8612_Hash(), new CaseInsensitiveComparer());
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
            hshIni = new Hashtable();
            for(int i=0; i<strKeys.Length; i++)
            {
                try 
                {
                    hshIni.Add(strKeys[i], null);
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_13753vdf! Unexpected exception, " + ex.ToString());
                }
            }
            hsh1 = null;
            try 
            {
                hsh1 = new Hashtable(hshIni, 0.72f, new CaseInsensitiveHashCodeProvider(), new CaseInsensitiveComparer());
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
            if(hsh1 != null)
            {
                iCountErrors++;
                Console.WriteLine("Err_54243fs! Unexpeceted value returned");
            }
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
        Co8612ctor_ISII cbA = new Co8612ctor_ISII();
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
