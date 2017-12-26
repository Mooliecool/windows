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
public class Co4276IsISOControl
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Character- Co4276IsISOControl runTest started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   char c2 = ' ';
   bool b2 = false;
   for ( short aa=0x00 ;aa<0x20 ;aa++ )
     {
     c2 = (char)aa;
     b2 = Char.IsControl( c2 );
     ++iCountTestcases;
     if ( ! b2 )
       {
       iCountErrors += 1;
       Console.Error.Write( "Co4276IsISOControl Error E_746po!  aa==" );
       Console.Error.WriteLine( aa );
       }
     } 
   c2 = (char)0x0020;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_400ki! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x007F;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_482sn! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x0080;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( ! b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_386fk! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x00FF;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_312zu! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x7FFF;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_204al! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x8000;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_222ms! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0xFFFA;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_161sl! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0xFFFE;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_003me! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0xFFFF;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_084fr! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   c2 = (char)0x007E;
   b2 = Char.IsControl( c2 );
   ++iCountTestcases;
   if ( b2 )
     {
     iCountErrors += 1;
     Console.Error.Write( "Co4276IsControl Error E_482sn! " );
     Console.Error.WriteLine( iCountTestcases );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Character- Co4276IsControl: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4276IsControl iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Character- Co4276IsControl: FAiL!" );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     Co4276IsISOControl o2Co4276IsControl = new Co4276IsISOControl();
     bResult = o2Co4276IsControl.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4276IsControl main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;  
   }
} 
