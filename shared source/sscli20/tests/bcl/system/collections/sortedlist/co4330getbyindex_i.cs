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
public class Co4330GetByIndex_i
    :
    IComparer
{
    internal static String strName = "SortedList.GetByIndex_i";
    internal static String strTest = "Co4330GetByIndex_i";
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
        Console.Error.WriteLine( "Co4330GetByIndex_i  runTest() started." );
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
        String s4 = null;
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
                strLoc="Loc_110aa";
                ++iCountTestcases;
                try 
                {
                    s3 = (String) sl2.GetByIndex(-1);	
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_23dg! - ArgExc expected..."  );
                }
                catch (ArgumentException aexc) {}
                ++iCountTestcases;
                try 
                {
                    s3 = (String) sl2.GetByIndex(0) ;	
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_23dg! - ArgExc expected..."  );
                }
                catch (ArgumentException aexc) {}
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
                strLoc="Loc_151aa";
                for (i=0; i < sl2.Count ; i++) 
                {
                    sblMsg.Length =  0 ;
                    sblMsg.Append("key_"); 
                    sblMsg.Append(i);
                    s1 = sblMsg.ToString();
                    sblMsg.Length =  0 ;
                    sblMsg.Append("val_"); 
                    sblMsg.Append(i);
                    s2 = sblMsg.ToString();
                    ++iCountTestcases;
                    s3 = (String) sl2.GetByIndex(sl2.IndexOfKey(s1)) ; 
                    s4 = (String) sl2.GetByIndex(sl2.IndexOfValue(s2)); 
                    strLoc="Loc_161aa";
                    if (!s3.Equals (s4)) 
                    { 
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_1032bc! - s4 == " );
                        sblMsg.Append( s4 );
                        sblMsg.Append( "; s3 == " );
                        sblMsg.Append( s3 );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                    }
                    strLoc="Loc_171aa";
                    ++iCountTestcases;
                    s3 = (String) sl2.GetByIndex(sl2.IndexOfKey(s1)) ;
                    if (!s3.Equals (s2)) 
                    {
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_1032.2bc! - s2 == " );
                        sblMsg.Append( s2 );
                        sblMsg.Append( "; s3 == " );
                        sblMsg.Append( s3 );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                    }
                }
                ++iCountTestcases;
                sblMsg.Length =  0 ;
                sblMsg.Append("key_50");
                s1 = sblMsg.ToString();
                sl2.Remove (s1); 
                strLoc="Loc_181aa";
                ++iCountTestcases;
                int j = sl2.IndexOfKey(s1);
                if (j != -1) 
                { 
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_39fd! - should return -1..."  );
                    Console.Error.WriteLine( "returned: " + j );
                }
                strLoc="Loc_191aa";
                ++iCountTestcases;
                try 
                {
                    s3 = (String) sl2.GetByIndex(j) ;	
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_3k34! - ArgExc expected..."  );
                }
                catch (ArgumentException aexc) {}
                strLoc="Loc_191.1aa";
                ++iCountTestcases;
                try 
                {
                    s3 = (String) sl2.GetByIndex(sl2.Count) ;	
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_3k34.2! - ArgExc expected..."  );
                }
                catch (ArgumentException aexc) {}
                strLoc="Loc_191.2aa";
                ++iCountTestcases;
                try 
                {
                    s3 = (String) sl2.GetByIndex(sl2.Count +1) ;	
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_3k34.3! - ArgExc expected..."  );
                }
                catch (ArgumentException aexc) {}
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4330GetByIndex_i) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4330GetByIndex_i.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4330GetByIndex_i.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4330GetByIndex_i cbA = new Co4330GetByIndex_i();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4330GetByIndex_i) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4330GetByIndex_i.cs   FAiL!"  );
        Console.WriteLine( "ACTIVE BUGS = " + strActiveBugs );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
