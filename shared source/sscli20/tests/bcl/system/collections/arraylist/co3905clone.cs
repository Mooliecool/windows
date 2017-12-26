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
public class Co3905Clone
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.Clone()";
    public static String s_strTFName        = "Co3905Clone.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        ArrayList alst1;
        ArrayList alst2;
        String strValue;
        Object oValue;
        try 
        {
            do
            {
                strLoc = "Loc_8345vdfv";
                alst1 = new ArrayList();
                for(int i=0; i<10; i++)
                {
                    strValue = "String_" + i;
                    alst1.Add(strValue);
                }
                alst2 = (ArrayList)alst1.Clone();
                iCountTestcases++;
                for(int i=0; i<10; i++)
                {
                    strValue = "String_" + i;
                    if(!strValue.Equals((String)alst2[i])) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_561dvs_" + i + "! Expected value not returned, " + strValue);
                    }
                }
                alst1.RemoveAt(9);
                try 
                {
                    iCountTestcases++;
                    oValue = alst1[9];
                    iCountErrors++;
                    Console.WriteLine("Err_034cd! exception not thrown");
                }
                catch(ArgumentOutOfRangeException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_03472fd! Unexpected exception, " + ex.ToString());
                }
                strValue = "String_" + 9;
                iCountTestcases++;
                if(!strValue.Equals((String)alst2[9])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_563vdf! Expected value not returned, <<" + alst1[9] + ">>");
                }
                strLoc = "Loc_625fd";
                alst1 = new ArrayList(1000);
                alst2 = (ArrayList)alst1.Clone();
                iCountTestcases++;
                if(alst1.Capacity == alst2.Capacity) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5234vsfd! Expected value not returned, <<" + alst1.Capacity + ">> <<"+ alst2.Capacity + ">>");
                }
                iCountTestcases++;
                if(alst1.Count != alst2.Count) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_453gfd! Expected value not returned, <<" + alst1.Count + ">> <<"+ alst2.Count + ">>");
                }
                iCountTestcases++;
                if(alst1.IsReadOnly != alst2.IsReadOnly) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5234vsfd! Expected value not returned, <<" + alst1.IsReadOnly + ">> <<"+ alst2.IsReadOnly + ">>");
                }
                iCountTestcases++;
                if(alst1.IsSynchronized != alst2.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_5234vsfd! Expected value not returned, <<" + alst1.IsSynchronized + ">> <<"+ alst2.IsSynchronized + ">>");
                }
                strLoc = "Loc_8345vdfv";
                alst1 = new ArrayList();
                for(int i=0; i<10; i++)
                {
                    alst1.Add(new Foo());
                }
                alst2 = (ArrayList)alst1.Clone();
                iCountTestcases++;
                for(int i=0; i<10; i++)
                {
                    strValue = "Hello World";
                    if(!strValue.Equals(((Foo)alst2[i]).strValue)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_5623rvsdf_" + i + "! Expected value not returned, " + strValue);
                    }
                }
                strValue = "Good Bye";
                ((Foo)alst1[0]).strValue = strValue;
                iCountTestcases++;
                if(!strValue.Equals(((Foo)alst1[0]).strValue)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_7453bvdf! Expected value not returned, " + strValue);
                }
                iCountTestcases++;
                if(!strValue.Equals(((Foo)alst2[0]).strValue)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_7453bvdf! Expected value not returned, " + strValue);
                }
                alst2[0] = new Foo();
                strValue = "Good Bye";
                iCountTestcases++;
                if(!strValue.Equals(((Foo)alst1[0]).strValue)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_525vdf! Expected value not returned, " + strValue);
                }
                strValue = "Hello World";
                iCountTestcases++;
                if(!strValue.Equals(((Foo)alst2[0]).strValue)) 
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
        Co3905Clone cbA = new Co3905Clone();
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
