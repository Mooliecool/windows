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
public class Co3942Clone
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Hashtable.Clone()";
    public static String s_strTFName        = "Co3942Clone.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        Hashtable hsh1;
        Hashtable hsh2;
        Hashtable hsh3;
        String strKey;
        String strValue;
        ICollection icol1;
        IDictionary idic1;
        ICloneable iclone1;
        try 
        {
            do
            {
                hsh1 = new Hashtable();
                hsh2 = (Hashtable)hsh1.Clone();
                if(hsh2.Count != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_453gfd! Expected value not returned, <<" + hsh1.Count + ">> <<"+ hsh2.Count + ">>");
                }
                iCountTestcases++;
                if(hsh2.IsReadOnly) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5234vsfd! Expected value not returned, <<" + hsh1.IsReadOnly + ">> <<"+ hsh2.IsReadOnly + ">>");
                }
                iCountTestcases++;
                if(hsh2.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5234vsfd! Expected value not returned, <<" + hsh1.IsSynchronized + ">> <<"+ hsh2.IsSynchronized + ">>");
                }
                strLoc = "Loc_8345vdfv";
                hsh1 = new Hashtable();
                for(int i=0; i<10; i++)
                {
                    strKey = "Key_" + i;
                    strValue = "String_" + i;
                    hsh1.Add(strKey, strValue);
                }
                hsh2 = (Hashtable)hsh1.Clone();
                iCountTestcases++;
                for(int i=0; i<10; i++)
                {
                    strValue = "String_" + i;
                    if(!strValue.Equals((String)hsh2["Key_" + i])) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_561dvs_" + i + "! Expected value not returned, " + strValue);
                    }
                }
                hsh1.Remove("Key_9");
                iCountTestcases++;
                if(hsh1["Key_9"]!=null) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_563vdf! Expected value not returned, <<" + hsh1["Key_9"] + ">>");
                }
                strValue = "String_" + 9;
                iCountTestcases++;
                if(!strValue.Equals((String)hsh2["Key_9"])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_563vdf! Expected value not returned, <<" + hsh1[9] + ">>");
                }
                strLoc = "Loc_625fd";
                hsh1 = new Hashtable(1000);
                hsh2 = (Hashtable)hsh1.Clone();
                iCountTestcases++;
                if(hsh1.Count != hsh2.Count) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_453gfd! Expected value not returned, <<" + hsh1.Count + ">> <<"+ hsh2.Count + ">>");
                }
                iCountTestcases++;
                if(hsh1.IsReadOnly != hsh2.IsReadOnly) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5234vsfd! Expected value not returned, <<" + hsh1.IsReadOnly + ">> <<"+ hsh2.IsReadOnly + ">>");
                }
                iCountTestcases++;
                if(hsh1.IsSynchronized != hsh2.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5234vsfd! Expected value not returned, <<" + hsh1.IsSynchronized + ">> <<"+ hsh2.IsSynchronized + ">>");
                }
                strLoc = "Loc_8345vdfv";
                hsh1 = new Hashtable();
                for(int i=0; i<10; i++)
                {
                    hsh1.Add(i, new Foo());
                }
                hsh2 = (Hashtable)hsh1.Clone();
                iCountTestcases++;
                for(int i=0; i<10; i++)
                {
                    strValue = "Hello World";
                    if(!strValue.Equals(((Foo)hsh2[i]).strValue)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_5623rvsdf_" + i + "! Expected value not returned, " + strValue);
                    }
                }
                strValue = "Good Bye";
                ((Foo)hsh1[0]).strValue = strValue;
                iCountTestcases++;
                if(!strValue.Equals(((Foo)hsh1[0]).strValue)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_7453bvdf! Expected value not returned, " + strValue);
                }
                iCountTestcases++;
                if(!strValue.Equals(((Foo)hsh2[0]).strValue)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_7453bvdf! Expected value not returned, " + strValue);
                }
                hsh2[0] = new Foo();
                strValue = "Good Bye";
                iCountTestcases++;
                if(!strValue.Equals(((Foo)hsh1[0]).strValue)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_525vdf! Expected value not returned, " + strValue);
                }
                strValue = "Hello World";
                iCountTestcases++;
                if(!strValue.Equals(((Foo)hsh2[0]).strValue)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_54363fgdf! Expected value not returned, " + strValue);
                }
                hsh1 = new Hashtable();
                for(int i=0; i<10; i++)
                {
                    strKey = "Key_" + i;
                    strValue = "String_" + i;
                    hsh1.Add(strKey, strValue);
                }
                hsh2 = new Hashtable(hsh1);
                hsh2.Remove("Key_0");
                hsh2.Remove("Key_1");
                hsh2.Remove("Key_2");
                if(hsh2.Count != 7)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_45235w! Expected value not returned");
                }
                hsh2["Key_10"] = "String_10";
                hsh2["Key_11"] = "String_11";
                hsh2["Key_12"] = "String_12";
                hsh3 = (Hashtable)hsh2.Clone();
                if(hsh3.Count != 10) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_453gfd! Expected value not returned, <<" + hsh1.Count + ">> <<"+ hsh2.Count + ">>");
                }
                for(int i=3; i<13; i++)
                {
                    if(!hsh3.Contains("Key_" + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_742ds8f! Expected value not returned, " + hsh3.Contains("Key_" + i));
                    }				
                    if(!hsh3.ContainsKey("Key_" + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_742389dsaf! Expected value not returned, " + hsh3.ContainsKey("Key_" + i));
                    }				
                    if(!hsh3.ContainsValue("String_" + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_6243dsf! Expected value not returned, " + hsh3.ContainsValue("Value_" + i));
                    }				
                }
                iCountTestcases++;
                hsh1 = new Hashtable();
                try
                {
                    icol1= (ICollection)hsh1.Clone();
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5734sfs! Unexpected exception thrown, " + ex);
                }
                try
                {
                    idic1= (IDictionary )hsh1.Clone();
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5734sfs! Unexpected exception thrown, " + ex);
                }
                try
                {
                    iclone1= (ICloneable)hsh1.Clone();
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5734sfs! Unexpected exception thrown, " + ex);
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
        Co3942Clone cbA = new Co3942Clone();
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
internal class Foo 
{
    internal String strValue;
    internal Foo()
    {
        strValue = "Hello World";
    }
}
