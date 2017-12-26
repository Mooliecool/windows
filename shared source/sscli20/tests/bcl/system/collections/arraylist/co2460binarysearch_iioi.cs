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
public class Co2460BinarySearch_iiOI : IComparer
{
    public static String strName = "ArrayList.BinarySearch";
    public static String strTest = "Co2460BinarySearch";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
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
        String [] strFindHeroes =
            {
                "Batman",
                "Superman",
                "SpiderMan",
                "Wonder Woman",
                "Green Lantern",
                "Flash",
                "Steel"
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
            Console.Error.WriteLine( "[]  Use BinarySearch to find selected items" );
            try
            {
                for ( int ii = 0; ii < strFindHeroes.Length; ++ii )
                {
                    int ndx = arrList.BinarySearch( 0, arrList.Count, strFindHeroes[ii], this );
                    if ( ndx >= strHeroes.Length )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Returned index <"+ ndx + "> out of range";
                        Console.WriteLine( strTest+ "E_303: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                    if ( strHeroes[ndx].CompareTo( strFindHeroes[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected index of hero <"+ strFindHeroes[ii] + "> ";
                        strInfo = strInfo + "Returned index of hero <"+ strHeroes[ndx] + "> ";
                        Console.WriteLine( strTest+ "E_404: " + strInfo );
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
            Console.Error.WriteLine( "[]  Locate item in list using null comparer" );
            try
            {
                int ndx = arrList.BinarySearch( 0, arrList.Count, "Batman", null );
                if ( ndx != 2 )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected index <2> ";
                    strInfo = strInfo + "Returned index <"+ ndx + "> ";
                    Console.WriteLine( strTest+ "E_505: " + strInfo );
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
            Console.Error.WriteLine( "[]  Locate insertion index of new list item" );
            try
            {
                int ndx = arrList.BinarySearch( 0, arrList.Count, "Batgirl", this );
                if ( ~ndx != 2 )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected insertion index <2> ";
                    strInfo = strInfo + "Returned insertion index <"+ ~ndx + "> ";
                    Console.WriteLine( strTest+ "E_505: " + strInfo );
                    ++iCountErrors;
                    break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[] Bogus Arguments" );
            try
            {
                arrList.BinarySearch( -100, 1000, arrList.Count, this );
                Console.WriteLine( strTest+ "E_606: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
                Console.Out.WriteLine( ex.ToString() + " caught" );
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10005: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            try
            {
                arrList.BinarySearch( -100, 1000, "Batman", this );
                Console.WriteLine( strTest+ "E_707: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
                Console.Out.WriteLine( ex.ToString() + " caught" );
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
    public virtual int Compare(Object x, Object y)
    {
        return ((String)x).CompareTo( (String)y );
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co2460BinarySearch_iiOI oCbTest = new Co2460BinarySearch_iiOI();
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
