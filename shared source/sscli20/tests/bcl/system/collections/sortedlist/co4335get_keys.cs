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
public class Co4335get_Keys
{
    internal static String strName = "SortedList.GetKeys";
    internal static String strTest = "Co4335get_Keys";
    internal static String strPath = "";
    internal static String strActiveBugs =  "" ;
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co4335get_Keys  runTest() started." );
        SortedList sl = null;
        ICollection ic = null;
        IEnumerator ie = null;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        int iNumItems = 100;
        Console.WriteLine( "[] Get an ICollection to Values and make sure there is correct number of them" );
        try
        {
            ++iCountTestcases;
            sl = new SortedList();
            for ( int i = 0; i < iNumItems; i++ )
            {
                sl.Add( i + iNumItems, i );
            }
            ic = sl.Keys;
            if ( ic.Count != iNumItems )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001a, Count should be " + iNumItems.ToString() + " but is " + ic.Count.ToString() );
            }
        }
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001b, noexeception expected but threw " + ex.ToString() );
        }
        Console.WriteLine( "[] Get an ICollection to Values and make sure enumerator throws without MoveNext" );
        try
        {
            ++iCountTestcases;
            sl = new SortedList();
            for ( int i = 0; i < iNumItems; i++ )
            {
                sl.Add( i + iNumItems, i );
            }
            ic = sl.Keys;
            ie = (IEnumerator) ic.GetEnumerator();
            Object objThrowAway = ie.Current;	
            ++iCountErrors;
            Console.WriteLine( "Err_002b, InvalidOperationException expected but threw noexception" );
        }
        catch ( InvalidOperationException )
        {}
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002c, InvalidOperationException expected but threw " + ex.ToString() );
        }
        Console.WriteLine( "[] Get an ICollection to Values and make sure we can enumerate through the collection" );
        try
        {
            ++iCountTestcases;
            sl = new SortedList();
            for ( int i = 0; i < iNumItems; i++ )
            {
                sl.Add( i, i + iNumItems );
            }
            ic = sl.Keys;
            ie = (IEnumerator) ic.GetEnumerator();
            int iCounter = 0; 
            while( ie.MoveNext() )
            {
                Object o = ie.Current;
                if ( o == null )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_003a, element should not be null but it should be " + iCounter.ToString() );
                }
                else if ( ! o.Equals( iCounter ) )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_003b, element should not be " + o.ToString() + " but it should be " + iCounter.ToString() );
                }
                iCounter++;
            }
            if ( iCounter != iNumItems )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_003c, counter should be " + iNumItems.ToString() + " but is " + iCounter.ToString() );
            }
        }
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( "Err_003d, Noexception expected but threw " + ex.ToString() );
        }
        Console.WriteLine( "[] Get an ICollection to Values and get an enumerator to this ICollection, and make sure if we modify the base collection then Enumerator blows up (thows)" );
        try
        {
            ++iCountTestcases;
            sl = new SortedList();
            for ( int i = 0; i < iNumItems; i++ )
            {
                sl.Add( i + iNumItems, i );
            }
            ic = sl.Keys;
            ie = (IEnumerator) ic.GetEnumerator();
            sl.Add( Int32.MaxValue, iNumItems );
            ie.MoveNext();
            ++iCountErrors;
            Console.WriteLine( "Err_004a, InvalidOperationException expected but threw nothing" );
            Console.WriteLine( "          Collection has " + ic.Count.ToString() + " members but should have " + iNumItems.ToString() );
        }
        catch ( InvalidOperationException  )
        {}
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( "Err_004b, InvalidOperationException expected but threw " + ex.ToString() );
        }
        Console.WriteLine( "[] Get ICollection to Values make sure that CopyTo throws with incorrect arguments 1" );
        try
        {
            ++iCountTestcases;
            sl = new SortedList();
            for ( int i = 0; i < iNumItems; i++ )
            {
                sl.Add( i + iNumItems, i );
            }
            ic = sl.Keys;
            ic.CopyTo( null, 0 );
            ++iCountErrors;
            Console.WriteLine( "Err_005a, ArgumentNullException expected but threw nothing" );
        }
        catch ( ArgumentNullException  )
        {}
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( "Err_005b, ArgumentNullException expected but threw " + ex.ToString() );
        }
        Console.WriteLine( "[] Get ICollection to Values make sure that CopyTo throws with incorrect arguments 2" );
        try
        {
            ++iCountTestcases;
            sl = new SortedList();
            for ( int i = 0; i < iNumItems; i++ )
            {
                sl.Add( i + iNumItems, i );
            }
            ic = sl.Keys;
            Object [] obj = new Object[iNumItems];
            ic.CopyTo( obj, iNumItems );
            ++iCountErrors;
            Console.WriteLine( "Err_006a, ArgumentException expected but threw nothing" );
        }
        catch ( ArgumentException  )
        {}
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( "Err_006b, ArgumentOutOfRangeException expected but threw " + ex.ToString() );
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   SortedList\\Co4335get_Keys.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.WriteLine( "FAiL!   SortedList\\Co4335get_Keys.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        Co4335get_Keys cbA = new Co4335get_Keys();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine( "Err_9999,  Unhandeled Exception" + exc_main.ToString() );
        }
        if ( strActiveBugs.Length != 0 ) Console.WriteLine( "ACTIVE BUGS = " + strActiveBugs );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
