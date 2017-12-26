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
public class Co4269IsLetter
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Character- Co4269IsLetter runTest started." );
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   int i2 = 0;
   char c2 = ' ';
   bool b2 = false;
   iCountTestcases++;
   i2 = 0x00; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 471q, 0x00." );
     }
   iCountTestcases++;
   i2 = 0x7F; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 637c, 0x7F." );
     }
   iCountTestcases++;
   i2 = 0xFF; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 297f, 0xFF." );
     }
   iCountTestcases++;
   i2 = 0x40; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 561o, 0x40." );
     }
   iCountTestcases++;
   i2 = 0x41; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 470y, 0x41." );
     }
   iCountTestcases++;
   i2 = 0x5A; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 618v, 0x5A." );
     }
   iCountTestcases++;
   i2 = 0x5B; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 178a, 0x5B." );
     }
   iCountTestcases++;
   i2 = 0x60; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 865w, 0x60." );
     }
   iCountTestcases++;
   i2 = 0x61; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 654b, 0x61." );
     }
   iCountTestcases++;
   i2 = 0x7A; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 522q, 0x7A." );
     }
   iCountTestcases++;
   i2 = 0x7B; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 323e, 0x7B." );
     }
   iCountTestcases++;
   i2 = 0x0E; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 629h, 0x0E." );
     }
   iCountTestcases++;
   i2 = 0x20; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 611g, 0x20." );
     }
   iCountTestcases++;
   i2 = 0x1234; c2 = (char)i2;
   b2 = Char.IsLetter( c2 );
   if ( !b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4269IsLetter Error at 445y, 0x1234." );
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.  ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!  ,iCountErrors=="+iCountErrors.ToString());
     return false;
     }
   }
 public static void Main( String[] args )
   {
   Console.Out.WriteLine( "Character- Co4269IsLetter main started." );
   bool bResult = false; 
   Co4269IsLetter o2Co4269IsLetter = new Co4269IsLetter();
   try
     {
     bResult = o2Co4269IsLetter.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4269IsLetter main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;  
   }
} 
