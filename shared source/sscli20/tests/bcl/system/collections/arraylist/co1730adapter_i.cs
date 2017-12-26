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
public class Co1730Adapter_I
{
    public static String strName = "ArrayList.Adapter(IList)";
    public static String strTest = "Co1730Adapter";
    public static String strPath = "";
    public static String strActiveBugs = "";
    public Boolean verbose = false;
    public Boolean runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        ArrayList lAdapter = null;
        IEnumerator ienumList = null;
        IEnumerator ienumWrap = null;
        Console.WriteLine( strName + ": " + strTest + " runTest started..." );
        Console.WriteLine( "To run the test in verbose mode try /v" );
        if ( strActiveBugs.Length != 0 )
        {
            Console.WriteLine( "ACTIVE BUGS " + strActiveBugs );
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] check for ArgumentNullException when argument null" );
        try
        {
            lAdapter = ArrayList.Adapter( null );
        }
        catch ( ArgumentNullException )
        {}
        catch (Exception ex)
        {
            Console.WriteLine( "Err_001a, unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] make sure changes Through listAdapter show up in list" );
        try
        {
            ArrayList tempList = new ArrayList();
            for ( int i = 0; i < 10; i++ )
            {
                tempList.Add( i.ToString() + " from before" );
            }
            lAdapter = ArrayList.Adapter( tempList );
            lAdapter.Reverse(0, lAdapter.Count);
            int j = 9;
            for ( int i = 0; i < lAdapter.Count; i++ )
            {
                if ( ! lAdapter[i].Equals( j.ToString() + " from before" ) )
                {
                    Console.WriteLine( "Err_002b,  Element from listadapter '" + lAdapter[i] + "' should equal element from ilist '" + j.ToString() + " from before' but it does not" );
                    ++iCountErrors;
                }
                j--;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine( "Err_002a,  unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] make sure changes Through list show up in listAdapter" );
        try
        {
            ArrayList tempList = new ArrayList();
            for ( int i = 0; i < 10; i++ )
            {
                tempList.Add( i.ToString() + " from before" );
            }
            lAdapter = ArrayList.Adapter( tempList );
            tempList.Clear();
            if ( lAdapter.Count != 0 )
            {
                Console.WriteLine( "Err_003b,  listadapter does not have changes made to templist should have count 0 but has count " + lAdapter.Count );
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine( "Err_003a,  unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] test to see if enumerators are correctly enumerate through elements" );
        try
        {
            ArrayList tempList = new ArrayList();
            for ( int i = 0; i < 10; i++ )
            {
                tempList.Add( i.ToString() );
            }
            ienumList = tempList.GetEnumerator();
            lAdapter = ArrayList.Adapter( tempList );
            ienumWrap = tempList.GetEnumerator();
            int j = 0;
            while ( ienumList.MoveNext() )
            {
                if ( ! ienumList.Current.Equals( j.ToString() ) )
                {
                    Console.WriteLine( "Err_004a,  enumerator on list expected to return " + j + " but returned " + ienumList.Current );
                    ++iCountErrors;
                }
                j++;
            }
            j = 0;
            while ( ienumWrap.MoveNext() )
            {
                if ( ! ienumWrap.Current.Equals( j.ToString() ) )
                {
                    Console.WriteLine( "Err_004b,  enumerator on listadapter expected to return " + j + " but returned " + ienumWrap.Current );
                    ++iCountErrors;
                }
                j++;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine( "Err_004c,  unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] test to see if enumerators are correctly enumerate through elements" );
        try
        {
            ArrayList tempList = new ArrayList();
            for ( int i = 0; i < 10; i++ )
            {
                tempList.Add( i.ToString() );
            }
            ienumList = tempList.GetEnumerator();
            lAdapter = ArrayList.Adapter( tempList );
            ienumWrap = tempList.GetEnumerator();
            int j = 0;
            while ( ienumList.MoveNext() )
            {
                if ( ! ienumList.Current.Equals( j.ToString() ) )
                {
                    Console.WriteLine( "Err_004a,  enumerator on list expected to return " + j + " but returned " + ienumList.Current );
                    ++iCountErrors;
                }
                j++;
            }
            j = 0;
            while ( ienumWrap.MoveNext() )
            {
                if ( ! ienumWrap.Current.Equals( j.ToString() ) )
                {
                    Console.WriteLine( "Err_004b,  enumerator on listadapter expected to return " + j + " but returned " + ienumWrap.Current );
                    ++iCountErrors;
                }
                j++;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine( "Err_004c,  unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] test to see if enumerators are correctly getting invalidated with list modified through list" );
        try
        {
            ArrayList tempList = new ArrayList();
            for ( int i = 0; i < 10; i++ )
            {
                tempList.Add( i.ToString() );
            }
            ienumList = tempList.GetEnumerator();
            lAdapter = ArrayList.Adapter( tempList );
            ienumWrap = tempList.GetEnumerator();
            ienumList.MoveNext();
            ienumWrap.MoveNext();
            tempList.Add( "Hey this is new element" );
            try
            {
                ienumList.MoveNext();
                Console.WriteLine( "Err_006b, ienumList.MoveNext should have thrown since list was modified" );
                ++iCountErrors;
            }
            catch ( InvalidOperationException )
            {}
            try
            {
                ienumWrap.MoveNext();
                Console.WriteLine( "Err_006c, ienumWrap.MoveNext should have thrown since list was modified" );
                ++iCountErrors;
            }
            catch ( InvalidOperationException )
            {}
        }
        catch (Exception ex)
        {
            Console.WriteLine( "Err_006c,  unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] test to see if enumerators are correctly getting invalidated with list modified through listAdapter" );
        try
        {
            ArrayList tempList = new ArrayList();
            for ( int i = 0; i < 10; i++ )
            {
                tempList.Add( i.ToString() );
            }
            ienumList = tempList.GetEnumerator();
            lAdapter = ArrayList.Adapter( tempList );
            ienumWrap = tempList.GetEnumerator();
            ienumList.MoveNext();
            ienumWrap.MoveNext();
            lAdapter.Add( "Hey this is new element" );
            try
            {
                ienumList.MoveNext();
                Console.WriteLine( "Err_007b, ienumList.MoveNext should have thrown since list was modified" );
                ++iCountErrors;
            }
            catch ( InvalidOperationException )
            {}
            try
            {
                ienumWrap.MoveNext();
                Console.WriteLine( "Err_007c, ienumWrap.MoveNext should have thrown since list was modified" );
                ++iCountErrors;
            }
            catch ( InvalidOperationException )
            {}
        }
        catch (Exception ex)
        {
            Console.WriteLine( "Err_007c,  unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
        ++iCountTestcases;
        if ( verbose ) Console.Error.WriteLine( "[] to see if listadaptor modified using InsertRange works" );
        try
        {
            ArrayList tempList = new ArrayList();
            for ( int i = 0; i < 10; i++ )
            {
                tempList.Add( i.ToString() );
            }
            lAdapter = ArrayList.Adapter ( tempList );
            ArrayList tempListSecond = new ArrayList();
            for ( int i = 10; i < 20; i++ )
            {
                tempListSecond.Add( i.ToString() );
            }
            lAdapter.InsertRange( lAdapter.Count, tempListSecond );
            if ( lAdapter.Count != 20 )
            {
                Console.WriteLine( "Err_008a,  lAdapter should have had count 20 but had count " + lAdapter.Count );
                ++iCountErrors;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine( "Err_008b,  unexpected exception " + ex.ToString() );
            ++iCountErrors;
        }
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
        Co1730Adapter_I oCbTest = new Co1730Adapter_I();
        if ( args != null )
        {
            if ( Array.IndexOf( args, "/v" ) != -1 )
            {
                oCbTest.verbose = true;
            }
        }
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
