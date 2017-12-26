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
public class Co4310Add_Stress
{
    public Boolean runTest
        (
        int p_inLoops0  
        ,int p_inLoops1
        )
    {
        Console.Error.WriteLine( "Co4310Add_Stress  runTest(stress/perf) started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Hashtable ht2 = null;
        Hashtable ht = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sbl5 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        StringBuilder k1 = new StringBuilder(99);
        StringBuilder k2 = new StringBuilder(99);
        StringBuilder v1 = new StringBuilder(99);
        StringBuilder v2 = new StringBuilder(99);
        String str5 = null;
        String str6 = null;
        String str7 = null;
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
                strLoc="10cc";
                iCountTestcases++;
                k1 = new StringBuilder("key_0");
                k2 = new StringBuilder("key_1");
                v1 = new StringBuilder("val_0");
                v2 = new StringBuilder("val_1");
                ht = new Hashtable();
                ht.Add (k1, v1);
                ht.Add (k2, v2);
                strLoc="20cc";
                iCountTestcases++;
                if (!ht.ContainsKey (k2)) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_287!  "  );
                }
                strLoc="30cc";
                iCountTestcases++;
                if (!ht.ContainsValue (v2)) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_348!  "  );
                }
                strLoc="40cc";
                iCountTestcases++;
                try 
                {
                    ICollection allkeys = ht.Keys;
                    if (allkeys.Count != ht.Count) 
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error Err_892!  "  );
                    }
                    IEnumerator allkeysEnum = allkeys.GetEnumerator();
                    while ( allkeysEnum.MoveNext() )
                    {
                        String strTemp = allkeysEnum.Current.ToString();
                    }
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_2yi!  exc==" + exc  );
                }
                strLoc="50cc";
                iCountTestcases++;
                try 
                {
                    ICollection allvalues  = ht.Values; 
                    if (allvalues.Count != ht.Count) 
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error Err_892!  "  );
                    }
                    IEnumerator allvaluesEnum = allvalues.GetEnumerator();
                    while( allvaluesEnum.MoveNext() )
                    {
                        String strTemp = (String) allvaluesEnum.Current.ToString();
                    }
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_1io!  exc==" + exc  );
                }
                strLoc="100cc";
                ht2 = new Hashtable(); 
                strLoc="101cc";
                str5 = "key_150";   str6 = "value_150";
                ht2.Add( str5 ,str6 );
                in4a[0] = ht2.Count;
                ++iCountTestcases;
                if ( in4a[0] != 1 )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144cg!  in4a[0]==" + in4a[0]  );
                }
                str7 = (String)ht2[ str5 ];
                ++iCountTestcases;
                if ( str7.Equals( str6 ) == false )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184pr!  str7==" + str7  );
                }
                strLoc="102cc";
                str5 = "key_130";   str6 = "value_130"; 
                ht2.Add( str5 ,str6 );
                in4a[2] = ht2.Count;  
                ++iCountTestcases;
                if ( in4a[2] != 2 )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_105bo!  in4a[2]==" + in4a[2]  );
                }
                str7 = (String)ht2[ "key_150" ];
                ++iCountTestcases;
                if ( (str7 == null) || (str7.Equals( "value_150" ) == false ) )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_155tf!  str7==" + str7  );
                }
                str7 = (String)ht2[ str5 ];
                ++iCountTestcases;
                if ( (str7 == null) || (str7.Equals( str6 ) == false ) )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_166tf!  str7==" + str7  );
                }
                strLoc="107ex";
                try
                {
                    ht2.Add( str5 , str6 + "_b"  );  
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_366uf!  ht2.GetSize()==" + ht2.Count  );
                }
                catch ( ArgumentException )  
                {}
                catch ( Exception exc)  
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_466kf!  exc==" + exc  );
                }
                strLoc="108ex";
                str5 = null;   str6 = "value_null";
                try
                {
                    ht2.Add( str5 ,str6 );
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_726uw!  ht2.GetSize()==" + ht2.Count  );
                }
                catch ( ArgumentNullException )
                {}
                catch ( Exception exc )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_725yf!  exc==" + exc  );
                }
                strLoc="139ou";
                ht2 = new Hashtable();
                sbl3.Length =  0 ;
                sbl3.Append( "key_f3" );
                sbl4.Length =  0 ;
                sbl4.Append( "value_f3" );
                ht2.Add( sbl3 ,sbl4 );
                sbl4.Length =  0 ;
                sbl4.Append( "value_f4" );  
                sblWork1 = (StringBuilder)ht2[ sbl3 ];
                ++iCountTestcases;
                if ( sblWork1.ToString().Equals( sbl4.ToString() ) == false )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_498so!  sblWork1=="+ sblWork1  );
                }
                strLoc="141pi";
                ht2 = new Hashtable();
                sbl3.Length =  0 ;
                sbl3.Append( "key_m5" );
                sbl4.Length =  0 ;
                sbl4.Append( "value_m5" );
                ht2.Add( sbl3 ,sbl4 );
                sbl5 = new StringBuilder( "key_p7" ); 
                sbl4.Length =  0 ;
                sbl4.Append( "value_p7" );
                ht2.Add( sbl5 ,sbl4 );
                try
                {
                    strLoc="142ir";
                    sbl5.Length =  0 ; 
                    ++iCountTestcases;
                    if ( ht2.Count != 2 )
                    {
                        ++iCountErrors;
                        Console.Error.WriteLine(  "POINTTOBREAK: Error Err_798kz!  ht2.GetSize()=="+ ht2.Count  );
                    }
                    strLoc="142bs";
                    ++iCountTestcases;
                    sbl5.Append( "key_m5" ); 
                    try 
                    {
                        sblWork1 = (StringBuilder) ht2[sbl5]; 
                        strLoc="142bs.2";
                        Boolean bol2 = ht2.ContainsKey (sbl5); 
                    }
                    catch (Exception exc) 
                    {
                        Console.Error.WriteLine( "Find: Err_rh20! Exception caught  exc==" + exc );
                    }
                    strLoc="142qk";
                    strLoc="142cl";
                    ht2.Clear();
                }
                catch ( Exception exc )
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_180yl!  ?!?! what exc is expected??  exc==" + exc  );
                    Console.Error.WriteLine(  "EXTENDEDINFO: (Err_180yl)  strLoc=="+ strLoc +" ,sbl3=="+ sbl3  );
                }
                strLoc="200dd";
                in4TickCount[inTCIdx++] = Environment.TickCount;
                ht2 = new Hashtable();
                ++iCountTestcases;
                for ( int aa = 0 ;aa < p_inLoops0 ;aa++ )
                {
                    strLoc="202dd";
                    for ( int bb = 0 ;bb < p_inLoops1 ;bb++ )
                    {
                        ht2.Add(  "KEY: aa==" + aa + " ,bb==" + bb  ,"VALUE: aa==" + aa + " ,bb==" + bb  );
                    }
                    strLoc="206dd";
                    ht.Clear();
                }  
                in4TickCount[inTCIdx++] = Environment.TickCount;
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4310Add_Stress) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        Console.Error.WriteLine( "Info Inf_483xb.  Total Duration in Tick Counts==" + (in4TickCount[inTCIdx-1] - in4TickCount[0]) );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   Hashtable\\Co4310Add_Stress.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   Hashtable\\Co4310Add_Stress.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        int inLoops0 = -2;
        int inLoops1 = -2;
        Boolean bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4310Add_Stress cbA = new Co4310Add_Stress();
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
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4310Add_Stress) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4310Add_Stress.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
