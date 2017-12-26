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
public class Co4079Tan
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4079Tan.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double dubExpected = 0;
   double dubEquation = 0;
   double dubOne = 0;
   double dubTwo = 0;
   do
     {
     try
       {
       dubExpected = 0.0;  
       dubOne = ( dubExpected - 0.01 );
       dubTwo = ( dubExpected + 0.01 );
       dubEquation = Math.Tan( 0 );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )	
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_d903m, Tan not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.5773;  
       dubOne = ( dubExpected - 0.1 );
       dubTwo = ( dubExpected + 0.1 );
       dubEquation = Math.Tan( Math.PI/6 );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_c93n, Tan not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 1.0;  
       dubOne = ( dubExpected - 0.1 );
       dubTwo = ( dubExpected + 0.1 );
       dubEquation = Math.Tan( Math.PI/4 );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_xc85, Tan not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 1.7320;  
       dubOne = ( dubExpected - 0.1 );
       dubTwo = ( dubExpected + 0.1 );
       dubEquation = Math.Tan( Math.PI/3 );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_d8n3, Tan not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
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
     Console.Error.Write( "Math\\Co4079Tan.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4079Tan.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4079Tan.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4079Tan cbA = new Co4079Tan();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4079Tan.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
