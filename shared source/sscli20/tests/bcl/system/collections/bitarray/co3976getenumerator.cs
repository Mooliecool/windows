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
public class Co3976GetEnumerator
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitArray.GetEnumerator";
    public static String s_strTFName        = "Co3976GetEnumerator.cs";
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
        Int32 iNumOfElements;
        Int32 iCount;
        Random rnd1;
        Object oValue;
        IEnumerator ienm1;
        IEnumerator ienm2;
        ICloneable iclo1;
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
                ienm1 = bitArr1.GetEnumerator();
                try
                {
                    oValue = ienm1.Current;
                    iCountErrors++;
                    Console.WriteLine("Err_535wsfd! No exception thrown");					
                }
                catch(InvalidOperationException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_07842af! Unexpected exception thrown, " + ex);					
                }
                iCount=0;
            while(ienm1.MoveNext())
            {
                if((Boolean)ienm1.Current != bolArr1[iCount++])
                {
                    iCountErrors++;
                    Console.WriteLine("Err_753qn_" + iCount + "! Wrong value returned, " + ienm1.Current );
                }
            }
                try
                {
                    oValue = ienm1.Current;
                    Console.WriteLine(oValue);
                    iCountErrors++;
                    Console.WriteLine("Err_07214sff! No exception thrown");					
                }
                catch(InvalidOperationException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_07842af! Unexpected exception thrown, " + ex);					
                }
                ienm1.Reset();
                iCount=0;
            while(ienm1.MoveNext())
            {
                if((Boolean)ienm1.Current != bitArr1[iCount++])
                {
                    iCountErrors++;
                    Console.WriteLine("Err_753qn_" + iCount + "! Wrong value returned, " + ienm1.Current );
                }
            }
                ienm1.Reset();
                iclo1 = (ICloneable)bitArr1.GetEnumerator();
                ienm2 = (IEnumerator)iclo1.Clone();
                iCount=0;
            while(ienm2.MoveNext())
            {
                if((Boolean)ienm2.Current != bitArr1[iCount++])
                {
                    iCountErrors++;
                    Console.WriteLine("Err_753qn_" + iCount + "! Wrong value returned, " + ienm2.Current );
                }
            }
                try
                {
                    oValue = ienm1.Current;
                    iCountErrors++;
                    Console.WriteLine("Err_0723sfg! No exception thrown");					
                }
                catch(InvalidOperationException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_07842af! Unexpected exception thrown, " + ex);					
                }
                strLoc = "Loc_0472eaf!";
                iCountTestcases++;
                ienm1.Reset();
                ienm1.MoveNext();
                bitArr1[0]=false;
                try
                {
                    oValue = ienm1.Current;
                }
                catch(InvalidOperationException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_07842af! Unexpected exception thrown, " + ex);					
                }
                try
                {
                    ienm1.MoveNext();
                    iCountErrors++;
                    Console.WriteLine("Err_2345esfg! No exception thrown");					
                }
                catch(InvalidOperationException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_07842af! Unexpected exception thrown, " + ex);					
                }
                try
                {
                    ienm1.Reset();
                    iCountErrors++;
                    Console.WriteLine("Err_5634dsg! No exception thrown");					
                }
                catch(InvalidOperationException)
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
        Co3976GetEnumerator cbA = new Co3976GetEnumerator();
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
