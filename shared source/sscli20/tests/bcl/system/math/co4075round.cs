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
using System.Globalization;
using System.Text;
using System;
public class Co4075Round
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4075Round.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double expectedVal = 0;
   String stVal = null;
   do
     {
     try
       {
       ++iCountTestcases;
       for ( int a = 0 ; a < 1000 ; a++ )
	 {
	 stVal = "0." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 if ( doubleVal1 <= 0.500 )
	   { expectedVal = 0.0; }
	 if ( doubleVal1 > 0.500 ) 
	   { expectedVal = 1.0; }
	 if ( Math.Round( doubleVal1 ) != expectedVal )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_0cm3, Did not round properly! in loop number == " );
	   Console.Error.WriteLine( a );
	   Console.Error.WriteLine( "; doubleVal1 == " + doubleVal1 + "; stVal == " + stVal);
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 0 ; a < 1000 ; a++ )
	 {
	 stVal = "1." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 if ( doubleVal1 < 1.500 )
	   { expectedVal = 1.0; }
	 if ( doubleVal1 >= 1.500 ) 
	   { expectedVal = 2.0; }
	 if ( Math.Round( doubleVal1 ) != expectedVal )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_42d6, Did not round properly! in loop number == " );
	   Console.Error.WriteLine( a );
	   Console.Error.WriteLine( "; doubleVal1 == " + doubleVal1 + "; stVal == " + stVal);
	   Console.Error.WriteLine( "; expectedVal == " + expectedVal + " Math.Round( doubleVal1 ) == " + Math.Round( doubleVal1 ));
	   }
	 }
       ++iCountTestcases;
       for ( int a = 0 ; a < 1000 ; a++ )
	 {
	 stVal = "-1." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 if ( doubleVal1 >= -1.500 )
	   { expectedVal = -1.0; }
	 if ( doubleVal1 <= -1.500 )
	   { expectedVal = -2.0; }
	 if ( Math.Round( doubleVal1 ) != expectedVal )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_9f3g, Did not round properly! in loop number == " );
	   Console.Error.WriteLine( a );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 0 ; a < 1000 ; a++ )
	 {
	 stVal = "2147483646." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 if ( doubleVal1 <= 2147483646.500 )
	   { expectedVal = 2147483646.0; }
	 if ( doubleVal1 > 2147483646.500 )
	   { expectedVal = 2147483647.0; }
	 if ( Math.Round( doubleVal1 ) != expectedVal )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_bg25 Did not round properly! in loop number == " );
	   Console.Error.WriteLine( a );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 0 ; a < 1000 ; a++ )
	 {
	 stVal = "-2147483646." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 if ( doubleVal1 >= -2147483646.500 )
	   { expectedVal = -2147483646.0; }
	 if ( doubleVal1 < -2147483646.500 )
	   { expectedVal = -2147483647.0; }
	 if ( Math.Round( doubleVal1 ) != expectedVal )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_6pr1, Did not round properly! in loop number == " );
	   Console.Error.WriteLine( a );
	   break;
	   }
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
     Console.Error.Write( "Math\\Co4075Round.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4075Round.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4075Round.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4075Round cbA = new Co4075Round();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4075Round.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
