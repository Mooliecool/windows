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
using System.Collections; 
using System.Reflection; 
delegate int        Co3164_dlg_int_int        ( int p_1, int p_2 );
delegate void       Co3164_dlg_void_void();
delegate String     Co3164_dlg_String_String  ( String p_str1 );
delegate Boolean    Co3164_dlg_Boolean_Boolean( Boolean p_1 );  
delegate Byte      Co3164_dlg_Byte_Byte    ( Byte   p_1 );
delegate char       Co3164_dlg_char_char      ( char   p_1 );
delegate double     Co3164_dlg_double_double  ( double p_1 );
delegate float      Co3164_dlg_float_float    ( float  p_1 );
delegate long       Co3164_dlg_long_long      ( long   p_1 );
delegate short      Co3164_dlg_short_short    ( short  p_1 );
delegate void       Co3164_dlg_Exception      ();
public class Co3164DynamicInvoke
{
 static String strLoc;
 public int returnModuloFirstSecond( int p_i4top ,int p_i4bottom )
   { return ( p_i4top % p_i4bottom ); }
 public void return_void_void()
   { Console.Out.WriteLine( "Inside method1 return_void_() method of Delegate\\Cb1164DynamicInvoke." ); }
 public String return_String_String( String p_str1 )
   { return p_str1; }
 public Boolean return_Boolean_Boolean( Boolean p_1 )
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
 public void throw_exception()  {throw new Exception();}
 public static int returnModuloFirstSecond_static( int p_i4top ,int p_i4bottom )
   { return ( p_i4top % p_i4bottom ); }
 public static void return_void_void_static()
   { Console.Out.WriteLine( "Inside method2 return_void_() method of Delegate\\Cb1164DynamicInvoke." ); }
 public static String return_String_String_static( String p_str1 )
   { return p_str1; }
 public static Boolean return_Boolean_Boolean_static( Boolean p_1 )
   { return p_1; }
 public static Byte return_Byte_Byte_static( Byte p_1 )
   { return p_1; }
 public static char return_char_char_static( char p_1 )
   { return p_1; }
 public static double return_double_double_static( double p_1 )
   { return p_1; }
 public static float return_float_float_static( float p_1 )
   { return p_1; }
 public static long return_long_long_static( long p_1 )
   { return p_1; }
 private static short return_short_short_static( short p_1 )
   { return p_1; }
 public static void throw_exception_static() {throw new Exception();}
 public Boolean runTest()
   {
   Console.Out.WriteLine( "Delegate\\Co3164DynamicInvoke runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   Co3164DynamicInvoke cb1 = new Co3164DynamicInvoke();
   Co3164DynamicInvoke cb2 = new Co3164DynamicInvoke();
   Co3164_dlg_int_int          dlg_int = null;
   Co3164_dlg_void_void        dlg_void = null;
   Co3164_dlg_String_String    dlg_string = null;
   Co3164_dlg_Boolean_Boolean  dlg_bool = null;
   Co3164_dlg_Byte_Byte      dlg_Byte = null;
   Co3164_dlg_char_char        dlg_char = null;
   Co3164_dlg_double_double    dlg_double = null;
   Co3164_dlg_float_float      dlg_float = null;
   Co3164_dlg_long_long        dlg_long = null;
   Co3164_dlg_short_short      dlg_short = null;
   Co3164_dlg_Exception        dlg_exc = null;
   Object[] args = null; 
   Object var2 = null;
   try {
   strLoc = "Loc_387su";
   dlg_void = new Co3164_dlg_void_void(Co3164DynamicInvoke.return_void_void_static);
   iCountTestcases++;
   try {
   dlg_void.DynamicInvoke((Object[])null);
   } catch (Exception exc2)
     {
     iCountErrors++;
     print("E_573s");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   strLoc ="Loc_9438u";
   dlg_int = new Co3164_dlg_int_int(cb1.returnModuloFirstSecond);
   iCountTestcases++;
   try {
   dlg_int.DynamicInvoke((Object[])null);
   iCountErrors++;
   print("E_239a");
   } catch (TargetParameterCountException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_28kd");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   strLoc = "Loc_98usa";
   dlg_int = new Co3164_dlg_int_int(Co3164DynamicInvoke.returnModuloFirstSecond_static);
   iCountTestcases++;
   try {
   dlg_int.DynamicInvoke((Object[])null);
   iCountErrors++;
   print("E_128w");
   } catch (TargetParameterCountException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_568a");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   strLoc = "Loc_598sy";
   dlg_exc = new Co3164_dlg_Exception(cb1.throw_exception);
   iCountTestcases++;
   try {
   dlg_exc.DynamicInvoke((Object[])null);
   iCountErrors++;
   print("E_84ds");
   } catch (TargetInvocationException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_384j");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   strLoc = "Loc_798ue";
   dlg_exc = new Co3164_dlg_Exception(Co3164DynamicInvoke.throw_exception_static);
   iCountTestcases++;
   try {
   dlg_exc.DynamicInvoke((Object[])null);
   iCountErrors++;
   print("E_580j");
   } catch (TargetInvocationException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_32iw");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   strLoc = "Loc_473yd";
   args = new Object[2];
   args[0] = 8;
   args[1] = 3;
   dlg_int = new Co3164_dlg_int_int(cb1.returnModuloFirstSecond);
   try {
   var2 = dlg_int.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_398d");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Out.WriteLine(strError);
     }
   iCountTestcases++;
   if((Int32)var2 != (8%3))
     {
     iCountErrors++;
     print("E_92ja");
     }
   strLoc = "Loc_58ijg";
   args = new Object[2];
   args[0] = 8;
   args[1] = 3;
   dlg_int = new Co3164_dlg_int_int(Co3164DynamicInvoke.returnModuloFirstSecond_static);
   try {
   var2 = dlg_int.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_972e");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Out.WriteLine(strError);
     }
   iCountTestcases++;
   if((Int32)var2 != (8%3))
     {
     iCountErrors++;
     print("E_591o");
     }
   strLoc = "Loc_9809j";
   dlg_void = new Co3164_dlg_void_void(cb1.return_void_void);
   try {
   dlg_void.DynamicInvoke((Object[])null);
   } catch (Exception exc) {Console.Out.WriteLine(exc.ToString());}
   strLoc = "Loc_908uy";
   dlg_void = new Co3164_dlg_void_void(Co3164DynamicInvoke.return_void_void_static);
   try {
   dlg_void.DynamicInvoke((Object[])null);
   } catch (Exception exc) {
   iCountErrors++;
   print("E_473q");
   Console.Out.WriteLine(exc.ToString());}
   strLoc = "Loc_598ud";
   dlg_string = new Co3164_dlg_String_String(cb1.return_String_String);
   args = new Object[1];
   args[0] = "test";
   try {
   var2 = dlg_string.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_20qo");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( ! var2.Equals("test"))
     {
     iCountErrors++;
     print("E_28au");
     }
   strLoc = "Loc_9890w";
   dlg_string = new Co3164_dlg_String_String(Co3164DynamicInvoke.return_String_String_static);
   args = new Object[1];
   args[0] = "test";
   try {
   var2 = dlg_string.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_843w");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( ! var2.Equals("test"))
     {
     iCountErrors++;
     print("E_19wu");
     }
   strLoc = "Loc_4871d";
   dlg_bool = new Co3164_dlg_Boolean_Boolean(cb1.return_Boolean_Boolean);
   args = new Object[1];
   args[0] = true;
   try {
   var2 = dlg_bool.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_41si");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( ! (Boolean)var2 )
     {
     iCountErrors++;
     print("E_290o");
     }
   strLoc = "Loc_58237";
   dlg_bool = new Co3164_dlg_Boolean_Boolean(Co3164DynamicInvoke.return_Boolean_Boolean_static);
   args = new Object[1];
   args[0] = true;
   try {
   var2 = dlg_bool.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_418w");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( ! (Boolean)var2 )
     {
     iCountErrors++;
     print("E_83jw");
     }
   strLoc = "Loc_8475y";
   dlg_Byte = new Co3164_dlg_Byte_Byte(cb1.return_Byte_Byte);
   args = new Object[1];
   args[0] = (Byte)10;
   try {
   var2 = dlg_Byte.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_29eu");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (Byte)var2 != (Byte)10 )
     {
     iCountErrors++;
     print("E_283q");
     }
   strLoc = "Loc_40598";
   dlg_Byte = new Co3164_dlg_Byte_Byte(Co3164DynamicInvoke.return_Byte_Byte_static);
   args = new Object[1];
   args[0] = (Byte)10;
   try {
   var2 = dlg_Byte.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_58jw");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (Byte)var2 != (Byte)10 )
     {
     iCountErrors++;
     print("E_42wh");
     }
   strLoc = "Loc_87tyr";
   dlg_char = new Co3164_dlg_char_char(cb1.return_char_char);
   args = new Object[1];
   args[0] = '\0';
   try {
   var2 = dlg_char.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_41sk");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (Char)var2 != '\0')
     {
     iCountErrors++;
     print("E_512j");
     }
   strLoc = "Loc_045fy";
   dlg_char = new Co3164_dlg_char_char(Co3164DynamicInvoke.return_char_char_static);
   args = new Object[1];
   args[0] = '\0';
   try {
   var2 = dlg_char.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_58je");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (Char)var2 != '\0')
     {
     iCountErrors++;
     print("E_5471");
     }
   strLoc = "Loc_40962";
   dlg_double = new Co3164_dlg_double_double(cb1.return_double_double);
   args = new Object[1];
   args[0] = 5.5;
   try {
   var2 = dlg_double.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_251d");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (double)var2 != 5.5 )
     {
     iCountErrors++;
     print("E_28gf");
     }
   strLoc = "Loc_9085v";
   dlg_double = new Co3164_dlg_double_double(Co3164DynamicInvoke.return_double_double_static);
   args = new Object[1];
   args[0] = 5.5;
   try {
   var2 = dlg_double.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_52kq");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (double)var2 != 5.5 )
     {
     iCountErrors++;
     print("E_484gf");
     }
   strLoc = "Loc_0598f";
   dlg_float = new Co3164_dlg_float_float(cb1.return_float_float);
   args = new Object[1];
   args[0] = (float)7.5;
   try {
   var2 = dlg_float.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_92ka");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Out.WriteLine(strError);
     }
   iCountTestcases++;
   if( (float)var2 != (float)7.5 )
     {
     iCountErrors++;
     print("E_283k");
     }
   strLoc = "Loc_0958u";
   dlg_float = new Co3164_dlg_float_float(Co3164DynamicInvoke.return_float_float_static);
   args = new Object[1];
   args[0] = (float)7.5;
   try {
   var2 = dlg_float.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_581w");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Out.WriteLine(strError);
     }
   iCountTestcases++;
   if( (float)var2 != (float)7.5 )
     {
     iCountErrors++;
     print("E_r93w");
     }
   strLoc = "Loc_098uf";
   dlg_long = new Co3164_dlg_long_long(cb1.return_long_long);
   args = new Object[1];
   args[0] = 10000L;
   try {
   var2 = dlg_long.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_289e");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (Int64)var2 != 10000L )
     {
     iCountErrors++;
     print("E_81jd");
     }
   strLoc = "Loc_9845f";
   dlg_long = new Co3164_dlg_long_long(Co3164DynamicInvoke.return_long_long_static);
   args = new Object[1];
   args[0] = 10000L;
   try {
   var2 = dlg_long.DynamicInvoke(args);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_259j");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (Int64)var2 != 10000L )
     {
     iCountErrors++;
     print("E_399j");
     }
   strLoc = "Loc_0985y";
   dlg_short = new Co3164_dlg_short_short(cb1.return_short_short);
   args = new Object[1];
   args[0] = (short)150;
   iCountTestcases++;
   try {
   var2 = dlg_short.DynamicInvoke(args);
   }catch (MemberAccessException ) {}
   catch (Exception exc)
     {
     iCountErrors++;
     print("E_289e");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (short)var2 != (short)150 )
     {
     iCountErrors++;
     print("E_397df");
     }
   strLoc = "Loc_098ue";
   dlg_short = new Co3164_dlg_short_short(Co3164DynamicInvoke.return_short_short_static);
   args = new Object[1];
   args[0] = (short)150;
   iCountTestcases++;
   try {
   var2 = dlg_short.DynamicInvoke(args);
   }catch (MemberAccessException ) {}
   catch (Exception exc)
     {
     iCountErrors++;
     print("E_21jq");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if( (short)var2 != (short)150 )
     {
     iCountErrors++;
     print("E_759zk");
     }
   } catch ( Exception exc )
     {
     iCountErrors++;
     Console.Error.WriteLine("Uncaught Exception exc=="+exc);
     Console.Error.WriteLine("Location=="+strLoc);
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3164DynamicInvoke)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   Co3164DynamicInvoke cb0 = new Co3164DynamicInvoke();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3164DynamicInvoke"  );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
