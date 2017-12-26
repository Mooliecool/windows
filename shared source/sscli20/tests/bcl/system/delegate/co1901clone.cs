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
using System.Threading;
delegate int Del();
delegate int 		dlg_int_int_int		( int p_i4top ,int p_i4bottom );
delegate void 		dlg_void			();
delegate String 	dlg_String_String	( String p_str1 );
delegate Boolean	dlg_Boolean_Boolean	( Boolean p_1 );  
delegate SByte    	dlg_SByte_SByte  	( SByte   p_1 );
delegate Byte   	dlg_Byte_Byte    	( Byte   p_1 );
delegate char    	dlg_char_char      	( char   p_1 );
delegate double  	dlg_double_double  	( double p_1 );
delegate float   	dlg_float_float    	( float  p_1 );
delegate long    	dlg_long_long      	( long   p_1 );
delegate short   	dlg_short_short    	( short  p_1 );
delegate Int32   	dlg_OM             	( Int32 parm1 ); 
delegate String 	dlg_string_string 	( String p_1 );
class Co1901Clone
{
 public String s_strActiveBugNums          = "";
 public String s_strDtTmVer                = "";
 public String s_strComponentBeingTested   = "Delegate.???()";
 public String s_strTFName                 = "Co1901Clonector()";
 public String s_strTFAbbrev               = "Co1901Clone";
 public String s_strTFPath                 = "";
 public void return_void		() {inv_num++;}
 public String return_String_String	( String p_str1 ) { inv_num++; return p_str1; }
 public Boolean return_Boolean_Boolean	( Boolean p_1 )	{ inv_num++; return p_1; }
 public SByte return_SByte_SByte	( SByte p_1 ) 	{ inv_num++; return p_1; }
 public Byte return_Byte_Byte		( Byte p_1 ) 	{ inv_num++; return p_1; }
 public char return_char_char		( char p_1 ) 	{ inv_num++; return p_1; }
 public double return_double_double	( double p_1 )	{ inv_num++; return p_1; }
 public float return_float_float	( float p_1 )	{ inv_num++; return p_1; }
 public long return_long_long		( long p_1 ) 	{ inv_num++; return p_1; }
 private short return_short_short	( short p_1 )	{ inv_num++; return p_1; }
 private int   return_int_add_int_int	( int p_1, int p_2 )	{ inv_num++; return p_1+p_2; }
 public static void static_return_void	() {inv_num++;}
 public static Boolean static_return_Boolean_Boolean	( Boolean p_1 )		{ inv_num++; return p_1; }
 public static SByte static_return_SByte_SByte		( SByte p_1 )		{ inv_num++; return p_1; }
 public static Byte static_return_Byte_Byte		( Byte p_1 )		{ inv_num++; return p_1; }
 public static char static_return_char_char		( char p_1 )		{ inv_num++; return p_1; }
 public static double static_return_double_double	( double p_1 )		{ inv_num++; return p_1; }
 public static float static_return_float_float		( float p_1 )		{ inv_num++; return p_1; }
 public static long static_return_long_long		( long p_1 )		{ inv_num++; return p_1; }
 public static String static_return_String_String	( String p_str1 )	{ inv_num++; return p_str1; }
 int CheckInvkList_int_int_int 		(dlg_int_int_int p) 	{inv_num=0; p(1, 1); return inv_num;}
 int CheckInvkList_void 			(dlg_void p)		{inv_num=0; p(); return inv_num;}
 int CheckInvkList_Boolean_Boolean 	(dlg_Boolean_Boolean p)	{inv_num=0; p(false); return inv_num;}
 int CheckInvkList_SByte_SByte 		(dlg_SByte_SByte p)  	{inv_num=0; p(-3); return inv_num;}
 int CheckInvkList_Byte_Byte 		(dlg_Byte_Byte p)    	{inv_num=0; p(2); return inv_num;}
 int CheckInvkList_char_char 		(dlg_char_char p)      	{inv_num=0; p('D'); return inv_num;}
 int CheckInvkList_double_double 	(dlg_double_double p)  	{inv_num=0; p(3.32); return inv_num;}
 static int inv_num = 0;
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors    = 0;
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_int_int_int inst_dlg_int_int_int = new dlg_int_int_int (instCo1901Clone.return_int_add_int_int);
   inst_dlg_int_int_int = inst_dlg_int_int_int;				
   int numberInvoked = CheckInvkList_int_int_int(inst_dlg_int_int_int);
   dlg_int_int_int delClone_int_int_int = (dlg_int_int_int) inst_dlg_int_int_int.Clone();
   if (numberInvoked!=(CheckInvkList_int_int_int(inst_dlg_int_int_int)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x001: inst_dlg_int_int_int returned a wrong result");
     }
   int yehInt = delClone_int_int_int (2, -4);
   if (yehInt!= -2)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_001: invokation list changed");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_001,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_void inst_dlg_void = new dlg_void (instCo1901Clone.return_void);
   dlg_void static_dlg_void = new dlg_void (Co1901Clone.static_return_void);
   inst_dlg_void = inst_dlg_void;
   inst_dlg_void = (dlg_void) Delegate.Combine (inst_dlg_void, static_dlg_void);
   int numberInvoked = CheckInvkList_void (inst_dlg_void);
   dlg_void dlgClone_void = (dlg_void) inst_dlg_void.Clone();		
   if (numberInvoked!=(CheckInvkList_void (inst_dlg_void)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x002: invokation list changed");
     }
   dlgClone_void ();
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_002,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_Boolean_Boolean inst_dlg_Boolean_Boolean = new dlg_Boolean_Boolean (instCo1901Clone.return_Boolean_Boolean);
   dlg_Boolean_Boolean static_dlg_Boolean_Boolean = new dlg_Boolean_Boolean (Co1901Clone.static_return_Boolean_Boolean);
   inst_dlg_Boolean_Boolean = (dlg_Boolean_Boolean) Delegate.Combine (static_dlg_Boolean_Boolean, inst_dlg_Boolean_Boolean);
   int numberInvoked = CheckInvkList_Boolean_Boolean (inst_dlg_Boolean_Boolean);
   dlg_Boolean_Boolean dlgClone_Boolean_Boolean = (dlg_Boolean_Boolean) inst_dlg_Boolean_Boolean.Clone();		
   if (numberInvoked != (CheckInvkList_Boolean_Boolean (inst_dlg_Boolean_Boolean)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x003: invokation list changed");
     }
   Boolean yehBool	= dlgClone_Boolean_Boolean (true);
   if (yehBool!= true)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_003: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_003,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_SByte_SByte inst_dlg_SByte_SByte = new dlg_SByte_SByte (instCo1901Clone.return_SByte_SByte);
   dlg_SByte_SByte static_dlg_SByte_SByte = new dlg_SByte_SByte (Co1901Clone.static_return_SByte_SByte);
   static_dlg_SByte_SByte = (dlg_SByte_SByte) Delegate.Combine (static_dlg_SByte_SByte, inst_dlg_SByte_SByte);
   static_dlg_SByte_SByte = static_dlg_SByte_SByte;
   int numberInvoked = CheckInvkList_SByte_SByte (static_dlg_SByte_SByte);
   dlg_SByte_SByte dlgClone_SByte_SByte = (dlg_SByte_SByte) static_dlg_SByte_SByte.Clone();		
   if (numberInvoked != (CheckInvkList_SByte_SByte (static_dlg_SByte_SByte)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x004: invokation list changed");
     }
   SByte yehSByte = dlgClone_SByte_SByte (-1);
   if (yehSByte!= -1)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_004: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_004,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_Byte_Byte inst_dlg_Byte_Byte = new dlg_Byte_Byte (instCo1901Clone.return_Byte_Byte);
   dlg_Byte_Byte static_dlg_Byte_Byte = new dlg_Byte_Byte (Co1901Clone.static_return_Byte_Byte);
   static_dlg_Byte_Byte = static_dlg_Byte_Byte;
   static_dlg_Byte_Byte = (dlg_Byte_Byte) Delegate.Combine (inst_dlg_Byte_Byte, static_dlg_Byte_Byte);
   int numberInvoked = CheckInvkList_Byte_Byte (static_dlg_Byte_Byte);
   dlg_Byte_Byte dlgClone_Byte_Byte = (dlg_Byte_Byte) static_dlg_Byte_Byte.Clone();		
   if (numberInvoked != (CheckInvkList_Byte_Byte (static_dlg_Byte_Byte)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x005: invokation list changed");
     }
   Byte yehByte = dlgClone_Byte_Byte (0);
   if (yehByte!= 0)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_005: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_005,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_char_char inst_dlg_char_char = new dlg_char_char (instCo1901Clone.return_char_char);
   dlg_char_char static_dlg_char_char = new dlg_char_char (Co1901Clone.static_return_char_char);
   inst_dlg_char_char = (dlg_char_char) Delegate.Combine (static_dlg_char_char, inst_dlg_char_char);
   static_dlg_char_char = (dlg_char_char) Delegate.Combine (inst_dlg_char_char, static_dlg_char_char);
   inst_dlg_char_char = (dlg_char_char) Delegate.Combine (static_dlg_char_char, inst_dlg_char_char);
   static_dlg_char_char = (dlg_char_char) Delegate.Combine (inst_dlg_char_char, static_dlg_char_char);
   inst_dlg_char_char = (dlg_char_char) Delegate.Combine (inst_dlg_char_char, static_dlg_char_char);
   int numberInvoked = CheckInvkList_char_char (inst_dlg_char_char);
   dlg_char_char dlgClone_char_char = (dlg_char_char) inst_dlg_char_char.Clone();		
   if (numberInvoked != (CheckInvkList_char_char (inst_dlg_char_char)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x006: invokation list changed");
     }
   numberInvoked = CheckInvkList_char_char (inst_dlg_char_char);
   dlgClone_char_char = (dlg_char_char) dlgClone_char_char.Clone();		
   if (numberInvoked != (CheckInvkList_char_char (dlgClone_char_char)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x006a: invokation list changed");
     }
   char yehChar = dlgClone_char_char ('a');
   if (yehChar!= 'a')
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_006: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_006,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_double_double inst_dlg_double_double = new dlg_double_double (instCo1901Clone.return_double_double);
   int numberInvoked = CheckInvkList_double_double (inst_dlg_double_double);
   dlg_double_double dlgClone_double_double = (dlg_double_double) inst_dlg_double_double.Clone();		
   if (numberInvoked != (CheckInvkList_double_double (inst_dlg_double_double)))
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_x007: invokation list changed");
     }
   double yehDouble = dlgClone_double_double (332);
   if (yehDouble!= 332)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_007: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_007,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   dlg_double_double static_dlg_double_double = new dlg_double_double (Co1901Clone.static_return_double_double);
   dlg_double_double dlgClone_double_double = (dlg_double_double) static_dlg_double_double.Clone();		
   double yehDouble = dlgClone_double_double (332);
   if (yehDouble!= 332)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_007a: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_007a,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_float_float inst_dlg_float_float = new dlg_float_float (instCo1901Clone.return_float_float);
   dlg_float_float dlgClone_float_float = (dlg_float_float) inst_dlg_float_float.Clone();		
   float yehFloat = (float)-2.2;
   yehFloat = dlgClone_float_float (yehFloat);
   if (yehFloat!= (float)-2.2)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_008: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_008,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   dlg_float_float static_dlg_float_float = new dlg_float_float (Co1901Clone.static_return_float_float);
   dlg_float_float dlgClone_float_float = (dlg_float_float) static_dlg_float_float.Clone();		
   float yehFloat = (float)-2.2;
   yehFloat = dlgClone_float_float (yehFloat);
   if (yehFloat!= (float)-2.2)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_008a: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_008a,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_long_long inst_dlg_long_long = new dlg_long_long (instCo1901Clone.return_long_long);
   dlg_long_long dlgClone_long_long = (dlg_long_long) inst_dlg_long_long.Clone();		
   long yehLong = (long)-2;  
   yehLong = dlgClone_long_long (yehLong);
   if (yehLong!= (long)-2)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_009: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_009,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   dlg_long_long static_dlg_long_long = new dlg_long_long (Co1901Clone.static_return_long_long);
   dlg_long_long dlgClone_long_long = (dlg_long_long) static_dlg_long_long.Clone();		
   long yehLong = (long)-2;  
   yehLong = dlgClone_long_long (yehLong);
   if (yehLong!= (long)-2)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_009a: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_009a,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_short_short inst_dlg_short_short = new dlg_short_short (instCo1901Clone.return_short_short);
   dlg_short_short dlgClone_short_short = (dlg_short_short) inst_dlg_short_short.Clone();		
   short yehShort = (short)-2;
   yehShort = dlgClone_short_short (yehShort);
   if (yehShort!= (short)-2)
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_010: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_010,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   Co1901Clone instCo1901Clone = new Co1901Clone();
   dlg_string_string inst_dlg_string_string = new dlg_string_string (instCo1901Clone.return_String_String);
   dlg_string_string dlgClone_string_string = (dlg_string_string) inst_dlg_string_string.Clone();		
   String yehString = "anything\\,\\+weird";
   yehString = dlgClone_string_string (yehString);
   if (yehString!= "anything\\,\\+weird")
     {
     iCountErrors++;
     Console.WriteLine ("Yeh_Err_001: inst_dlg_int_int_int returned a wrong result");
     }
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Yeh_Ex_011,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   iCountTestcases++;
   try {
   someclass sc  = new someclass();
   Del del1 = new Del(sc.counter);
   Del del2 = (Del) del1.Clone();
   if (del1() != 1)
     throw new Exception("wrong return from del1()");
   if (del2() != 2)
     throw new Exception("wrong return from del1()");
   }
   catch (Exception ex) {
   ++iCountErrors;
   Console.WriteLine("Err_001a,  Unexpected exception was thrown ex: " + ex.ToString());
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String [] args )
   {
   Co1901Clone runClass = new Co1901Clone();
   Boolean bResult = runClass.runTest();
   if (! bResult) {
   Console.WriteLine( runClass.s_strTFPath  + runClass.s_strTFName );
   Console.Error.WriteLine( " " );
   Console.Error.WriteLine( "FAiL!  " + runClass.s_strTFAbbrev );
   Console.Error.WriteLine( " " );
   Console.Error.WriteLine( "ACTIVE BUGS: "  + runClass.s_strActiveBugNums );
   }
   if ( bResult == true )
     Environment.ExitCode = 0;
   else
     Environment.ExitCode = 11;
   }
}
class someclass {
 internal int i; public someclass() { i = 0; } public int counter() { return ++i; }
}
