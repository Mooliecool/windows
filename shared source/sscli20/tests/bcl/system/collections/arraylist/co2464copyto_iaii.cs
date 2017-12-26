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
public class Co2464CopyTo_iAii
{
    public static String strName = "ArrayList.CopyTo";
    public static String strTest = "Co2464CopyTo_iAii";
    public static String strPath = "";
    public static String strActiveBugs = "6313";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        Console.WriteLine( "ACTIVE BUGS " + strActiveBugs );
        ArrayList arrList = null;
        String[] arrCopy = null;
        String [] strHeroes =
            {
                "Aquaman",
                "Atom",
                "Batman",
                "Black Canary",
                "Captain America",
                "Captain Atom",
                "Catwoman",
                "Cyborg",
                "Flash",
                "Green Arrow",
                "Green Lantern",
                "Hawkman",
                "Huntress",
                "Ironman",
                "Nightwing",
                "Robin",
                "SpiderMan",
                "Steel",
                "Superman",
                "Thor",
                "Wildcat",
                "Wonder Woman",
        };
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
                for ( int ii = 0; ii < strHeroes.Length; ++ii )
                {
                    arrList.Add( strHeroes[ii] );
                }
                if ( arrList.Count != strHeroes.Length )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected size <"+ strHeroes.Length + "> ";
                    strInfo = strInfo + "Returned size <"+ arrList.Count + "> ";
                    Console.WriteLine( strTest+ "E_202: " + strInfo );
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
            Console.Error.WriteLine( "[]  Use CopyTo copy range of items to array" );
            try
            {
                int start = 3;
                int count = 15;
                arrCopy = new String[100];
                arrList.CopyTo( start, arrCopy, start, count );
                for ( int ii = start; ii < start + count; ++ii )
                {
                    if ( ((String)arrList[ii]).CompareTo( arrCopy[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected hero <"+ arrCopy[ii] + "> ";
                        strInfo = strInfo + "Returned hero <"+ (String)arrList[ii] + "> ";
                        Console.WriteLine( strTest+ "E_303: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10002: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Bogus Arguments" );
            try
            {
                arrList.CopyTo( 0, arrCopy, -100, 1000 );
                Console.WriteLine( strTest+ "E_404a: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004a: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            try
            {
                arrList.CopyTo( -1, arrCopy, 0, 1 );
                Console.WriteLine( strTest+ "E_404b: Expected ArgumentOutOfRangeException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentOutOfRangeException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004b: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            try
            {
                arrCopy = new String[100];
                arrList.CopyTo( arrList.Count, arrCopy, 0, 0 );
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004c: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            try
            {
                arrCopy = new String[100];
                arrList.CopyTo( arrList.Count - 1, arrCopy, 0, 24 );
                Console.WriteLine( strTest+ "E_404d: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004d: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            try
            {
                arrList.CopyTo( 0, null, 3, 15 );
                Console.WriteLine( strTest+ "E_505: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10005: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            try
            {
                arrCopy = new String[1];
                arrList.CopyTo( 0, arrCopy, 3, 15 );
                Console.WriteLine( strTest+ "E_606: Expected IndexOutArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10006: Unexpected Exception: " + ex.ToString() );
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
        Co2464CopyTo_iAii oCbTest = new Co2464CopyTo_iAii();
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
