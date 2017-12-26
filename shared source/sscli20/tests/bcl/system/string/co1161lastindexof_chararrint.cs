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
public class Co1161LastIndexOfAny_CharArrInt
{
 public Boolean runTest
   ()
   {
   System.Console.Error.WriteLine( "String.LastIndexOfAny: Co1161LIO runTest starting..." );
   int iCountErrors = 0, iCountTestcases = 0; 
   int n2 = -2;
   System.String swrString2 = null;
   System.String swrString3 = null;
   System.String swrString4 = null;       
   char[] chArr1 = {'H', 'i'};
   int iResult = 0;
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100, 100000, 10000, 1000, 100, 21 };
   int[] iArrValidValues = new Int32[]{ 100000, 99999 , 10000 , 1000, 100, 10, 3 };
   IntlStrings intl = new IntlStrings();
   String intlString = intl.GetString(20, true, true); 
   String str1 = intlString.Substring( 5, 5);   
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {
     iResult = str1.LastIndexOfAny( chArr1 , iArrInvalidValues[iLoop] );
     iCountErrors++;
     Console.WriteLine("Error_0000!!! Expected exception not occured...");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     iResult = intlString.LastIndexOfAny( chArr1 , iArrLargeValues[iLoop] );
     iCountErrors++;
     Console.WriteLine("Error_1111!!! Expected exception not occured...");                
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.WriteLine( "Error_3333!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;                                                             
       }
   }
   iCountTestcases++;
   intlString = intl.GetString(100002, true, true);  
   chArr1 = intlString.Substring( 0, 2).ToCharArray();      
   for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
   try
     {                            
     iResult = intlString.LastIndexOfAny( chArr1 , iArrValidValues[iLoop] );
     if ( iResult == -1 ){
     iCountErrors++;
     Console.WriteLine( "Error_6666!!!! Incorrect LastIndexOfAny value.... Actual...{0}", iResult );
     }
     } catch ( Exception ex ){
     Console.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
   }
   char[] cArr = {'i'}; 
   char[] cArr1 = {'H', 'i'}; 
   swrString2 = "abc X def X ghi"; 
   n2 = swrString2.LastIndexOfAny( cArr ,13  );
   if (n2 != -1)
     iCountErrors = iCountErrors | 0x1;
   try
     {
     n2 = swrString2.LastIndexOfAny( cArr ,-1  );
     iCountErrors = iCountErrors | 0x2;
     }
   catch (ArgumentException)
     {
     }
   catch (Exception)
     {
     iCountErrors = iCountErrors | 0x40;
     }
   cArr[0]='c';
   n2 = swrString2.LastIndexOfAny( cArr ,3  );
   if (n2 != 2)
     iCountErrors = iCountErrors | 0x4;
   cArr[0] = 'X';
   n2 = swrString2.LastIndexOfAny( cArr ,7  );
   if (n2 != 4)
     iCountErrors = iCountErrors | 0x8;
   n2 = swrString2.LastIndexOfAny( cArr1 ,12  );
   if (n2 != -1)
     iCountErrors = iCountErrors | 0x40;
   n2 = swrString2.LastIndexOfAny( cArr1 ,13  );
   if (n2 != -1)
     iCountErrors = iCountErrors | 0x80;
   intl = new IntlStrings();
   swrString2 = intl.GetString(4, true, true);
   swrString3 = intl.GetString(1, true, true);
   swrString4 = String.Concat(swrString2, swrString3);
   swrString4 = String.Concat(swrString4, swrString2);
   swrString4 = String.Concat(swrString4, swrString3);
   swrString4 = String.Concat(swrString4, swrString2);
   n2 = swrString4.LastIndexOfAny(swrString2.ToCharArray() , 7);
   if(n2 < 7) {
   iCountErrors = iCountErrors | 0x100;
   }
   System.Console.Error.Write( "String.LastIndexOfAny: Co1161LIO iCountErrors==" );
   System.Console.Error.WriteLine( iCountErrors  );
   if (iCountErrors == 0) {
   return true; 
   }
   else {
   return false; 
   }
   }
 public static void Main( String[] args ) 
   {
   System.Console.Error.WriteLine( "String.LastIndexOfAny: Co1161LIO main starting..." );
   Boolean bResult = false; 
   Co1161LastIndexOfAny_CharArrInt oCo1161LIOa = new Co1161LastIndexOfAny_CharArrInt();
   bResult = oCo1161LIOa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "String.LastIndexOfAny: Co1161LIO paSs." );
   else                 System.Console.Error.WriteLine( "String.LastIndexOfAny: Co1161LIO FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
