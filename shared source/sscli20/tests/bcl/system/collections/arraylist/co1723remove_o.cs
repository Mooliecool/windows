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
public class Co1723Remove_O
{
    public static String strName = "ArrayList.Remove( Object )";
    public static String strTest = "Co1723Remove_O";
    public static String strPath = "";
    public static String strActiveBugs = "";
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
            Console.Error.WriteLine( "[] try removing each element make sure count goes down by one also" );
            try
            {
                arrList = new ArrayList(strHeroes );
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    arrList.Remove( strHeroes[i] );
                    if ( arrList.Count != strHeroes.Length - i - 1 )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_101a count has not be decremented by remove at loop position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_101c, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  make sure count goes back to 0" );
            try
            {
                arrList = new ArrayList();
                arrList.Add( null );
                arrList.Add( arrList );
                arrList.Add( null );
                arrList.Remove( arrList );
                arrList.Remove( null );
                arrList.Remove( null );
                if ( arrList.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_102b, count should be zero but is " + arrList.Count.ToString() );
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_102c, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  remove from empty list" );
            try
            {
                arrList = new ArrayList();
                arrList.Remove( null );
            }
            catch (ArgumentException)
            {}
            catch (Exception ex)
            {
                Console.WriteLine( "Err_104a, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  remove elemnt which does not exist should throw" );
            try
            {
                arrList = new ArrayList();
                arrList.Add( arrList );
                arrList.Remove( new ArrayList() );
            }
            catch (ArgumentException)
            {}
            catch (Exception ex)
            {
                Console.WriteLine( "Err_105a, unexpected exception " + ex.ToString() );
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
        Co1723Remove_O oCbTest = new Co1723Remove_O();
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
