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
public class Co3954Clone
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "SortedList.Clone()";
    public static String s_strTFName        = "Co3954Clone.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        SortedList hsh1;
        SortedList hsh2;
        String strKey;
        String strValue;
        try 
        {
            do
            {
                strLoc = "Loc_8345vdfv";
                hsh1 = new SortedList();
                for(int i=0; i<10; i++)
                {
                    strKey = "Key_" + i;
                    strValue = "String_" + i;
                    hsh1.Add(strKey, strValue);
                }
                hsh2 = (SortedList)hsh1.Clone();
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
                hsh1 = new SortedList(1000);
                hsh2 = (SortedList)hsh1.Clone();
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
                hsh1 = new SortedList();
                for(int i=0; i<10; i++)
                {
                    hsh1.Add(i, new Foo());
                }
                hsh2 = (SortedList)hsh1.Clone();
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
        Co3954Clone cbA = new Co3954Clone();
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
