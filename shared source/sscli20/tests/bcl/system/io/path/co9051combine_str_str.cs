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
using System;
using System.IO;
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co9051Combine_str_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.Combine(String,String)";
    public static String s_strTFName        = "Co9051Combine_str_str";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            String str1, str2;
            strLoc = "Loc_48v88";
            iCountTestcases++;
            try 
            {
                str1 = Path.Combine(null, "");
                iCountErrors++;
                printerr( "Error_f4999! Expected exception not thrown, str=="+str1);
            } 
            catch( ArgumentNullException aexc) 
            {
                printinfo( "Info_t87g7! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch( Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_28yv8! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                str1 = Path.Combine("", null);
                iCountErrors++;
                printerr( "Error_t980f! Expected exception not thrown, str=="+str1);
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo( "Info_488v8! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_4389v! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_10000";
            iCountTestcases++;
            try 
            {
#if PLATFORM_UNIX
				str1 = Path.Combine("/", "/");
                if(! str1.Equals("/")) 
#else
                str1 = Path.Combine("C:\\", "C:\\");
                if(! str1.Equals("C:\\")) 
#endif
                {
                    iCountErrors++;
                    printerr( "Error_5545! Unexpected string=="+str1);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_1000c! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_20000";
            str1 = Path.Combine("C:\\", "");
            iCountTestcases++;
            if(!str1.Equals("C:\\")) 
            {
                iCountErrors++;
                printerr( "Error_288vf! Unexpected string=="+str1);
            }
            strLoc = "Loc_30000";
            str1 = Path.Combine("", "C:\\");
            iCountTestcases++;
            if(!str1.Equals("C:\\")) 
            {
                iCountErrors++;
                printerr( "Error_3000a! Unexpected string=="+str1);
            }
            strLoc = "Loc_432434";
            str1 = Path.Combine("", "");         
            iCountTestcases++;
            if(!str1.Equals("")) 
            {
                iCountErrors++;
                printerr( "Error_3000a! Unexpected string=="+str1);
            }
            strLoc = "Loc_40000";
            str1 = Path.Combine("Hello", "World");
            iCountTestcases++;
            if(!str1.Equals("Hello" + Path.DirectorySeparatorChar + "World")) 
            {
                iCountErrors++;
                printerr( "Error_4000a! Expected==<Hello\\World>, Got=="+str1);
            }
            strLoc = "Loc_50000";
            str1 = Path.Combine("C:\\Hello\\", "World");
            iCountTestcases++;
            if(!str1.Equals("C:\\Hello\\World")) 
            {
                iCountErrors++;
                printerr( "Error_5000a! Expected==C:\\Hello\\World, got=="+str1);
            }
            strLoc = "Loc_60000";
            iCountTestcases++;
            try 
            {				
                str1 = Path.Combine("C:\\Hello", "\\world");   
                if (! str1.Equals("\\world")) 
                {
                    iCountErrors++;
                    printerr( "Error_4324! Expected==\\world, got=="+str1);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_6000c! Incorrect exception thrown, exc=="+exc.ToString());
            }
#if !PLATFORM_UNIX
            strLoc = "Loc_70000";
            String strUNCName = @"\\radt\VbSsDb\VbTests\shadow\FXBCL\test\auto\System_IO\Path\";
            str1 = Path.Combine(strUNCName, "Testing");
            iCountTestcases++;
            if(!str1.Equals(strUNCName + "Testing")) 
            {
                iCountErrors++;
                printerr( "Error_7000a! Expected==" + strUNCName + "Testing, got=="+str1);
            }
            strLoc = "Loc_80000";
            iCountTestcases++;
            try 
            {
                strUNCName = @"\\radt\VbSsDb\VbTests\shadow\FXBCL\test\auto\System_IO\Path\";
                str1 = Path.Combine("C:\\", strUNCName);
                if(!str1.Equals(strUNCName)) 
                {
                    iCountErrors++;
                    printerr( "Error_8000a! Expected==" + strUNCName + "Testing, got=="+str1);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_8000c! Incorrect exception thrown, exc=="+exc.ToString());
            }
#endif
            strLoc = "Loc_12000";
            str1 = Path.Combine("\\MyDir\\Hello\\", "World\\You\\Are\\My\\Creation");
            iCountTestcases++;
            if(!str1.Equals("\\MyDir\\Hello\\World\\You\\Are\\My\\Creation")) 
            {
                iCountErrors++;
                printerr( "Error_1200a! incorrect string returned, str1=="+str1);
            }
            strLoc = "Loc_13000";
#if PLATFORM_UNIX
			str1 = Path.Combine("/MyDirectory/Sample", "Test");
			iCountTestcases++;
			if(!str1.Equals("/MyDirectory/Sample/Test")) 
#else
            str1 = Path.Combine("C:\\MyDirectory\\Sample", "Test");
            iCountTestcases++;
            if(!str1.Equals("C:\\MyDirectory\\Sample\\Test")) 
#endif
            {
                iCountErrors++;
                printerr( "Error_1300a! Incorrect string returned, str1=="+str1);
            }
            strLoc = "Loc_15000";
#if !PLATFORM_UNIX
            str1 = Path.Combine("C:\\Directory", ".\\SubDir");
            iCountTestcases++;
            if(!str1.Equals("C:\\Directory\\.\\SubDir")) 
            {
                iCountErrors++;
                printerr( "Error_1500a! Incorrect string returned, str1=="+str1);
            }
#endif
            strLoc = "Loc_16000";
#if PLATFORM_UNIX
			str1 = Path.Combine("/Directory/..", "SubDir");
			iCountTestcases++;
			if(!str1.Equals("/Directory/../SubDir")) 
#else
            str1 = Path.Combine("C:\\Directory\\..", "SubDir");
            iCountTestcases++;
            if(!str1.Equals("C:\\Directory\\..\\SubDir")) 
#endif
            {
                iCountErrors++;
                printerr( "Error_1600a! Incorrect string returned, str1=="+str1);
            }
            strLoc = "Loc_17000";
            str1 = "This is a Directory";
            str2 = "This is another directory";
            str1 = Path.Combine(str1, str2);
            if(!str1.Equals("This is a Directory" + Path.DirectorySeparatorChar + str2)) 
            {
                iCountErrors++;
                printerr( "Error_1700a! Incorrect string returned, str1=="+str1);
            }
            strLoc = "Loc_19000";
            str1 = Path.Combine("C://Directory//", "SubDir//Hello");
            iCountTestcases++;
            if(!str1.Equals("C://Directory//SubDir//Hello")) 
            {
                iCountErrors++;
                printerr( "Error_1900a! Incorrect string returned, str1=="+str1);
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
            return true;
        }
        else
        {
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }      
    public void printerr ( String err )
    {
        Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
    }
    public void printinfo ( String info )
    {
        Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co9051Combine_str_str cbA = new Co9051Combine_str_str();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
