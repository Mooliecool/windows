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
public class Co8599get_IsFixedSize
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.IsFixedSize";
    public static String s_strTFName        = "Co8599get_IsFixedSize";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public Boolean verbose = false;
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( s_strClassMethod + ": " + s_strTFName + " runTest started..." );
        ArrayList list = null;
        ArrayList fixedList = null;
        String strLoc = "Loc_000oo";
        try 
        {
            strLoc = "Loc_001o1";
            iCountTestcases++;
            list = new ArrayList();
            if(list.IsFixedSize)
            {
                iCountErrors++;
                Console.WriteLine( "Err_983475dsg! wrong value returned" + list.IsFixedSize);
            }
            iCountTestcases++;
            try
            {
                for(int i=0; i<100;i++)
                {
                    list.Add(i);
                }
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine( "Err_983475dsg! Exception thrown, " + ex.GetType().Name);    			
            }
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            list = new ArrayList();
            fixedList = ArrayList.FixedSize(list);
            if(!fixedList.IsFixedSize)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned" + fixedList.IsFixedSize);
            }
            iCountTestcases++;
            try
            {
                fixedList.Add(100);
                iCountErrors++;
                Console.WriteLine( "Err_3947sdg! Exception not thrown");
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine( "Err_983475dsg! Exception thrown, " + ex.GetType().Name);    			
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        Console.Error.Write( s_strClassMethod );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( s_strTFName + " iCountTestcases==" + iCountTestcases + " paSs" );
            return true;
        }
        else
        {
            Console.WriteLine( s_strTFName+ s_strTFPath );
            Console.WriteLine( s_strTFName+ "FAiL" );
            Console.Error.WriteLine( s_strTFName + " iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co8599get_IsFixedSize oCbTest = new Co8599get_IsFixedSize();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( s_strTFName+ s_strTFPath );
            Console.WriteLine( s_strTFName+ "E_1000000" );
            Console.WriteLine( s_strTFName+ "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( s_strTFName+ ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
