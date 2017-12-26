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
public class Co8666CreateMask_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.CreateMask(int32)";
    public static String s_strTFName        = "Co8666CreateMask_int.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        int mask = 0;                   
        int maskArgument = 0;        
        int iter = 0;               
        int expected = 0;              
        try
        {
            Console.WriteLine("1. CreateMask()");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            maskArgument = BitVector32.CreateMask();
            Console.WriteLine("     first mask: " + maskArgument);
            expected = (int)System.Math.Pow(2, iter);
            if (maskArgument != expected) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001b, Created Mask: {0} , expected: {1}", maskArgument, expected);
            }
            Console.WriteLine("2. second mask: CreateMask(first_mask)");
            iter++;
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            mask = BitVector32.CreateMask(maskArgument);
            Console.WriteLine("     second mask: " + mask);
            expected = (int)System.Math.Pow(2, iter);
            Console.WriteLine("Pow(2, {0}) = {1}", maskArgument, expected);
            if (mask != expected) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, Created Mask: {0} , expected: {1}", mask, expected);
            }
            Console.WriteLine("3. Create multiple second Masks");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            int cnt = 50;                 
            for (int i = 0; i < cnt; i++) 
            {
                mask = BitVector32.CreateMask(maskArgument);
                if (mask != expected) 
                {
                    iCountErrors++;
                    string err = "Err_0003" + (i+1).ToString();
                    Console.WriteLine(err + ", Created Mask: {0} , expected: {1}", mask, expected);
                }
            }
            Console.WriteLine("4. third mask: CreateMask(second_mask)");
            strLoc = "Loc_004oo"; 
            iter++;
            maskArgument = BitVector32.CreateMask(BitVector32.CreateMask());
            expected = (int)System.Math.Pow(2, iter);
            iCountTestcases++;
            mask = BitVector32.CreateMask(maskArgument);
            Console.WriteLine("     third mask: " + mask);
            if (mask != expected) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, Created Mask: {0} , expected: {1}", mask, expected);
            }
            Console.WriteLine("5. Create multiple third Masks");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            for (int i = 0; i < cnt; i++) 
            {
                mask = BitVector32.CreateMask(maskArgument);
                if (mask != expected) 
                {
                    iCountErrors++;
                    string err = "Err_0005" + (i+1).ToString();
                    Console.WriteLine(err + ", Created Mask: {0} , expected: {1}", mask, expected);
                }
            }
            Console.WriteLine("6. fourth mask: CreateMask(third_mask)");
            strLoc = "Loc_006oo";
            iter++; 
            maskArgument = BitVector32.CreateMask(BitVector32.CreateMask(BitVector32.CreateMask()));
            expected = (int)System.Math.Pow(2, iter);
            iCountTestcases++;
            mask = BitVector32.CreateMask(maskArgument);
            Console.WriteLine("     third mask: " + mask);
            if (mask != expected) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006, Created Mask: {0} , expected: {1}", mask, expected);
            }
            Console.WriteLine("7. Create multiple fourth Masks");
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            for (int i = 0; i < cnt; i++) 
            {
                mask = BitVector32.CreateMask(maskArgument);
                if (mask != expected) 
                {
                    iCountErrors++;
                    string err = "Err_0007" + (i+1).ToString();
                    Console.WriteLine(err + ", Created Mask: {0} , expected: {1}", mask, expected);
                }
            }
            Console.WriteLine("8. Circular list of masks");
            Console.WriteLine(" - positive masks");
            strLoc = "Loc_008oo"; 
            maskArgument = BitVector32.CreateMask();
            mask = 0;
            for (iter = 1; iter < 32; iter++) 
            {
                iCountTestcases++;
                mask = BitVector32.CreateMask(maskArgument);
                Console.WriteLine("     CreateMask({0}) = {1} ", maskArgument, mask);
                expected = (int)System.Math.Pow(2, iter);
                if ( mask != expected && !(iter == 31 && mask == -2147483648)) 
                {
                    iCountErrors++;
                    string err = "Err_0008" + iter.ToString();
                    Console.WriteLine(err + ", Created Mask: {0} , expected: {1}", maskArgument, expected);
                }
                maskArgument = mask;
            }
            Console.WriteLine(" - negative masks");
            strLoc = "Loc_009oo"; 
            Console.WriteLine("   Start from CreateMask(Int32.MaxValue)");
            maskArgument = Int32.MaxValue;
            mask = 0;
            for (iter = 1; iter < 32; iter++) 
            {
                iCountTestcases++;
                mask = BitVector32.CreateMask(maskArgument);
                Console.WriteLine("     CreateMask({0}) = {1} ", maskArgument, mask);
                expected = (-1) * (int)System.Math.Pow(2, iter);
                if (mask != expected && !(iter == 31 && mask == -2147483648)) 
                {
                    iCountErrors++;
                    string err = "Err_0009" + iter.ToString();
                    Console.WriteLine(err + ", Created Mask: {0} , expected: {1}", maskArgument, expected);
                }
                maskArgument = mask;
            }
            Console.WriteLine("10. CreateMask(0)");
            strLoc = "Loc_010oo"; 
            maskArgument = 0;
            mask = 0;
            iCountTestcases++;
            expected = (int)System.Math.Pow(2, maskArgument);
            iCountTestcases++;
            mask = BitVector32.CreateMask(maskArgument);
            Console.WriteLine("     CreateMask({0}) returned {1} ", maskArgument, mask);
            if (mask != expected) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010, CreatedMask() returned: {0} , expected: {1}", mask, expected);
            }
            DateTime time = DateTime.Now;
            maskArgument = time.DayOfYear + time.Hour + time.Minute + time.Second;
            System.Random random = new System.Random(maskArgument);
            maskArgument = random.Next(System.Int32.MinValue / 2 + 1, System.Int32.MaxValue / 2 - 1);
            Console.WriteLine("11. CreateMask(random_value)");
            strLoc = "Loc_011oo"; 
            mask = 0;
            iCountTestcases++;
            if (maskArgument == 0)
                expected = (int)System.Math.Pow(2, maskArgument);
            else
                expected = maskArgument * 2;
            iCountTestcases++;
            mask = BitVector32.CreateMask(maskArgument);
            Console.WriteLine("     CreateMask({0}) returned {1} ", maskArgument, mask);
            if (mask != expected) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0011, CreatedMask() returned: {0} , expected: {1}", mask, expected);
            }
            Console.WriteLine("12. CreateMask(Int32.MinValue)");
            strLoc = "Loc_012oo"; 
            Console.WriteLine(" Int32.MinValue = " + Int32.MinValue+ ", Pow(2, 31) = " + (int)System.Math.Pow(2, 31) );
            maskArgument = Int32.MinValue;
            mask = 0;
            iCountTestcases++;
            try 
            {
                mask = BitVector32.CreateMask(maskArgument);
                iCountErrors++;
                Console.WriteLine(" Err_0012a: no exception");
            }
            catch (InvalidOperationException e) 
            {
                Console.WriteLine(" expected exception: " + e.Message);
            }
            catch (Exception ex) 
            {
                iCountErrors++;
                Console.WriteLine(" Err_0012b: unexpected exception: " + ex.ToString());
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
        Co8666CreateMask_int cbA = new Co8666CreateMask_int();
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
