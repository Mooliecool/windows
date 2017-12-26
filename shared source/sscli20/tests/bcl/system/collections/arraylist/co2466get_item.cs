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
public class Co2466get_Item
{
    public static String strName = "ArrayList.Get";
    public static String strTest = "Co2466get_Item";
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
            Console.Error.WriteLine( "[]  Verify get method" );
            try
            {
                for ( int ii = 0; ii < strHeroes.Length; ++ii )
                {
                    if ( ((String)arrList[ii] ).CompareTo( strHeroes[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected hero <"+ strHeroes[ii] + "> ";
                        strInfo = strInfo + "Returned hero <"+ (String)arrList[ii]  + "> ";
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
            Console.Error.WriteLine( "[]  Bogus Index" );
            try
            {
                String str = (String)arrList[(int)arrList.Count];
                Console.WriteLine( strTest+ "E_404: Expected ArgumentOutOfRangeException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentOutOfRangeException ex)
            {
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
    public virtual int Compare(Object x, Object y)
    {
        return ((String)x).CompareTo( (String)y );
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co2466get_Item oCbTest = new Co2466get_Item();
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
