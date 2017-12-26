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
public class Co8724CopyTo_array_int
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "NameValueCollection.CopyTo(array, int)";
    public static String s_strTFName        = "Co8724CopyTo_array_int.cs";
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
        NameValueCollection nvc; 
        string [] values = 
        {
            "",
            " ",
            "a",
            "aA",
            "text",
            "     SPaces",
            "1",
            "$%^#",
            "2222222222222222222222222",
            System.DateTime.Today.ToString(),
            Int32.MaxValue.ToString()
        };
        string [] keys = 
        {
            "zero",
            "oNe",
            " ",
            "",
            "aa",
            "1",
            System.DateTime.Today.ToString(),
            "$%^#",
            Int32.MaxValue.ToString(),
            "     spaces",
            "2222222222222222222222222"
        };
        string [] destination;
        int cnt = 0;            
        try
        {
            intl = new IntlStrings(); 
            Console.WriteLine("--- create collection ---");
            strLoc = "Loc_001oo"; 
            iCountTestcases++;
            nvc = new NameValueCollection();
            Console.WriteLine("1. Copy empty collection into empty array");
            iCountTestcases++;
            destination = new string[]{};
            Console.WriteLine("     - CopyTo(arr, -1)");
            try 
            {
                nvc.CopyTo(destination, -1);
                iCountErrors++;
                Console.WriteLine("Err_0001a, no exception");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_001b, unexpected exception: {0}", e.ToString());
            }
            iCountTestcases++;
            Console.WriteLine("     - CopyTo(arr, 0)");
            try 
            {
                nvc.CopyTo(destination, 0);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_001c, unexpected exception: {0}", e.ToString());
            }
            iCountTestcases++;
            Console.WriteLine("     - CopyTo(arr, 1)");
            try 
            {
                nvc.CopyTo(destination, 1);
                iCountErrors++;
                Console.WriteLine("Err_0001d, no exception");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_001e, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("2. Copy empty collection into non-empty array");
            iCountTestcases++;
            destination = new string[values.Length];
            for (int i = 0; i < values.Length; i++) 
            {
                destination[i] = values[i];
            }
            nvc.CopyTo(destination, 0);
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
            Console.WriteLine("3. add simple strings and CopyTo(Array, 0)");
            strLoc = "Loc_003oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            int len = values.Length;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(keys[i], values[i]);
            }
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0003a, count is {0} instead of {1}", nvc.Count, values.Length);
            } 
            destination = new string [len];
            nvc.CopyTo(destination, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc[i], destination[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, copied \"{1}\" instead of \"{2}\"", i, destination[i], nvc[i]);
                }  
            }
            Console.WriteLine("4. add simple strings and CopyTo(Array, {0})", values.Length);
            nvc.Clear();
            strLoc = "Loc_004oo"; 
            iCountTestcases++;
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(keys[i], values[i]);
            }
            if (nvc.Count != len) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0004a, count is {0} instead of {1}", nvc.Count, values.Length);
            } 
            destination = new string[len*2];
            nvc.CopyTo(destination, len);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc[i], destination[i+len], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, copied \"{1}\" instead of \"{2}\"", i, destination[i+len], nvc[i]);
                }  
            }
            Console.WriteLine("5. add intl strings and CopyTo(Array, 0)");
            strLoc = "Loc_005oo"; 
            iCountTestcases++;
            string [] intlValues = new string [len * 2];
            for (int i = 0; i < len*2; i++) 
            {
                string val = intl.GetString(MAX_LEN, true, true, true);
                while (Array.IndexOf(intlValues, val) != -1 )
                    val = intl.GetString(MAX_LEN, true, true, true);
                intlValues[i] = val;
            } 
            Boolean caseInsensitive = false;
            for (int i = 0; i < len * 2; i++) 
            {
                if(intlValues[i].Length!=0 && intlValues[i].ToLower()==intlValues[i].ToUpper())
                    caseInsensitive = true;
            }
            iCountTestcases++;
            nvc.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);
            } 
            if ( nvc.Count != (len) ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0005a, count is {0} instead of {1}", nvc.Count, len);
            } 
            destination = new string[len];
            nvc.CopyTo(destination, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc[i], destination[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0003_{0}b, copied \"{1}\" instead of \"{2}\"", i, destination[i], nvc[i]);
                }  
            }
            Console.WriteLine("6. add intl strings and CopyTo(Array, {0})", len);
            strLoc = "Loc_006oo"; 
            destination = new string[len*2];
            nvc.CopyTo(destination, len);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc[i], destination[i+len], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}a, copied \"{1}\" instead of \"{2}\"", i, destination[i+len], nvc[i]);
                }  
            }
            Console.WriteLine("7. case sensitivity");
            strLoc = "Loc_007oo"; 
            string [] intlValuesLower = new string [len * 2];
            for (int i = 0; i < len * 2; i++) 
            {
                intlValues[i] = intlValues[i].ToUpper();
            }
            for (int i = 0; i < len * 2; i++) 
            {
                intlValuesLower[i] = intlValues[i].ToLower();
            } 
            nvc.Clear();
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(intlValues[i+len], intlValues[i]);     
            }
            destination = new string[len];
            nvc.CopyTo(destination, 0);
            for (int i = 0; i < len; i++) 
            {
                iCountTestcases++;
                if ( String.Compare(nvc[i], destination[i], false) != 0 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}a, copied \"{1}\" instead of \"{2}\"", i, destination[i], nvc[i]);
                }
                iCountTestcases++;
                if ( !caseInsensitive && Array.IndexOf(intlValuesLower, destination[i]) != -1 ) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0006_{0}b, copied lowercase string");
                }  
            }
            Console.WriteLine("8. CopyTo(null, int)");
            strLoc = "Loc_008oo"; 
            iCountTestcases++;
            destination = null;
            try 
            {
                nvc.CopyTo(destination, 0);
                iCountErrors++;
                Console.WriteLine("Err_0008a: no exception ");
            }
            catch (ArgumentNullException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0008b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("9. CopyTo(Array, -1)");
            strLoc = "Loc_009oo"; 
            iCountTestcases++;
            cnt = nvc.Count;
            destination = new string[]{};
            try 
            {
                nvc.CopyTo(destination, -1);
                iCountErrors++;
                Console.WriteLine("Err_0009b: no exception ");
            }
            catch (ArgumentOutOfRangeException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0009c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("10. CopyTo(Array, upperBound+1)");
            strLoc = "Loc_0010oo"; 
            iCountTestcases++;
            if (nvc.Count < 1) 
            {
                for (int i = 0; i < len; i++) 
                {
                    nvc.Add(keys[i], values[i]);
                }
            }
            destination = new string[len];
            try 
            {
                nvc.CopyTo(destination, len);
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
                Console.WriteLine("Err_0010c, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("11. CopyTo(Array, upperBound+2)");
            strLoc = "Loc_010oo"; 
            iCountTestcases++;
            try 
            {
                nvc.CopyTo(destination, len+1);
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
            Console.WriteLine("12. CopyTo(Array, not_enough_space)");
            strLoc = "Loc_012oo"; 
            iCountTestcases++;
            try 
            {
                nvc.CopyTo(destination, len / 2);
                iCountErrors++;
                Console.WriteLine("Err_0012a: no exception ");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0012b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("13. CopyTo(multidim_Array, 0)");
            strLoc = "Loc_013oo"; 
            iCountTestcases++;
            Array dest = Array.CreateInstance(typeof(string), len, len);
            try 
            {
                nvc.CopyTo(dest, 0);
                iCountErrors++;
                Console.WriteLine("Err_0013a: no exception ");
            }
            catch (ArgumentException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0013b, unexpected exception: {0}", e.ToString());
            }
            Console.WriteLine("14. multiple items with same key and CopyTo(Array, 0)");
            strLoc = "Loc_0014oo"; 
            iCountTestcases++;
            nvc.Clear();
            len = values.Length;
            string k = "keykey";
            string exp = "";
            for (int i = 0; i < len; i++) 
            {
                nvc.Add(k, "Value"+i);
                if (i < len-1)
                    exp += "Value"+i+",";
                else
                    exp += "Value"+i;
            }
            if (nvc.Count != 1) 
            {
                iCountErrors++;
                Console.WriteLine("Err_00014a, count is {0} instead of {1}", nvc.Count, 1);
            } 
            destination = new string [1];
            nvc.CopyTo(destination, 0);
            iCountTestcases++;
            if ( String.Compare(nvc[0], destination[0], false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_00014b, copied \"{0}\" instead of \"{1}\"", destination[0], nvc[0]);
            }  
            if ( String.Compare(exp, destination[0], false) != 0 ) 
            {
                iCountErrors++;
                Console.WriteLine("Err_00014b, copied string is not the same as expected: {0}", destination[0]);
            }  
            Console.WriteLine("15. CopyTo(wrong_type, 0)");
            strLoc = "Loc_013oo"; 
            iCountTestcases++;
            dest = Array.CreateInstance(typeof(DictionaryEntry), len);
            try 
            {
                nvc.CopyTo(dest, 0);
                iCountErrors++;
                Console.WriteLine("Err_0015a: no exception ");
            }
            catch (System.InvalidCastException ex) 
            {
                Console.WriteLine("  Expected exception: {0}", ex.Message);
            }
            catch (Exception e) 
            {
                iCountErrors++;
                Console.WriteLine("Err_0015b, unexpected exception: {0}", e.ToString());
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
        Co8724CopyTo_array_int cbA = new Co8724CopyTo_array_int();
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
