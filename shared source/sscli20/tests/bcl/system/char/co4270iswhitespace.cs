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
public class Co4270IsWhiteSpace
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Character- Co4270IsWhiteSpace runTest started." );
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   char c2 = ' ';
   for ( short aa = 0 ;aa <= 255 ;aa++ )
     {
     iCountTestcases++;
     if(aa <=127)
       c2 = (char)aa;
     if ( Char.IsWhiteSpace( c2 ) == true )
       {
       if ( aa > 127 )
	 {
	 Console.Error.WriteLine();
	 Console.Error.WriteLine( " - - - - - (greater than 127, anything accepted) - - -" );
	 }
       Console.Error.WriteLine();
       Console.Error.WriteLine( "(894f) Whitespace for  aa=="+ aa +" ,c2=="+ c2 );
       if ( aa > 127 )
	 continue;
       if (
	   aa== 9
	   ||  aa==10
	   ||  aa==11
	   ||  aa==12
	   ||  aa==13
	   ||  aa==32
	   )
	 {
	 ;
	 }
       else
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "Error 448qs!  aa=="+ aa );
	 }
       }
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. Co4270IsWhiteSpace ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! Co4270IsWhiteSpace ,iCountErrors=="+iCountErrors.ToString());
     return false;
     }
   }
 public static void Main( String[] args )
   {
   Console.Out.WriteLine( "Character- Co4270IsWhiteSpace main started." );
   bool bResult = false; 
   Co4270IsWhiteSpace o2Co4270IsWhiteSpace = new Co4270IsWhiteSpace();
   try
     {
     bResult = o2Co4270IsWhiteSpace.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4270IsWhiteSpace main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;  
   }
} 
