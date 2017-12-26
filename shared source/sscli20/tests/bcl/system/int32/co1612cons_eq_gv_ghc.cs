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
using System;
using System.Collections;
using System.IO;
using System.Reflection;
using System.Text;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co1612Cons_Eq_GV_GHC: IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Int32);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("GetHashCode", new Type[]{}));
   list.Add(type.GetMethod("Equals", new Type[]{typeof(Object)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1612Cons_Eq_GV_GHC runTest started." );
   Console.Out.WriteLine ("");
   Console.Out.WriteLine ("Method_Covered: Integer4.Equals(Object obj)");
   Console.Out.WriteLine ("Method_Covered: Integer4.GetValue ()");
   Console.Out.WriteLine ("Method_Covered: Integer4.GetHashCode ()");
   Console.Out.WriteLine ("Method_Count==3 (Co1612Cons_Eq_GV_GHC)");
   Console.Out.WriteLine ("");
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str0 = null;
   String str2 = null;
   StringBuilder sbl5 = new StringBuilder( 99 );
   int in4z = -2;
   int in4y = -2;
   Int16 int2a = (Int16)0;
   Int32 int4a = (Int32)0;
   Int32 int4b = (Int32)0;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       in4z = 3;   strLoc="Loc_110th";
       sbl5.Length = 0;
       str0 = sbl5.Append( in4z ).ToString();
       int4a = (Int32)( in4z );  
       int4b = (Int32)( in4z );
       in4y = int4a.GetHashCode();  
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184pr!  in4y==" + in4y  );
	 }
       in4y = int4a;
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_125ci!  in4y==" + in4y  );
	 }
       str2 = int4a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int4a.Equals( int4b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_199nd!  in4z==" + in4z  );
	 }
       in4z = -4;   strLoc="Loc_120tr";
       sbl5.Length = 0;
       str0 = sbl5.Append( in4z ).ToString();
       int4a = (Int32)( in4z );  
       int4b = (Int32)( in4z );
       in4y = int4a.GetHashCode();  
       ++iCountTestcases;
       if ( in4z != -4 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184zl!  -in4y==" + -in4y  );
	 }
       in4y = int4a;
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_125wa!  in4y==" + in4y  );
	 }
       str2 = int4a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144dy!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int4a.Equals( int4b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_199uc!  in4z==" + in4z  );
	 }
       in4z = Int32.MinValue;   strLoc="Loc_220nv";
       sbl5.Length = 0;
       str0 = sbl5.Append( in4z ).ToString();
       int4a = (Int32)( in4z );  
       int4b = (Int32)( in4z );
       in4y = int4a.GetHashCode();  
       ++iCountTestcases;
       if ( in4z == 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_284pr!  ( -in4y + 1 )==" + ( -in4y + 1 )  );
	 }
       in4y = int4a;
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_225ci!  in4y==" + in4y  );
	 }
       str2 = int4a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_244ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int4a.Equals( int4b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_299nd!  in4z==" + in4z  );
	 }
       in4z = Int32.MaxValue;   strLoc="Loc_330xv";
       sbl5.Length = 0;
       str0 = sbl5.Append( in4z ).ToString();
       int4a = (Int32)( in4z );  
       int4b = (Int32)( in4z );
       in4y = int4a.GetHashCode();  
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_384pr!  in4y==" + in4y  );
	 }
       in4y = int4a;
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_325ci!  in4y==" + in4y  );
	 }
       str2 = int4a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_344ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int4a.Equals( int4b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_399nd!  in4z==" + in4z  );
	 }
       in4z = -0;   strLoc="Loc_440nz";
       sbl5.Length =  -0;
       str0 = sbl5.Append( in4z ).ToString();
       int4a = (Int32)( in4z );  
       int4b = (Int32)( in4z );
       in4y = int4a.GetHashCode();  
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_484pr!  in4y==" + in4y  );
	 }
       in4y = int4a;
       ++iCountTestcases;
       if ( in4z != in4y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_425ci!  in4y==" + in4y  );
	 }
       str2 = int4a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_444ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int4a.Equals( int4b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_499nd!  in4z==" + in4z  );
	 }
       ++iCountTestcases;
       if ( ! str2.Equals( "0" ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_461vj!  str2==" + str2  );
	 }
       int2a = (Int16)( (short)-0 );
       ++iCountTestcases;
       if ( !int4a.Equals( int2a ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_462mj!  int2a.ToString()==" + int2a.ToString()  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1612Cons_Eq_GV_GHC) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Integer4\\Co1612Cons_Eq_GV_GHC.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Integer4\\Co1612Cons_Eq_GV_GHC.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1612Cons_Eq_GV_GHC cbA = new Co1612Cons_Eq_GV_GHC();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1612Cons_Eq_GV_GHC) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1612Cons_Eq_GV_GHC.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
