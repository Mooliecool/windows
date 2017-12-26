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
using GenStrings;
using System.Globalization; 
using System;
public class Co3422IndexOf_StrIntInt
{
 internal static String strName = "String.IndexOf";
 internal static String strTest = "Co3422IndexOf_StrIntInt.";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   String str1 =String.Empty ;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   try
     {
     int iResult = 0;
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100, 100000, 10000, 1000, 100, 21 };
     int[] iArrValidValues = new Int32[]{ 100000, 99999 , 10000 , 1000, 100, 10, 1, 0 };
     IntlStrings intl = new IntlStrings();
     String intlString = intl.GetString(20, true, true);    
     str1 = intlString.Substring ( 10,2 );
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = intlString.IndexOf( str1 , iArrInvalidValues[iLoop], 5 );
       iCountErrors++;
       Console.WriteLine("Error_0000!!! Expected exception not occured...");
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.WriteLine( "Error_1111!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = intlString.IndexOf( str1 , 5, iArrInvalidValues[iLoop]);
       iCountErrors++;
       Console.WriteLine("Error_2222!!! Expected exception not occured...");
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
       iResult = intlString.IndexOf( str1 , iArrLargeValues[iLoop], 5 );
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
       iResult = intlString.IndexOf( str1 , 5, iArrLargeValues[iLoop] );
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
     str1 = intlString.Substring( 100000, 2);      
     for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
     try
       {                            
       iResult = intlString.IndexOf( str1 , iArrValidValues[iLoop], intlString.Length - iArrValidValues[iLoop] );
       if ( iResult == -1 ){
       iCountErrors++;
       Console.WriteLine( "Error_8888!!!! Incorrect IndexOf value.... Actual...{0}", iResult );
       }
       } catch ( Exception ex ){
       Console.WriteLine( "Error_9999!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     str1 = "Dill Guv Dill Guv Dill" ;
     String str2 = "Dill";
     ++iCountTestcases;
     if ( str1.IndexOf(str2, 0, str1.Length) != 0 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_738ke " ;
       Console.WriteLine( strTest+ strInfo );
       }
     intl = new IntlStrings();
     String myIntlString = intl.GetString(5, true, true);
     String myIntlString2 = intl.GetString(13, true, true);
     myIntlString = String.Concat(myIntlString, myIntlString2);
     if(myIntlString.IndexOf(myIntlString2, 0, 18) != 5) {
     ++iCountErrors;
     }
     ++iCountTestcases;
     if ( str1.IndexOf(str2, 0, 2) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " FAiL. Err_625sd, Index ==  " + str1.IndexOf(str2, 0, 2);
       Console.WriteLine( strTest+ strInfo );
       }
     ++iCountTestcases;
     if ( str1.IndexOf(str2, 1, 12) != 9 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_767fq " ;
       Console.WriteLine( strTest+ strInfo );
       }
     ++iCountTestcases;
     if ( str1.IndexOf(str2, 19, 2) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_013oi " ;
       Console.WriteLine( strTest+ strInfo );
       }
     str2 = "Dill Guv Dill Guv Dill" ;
     if ( str1.IndexOf(str2, 0, 22) != 0 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_892dds " ;
       Console.WriteLine( strTest+ strInfo );
       }
     str2 = "Dill Guv Dill Guv Dill Dill Bill" ;
     if ( str1.IndexOf(str2, 0, 22) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_256we " ;
       Console.WriteLine( strTest+ strInfo );
       }
     str2 = "ll" ;
     if ( str1.IndexOf(str2, 15, 7) != 20 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_739cp" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str2 = "Bill" ;
     if ( str1.IndexOf(str2, 0, 22) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_720mr" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str2 = "DILL" ;
     if ( str1.IndexOf(str2, 0, 22) != -1 )
       {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_249vu" ;
       Console.WriteLine( strTest+ strInfo );
       }
     str2 = " " ;
     if (str1.IndexOf(str2, 10, 11)!= 13)
       {
       ++iCountErrors;
       strInfo += "FAiL. E_240hg " ;
       strInfo += ", Exception not thrown ==" + (str1.IndexOf(str2)).ToString() ;
       Console.WriteLine( strTest+ strInfo );
       }
     str2 = "" ;
     if (str1.IndexOf(str2, 5, 1)!= 5)
       {
       ++iCountErrors;
       strInfo += "FAiL. E_235jf " ;
       strInfo += ", Index ==" +  "< " + (str1.IndexOf(str2)).ToString() + " >" ;
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str2 = "Dill" ;
       str1.IndexOf(str2, 23, 1);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_067sf" ;
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
       strInfo = "FAiL. E_161gh" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str2 = "Dill" ;
       str1.IndexOf(str2, -50, 21);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_135tq" ;
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
       strInfo = "FAiL. E_175sg" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str2 = "Dill" ;
       str1.IndexOf(str2, 10, -1);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_125xe" ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       strInfo = strTest + " error: ";
       strInfo = "FAiL. E_346ser" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str2 = "Dill" ;
       str1.IndexOf(str2, 10, str1.Length - 9);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_423se" ;
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
       strInfo = "FAiL. E_620ap" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str2 = "Dill" ;
       str1.IndexOf(str2, 10, -1);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_3649se" ;
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
       strInfo = "FAiL. E_236ser" ;
       strInfo += ", Wrong Exception thrown == " + ex.ToString();
       Console.WriteLine( strTest+ strInfo );
       }
     try
       {
       str2 = null ;
       str1.IndexOf(str2, 0, 22);
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
     str1 = "test";
     str2 = null;
     try
       {
       str1.IndexOf(str2, 0);
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_015qp " ;
       strInfo += ", Exception not thrown" ;
       Console.WriteLine( strTest+ strInfo );
       }
     catch (ArgumentNullException)
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
   Co3422IndexOf_StrIntInt oCbTest = new Co3422IndexOf_StrIntInt();
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
