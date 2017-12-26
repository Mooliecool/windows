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
public class Co3934Sort
{
    public static String strName = "ArrayList.Sort";
    public static String strTest = "Co3934Sort";
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
        String [] strHeroesSorted =
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
            Console.Error.WriteLine( "[]  Sort array list using default comparer" );
            try
            {
                arrList = new ArrayList( (ICollection) strHeroes );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_404: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                arrList.Sort( );
                for ( int ii = 0; ii < arrList.Count; ++ii )
                {
                    if ( strHeroesSorted[ii].CompareTo( (String)arrList[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected index of hero <"+ strHeroesSorted[ii] + "> ";
                        strInfo = strInfo + "Returned index of hero <"+ (String)arrList[ii] + "> ";
                        Console.WriteLine( strTest+ "E_405: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            arrList = new ArrayList();
            try
            {
                arrList.Sort();
            }
            catch(Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine("Err_7842f! Unexpected exception thrown, " + ex);
            }			
            if(arrList.Count != 0)
            {
                ++iCountErrors;
                Console.WriteLine("Err_74523dsv! wrong value returned man. " + arrList.Count);
            }
            Int16 i16;
            Int32 i32;
            Int64 i64;
            UInt16 ui16;
            UInt32 ui32;
            UInt64 ui64;
            ArrayList alst;
            Boolean fPass;
            i16 = 1;
            i32 = 2;
            i64 = 3;
            ui16 = 4;
            ui32 = 5;
            ui64 = 6;
            alst = new ArrayList();
            alst.Add(i16);
            alst.Add(i32);
            alst.Add(i64);
            alst.Add(ui16);
            alst.Add(ui32);
            alst.Add(ui64);
            fPass = true;
            iCountTestcases++;
            try
            {
                alst.Sort();
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception ex)
            {
                fPass = false;
                Console.WriteLine(ex);
            }
            if(!fPass)
            {
                iCountErrors++;
                Console.WriteLine("Err_7423dsf! Unexpected value returned");
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
        Co3934Sort oCbTest = new Co3934Sort();
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
