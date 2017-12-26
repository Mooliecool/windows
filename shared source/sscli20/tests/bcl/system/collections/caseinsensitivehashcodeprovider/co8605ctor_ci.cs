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
public class Co8605ctor_ci
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "CaseInsensitiveHashCodeProvider.ctor(CultureInfo)";
    public static String s_strTFName        = "Co8605ctor_ci.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        CaseInsensitiveHashCodeProvider cis;
        CultureInfo[] cultures;
        try 
        {
            do
            {
                strLoc = "Loc_384sdg";
                cultures = CultureInfo.GetCultures(CultureTypes.AllCultures);
                for(int i=0; i<cultures.Length; i++)
                {
                    iCountTestcases++;
                    cis = new CaseInsensitiveHashCodeProvider(cultures[i]);
                    if(cis.GetHashCode("hello") != cis.GetHashCode("HELLO"))
                    {
                        iCountErrors++;
                        Console.WriteLine( "Err_93745sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", cis.GetHashCode("hello"), cis.GetHashCode("HELLO"));
                    }
                    if(cis.GetHashCode("hello") != cis.GetHashCode("hello"))
                    {
                        iCountErrors++;
                        Console.WriteLine( "Err_4953rewg! wrong value returned. Expected - <{0}>, Returned - <{1}>", cis.GetHashCode("hello"), cis.GetHashCode("HELLO"));
                    }
                    if(cis.GetHashCode("hello") == cis.GetHashCode("mello"))
                    {
                        iCountErrors++;
                        Console.WriteLine( "Err_94375sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", cis.GetHashCode("hello"), cis.GetHashCode("HELLO"));
                    }
                    if(cis.GetHashCode(5) != cis.GetHashCode(5))
                    {
                        iCountErrors++;
                        Console.WriteLine( "Err_9745sdg! wrong value returned");
                    }
                    if(cis.GetHashCode(5) == cis.GetHashCode(10))
                    {
                        iCountErrors++;
                        Console.WriteLine( "Err_9745sdg! wrong value returned");
                    }    		
                }
                strLoc = "Loc_dfyusg";
                iCountTestcases++;
                try
                {
                    cis = new CaseInsensitiveHashCodeProvider(null);
                    iCountErrors++;
                    Console.WriteLine( "Err_9745sdg! Exception not thrown");
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_9745sdg! Unexpected exception thrown, " + ex.GetType().Name);
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
        Co8605ctor_ci cbA = new Co8605ctor_ci();
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
