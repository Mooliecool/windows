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
using System;
using System.Collections;
using System.Globalization;
class Co1672
{
    public String s_strActiveBugNums                 = "";
    public static String s_strDtTmVer                = "";
    public static String s_strComponentBeingTested   = "Comparer(Object, Object)";
    public String s_strTFName                        = "Co1672Compare_OO.cs";
    public String s_strTFAbbrev                      = "Co1672";
    public String s_strTFPath                        = "";
    public Boolean verbose                           = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "use Comparer to compare two different objects" );
        try
        {
            ++iCountTestcases;
            if ( Comparer.Default.Compare( 2, 1 ) <= 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001a,  compare should have returned positive integer" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "use Comparer to compare two different objects" );
        try
        {
            ++iCountTestcases;
            if ( Comparer.Default.Compare( 1, 2 ) >= 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_002a,  compare should have returned negative integer" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "use Comparer to compare two equal objects" );
        try
        {
            ++iCountTestcases;
            if ( Comparer.Default.Compare( 1, 1 ) != 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_003a,  compare should have returned 0 for equal objects" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_003b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "use Comparer to compare a null object to a non null object" );
        try
        {
            ++iCountTestcases;
            if ( Comparer.Default.Compare( null, 1 ) >= 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_004a,  a null object should be always less than something else" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_004b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "use Comparer to compare an object to a null object" );
        try
        {
            ++iCountTestcases;
            if ( Comparer.Default.Compare( 0, null ) <= 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_005a,  a null object should be always less than something else" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_005b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "for two null objects comparer returns equal" );
        try
        {
            ++iCountTestcases;
            if ( Comparer.Default.Compare( null, null ) != 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_006a,  two nulls should be equal" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_006b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "compare two objects that do not implement IComparable" );
        try
        {
            ++iCountTestcases;
            Comparer.Default.Compare( new Object(), new Object());
            ++iCountErrors;
            Console.WriteLine( "Err_007a,  Expected exception ArgumentException was not thrown." );
        }
        catch (ArgumentException)
        {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_007b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "compare two objects that are not of the same type" );
        try
        {
            ++iCountTestcases;
            Comparer.Default.Compare( 1L, 1 );
            ++iCountErrors;
            Console.WriteLine( "Err_008a,  Expected exception ArgumentException was not thrown." );      
        }
        catch (ArgumentException)
        {}	
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_008b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "compare two objects that are not of the same type" );
        try
        {
            ++iCountTestcases;
            Comparer.Default.Compare( 1, 1L );
            ++iCountErrors;
            Console.WriteLine( "Err_009a,  Expected exception ArgumentException was not thrown." );      
        }
        catch (ArgumentException)
        {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_009b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases="+ iCountTestcases.ToString() );
            return true;
        }
        else
        {
            Console.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors="+ iCountErrors.ToString() +" ,BugNums?: "+ s_strActiveBugNums );
            return false;
        }
    }
    public static void Main( String [] args )
    {
        Co1672 runClass = new Co1672();
        Boolean bResult = runClass.runTest();
        if ( ! bResult )
        {
            Console.WriteLine( runClass.s_strTFPath + runClass.s_strTFName );
            Console.Error.WriteLine( " " );
            Console.Error.WriteLine( "FAiL!  "+ runClass.s_strTFAbbrev );  
            Console.Error.WriteLine( " " );
            Console.Error.WriteLine( "ACTIVE BUGS: " + runClass.s_strActiveBugNums ); 
        }
        if ( bResult == true ) Environment.ExitCode = 0;
        else Environment.ExitCode = 1; 
    }
}  
