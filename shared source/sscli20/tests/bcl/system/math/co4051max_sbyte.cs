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
public class Co4051Max_SByte
{
 public bool runTest()
   {
   Console.Out.WriteLine( "Co4051Max_SByte.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   SByte byteVal1 = 0;
   SByte byteVal2 = 0;
   SByte byexpectedValue = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       byteVal1 = (SByte)0x7E;  
       byteVal2 = (SByte)0x7F;  
       byexpectedValue = (SByte)0x7F;
       Console.WriteLine ("Testing: " + byteVal1 + " " + byteVal2 + " Expected: " + byexpectedValue);
       if ( Math.Max( byteVal1, byteVal2) != byexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Max value did not come up, Expected byteVal1 == " + byteVal1);
	 sblMsg.Append( "byteVal2 == " + byteVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       byteVal1 = (SByte)0x7F;  
       byteVal2 = (SByte)0x7E;  
       byexpectedValue = (SByte)0x7F;
       Console.WriteLine ("Testing: " + byteVal1 + " " + byteVal2 + " Expected: " + byexpectedValue);
       if ( Math.Max( byteVal1, byteVal2) != byexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Max value did not come up, Expected byteVal1 == " + byteVal1);
	 sblMsg.Append( "byteVal2 == " + byteVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       unchecked {
       byteVal1 = (SByte)0x80;  
       byteVal2 = (SByte)0x81;  
       byexpectedValue = (SByte)0x81;
       }
       Console.WriteLine ("Testing: " + byteVal1 + " " + byteVal2 + " Expected: " + byexpectedValue);
       if ( Math.Max( byteVal1, byteVal2) != byexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Max value did not come up, Expected byteVal1 == " + byteVal1);
	 sblMsg.Append( "byteVal2 == " + byteVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       unchecked {
       byteVal1 = (SByte)0x80;  
       byteVal2 = (SByte)0x79;  
       } 
       byexpectedValue = (SByte)0x79;
       Console.WriteLine ("Testing: " + byteVal1 + " " + byteVal2 + " Expected: " + byexpectedValue);
       if ( Math.Max( byteVal1, byteVal2) != byexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Max value did not come up, Expected byteVal1 == " + byteVal1);
	 sblMsg.Append( "byteVal2 == " + byteVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       byteVal1 = (SByte)0x00;  
       byteVal2 = (SByte)0x00;  
       byexpectedValue = (SByte)0x00;
       Console.WriteLine ("Testing: " + byteVal1 + " " + byteVal2 + " Expected: " + byexpectedValue);
       if ( Math.Max( byteVal1, byteVal2) != byexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_67dq, Max value did not come up, Expected byteVal1 == " + byteVal1);
	 sblMsg.Append( "byteVal2 == " + byteVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       try
	 {
	 ++iCountTestcases;
	 unchecked {
	 byteVal1 = (SByte)0x81;
	 byteVal2 = (SByte)0x81;
	 byexpectedValue = (SByte)0x81;
	 Console.WriteLine ("Testing: " + byteVal1 + " " + byteVal2 + " Expected: " + byexpectedValue);
	 }
	 if ( Math.Max( byteVal1, byteVal2) != byexpectedValue ){}
	 }
       catch ( IndexOutOfRangeException ){
       Console.WriteLine ("INFO_345gf: Caught exception");}
       catch ( Exception Exc )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_v91k, General Exception Thrown, Exc.ToString() == " );
	 sblMsg.Append( Exc.ToString() );
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
     Console.Error.Write( "Math\\Co4051Max_SByte.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4051Max_SByte.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4051Max_SByte.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4051Max_SByte cbA = new Co4051Max_SByte();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4051Max_SByte.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
