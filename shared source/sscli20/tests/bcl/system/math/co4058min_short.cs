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
public class Co4058Min_short
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4058Min_short.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   short shortVal1 = 0;
   short shortVal2 = 0;
   short shexpectedValue = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       shortVal1 = (short)0x7FFE;  
       shortVal2 = (short)0x7FFF;  
       shexpectedValue = (short)0x7FFE;
       Console.WriteLine ("Testing: " + shortVal1 + " " + shortVal2 + " Expected: " + shexpectedValue);
       if ( Math.Min( shortVal1, shortVal2) != shexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected shortVal1 == " + shortVal1);
	 sblMsg.Append( " shortVal2 == " + shortVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       shortVal1 = (short)0x7FFF;  
       shortVal2 = (short)0x7FFE;  
       shexpectedValue = (short)0x7FFE;
       Console.WriteLine ("Testing: " + shortVal1 + " " + shortVal2 + " Expected: " + shexpectedValue);
       if ( Math.Min( shortVal1, shortVal2) != shexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Min value did not come up, Expected shortVal1 == " + shortVal1);
	 sblMsg.Append( "shortVal2 == " + shortVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       shortVal1 = unchecked((short)0x8001);  
       shortVal2 = unchecked((short)0x8000);  
       shexpectedValue = unchecked((short)0x8000);
       Console.WriteLine ("Testing: " + shortVal1 + " " + shortVal2 + " Expected: " + shexpectedValue);
       if ( Math.Min( shortVal1, shortVal2) != shexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected shortVal1 == " + shortVal1);
	 sblMsg.Append( "shortVal2 == " + shortVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       shortVal1 = unchecked((short)0x8000);  
       shortVal2 = unchecked((short)0x8001);  
       shexpectedValue = unchecked((short)0x8000);
       Console.WriteLine ("Testing: " + shortVal1 + " " + shortVal2 + " Expected: " + shexpectedValue);
       if ( Math.Min( shortVal1, shortVal2) != shexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ve73, Min value did not come up, Expected shortVal1 == " + shortVal1);
	 sblMsg.Append( "shortVal2 == " + shortVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       shortVal1 = (short)0x7FFF;  
       shortVal2 = unchecked((short)0x8000);  
       shexpectedValue = unchecked((short)0x8000);
       Console.WriteLine ("Testing: " + shortVal1 + " " + shortVal2 + " Expected: " + shexpectedValue);
       if ( Math.Min( shortVal1, shortVal2) != shexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_s03p, Min value did not come up, Expected shortVal1 == " + shortVal1);
	 sblMsg.Append( "shortVal2 == " + shortVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       shortVal1 = unchecked((short)0x8000);  
       shortVal2 = (short)0x7FFF;  
       shexpectedValue = unchecked((short)0x8000);
       Console.WriteLine ("Testing: " + shortVal1 + " " + shortVal2 + " Expected: " + shexpectedValue);
       if ( Math.Min( shortVal1, shortVal2) != shexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find f90k, Min value did not come up, Expected shortVal1 == " + shortVal1);
	 sblMsg.Append( "shortVal2 == " + shortVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       shortVal1 = (short)0;  
       shortVal2 = (short)0;  
       shexpectedValue = (short)0;
       Console.WriteLine ("Testing: " + shortVal1 + " " + shortVal2 + " Expected: " + shexpectedValue);
       if ( Math.Min( shortVal1, shortVal2) != shexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_67dq, Min value did not come up, Expected shortVal1 == " + shortVal1);
	 sblMsg.Append( "shortVal2 == " + shortVal2 );
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
     Console.Error.Write( "Math\\Co4058Min_short.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4058Min_short.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4058Min_short.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4058Min_short cbA = new Co4058Min_short();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4058Min_short.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
