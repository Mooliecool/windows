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
public class Co4338GetEnumerator
    :
    IComparer
{
    internal static String strName = "SortedList.GetEnumerator";
    internal static String strTest = "Co4338GetEnumerator";
    internal static String strPath = "";
    public virtual int Compare  
        (
        Object p_obj1
        ,Object p_obj2
        )
    {
        int mrI4_1 = (Int32) p_obj1;
        int mrI4_2 = (Int32) p_obj2;
        if (mrI4_1 == mrI4_2)
            return 0;
        else
            return (mrI4_1 - mrI4_2);
    }
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co4338GetEnumerator  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        IDictionaryEnumerator dicen = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        int i3 = 0;
        int i2 = 0;
        int[] in4a = new int[9];
        int i = 0;
        int j = 0;
        try
        {
            do
            {
                strLoc="100cc";
                iCountTestcases++;
                sl2 = new SortedList( this );
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
                    sl2[null] = 0 ; 
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_2764! - must 've thrown ArgExc" );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                catch (ArgumentException) {}
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
                catch (InvalidOperationException aexc) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_34fj! - must not 've thrown ArgExc " + aexc.ToString() );
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
                    sl2[(Object)100] = 1 ; 
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
                strLoc = "Loc_02tgr";
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
                    ++iCountTestcases;
                    try 
                    {
                        Object o2 = sl2[(Object)(j)]; 
                        if (o2 != null)
                            i2 = (int) o2;
                        else 
                        {
                            ++iCountErrors;
                            sblMsg.Length =  0 ;
                            sblMsg.Append( "POINTTOBREAK: Error Err_9dsh! -Get (Object) failed at i == " );
                            sblMsg.Append( i );
                            Console.Error.WriteLine(  sblMsg.ToString()  );
                            break;
                        }
                    }
                    catch (Exception exc) 
                    {
                        Console.Error.WriteLine (exc);
                    }
                    strLoc = "Loc_t02hf";
                    i3 = (int)sl2.GetByIndex(i); 
                    ++iCountTestcases;
                    if ((i3 != i) || (i2 != i)) 
                    {
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_03dh! - i3 != i; at i == " );
                        sblMsg.Append( i );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                        break;
                    }
                }
                strLoc="Loc_ugj2";
                ++iCountTestcases;
                dicen = (IDictionaryEnumerator) sl2.GetEnumerator();
                ++iCountTestcases;
                try 
                {
                    Object throwaway = dicen.Current;
                    ++iCountErrors;
                    Console.WriteLine( "Err_001,  InvalidOperationException should have been thrown but it was not" );
                }
                catch (InvalidOperationException)
                {}
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_1127dh! - "  + exc.ToString() );
                }
                strLoc="Loc_f2aa";
                j = 0;
            while (dicen.MoveNext()) 
            {
                ++iCountTestcases;
                i3 = (int) dicen.Value;
                if (! j.Equals( i3 ) ) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_1128dh! - i3 != j; at j == " );
                    sblMsg.Append( j );
                    sblMsg.Append( "i3 == " + i3 );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                    break;
                }
                ++iCountTestcases;
                i3 = (int) dicen.Value;
                if (i3 != j) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_1109dh! - i3 != j; at j == " );
                    sblMsg.Append( j );
                    sblMsg.Append( "i3 == " + i3 );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                    break;
                }
                j++;
            }
                strLoc="Loc_oi21u";
                ++iCountTestcases;
                try 
                {
                    Object throwaway = dicen.Current;
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_001dt! - should have thrown InvalidOperationException" );
                }
                catch ( InvalidOperationException )
                {}
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_11gd29! - "  + exc.ToString() );
                }
                strLoc="Loc_783";
                ++iCountTestcases;
                if ((dicen.MoveNext()) || (j != 100)) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_11oh32! - dicen.MoveNext returned " );
                    sblMsg.Append( dicen.MoveNext() );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_1jo";
                ++iCountTestcases;
                if (dicen.MoveNext()) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_1130dh! - dicen.MoveNext returned " );
                    sblMsg.Append( dicen.MoveNext() );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
            } while ( false );
        }
        catch( Exception exc_general ) 
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4338GetEnumerator) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4338GetEnumerator.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4338GetEnumerator.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4338GetEnumerator cbA = new Co4338GetEnumerator();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4338GetEnumerator) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4338GetEnumerator.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
