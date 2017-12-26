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
public class Co1160LastIndexOf_CharInt
{
 public virtual bool runTest
   ()
   {
   System.Console.Error.WriteLine( "String.LastIndexOf: Co1160LIO runTest starting..." );
   int iCountTestcases = 0 ;
   int nErrorBits = 0; 
   int n2 = -2;
   String strLoc="Loc_000oo";
   System.String swrString2 = null;
   System.String swrString3 = null;
   System.String swrString4 = null;
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
     iResult = intlString.LastIndexOf( intlString[5] , iArrInvalidValues[iLoop] );
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
     iResult = intlString.LastIndexOf( intlString[5] , iArrLargeValues[iLoop] );
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
     iResult = intlString.LastIndexOf( intlString[iArrValidValues[iLoop]] , intlString.Length - 1 );
     if ( iResult == -1 ){
     nErrorBits++;
     Console.WriteLine( "Error_6666!!!! Incorrect LastIndexOf value.... Actual...{0}", iResult );
     }
     } catch ( Exception ex ){
     Console.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
     nErrorBits++ ;
     }
   }
   try
     {
     iCountTestcases++ ;
     swrString2 = "abc X def X ghi";
     n2 = swrString2.LastIndexOf( 'i' ,13 );
     if (n2 != -1)
       nErrorBits = nErrorBits | 0x1;
     strLoc="Loc_304sd";
     try
       {
       swrString2 = "abc X def X ghi";
       n2 = swrString2.LastIndexOf( 'i' ,-1  );
       nErrorBits = nErrorBits | 0x2;
       }
     catch (ArgumentException)
       {
       }
     catch (Exception)
       {
       nErrorBits = nErrorBits | 0x40;
       }
     iCountTestcases++ ;
     strLoc="Loc_204_os";
     try
       {
       n2 = swrString2.LastIndexOf( 'I' ,-1  );
       nErrorBits = nErrorBits | 0x10;
       }
     catch (ArgumentException)
       {
       }
     catch (Exception)
       {
       nErrorBits = nErrorBits | 0x80;
       }
     iCountTestcases++ ;
     strLoc="Loc_038_ai";
     swrString2 = "abc X def X ghi";
     n2 = swrString2.LastIndexOf( 'c' ,2 );
     if (n2 != 2)
       nErrorBits = nErrorBits | 0x4;
     iCountTestcases++ ;
     strLoc="Loc_543_lw";
     swrString2 = "abc X def X ghi";
     n2 = swrString2.LastIndexOf( 'X' ,7  );
     if (n2 != 4)
       nErrorBits = nErrorBits | 0x8;
     intl = new IntlStrings();
     swrString2 = intl.GetString(4, true, true);
     swrString3 = intl.GetString(1, true, true);
     swrString4 = String.Concat(swrString2, swrString3);
     swrString4 = String.Concat(swrString4, swrString2);
     swrString4 = String.Concat(swrString4, swrString3);
     swrString4 = String.Concat(swrString4, swrString2);
     n2 = swrString4.LastIndexOf(swrString3[0], 7);
     if(n2 < 4) 
       nErrorBits = nErrorBits | 0x8;
     }
   catch (Exception ex)
     {
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1160LastIndexOf_CharInt) ex==" + ex  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     nErrorBits = nErrorBits | 0x20;
     }
   System.Console.Error.Write( "String.LastIndexOf: Co1160LIO nErrorBits==" );
   System.Console.Error.WriteLine( nErrorBits  );
   if (nErrorBits == 0) {
   return true; 
   }
   else {
   return false; 
   }
   }
 public static void Main( String[] args )
   {
   System.Console.Error.WriteLine( "String.LastIndexOf: Co1160LIO main starting..." );
   bool bResult = false; 
   Co1160LastIndexOf_CharInt oCo1160LIOa = new Co1160LastIndexOf_CharInt();
   bResult = oCo1160LIOa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "String.LastIndexOf: Co1160LIO paSs." );
   else                 System.Console.Error.WriteLine( "String.LastIndexOf: Co1160LIO FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
