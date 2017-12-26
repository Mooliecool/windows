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
using GenStrings;
public class Co8742CopyTo_strA_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StringCollection.CopyTo(string[], int)";
    public static String s_strTFName        = "Co8742CopyTo_strA_int.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public const int MAX_LEN = 50;          
    public virtual bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver: " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        IntlStrings intl;
        String strLoc = "Loc_000oo";
        StringCollection sc; 
        string [] values = 
        {
            "",
            " ",
            "a",
            "aa",
            "text",
            "     spaces",
            "1",
            "$%^#",
            "2222222222222222222222222",
            System.DateTime.Today.ToString(),
            Int32.MaxValue.ToString()
        };
        string[] destination;
        int cnt = 0;            
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            sc = new StringCollection();
            Console.WriteLine("1. Copy empty collection into empty array");
            iCountTestcases++;
            destination = new string[values.Length];
            for (int i = 0; i < values.Length; i++) 
            {
                destination[i] = "";
            }
            sc.CopyTo(destination, 0);
            if( destination.Length != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0001a, altered array after copying empty collection");
            } 
            if (destination.Length == values.Length) 
            {
                for (int i = 0; i < values.Length; i++) 
                {
                    iCountTestcases++;
                    if (String.Compare(destination[i], "", false) != 0) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0001_{0}b, item = \"{1}\" insteead of \"{2}\" after copying empty collection", i, destination[i], "");
                    }
                } 
            }
            Console.WriteLine("2. Copy empty collection into non-empty array");
            iCountTestcases++;
            destination = values;
            sc.CopyTo(destination, 0);
            if( destination.Length != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0002a, altered array after copying empty collection");
            } 
            if (destination.Length == values.Length) 
            {
                for (int i = 0; i < values.Length; i++) 
                {
                    iCountTestcases++;
                    if (String.Compare(destination[i], values[i], false) != 0) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0002_{0}b, altered item {0} after copying empty collection", i);
                    }
                } 
            }
            Console.WriteLine("3. add simple strings and CopyTo([], 0)");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            cnt = sc.Count;
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            destination = new string[values.Length];
            sc.CopyTo(destination, 0);
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(sc[i], destination[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, copied \"{1}\" instead of \"{2}\"", i, destination[i], sc[i]);
                } 
            }
            Console.WriteLine("4. add simple strings and CopyTo([], {0})", values.Length);
            sc.Clear();
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            cnt = sc.Count;
            sc.AddRange(values);
            if (sc.Count != values.Length) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", sc.Count, values.Length);
            } 
            destination = new string[values.Length * 2];
            sc.CopyTo(destination, values.Length);
            for (int i = 0; i < values.Length; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(sc[i], destination[i+values.Length], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, copied \"{1}\" instead of \"{2}\"", i, destination[i+values.Length], sc[i]);
                } 
            }
            Console.WriteLine("5. add intl strings and CopyTo([], 0)");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            string [] intlValues = new string [values.Length];
            for (int i = 0; i < values.Length; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(intlValues);
            if ( sc.Count != (intlValues.Length) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", sc.Count, intlValues.Length);
            } 
            destination = new string[intlValues.Length];
            sc.CopyTo(destination, 0);
            for (int i = 0; i < intlValues.Length; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(sc[i], destination[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0005_{0}b, copied \"{1}\" instead of \"{2}\"", i, destination[i], sc[i]);
                } 
            }
            Console.WriteLine("6. add intl strings and CopyTo([], {0})", intlValues.Length);
            strLoc = "Loc_006oo"; 
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(intlValues);
            if ( sc.Count != (intlValues.Length) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0006a, count is {0} instead of {1}", sc.Count, intlValues.Length);
            } 
            destination = new string[intlValues.Length*2];
            sc.CopyTo(destination, intlValues.Length);
            for (int i = 0; i < intlValues.Length; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(sc[i], destination[i+intlValues.Length], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}b, copied \"{1}\" instead of \"{2}\"", i, destination[i+intlValues.Length], sc[i]);
                } 
            }
            Console.WriteLine("7. CopyTo(null, int)");
            strLoc = "Loc_007oo"; 
            iCountTestcases++;
            sc.Clear();
            sc.AddRange(values);
            if ( sc.Count != (intlValues.Length) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007a, count is {0} instead of {1}", sc.Count, intlValues.Length);
            } 
            destination = null;
            try 
            {
                sc.CopyTo(destination, 0);
                iCountErrors++;
                Console.WriteLine("Err_0007b: no exception ");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0007c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("8. CopyTo(string[], -1)");
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            if (sc.Count != values.Length ) 
            {
                sc.Clear();
                sc.AddRange(values);
                if ( sc.Count != (intlValues.Length) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0008a, count is {0} instead of {1}", sc.Count, intlValues.Length);
                } 
            }
            destination = new string[values.Length];
            try 
            {
                sc.CopyTo(destination, -1);
                iCountErrors++;
                Console.WriteLine("Err_0008b: no exception ");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("9. CopyTo(string[], upperBound+1)");
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            if (sc.Count != values.Length ) 
            {
                sc.Clear();
                sc.AddRange(values);
                if ( sc.Count != (intlValues.Length) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0009a, count is {0} instead of {1}", sc.Count, intlValues.Length);
                } 
            }
            destination = new string[values.Length];
            try 
            {
                sc.CopyTo(destination, values.Length);
                iCountErrors++;
                Console.WriteLine("Err_0009b: no exception ");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("10. CopyTo(string[], upperBound+2)");
            strLoc = "Loc_010oo"; 
            iCountTestcases++;
            if (sc.Count != values.Length ) 
            {
                sc.Clear();
                sc.AddRange(values);
                if ( sc.Count != (intlValues.Length+1) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0010a, count is {0} instead of {1}", sc.Count, intlValues.Length);
                } 
            }
            destination = new string[values.Length];
            try 
            {
                sc.CopyTo(destination, values.Length);
                iCountErrors++;
                Console.WriteLine("Err_0010b: no exception ");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("11. CopyTo(string[], not_enough_space)");
            strLoc = "Loc_011oo"; 
            iCountTestcases++;
            if (sc.Count != values.Length ) 
            {
                sc.Clear();
                sc.AddRange(values);
                if ( sc.Count != (intlValues.Length) ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0011a, count is {0} instead of {1}", sc.Count, intlValues.Length);
                } 
            }
            destination = new string[values.Length];
            try 
            {
                sc.CopyTo(destination, values.Length / 2);
                iCountErrors++;
                Console.WriteLine("Err_0011b: no exception ");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0011c, unexpected exception: {0}", e.ToString());
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
        Co8742CopyTo_strA_int cbA = new Co8742CopyTo_strA_int();
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
