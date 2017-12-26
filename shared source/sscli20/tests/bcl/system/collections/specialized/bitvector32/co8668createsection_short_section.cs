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
public class Co8668CreateSection_short_section
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.CreateSection(int16, Section)";
    public static String s_strTFName        = "Co8668CreateSection_short_section.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        BitVector32.Section section;                   
        BitVector32.Section sectionArgument;                   
        Int16 maxValue = 0;        
        BitVector32 bv32;
        int expected = 0;
        try
        {
            sectionArgument = BitVector32.CreateSection(1);
            maxValue = Int16.MinValue;
            Console.WriteLine("1. CreateSection({0}, section) ~ Int16.MinValue", maxValue);
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            try 
            {
                section = BitVector32.CreateSection(maxValue, sectionArgument);
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
            Console.WriteLine("2. CreateSection({0}, section)", maxValue);
            strLoc = "Loc_002oo"; 
            iCountTestcases++;
            try 
            {
                section = BitVector32.CreateSection(maxValue, sectionArgument);
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
            Console.WriteLine("3. CreateSection({0}, section)", maxValue);
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            try 
            {
                section = BitVector32.CreateSection(maxValue, sectionArgument);
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
            Console.WriteLine("4. CreateSection({0}, ({1}, {2}))", maxValue, (int)sectionArgument.Mask, (int)sectionArgument.Offset);
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue, sectionArgument);
            Console.WriteLine("  returned: ({0}, {1})", section.Mask, section.Offset);
            if (section.Mask != maxValue || section.Offset != 1) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0004, returned ({1}, {2}) instead of expected ({0}, {3})", maxValue, section.Mask, section.Offset, 1);
            }
            sectionArgument = BitVector32.CreateSection(2);      
            maxValue = 1;
            Console.WriteLine("5. CreateSection({0}, ({1}, {2}))", maxValue, (int)sectionArgument.Mask, (int)sectionArgument.Offset);
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue, sectionArgument);
            Console.WriteLine("  returned: ({0}, {1})", section.Mask, section.Offset);
            if (section.Mask != maxValue || section.Offset != 2) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0005, returned ({1}, {2}) instead of expected ({0}, {3})", maxValue, section.Mask, section.Offset, 2);
            }
            maxValue = 2;
            Console.WriteLine("6. CreateSection({0}, ({1}, {2}))", maxValue, (int)sectionArgument.Mask, (int)sectionArgument.Offset);
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue, sectionArgument);
            Console.WriteLine("  returned: ({0}, {1})", section.Mask, section.Offset);
            if (section.Mask != 3 || section.Offset != 2) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0006, returned ({1}, {2}) instead of expected ({0}, {3})", 3, section.Mask, section.Offset, 2);
            }
            sectionArgument = BitVector32.CreateSection(Int16.MaxValue);      
            maxValue = Int16.MaxValue;
            expected = 15;                  
            Console.WriteLine("7. CreateSection({0}, ({1}, {2}))", maxValue, (int)sectionArgument.Mask, (int)sectionArgument.Offset);
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue, sectionArgument);
            Console.WriteLine("  returned: ({0}, {1})", section.Mask, section.Offset);
            if (section.Mask != maxValue || section.Offset != expected) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0007, returned ({1}, {2}) instead of expected ({0}, {3})", maxValue, section.Mask, section.Offset, expected);
            }
            sectionArgument = section;      
            expected = 30;                  
            Console.WriteLine("8. CreateSection({0}, ({1}, {2}))", maxValue, (int)sectionArgument.Mask, (int)sectionArgument.Offset);
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            section = BitVector32.CreateSection(maxValue, sectionArgument);
            Console.WriteLine("  returned: ({0}, {1})", section.Mask, section.Offset);
            if (section.Mask != maxValue || section.Offset != expected) 
            {
                iCountErrors++;
                Console.WriteLine("  Err_0008, returned ({1}, {2}) instead of expected ({0}, {3})", maxValue, section.Mask, section.Offset, expected);
            }
            Console.WriteLine("10. Linked list of sections with maxValue 1");   
            maxValue = 1;
            sectionArgument = BitVector32.CreateSection(maxValue);
            bv32 = new BitVector32(Int32.MaxValue);           
            Console.WriteLine(" will apply sections to " + bv32.ToString());
            for (int i = 1; i < 32; i++) 
            {
                iCountTestcases++;
                section = BitVector32.CreateSection(maxValue, sectionArgument);
                if (section.Mask != maxValue || section.Offset != i) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0010a, returned ({0}, {1}) instead of ({2}, {3})", section.Mask, section.Offset, maxValue, i);
                }
                sectionArgument = section;
                iCountTestcases++;
                expected = 1;
                if (i == 31)
                    expected = 0;
                if (bv32[section] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0010b_{2}, returned {0} instead of {1} ", bv32[section], expected, i);
                }
                iCountTestcases++;
                bv32[section] = 0;             
                if (bv32[section] != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0010c_{2}, returned {0} instead of {1} ", bv32[section], 0, i);
                }
                iCountTestcases++;
                expected = 1;
                bv32[section] = 1;               
                if (bv32[section] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0010d_{2}, returned {0} instead of {1} ", bv32[section], expected, i);
                    if (i == 31 && bv32[section] == -1)
                        Console.WriteLine("  Error ");
                }
            } 
            Console.WriteLine("12. Linked list of sections with maxValue 3");   
            maxValue = 3;
            sectionArgument = BitVector32.CreateSection(maxValue);
            bv32 = new BitVector32(Int32.MaxValue);
            Console.WriteLine(" will apply sections to " + bv32.ToString());
            for (int i = 2; i < 32; i+=2) 
            {
                iCountTestcases++;
                section = BitVector32.CreateSection(maxValue, sectionArgument);
                if (section.Mask != maxValue || section.Offset != i) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0012a, returned ({0}, {1}) instead of ({2}, {3})", section.Mask, section.Offset, maxValue, i);
                }
                sectionArgument = section;
                iCountTestcases++;
                expected = 3;
                if (i == 30)
                    expected = 1;
                if (bv32[section] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0012b_{2}, returned {0} instead of {1} ", bv32[section], expected, i);
                }
                iCountTestcases++;
                bv32[section] = 0;             
                if (bv32[section] != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0012c_{2}, returned {0} instead of {1} ", bv32[section], 0, i);
                }
                iCountTestcases++;
                expected = 3;
                bv32[section] = expected;               
                if (i == 30)
                    Console.WriteLine("  " + bv32.ToString());
                if (bv32[section] != expected) 
                {
                    iCountErrors++;
                    Console.WriteLine("  Err_0012d_{2}, returned {0} instead of {1} ", bv32[section], expected, i);
                    if (i == 30 && bv32[section] == (-1))
                        Console.WriteLine("  Error ");
                }
            } 
            maxValue = 3;
            bv32 = new BitVector32(-1);
            Console.WriteLine("14. set section with maxValue {0} to out of range value {1}", maxValue, maxValue * 2);     
            strLoc = "Loc_014oo"; 
            sectionArgument = BitVector32.CreateSection(maxValue);
            section = BitVector32.CreateSection(maxValue, sectionArgument);
            Console.WriteLine("  " + bv32.ToString());
            Console.WriteLine("  init bv32[section] = " + bv32[section]);
            iCountTestcases++;
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
        Co8668CreateSection_short_section cbA = new Co8668CreateSection_short_section();
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
