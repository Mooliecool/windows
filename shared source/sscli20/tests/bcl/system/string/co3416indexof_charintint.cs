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
public class Co3416IndexOf_CharIntInt
{
 internal static String strName = "String.IndexOf";
 internal static String strTest = "Co3416IndexOf_CharIntInt.";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   String str1 = "Dill Guv Dill Guv Dill" ;
   Char ch1 = '\0';
   try
     {
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
       iResult = intlString.IndexOf( intlString[5] , iArrInvalidValues[iLoop], 5 );
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
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = intlString.IndexOf( intlString[5] , 5, iArrInvalidValues[iLoop]);
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
       iResult = intlString.IndexOf( intlString[5] , iArrLargeValues[iLoop], 5 );
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
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       iResult = intlString.IndexOf( intlString[5] , 5, iArrLargeValues[iLoop] );
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
     for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
     try
       {                            
       iResult = intlString.IndexOf( intlString[100001] , iArrValidValues[iLoop], intlString.Length - iArrValidValues[iLoop] );
       if ( iResult == -1 ){
       iCountErrors++;
       Console.WriteLine( "Error_6666!!!! Incorrect IndexOf value.... Actual...{0}", iResult );
       }
       } catch ( Exception ex ){
       Console.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     ch1 = 'G';
     ++iCountTestcases;
     if ( str1.IndexOf(ch1, 0, 21) != 5 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_738ke" ;
       Console.WriteLine( strTest+ strInfo );
       }
     intl = new IntlStrings();
     String myIntlString = intl.GetString(8, true, true);
     Console.WriteLine("myIntlString = [{0}]", myIntlString);
     if(myIntlString.IndexOf(myIntlString[2], 0, 4) > 2) {
     ++iCountErrors;
     strInfo = strTest + " error: ";
     strInfo += "FAiL. E_7358kg" ;
     Console.WriteLine( strTest+ strInfo );
     }
     ch1 = 'G';
     ++iCountTestcases;
     if ( str1.IndexOf(ch1, 6, 15) != 14 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_892dds" ;
       Console.WriteLine( strTest+ strInfo );
       }
     ch1 = 'G';
     ++iCountTestcases;
     if ( str1.IndexOf(ch1, 15, 6) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_739cp" ;
       Console.WriteLine( strTest+ strInfo );
       }
     ch1 = 'G';
     ++iCountTestcases;
     if ( str1.IndexOf(ch1, 5, 0) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_720mr" ;
       Console.WriteLine( strTest+ strInfo );
       }
     ch1 = 'G';
     try
       {
       ++iCountTestcases;
       str1.IndexOf(ch1, 7, -2);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_248ko" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_034fl" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     ch1 = 'G';
     try
       {
       ++iCountTestcases;
       str1.IndexOf(ch1, 7, 16);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_015qp " ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_104nu" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     ch1 = 'G';
     try
       {
       ++iCountTestcases;
       str1.IndexOf(ch1, 23, 0);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_260aw" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_614dq " ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     ch1 = '\0';
     ++iCountTestcases;
     if ( str1.IndexOf(ch1, 15, 6) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_839pl" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str1 = null;
     ch1 = 'G';
     try
       {
       ++iCountTestcases;
       str1.IndexOf(ch1, 0, 21);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_304_jk" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (NullReferenceException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_278pw" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
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
   bool bResult = false;	
   Co3416IndexOf_CharIntInt oCbTest = new Co3416IndexOf_CharIntInt();
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
