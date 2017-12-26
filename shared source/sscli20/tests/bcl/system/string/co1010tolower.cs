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
using System.Globalization;
using GenStrings;
using System;
public class Co1010ToLower
{
 public virtual bool runTest
   ()
   {
   int nErrorBits = 0; 
   System.String swrString2 = null;
   System.String swrString4 = null;
   IntlStrings intl = new IntlStrings();
   swrString2 = intl.GetString(50, false, true);
   swrString4 = swrString2.ToLower();
   Console.WriteLine(swrString4);
   foreach(Char c in swrString4) {
   if(Char.GetUnicodeCategory( c ) == UnicodeCategory.UppercaseLetter) {
   nErrorBits = nErrorBits | 0x1;
   }
   }
   System.Console.Error.WriteLine(nErrorBits );
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
   Co1010ToLower oCo1010TLa = new Co1010ToLower();
   bResult = oCo1010TLa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "String.ToLower: Co1010TL paSs." );
   else                 System.Console.Error.WriteLine( "String.ToLower: Co1010TL FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
