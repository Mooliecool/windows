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
public class Co3952GetHashCode_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "CaseInsensitiveHashCodeProvider.GetHashCode(Object)";
    public static String s_strTFName        = "Co3952GetHashCode_obj.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        CaseInsensitiveHashCodeProvider cis;            
        String strValue1;
        String strValue2;
        Int32 iValue;
        try 
        {
            do
            {
                cis = new CaseInsensitiveHashCodeProvider();
                iCountTestcases++;
                if(cis==null) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_8753bdd! Expected value not returned, ");
                }
                strValue1 = "hello world";
                strValue2 = strValue1.ToUpper();
                iCountTestcases++;
                if(cis.GetHashCode(strValue1)!=cis.GetHashCode(strValue2)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_8753bdd! Expected value not returned, " + cis.GetHashCode(strValue1) + " " + cis.GetHashCode(strValue2));
                }
                strValue2 = "Completely different";
                iCountTestcases++;
                if(cis.GetHashCode(strValue1)==cis.GetHashCode(strValue2)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_87453gdg! Expected value not returned, " + cis.GetHashCode(strValue1) + " " + cis.GetHashCode(strValue2));
                }
                iValue = 5;
                iCountTestcases++;
                if(cis.GetHashCode(iValue)!=iValue.GetHashCode()) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, " + cis.GetHashCode(iValue) + " " + iValue.GetHashCode());
                }
                try
                {
                    iCountTestcases++;
                    cis.GetHashCode(null);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_6572fdg! Unexpected exception thrown, " + ex);
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
        Co3952GetHashCode_obj cbA = new Co3952GetHashCode_obj();
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
