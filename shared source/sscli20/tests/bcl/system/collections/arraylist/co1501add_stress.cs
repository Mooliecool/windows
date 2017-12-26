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
public class Co1501Add_Stress
{
    public virtual bool runTest
        (
        int p_inLoops0  
        ,int p_inLoops1
        )
    {
        Console.Error.WriteLine( "Co1501Add_Stress  runTest(stress/perf) started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        ArrayList al2 = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        int[] in4a = new int[9];
        int[] in4TickCount = new int[9];
        int inTCIdx = 0;
        try
        {
            do
            {
                in4TickCount[inTCIdx++] = Environment.TickCount;
                if ( p_inLoops0 < 2 )
                    p_inLoops0 = 2;
                if ( p_inLoops1 < 2 )
                    p_inLoops1 = 2;
                Console.Error.WriteLine( "Info Inf_201pa.  parms are: " + p_inLoops0 + " ," + p_inLoops1 );
                strLoc="100cc";
                al2 = new ArrayList( 1 );  
                in4a[0] = al2.Capacity;
                ++iCountTestcases;
                if ( in4a[0] != 1 )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144cg!  in4a[0]==" + in4a[0]  );
                }
                strLoc="101cc";
                sbl3.Length =  0 ;
                sbl3.Append( "hi mom" );
                al2.Add( sbl3 );
                sbl4 = (StringBuilder)al2[ 0 ];
                ++iCountTestcases;
                if ( sbl3.ToString().Equals( sbl4.ToString() ) == false )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184pr!  sbl4==" + sbl4.ToString()  );
                }
                strLoc="102cc";
                sbl3.Length =  0 ;
                sbl3.Append( "low dad" );
                al2.Add( sbl3 );
                in4a[1] = al2.Capacity;
                ++iCountTestcases;
                if ( in4a[1] <= 1 )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_127sh!  in4a[1]==" + in4a[1]  );
                }
                sbl3 = (StringBuilder)al2[ 1 ];
                ++iCountTestcases;
                if ( sbl3.ToString().Equals( sbl4.ToString() ) == false )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_155tf!  sbl3==" + sbl3.ToString()  );
                }
                strLoc="200dd";
                in4TickCount[inTCIdx++] = Environment.TickCount;
                al2 = new ArrayList();
                ++iCountTestcases;
                for ( int aa = 0 ;aa < p_inLoops0 ;aa++ )
                {
                    strLoc="202dd";
                    al2.Capacity =  1 ;
                    strLoc="204dd";
                    for ( int bb = 0 ;bb < p_inLoops1 ;bb++ )
                    {
                        al2.Add(  "aa==" + aa + " ,bb==" + bb  );
                    }
                    strLoc="206dd";
                    while ( al2.Count > 0 )
                    {
                        al2.RemoveAt( 0 );
                    }
                }  
                in4TickCount[inTCIdx++] = Environment.TickCount;
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1501Add_Stress) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        Console.Error.WriteLine( "Info Inf_483xb.  Total Duration in Tick Counts==" + (in4TickCount[inTCIdx-1] - in4TickCount[0]) );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   ArrayList\\Co1501Add_Stress.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   ArrayList\\Co1501Add_Stress.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        int inLoops0 = -2;
        int inLoops1 = -2;
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co1501Add_Stress cbA = new Co1501Add_Stress();
        if ( inLoops0 < 2 )
            inLoops0 = 2;
        if ( inLoops1 < 2 )
            inLoops1 = 2;
        try
        {
            bResult = cbA.runTest
                (
                inLoops0
                ,inLoops1
                );
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1501Add_Stress) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co1501Add_Stress.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
