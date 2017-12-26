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
using System.IO;
using System.Text;    
using System;
using System.Collections;
using System.Collections.Specialized;
public class Co8665CreateMask
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.CreateMask";
    public static String s_strTFName        = "Co8665CreateMask.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        BitVector32 bv32;         
        int data = 0;
        int mask = 0;
        try
        {
            Console.WriteLine("1. Create first Mask");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            mask = BitVector32.CreateMask();
            Console.WriteLine(" first mask: " + mask);
            if (mask != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, Created Mask: {0} , expected: {1}", mask, 1);
            }
            Console.WriteLine("2. Create multiple first Masks");
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            mask = BitVector32.CreateMask();
            int cnt = 50;
            int mask1 = 0;
            for (int i = 0; i < cnt; i++) 
            {
                mask1 = BitVector32.CreateMask();
                if (mask1 != mask) 
                {
                    iCountErrors++;
                    string err = "Err_0002" + (i+1).ToString();
                    Console.WriteLine(err + ", Created Mask: {0} , expected: {1}", mask1, mask);
                }
            }
            data = 0;
            Console.WriteLine("3. Apply first Mask to BitVector32({0})", data);
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            if (bv32.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, Data: returned: {0} , expected: {1}", bv32.Data, data);
            }
            iCountTestcases++;
            mask = BitVector32.CreateMask();
            if (mask != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, Created Mask: {0} , expected: {1}", mask, 1);
            }
            iCountTestcases++;
            if ((bv32.Data & mask) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003c, {0} & {1} returned {2}, expected {3}", bv32.Data, mask, (bv32.Data & mask), 0);
            }
            iCountTestcases++;
            if ((bv32.Data | mask) != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003d, {0} | {1} returned {2}, expected {3}", bv32.Data, mask, (bv32.Data | mask), 1);
            }
            data = 1;
            Console.WriteLine("4. Apply first Mask to BitVector32({0})", data);
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            if (bv32.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, Data: returned: {0} , expected: {1}", bv32.Data, data);
            }
            iCountTestcases++;
            if ((bv32.Data & mask) != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, {0} & {1} returned {2}, expected {3}", bv32.Data, mask, (bv32.Data & mask), 1);
            }
            iCountTestcases++;
            if ((bv32.Data | mask) != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003d, {0} | {1} returned {2}, expected {3}", bv32.Data, mask, (bv32.Data | mask), 1);
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_general!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "Pass.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
            return true;
        }
        else
        {
            Console.WriteLine("Fail!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co8665CreateMask cbA = new Co8665CreateMask();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : Fail! Error Err_main! Uncaught Exception in main(), exc_main=="+exc_main);
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "Fail!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
    }
}
