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
using System.Globalization;
using GenStrings;
public class Co1030Compare_StrStrBool
{
 public Boolean runTest
   ()
   {
   Console.Error.WriteLine( "String-Co1030Compare_StrStrBool runTest started." );
   int nErrorBits = 0; 
   String swrString2 = null;
   String swrString3 = null;
   String swrString4 = null;
   swrString2 = "HIJ"; 
   swrString3 = "hij"; 
   if ( String.Compare( swrString2, swrString3 , true ) != 0 && CultureInfo.CurrentCulture.LCID != 0x41F){
   nErrorBits = nErrorBits | 0x1;
   }
   if ( String.Compare( swrString2, swrString3 , false ) <= 0 && CultureInfo.CurrentCulture.LCID != 0x41F){
   nErrorBits = nErrorBits | 0x2;
   }
   swrString2 = "efg"; 
   swrString3 = "hij"; 
   if ( String.Compare( swrString2, swrString3 , true ) >= 0 )
     nErrorBits = nErrorBits | 0x4;
   if ( String.Compare( swrString2, swrString3 , false) >= 0 )
     nErrorBits = nErrorBits | 0x8;
   swrString2 = "hij";
   swrString3 = "efg";
   if ( String.Compare( swrString2, swrString3 , true) <= 0 )
     nErrorBits = nErrorBits | 0x10;
   if ( String.Compare( swrString2, swrString3 ,  false) <= 0 )
     nErrorBits = nErrorBits | 0x20;
   swrString2 = "Foa";
   swrString3 = "fob";
   if ( String.Compare( swrString2, swrString3 ,  false) == 0 )
     nErrorBits = nErrorBits | 0x20;
   swrString2 = "Blah";
   swrString3 = "blab";
   if ( String.Compare( swrString2, swrString3 , true) == 0 )
     nErrorBits = nErrorBits | 0x40;
   if ( String.Compare( swrString2, swrString3 ,  false) == 0 )
     nErrorBits = nErrorBits | 0x80;
   swrString2 = "hij";
   swrString3 = "hij";
   if ( String.Compare(swrString2, swrString3 , true ) != 0 )
     nErrorBits = nErrorBits | 0x100;
   if ( String.Compare( swrString2, swrString3 , false ) != 0 )
     nErrorBits = nErrorBits | 0x200;
   swrString2 = "H"; 
   swrString3 = "q"; 
   swrString4 = "Q"; 
   if ( String.Compare( swrString3, swrString2, true ) < 0 ) 
     nErrorBits = nErrorBits | 0x400;
   if ( String.Compare( swrString3, swrString4 , false ) >= 0 ) 
     nErrorBits = nErrorBits | 0x800;
   if ( String.Compare(swrString3, swrString2 , true ) <= 0 ) 
     nErrorBits = nErrorBits | 0x1000;
   if ( String.Compare(swrString3, swrString4 , true) != 0 ) 
     nErrorBits = nErrorBits | 0x2000;
   long Var = 5;
   IntlStrings intl = new IntlStrings(Var);
   swrString2 = intl.GetString(6, true, true);
   swrString3 = swrString2;
   if ( String.Compare(swrString2, swrString3 , true ) != 0 )
     nErrorBits = nErrorBits | 0x4000;
   if ( String.Compare( swrString2, swrString3 , false ) != 0 )
     nErrorBits = nErrorBits | 0x8000;
   Console.Error.WriteLine(  nErrorBits );
   if (nErrorBits == 0) {
   return true; 
   }
   else {
   Console.WriteLine(nErrorBits);
   return false; 
   }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   Co1030Compare_StrStrBool oCo1030Ca = new Co1030Compare_StrStrBool();
   bResult = oCo1030Ca.runTest
     ();
   if (bResult == true) Console.Error.WriteLine( "Co1030C paSs." );
   else                 Console.Error.WriteLine( "Co1030C FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
