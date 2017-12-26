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
using GenStrings ;
public class Co3441Compare_StrIntStrIntIntBoolLoc
{
 static String strName = "String.Compare";
 static String strTest = "Co3441Compare_StrIntStrIntIntBoolLoc";
 static String strPath = "";
 public Boolean runTest()
   {
   String strInfo = null;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   Console.Error.Write( strTest );
   Console.Error.WriteLine( " runTest started..." );
   Console.Error.WriteLine(  "CAUTION" +"POINTTOBREAK:  C_358uc.  ReCheck validity of Japanese testcases."  );
   String strAlphabetsLower = "abcdefghijklmnopqrstuvwxyz";
   String strAlphabetsUpper = "ABCDEFGHGIJKLMNOPQRSTUVWXYZ";
   String str1 = null;
   String str2 = null;
   CultureInfo[] culInfos = new CultureInfo[3];
   CultureInfo locUSEnglish = null; 
   CultureInfo locGDR = null;  
   CultureInfo locJPN = null;  
   IntlStrings intl = new IntlStrings();
   String intlString = intl.GetString( 20, true, true );
   str1 = intlString ;
   do
     {
     Console.Error.WriteLine( "[] Create locales" ); 
     ++iCountTestcases;
     try
       {
       locUSEnglish = new CultureInfo("en-us");
       if ( locUSEnglish == null )
	 {
	 Console.WriteLine( strTest+ "E_101" );
	 Console.WriteLine( strTest+ "Failed to construct default locUSEnglish object" );
	 ++iCountErrors;
	 break;
	 } else culInfos[0] = locUSEnglish ;
       locGDR = new CultureInfo( 1031 ); 
       if ( locGDR == null )
	 {
	 Console.WriteLine( strTest+ "E_303" );
	 Console.WriteLine( strTest+ "Failed to construct German locale object" );
	 ++iCountErrors;
	 break;
	 }else culInfos[1] = locGDR ;
       locJPN = new CultureInfo( 1041 ); 
       if ( locJPN == null )
	 {
	 Console.WriteLine( strTest+ "E_404" );
	 Console.WriteLine( strTest+ "Failed to construct Japanese locale object" );
	 ++iCountErrors;
	 break;
	 } else culInfos[2] = locJPN ;
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_505" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     int iResult = 0;
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
     int[] iArrValidValues = new Int32[]{ 10000, 5000 , 1000 , 0 };
     for(int jLoop = 0 ; jLoop < culInfos.Length ; jLoop++ ){
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       iResult = String.Compare(intlString , iArrInvalidValues[iLoop], str1, 5 , 5, true , culInfos[jLoop]);
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
       iResult = String.Compare(intlString , 5, str1, iArrInvalidValues[iLoop] , 5, false , culInfos[jLoop]);
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
       iResult = String.Compare(intlString , 5, str1, 5 , iArrInvalidValues[iLoop], true , culInfos[jLoop]);
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
       iResult = String.Compare(intlString , iArrLargeValues[iLoop], str1, 5 , 5, false , culInfos[jLoop]);
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
       iResult = String.Compare(intlString , 5, str1, iArrLargeValues[iLoop] , 5, true , culInfos[jLoop]);
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
       iResult = String.Compare(intlString , 5, str1, 5 , iArrLargeValues[iLoop], false , culInfos[jLoop]);
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
       iResult = String.Compare(intlString , iLoop * 100, str1, iLoop*100 , iArrValidValues[iLoop], true , culInfos[jLoop]);
       if ( iResult != 0 ){
       iCountErrors++;
       Console.Error.WriteLine( "Error_6666!!!! Compare returned incorrect value.... Expected...{0},  Actual...{1}", 0, iResult );
       }
       } catch ( Exception ex ){
       Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     }
     Console.Error.WriteLine( "[] Compare string using U.S. English locale object" ); 
     try
       {
       ++iCountTestcases;
       str1 = strAlphabetsLower;
       if ( String.Compare(str1, 5, strAlphabetsLower, 5, 10, false, locUSEnglish) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1010" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       str1 = strAlphabetsLower;
       str2 = str1.ToUpper();
       ++iCountTestcases;
       if ( String.Compare(str1, 5, str2, 5, 10, true, locUSEnglish) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_323vq" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsLower;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, strAlphabetsLower, 6, 10, false, locUSEnglish) >= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_094qp" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsUpper;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, strAlphabetsUpper, 4, 10, true, locUSEnglish) <= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_033pw" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       try {
       str1 = strAlphabetsUpper;
       str2 = null;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, str2, 5, 10, false, locUSEnglish) != 1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_053jh" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       } catch( ArgumentOutOfRangeException ) {
       } catch(Exception e) {Console.WriteLine(e.ToString() + "FAILURES" + iCountErrors);}
       str1 = null;
       str2 = strAlphabetsLower;
       ++iCountTestcases;
       try {
       if ( String.Compare(str1, 5, str2, 8, 23, true, locUSEnglish) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;
	 Console.WriteLine( strTest+ strInfo );
	 } 
       }catch(ArgumentOutOfRangeException ){
       } catch(Exception e) {Console.WriteLine( "Exception comes from here..." + e.ToString());}
       str1 = strAlphabetsLower;
       str2 = strAlphabetsUpper;
       try
	 {
	 ++iCountTestcases;
	 String.Compare(str1, -1, str2, 5, 10, false, locUSEnglish);
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
	 String.Compare(str1, 5, str2, 5, -1, true, locUSEnglish);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_754lk" ;
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
	 strInfo = "FAiL. E_720sd" ;
	 strInfo += ", Wrong Exception thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsLower;
       str2 = strAlphabetsLower;
       try
	 {
	 ++iCountTestcases;
	 String.Compare(str1, 5, str2, 27, 1, true, locUSEnglish);
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
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.WriteLine( strTest+ "E_1313" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       break;
       }
     Console.Error.WriteLine( "[] Compare strings using German locale object" ); 
     try
       {
       ++iCountTestcases;
       str1 = strAlphabetsLower;
       if ( String.Compare(str1, 5, strAlphabetsLower, 5, 10, false, locGDR) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1515" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       str1 = strAlphabetsLower;
       str2 = str1.ToUpper();
       ++iCountTestcases;
       if ( String.Compare(str1, 5, str2, 5, 10, true, locGDR) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_323vq" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsLower;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, strAlphabetsLower, 6, 10, false, locGDR) >= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_094qp" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsUpper;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, strAlphabetsUpper, 4, 10, true, locGDR) <= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_033pw" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       try {
       str1 = strAlphabetsUpper;
       str2 = null;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, str2, 5, 10, false, locGDR) != 1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_053jh" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       str2 = strAlphabetsLower;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, str2, 8, 23, true, locGDR) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;				
	 Console.WriteLine( strTest+ strInfo );
	 }
       } catch(Exception) {}
       str1 = strAlphabetsLower;
       str2 = strAlphabetsUpper;
       try
	 {
	 ++iCountTestcases;
	 String.Compare(str1, -1, str2, 5, 10, false, locGDR);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_982ww" ;
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
	 strInfo = "FAiL. E_223mn" ;
	 strInfo += ", Wrong Exception thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsLower;
       str2 = strAlphabetsLower;
       try
	 {
	 ++iCountTestcases;
	 String.Compare(str1, 5, str2, 5, -22, true, locGDR);
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
	 String.Compare(str1, 5, str2, 27, 1, true, locGDR);
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
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.WriteLine( strTest+ "E_1818" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       break;
       }
     Console.Error.WriteLine( "[] Compare strings using Japanese locale object" ); 
     try
       {
       ++iCountTestcases;
       str1 = strAlphabetsLower;
       if ( String.Compare(str1, 5, strAlphabetsLower, 5, 10, false, locJPN) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1010" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       str1 = strAlphabetsLower;
       str2 = str1.ToUpper();
       ++iCountTestcases;
       if ( String.Compare(str1, 5, str2, 5, 10, true, locJPN) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_323vq" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsLower;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, strAlphabetsLower, 6, 10, false, locJPN) >= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_094qp" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsUpper;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, strAlphabetsUpper, 4, 10, true, locJPN) <= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_033pw" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsUpper;
       str2 = null;
       ++iCountTestcases;
       if ( String.Compare(str1, 5, str2, 5, 10, false, locJPN) != 1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_053jh" ;				
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       str2 = strAlphabetsLower;
       ++iCountTestcases;
       if (String.Compare(str1, 5, str2, 8, 23, true, locJPN) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;				
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsLower;
       str2 = strAlphabetsUpper;
       try
	 {
	 ++iCountTestcases;
	 String.Compare(str1, -1, str2, 5, 10, false, locJPN);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_982ww" ;
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
	 strInfo = "FAiL. E_223mn" ;
	 strInfo += ", Wrong Exception thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsLower;
       str2 = strAlphabetsLower;
       try
	 {
	 ++iCountTestcases;
	 String.Compare(str1, 5, str2, 5, -5, true, locJPN);
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
	 String.Compare(str1, 5, str2, 27, 1, true, locJPN);
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
     catch ( Exception )
       {
       Console.Error.WriteLine(  "CAUTION" +"POINTTOBREAK:  Should not throw Exception! RECHECK Later "  );
       }
     }while ( false );
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3441Compare_StrIntStrIntIntBoolLoc oCbTest = new Co3441Compare_StrIntStrIntIntBoolLoc();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.WriteLine( strTest+ strPath );
     Console.WriteLine( strTest+ "E_1000000" );
     Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
     Console.WriteLine( strTest+ ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
