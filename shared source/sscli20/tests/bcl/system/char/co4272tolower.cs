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
public class Co4272ToLower
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Character- Co4272ToLower runTest started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   char c2 = ' ';
   char c3 = ' ';
   char c9 = ' ';
   bool b2 = false;
   Char char2 = '\0';
   c2 = 'a';
   c9 = 'a';
   c3 = Char.ToLower( c2 );
   ++iCountTestcases;
   if ( ! (c3 == c9) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4272ToLower Error E_683db!" );
     }
   c2 = 'A';
   c9 = 'a';
   c3 = Char.ToLower( c2 );
   ++iCountTestcases;
   if ( ! (c3 == c9) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4272ToLower Error E_711am!" );
     }
   c2 = '?';
   c9 = '?';
   c3 = Char.ToLower( c2 );
   ++iCountTestcases;
   if ( ! (c3 == c9) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co4272ToLower Error E_910fg!" );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Character- Co4272ToLower: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4272ToLower iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Character- Co4272ToLower: FAiL!" );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     Co4272ToLower o2Co4272ToLower = new Co4272ToLower();
     bResult = o2Co4272ToLower.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4272ToLower main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;  
   }
} 
