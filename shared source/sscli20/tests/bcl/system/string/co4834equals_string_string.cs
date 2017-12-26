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
public class Co4834Equals_String_String
{
 public virtual bool runTest
   ()
   {
   System.Console.Error.WriteLine( "String.Equals: Co1100E runTest running:" );
   int nErrorBits = 0; 
   System.String swrString1 = null;
   System.String swrString2 = null;
   System.String swrString3 = null;
   System.String swrString4 = null;
   System.String swrString5 = null;
   System.String swrString6 = null;
   swrString2 = "nOpqrs"; 
   swrString3 = "nopqrs"; 
   swrString4 = "Dill Guv Dill Guv Dill"; 
   swrString5 = "Dill Guv Dill Guv Dill Guv Dill"; 
   swrString6 = "Y2"; 
   swrString1 = swrString2; 
   if ( String.Equals( swrString1, swrString2 ) != true ) 
     nErrorBits = nErrorBits | 0x1;
   IntlStrings intl = new IntlStrings();
   String str1 = intl.GetString(20, false, true);
   String str2 = str1;
   if(String.Equals(str1, str2) != true)
     nErrorBits = nErrorBits | 0x20;
   if ( String.Equals( swrString1, swrString3 ) != false ) 
     nErrorBits = nErrorBits | 0x2;
   swrString1 = swrString4; 
   if ( String.Equals ( swrString1, swrString5 ) != false ) 
     nErrorBits = nErrorBits | 0x4; 
   if ( String.Equals( swrString1, swrString6 ) != false ) 
     nErrorBits = nErrorBits | 0x8;
   swrString1 = swrString5; 
   if ( String.Equals( swrString1, swrString4 ) != false ) 
     nErrorBits = nErrorBits | 0x10;
   System.Console.Error.Write( "String.Equals: Co1100E nErrorBits==" );
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
   System.Console.Error.WriteLine( "String.Equals: Co1100E main running:" );
   bool bResult = false; 
   Co4834Equals_String_String oCo1100Ea = new Co4834Equals_String_String();
   bResult = oCo1100Ea.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "String.Equals: Co1100E paSs." );
   else                 System.Console.Error.WriteLine( "String.Equals: Co1100E FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 1;
   }
} 
