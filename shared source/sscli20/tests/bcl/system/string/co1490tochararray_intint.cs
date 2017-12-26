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
public class Co1490ToCharArray_IntInt
{
 public virtual bool runTest()
   {
   System.Console.Error.WriteLine( "String- Co1490ToCharArray_IntInt runTest started." );
   int iCountErrors = 0;
   int iCountTestcases = 0 ;
   char[] cArr2 = null;
   char[] cArr3 = null;
   System.String str7 = null;
   System.String str8 = null;
   System.String str9 = null;
   cArr2 = new char[10];
   cArr2[0] = 'a';
   cArr2[1] = 'b';
   cArr2[2] = 'c';
   cArr2[3] = 'd';
   cArr2[4] = 'e';
   cArr2[5] = 'f';
   cArr2[6] = 'g';
   cArr2[7] = 'h';
   cArr2[8] = 'i';
   cArr2[9] = 'j';
   Char[] chrArr1 ;
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
   int[] iArrValidValues = new Int32[]{ 1000000, 10000000 , 5000000 , 100000 , 5000, 10, 1, 0 };
   iCountTestcases++;
   String strNewString = "This is a tesing string" ;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){                     
   try
     {
     chrArr1 = strNewString.ToCharArray(iArrInvalidValues[iLoop], 5);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_0000!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){                     
   try
     {
     chrArr1 = strNewString.ToCharArray(5, iArrInvalidValues[iLoop]);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_3459!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_5320!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){                     
   try
     {
     chrArr1 = strNewString.ToCharArray(iArrInvalidValues[iLoop], iArrInvalidValues[iLoop]);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_4348!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_9023!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     chrArr1 = strNewString.ToCharArray(iArrLargeValues[iLoop], 5);                        
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_4504!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_3534!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     chrArr1 = strNewString.ToCharArray(5, iArrLargeValues[iLoop]);                        
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_3809!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_42348!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     chrArr1 = strNewString.ToCharArray(iArrLargeValues[iLoop], iArrLargeValues[iLoop]);                        
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_0984!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_0784!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   strNewString =  new String( 'a' , 10000000 );
   for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
   try
     {          
     Char[] chrArr = strNewString.ToCharArray(iArrValidValues[iLoop],( strNewString.Length - iArrValidValues[iLoop] )/2 );
     if ( chrArr.Length !=  ( strNewString.Length - iArrValidValues[iLoop] )/2){
     iCountErrors++;
     Console.Error.WriteLine( "Error_4444!!!! Incorrect string array length.... Expected...{0},  Actual...{1}", ( strNewString.Length - iArrValidValues[iLoop] )/2, chrArr.Length );
     }
     } catch ( Exception ex ){
     Console.Error.WriteLine( "Error_5555!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
   }
   iCountTestcases++ ;
   IntlStrings intl = new IntlStrings();
   char[] MyCharArr = new char[2];
   char[] CopyArr = new char[2];
   MyCharArr[0] = (intl.GetString(1, true, true))[0];
   MyCharArr[1] = (intl.GetString(1, true, true))[0];
   String intlString = new String(MyCharArr);
   CopyArr = intlString.ToCharArray(0, 2);
   if((CopyArr[0] != MyCharArr[0]) || (CopyArr[1] != MyCharArr[1]) ) {
   iCountErrors++;
   }
   iCountTestcases++ ;
   for ( int L_27y=0 ;L_27y==0 ;L_27y++ )   
     {
     str9 = new String( cArr2 );
     str8 =  "cdef" ;
     cArr3 = str9.ToCharArray( 2 ,4 );
     str7 = new String( cArr3 );
     if ( ! str8.Equals( str7 ) )
       {
       iCountErrors += 1;
       System.Console.Error.Write( "Co1490GetCharArray Error E_376vp!  str7 == " );
       System.Console.Error.WriteLine( str7 );
       }
     iCountTestcases++ ;
     str9 = new String( cArr2 );
     str8 =  "abcdefghij" ;
     cArr3 = str9.ToCharArray( 0 ,10 );
     str7 = new String( cArr3 );
     if ( ! str8.Equals( str7 ) )
       {
       iCountErrors += 1;
       System.Console.Error.WriteLine( "Co1490GetCharArray Error E_510fa!" );
       }
     iCountTestcases++ ;
     str9 = new String( cArr2 );
     try
       {
       cArr3 = str9.ToCharArray( 1 ,10 ); 
       iCountErrors += 1;
       System.Console.Error.WriteLine( "Co1490GetCharArray Error E_783cd!" );
       break;   
       }
     catch ( System.Exception) {}
     }   
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co1490ToCharArray_IntInt oCbTest = null;
   try
     {
     oCbTest = new Co1490ToCharArray_IntInt();
     bResult = oCbTest.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "String- Co1490ToCharArray_IntInt main caught Exception!" );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
} 
