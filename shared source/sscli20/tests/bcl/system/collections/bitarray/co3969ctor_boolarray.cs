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
public class Co3969ctor_BoolArray
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitArray.ctor(Boolean[])";
    public static String s_strTFName        = "Co3969ctor_BoolArray.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        BitArray bitArr1;
        Boolean[] bolArr1;
        Boolean[] bolArr2;
        Int32 iNumOfElements;
        Random rnd1;
        try 
        {
            do
            {
                iNumOfElements = 10;
                rnd1 = new Random();
                strLoc = "Loc_742dsf!";
                iCountTestcases++;
                bolArr1 = new Boolean[iNumOfElements];
                for(int i=0; i<iNumOfElements; i++)
                {
                    if(rnd1.Next(10)>5)
                        bolArr1[i] = true;
                    else
                        bolArr1[i] = false;
                }
                bitArr1 = new BitArray(bolArr1);
                bolArr2 = new Boolean[iNumOfElements];
                bitArr1.CopyTo(bolArr2, 0);
                for(int i=0; i<iNumOfElements; i++)
                {
                    if(bolArr1[i] != bolArr2[i])
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_753qn_" + i + "! Wrong value returned, " + bolArr1[i] + " " + bolArr1[2]);
                    }
                }
                if(bitArr1.Count != iNumOfElements)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_297453dsgf! Wrong value returned");
                }
                for(int i=0; i<bitArr1.Count; i++)
                {
                    iCountTestcases++;
                    if(bolArr1[i] != bitArr1[i])
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_753qn_" + i + "! Wrong value returned, " + bolArr1[i] + " " + bitArr1[i]);
                    }
                }
                strLoc = "Loc_43987";
                iCountTestcases++;
                try
                {
                    bolArr1 = null;
                    bitArr1 = new BitArray(bolArr1);
                    iCountErrors++;
                    Console.WriteLine("Err_97452dsaf! No exception thrown");					
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_07842af! Unexpected exception thrown, " + ex);					
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
        Co3969ctor_BoolArray cbA = new Co3969ctor_BoolArray();
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
