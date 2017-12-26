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
using GenStrings ;
using System.Globalization;
using System;
public class Co1000ToUpper
{
 public virtual bool runTest
   ()
   {
   int nErrorBits = 0; 
   System.String swrString2 = null;
   System.String swrString3 = null;
   IntlStrings intl = new IntlStrings();
   swrString2 = intl.GetString(50, false, true);
   swrString3 = swrString2.ToUpper();
   foreach(Char c in swrString3)    {
   if ( Char.GetUnicodeCategory( c ) == UnicodeCategory.LowercaseLetter )
     nErrorBits = nErrorBits | 0x1;   
   } 
   System.Console.Error.WriteLine( nErrorBits);
   if (nErrorBits == 0)
     {
     return true;
     }
   else
     {
     return false; 
     }
   }
 public static void Main( String[] args ) 
   {
   bool bResult = false; 
   Co1000ToUpper oCo1000TUa = new Co1000ToUpper();
   bResult = oCo1000TUa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "String.ToUpper: Co1000TU paSs." );
   else                 System.Console.Error.WriteLine( "String.ToUpper: Co1000TU FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
