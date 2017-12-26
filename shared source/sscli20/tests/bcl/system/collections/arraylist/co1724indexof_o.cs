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
public class Co1724IndexOf_O
{
    public static String strName = "ArrayList.IndexOf( Object )";
    public static String strTest = "Co1724IndexOf_O";
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
            "Daniel Takacs",
            "Ironman",
            "Nightwing",
            "Robin",
            "SpiderMan",
            "Steel",
            "Gene",
            "Thor",
            "Wildcat",
            null
        };
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Normal IndexOf" );
            try
            {
                arrList = new ArrayList(strHeroes );
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    if ( arrList.IndexOf( strHeroes[i] ) != i )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_101a index of should have returned " + i.ToString() + " but returned " + arrList.IndexOf( strHeroes[i] ).ToString() );
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
            Console.Error.WriteLine( "[]  Check IndexOf when element is in list twice" );
            try
            {
                arrList = new ArrayList();
                arrList.Add( null );
                arrList.Add( arrList );
                arrList.Add( null );
                if ( arrList.IndexOf( null ) != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_102a index of should have returned " + 0.ToString() + " but returned " + arrList.IndexOf( null ).ToString() );
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_102c, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  check for something which does not exist in a list" );
            try
            {
                arrList = new ArrayList();
                if ( arrList.IndexOf( null ) != -1 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_103b, should have returned -1 but returned " + arrList.IndexOf( null ).ToString() );
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_103c, unexpected exception " + ex.ToString() );
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
        Co1724IndexOf_O oCbTest = new Co1724IndexOf_O();
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
