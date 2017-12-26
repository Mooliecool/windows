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
public class Co3906CopyTo_A
{
    public static String strName = "ArrayList.CopyTo( Array)";
    public static String strTest = "Co3906CopyTo_A";
    public static String strPath = "";
    public static String strActiveBugs = "";
    public Boolean runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        Console.WriteLine( "ACTIVE BUGS " + strActiveBugs );
        ArrayList arrList = null;
        String[] arrCopy = null;
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
            Console.Error.WriteLine( "[]  Normal Copy Test 1" );
            try
            {
                arrList = new ArrayList(strHeroes );
                arrCopy = new String[strHeroes.Length];
                arrList.CopyTo( arrCopy);
                for ( int i = 0; i < arrCopy.Length; i++ )
                {
                    if (   (arrCopy[i] == null && strHeroes[i] != null ) ||
                        (arrCopy[i] != null && ! arrCopy[i].Equals( strHeroes[i] ) )
                        )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_101a arrays do not match at position " + i.ToString() );
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
            Console.Error.WriteLine( "[]  Normal Copy Test 2 - copy 0 elements" );
            try
            {
                arrList = new ArrayList();
                arrList.Add( null );
                arrList.Add( arrList );
                arrList.Add( null );
                arrList.Remove( null );
                arrList.Remove( null );
                arrList.Remove( arrList );
                if ( arrList.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_102b, count should be zero but is " + arrList.Count.ToString() );
                }
                arrCopy = new String[strHeroes.Length];
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    arrCopy[i] = strHeroes[i];
                }
                arrList.CopyTo( arrCopy );
                for ( int i = 0; i < arrCopy.Length; i++ )
                {
                    if (   (arrCopy[i] == null && strHeroes[i] != null ) ||
                        (arrCopy[i] != null && ! arrCopy[i].Equals( strHeroes[i] ) )
                        )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_102a, arrays do not match at position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_102c, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            arrList = new ArrayList();
            if ( arrList.Count != 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_102b, count should be zero but is " + arrList.Count.ToString() );
            }
            arrCopy = new String[0];
            arrList.CopyTo( arrCopy );
            if ( arrCopy.Length != 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_630vfd, count should be zero but is " + arrCopy.Length );
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Copy so that exception should be thrown " );
            try
            {
                arrList = new ArrayList();
                if ( arrList.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_105b, count should be zero but is " + arrList.Count.ToString() );
                }
                arrCopy = null;
                arrList.CopyTo( arrCopy );
                ++iCountErrors;
                Console.WriteLine( "Err_649vdf! EXception not thrown" );
            }
            catch(ArgumentNullException)
            {
            }
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
        Co3906CopyTo_A oCbTest = new Co3906CopyTo_A();
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
