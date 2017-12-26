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
public class Co5151Equals_str_str
{
 public virtual bool runTest
   ()
   {
   Console.Error.WriteLine( "String.Equals: Cb1100E runTest running:" );
   int nErrorBits = 0; 
   String swrString1 = null;
   String swrString2 = null;
   String swrString3 = null;
   String swrString4 = null;
   String swrString5 = null;
   String swrString6 = null;
   swrString2 = "nOpqrs"; 
   swrString3 = "nopqrs"; 
   swrString4 = "Dill Guv Dill Guv Dill"; 
   swrString5 = "Dill Guv Dill Guv Dill Guv Dill"; 
   swrString6 = "Y2"; 
   swrString1 = swrString2; 
   if ( String.Equals( swrString1, swrString2 ) != true ) 
     nErrorBits = nErrorBits | 0x1;
   if ( String.Equals( swrString1, swrString3 ) != false ) 
     nErrorBits = nErrorBits | 0x2;
   IntlStrings intl = new IntlStrings();
   String str1 = intl.GetString(3, true, true);
   String str2 =  str1;
   if(!str1.Equals(str2))
     {
     nErrorBits = nErrorBits | 0x20;
     }
   swrString1 = swrString4; 
   if ( String.Equals( swrString1, swrString5 ) != false ) 
     nErrorBits = nErrorBits | 0x4; 
   if ( String.Equals( swrString1, swrString6 ) != false ) 
     nErrorBits = nErrorBits | 0x8;
   swrString1 = swrString5; 
   if ( String.Equals( swrString1, swrString4 ) != false ) 
     nErrorBits = nErrorBits | 0x10;
   Console.Error.Write( "String.Equals: Cb1100E nErrorBits==" );
   Console.Error.WriteLine( nErrorBits  );
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
   Console.Error.WriteLine( "String.Equals: Cb1100E main running:" );
   bool bResult = false; 
   Co5151Equals_str_str oCb1100Ea = new Co5151Equals_str_str();
   bResult = oCb1100Ea.runTest
     ();
   if (bResult == true) Console.Error.WriteLine( "String.Equals: Cb1100E paSs." );
   else                 Console.Error.WriteLine( "String.Equals: Cb1100E FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
