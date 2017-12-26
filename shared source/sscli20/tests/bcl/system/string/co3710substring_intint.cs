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
public class Co3710Substring_IntInt
{
 public virtual bool runTest
   ()
   {
   System.Console.Error.WriteLine( "String- Co3710Substring_IntInt runTest started." );  
   int iCountErrors = 0 ;
   int iCountTestcases = 0 ;
   System.String swrString2 = null;
   System.String swrString3 = null;
   System.String swrString4 = null;
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
   int[] iArrValidValues = new Int32[]{ 1000000, 10000000 , 5000000 , 100000 , 5000, 10, 1, 0 };
   iCountTestcases++;
   String strNewString = "This is a tesing string" ;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){                     
   try
     {
     strNewString = strNewString.Substring(iArrInvalidValues[iLoop], 5);
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
     strNewString = strNewString.Substring(5, iArrInvalidValues[iLoop]);
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
     strNewString = strNewString.Substring(iArrInvalidValues[iLoop], iArrInvalidValues[iLoop]);
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
     strNewString = strNewString.Substring(iArrLargeValues[iLoop], 5);                        
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
     strNewString = strNewString.Substring(5, iArrLargeValues[iLoop]);                        
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
     strNewString = strNewString.Substring(iArrLargeValues[iLoop], iArrLargeValues[iLoop]);                        
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
     String strTemp = strNewString.Substring(iArrValidValues[iLoop],( strNewString.Length - iArrValidValues[iLoop] )/2 );
     if ( strTemp.Length !=  ( strNewString.Length - iArrValidValues[iLoop] )/2){
     iCountErrors++;
     Console.Error.WriteLine( "Error_4444!!!! Incorrect string array length.... Expected...{0},  Actual...{1}", ( strNewString.Length - iArrValidValues[iLoop] )/2, strTemp.Length );
     }
     } catch ( Exception ex ){
     Console.Error.WriteLine( "Error_5555!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
   }
   swrString2 = "abcde";
   swrString3 =  "bcd";
   swrString4 = swrString2.Substring( 1,3 );
   if ( swrString3.Equals( swrString4 ) != true ) 
     iCountErrors = iCountErrors | 0x1;
   IntlStrings intl = new IntlStrings();
   swrString2 = intl.GetString(1, true, true);
   swrString3 = intl.GetString(5, true, true);
   swrString2 = String.Concat(swrString2, swrString3);
   swrString4 = swrString2.Substring(1, 5);
   if(swrString3.Equals(swrString4) != true) {
   iCountErrors = iCountErrors | 0x4;
   } 
   swrString2 = "abcde";
   swrString3 =  "bcde";
   swrString4 = swrString2.Substring( 1 );
   if ( swrString3.Equals( swrString4 ) != true ) 
     iCountErrors = iCountErrors | 0x2;
   if (iCountErrors == 0)
     {
     return true; 
     }
   else
     {
     System.Console.Error.WriteLine(  iCountErrors.ToString() );
     return false; 
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3710Substring_IntInt oCb1126Sa = new Co3710Substring_IntInt();
   bResult = oCb1126Sa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "String.Substring: Cb1126S paSs." );
   else                 System.Console.Error.WriteLine( "String.Substring: Cb1126S FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
