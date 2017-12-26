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
public class Co4322ctor_IComparer_int
    :
    IComparer
{
    internal static String strName = "SortedList.Constructor_IComparer_int";
    internal static String strTest = "Co4322ctor_IComparer_int";
    internal static String strPath = "";
    public virtual int Compare  
        (
        Object p_obj1
        ,Object p_obj2
        )
    {
        return String.Compare( p_obj1.ToString() ,p_obj2.ToString() );
    }
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co4322ctor_IComparer_int  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        String str5 = null;
        String str6 = null;
        String str7 = null;
        int[] in4a = new int[9];
        int nCapacity = 100;
        try
        {
        LABEL_860_GENERAL:
            do
            {
                strLoc="100cc";
                iCountTestcases++;
                sl2 = new SortedList( this, nCapacity );
                iCountTestcases++;
                iCountTestcases++;
                if ( sl2 == null )
                {
                    Console.WriteLine( strTest+ "E_101" );
                    Console.WriteLine( strTest+ "SortedList creation failure" );
                    ++iCountErrors;
                    break;
                }
                iCountTestcases++;
                if ( sl2.Count  != 0 )
                {
                    Console.WriteLine( strTest+ "E_202" );
                    Console.WriteLine( strTest+ "New SortedList is not empty" );
                    ++iCountErrors;
                }
                iCountTestcases++;
                try
                {
                    sl2 = new SortedList( this, 0 );
                }
                catch( Exception ex )
                {
                    Console.WriteLine( strTest+ "E_404" );
                    Console.WriteLine( strTest+ "SortedList creation Exception" );
                    Console.WriteLine( strTest+ ex.ToString() );
                    ++iCountErrors;
                }
                iCountTestcases++;
                try
                {
                    sl2 = new SortedList( this, -1 );
                    Console.WriteLine( strTest+ "E_303" );
                    Console.WriteLine( strTest+ "Expected ArgumentException" );
                    ++iCountErrors;
                }
                catch( ArgumentException ex )
                {
                }
                catch( Exception ex )
                {
                    Console.WriteLine( strTest+ "E_404" );
                    Console.WriteLine( strTest+ "Generic exception caught" );
                    Console.WriteLine( strTest+ ex.ToString() );
                    ++iCountErrors;
                }
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4322ctor_IComparer_int) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4322ctor_IComparer_int.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4322ctor_IComparer_int.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4322ctor_IComparer_int cbA = new Co4322ctor_IComparer_int();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4322ctor_IComparer_int) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4322ctor_IComparer_int.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
