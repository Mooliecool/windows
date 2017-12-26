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
public class Co8672ToString_bv
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.ToString(BitVector32)";
    public static String s_strTFName        = "Co8672ToString_bv.cs";
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
        string str = "";              
        string str_1 = "";                    
        int data = 0;                 
        try
        {
            Console.WriteLine("1. two default structs");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            bv32 = new BitVector32();
            bv32_1 = new BitVector32();
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, ToString() of two default structs: \"{0}\" != \"{1}\"", str, str_1);
            }
            Console.WriteLine("2. two random vectors");
            DateTime time = DateTime.Now;
            data = time.DayOfYear + time.Hour + time.Minute + time.Second;
            System.Random random = new System.Random(data);
            data = random.Next(System.Int32.MinValue, System.Int32.MaxValue);
            iCountTestcases++;
            bv32 = new BitVector32(data);
            bv32_1 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002, ToString() of two equal vectors: \"{0}\" != \"{1}\"", str, str_1);
            }
            Console.WriteLine("3. two different vectors");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            if (data < Int32.MaxValue)
                data++;
            else
                data--;
            bv32_1 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) == 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003, ToString() of two different vectors: \"{0}\" == \"{1}\"", str, str_1);
            }
            Console.WriteLine("4. same vector - default struct");
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            bv32 = new BitVector32();
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004, ToString() of the same default struct: \"{0}\" != \"{1}\"", str, str_1);
            }
            Console.WriteLine("5. same vector");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005, ToString() of the same vector: \"{0}\" != \"{1}\"", str, str_1);
            }
            data = 0;     
            Console.WriteLine("6. two vectors({0})", data);
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            bv32_1 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006, ToString() of two {2}-vectors: \"{0}\" != \"{1}\"", str, str_1, data);
            }
            data = 1;     
            Console.WriteLine("7. two vectors({0})", data);
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            bv32_1 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007, ToString() of two {2}-vectors: \"{0}\" != \"{1}\"", str, str_1, data);
            }
            data = -1;     
            Console.WriteLine("8. two vectors({0})", data);
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            bv32_1 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008, ToString() of two {2}-vectors: \"{0}\" != \"{1}\"", str, str_1, data);
            }
            data = Int32.MaxValue;     
            Console.WriteLine("9. two vectors({0})", data);
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            bv32_1 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009, ToString() of two {2}-vectors: \"{0}\" != \"{1}\"", str, str_1, data);
            }
            data = Int32.MinValue;     
            Console.WriteLine("10. two vectors({0})", data);
            strLoc = "Loc_010oo"; 
            iCountTestcases++;
            bv32 = new BitVector32(data);
            bv32_1 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            str_1 = BitVector32.ToString(bv32_1);
            if (String.Compare(str, str_1) != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0010, ToString() of two {2}-vectors: \"{0}\" != \"{1}\"", str, str_1, data);
            }
            Console.WriteLine("11. contains \"BitVector32\"");
            strLoc = "Loc_011oo"; 
            iCountTestcases++;
            time = DateTime.Now;
            data = time.DayOfYear + time.Hour + time.Minute + time.Second;
            random = new System.Random(data);
            data = random.Next(System.Int32.MinValue, System.Int32.MaxValue);
            bv32 = new BitVector32(data);
            str = BitVector32.ToString(bv32);
            Console.WriteLine("BitVector32.ToString(BitVector32) returns: " + str);
            if (str.IndexOf("BitVector32") == -1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0011, ToString() doesn't contain \"BitVector32\"");
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
        Co8672ToString_bv cbA = new Co8672ToString_bv();
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
