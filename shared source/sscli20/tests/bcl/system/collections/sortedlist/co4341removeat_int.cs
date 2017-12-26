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
public class Co4341RemoveAt_int
    :
    IComparer
{
    internal static String strName = "SortedList.RemoveAt_int";
    internal static String strTest = "Co4341RemoveAt_int.cs";
    internal static String strPath = "";
    internal static String strActiveBugs =  "7616" ;
    public virtual int Compare  
        (
        Object p_obj1
        ,Object p_obj2
        )
    {
        return String.Compare( p_obj1.ToString() ,p_obj2.ToString() );
    }
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co4341Remove_int.cs  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        String str5 = null;
        String str6 = null;
        String str7 = null;
        String s1 = null;
        String s2 = null;
        String s3 = null;
        int[] in4a = new int[9];
        int nCapacity = 100;
        bool bol = false;
        int i = 0;
        try
        {
        LABEL_860_GENERAL:
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
                strLoc="Loc_141aa";
                for (i=0; i<100; i++) 
                {
                    ++iCountTestcases;
                    sblMsg.Length =  0 ;
                    sblMsg.Append("key_");
                    sblMsg.Append(i);
                    s1 = sblMsg.ToString();
                    sblMsg.Length =  0 ;
                    sblMsg.Append("val_");
                    sblMsg.Append(i);
                    s2 = sblMsg.ToString();
                    sl2.Add (s1, s2);
                }
                iCountTestcases++;
                if (sl2.Count  != 100) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_hbdw! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_ug2";
                for (i=0; i < 100; i++) 
                {
                    ++iCountTestcases;
                    sl2.RemoveAt((int) (99-i)); 
                    if (sl2.Count  != (100-(i+1))) 
                    {
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_hf49! - Count == " );
                        sblMsg.Append( sl2.Count  );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                    }
                }
                strLoc="Loc_u32";
                ++iCountTestcases;
                try 
                {
                    sl2.RemoveAt(-1); 
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_208d! - Remove shld have failed " );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                catch (ArgumentException aexc) 
                {
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_38db! - "  + exc.ToString() );
                }
                strLoc="Loc_uh3";
                ++iCountTestcases;
                try 
                {
                    sl2.RemoveAt(0); 
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_hd30! - Remove shld have failed " );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                catch (ArgumentException aexc) 
                {
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_38db! - "  + exc.ToString() );
                }
                strLoc="Loc_oiu43";
                ++iCountTestcases;
                try 
                {
                    sl2.RemoveAt(Int32.MaxValue);
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_cn40! - Remove shld have failed " );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                catch (ArgumentOutOfRangeException aexc) 
                {
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine( "POINTTOBREAK: Error Err_vh40! - "  + exc.ToString() +
                        "   strLoc == " + strLoc) ;
                }
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4341Remove_int.cs) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4341Remove_int.cs.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4341Remove_int.cs.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4341RemoveAt_int cbA = new Co4341RemoveAt_int();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4341Remove_int.cs) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
        {
            Console.Error.WriteLine(  "Co4341Remove_int.cs.cs   FAiL!"  );
            Console.WriteLine( "ACTIVE BUGS = " + strActiveBugs );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
