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
public class Co2494TrimToSize
{
    public static String strName = "ArrayList.TrimToSize";
    public static String strTest = "Co2494TrimToSize";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
        String [] strHeroes =
            {
                "Green Arrow",
                "Atom",
                "Batman",
                "Steel",
                "Superman",
                "Wonder Woman",
                "Hawkman",
                "Flash",
                "Aquaman",
                "Green Lantern",
                "Catwoman",
                "Huntress",
                "Robin",
                "Captain Atom",
                "Wildcat",
                "Nightwing",
                "Ironman",
                "SpiderMan",
                "Black Canary",
                "Thor",
                "Cyborg",
                "Captain America",
        };
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Construct ArrayList" );
            try
            {
                arrList = new ArrayList((ICollection) strHeroes );
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
            Console.Error.WriteLine( "[]  Test ascending TrimToSize" );
            try
            {
                arrList.Capacity =  2 * arrList.Count ;
                if ( arrList.Capacity <= arrList.Count )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected capacity <"+ 2 * arrList.Count + "> ";
                    strInfo = strInfo + "Returned capacity <"+ arrList.Capacity + "> ";
                    Console.WriteLine( strTest+ "E_202: " + strInfo );
                    ++iCountErrors;
                    break;
                }
                arrList.TrimToSize();
                if ( arrList.Capacity != arrList.Count )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected capacity <"+ 2 * arrList.Count + "> ";
                    strInfo = strInfo + "Returned capacity <"+ arrList.Capacity + "> ";
                    Console.WriteLine( strTest+ "E_202: " + strInfo );
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
        Co2494TrimToSize oCbTest = new Co2494TrimToSize();
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
