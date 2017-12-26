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
using GenStrings;
using System.Globalization;
public class Co3409Compare_StrIntStrIntInt
{
 static String strName = "String.Compare";
 static String strTest = "Co3409Compare_StrIntStrIntInt.";
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   String strAlphabetsLower = "abcdefghijklmnopqrstuvwxyz";
   String strAlphabetsUpper = "ABCDEFGHGIJKLMNOPQRSTUVWXYZ";
   String str1 = null;
   String str2 = null;
   try
     {
     IntlStrings intl = new IntlStrings();
     String intlString = intl.GetString(20, true, true);
     str1 = intlString;
     int iResult = 0;
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
     int[] iArrValidValues = new Int32[]{ 10000, 5000 , 1000 , 0 };
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = String.Compare(intlString , iArrInvalidValues[iLoop], str1, 5 , 5);
       iCountErrors++;
       Console.Error.WriteLine( "Error_0000!!!! Expected exception not occured" );
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     if ( intlString != str1 )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_3333!!!! Content changed while comparing" );
       }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = String.Compare(intlString , 5, str1, iArrInvalidValues[iLoop] , 5);
       iCountErrors++;
       Console.Error.WriteLine( "Error_4444!!!! Expected exception not occured" );
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_5555!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     if ( intlString != str1 )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_6666!!!! Content changed while comparing" );
       }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = String.Compare(intlString , 5, str1, 5 , iArrInvalidValues[iLoop]);
       iCountErrors++;
       Console.Error.WriteLine( "Error_7777!!!! Expected exception not occured" );
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_8888!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     if ( intlString != str1 )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_9999!!!! Content changed while comparing" );
       }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       iResult = String.Compare(intlString , iArrLargeValues[iLoop], str1, 5 , 5);
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_2134!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       iResult = String.Compare(intlString , 5, str1, iArrLargeValues[iLoop] , 5);
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_8787!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       iResult = String.Compare(intlString , 5, str1, 5 , iArrLargeValues[iLoop]);
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_43433!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     intlString = intl.GetString( 100000, true , true );
     str1 = intlString ;
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
     try
       {                            
       iResult = String.Compare(intlString , iLoop * 100, str1, iLoop*100 , iArrValidValues[iLoop]);
       if ( iResult != 0 ){
       iCountErrors++;
       Console.Error.WriteLine( "Error_6666!!!! Compare returned incorrect value.... Expected...{0},  Actual...{1}", 0, iResult );
       }
       } catch ( Exception ex ){
       Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     intlString = intl.GetString(20, true, true);
     str1 = intlString;
     if( String.Compare(str1, 5, intlString, 5 , 10) != 0 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_738ke" ;
       Console.WriteLine( strTest+ strInfo );
       }  
     str1 = strAlphabetsLower;
     ++iCountTestcases;
     if ( String.Compare(str1, 5, strAlphabetsLower, 5, 10) != 0 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_738ke" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str1 = strAlphabetsLower;
     ++iCountTestcases;
     if ( String.Compare(str1, 5, strAlphabetsLower, 6, 10) >= 0 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_023Sd" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str1 = strAlphabetsUpper;
     ++iCountTestcases;
     if ( String.Compare(str1, 5, strAlphabetsUpper, 4, 10) <= 0 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_033pw" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str1 = strAlphabetsUpper;
     str2 = null;
     ++iCountTestcases;
     if ( String.Compare(str1, 5, str2, 0, 0) != 1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_053jh" ;
       Console.WriteLine( strTest+ strInfo );
       }
     try {		
     str1 = null;
     str2 = strAlphabetsLower;
     ++iCountTestcases;
     if ( String.Compare(str1, 5, str2, 8, 21) != -1)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_304_jk" ;
       Console.WriteLine( strTest+ strInfo );
       }
     } catch(ArgumentOutOfRangeException ){        
     } catch(Exception e) {
     iCountErrors++ ;
     Console.WriteLine("Error_1234!!!! Unexpected exception occured..." + e.Message ) ;
     }
     str1 = strAlphabetsLower;
     str2 = strAlphabetsUpper;
     try
       {	
       ++iCountTestcases;
       String.Compare(str1, -1, str2, 5, 10);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_982ww" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentOutOfRangeException )
       {
       }
     catch (Exception )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_223mn" ;
       strInfo += ", Wrong Exception thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str1 = strAlphabetsLower;
     str2 = strAlphabetsLower;
     try
       {
       ++iCountTestcases;
       String.Compare(str1, 5, str2, 5, Int32.MinValue);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_754lk" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException )
       {
       }
     catch (Exception )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_720sd" ;
       strInfo += ", Wrong Exception thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str1 = strAlphabetsLower;
     str2 = strAlphabetsLower;
     try
       {	
       ++iCountTestcases;
       String.Compare(str1, 5, str2, 27, 1);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_724we" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException )
       {
       }
     catch (Exception )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_363gd" ;
       strInfo += ", Wrong Exception thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.Error.WriteLine ( "POINTTOBREAK: Error Err_103! strLoc=="+ strLoc + " ,exc_general=="+exc_general  );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3409Compare_StrIntStrIntInt oCbTest = new Co3409Compare_StrIntStrIntInt();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
