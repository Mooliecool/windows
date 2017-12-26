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
public class Co2470IndexOf_Oii
{
    public static String strName = "ArrayList.IndexOf";
    public static String strTest = "Co2470IndexOf_Oii";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
        int ndx = -1;
        String [] strHeroes =
            {
                "Aquaman",
                "Atom",
                "Batman",
                "Black Canary",
                "Captain America",
                "Captain Atom",
                "Batman",
                "Catwoman",
                "Cyborg",
                "Flash",
                "Green Arrow",
                "Batman",
                "Green Lantern",
                "Hawkman",
                "Huntress",
                "Ironman",
                "Nightwing",
                "Batman",
                "Robin",
                "SpiderMan",
                "Steel",
                "Superman",
                "Thor",
                "Batman",
                "Wildcat",
                "Wonder Woman",
                "Batman",
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
            Console.Error.WriteLine( "[]  Obtain index of \"Batman\" items" );
            try
            {
                while ( ( ndx = arrList.IndexOf( "Batman", ++ndx, (arrList.Count-ndx) ) ) != -1 )
                {
                    if ( strHeroes[ndx].CompareTo( (String)arrList[ndx] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "On IndexOf==" + ndx + " ";
                        strInfo = strInfo + "Expected hero <"+ strHeroes[ndx] + "> ";
                        strInfo = strInfo + "Returned hero <"+ (String)arrList[ndx] + "> ";
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
            Console.Error.WriteLine( "[]  Attempt to find null object" );
            try
            {
                ndx = arrList.IndexOf( null, 0, arrList.Count );
                if ( ndx != -1 )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected index <-1> ";
                    strInfo = strInfo + "Returned index <"+ ndx + "> ";
                    Console.WriteLine( strTest+ "E_303: " + strInfo );
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
            Console.Error.WriteLine( "[]  Attempt bogus IndexOf using negative index" );
            try
            {
                arrList.IndexOf( "Batman", -1000, arrList.Count );
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
            Console.Error.WriteLine( "[]  Attempt bogus IndexOf using out of range index" );
            try
            {
                arrList.IndexOf( "Batman", 1000, arrList.Count );
                if ( ndx != -1 )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected index <-1> ";
                    strInfo = strInfo + "Returned index <"+ ndx + "> ";
                    Console.WriteLine( strTest+ "E_505: " + strInfo );
                    ++iCountErrors;
                    break;
                }
            }
            catch (ArgumentOutOfRangeException)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10005: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt bogus IndexOf using endIndex greater than the size" );
            try
            {
                arrList.IndexOf( "Batman", 3, arrList.Count + 10 );
                Console.WriteLine( strTest+ "E_606: Expected ArgumentException" );
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
            for(int i=0; i<10; i++)
                arrList.Add(i);
            if(arrList.IndexOf(50, 0, arrList.Count) != -1)
            {
                ++iCountErrors;
                Console.WriteLine("Err_7342dvs! This is clearly wrong, ");
            }
            if(arrList.IndexOf(0, 1, arrList.Count-1) != -1)
            {
                ++iCountErrors;
                Console.WriteLine("Err_7342dvs! This is clearly wrong, ");
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
        Co2470IndexOf_Oii oCbTest = new Co2470IndexOf_Oii();
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
