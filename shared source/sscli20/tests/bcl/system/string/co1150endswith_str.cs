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
public class Co1150EndsWith_Str
{
 public virtual bool runTest
   ()
   {
   System.Console.Error.WriteLine( "String.EndsWith: Co1150EW runTest starting..." );
   int nErrorBits = 0; 
   System.String swrString2 = null;
   swrString2 = "nOpqRs";
   if ( swrString2.EndsWith( "qRs" ) != true ) 
     nErrorBits = nErrorBits | 0x1;
   if ( swrString2.EndsWith( "qrs" ) != false )
     nErrorBits = nErrorBits | 0x2;
   if ( swrString2.EndsWith( "nOp" ) != false )
     nErrorBits = nErrorBits | 0x4;
   Char[] swrString3 = new Char[8];
   IntlStrings intl = new IntlStrings();
   swrString2 = intl.GetString(10, true, true);
   swrString2.CopyTo(2, swrString3, 0, swrString2.Length - 2);
   String swrString4 = new String(swrString3);
   if(swrString2.EndsWith(swrString4) != true) {
   nErrorBits = nErrorBits | 0x1;
   }
   System.Console.Error.WriteLine( nErrorBits  );
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
   System.Console.Error.WriteLine( "String.EndsWith: Co1150EW main starting..." );
   bool bResult = false; 
   Co1150EndsWith_Str oCo1150EWa = new Co1150EndsWith_Str();
   bResult = oCo1150EWa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "String.EndsWith: Co1150EW paSs." );
   else                 System.Console.Error.WriteLine( "String.EndsWith: Co1150EW FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
