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
delegate int Co1162_dlg_int_int_int( int p_i4top ,int p_i4bottom );
delegate void Co1162_dlg_void_();
delegate String Co1162_dlg_String_String( String p_str1 );
delegate Boolean Co1162_dlg_Boolean_Boolean( Boolean p_1 );  
delegate SByte    Co1162_dlg_SByte_SByte      ( SByte   p_1 );
delegate Byte   Co1162_dlg_Byte_Byte    ( Byte   p_1 );
delegate char    Co1162_dlg_char_char      ( char   p_1 );
delegate double  Co1162_dlg_double_double  ( double p_1 );
delegate float   Co1162_dlg_float_float    ( float  p_1 );
delegate long    Co1162_dlg_long_long      ( long   p_1 );
delegate short   Co1162_dlg_short_short    ( short  p_1 );
delegate Int32   Co1162_dlg_OM             ( Int32 parm1 ); 
public class Co1162invoke  
  : Co1162_A1
{
 public static int s_i4a = -2;
 public int returnModuloFirstSecond( int p_i4top ,int p_i4bottom )
   { return ( p_i4top % p_i4bottom ); }
 public void return_void_()
   { Console.Out.WriteLine( "Inside method return_void_() method of Delegate\\Co1162invoke" ); }
 public String return_String_String( String p_str1 )
   { return p_str1; }
 public Boolean return_Boolean_Boolean( Boolean p_1 )
   { return p_1; }
 public SByte return_SByte_SByte( SByte p_1 )
   { return p_1; }
 public Byte return_Byte_Byte( Byte p_1 )
   { return p_1; }
 public char return_char_char( char p_1 )
   { return p_1; }
 public double return_double_double( double p_1 )
   { return p_1; }
 public float return_float_float( float p_1 )
   { return p_1; }
 public long return_long_long( long p_1 )
   { return p_1; }
 private short return_short_short( short p_1 )
   { return p_1; }
 public static int static_returnModuloFirstSecond( int p_i4top ,int p_i4bottom )
   { return ( p_i4top % p_i4bottom ); }
 public static void static_return_void_()
   { Console.Out.WriteLine( "Inside method return_void_() method of Delegate\\Co1162invoke" ); }
 public static String static_return_String_String( String p_str1 )
   { return p_str1; }
 public static Boolean static_return_Boolean_Boolean( Boolean p_1 )
   { return p_1; }
 public static SByte static_return_SByte_SByte( SByte p_1 )
   { return p_1; }
 public static Byte static_return_Byte_Byte( Byte p_1 )
   { return p_1; }
 public static char static_return_char_char( char p_1 )
   { return p_1; }
 public static double static_return_double_double( double p_1 )
   { return p_1; }
 public static float static_return_float_float( float p_1 )
   { return p_1; }
 public static long static_return_long_long( long p_1 )
   { return p_1; }
 private static short static_return_short_short( short p_1 )
   { return p_1; }
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co1162invoke  runTest() started." );
   StringBuilder sblMsg = null;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "L_101";
   int i4a = -2;
   Boolean b2 = false;
   SByte b9 = -2;
   Byte ub9 = 0;
   char c2 = 'A';
   double d2 = -2.2;
   float f2 = (float)-2.2;
   long l2 = (long)-2;  
   short s2 = (short)-2;
   String str1 = null;
   Co1162_dlg_int_int_int dlg_int_int_int = null;
   Co1162_dlg_void_ dlg_void_ = null;
   Co1162_dlg_String_String dlg_String_String = null;
   Co1162_dlg_Boolean_Boolean dlg_Boolean_Boolean = null;
   Co1162_dlg_SByte_SByte dlg_SByte_SByte = null;
   Co1162_dlg_Byte_Byte dlg_Byte_Byte = null;
   Co1162_dlg_char_char dlg_char_char = null;
   Co1162_dlg_double_double dlg_double_double = null;
   Co1162_dlg_float_float dlg_float_float = null;
   Co1162_dlg_long_long dlg_long_long = null;
   Co1162_dlg_short_short dlg_short_short = null;
   Co1162invoke cb1162a = new Co1162invoke();
   Co1162_dlg_OM  dlg_OM  =  null;
   try  
     {
     Console.WriteLine( "Point a delegate to the base's overridden method." );
     strLoc="Loc_1935ovr";
     dlg_OM = new Co1162_dlg_OM( this.OverriddenMethod );  
     i4a = -3;
     i4a = dlg_OM( 2 );
     ++iCountTestcases;
     if ( i4a != 200 )
       {
       ++iCountErrors;
       Console.Error.WriteLine( "Error_1557hun (Co1162invoke), i4a=="+ i4a.ToString() );
       }
     strLoc = "L_101";
     dlg_int_int_int = new Co1162_dlg_int_int_int( cb1162a.returnModuloFirstSecond );
     i4a = -2;
     i4a = dlg_int_int_int( 8 ,3 );
     ++iCountTestcases;
     if ( i4a != ( 8 % 3 ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_09pv (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_09pv ,Co1162invoke)  i4a==" );
       sblMsg.Append( i4a );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_101b";
     dlg_int_int_int = new Co1162_dlg_int_int_int( Co1162invoke.static_returnModuloFirstSecond );
     i4a = -2;
     i4a = dlg_int_int_int( 8 ,3 );
     ++iCountTestcases;
     if ( i4a != ( 8 % 3 ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_093pv (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_093pv ,Co1162invoke)  i4a==" );
       sblMsg.Append( i4a );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_102";
     ++iCountTestcases;
     dlg_void_ = new Co1162_dlg_void_( cb1162a.return_void_ );
     dlg_void_();
     strLoc = "L_102b";
     ++iCountTestcases;
     dlg_void_ = new Co1162_dlg_void_( Co1162invoke.static_return_void_ );
     dlg_void_();
     strLoc = "L_103";
     dlg_String_String = new Co1162_dlg_String_String( cb1162a.return_String_String );
     str1 = " initial value bad";
     str1 = dlg_String_String( "parm_Value_good" );
     ++iCountTestcases;
     if ( ! str1.Equals( "parm_Value_good" ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_35si (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_35si ,Co1162invoke)  b9==" );
       sblMsg.Append( b9.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_103b";
     dlg_String_String = new Co1162_dlg_String_String( Co1162invoke.static_return_String_String );
     str1 = " initial value bad";
     str1 = dlg_String_String( "parm_Value_good" );
     ++iCountTestcases;
     if ( ! str1.Equals( "parm_Value_good" ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_353si (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_353si ,Co1162invoke)  b9==" );
       sblMsg.Append( b9.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_104";
     dlg_Boolean_Boolean = new Co1162_dlg_Boolean_Boolean( cb1162a.return_Boolean_Boolean );
     b2 = false;
     b2 = dlg_Boolean_Boolean( true );
     ++iCountTestcases;
     if ( ! b2 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_29zj (Co1162invoke)"  );
       }
     strLoc = "L_104b";
     dlg_Boolean_Boolean = new Co1162_dlg_Boolean_Boolean( Co1162invoke.static_return_Boolean_Boolean );
     b2 = false;
     b2 = dlg_Boolean_Boolean( true );
     ++iCountTestcases;
     if ( ! b2 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_29wzj (Co1162invoke)"  );
       }
     strLoc = "L_104.5";
     dlg_Byte_Byte = new Co1162_dlg_Byte_Byte( cb1162a.return_Byte_Byte );
     strLoc = "L_105.1";
     ub9 = 0;
     strLoc = "L_105.2";
     ub9 = dlg_Byte_Byte( (Byte)3 );
     ++iCountTestcases;
     if ( ub9 != 3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_80ys (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_80ys ,Co1162invoke)  ub9==" );
       sblMsg.Append( ub9.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_104.5b";
     dlg_Byte_Byte = new Co1162_dlg_Byte_Byte( Co1162invoke.static_return_Byte_Byte );
     strLoc = "L_105.1b";
     ub9 = 0;
     strLoc = "L_105.2b";
     ub9 = dlg_Byte_Byte( (Byte)3);
     ++iCountTestcases;
     if ( ub9 != 3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_803ys (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_803ys ,Co1162invoke)  ub9==" );
       sblMsg.Append( ub9.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_105";
     dlg_SByte_SByte = new Co1162_dlg_SByte_SByte( cb1162a.return_SByte_SByte );
     strLoc = "L_105.1";
     b9 = -2;
     strLoc = "L_105.2";
     b9 = dlg_SByte_SByte( (SByte)3 );
     ++iCountTestcases;
     if ( b9 != 3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_80ys (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_80ys ,Co1162invoke)  b9==" );
       sblMsg.Append( b9.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_105b";
     dlg_SByte_SByte = new Co1162_dlg_SByte_SByte( Co1162invoke.static_return_SByte_SByte );
     strLoc = "L_105.1b";
     b9 = -2;
     strLoc = "L_105.2b";
     b9 = dlg_SByte_SByte( (SByte)3 );
     ++iCountTestcases;
     if ( b9 != 3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_80y1s (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_80y1s ,Co1162invoke)  b9==" );
       sblMsg.Append( b9.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_106";
     dlg_char_char = new Co1162_dlg_char_char( cb1162a.return_char_char );
     c2 = 'A';
     c2 = dlg_char_char( 'b' );
     ++iCountTestcases;
     if ( c2 != 'b' )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_66fg (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_66fg ,Co1162invoke)  c2==" );
       sblMsg.Append( c2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_106b";
     dlg_char_char = new Co1162_dlg_char_char( Co1162invoke.static_return_char_char );
     c2 = 'A';
     c2 = dlg_char_char( 'b' );
     ++iCountTestcases;
     if ( c2 != 'b' )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_666fg (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_666fg ,Co1162invoke)  c2==" );
       sblMsg.Append( c2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_107";
     dlg_double_double = new Co1162_dlg_double_double( cb1162a.return_double_double );
     d2 = -2.2;
     d2 = dlg_double_double( 3.3 );
     ++iCountTestcases;
     if ( d2 != 3.3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_47uq (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_47uq ,Co1162invoke)  d2==" );
       sblMsg.Append( d2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_107b";
     dlg_double_double = new Co1162_dlg_double_double( Co1162invoke.static_return_double_double );
     d2 = -2.2;
     d2 = dlg_double_double( 3.3 );
     ++iCountTestcases;
     if ( d2 != 3.3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_471uq (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_471uq ,Co1162invoke)  d2==" );
       sblMsg.Append( d2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_108";
     dlg_float_float = new Co1162_dlg_float_float( cb1162a.return_float_float );
     f2 = (float)-2.2;
     f2 = dlg_float_float( (float)3.3 );
     ++iCountTestcases;
     if ( f2 != (float)3.3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_38ki (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_38ki ,Co1162invoke)  f2==" );
       sblMsg.Append( f2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_108b";
     dlg_float_float = new Co1162_dlg_float_float( Co1162invoke.static_return_float_float );
     f2 = (float)-2.2;
     f2 = dlg_float_float( (float)3.3 );
     ++iCountTestcases;
     if ( f2 != (float)3.3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_138ki (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_138ki ,Co1162invoke)  f2==" );
       sblMsg.Append( f2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_109";
     dlg_long_long = new Co1162_dlg_long_long( cb1162a.return_long_long );
     l2 = (long)-2;
     l2 = dlg_long_long(  (long)3 );
     ++iCountTestcases;
     if ( l2 != (long)3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_20pf (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_20pf ,Co1162invoke)  l2==" );
       sblMsg.Append(l2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_109b";
     dlg_long_long = new Co1162_dlg_long_long( Co1162invoke.static_return_long_long );
     l2 = (long)-2;
     l2 = dlg_long_long(  (long)3 );
     ++iCountTestcases;
     if ( l2 != (long)3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_220pf (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_220pf ,Co1162invoke)  l2==" );
       sblMsg.Append(l2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_110";
     dlg_short_short = new Co1162_dlg_short_short( cb1162a.return_short_short );
     s2 = (short)-2;
     s2 = dlg_short_short( (short)3 );
     ++iCountTestcases;
     if ( s2 != (short)3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_19ic (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_19ic ,Co1162invoke)  s2==" );
       sblMsg.Append( s2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strLoc = "L_110b";
     dlg_short_short = new Co1162_dlg_short_short( Co1162invoke.static_return_short_short );
     s2 = (short)-2;
     s2 = dlg_short_short( (short)3 );
     ++iCountTestcases;
     if ( s2 != (short)3 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_119ic (Co1162invoke)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_119ic ,Co1162invoke)  s2==" );
       sblMsg.Append( s2.ToString() );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: find error E_41ba (Co1162invoke) - strLoc == " + strLoc  );
     sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_41ba ,Co1162invoke)  exc_general.ToString()==" );
     sblMsg.Append( exc_general.ToString() );
     Console.Error.WriteLine(  sblMsg.ToString()  );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 new public Int32 OverriddenMethod( Int32 parm1 )  
   {
   return ( parm1 * 100 );
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co1162invoke cbA = new Co1162invoke();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co1162invoke"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
public class Co1162_A1
{
 public Int32 OverriddenMethod( Int32 parm1 )
   {
   return ( parm1 * 10 );
   }
}
