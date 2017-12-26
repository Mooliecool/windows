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
using System.Collections;
using System.Text;
public class Co3148ctor_CharArrIntInt
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "String\\Co3148ctor_CharArrIntInt. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   char[] cArr2 = null;
   int startIndex = 0;
   int length = 0;
   String str8 = null;
   String str9 = null;
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };                      
   iCountTestcases++;
   String strNewString = String.Empty ;
   char[] cArr = new char[256];
   for (int i = 0; i < 256; i++) cArr[i] = (char)i;
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {                                                                                                              
     strNewString = new String(cArr , iArrInvalidValues[iLoop], 1);
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
     strNewString = new String(cArr , 1, iArrInvalidValues[iLoop]);
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
     strNewString = new String(cArr , iArrLargeValues[iLoop] , 1);
     if ( strNewString.Length != iArrLargeValues[iLoop] )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_5555!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrLargeValues[iLoop], strNewString.Length );
       }
     Console.WriteLine("String content..." + strNewString );
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_4444!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     strNewString = new String(cArr , 1 , iArrLargeValues[iLoop] );
     if ( strNewString.Length != iArrLargeValues[iLoop] )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_6666!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrLargeValues[iLoop], strNewString.Length );
       }
     } catch ( ArgumentOutOfRangeException ){                            
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   int iString_Length = 10000;
   char[] p2 = new char[ iString_Length];
   for (int i = 0; i < iString_Length; i++) p2[i] = (char)(i%65000);
   int[] iArrLengths = new int[]{ 1 , iString_Length , iString_Length -1 , iString_Length/2 , iString_Length/10 }; 
   for(int iLoop = 0 ; iLoop < iArrLengths.Length ; iLoop++ ){
   try
     {                            
     strNewString = new String(p2 , iString_Length, iArrLengths[iLoop]);
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_9999!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   if ( strNewString.Length != 0 )
     {
     iCountErrors++;
     Console.Error.WriteLine( "Error_8888!!!! Incorrect string Length.... Expected...{0},  Actual...{1}", String.Empty.Length, strNewString.Length );
     }
   }
   iCountTestcases++;
   try {
   str9 = new String(cArr2, startIndex, length);
   iCountErrors++;
   print("E_31kk");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_39mz");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   cArr2 = new char[10];
   startIndex = -1;
   length = 0;
   iCountTestcases++;
   try {
   str9 = new String(cArr2, startIndex, length);
   iCountErrors++;
   print("E_328i");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_7auo");
     strError = "EXTENDEDINFO: ";
     strError = strError + exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   startIndex = 11;
   length = 3;
   iCountTestcases++;
   try {
   str9 = new String(cArr2, startIndex, length);
   iCountErrors++;
   print("E_489a");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_34zm");
     Console.Error.WriteLine(exc2.ToString());
     }
   cArr2 = new char[10];
   startIndex = 0;
   length = -4;
   iCountTestcases++;
   try {
   str9 = new String(cArr2, startIndex, length);
   iCountErrors++;
   print("E_329w");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_38sn");
     Console.Error.WriteLine(exc2.ToString());
     }
   cArr2 = new char[10];
   startIndex = 0;
   length = 15;
   iCountTestcases++;
   try {
   str9 = new String(cArr2, startIndex, length);
   iCountErrors++;
   print("E_12xj");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_39ds");
     Console.Error.WriteLine(exc2.ToString());
     }
   cArr2 = new char[10];
   for(int i=0;i<10;i++)
     cArr2[i] = '\0';
   StringBuilder strb = new StringBuilder("\0\0\0\0\0\0\0\0\0\0");
   str9 = new String(cArr2, 0, cArr2.Length);
   str8 = strb.ToString();
   iCountTestcases++;
   if(!str9.Equals(str8)) 
     {
     iCountErrors++;
     print("E_57ai");
     }
   cArr2 = new char[10];
   cArr2[0] = 'a';
   cArr2[1] = 'b';
   cArr2[2] = 'c';
   cArr2[3] = 'd';
   cArr2[4] = 'e';
   cArr2[5] = 'f';
   cArr2[6] = 'g';
   cArr2[7] = 'h';
   cArr2[8] = 'i';
   cArr2[9] = 'j';
   str9 =  new String(cArr2, 0, cArr2.Length) ;
   str8 = "abcdefghij";
   iCountTestcases++;
   if ( ! str9.Equals( str8 ) )
     {
     iCountErrors ++;
     print("E_89is");
     }
   cArr2 = new char[10];
   cArr2[0] = 'a';
   cArr2[1] = 'b';
   cArr2[2] = 'c';
   cArr2[3] = 'd';
   cArr2[4] = 'e';
   cArr2[5] = 'f';
   cArr2[6] = 'g';
   cArr2[7] = 'h';
   cArr2[8] = 'i';
   cArr2[9] = 'j';
   str9 = new String(cArr2, 0, 5);
   str8 = "abcde";
   iCountTestcases++;
   if(!str9.Equals(str8))
     {
     iCountErrors++;
     print("E_47nd");
     }
   cArr2 = new char[10];
   cArr2[0] = 'a';
   cArr2[1] = 'b';
   cArr2[2] = 'c';
   cArr2[3] = 'd';
   cArr2[4] = 'e';
   cArr2[5] = 'f';
   cArr2[6] = 'g';
   cArr2[7] = 'h';
   cArr2[8] = 'i';
   cArr2[9] = 'j';
   str9 = new String(cArr2, 5, 5);
   str8 = "fghij";
   iCountTestcases++;
   if(!str9.Equals(str8))
     {
     iCountErrors++;
     print("E_31sj");
     }
   IntlStrings intl = new IntlStrings();
   String testString = intl.GetString(8, true, true);
   cArr2 = testString.ToCharArray();
   str9 = new String(cArr2, 0, 8);
   if(! str9.Equals(testString)) {
   ++iCountErrors;
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3148ctor_CharArrIntInt.)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3148ctor_CharArrIntInt cb0 = new Co3148ctor_CharArrIntInt();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3148ctor_CharArrIntInt."  );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
