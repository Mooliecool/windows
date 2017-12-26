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
public class Co8664ctor_bitvector32
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.ctor(bitvector32)";
    public static String s_strTFName        = "Co8664ctor_bitvector32.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        BitVector32 bv32;         
        BitVector32 bvExtra;         
        int data = 0;
        try
        {
            data = 0;
            Console.WriteLine("1. ctor(BitVector32({0}))", data);
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = 1;
            Console.WriteLine("2. ctor(BitVector32({0}))", data);
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = -1;
            Console.WriteLine("3. ctor(BitVector32({0}))", data);
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = 2;
            Console.WriteLine("4. ctor(BitVector32({0}))", data);
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = 10;
            Console.WriteLine("5. ctor(BitVector32({0}))", data);
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = 99;
            Console.WriteLine("6. ctor(BitVector32({0}))", data);
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = -9;
            Console.WriteLine("7. ctor(BitVector32({0}))", data);
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = System.Int32.MinValue;
            Console.WriteLine("8. ctor(BitVector32({0}))", data);
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = System.Int32.MaxValue;
            Console.WriteLine("9. ctor(BitVector32({0}))", data);
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = System.Int32.MinValue / 2;
            Console.WriteLine("10. ctor(BitVector32({0}))", data);
            strLoc = "Loc_010oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            data = System.Int32.MaxValue / 2;
            Console.WriteLine("11. ctor(BitVector32({0}))", data);
            strLoc = "Loc_011oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0011a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0011b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
            DateTime time = DateTime.Now;
            data = time.DayOfYear + time.Hour + time.Minute + time.Second;
            System.Random random = new System.Random(data);
            data = random.Next(System.Int32.MinValue, System.Int32.MaxValue);
            Console.WriteLine("12. ctor(BitVector32(random data: {0}))", data);
            strLoc = "Loc_012oo"; 
            iCountTestcases++;
            bvExtra = new BitVector32(data);
            if (bvExtra.Data != data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0012a, Data = {0} after ctor({1})", bvExtra.Data, data);
            }
            iCountTestcases++;
            bv32 = new BitVector32(bvExtra);
            if (bv32.Data != bvExtra.Data) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0012b, Data: returned: {0} , expected: {1}", bv32.Data, bvExtra.Data);
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
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
        Co8664ctor_bitvector32 cbA = new Co8664ctor_bitvector32();
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
