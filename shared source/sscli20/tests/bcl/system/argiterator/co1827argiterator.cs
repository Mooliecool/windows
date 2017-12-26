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
using System.IO;
using System.Text;
using System.Collections;
using System.Reflection;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
class Co1827 : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(ArgIterator);
   ArrayList list = new ArrayList();
   list.Add(type.GetConstructor(new Type[]{typeof(RuntimeArgumentHandle)}));
   list.Add(type.GetMethod("GetNextArg", new Type[]{}));
   list.Add(type.GetMethod("GetRemainingCount", new Type[]{}));
   MemberInfo[] methods = new MemberInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public String s_strActiveBugNums          = "";
 public String s_strDtTmVer                = "";
 public String s_strComponentBeingTested   = "GettingAndUsingMemberTokens";
 public String s_strTFName                 = "Co1827GettingAndUsingMemberTokens";
 public String s_strTFAbbrev               = "Co1827";
 public String s_strTFPath                 = "";
 public Boolean verbose                    = false;
 class NormClass
   {
   public static void argit0( __arglist )
     {
     ArgIterator args = new ArgIterator( __arglist );
     TypedReference trTypRef = args.GetNextArg();
     }
   public static void argit1( __arglist )
     {
     ArgIterator args = new ArgIterator( __arglist );
     try
       {
       int argCount = args.GetRemainingCount();
       for (int i = 0; i < argCount; i++) 
	 {
	 TypedReference trTypRef =  args.GetNextArg();
	 }
       }
     catch(Exception ex)
       {
       throw new Exception( "ExcErr001  ," + ex.ToString() );
       }
     for ( int j = 0; j < 5; j++ )
       {
       try
	 {
	 TypedReference trTypRef = args.GetNextArg();
	 throw new Exception( "ExcErr002  , Last call should have thrown." );
	 }
       catch (InvalidOperationException)
	 {}
       }
     }
   public static void argit2( __arglist )
     {
     ArgIterator args = new ArgIterator( __arglist );
     Object o = args.GetNextArgType();
     }
   public static void argit3( __arglist )
     {
     ArgIterator args = new ArgIterator( __arglist );
     try
       {
       int argCount = args.GetRemainingCount();
       for (int i = 0; i < argCount; i++) 
	 {
	 TypedReference trTypRef = args.GetNextArg();
	 }
       }
     catch(Exception ex)
       {
       throw new Exception( "ExcErr007  ," + ex.ToString() );
       }
     for ( int j = 0; j < 5; j++ )
       {
       try
	 {
	 RuntimeTypeHandle rthRunTypHan = args.GetNextArgType();
	 throw new Exception( "ExcErr006  , Last call should have thrown." );
	 }
       catch (InvalidOperationException)
	 {}
       }
     }
   public void argit4( Object o, __arglist)
     {
     }
   private void argit5( __arglist )
     {
     }
   public Object[] argit6( Object[] objExpectedArr, __arglist )
     {
     ArgIterator args = new ArgIterator( __arglist );
     int iCount = args.GetRemainingCount();
     Object[] objArr = new Object[iCount];
     for ( int i = 0; i < iCount; i++ )
       {
       objArr[i] = TypedReference.ToObject(args.GetNextArg());
       if ( objExpectedArr[i] == null )
	 {
	 if ( objArr[i] != null )
	   {
	   throw new Exception( "argit6 - 1, __arglist[i] was null but it did not equal to objExpectedArr[i]" );
	   }
	 }
       else if ( ! objExpectedArr[i].Equals( objExpectedArr[i] ) )
	 {
	 throw new Exception( "argit6 - 2, __arglist[i] was null but it did not equal to objExpectedArr[i]" );
	 }
       }
     return objArr;
     }
   public Type[] argit7( __arglist )
     {
     Console.WriteLine( 1 );
     ArgIterator args = new ArgIterator( __arglist );
     int iCount = args.GetRemainingCount();
     Type[] typArr = new Type[iCount];
     for ( int i = 0; i < iCount; i++ )
       {
       Console.WriteLine( "       -> " + typeof( System.TypedReference ).TypeHandle.Value );
       Console.WriteLine( "       => " + args.GetNextArgType() );
       }
     return typArr;
     }
   }
 struct NormStruct
   {
   public static void argit0( __arglist )
     {
     ArgIterator args = new ArgIterator( __arglist );
     int iCount = args.GetRemainingCount();
     for ( int i = 0; i < iCount + 15; i++ )
       {
       try
	 {
	 TypedReference trTypRef =  args.GetNextArg();
	 if ( args.GetRemainingCount() != (iCount - i - 1) )
	   {
	   throw new Exception( "ExcErr5  ,Should have had remaining count " + (iCount - i - 1) + " but had remaining count " + args.GetRemainingCount() );
	   }
	 }
       catch(Exception ex)
	 {			
	 if ( i < iCount )
	   {
	   Console.WriteLine( i );
	   throw ex;
	   }
	 int iRemCount = args.GetRemainingCount();
	 if ( iRemCount != 0 )
	   {
	   throw new Exception( "ExcErr3  ,Should have had remaining count 0 but had remaining count " + iRemCount );
	   }
	 }
       }
     if ( args.GetRemainingCount() != 0 )
       {
       throw new Exception( "ExcErr4  ,Should have had remaining count 0");
       }
     }
   private void argit3( __arglist )
     {
     }
   }
 interface NormInterface
   {
   void argit0( __arglist );
   void argit1( Object o, __arglist);	
   }
 abstract class NormAbstractClass
   {
   public void argit0( __arglist )
     {
     }
   public abstract void argit1( __arglist);	
   }
 public Boolean runTest()
   {
   Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
   int iCountTestcases = 0;
   int iCountErrors    = 0;
   ++iCountTestcases;   
   if ( verbose ) Console.WriteLine( "call GetNextArgs on Enumerator with no Elements in it should throw InvalidOperationException" );	
   try
     {
     NormClass.argit0( __arglist(  ) );
     ++iCountErrors;
     Console.WriteLine( "Err_001b,  Expected exception was not thrown." );
     }
   catch (InvalidOperationException)
     {}
   catch (Exception ex)
     {
     ++iCountErrors;
     Console.WriteLine( "Err_001a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
   ++iCountTestcases;   
   if ( verbose ) Console.WriteLine( "call GetNextArgs on Enumerator after enumeration ended should result in InvalidOperationException" );	
   try
     {
     NormClass.argit1( __arglist( "a", "b", "c" ) );
     }
   catch (Exception ex)
     {
     ++iCountErrors;
     Console.WriteLine( "Err_002a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
   ++iCountTestcases;   
   if ( verbose ) Console.WriteLine( "Make sure GetRemainingCount is correctly incremented decremented for different arglists" );	
   try
     {
     NormStruct.argit0( __arglist( ) );
     NormStruct.argit0( __arglist( "a", "b", "c", "a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c"  ) );
     }
   catch (Exception ex)
     {
     ++iCountErrors;
     Console.WriteLine( "Err_003a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
   ++iCountTestcases;   
   if ( verbose ) Console.WriteLine( "call GetNextArgType on Enumerator with no Elements in it should throw InvalidOperationException" );	
   try
     {
     NormClass.argit2( __arglist(  ) );
     ++iCountErrors;
     Console.WriteLine( "Err_004b,  Expected exception was not thrown." );
     }
   catch (InvalidOperationException)
     {}
   catch (Exception ex)
     {
     ++iCountErrors;
     Console.WriteLine( "Err_004a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
   ++iCountTestcases;   
   if ( verbose ) Console.WriteLine( "call GetNextArgType on Enumerator after enumeration ended should result in InvalidOperationException" );	
   try
     {
     NormClass.argit3( __arglist( "a", "b", "c" ) );
     }
   catch (Exception ex)
     {
     ++iCountErrors;
     Console.WriteLine( "Err_005a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
   ++iCountTestcases;   
   if ( verbose ) Console.WriteLine( "call GetNextArgType on Enumerator after enumeration ended should result in InvalidOperationException" );	
   try
     {
     NormClass st = new NormClass();
     Object[] o = new Object[] { typeof(System.String), null, (int) 1975, (long) 6, (float) 4.3, BindingFlags.NonPublic, new Object(), new Hashtable(), (char) 'a', (byte) 0x80, "Some String", Guid.NewGuid(), Int16.MinValue, DateTime.Now };
     st.argit6( o, __arglist(   (Type) o[0], 
				o[1], 
				(int) o[2], 
				(long) o[3], 
				(float) o[4], 
				(BindingFlags) o[5],
				o[6], 
				(Hashtable) o[7],
				(char) o[8], 
				(byte) o[9], 
				(String) o[10],
				(Guid) o[11], 
				(short) o[12], 
				(DateTime) o[13] ) );
     }
   catch (Exception ex)
     {
     ++iCountErrors;
     Console.WriteLine( "Err_005a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
   ++iCountTestcases;   
   if ( verbose ) Console.WriteLine( "call GetNextArgType on Enumerator after enumeration ended should result in InvalidOperationException" );	
   try
     {
     NormClass st = new NormClass();
     Object[,] compObj = new Object[2,2];
     Object[] o = new Object[] {  new DBNull[]{}, new Object[]{}, compObj };
     st.argit6( o, __arglist(    (DBNull[]) o[0], 
				 (Object[]) o[1], 
				 (Object[,]) o[2] ) );
     }
   catch (Exception ex)
     {
     ++iCountErrors;
     Console.WriteLine( "Err_005a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String [] args )
   {
   Co1827 runClass = new Co1827();
   Console.WriteLine( runClass.s_strTFPath + runClass.s_strTFName );
   Console.WriteLine( " " );
   if ( args.Length > 0 )
     {
     Console.WriteLine( "Verbose ON!" );
     runClass.verbose = true;
     }
   Boolean bResult = runClass.runTest();
   if ( ! bResult )
     {
     Console.Error.WriteLine( "FAiL!  "+ runClass.s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "ACTIVE BUGS: " + runClass.s_strActiveBugNums ); 
     }
   if ( bResult == true ) Environment.ExitCode = 0;
   else Environment.ExitCode = 11; 
   }
}  
