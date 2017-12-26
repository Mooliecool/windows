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
public class Co3911ReadOnly_IL
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.Readonly(IList)";
    public static String s_strTFName        = "Co3911ReadOnly_IL.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        ArrayList alst1;
        IList ilst1;
        ArrayList olst1;
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
                ilst1 = ArrayList.ReadOnly((IList)alst1);
                iCountTestcases++;
                for(int i=0; i<10; i++)
                {
                    strValue = "String_" + i;
                    if(!strValue.Equals((String)ilst1[i])) 
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
                try 
                {
                    iCountTestcases++;
                    oValue = ilst1[9];
                    iCountErrors++;
                    Console.WriteLine("Err_8452vs! exception not thrown");
                }
                catch(ArgumentOutOfRangeException)
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
                    ilst1.RemoveAt(0);
                    iCountErrors++;
                    Console.WriteLine("Err_8342sf! exception not thrown");
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_135234vdf! Unexpected exception, " + ex.ToString());
                }
                try 
                {
                    iCountTestcases++;
                    ilst1.Remove("String_1");
                    iCountErrors++;
                    Console.WriteLine("Err_8342sf! exception not thrown");
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_135234vdf! Unexpected exception, " + ex.ToString());
                }
                try 
                {
                    iCountTestcases++;
                    ilst1.Clear();
                    iCountErrors++;
                    Console.WriteLine("Err_8342sf! exception not thrown");
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_135234vdf! Unexpected exception, " + ex.ToString());
                }
                try 
                {
                    iCountTestcases++;
                    ilst1.Add("This sort of thing will not be allowed");
                    iCountErrors++;
                    Console.WriteLine("Err_8342sf! exception not thrown");
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_135234vdf! Unexpected exception, " + ex.ToString());
                }
                try 
                {
                    iCountTestcases++;
                    ilst1.Insert(0, "This sort of thing will not be allowed");
                    iCountErrors++;
                    Console.WriteLine("Err_8342sf! exception not thrown");
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_135234vdf! Unexpected exception, " + ex.ToString());
                }
                try 
                {
                    iCountTestcases++;
                    strValue = "Hello World";
                    ilst1[0] = strValue;
                    iCountErrors++;
                    Console.WriteLine("Err_8342sf! exception not thrown");
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_135234vdf! Unexpected exception, " + ex.ToString());
                }
                strLoc = "Loc_75623fdg";
                olst1 = new ArrayList();
                for(int i=0; i<10; i++)
                {
                    strValue = "String_" + i;
                    olst1.Add(strValue);
                }
                ilst1 = ArrayList.ReadOnly((IList)olst1);
                iCountTestcases++;
                for(int i=0; i<10; i++)
                {
                    strValue = "String_" + i;
                    if(!strValue.Equals((String)ilst1[i])) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_561dvs_" + i + "! Expected value not returned, " + strValue);
                    }
                }
                iCountTestcases++;
                for(int i=0; i<10; i++)
                {
                    strValue = "String_" + i;
                    if(!ilst1.Contains(strValue)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_542gds_" + i + "! Expected value not returned, " + strValue);
                    }
                    if(ilst1.IndexOf(strValue) != i) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_452dv0_" + i + "! Expected value not returned, " + strValue);
                    }
                }
                iCountTestcases++;
                if(alst1.IsReadOnly) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_65323gdfgb! Expected value not returned, " + alst1.IsReadOnly);
                }
                iCountTestcases++;
                if(olst1.IsReadOnly) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_65323gdfgb! Expected value not returned, " + olst1.IsReadOnly);
                }
                iCountTestcases++;
                if(!ilst1.IsReadOnly) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_65323gdfgb! Expected value not returned, " + ilst1.IsReadOnly);
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
        Co3911ReadOnly_IL cbA = new Co3911ReadOnly_IL();
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
