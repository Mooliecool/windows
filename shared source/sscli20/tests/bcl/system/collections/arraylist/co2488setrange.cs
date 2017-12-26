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
public class Co2488SetRange
{
    public static String strName = "ArrayList.SetRange";
    public static String strTest = "Co2488SetRange";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
        ArrayList arrSetRange = null;
        int start = 3;
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
        String [] strSetRange =
            {
                "Hardware",
                "Icon",
                "Johnny Quest",
                "Captain Sisko",
                "Captain Picard",
                "Captain Kirk",
                "Agent J",
                "Agent K",
                "Space Ghost",
                "Wolverine",
                "Cyclops",
                "Storm",
                "Lone Ranger",
                "Tonto",
                "Supergirl",
        };
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Construct ArrayList" );
            try
            {
                arrList = new ArrayList( (ICollection) strHeroes );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_101: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                arrSetRange = new ArrayList( (ICollection) strSetRange );
                if ( arrSetRange == null )
                {
                    Console.WriteLine( strTest+ "E_102: Failed to construct new ArrayList arrSetRange" );
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
            Console.Error.WriteLine( "[]  Set range in the array list" );
            try
            {
                arrList.SetRange( start, arrSetRange );
                for ( int ii = 0; ii < arrSetRange.Count; ++ii )
                {
                    if ( ((String)arrList[start+ii]).CompareTo( (String)arrSetRange[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected hero <" + (String)arrList[start+ii] + "> ";
                        strInfo = strInfo + "Returned hero <"+ (String)arrSetRange[ii] + "> ";
                        Console.WriteLine( strTest+ "E_202: " + strInfo );
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
            Console.Error.WriteLine( "[]  Attempt bogus SetRange using collection that exceed valid range" );
            try
            {
                arrList.SetRange( start, arrList );
                Console.WriteLine( strTest+ "E_303: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10003: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt bogus SetRange using negative index" );
            try
            {
                arrList.SetRange( -100, arrSetRange );
                Console.WriteLine( strTest+ "E_303: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10003: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt SetRange using out of range index" );
            try
            {
                arrList[ 1000] = arrSetRange  ;
                Console.WriteLine( strTest+ "E_404: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException ex)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt SetRange using null value" );
            try
            {
                arrSetRange.SetRange( 0, null );
                Console.WriteLine( strTest+ "E_404: Expected ArgumentException" );
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
        Co2488SetRange oCbTest = new Co2488SetRange();
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
