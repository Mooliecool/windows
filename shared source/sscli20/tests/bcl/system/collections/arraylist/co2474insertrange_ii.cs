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
public class Co2474InsertRange_iI
{
    public static String strName = "ArrayList.InsertRange";
    public static String strTest = "Co2474InsertRange_iI";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
        ArrayList arrInsert = null;
        int ii = 0;
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
        String [] strInsert =
            {
                "Dr. Fate",
                "Dr. Light",
                "Dr. Manhattan",
                "Hardware",
                "Hawkeye",
                "Icon",
                "Spawn",
                "Spectre",
                "Supergirl",
        };
        String [] strResult =
            {
                "Aquaman",
                "Atom",
                "Batman",
                "Dr. Fate",
                "Dr. Light",
                "Dr. Manhattan",
                "Hardware",
                "Hawkeye",
                "Icon",
                "Spawn",
                "Spectre",
                "Supergirl",
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
                arrList = new ArrayList( (ICollection) strHeroes );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_101: Failed to construct new ArrayList" );
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
            Console.Error.WriteLine( "[]  Construct insert array list" );
            try
            {
                arrInsert = new ArrayList( (ICollection) strInsert );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_202: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10002: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Insert collection into array list" );
            try
            {
                arrList.InsertRange( start, arrInsert );
                for ( ii = 0; ii < strResult.Length; ++ii )
                {
                    if ( strResult[ii].CompareTo( (String)arrList[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected Hero <"+ strResult[ii] + "> ";
                        strInfo = strInfo + "Returned Hero <"+ (String)arrList[ii] + "> ";
                        Console.WriteLine( strTest+ "E_303: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10003: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt bogus InsertRange using negative index" );
            try
            {
                arrList.InsertRange( -1000, arrInsert );
                Console.WriteLine( strTest+ "E_404: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt bogus InsertRange using out of range index" );
            try
            {
                arrList.InsertRange( 1000, arrInsert );
                Console.WriteLine( strTest+ "E_505: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10005: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt insertion of null collection" );
            try
            {
                arrInsert = new ArrayList( (ICollection) strInsert );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_606: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                arrList.InsertRange( start, null );
                Console.WriteLine( strTest+ "E_707: Expected ArgumentException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10006: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            arrList = new ArrayList();
            Queue que = new Queue();
            arrList.InsertRange(0, que);
            if(arrList.Count != 0)
            {
                iCountErrors++;
                Console.WriteLine("Err_742sf! incorrect value returned");
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
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "FAiL" );
            Console.Error.WriteLine( strTest + " iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co2474InsertRange_iI oCbTest = new Co2474InsertRange_iI();
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
