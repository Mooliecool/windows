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
using System.IO;
using System.Reflection;
using System.Text;
using System.Collections;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co1632Cons_Eq_GV_GHC: IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Int64);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("GetHashCode", new Type[]{}));
   list.Add(type.GetMethod("Equals", new Type[]{typeof(Object)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1632Cons_Eq_GV_GHC runTest started." );
   Console.Out.WriteLine ("");
   Console.Out.WriteLine ("Method_Covered: Integer8.Equals(Object obj)");
   Console.Out.WriteLine ("Method_Covered: Integer8.GetValue ()");
   Console.Out.WriteLine ("Method_Covered: Integer8.GetHashCode ()");
   Console.Out.WriteLine ("Method_Count==3 (Co1632Cons_Eq_GV_GHC)");
   Console.Out.WriteLine ("");
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str0 = null;
   String str2 = null;
   StringBuilder sbl5 = new StringBuilder( 99 );
   int in_4e = -2;
   long lo8z = -2L;
   long lo8y = -2L;
   Int32 int4c = 0;
   Int64 int8a = 0L;
   Int64 int8b = 0L;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       lo8z = 3L;   strLoc="Loc_110th";
       sbl5.Length = 0;
       str0 = sbl5.Append( lo8z ).ToString();
       int8a = (Int64)( lo8z );  
       int8b = (Int64)( lo8z );
       in_4e = int8a.GetHashCode();  
       ++iCountTestcases;
       if ( (int)lo8z != in_4e )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184pr!  lo8y==" + lo8y  );
	 }
       lo8y = int8a;
       ++iCountTestcases;
       if ( lo8z != lo8y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_125ci!  lo8y==" + lo8y  );
	 }
       str2 = int8a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int8a.Equals( int8b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_199nd!  lo8z==" + lo8z  );
	 }
       lo8z = -4L;   strLoc="Loc_120tr";
       sbl5.Length = 0;
       str0 = sbl5.Append( lo8z ).ToString();
       int8a = (Int64)( lo8z );  
       int8b = (Int64)( lo8z );
       in_4e = int8a.GetHashCode();  
       ++iCountTestcases;
       if ( (int)lo8z+1 != -in_4e )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_184zl!  -lo8y==" + -lo8y  );
	 }
       lo8y = int8a;
       ++iCountTestcases;
       if ( lo8z != lo8y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_125wa!  lo8y==" + lo8y  );
	 }
       str2 = int8a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_144dy!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int8a.Equals( int8b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_199uc!  lo8z==" + lo8z  );
	 }
       lo8z = Int64.MinValue;   strLoc="Loc_220nv";
       sbl5.Length = 0;
       str0 = sbl5.Append( lo8z ).ToString();
       int8a = (Int64)( lo8z );  
       int8b = (Int64)( lo8z );
       in_4e = int8a.GetHashCode();  
       ++iCountTestcases;
       if ( in_4e != Int32.MinValue )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_284pr!  ( -lo8z + 1 )==" + ( -(int)lo8z  )  );
	 }
       lo8y = int8a;
       ++iCountTestcases;
       if ( lo8z != lo8y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_225ci!  lo8y==" + lo8y  );
	 }
       str2 = int8a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_244ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int8a.Equals( int8b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_299nd!  lo8z==" + lo8z  );
	 }
       lo8z = Int64.MaxValue;   strLoc="Loc_330xv";
       sbl5.Length = 0;
       str0 = sbl5.Append( lo8z ).ToString();
       int8a = (Int64)( lo8z );  
       int8b = (Int64)( lo8z );
       in_4e = int8a.GetHashCode();  
       if ( in_4e != Int32.MinValue)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_384pr!  lo8z==" + (int)lo8z  );
	 }
       lo8y = int8a;
       ++iCountTestcases;
       if ( lo8z != lo8y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_325ci!  lo8y==" + lo8y  );
	 }
       str2 = int8a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_344ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int8a.Equals( int8b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_399nd!  lo8z==" + lo8z  );
	 }
       lo8z = -0L;   strLoc="Loc_440nz";
       sbl5.Length =  -0;
       str0 = sbl5.Append( lo8z ).ToString();
       int8a = (Int64)( lo8z );  
       int8b = (Int64)( lo8z );
       in_4e = int8a.GetHashCode();  
       ++iCountTestcases;
       if ( (int)lo8z != in_4e )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_484pr!  lo8y==" + lo8y  );
	 }
       lo8y = int8a;
       ++iCountTestcases;
       if ( lo8z != lo8y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_425ci!  lo8y==" + lo8y  );
	 }
       str2 = int8a.ToString();
       ++iCountTestcases;
       if ( ! str2.Equals( str0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_444ye!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( ! int8a.Equals( int8b ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_499nd!  lo8z==" + lo8z  );
	 }
       ++iCountTestcases;
       if ( ! str2.Equals( "0" ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_461vj!  str2==" + str2  );
	 }
       int4c = (Int32)( -0 );
       ++iCountTestcases;
       if ( !int8a.Equals( int4c ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_462mj!  int4c.ToString()==" + int4c.ToString()  );
	 }
       int max = 0;
       Hashtable hDup = new Hashtable ();
       strLoc = "Loc_920dz";
       for (Int64 testI = -100; testI < 100; testI++) 
	 {
	 iCountTestcases++;
	 if (testI.GetHashCode() < 0) 
	   {
	   iCountErrors++;
	   Console.WriteLine( "ERROR Err_9284pp: The Hash code for " + testI + " is negative ");
	   }
	 try
	   {
	   hDup.Add (testI.GetHashCode(), 0);
	   }
	 catch(ArgumentException argexc)
	   {
	   hDup[testI.GetHashCode()] = (int) hDup[testI.GetHashCode()] + 1;
	   if ((int) hDup[testI.GetHashCode()] > max)
	     {
	     max = (int)hDup[testI.GetHashCode()];
	     }
	   }
	 }
       Console.WriteLine ("INFO: In_893 Maximum Duplicate hash codes that exist: " + max);
       if (max > 5)
	 {
	 iCountErrors++;
	 Console.WriteLine( "ERROR Err_9284zz: TOO MANY DUPLICATE HASH CODES");
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1632Cons_Eq_GV_GHC) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Integer8\\Co1632Cons_Eq_GV_GHC.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Integer8\\Co1632Cons_Eq_GV_GHC.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1632Cons_Eq_GV_GHC cbA = new Co1632Cons_Eq_GV_GHC();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1632Cons_Eq_GV_GHC) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1632Cons_Eq_GV_GHC.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
