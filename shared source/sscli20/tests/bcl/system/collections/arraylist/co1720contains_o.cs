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
public class Co1720Contains_O
{
    public static String strName = "ArrayList.Contains( Object )";
    public static String strTest = "Co1720Contains_O";
    public static String strPath = "";
    public static String strActiveBugs = "8281";
    public Boolean runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        Console.WriteLine( "ACTIVE BUGS " + strActiveBugs );
        ArrayList arrList = null;
        String [] strHeroes = new String[]
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
            null,
            "Ironman",
            "Nightwing",
            "Robin",
            "SpiderMan",
            "Steel",
            null,
            "Thor",
            "Wildcat",
            null
        };
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "Normal Contains" );
            try
            {
                arrList = new ArrayList(strHeroes );
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    if ( ! arrList.Contains( strHeroes[i] ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_101a, Contains returns false but shour return true at position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_101b, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Normal Contains which expects false" );
            try
            {
                arrList = new ArrayList(strHeroes );
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    for ( int j = 0; j < strHeroes.Length; j++ )
                    {
                        arrList.Remove( strHeroes[i] );
                    }
                    if ( arrList.Contains( strHeroes[i] ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_102a, Contains returns true but should return false at position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_102b, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Normal Contains on empty list" );
            try
            {
                arrList = new ArrayList();
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    if ( arrList.Contains( strHeroes[i] ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_103a, Contains returns true but should return false at position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_103b, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
        }
        while ( false );
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( strTest + " iCountTestcases==" + iCountTestcases.ToString() + " paSs" );
            return true;
        }
        else
        {
            Console.WriteLine( strTest + " FAiL");
            Console.Error.WriteLine( strTest + " iCountErrors==" + iCountErrors.ToString() );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        Boolean bResult = false;	
        Co1720Contains_O oCbTest = new Co1720Contains_O();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( strTest + " " + strPath );
            Console.WriteLine( strTest + " E_1000000" );
            Console.WriteLine( strTest + "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( strTest + " " + ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
} 
