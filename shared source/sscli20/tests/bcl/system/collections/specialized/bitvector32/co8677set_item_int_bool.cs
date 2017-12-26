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
public class Co8677set_Item_int_bool
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.Item[int32]_set";
    public static String s_strTFName        = "Co8677set_Item_int_bool.cs";
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
        int [] bits =              
        {             
            0,                  
            1,                  
            2,                  
            3,                  
            7,                  
            15,                 
            16,                 
            Int16.MaxValue,     
            Int32.MaxValue - 1, 
            Int32.MinValue,     
            Int16.MinValue,     
            -1                  
        };                      
        try
        {
            Console.WriteLine("1. default ctor");
            strLoc = "Loc_001oo"; 
            bv32 = new BitVector32();
            Console.WriteLine(" " + bv32.ToString());
            iCountTestcases++;
            if (bv32.Data != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, Data = {0} after default ctor", bv32.Data);
            }
            bool expected = false;
            for (int i = 0; i < bits.Length; i++) 
            {
                iCountTestcases++;
                bv32[bits[i]] = true;
               // if (i==9 || i == 10 || i == 11)
                 //   expected = false;
                //else
                    expected = true;
                if (bv32[bits[i]] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}a, bit[{3}]: returned {1} instead of {2}", i, bv32[bits[i]], true, bits[i]);
                }
                iCountTestcases++;
                if (bv32.Data != bits[i]) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}b, Data returned {1} instead of {2}", i, bv32.Data, bits[i]);
                }
                iCountTestcases++;
                bv32[bits[i]] = false;
                if (i==0)
                    expected = true;
                else
                    expected = false;
                if (bv32[bits[i]] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}c, bit[{3}]: returned {1} instead of {2}", i, bv32[bits[i]], expected, bits[i]);
                }
                iCountTestcases++;
                if (bv32.Data != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0001_{0}d, Data returned {1} instead of {2}", i, bv32.Data, 0);
                }
            }
            data = -1;
            Console.WriteLine("2. BitVector32({0})", data);
            strLoc = "Loc_002oo"; 
            bv32 = new BitVector32(data);
            Console.WriteLine(" " + bv32.ToString());
            iCountTestcases++;
            if (bv32.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Data = {0} ", bv32.Data);
            }
            for (int i = 0; i < bits.Length; i++) 
            {
                iCountTestcases++;
                bv32[bits[i]] = false;
                if (i==0)
                    expected = true;
                else
                    expected = false;
                if (bv32[bits[i]] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}a, bit[{3}]: returned {1} instead of {2}", i, bv32[bits[i]], expected, bits[i]);
                }
                iCountTestcases++;
                if (bv32.Data != ~bits[i]) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}b, Data returned {1} instead of {2}", i, bv32.Data, ~bits[i]);
                }
                iCountTestcases++;
                bv32[bits[i]] = true;
				//if (i==9 || i == 10 || i == 11)
				//	expected = false;
				//else
					expected = true;
				if (bv32[bits[i]] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}c, bit[{3}]: returned {1} instead of {2}", i, bv32[bits[i]], true, bits[i]);
                }
                iCountTestcases++;
                if (bv32.Data != data) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}d, Data returned {1} instead of {2}", i, bv32.Data, data);
                }
            }
            for (int i = 0; i < bits.Length; i++) 
            {
                int ind = i+3;
                data = bits[i];
                Console.WriteLine((ind).ToString() + ". BitVector32({0})", data);
                strLoc = "Loc_00" + ind + "oo"; 
                bv32 = new BitVector32(data);
                Console.WriteLine(" " + bv32.ToString());
                iCountTestcases++;
                if (bv32.Data != data) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_000{1}, Data = {0} ", bv32.Data, ind);
                }
                iCountTestcases++;
                bv32[bits[i]] = false;
                if (i==0)
                    expected = true;
                else
                    expected = false;
                if (bv32[bits[i]] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_000{4}_{0}a, bit[{3}]: returned {1} instead of {2}", i, bv32[bits[i]], expected, bits[i], ind);
                }
                iCountTestcases++;
                if (bv32.Data != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_000{4}_{0}b, Data returned {1} instead of {2}", i, bv32.Data, 0, ind);
                }
                iCountTestcases++;
                bv32[bits[i]] = true;
                //if (i==9 || i == 10 || i == 11)
                  //  expected = false;
                //else
                    expected = true;
                if (bv32[bits[i]] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_000{4}_{0}c, bit[{3}]: returned {1} instead of {2}", i, bv32[bits[i]], true, bits[i], ind);
                }
                iCountTestcases++;
                if (bv32.Data != data) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_000{3}_{0}d, Data returned {1} instead of {2}", i, bv32.Data, data, ind);
                }
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine("");
            Console.WriteLine( "Pass.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
            return true;
        }
        else
        {
            Console.WriteLine("");
            Console.WriteLine("Fail!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    private void ClearArray(int[] arr) 
    {
        for (int i = 0; i < arr.Length; i++) 
        {
            arr[i] = 0;
        }
    }  
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co8677set_Item_int_bool cbA = new Co8677set_Item_int_bool();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : Fail! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
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
