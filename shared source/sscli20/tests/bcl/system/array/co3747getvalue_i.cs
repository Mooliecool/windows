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
using System.Text;
public class Co3747GetValue_i
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.GetValue( int )";
 public static String s_strTFName        = "Co3747GetValue_i";
 public static String s_strTFAbbrev      = "Co3747";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int[] in4Array1Target = null; 
   int in4Index = -1;
   Object var3;
   String str3 = null;
   String[] str2Arr = null; 
   Int16[] in2Arr; 
   Int32[] in4Arr; 
   Int64[] in8Arr; 
   Single[] sglArr; 
   Double[] dblArr; 
   Byte[] bytArr; 
   SByte[] sbytArr; 
   Decimal[] currArr; 
   Decimal[] decArr; 
   try
     {
     do
       {
       strLoc="Loc_110dt";
       in4Array1Target = new int[ (int)Byte.MaxValue + 22 ];  
       for ( int ia = 0 ;ia < in4Array1Target.Length ;ia++ )
	 {
	 in4Array1Target[ia] = ia;
	 }
       in4Index =  (int)Byte.MaxValue + 6;
       var3 = in4Array1Target.GetValue( in4Index );
       ++iCountTestcases;
       if ( !var3.Equals(in4Index) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_342ji!  var3=="+ var3 );
	 }
       strLoc="Loc_325kf";
       str2Arr = new String[5];
       str2Arr.SetValue( "zero", 0 );
       str3 = (String)str2Arr.GetValue( 0 );
       ++iCountTestcases;
       if ( str3.Equals( str2Arr[0] ) != true )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_084nw!  str2Arr[0]=="+ str2Arr[0] +" , str3=="+ str3 );
	 }
       strLoc = "Loc_374wq";
       bytArr = new Byte[2];
       bytArr[0] = (Byte)23;
       bytArr[1] = (Byte)5;
       iCountTestcases++;
       if(!bytArr.GetValue(0).Equals((Byte)23) || !bytArr.GetValue(1).Equals((Byte)5))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_23093! , bytArr[0]=="+bytArr.GetValue(0)+" bytArr[1]=="+bytArr.GetValue(1));
	 }
       strLoc = "Loc_4281i";
       sbytArr = new SByte[2];
       sbytArr[0] = (SByte)(-5);
       sbytArr[1] = (SByte)4;
       iCountTestcases++;
       if(!sbytArr.GetValue(0).Equals((SByte)(-5)) || !sbytArr.GetValue(1).Equals((SByte)4))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_299eu! , bytArr[0]=="+bytArr.GetValue(0)+", bytArr[1]=="+bytArr.GetValue(1));
	 }
       strLoc = "Loc_388uq";
       in2Arr = new Int16[2];
       in2Arr[0] = (short)3;
       in2Arr[1] = (short)20;
       iCountTestcases++;
       if(!in2Arr.GetValue(0).Equals((short)3) || !in2Arr.GetValue(1).Equals((short)20))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_9823u");
	 }
       strLoc = "Loc_982ew";
       in4Arr = new Int32[2];
       in4Arr[0] = 5;
       in4Arr[1] = 200;
       iCountTestcases++;
       if(!in4Arr.GetValue(0).Equals(5) || !in4Arr.GetValue(1).Equals(200))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_398yw");
	 }
       strLoc = "Loc_123uq";
       in8Arr = new Int64[2];
       in8Arr[0] = 32;
       in8Arr[1] = 878;
       iCountTestcases++;
       if(!in8Arr.GetValue(0).Equals((long)32) || !in8Arr.GetValue(1).Equals((long)878))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_488ru! , inArr[0]=="+in8Arr.GetValue(0)+" inArr[1]=="+in8Arr.GetValue(1));
	 }
       strLoc = "Loc_0982i";
       sglArr = new Single[2];
       sglArr[0] = (Single)(-5.5);
       sglArr[1] = (Single)5.5;
       iCountTestcases++;
       if(!sglArr.GetValue(0).Equals((Single)(-5.5)) || !sglArr.GetValue(1).Equals((Single)5.5))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_908ye");
	 }
       strLoc = "Loc_824qu";
       dblArr = new Double[2];
       dblArr[0] = 3.3;
       dblArr[1] = -2.7;
       iCountTestcases++;
       if(!dblArr.GetValue(0).Equals(3.3) || !dblArr.GetValue(1).Equals(-2.7))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_0983u");
	 }
       strLoc = "Loc_877yy";
       currArr = new Decimal[2];
       currArr[0] = (Decimal)(-3.4);
       currArr[1] = (Decimal)(5.5);
       iCountTestcases++;
       if(!currArr.GetValue(0).Equals((Decimal)(-3.4)) || !currArr.GetValue(1).Equals((Decimal)5.5))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_347ai");
	 }
       strLoc = "Loc_283cs";
       decArr = new Decimal[2];
       decArr[0] = (Decimal)(-3.8);
       decArr[1] = (Decimal)2.9;
       iCountTestcases++;
       if(!decArr.GetValue(0).Equals((Decimal)(-3.8)) || !decArr.GetValue(1).Equals((Decimal)2.9))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_5738c");
	 }
       strLoc="Loc_218ux";
       in4Array1Target = new int[3];
       for ( int ia = 0 ;ia < in4Array1Target.Length ;ia++ )
	 {
	 in4Array1Target[ia] = ia;
	 }
       in4Index = -1;
       try
	 {
	 ++iCountTestcases;
	 var3 = in4Array1Target.GetValue( in4Index );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_437sb!  Should have thrown.  var3=="+ var3 );
	 }
       catch ( IndexOutOfRangeException )
	 {}
       catch ( Exception excep )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_132xi!  Wrong exc.  excep=="+ excep );
	 }
       strLoc="Loc_783qn";
       in4Array1Target = new int[3];
       for ( int ia = 0 ;ia < in4Array1Target.Length ;ia++ )
	 {
	 in4Array1Target[ia] = ia;
	 }
       in4Index = in4Array1Target.Length;
       try
	 {
	 ++iCountTestcases;
	 var3 = in4Array1Target.GetValue( in4Index );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_808yl!  Should have thrown.  var3=="+ var3 );
	 }
       catch ( IndexOutOfRangeException )
	 {}
       catch ( Exception excep )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_638an!  Wrong exc.  excep=="+ excep );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   } 
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co3747GetValue_i cbA = new Co3747GetValue_i();
   try
     {
     bResult = cbA.runTest();
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
