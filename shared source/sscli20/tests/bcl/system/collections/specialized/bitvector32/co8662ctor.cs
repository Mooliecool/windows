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
public class Co8662ctor
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.ctor";
    public static String s_strTFName        = "Co8662ctor.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        BitVector32 bv32; 
        BitVector32 bv32Temp;       
        try
        {
            Console.WriteLine("--- default ctor ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            bv32 = new BitVector32();
            Console.WriteLine("1. check Data");
            if (bv32.Data != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, Data = {0} after default ctor", bv32.Data);
            }
            iCountTestcases++;
            Console.WriteLine("2. check returned Type");
            string type = bv32.GetType().ToString().Trim();
            Console.WriteLine(" GetType(): " + type);
            if (type.IndexOf("BitVector32") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0002: returned type doesn't contain \"BitVector32\"");
            }
            bv32Temp = new BitVector32(3);         
            string type1 = bv32Temp.GetType().ToString().Trim();      
            if (String.Compare(type, type1) != 0) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0003: returned types of two vectors differ");
            }
            iCountTestcases++;
            Console.WriteLine("3. call ToString()");
            string result = bv32.ToString();
            Console.WriteLine(" ToString(): " + result);
            if (result.IndexOf("BitVector32") == -1) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0004: ToString() doesn't contain \"BitVector32\"");
            }
            iCountTestcases++;
            Console.WriteLine("4. Item(1)");
            bool item = bv32[1];
            Console.WriteLine(" Item(1): " + item);
            if (item) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0005: Item(0) returned {0} instead of {1}", item, false);
            }
            iCountTestcases++;
            Console.WriteLine("5. Equals");
            bv32Temp = new BitVector32();
            if (! bv32.Equals(bv32Temp) ) 
            {  
                iCountErrors++;
                Console.WriteLine("Err_0006: two default vectors are not equal");
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
        Co8662ctor cbA = new Co8662ctor();
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
