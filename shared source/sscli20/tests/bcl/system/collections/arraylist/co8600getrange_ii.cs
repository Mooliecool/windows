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
using System.Collections;
using System.IO;
public class Co8600GetRange_ii
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.GetRange(Int32 index, Int32 count)";
    public static String s_strTFName        = "Co8600GetRange_ii.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int iCountErrors = 0;
        int iCountTestcases = 0;		
        ArrayList list;
        ArrayList range;
        try
        {
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            list = new ArrayList();
            for(int i=0; i<100; i++)
                list.Add(i);
            range = list.GetRange(10, 50);
            if(range.Count != 50)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned" + range.Count);
            }
            for(int i=0; i<range.Count; i++)
            {
                if((int)range[i] != (i+10))
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_9347sdg! wrong value returned" + range[i]);
                }    			
            }
            strLoc = "Loc_9346tsdg";
            iCountTestcases++;
            for(int i=0; i<100; i++)
                range.Add(i+1000);
            if((range.Count != 150) || (list.Count != 200))
            {
                iCountErrors++;
                Console.WriteLine( "Err_93425sdg! wrong value returned" + range.Count);
            }
            for(int i=0; i<50; i++)
            {
                if((int)range[i] != (i+10))
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_74326tsg! wrong value returned" + range[i]);
                }    			
            }
            for(int i=0; i<100; i++)
            {
                if((int)range[50+i] != (i+1000))
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_93475sdg! wrong value returned" + range[50+i]);
                }    			
            }
            strLoc = "Loc_9346tsdg";
            iCountTestcases++;
            for(int i=0; i<100; i++)
                list.Add(i+1000);
            try
            {
                Int32 iTemp = range.Count;
                iCountErrors++;
                Console.WriteLine( "Err_3947sdg! Exception not thrown");
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine( "Err_983475dsg! Exception thrown, " + ex.GetType().Name);    			
            }
            strLoc = "Loc_7356dfsg";
            iCountTestcases++;
            list = new ArrayList();
            for(int i=0; i<100; i++)
                list.Add(i);
            try
            {
                range = list.GetRange(0, 500);
                iCountErrors++;
                Console.WriteLine( "Err_94375wgd! Exception not thrown");
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine( "Err_sdgs! Exception thrown, " + ex.GetType().Name);    			
            }
            try
            {
                range = list.GetRange(0, -1);
                iCountErrors++;
                Console.WriteLine( "Err_94375wgd! Exception not thrown");
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine( "Err_sdgs! Exception thrown, " + ex.GetType().Name);    			
            }
            try
            {
                range = list.GetRange(-1, 50);
                iCountErrors++;
                Console.WriteLine( "Err_94375wgd! Exception not thrown");
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine( "Err_sdgs! Exception thrown, " + ex.GetType().Name);    			
            }
            try
            {
                range = list.GetRange(100, 1);
                iCountErrors++;
                Console.WriteLine( "Err_94375wgd! Exception not thrown");
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine( "Err_sdgs! Exception thrown, " + ex.GetType().Name);    			
            }
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            list = new ArrayList();
            range = list.GetRange(0, 0);
            if(range.Count != 0)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned" + range.Count);
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co8600GetRange_ii cbA = new Co8600GetRange_ii();
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
            Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
