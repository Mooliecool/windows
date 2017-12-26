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
public class Co1622Cons_Eq_GV_GHC : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Int16);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("GetHashCode", new Type[]{}));
   list.Add(type.GetMethod("Equals", new Type[]{typeof(Object)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1622Cons_Eq_GV_GHC runTest started." );
   Console.Out.WriteLine ("");
   Console.Out.WriteLine ("Method_Covered: Integer2.Equals (Microsoft/Runtime/Object)");
   Console.Out.WriteLine ("Method_Covered: Integer2.GetValue ()");
   Console.Out.WriteLine ("Method_Covered: Integer2.GetHashCode ()");
   Console.Out.WriteLine ("Method_Count==3 (Co1622Cons_Eq_GV_GHC)");
   Console.Out.WriteLine ("");
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str0 = null;
   String str2 = null;
   StringBuilder sbl5 = new StringBuilder( 99 );
   int in4r = -2;
   int in4s = -2;
   short sh2z = (short)-2;
   short sh2y = (short)-2;
   Int32 int4e = (Int32)0;
   Int16 sho2a = (Int16)0;
   Int16 sho2b = (Int16)0;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       sh2z = (short)3;   strLoc="Loc_110th";
       sbl5.Length = 0 ;
       str0 = sbl5.Append( sh2z ).ToString();
       sho2a = (Int16)( sh2z );  
       sho2b = (Int16)( sh2z );
       in4r = sho2a.GetHashCode();  
       ++iCountTestcases;
       if ( sh2z ==0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184pr!  sh2z=="+ sh2z +" ,in4r=="+ in4r  );
	 }
       sh2y = sho2a;
       ++iCountTestcases;
       if ( sh2z != sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_125ci!  sh2y==" + sh2y  );
	 }
       str2 = sho2a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! sho2a.Equals( sho2b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_199nd!  sh2z==" + sh2z  );
	 }
       sh2z = (short)-4;   strLoc="Loc_120tr";
       sbl5.Length = 0;
       str0 = sbl5.Append( sh2z ).ToString();
       sho2a = (Int16)( sh2z );  
       sho2b = (Int16)( sh2z );
       in4r = sho2a.GetHashCode();  
       ++iCountTestcases;
       if ( sh2z != -4 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184zl!  -sh2z==" + -sh2z  );
	 }
       sh2y = sho2a;
       ++iCountTestcases;
       if ( sh2z != sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_125wa!  sh2y==" + sh2y  );
	 }
       str2 = sho2a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144dy!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! sho2a.Equals( sho2b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_199uc!  sh2z==" + sh2z  );
	 }
       sh2z = Int16.MinValue;   strLoc="Loc_220nv";
       sbl5.Length = 0 ;
       str0 = sbl5.Append( sh2z ).ToString();
       sho2a = (Int16)( sh2z );  
       sho2b = (Int16)( sh2z );
       in4r = sho2a.GetHashCode();  
       ++iCountTestcases;
       if ( sh2z != -32768 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_284pr!  ( sh2z  )==" + ( sh2z  )  );
	 }
       sh2y = sho2a;
       ++iCountTestcases;
       if ( sh2z != sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_225ci!  sh2y==" + sh2y  );
	 }
       str2 = sho2a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_244ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! sho2a.Equals( sho2b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_299nd!  sh2z==" + sh2z  );
	 }
       sh2z = Int16.MaxValue;   strLoc="Loc_330xv";
       sbl5.Length = 0 ;
       str0 = sbl5.Append( sh2z ).ToString();
       sho2a = (Int16)( sh2z );  
       sho2b = (Int16)( sh2z );
       in4r = sho2a.GetHashCode();  
       ++iCountTestcases;
       if ( sh2z == 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_384pr!  sh2y==" + sh2y  );
	 }
       sh2y = sho2a;
       ++iCountTestcases;
       if ( sh2z != sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_325ci!  sh2y==" + sh2y  );
	 }
       str2 = sho2a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_344ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! sho2a.Equals( sho2b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_399nd!  sh2z==" + sh2z  );
	 }
       sh2z = (short)-0;   strLoc="Loc_440nz";
       sbl5.Length =  -0;
       str0 = sbl5.Append( sh2z ).ToString();
       sho2a = (Int16)( sh2z );  
       sho2b = (Int16)( sh2z );
       in4r = sho2a.GetHashCode();  
       ++iCountTestcases;
       if ( sh2z != in4r )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_484pr!  sh2y==" + sh2y  );
	 }
       sh2y = sho2a;
       ++iCountTestcases;
       if ( sh2z != sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_425ci!  sh2y==" + sh2y  );
	 }
       str2 = sho2a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_444ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! sho2a.Equals( sho2b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_499nd!  sh2z==" + sh2z  );
	 }
       ++iCountTestcases;
       if ( ! str2.Equals( "0" ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_461vj!  str2==" + str2  );
	 }
       int4e = (Int32)( -0 );
       ++iCountTestcases;
       if ( sho2a.Equals( int4e ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_462mj!  int4e.ToString()==" + int4e.ToString()  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1622Cons_Eq_GV_GHC) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Integer2\\Co1622Cons_Eq_GV_GHC.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Integer2\\Co1622Cons_Eq_GV_GHC.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1622Cons_Eq_GV_GHC cbA = new Co1622Cons_Eq_GV_GHC();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1622Cons_Eq_GV_GHC) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1622Cons_Eq_GV_GHC.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
