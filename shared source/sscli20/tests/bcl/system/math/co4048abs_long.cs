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
public class Co4048Abs_long
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4048Abs_long .cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   long longVal;
   long longKnown;
   do
     {
     try
       {
       ++iCountTestcases;
       longVal = -( (long)9223372036854775807L );
       longKnown = 9223372036854775807L;
       if ( Math.Abs( longVal ) != longKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_b9wu, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected longKnown == " + longKnown );
	 sblMsg.Append( " intead got Math.Abs( longVal ) == " + Math.Abs( longVal ) );
	 break;
	 }
       ++iCountTestcases;
       longVal = -( 0 );
       longKnown = 0;
       if ( Math.Abs( longVal ) != longKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_pf71, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected longKnown == " + longKnown );
	 sblMsg.Append( " intead got Math.Abs( longVal ) == " + Math.Abs( longVal ) );
	 break;
	 }
       ++iCountTestcases;
       longVal = -( 1 );
       longKnown = 1;
       if ( Math.Abs( longVal ) != longKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_6mz2, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected longKnown == " + longKnown );
	 sblMsg.Append( " intead got Math.Abs( longVal ) == " + Math.Abs( longVal ) );
	 break;
	 }
       ++iCountTestcases;
       longVal = ( (long)9223372036854775807L );
       longKnown = 9223372036854775807L;
       if ( Math.Abs( longVal ) != longKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_ss81, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected longKnown == " + longKnown );
	 sblMsg.Append( " intead got Math.Abs( longVal ) == " + Math.Abs( longVal ) );
	 break;
	 }
       ++iCountTestcases;
       longVal = ( 0 );
       longKnown = 0;
       if ( Math.Abs( longVal ) != longKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_4f9v, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected longKnown == " + longKnown );
	 sblMsg.Append( " intead got Math.Abs( longVal ) == " + Math.Abs( longVal ) );
	 break;
	 }
       ++iCountTestcases;
       longVal = ( 1 );
       longKnown = 1;
       if ( Math.Abs( longVal ) != longKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_bbq11, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected longKnown == " + longKnown );
	 sblMsg.Append( " intead got Math.Abs( longVal ) == " + Math.Abs( longVal ) );
	 break;
	 }
       }
     catch ( Exception Exc )
       {
       ++iCountErrors;
       sblMsg.Length =  99 ;
       sblMsg.Append( "POINTTOBREAK: find E_f3h5, Generic Exception Caught, Exc.ToString() == " );
       sblMsg.Append( Exc.ToString() );
       Console.Error.WriteLine( sblMsg.ToString() );
       break;
       }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Math\\Co4048Abs_long .cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4048Abs_long .cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4048Abs_long .cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4048Abs_long  cbA = new Co4048Abs_long ();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4048Abs_long .cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
