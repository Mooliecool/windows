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
public class Co8669Equals_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.Equals(obj)";
    public static String s_strTFName        = "Co8669Equals_obj.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        BitVector32 bv32; 
        BitVector32 bv32_1;       
        int data = 0;                 
        try
        {
            Console.WriteLine("1. two default structs");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            bv32 = new BitVector32();
            bv32_1 = new BitVector32();
            if (! bv32.Equals(bv32_1)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, two default structs are not equal");
            }
            Console.WriteLine("2. two random vectors");
            DateTime time = DateTime.Now;
            data = time.DayOfYear + time.Hour + time.Minute + time.Second;
            System.Random random = new System.Random(data);
            data = random.Next(System.Int32.MinValue, System.Int32.MaxValue);
            iCountTestcases++;
            bv32 = new BitVector32(data);
            bv32_1 = new BitVector32(data);
            if (! bv32.Equals(bv32_1)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, two vectores with random data are not equal");
            }
            Console.WriteLine("3. vector and null");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            if (bv32.Equals(null)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, vector and null are equal");
            }
            Console.WriteLine("4. two different vectors");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            if (data < Int32.MaxValue)
                data++;
            else
                data--;
            bv32_1 = new BitVector32(data);
            if (bv32.Equals(bv32_1)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, two different vectors are equal");
            }
            Console.WriteLine("5. vector and non-vector-object");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            if (bv32.Equals(data)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005, vector and non-vector-object are equal");
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
        Co8669Equals_obj cbA = new Co8669Equals_obj();
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
