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
using System.Text;
public class Co4045Abs_SByte
{
 public Boolean runTest()
   {
   Console.Out.WriteLine( "Co4045Abs_SByte .cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   SByte SByteVal;
   SByte SByteKnown;
   do
     {
     try
       {
       ++iCountTestcases;
       SByteVal = -( (SByte)0x79 ); 
       SByteKnown = (SByte)0x79;
       if ( Math.Abs( SByteVal ) != SByteKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_3nk4, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected SByteKnown == " + SByteKnown );
	 sblMsg.Append( " intead got Math.Abs( SByteVal ) == " + Math.Abs( SByteVal ) );
	 break;
	 }
       ++iCountTestcases;
       SByteVal = -( (SByte)0x00 );
       SByteKnown = 0;
       if ( Math.Abs( SByteVal ) != SByteKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_pf71, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected SByteKnown == " + SByteKnown );
	 sblMsg.Append( " intead got Math.Abs( SByteVal ) == " + Math.Abs( SByteVal ) );
	 break;
	 }
       ++iCountTestcases;
       SByteVal = -( (SByte)0x01 );
       SByteKnown = 1;
       if ( Math.Abs( SByteVal ) != SByteKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_6mz2, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected SByteKnown == " + SByteKnown );
	 sblMsg.Append( " intead got Math.Abs( SByteVal ) == " + Math.Abs( SByteVal ) );
	 break;
	 }
       ++iCountTestcases;
       SByteVal = ( (SByte)0x7F );  
       SByteKnown = 127;
       if ( Math.Abs( SByteVal ) != SByteKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_ss81, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected SByteKnown == " + SByteKnown );
	 sblMsg.Append( " intead got Math.Abs( SByteVal ) == " + Math.Abs( SByteVal ) );
	 break;
	 }
       ++iCountTestcases;
       SByteVal = ( (SByte)0x00 );
       SByteKnown = 0;
       if ( Math.Abs( SByteVal ) != SByteKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_4f9v, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected SByteKnown == " + SByteKnown );
	 sblMsg.Append( " intead got Math.Abs( SByteVal ) == " + Math.Abs( SByteVal ) );
	 break;
	 }
       ++iCountTestcases;
       SByteVal = ( (SByte)0x01 );
       SByteKnown = 1;
       if ( Math.Abs( SByteVal ) != SByteKnown )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_bbq11, ABS wrong! " );
	 sblMsg.Append( "EXTENDEDINFO: Expected SByteKnown == " + SByteKnown );
	 sblMsg.Append( " intead got Math.Abs( SByteVal ) == " + Math.Abs( SByteVal ) );
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
     Console.Error.Write( "Math\\Co4045Abs_SByte .cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4045Abs_SByte .cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4045Abs_SByte .cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co4045Abs_SByte  cbA = new Co4045Abs_SByte ();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4045Abs_SByte .cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
