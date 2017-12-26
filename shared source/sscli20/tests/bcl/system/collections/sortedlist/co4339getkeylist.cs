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
using System.Text;
using System;
using System.Collections;
public class Co4339GetKeyList
{
    internal static String strName = "SortedList.GetKeyList";
    internal static String strTest = "Co4339GetKeyList";
    internal static String strPath = "";
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co4339GetKeyList  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        IEnumerator en = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        int i3 = 0;
        int[] in4a = new int[9];
        int i = 0;
        int j = 0;
        try
        {
            do
            {
                strLoc="100cc";
                iCountTestcases++;
                sl2 = new SortedList(); 
                iCountTestcases++;
                if ( sl2 == null )
                {
                    Console.WriteLine( strTest+ "E_101" );
                    Console.WriteLine( strTest+ "SortedList creation failure" );
                    ++iCountErrors;
                    break;
                }
                iCountTestcases++;
                if ( sl2.Count  != 0 )
                {
                    Console.WriteLine( strTest+ "E_102" );
                    Console.WriteLine( strTest+ "New SortedList is not empty" );
                    ++iCountErrors;
                }
                strLoc="Loc_100aa";
                ++iCountTestcases;
                try 
                {
                    sl2[null] = 0;
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_2764! - must 've thrown ArgExc" );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                catch (ArgumentNullException ) {}
                catch (Exception ex )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001dt, ArgumentNullException expected but got " + ex.ToString() );
                }
                ++iCountTestcases;
                if (sl2.Count  != 0) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_48ff! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_110aa";
                ++iCountTestcases;
                try 
                {
                    sl2[(Object)100] = (Object)null ;
                }
                catch (ArgumentException) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_34fj! - must not 've thrown ArgExc" );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                ++iCountTestcases;
                if (sl2.Count  != 1) 
                { 
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_60ff! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_120aa";
                ++iCountTestcases;
                try 
                {
                    sl2[(Object)100] = 1;
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine ( "POINTTOBREAK: Error Err_49ff! - " + exc.ToString());
                }
                if (sl2.Count  != 1) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_2i20bc! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                sl2.Clear();
                if (sl2.Count  != 0) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_03rf! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_141aa";
                for (i=0; i<100; i++) 
                {
                    ++iCountTestcases;
                    sl2.Add (i+100, i);
                }
                if (sl2.Count  != 100) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_dhc3! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                for (i=0; i<100; i++) 
                {
                    ++iCountTestcases;
                    j = i+100;
                    if (!sl2.ContainsKey ((int)j)) 
                    { 
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_0dj! - ContainsKey failed at i == " );
                        sblMsg.Append( i );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                        break;
                    }
                    ++iCountTestcases;
                    if (!sl2.ContainsValue (i)) 
                    {
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_hd3! -ContainsValue failed at  i == " );
                        sblMsg.Append( i );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                        break;
                    }
                }
                strLoc="Loc_346aws";
                ++iCountTestcases;
                en = (IEnumerator) sl2.GetKeyList().GetEnumerator();
                ++iCountTestcases;
                try 
                {
                    Object objThrowAway = en.Current;
                    ++iCountErrors;
                    Console.WriteLine( "Err_002dt, Expected InvalidOperationException to be thrown but nothing was thrown" );
                }
                catch (InvalidOperationException) {}
                catch (Exception exc )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_27dh! - "  + exc.ToString() );
                }
                j = 100;
                en = (IEnumerator) sl2.GetKeyList().GetEnumerator();
            while (en.MoveNext()) 
            {
                ++iCountTestcases;
                i3 = (int) en.Current;
                if (i3 != j) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_28dh! - i3 != j; at j == " );
                    sblMsg.Append( j );
                    sblMsg.Append( "i3 == " + i3 );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                    break;
                }
                ++iCountTestcases;
                i3 = (int) en.Current;
                if (i3 != j) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_09dh! - i3 != j; at j == " );
                    sblMsg.Append( j );
                    sblMsg.Append( "i3 == " + i3 );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                    break;
                }
                j++;
            }
                ++iCountTestcases;
                try 
                {
                    Object objThrowAway = en.Current;
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_001dt! - InvalidOperationException expected");
                }
                catch ( InvalidOperationException )
                {}
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_gd29! - "  + exc.ToString() );
                }
                strLoc="Loc_23aer";
                ++iCountTestcases;
                if ((en.MoveNext()) || (j != 200)) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_oh32! - en.MoveNext returned " );
                    sblMsg.Append( en.MoveNext() );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                ++iCountTestcases;
                if (en.MoveNext()) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_30dh! - en.MoveNext returned " );
                    sblMsg.Append( en.MoveNext() );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
            } while ( false );
        }
        catch( Exception exc_general ) 
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4339GetKeyList) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4339GetKeyList.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4339GetKeyList.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4339GetKeyList cbA = new Co4339GetKeyList();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4339GetKeyList) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4339GetKeyList.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
