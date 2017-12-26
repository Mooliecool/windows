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
public class Co8676set_Item_Section_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BitVector32.Item[section]";
    public static String s_strTFName        = "Co8676set_Item_Section_int.cs";
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
        BitVector32.Section [] sect = new BitVector32.Section [6];
        sect[0] = BitVector32.CreateSection(1);        
        sect[1] = BitVector32.CreateSection(1, sect[0]);        
        sect[2] = BitVector32.CreateSection(7);        
        sect[3] = BitVector32.CreateSection(Int16.MaxValue);        
        sect[4] = BitVector32.CreateSection(Int16.MaxValue, sect[3]);        
        sect[5] = BitVector32.CreateSection(1, sect[4]);        
        sect[5] = BitVector32.CreateSection(1, sect[5]);        
        try
        {
            Console.WriteLine("- default ctor");
            strLoc = "Loc_001oo"; 
            bv32 = new BitVector32();
            Console.WriteLine(bv32.ToString());
            iCountTestcases++;
            if (bv32.Data != 0) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001, Data = {0} after default ctor", bv32.Data);
            }
            Console.WriteLine("1. set all sections to max value");
            for (int i = 0; i < sect.Length; i++) 
            {
                iCountTestcases++;
                bv32[sect[i]] = (int)sect[i].Mask;
                if (bv32[sect[i]] != (int)sect[i].Mask ) 
                {
                    iCountErrors++;
                    string temp = "Err_0001_" + i;
                    if (i == sect.Length - 1)
                        temp += " - Error - section for last bit";
                    Console.WriteLine("{0} returned {1} instead of {2}", temp, bv32[sect[i]], (int)sect[i].Mask);
                }
            }
            Console.WriteLine("2. set all sections to 0");
            for (int i = 0; i < sect.Length; i++) 
            {
                iCountTestcases++;
                bv32[sect[i]] = 0;
                if (bv32[sect[i]] != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0002_{0}, returned {1} instead of {2}", i, bv32[sect[i]], 0);
                }
            }
            Console.WriteLine("3. set all sections to 1");
            for (int i = 0; i < sect.Length; i++) 
            {
                iCountTestcases++;
                bv32[sect[i]] = 1;
                if (bv32[sect[i]] != 1 ) 
                {
                    iCountErrors++;
                    string temp = "Err_0003_" + i;
                    if (i == sect.Length - 1)
                        temp += " - Error - section for last bit";
                    Console.WriteLine("{0} returned {1} instead of {2}", temp, bv32[sect[i]], 1);
                }
            }
            Console.WriteLine("4. set to (max/2) where valid");
            for (int i = 0; i < sect.Length; i++) 
            {
                iCountTestcases++;
                if (((int)sect[i].Mask) > 3) 
                {
                    data = ((int)sect[i].Mask) / 2;
                    bv32[sect[i]] = data;
                    if (bv32[sect[i]] != data ) 
                    {
                        iCountErrors++;
                        string temp = "Err_0004_" + i;
                        Console.WriteLine("{0} returned {1} instead of {2}", temp, bv32[sect[i]], data);
                    }
                }
            }
# if retail
            Console.WriteLine("5. set all to -1");
            for (int i = 0; i < sect.Length; i++) {
    		    iCountTestcases++;
                try {
                    bv32[sect[i]] = -1;
                    if (bv32[sect[i]] != sect[i].Mask ) {
        				iCountErrors++;
                        string temp = "Err_0005a_" + i;
        				Console.WriteLine(temp + ": didn't set section to " + sect[i].Mask);
                    }
                }
                catch (Exception e) {
    				iCountErrors++;
                    string temp = "Err_0005b_" + i;
    				Console.WriteLine(temp + ": unexpected exception - according to spec should not throw");
    				Console.WriteLine(e.ToString());
                }
            }
            Console.WriteLine("6. set all to (max + 1)");
            for (int i = 0; i < sect.Length; i++) {
    		    iCountTestcases++;
                try {
                    int exp = (int)(sect[i].Mask) + 1;
                    bv32[sect[i]] = exp;
                    exp = sect[i].Mask & exp;
                    if (bv32[sect[i]] != exp ) {
        				iCountErrors++;
                        string temp = "Err_0006a_" + i;
        				Console.WriteLine(temp + ": didn't set section to " + exp);
                    }
                }
                catch (Exception e) {
    				iCountErrors++;
                    string temp = "Err_0006b_" + i;
    				Console.WriteLine(temp + ": unexpected exception - according to spec should not throw");
    				Console.WriteLine(e.ToString());
                }
            }
#endif        
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
        Co8676set_Item_Section_int cbA = new Co8676set_Item_Section_int();
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
