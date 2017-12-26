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
public class Co2454ctor_Collection
{
    public static String strName = "ArrayList.Constructor_Collection";
    public static String strTest = "Co2454ctor_Collection";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
        ArrayList arrListColl = null;
        int nItems = 100;
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Construct ArrayList" );
            try
            {
                arrList = new ArrayList();
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_101: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                for ( int ii = 0; ii < nItems; ++ii )
                {
                    arrList.Add( ii.ToString() );
                }
                if ( arrList.Count != nItems )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected size <"+ nItems + "> ";
                    strInfo = strInfo + "Returned size <"+ arrList.Count + "> ";
                    ++iCountErrors;
                    break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10001: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Construct new ArrayList from current ArrayList (collection)" );
            try
            {
                arrListColl = new ArrayList( arrList );
                if ( arrListColl == null )
                {
                    Console.WriteLine( strTest+ "E_303: Failed to construct new ArrayList from previous ArrayList" );
                    ++iCountErrors;
                    break;
                }
                if ( arrListColl.Count != nItems )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected size <"+ nItems + "> ";
                    strInfo = strInfo + "Returned size <"+ arrListColl.Count + "> ";
                    Console.WriteLine( strTest+ "E_404: " + strInfo );
                    ++iCountErrors;
                    break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10003: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt bogus construction" );
            try
            {
                arrListColl = new ArrayList( null );
                Console.WriteLine( strTest+ "E_505: Expected NullReferenceException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
                Console.Out.WriteLine( ex.ToString() + " caught" );
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
        }
        while ( false );
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( strTest + " iCountTestcases==" + iCountTestcases + " paSs" );
            return true;
        }
        else
        {
            System.String strFailMsg = null;
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "FAiL" );
            Console.Error.WriteLine( strTest + " iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co2454ctor_Collection oCbTest = new Co2454ctor_Collection();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "E_1000000" );
            Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( strTest+ ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
