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
public class Co3425LastIndexOfAny_CharArrIntInt
{
 static String strName = "String.LastIndexOfAny";
 static String strTest = "Co3425LastIndexOfAny_CharArrIntInt.";
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
   String str1 = "Dill Guv Dill Guv Dill" ;
   Char[] chArr1 = {'G', 'M', 'X', 'Y', 'Z'}; 
   Char[] chArr2 = null; 
   Char[] chArr3 = {}; 
   String str2 = "abcd" ;
   Char[] chArr91 = new Char[] { 'a' ,'b' ,'c' };
   Int32 iAA;
   try
     {
     int iResult = 0;
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100, 100000, 10000, 1000, 100, 21 };
     int[] iArrValidValues = new Int32[]{ 100000, 99999 , 10000 , 1000, 100, 10, 3 };
     IntlStrings intl = new IntlStrings();
     String intlString = intl.GetString(20, true, true); 
     str1 = intlString.Substring( 5, 5);   
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = str1.LastIndexOfAny( chArr1 , iArrInvalidValues[iLoop], 5 );
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
       iResult = str1.LastIndexOfAny( chArr1 , 5, iArrInvalidValues[iLoop] );
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
       iResult = intlString.LastIndexOfAny( chArr1 , iArrLargeValues[iLoop], 5 );
       iCountErrors++;
       Console.WriteLine("Error_4444!!! Expected exception not occured...");                
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.WriteLine( "Error_5555!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;                                                             
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       iResult = intlString.LastIndexOfAny( chArr1 , 5, iArrLargeValues[iLoop] );
       iCountErrors++;
       Console.WriteLine("Error_6666!!! Expected exception not occured...");                
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;                                                             
	 }
     }
     iCountTestcases++;
     intlString = intl.GetString(100002, true, true);  
     chArr1 = intlString.Substring( 0, 2).ToCharArray();      
     for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
     try
       {                            
       iResult = intlString.LastIndexOfAny( chArr1 , iArrValidValues[iLoop]-1, iArrValidValues[iLoop] );
       if ( iResult == -1 ){
       iCountErrors++;
       Console.WriteLine( "Error_8888!!!! Incorrect LastIndexOfAny value.... Actual...{0}", iResult );
       }
       } catch ( Exception ex ){
       Console.WriteLine( "Error_9999!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     str1 = "Dill Guv Dill Guv Dill" ;
     chArr1 = new Char[]{'G', 'M', 'X', 'Y', 'Z'}; 
     intl = new IntlStrings();
     String str5 = intl.GetString(8, true, true);
     Char[] chArray = {str5[1], str5[7]};
     if(str5.LastIndexOfAny(chArray, 0, 0) != -1) {
     ++iCountErrors;
     strInfo = strTest + " error: ";
     strInfo += "FAiL. E_73231ke, Index == < " + str1.LastIndexOfAny(chArr1, 5, 3).ToString() + " >";
     Console.WriteLine( strTest+ strInfo );
     }
     iAA = -2;
     iAA = str2.LastIndexOfAny( chArr91 ,3 ,2 );
     ++iCountTestcases;
     if ( iAA != 2 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL! Error_gat765" ;
       Console.WriteLine( strTest+ strInfo );
       }
     if ( str1.LastIndexOfAny(chArr1, 21, 22) != 14 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_738ke" ;
       Console.WriteLine( strTest+ strInfo );
       }
     if ( str1.LastIndexOfAny(chArr1, 13, 14 ) != 5 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_892dds " ;
       Console.WriteLine( strTest+ strInfo );
       }
     if ( str1.LastIndexOfAny(chArr1, 21, 6) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_739cp" ;
       Console.WriteLine( strTest+ strInfo );
       }
     if ( str1.LastIndexOfAny(chArr1, 5, 1) != 5 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_720mr" ;
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       if ( str1.LastIndexOfAny(chArr3, 22, 23) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_032sp" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       }
     catch (ArgumentException )
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_904nu" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       chArr1[0]='I';
       if ( str1.LastIndexOfAny(chArr1, 22, 23) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_025wp, Index == " + (str1.LastIndexOfAny(chArr1, 0, 22)).ToString() + "  ";
	 Console.WriteLine( strTest+ strInfo );
	 }
       chArr1[0]='M';
       chArr1[1]='I';
       chArr1[2]='i';
       }
     catch (ArgumentException )
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_304nu" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       if ( str1.LastIndexOfAny(chArr1, 22, 23) != 19 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_076tw " ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       chArr1[1]='M';
       chArr1[2]='X';
       }
     catch (ArgumentException )
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_204vu" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str1.LastIndexOfAny(chArr1, 5, 7);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_248ko" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException )
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
     try
       {
       str1.LastIndexOfAny(chArr1, 23, 16);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_015qp " ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException )
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
     try
       {
       str1.LastIndexOfAny(chArr1, 23, 1);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_260aw" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException )
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
     try
       {
       chArr1[0] = '\0';
       if ( str1.LastIndexOfAny(chArr1, 22, 23) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_839pl, Index returned == " + str1.LastIndexOfAny(chArr1, 22, 0).ToString();
	 strInfo += " Expected: 1 ";
	 Console.WriteLine( strTest+ strInfo );
	 }
       }
     catch (ArgumentException )
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_804nu" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str1.LastIndexOfAny(chArr2, 22, 23);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_924ne" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException )
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_175sd" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     str1 = null;
     try
       {
       str1.LastIndexOfAny(chArr1, 21, 22);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_304_jk" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (NullReferenceException )
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
   Boolean bResult = false;	
   Co3425LastIndexOfAny_CharArrIntInt oCbTest = new Co3425LastIndexOfAny_CharArrIntInt();
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
