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
public class Co1022get_Length
{
 public virtual bool runTest
   ()
   {
   int nErrorBits = 0; 
   int n2 = 0;
   System.String swrString2 = null;
   IntlStrings intl = new IntlStrings();
   swrString2 =  "" ;
   if ( swrString2.Length != 0x0)
     nErrorBits = nErrorBits | 0x1;
   swrString2 =  intl.GetString(6 , true, true);
   if ( swrString2.Length != 6)
     nErrorBits = nErrorBits | 0x2;
   if ( swrString2.Length != 6)
     nErrorBits = nErrorBits | 0x4;
   System.Console.Error.WriteLine( nErrorBits);
   if (nErrorBits == 0){
   return true; 
   }
   else {
   return false; 
   }
   }
 public static void Main( String[] args ) 
   {
   bool bResult = false; 
   Co1022get_Length oCo1022GLa = new Co1022get_Length();
   bResult = oCo1022GLa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "Co1022GL paSs." );
   else                 System.Console.Error.WriteLine( "Co1022GL FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
