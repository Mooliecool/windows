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
using System.Text;
using System;
public class Co4250Parse
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co4250Parse.cs  runTest started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   float so2Result;
   float so3Expect;
   String str4 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_100mm";
       str4 = "12.3";
       so3Expect = (float)12.3;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result < (so3Expect - so3Expect/(float)100.0)
	   ||  so2Result > (so3Expect + so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_101sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_110mm";
       str4 = "-12.3";  
       so3Expect = (float)-12.3;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result < (so3Expect + so3Expect/(float)100.0)
	   ||  so2Result > (so3Expect - so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_111sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_120mm";
       str4 = "-0.0";
       so3Expect = (float)0.0;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if ( so2Result != so3Expect )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_121sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_562gy";
       ++iCountTestcases;
       str4 = "-1.79769313486231570E+308";  
       try{
       so2Result = Single.Parse( str4 );
       ++iCountErrors;
       Console.Error.WriteLine(  "Err_3wrg! No exception thrown, ");
       }catch(OverflowException){
       }catch(Exception ex){
       ++iCountErrors;
       Console.Error.WriteLine(  "Err_3wrg! Wrong exception thrown, " + ex.GetType().Name  );
       }
       strLoc="Loc_561ii";
       ++iCountTestcases;
       str4 = "1.79769313486231570E+308";  
       so3Expect = (float) 1.79769313486231570E+308;
       try{
       so2Result = Single.Parse( str4 );
       ++iCountErrors;
       Console.Error.WriteLine(  "Err_3wrg! No exception thrown, ");
       }catch(OverflowException){
       }catch(Exception ex){
       ++iCountErrors;
       Console.Error.WriteLine(  "Err_3wrg! Wrong exception thrown, " + ex.GetType().Name  );
       }
       strLoc="Loc_200mm";
       str4 = "-3.40282347E+38 ";  
       so3Expect = (float)-3.40282347E+38;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result != so3Expect
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_201sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_210mm";
       str4 = "3.40282347E+38";  
       so3Expect = (float)3.40282347E+38;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result != so3Expect
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_211sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_300mm";
       str4 = "-2.0E+18";  
       so3Expect = (float)-2.0E+18;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result < (so3Expect + so3Expect/(float)100.0)
	   ||  so2Result > (so3Expect - so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_301sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_310mm";
       str4 = "-2";  
       so3Expect = (float)-2.0;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result < (so3Expect + so3Expect/(float)100.0)
	   ||  so2Result > (so3Expect - so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_311sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_320mm";
       str4 = "-23.4";
       so3Expect = (float)-23.4;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result < (so3Expect + so3Expect/(float)100.0)
	   ||  so2Result > (so3Expect - so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_321sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_330mm";
       str4 = "09";  
       so3Expect = (float)09.0;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result < (so3Expect - so3Expect/(float)100.0)
	   ||  so2Result > (so3Expect + so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_331sg!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_340mm";
       str4 = "0xA";  
       so3Expect = (float)0.0;
       try
	 {
	 ++iCountTestcases;
	 so2Result = Single.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_7ff32!  so2Result==" + so2Result  );
	 }
       catch ( FormatException fexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_98frsg!  exc==" + exc  );
	 }
       strLoc="Loc_400mm";
       str4 = "-";  
       so2Result = (float)2.0;
       try
	 {
	 ++iCountTestcases;
	 so2Result = Single.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_402hu!  so2Result==" + so2Result  );
	 }
       catch ( FormatException fexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_401sg!  exc==" + exc  );
	 }
       strLoc="Loc_410mm";
       str4 = ".";  
       so2Result = (float)2.0;
       try
	 {
	 ++iCountTestcases;
	 so2Result = Single.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_412hu!  so2Result==" + so2Result  );
	 }
       catch ( FormatException fexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_411sg!  exc==" + exc  );
	 }
       strLoc="Loc_420mm";
       str4 = "";  
       so2Result = (float)2.0;
       try
	 {
	 ++iCountTestcases;
	 so2Result = Single.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_422hu!  so2Result==" + so2Result  );
	 }
       catch ( FormatException fexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_421sg!  exc==" + exc  );
	 }
       strLoc="Loc_430mm";
       str4 = " - 2 3 . 4 mnop ";  
       so2Result = (float)2.0;
       try
	 {
	 ++iCountTestcases;
	 so2Result = Single.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_432hu!  so2Result==" + so2Result  );
	 }
       catch ( FormatException fexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_431sg!  exc==" + exc  );
	 }
       strLoc="Loc_440mm";
       str4 = "2.79769313486231570E+309";  
       so2Result = (float)2.0;
       ++iCountTestcases;
       try{
       so2Result = Single.Parse( str4 );
       }catch(OverflowException){
       }catch(Exception ex){
       ++iCountErrors;
       Console.Error.WriteLine(  "Err_3wrg! Wrong exception thrown, " + ex.GetType().Name  );
       }
       strLoc="Loc_500mm";
       str4 = null;  
       so2Result = (float)2.0;
       try
	 {
	 ++iCountTestcases;
	 so2Result = Single.Parse( str4 );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_502hu!  so2Result==" + so2Result  );
	 }
       catch ( ArgumentNullException aexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_501sg!  exc==" + exc  );
	 }
       strLoc="Loc_510dt";
       str4 = "-.4";
       so3Expect = (float)-00.4;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result < (so3Expect + so3Expect/(float)100.0)
	   ||  so2Result > (so3Expect - so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_000dt!  so2Result==" + so2Result  );
	 }
       strLoc="Loc_520dt";
       str4 = "+00000000000000000000000000000000000000000.4";
       so3Expect = (float)0.4;
       so2Result = Single.Parse( str4 );
       ++iCountTestcases;
       if (
	   so2Result > (so3Expect + so3Expect/(float)100.0)
	   ||  so2Result < (so3Expect - so3Expect/(float)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_001sg!  so2Result==" + so2Result  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4250Parse) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Single\\Co4250Parse.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Single\\Co4250Parse.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co4250Parse cbA = new Co4250Parse();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4250Parse) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co4250Parse.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
