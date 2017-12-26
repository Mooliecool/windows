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
public class Co4274IsLower
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Character- Co4274IsLower runTest started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int i2 = 0;
   char c2 = ' ';
   bool b2 = false;
   iCountTestcases++;
   i2 = 0x41; c2 = (char)i2;
   b2 = Char.IsLower( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4274IsLower Error at E_454fk!  0x41 'A'." );
     }
   iCountTestcases++;
   i2 = 0x61; c2 = (char)i2;
   b2 = Char.IsLower( c2 );
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4274IsLower Error at E_691al!  0x61 'a'." );
     }
   iCountTestcases++;
   i2 = 0x00; c2 = (char)i2;
   b2 = Char.IsLower( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4274IsLower Error at E_324kq!  0x00." );
     }
   iCountTestcases++;
   i2 = 0xFF; c2 = (char)i2;
   b2 = Char.IsLower( c2 );
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4274IsLower Error at E_448ij!  0xFF." );
     }
   iCountTestcases++;
   i2 = 0x30; c2 = (char)i2;
   b2 = Char.IsLower( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4274IsLower Error at E_867zb!  0x30 '0'." );
     }
   iCountTestcases++;
   i2 = 0x20; c2 = (char)i2;
   b2 = Char.IsLower( c2 );
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4274IsLower Error at E_860ob!  0x20 ' '." );
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. Co4274IsLower ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! Co4274IsLower ,iCountErrors=="+iCountErrors.ToString());
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co4274IsLower o2Co4274IsLower = new Co4274IsLower();
   try
     {
     bResult = o2Co4274IsLower.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4274IsLower main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;  
   }
} 
