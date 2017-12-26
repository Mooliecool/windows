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
public class Co4237IsDigit
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co4237IsDigit runTest started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int i2 = 0;
   char c2 = ' ';
   bool b2 = false;
   ++iCountTestcases;
   i2 = 0x00; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_h28");
   }
   ++iCountTestcases;
   i2 = 0x7F; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_u37");
   }
   ++iCountTestcases;
   i2 = 0xFF; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_dh3");
   }
   ++iCountTestcases;
   i2 = 0x41; c2 = (char)i2; 
   b2 = Char.IsDigit( c2 );
   if ( b2 == true ){
   ++iCountErrors;
   print("E_j3");
   }
   ++iCountTestcases;
   i2 = 0x61; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 == true ){
   ++iCountErrors;
   print("E_k213");
   }
   ++iCountTestcases;
   i2 = 0x4F; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_oi43");
   }
   ++iCountTestcases;
   i2 = 0x6C; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_f4h");
   }
   ++iCountTestcases;
   i2 = 0x05; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_d43");
   }
   ++iCountTestcases;
   i2 = 0x20; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_dh43");
   }
   ++iCountTestcases;
   i2 = 0x46; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 == true ) {
   ++iCountErrors;
   print("E_y327");
   }
   ++iCountTestcases;
   i2 = 0x66; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ) {
   ++iCountErrors;
   print("E_eh38");
   }
   ++iCountTestcases;
   i2 = 0x47; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_4u4");
   }
   ++iCountTestcases;
   i2 = 0x2F; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_r4i");
   }
   ++iCountTestcases;
   i2 = 0x30; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( ! b2 ){
   ++iCountErrors;
   print("E_3iu");
   }
   ++iCountTestcases;
   i2 = 0x39; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( ! b2 ){
   ++iCountErrors;
   print("E_eu3");
   }
   ++iCountTestcases;
   i2 = 0x3A; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_iu43");
   }
   ++iCountTestcases;
   i2 = 0x1234; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( b2 ){
   ++iCountErrors;
   print("E_f4k");
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Co4237IsDigit: paSs. iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4237IsDigit iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Co4237IsDigit: FAiL!" );
     return false;
     }
   }
 private void printexc(Exception exc)
   {
   String output = "EXTENDEDINFO: "+exc.ToString();
   Console.Error.WriteLine(output);
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Cb4201Put.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     Co4237IsDigit o2Co4237IsDigit = new Co4237IsDigit();
     bResult = o2Co4237IsDigit.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4237IsDigit main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
