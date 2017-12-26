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
public class Co3713LastIndexOf_StrIntInt
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "String.LastIndexOf(String, int, int)";
 public static readonly String s_strTFName        = "Co3713LastIndexOf_StrIntInt.";
 public static readonly String s_strTFAbbrev      = "Co3713";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   String str1;
   String str2;
   int iPosition;
   try
     {
     int iResult = 0;
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100, 100000, 10000, 1000, 100, 21 };
     int[] iArrValidValues = new Int32[]{ 100000, 99999 , 50000, 10000 , 1000, 200, 500, 100, 10, 3 };
     IntlStrings intl = new IntlStrings();
     String intlString = intl.GetString(20, true, true);    
     str1 = intlString.Substring ( 10,2 );
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = intlString.LastIndexOf( str1 , iArrInvalidValues[iLoop], 5 );
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
       iResult = intlString.LastIndexOf( str1 , 5, iArrInvalidValues[iLoop]);
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
       iResult = intlString.LastIndexOf( str1 , iArrLargeValues[iLoop], 5 );
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
       iResult = intlString.LastIndexOf( str1 , 5, iArrLargeValues[iLoop] );
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
     str1 = intlString.Substring( 0, 2);      
     for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
     try
       {                            
       iResult = intlString.LastIndexOf( str1 , iArrValidValues[iLoop] - 1, iArrValidValues[iLoop] );
       if ( iResult == -1 ){
       iCountErrors++;
       Console.WriteLine( "Error_8888!!!! Incorrect LastIndexOf value.... Actual...{0}", iResult );
       }
       } catch ( Exception ex ){
       Console.WriteLine( "Error_9999!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     iPosition = str1.LastIndexOf(str2, str1.Length, str1.Length + 1);
     ++iCountTestcases;
     if ( iPosition != 21 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_320ye!  iPosition=="+ iPosition );
       }
     try {
     intl = new IntlStrings();
     String myIntlString = intl.GetString(5, true, true);
     String myIntlString2 = intl.GetString(13, true, true);
     myIntlString = String.Concat(myIntlString2, myIntlString);
     myIntlString = String.Concat(myIntlString, myIntlString2);
     Console.WriteLine(myIntlString.Length);
     Console.WriteLine(myIntlString + "$" + myIntlString2);
     if(myIntlString.LastIndexOf(myIntlString2, myIntlString.Length -2, myIntlString.Length -1) != 0) {
     Console.WriteLine(myIntlString.LastIndexOf(myIntlString2, 0, 0));
     ++iCountErrors;
     }
     } catch (Exception e ) { ++iCountErrors;  Console.WriteLine("hi" +  e.ToString());}
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     iPosition = str1.LastIndexOf(str2, str1.Length-2, str1.Length-1);
     if ( iPosition != 0 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_826se!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     iPosition = str1.LastIndexOf(str2, str1.Length-2, str1.Length-2);
     if ( iPosition != -1 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_026qm!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "gOOD";
     iPosition = str1.LastIndexOf(str2, str1.Length-1, str1.Length);
     if ( iPosition != -1 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_672xe!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     str2 = String.Empty;
     iPosition = str1.LastIndexOf(str2, str1.Length-1, str1.Length);
     if ( iPosition != str1.Length-1 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_539dr!  iPosition=="+ iPosition );
       }
     str1 = String.Empty;
     str2 = String.Empty;
     iPosition = str1.LastIndexOf(str2, 0, 0);
     if ( iPosition != 0 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_873se!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     iPosition = str1.LastIndexOf(str1, str1.Length, str1.Length+1);
     if ( iPosition != 0 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_835xe!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     iPosition = str1.LastIndexOf(str1, str1.Length, str1.Length);
     if ( iPosition != -1 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_382dr!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     iPosition = str1.LastIndexOf(str2, str1.Length, str1.Length - 21);
     if ( iPosition != -1 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_357ze!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     iPosition = str1.LastIndexOf(str2, 0, 1);
     if ( iPosition != -1 )
       {
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_571sw!  iPosition=="+ iPosition );
       }
     str1 = "Good Will Hunting is Good";
     str2 = null;
     try
       {
       iPosition = str1.LastIndexOf(str2, str1.Length-1, str1.Length);
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_835xr!  iPosition=="+ iPosition );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  , Wrong Exception thrown == " + ex  );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     try
       {
       iPosition = str1.LastIndexOf(str2, -1, -1);
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_753xr!  iPosition=="+ iPosition );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_904cr!  , Wrong Exception thrown == " + ex  );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     try
       {
       iPosition = str1.LastIndexOf(str2, str1.Length + 1, str1.Length + 2);
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_844gf!  iPosition=="+ iPosition );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_853xr!  , Wrong Exception thrown == " + ex  );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     try
       {
       iPosition = str1.LastIndexOf(str2, str1.Length, str1.Length + 2);
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_862sa!  iPosition=="+ iPosition );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_975se!  , Wrong Exception thrown == " + ex  );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     try
       {
       iPosition = str1.LastIndexOf(str2, str1.Length + 1, str1.Length + 2);
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_864aq!  iPosition=="+ iPosition );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_162sw!  , Wrong Exception thrown == " + ex  );
       }
     str1 = "Good Will Hunting is Good";
     str2 = "Good";
     try
       {
       iPosition = str1.LastIndexOf(str2, 0, str1.Length + 1);
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_926dt!  iPosition=="+ iPosition );
       }
     catch (ArgumentException)
       {
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_108dw!  , Wrong Exception thrown == " + ex  );
       }
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3713LastIndexOf_StrIntInt oCbTest = new Co3713LastIndexOf_StrIntInt();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev +"FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFName +s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
