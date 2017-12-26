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
using System.Globalization;
using System.Globalization;
using System;
public class Co1645Parse
{
 public virtual String LongNumberString( int iNum )
   {
   String strBuild;
   if ( iNum == 1 )
     {
     Random rand = new Random();
     return rand.Next().ToString();
     }
   strBuild = LongNumberString( iNum - 1 ) + LongNumberString( iNum - 1 );
   return strBuild;
   }
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1645Parse.cs  runTest started." );
   Console.Error.WriteLine();
   Console.Error.WriteLine( "CAUTION:  DateTimeFormatInfo/Co1645* may exist too, duplicate first 6, darn." );
   Console.Error.WriteLine();
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double do2Result;
   double do3Expect;
   Double dTest1 = 0;
   Double dTest2 = 0;
   String str4 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_100mm";
       str4 = "12.3";
       do3Expect = (double)12.3;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result < (do3Expect - do3Expect/(double)100.0)
	   ||  do2Result > (do3Expect + do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_101sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_110mm";
       str4 = "-12.3";  
       do3Expect = (double)-12.3;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result < (do3Expect + do3Expect/(double)100.0)
	   ||  do2Result > (do3Expect - do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_111sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_120mm";
       str4 = "-0.0";
       do3Expect = (double)0.0;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if ( do2Result != do3Expect )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_121sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_200mm";
       str4 = "-1.79769313486231570E+308";  
       do3Expect = (double)-1.79769313486231570E+308;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result > (do3Expect - do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_201sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_210mm";
       str4 = "1.79769313486231570E+308";  
       do3Expect = (double)1.79769313486231570E+308;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result < (do3Expect - do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_211sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_300mm";
       str4 = "-2.0E+98";  
       do3Expect = (double)-2.0E+98;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result < (do3Expect + do3Expect/(double)100.0)
	   ||  do2Result > (do3Expect - do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_301sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_310mm";
       str4 = "-2";  
       do3Expect = (double)-2.0;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result < (do3Expect + do3Expect/(double)100.0)
	   ||  do2Result > (do3Expect - do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_311sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_320mm";
       str4 = " -23.4mnop ";  
       do3Expect = (double)(-23.4);
       ++iCountTestcases;
       try {
       do2Result = Double.Parse( str4 );
       if (
	   do2Result < (do3Expect + do3Expect/(double)100.0)
	   ||  do2Result > (do3Expect - do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_321sg!  do2Result==" + do2Result  );
	 }
       } catch (FormatException fExc) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.Error.WriteLine(  "POINTTOBREAK: Err_485ws! exc=="+exc);
       }
       strLoc="Loc_330mm";
       str4 = "09";  
       do3Expect = (double)09.0;
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result < (do3Expect - do3Expect/(double)100.0)
	   ||  do2Result > (do3Expect + do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_331sg!  do2Result==" + do2Result  );
	 }
       strLoc="Loc_340mm";
       str4 = "0xA";  
       do3Expect = (double)0.0;
       try {
       do2Result = Double.Parse( str4 );
       ++iCountTestcases;
       if (
	   do2Result < (do3Expect + do3Expect/(double)100.0)
	   ||  do2Result > (do3Expect - do3Expect/(double)100.0)
    	   )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_341sg!  do2Result==" + do2Result  );
	 }
       } catch (FormatException fExc) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.Error.WriteLine( "POINTTOBREAK: Err_519qh! exc=="+exc);
       }
       strLoc="Loc_400mm";
       str4 = "-";  
       do2Result = (double)2.0;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_402hu!  do2Result==" + do2Result  );
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
       do2Result = (double)2.0;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_412hu!  do2Result==" + do2Result  );
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
       do2Result = (double)2.0;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_422hu!  do2Result==" + do2Result  );
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
       do2Result = (double)2.0;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_432hu!  do2Result==" + do2Result  );
	 }
       catch ( FormatException fexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_431sg!  exc==" + exc  );
	 }
       strLoc="Loc_450mm";
       str4 = null;
       do2Result = (double)2.0;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_502hu!  do2Result==" + do2Result  );
	 }
       catch ( ArgumentException aexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_501sg!  exc==" + exc  );
	 }
       strLoc="Loc_460mm";
       str4 = ".9";
       do3Expect = (double).9;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if (
	     do2Result < (do3Expect - do3Expect/(double)100.0)
	     ||  do2Result > (do3Expect + do3Expect/(double)100.0)
	     )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_341ct!  do2Result==" + do2Result  );
	   }
	 }
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_342ct!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_470mm";
       str4 = ".9";
       do3Expect = (double).9;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if (
	     do2Result < (do3Expect - do3Expect/(double)100.0)
	     ||  do2Result > (do3Expect + do3Expect/(double)100.0)
	     )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_341ct!  do2Result==" + do2Result  );
	   }
	 }
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_342ct!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_480mm";
       str4 = ".9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
       do3Expect = (double).9;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if (
	     do2Result < (do3Expect - do3Expect/(double)100.0)
	     ||  do2Result > (do3Expect + do3Expect/(double)100.0)
	     )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_341ct!  do2Result==" + do2Result  );
	   }
	 }
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_342ct!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_490mm";
       str4 = ".00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
       do3Expect = (double)0;
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if (
	     do2Result < (do3Expect - do3Expect/(double)100.0)
	     ||  do2Result > (do3Expect + do3Expect/(double)100.0)
	     )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_341ct!  do2Result==" + do2Result  );
	   }
	 }
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_342ct!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_500mm";
       str4 = "..";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_341st!  expected exception"  );
	 }
       catch ( FormatException exarg )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_342st!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_510mm";
       str4 = "";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_341pt!  expected exception"  );
	 }
       catch ( FormatException exarg )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_342pt!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_520mm";
       str4 = "--1";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_342jt!  expected exception"  );
	 }
       catch ( FormatException exarg )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343jt!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_530mm";
       str4 = "-1.-2";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_344jt!  expected exception"  );
	 }
       catch ( FormatException exarg )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_345jt!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_440mm";
       str4 = "2.79769313486231570E+309";  
       do2Result = (double)2.0;
       ++iCountTestcases;
       try{
       do2Result = Double.Parse( str4 );
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error Err_346jt! EXcepiton not thrown, ");
       }catch(OverflowException){
       }catch(Exception ex){
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error Err_346jt! EXcepiton thrown, " + ex.GetType().Name );
       }
       strLoc="Loc_540mm";
       str4 = LongNumberString(17);
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if ( ! Double.IsPositiveInfinity( do2Result ) )
	   {
       	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_346jt! number should be infinity! but it is " + do2Result );
	   }
	 }catch(OverflowException){
	 }
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_347jt!  exc==" + exc.ToString()  );
	   }
       strLoc="Loc_550mm";
       str4 = LongNumberString(17);
       str4 += "e+999";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if ( ! Double.IsPositiveInfinity( do2Result ) )
	   {
       	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_948jt! number should be infinity! but it is " + do2Result );
	   }
	 }catch(OverflowException){
	 }
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_949jt!  exc==" + exc.ToString()  );
	   }
       strLoc="Loc_560mm";
       str4 = "1e99999999999999999999999999999";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if ( ! Double.IsPositiveInfinity( do2Result ) )
	   {
       	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_348jt! number should be infinity! but it is " + do2Result );
	   }
	 }catch(OverflowException){
	 }
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_349jt!  exc==" + exc.ToString()  );
	   }
       strLoc="Loc_570mm";
       str4 = "1e-99999999999999999999999999999";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if ( do2Result != 0 )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_348jt! number should be infinity! but it is " + do2Result );
	   }
	 }catch(OverflowException){
	 }
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_349jt!  exc==" + exc.ToString()  );
	   }
       strLoc="Loc_580mm";
       str4 = "42341431fsdklj;lkewjlrkwejropweqiutoipj;krejwl;rjlkqjlkfjlakjlkn;lvncxl;zn;lnadl;jhl;234324324";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_001jt!  expected exception"  );
	 }
       catch ( FormatException exarg )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_002jt!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_590mm";
       str4 = "(4)";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_003jt!  expected exception"  );
	 }
       catch ( FormatException exarg )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_004jt!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_600mm";
       str4 = "-   -4";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_005jt!  expected exception " + do2Result   );
	 }
       catch ( FormatException exarg )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_006jt!  exc==" + exc.ToString()  );
	 }
       strLoc="Loc_610mm";
       str4 = "-1e99999999999999999999999999999";
       try
	 {
	 ++iCountTestcases;
	 do2Result = Double.Parse( str4 );
	 if ( ! Double.IsNegativeInfinity( do2Result ) )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_007jt! number should be infinity! but it is " + do2Result );
	   }
	 }catch(OverflowException){
	 }
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error Err_008jt!  exc==" + exc.ToString()  );
	   }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1645Parse) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Double\\Co1645Parse.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Double\\Co1645Parse.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1645Parse cbA = new Co1645Parse();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1645Parse) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1645Parse.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
