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
using System.Globalization;
using GenStrings;
using System.Text;
using System;
using System.IO;
using System.Collections;
public class Co4827ctor_sbyte_int_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String(sbyte*)";
 public static String s_strTFName        = "Co4827ctor_sbyte_int_int";
 public static String s_strTFAbbrev      = "Co4827";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public unsafe bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     sbyte* chptr = null, baseptr = null;
     String str1 = String.Empty, str2 = String.Empty;
     strLoc = "Loc_498hv";
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };                      
     iCountTestcases++;
     String strNewString = String.Empty ;
     SByte* p1 = stackalloc SByte[256];
     for (int i = 0; i < 256; i++) p1[i] = (SByte)i;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {                                                                                                              
       strNewString = new String(p1 , iArrInvalidValues[iLoop], 1);
       iCountErrors++;
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_0000!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     if ( strNewString != String.Empty )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_1111!!!! Incorrect string content.... Expected...{0},  Actual...{1}", String.Empty, strNewString );
       }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       strNewString = new String(p1 , 1, iArrInvalidValues[iLoop]);
       iCountErrors++;
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     if ( strNewString != String.Empty )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_3333!!!! Incorrect string content.... Expected...{0},  Actual...{1}", String.Empty, strNewString );
       }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       strNewString = new String(p1 , iArrLargeValues[iLoop] , 10);
       Console.WriteLine( strNewString );
       if ( strNewString.Length != iArrLargeValues[iLoop] )
	 {
	 iCountErrors++;
	 Console.Error.WriteLine( "Error_5555!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrLargeValues[iLoop], strNewString.Length );
	 }
       Console.WriteLine("String content..." + strNewString );
       } catch ( ArgumentOutOfRangeException ){
       Console.WriteLine("Info_1000!!!!! Expected exception occured");
       } catch ( ArgumentException ){
       iCountErrors++ ;
       Console.WriteLine("Error_5492!!!! Unexpected expection occured"); 
       } catch ( Exception ex )	 {
       Console.Error.WriteLine( "Error_4444!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       strNewString = new String(p1 , 1 , iArrLargeValues[iLoop] );
       if ( strNewString.Length != iArrLargeValues[iLoop] )
	 {
	 iCountErrors++;
	 Console.Error.WriteLine( "Error_6666!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrLargeValues[iLoop], strNewString.Length );
	 }
       } catch ( ArgumentOutOfRangeException ){  
       } catch ( OutOfMemoryException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     int iString_Length = 10000;
     SByte* p2 = stackalloc SByte[2*iString_Length];
	 byte[] p3 = new byte[2*iString_Length];
     for (int i = 0; i < 2*iString_Length; i++) p2[i] = (SByte)(i%65000);
	 for (int i = 0; i < 2*iString_Length; i++) p3[i] = (byte)(i%65000);
     int[] iArrLengths = new int[]{ 1 , iString_Length , iString_Length -1 , iString_Length/2 , iString_Length/10 }; 
	 for(int iLoop = 0 ; iLoop < iArrLengths.Length ; iLoop++ )
	 {
	   try
	   {                            
		   strNewString = new String(p2 , iString_Length, iArrLengths[iLoop]);
		   int LengthShouldBe = Encoding.Default.GetCharCount(p3, iString_Length, iArrLengths[iLoop]); 
		   //if (strNewString.Length != iArrLengths[iLoop] )
		   if (strNewString.Length != LengthShouldBe)
		   {
			   iCountErrors++;
			   Console.Error.WriteLine( "Error_8888!!!! Incorrect string Length.... Expected...{0},  Actual...{1}", String.Empty.Length, strNewString.Length );
		   }
	   } 
	   catch ( ArgumentOutOfRangeException )
	   {
	   } 
	   catch ( Exception ex )
	   {
		   Console.Error.WriteLine( "Error_9999!!! Unexpected exception " + ex.ToString() );
		   iCountErrors++ ;
	   }
	 }

     iCountTestcases++;
     str2 = new String(chptr);
     if(!str2.Equals(String.Empty))
       {
       iCountErrors++;
       printerr( "Error_498ch! incorrect string returned for null argument=="+str2);
       } 
     strLoc = "Loc_982hc";
     iCountTestcases++;
     char[] testStr = new char[1];
     byte[] bytes = Encoding.Default.GetBytes(testStr, 0, 1);
     String testString = new String(testStr);
     fixed(byte *pbytes = bytes ) {
     sbyte* sbytePtr = (sbyte*)pbytes;
     str2 = new String (sbytePtr, 0, 1);
     }
     if(!str2.Equals(testString))
       {
       iCountErrors++;
       printerr( "Error_49hcx! incorrect string constructed=="+str2+ "Correct string="+testString);
       }
     strLoc = "Loc_9fh56";
     iCountTestcases++;
     sbyte* p = stackalloc sbyte[10];
     for (int i = 0; i < 10; i++) p[i] = (sbyte)(i + (int)'0');
     for(int i = 0 ; i < 5 ; i++)
       {
       strLoc = "Loc_9fh56.4";
       str1 += ((sbyte) i);
       }
     strLoc = "Loc_93286";
     iCountTestcases++;
     str2 = new String (p, 0, 5);
     strLoc = "Loc_93287";
     if(!str2.Equals(str1))
       { 
       iCountErrors++;
       printerr( "Error_209xu! Expected=="+str1+"\nIncorrect string=="+str2);
       }
     strLoc = "Loc_93286.2";
     iCountTestcases++;
     try {
     str2 = new String (p, -1, 5);
     iCountErrors++;
     printerr( "Error_209xu.2! Expected ArgExc instead Returned string =="+str2);
     }
     catch (ArgumentException ) {
     }
     strLoc = "Loc_93286.3";
     iCountTestcases++;
     try {
     str2 = new String (p, 71, 5); 
     }
     catch (ArgumentException ) {
     }
     strLoc = "Loc_93286.4";
     iCountTestcases++;
     try {
     str2 = new String (p, 1, 5); 
     }
     catch (ArgumentException ) {
     }
     strLoc = "Loc_93286.5";
     iCountTestcases++;
     try {
     str2 = new String (p, 1, -1);
     iCountErrors++;
     printerr( "Error_209xu.5! Expected ArgExc instead Returned string =="+str2);
     }
     catch (ArgumentException ) {
     }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co4827ctor_sbyte_int_int cbA = new Co4827ctor_sbyte_int_int();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
