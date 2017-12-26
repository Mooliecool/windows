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
public class Co8667CreateSection_short
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.CreateSection(int16)";
    public static String s_strTFName        = "Co8667CreateSection_short.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        BitVector32.Section section;                   
        Int16 maxValue = 0;        
        BitVector32 bv32;
        int expected = 0;
        try
        {
            maxValue = Int16.MinValue;
            Console.WriteLine("1. CreateSection({0}) ~ Int16.MinValue", maxValue);
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            try 
            {
                section = BitVector32.CreateSection(maxValue);
                iCountErrors++;
                Console.WriteLine("  Err_0001a, no exception for CreateSection({0})", maxValue);
            }
            catch (ArgumentException e) 
            {
                Console.WriteLine("  Expected exception for CreateSection({0}):{1}", maxValue, e.Message);
            }
            catch (Exception ex) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0001b, unexpected exception for CreateSection({0}): {1}", maxValue, ex.ToString());
            }
            maxValue = -1;
            Console.WriteLine("2. CreateSection({0})", maxValue);
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            try 
            {
                section = BitVector32.CreateSection(maxValue);
                iCountErrors++;
                Console.WriteLine("  Err_0002a, no exception for CreateSection({0})", maxValue);
            }
            catch (ArgumentException e) 
            {
                Console.WriteLine("  Expected exception for CreateSection({0}):{1}", maxValue, e.Message);
            }
            catch (Exception ex) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0002b, unexpected exception for CreateSection({0}): {1}", maxValue, ex.ToString());
            }
            maxValue = 0;
            Console.WriteLine("3. CreateSection({0})", maxValue);
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            try 
            {
                section = BitVector32.CreateSection(maxValue);
                iCountErrors++;
                Console.WriteLine("  Err_0003a, no exception for CreateSection({0})", maxValue);
            }
            catch (ArgumentException e) 
            {
                Console.WriteLine("  Expected exception for CreateSection({0}):{1}", maxValue, e.Message);
            }
            catch (Exception ex) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0003b, unexpected exception for CreateSection({0}): {1}", maxValue, ex.ToString());
            }
            maxValue = 1;
            Console.WriteLine("4. CreateSection({0})", maxValue);
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue);
            Console.WriteLine("  CreateSection(1): ({0}, {1})", section.Mask, section.Offset);
            if (section.Mask != maxValue || section.Offset != 0) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0004, CreateSection({0}) returned ({1}, {2})", maxValue, section.Mask, section.Offset);
            }
            maxValue = Int16.MaxValue;
            Console.WriteLine("5. CreateSection({0}) ~ Int16.MaxValue", maxValue);
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue);
            Console.WriteLine("  CreateSection(Int16.MaxValue): ({0}, {1})", section.Mask, section.Offset);
            if (section.Mask != maxValue || section.Offset != 0) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0005, CreateSection({0}) returned ({1}, {2})", maxValue, section.Mask, section.Offset);
            }
            DateTime time = DateTime.Now;
            int max = time.DayOfYear + time.Hour + time.Minute + time.Second;
            System.Random random = new System.Random(max);
            max = random.Next(1, System.Int16.MaxValue);
            maxValue = (Int16)max;
            Console.WriteLine("6. CreateSection({0}) ~ random value", maxValue);
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue);
            expected = 0;           
            while (maxValue > (int)System.Math.Pow(2, expected))
                expected++;
            Console.WriteLine("  Mask: (real, exp): ({0}, {1})", section.Mask, (int)System.Math.Pow(2, expected) - 1);
            if (section.Mask != (int)System.Math.Pow(2, expected) - 1 || section.Offset != 0) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0006, CreateSection({0}) returned ({1}, {2}) instead of expected ({3}, {4})", 
                    maxValue, section.Mask, section.Offset, (int)System.Math.Pow(2, expected) - 1, 0); 
            }
            maxValue = 1;
            Console.WriteLine("7. CreateSection({0}) and apply to BitVector32({0})", maxValue);
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue);
            bv32 = new BitVector32(maxValue);
            Console.WriteLine("  - get section value");
            if (bv32[section] != maxValue) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0007a, BitVector32[{0}] returned {1} instead of expected {2}", section.ToString(), bv32[section], maxValue);
            }
            expected = 0;
            iCountTestcases++;
            Console.WriteLine("  - set section value to {0}", expected);
            bv32[section] = expected;
            if (bv32[section] != expected || bv32.Data != expected) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0007b, failed to set section to {0}", expected);
            }
            expected = 1;
            iCountTestcases+=5;
            Console.WriteLine("  - set section value to {0}", expected);
            bv32[section] = expected;
            if (bv32[section] != expected || bv32.Data != expected) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0007c, failed to set section to {0}", expected);
            }
            maxValue = 6;
            Console.WriteLine("8. CreateSection({0}) and apply to BitVector32({0})", maxValue);
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue);
            bv32 = new BitVector32(maxValue);
            Console.WriteLine("  - get section value");
            if (bv32[section] != maxValue) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0008a, BitVector32[{0}] returned {1} instead of expected {2}", section.ToString(), bv32[section], maxValue);
            }
            expected = 0;
            iCountTestcases++;
            Console.WriteLine("  - set section value to {0}", expected);
            bv32[section] = expected;
            if (bv32[section] != expected || bv32.Data != expected) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0008b, failed to set section to {0}", expected);
            }
            expected = 1;
            iCountTestcases++;
            Console.WriteLine("  - set section value to {0}", expected);
            bv32[section] = expected;
            if (bv32[section] != expected || bv32.Data != expected) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0008c, failed to set section to {0}", expected);
            }
            expected = 3;
            iCountTestcases++;
            Console.WriteLine("  - set section value to {0}", expected);
            bv32[section] = expected;
            if (bv32[section] != expected || bv32.Data != expected) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0008d, failed to set section to {0}", expected);
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
        Co8667CreateSection_short cbA = new Co8667CreateSection_short();
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
