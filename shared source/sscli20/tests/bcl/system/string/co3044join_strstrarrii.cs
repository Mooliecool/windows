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
public class Co3044Join_StrStrArrII
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co3044Join_StrStrArrII.  runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   String stra = null;
   String strb = null;
   String strc = null;
   String strd = null;
   String seperator = null;
   String result = null;
   String check = null;
   String []values = new String[4];
   String strNewString = String.Empty ;
   String[] strArr = new String[]{"Test", "Cool", "Fail"};
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
   int[] iArrValidValues = new Int32[]{ 10, 100, 200, 400, 800,999};
   iCountTestcases++;
   String strSeparator = "This is a tesing string" ;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {
     strNewString = String.Join(strSeparator, strArr, iArrInvalidValues[iLoop], 2);
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
     strNewString = String.Join(strSeparator, strArr, 2, iArrInvalidValues[iLoop]);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_3535!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_53523!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {
     strNewString = String.Join(strSeparator, strArr, iArrInvalidValues[iLoop], iArrInvalidValues[iLoop]);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_4324!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_1321!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     strNewString = String.Join(strSeparator, strArr, iArrLargeValues[iLoop], 2);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_4644!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_98897!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     strNewString = String.Join(strSeparator, strArr, 2, iArrLargeValues[iLoop]);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_42243!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_8554!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     strNewString = String.Join(strSeparator, strArr, iArrLargeValues[iLoop], iArrLargeValues[iLoop]);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_4390!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_7432!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   strSeparator =  new String( 'a' , 1000 );
   strArr = new String[ 1000 ];
   for(int i = 0 ; i < 1000 ; i++)
     strArr[i] = new String( 'b' , 10);
   for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
   try
     {                            
     strNewString = String.Join(strSeparator, strArr, 0, iArrValidValues[iLoop]);
     if ( strNewString.Length < 1000 * (iArrValidValues[iLoop]-1) ){
     iCountErrors++;
     Console.Error.WriteLine( "Error_0088!!!! Incorrect string length.... Expected...{0},  Actual...{1}", 1000 + ( 10000 * (iArrValidValues[iLoop]-1)), strNewString.Length );
     }                
     } catch (OutOfMemoryException ex ){
     } catch ( Exception ex ){
     Console.Error.WriteLine( "Error_5400!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
   }
   iCountTestcases++;
   strSeparator =  new String( 'a' , 1000 );
   try
     {                            
     strNewString = String.Join(strSeparator, strArr, 0 , 1000);
     if ( strNewString.Length != 1009000  ){
     iCountErrors++;
     Console.Error.WriteLine( "Error_54758!!!! Incorrect string length.... Expected...{0},  Actual...{1}", Int32.MaxValue/200 , strNewString.Length );
     }
     } catch ( Exception ex ){
     Console.Error.WriteLine( "Error_5345!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
   iCountTestcases++;
   values[0] = stra;
   values[1] = strb;
   values[2] = strc;
   values[3] = strd;
   try {
   result = String.Join(seperator, values, 0, 3);
   } catch (Exception excp)
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_12ie (Co3044Join_StrStrArrII.)  result==" + result);
     strError = "EXTENDEDINFO ";
     strError = strError + excp.ToString();
     Console.Error.WriteLine(strError);
     }
   stra = "abcd";
   seperator = "###!!66t";
   check = "abcd###!!66t###!!66t";  
   values[0] = stra;
   values[1] = null;
   values[2] = null;
   values[3] = strd;
   result = String.Join(seperator, values, 0, 3);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_89pq (Co3044Join_StrStrArrII.)  result==" + result);
     }
   stra = "abcde";
   strb = "12345";
   values[0] = stra;
   values[1] = strb;
   values[2] = null;
   values[3] = strd;
   seperator = "!@#$%";
   check = "abcde!@#$%12345!@#$%";
   result = String.Join(seperator, values, 0, 3);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_23yy (Co3044Join_StrStrArrII.)  result==" + result);
     }
   IntlStrings intl = new IntlStrings();
   strb = intl.GetString(8, true, true);
   strc = intl.GetString(3, true, true);
   strd = intl.GetString(15, true, true);
   values[0] = stra;
   values[1] = strb;
   values[2] = strc;
   values[3] = strd;
   seperator = "!@#$%";
   check = String.Concat(strb, seperator);
   check = String.Concat(check, strc);
   check = String.Concat(check, seperator);
   check = String.Concat(check, strd);
   result = String.Join(seperator, values, 1, 3);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_14mm (Co3044Join_StrStrArrII.)  result==" + result);
     }
   stra = "abcde";
   strb = "12345";
   strc = "!@#$%";
   seperator = String.Empty;
   values[0] = stra;
   values[1] = strb;
   values[2] = strc;
   values[3] = strd;
   result = String.Join(seperator, values, 0, 3);
   check = "abcde12345!@#$%";
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_77jj (Co3044Join_StrStrArrII.)  result==" + result);
     }
   stra = "abcde";
   strb = "12345";
   strc = "!@#$%";
   seperator = null;
   values[0] = stra;
   values[1] = strb;
   values[2] = strc;
   values[3] = strd;
   try
     {
     result = String.Join(seperator, values, 0, 3);
     }
   catch ( Exception exc )
     {
     ++iCountErrors;
     Console.Error.WriteLine( "Is this an error? exc=" + exc.ToString() );
     }
   check = "abcde12345!@#$%";
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_77jj (Co3044Join_StrStrArrII.)  result==" + result);
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3044Join_StrStrArrII cb0 = new Co3044Join_StrStrArrII();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3044Join_StrStrArrII." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
