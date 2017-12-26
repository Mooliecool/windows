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
using System;
public class Test{
 public static void Main(){
 Test t = new Test();
 t.Run();
 }
 public void Run(){
 int iCountErrors = 0 ;
 int iCountTestcases = 0 ;
 int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
 int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
 int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/50 ,Int32.MaxValue/200 , Int32.MaxValue/1000 };
 iCountTestcases++;
 String strNewString = "AAAAABBBBB" ;
 for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
 try
   {
   strNewString = strNewString.PadLeft(iArrInvalidValues[iLoop]);
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
 for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
 strNewString = String.Empty ;
 try
   {
   strNewString = strNewString.PadLeft(iArrLargeValues[iLoop]);
   if( strNewString.Length != iArrLargeValues[iLoop] ){
   iCountErrors++;                        
   Console.Error.WriteLine( "Error_1111!! Expected exception not occured or incorrect string length");
   }
   } catch ( OutOfMemoryException ){
   } catch ( Exception ex )
     {
     Console.Error.WriteLine( "Error_3333!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
 }
 iCountTestcases++;
 for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
 strNewString = String.Empty ;
 try
   {                            
   strNewString = strNewString.PadLeft(iArrValidValues[iLoop]);
   if ( strNewString.Length != iArrValidValues[iLoop] ){
   iCountErrors++;
   Console.Error.WriteLine( "Error_6666!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrValidValues[iLoop], strNewString.Length );
   }
   } catch ( Exception ex ){
   Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
   iCountErrors++ ;
   }
 }
 if( iCountErrors == 0) 
   {
   Console.WriteLine("Test PASSED");
   Environment.ExitCode = 0;
   }
 else
   {
   Console.WriteLine("Test FAILED");
   Environment.ExitCode = 1;
   }
 }
}
