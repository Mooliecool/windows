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
public class Co1090IndexOf_CharInt
{
 public virtual bool runTest
   ()
   {
   System.Console.Error.Write( "String.IndexOf: Co1090IO runTest starting..." );
   int iCountTestcases = 0 ;
   int nErrorBits = 0; 
   int iResult = 0;
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100, 100000, 10000, 1000, 100, 21 };
   int[] iArrValidValues = new Int32[]{ 100000, 99999 , 10000 , 1000, 100, 10, 1, 0 };
   IntlStrings intl = new IntlStrings();
   String intlString = intl.GetString(20, true, true);    
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {
     iResult = intlString.IndexOf( intlString[5] , iArrInvalidValues[iLoop] );
     nErrorBits++;
     Console.WriteLine("Error_0000!!! Expected exception not occured...");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
       nErrorBits++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     iResult = intlString.IndexOf( intlString[5] , iArrLargeValues[iLoop] );
     nErrorBits++;
     Console.WriteLine("Error_1111!!! Expected exception not occured...");                
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.WriteLine( "Error_3333!!! Unexpected exception " + ex.ToString() );
       nErrorBits++ ;
       }
   }
   iCountTestcases++;
   intlString = intl.GetString(100002, true, true);        
   for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
   try
     {                            
     iResult = intlString.IndexOf( intlString[100001] , iArrValidValues[iLoop] );
     if ( iResult == -1 ){
     nErrorBits++;
     Console.WriteLine( "Error_6666!!!! Incorrect IndexOf value.... Actual...{0}", iResult );
     }
     } catch ( Exception ex ){
     Console.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
     nErrorBits++ ;
     }
   }
   iCountTestcases++ ;
   String swrString2 =  "nOpqrs" ; 
   if ( swrString2.IndexOf( 'o' ,0  ) != -1 ) 
     nErrorBits = nErrorBits | 0x1; 
   if ( swrString2.IndexOf( 'O' ,0 ) != 1 ) 
     nErrorBits = nErrorBits | 0x2;
   if ( swrString2.IndexOf( 'O' ,1  ) != 1 ) 
     nErrorBits = nErrorBits | 0x4;
   swrString2 =  "Dill Guv Dill Guv Dill" ; 
   if ( swrString2.IndexOf( 'G' ,3 ) != 5 ) 
     nErrorBits = nErrorBits | 0x8;
   if ( swrString2.IndexOf( 'G' ,11  ) != 14 ) 
     nErrorBits = nErrorBits | 0x10;
   if ( swrString2.IndexOf( ' ' ,0  ) != 4 ) 
     nErrorBits = nErrorBits | 0x20;
   intl = new IntlStrings();
   swrString2 = intl.GetString(19, true, true);
   String swrString3 = swrString2.Substring(0, 1);
   char[] charArray = swrString3.ToCharArray();
   if(swrString2.IndexOf(charArray[0], 0) != 0) {
   nErrorBits = nErrorBits | 0x40;
   }
   System.Console.Error.Write( "String.IndexOf: Co1090IO nErrorBits==" );
   System.Console.WriteLine( nErrorBits );
   if (nErrorBits == 0) {
   return true; 
   }
   else {
   return false; 
   }
   }
 public static void Main( String[] args ) 
   {
   System.Console.WriteLine( "String.IndexOf: Co1090IO main starting..." );
   bool bResult = false; 
   Co1090IndexOf_CharInt oCo1090IOa = new Co1090IndexOf_CharInt();
   bResult = oCo1090IOa.runTest
     ();
   if (bResult == true) System.Console.WriteLine( "String.IndexOf: Co1090IO paSs." );
   else                 System.Console.WriteLine( "String.IndexOf: Co1090IO FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
