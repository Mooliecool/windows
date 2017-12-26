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
public class Co4275IsPunctuation
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Character- Co4275IsPunctuation runTest started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   char c2 = ' ';
   bool b2 = false;
   c2 = '!';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_111aa! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = ';';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_123bb! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = ';';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_145es! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = ',';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_222cc! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = '.';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_234zx! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = '?';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_268ut! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = '`';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_321qk! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = '-';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_849nd! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = ' ';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_369un! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x00;
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_416gb! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0xFF;
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_555ee! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x0D;
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_532xf! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x0A;
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_566jv! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = '\t';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_645ko! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = 'a';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_600ml! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = '@';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( !b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_777ff! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = '1';
   b2 = Char.IsPunctuation( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Cb1458IsPunctuation Error E_765ws! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Character- Co4275IsPunctuation: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4275IsPunctuation iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Character- Co4275IsPunctuation: FAiL!" );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     Co4275IsPunctuation o2Co4275IsPunctuation = new Co4275IsPunctuation();
     bResult = o2Co4275IsPunctuation.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4275IsPunctuation main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;  
   }
} 
