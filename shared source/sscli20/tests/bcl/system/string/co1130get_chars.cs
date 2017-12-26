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
public class Co1130get_Chars_Int
{
 public virtual bool runTest()
   {
   int iCountTestcases = 0 ;
   int nErrorBits = 0; 
   String swrString2= null, swrString3 = null;;
   Char c = '\0';
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
   int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/100 , Int32.MaxValue/1000 };
   iCountTestcases++;
   String strNewString = "AAAAAAAAAAAAAAAAAAAAAAAAAA" ;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {
     c = strNewString[iArrInvalidValues[iLoop]];
     nErrorBits++;
     } catch ( IndexOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
       nErrorBits++ ;
       }
   if ( c != '\0' )
     {
     nErrorBits++;
     Console.Error.WriteLine( "Error_3333!!!! Incorrect string content.... Expected...{0},  Actual...{1}", String.Empty, strNewString );
     }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     c = strNewString[iArrLargeValues[iLoop]];
     } catch ( IndexOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_4444!!! Unexpected exception " + ex.ToString() );
       nErrorBits++ ;
       }
   }
   iCountTestcases++;
   strNewString = new String('a', Int32.MaxValue/100 + 10 ) ;
   for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
   try
     {                            
     c = strNewString[iArrValidValues[iLoop]];
     if ( c != 'a' ){
     nErrorBits++;
     Console.Error.WriteLine( "Error_6666!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrValidValues[iLoop], strNewString.Length );
     }
     } catch ( Exception ex ){
     Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
     nErrorBits++ ;
     }
   }
   swrString2 = " abcdefghijklmnopqrstuvwxyz";
   swrString3 = "bcdefg";
   for ( int ii = 0; ii < 27; ++ii )
     {
     char c1 = swrString2[ ii ];
     if ( ii == 0 )
       {
       if ( c1 != ' ' )
	 {
	 nErrorBits = nErrorBits | 0x1;
	 break;
	 }
       }
     else
       {
       if ( c1 != (0x60 + ii) )
	 {
	 nErrorBits = nErrorBits | 0x1;
	 break;
	 }
       }
     }
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
   Co1130get_Chars_Int oCo1130Ca = new Co1130get_Chars_Int();
   bResult = oCo1130Ca.runTest();
   if (bResult == true) Console.Error.WriteLine( "paSs." );
   else                 Console.Error.WriteLine( "FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
