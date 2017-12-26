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
using System.Globalization;
using System.Reflection;
using System.Collections;
using System.Text;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co1565ToByte_allSigns: IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Boolean)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Byte)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToByte", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Convert.ToByte( allSigns ! )";
 public static readonly String s_strTFName        = "Co1565ToByte_allSigns.cs";
 public static readonly String s_strTFAbbrev      = "Co1565";
 public static readonly String s_strTFPath        = "";
 public void printoutCoveredMethods()
   {
   MemberInfo[] mmm = GetTestedMethods();
   Console.Error.WriteLine( "" );
   Console.Error.WriteLine( "Method_Count==  ("+ mmm.Length +"==confirm) !!" );
   Console.Error.WriteLine( "" );
   foreach(MemberInfo mm in mmm)
     Console.Error.WriteLine( mm );
   Console.Error.WriteLine( "" );
   }
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   String strBaseLoc;
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   byte ubyResult = 204;
   Object[] varArrOverflowExcInputs =
   {
     (SByte)(-5)
     ,(Char)(256)
     ,(Int32)(-5)
     ,(Int32)(256)
     ,(Int64)(-5)
     ,(Int64)(256)
     ,(UInt16)(256)
     ,(UInt32)(256)
     ,(UInt64)(256)
     ,(Int16)( -3 )
     ,(Int16)( 1234 )
     ,(Decimal)( 1234 )
     ,(Decimal)( -1234 )
     ,(Single)( (float)-3.0 )
     ,(Single)( (float)1234.5 )
     ,( (double)-3.0 )
     ,( (double)1234.5 )
     ,1234
   };
   String[] varArrFormatExcInputs =
   {
     (String)( "-3" )
     ,(String)( "1234" )
     ,(String)( "NotANuymber" )
     ,(String)( String.Empty )
   };
   Object[] varArrInputs =
   {
     (bool)( true ),
     (bool)( false ),
     (sbyte)( -0 )
     ,(sbyte)( 2 )
     ,(sbyte)( 127 )
     ,(Int16)( -0 )
     ,(Int16)( 2 )
     ,(Int16)( 255 )
     ,(Int32)( -0 )
     ,(Int32)( 2 )
     ,(Int32)( 255 )
     ,(Int64)( -0 )
     ,(Int64)( 2 )
     ,(Int64)( 255 )
     ,(Single)( (float)0.0 )
     ,(Single)(        2.0f )
     ,(Single)( 255.0f )
     ,(Double)( -0.0 )
     ,(Double)( 2.0 )
     ,(Double)( 2.4 )
     ,(Double)( 2.5 )
     ,(Double)( 2.6 )   
     ,(Double)( 255.0 )
     ,(Object)( -0 )
     ,(Object)( 2 )
     ,(Object)( 255 )
     ,(String)( "-0" )
     ,(String)( "2" )
     ,(String)( "255" )
     ,(UInt16)( 0 )
     ,(UInt16)( 50 )
     ,(UInt16)( 100 )
     ,(UInt16)( 255 )
     ,(UInt32)( 0 )
     ,(UInt32)( 50 )
     ,(UInt32)( 100 )
     ,(UInt32)( 255 )
     ,(UInt64)( 0 )
     ,(UInt64)( 50 )
     ,(UInt64)( 100 )
     ,(UInt64)( 255 )
     ,(Decimal)( 0 )
     ,(Decimal)( 50 )
     ,(Decimal)( 100 )
     ,(Decimal)( 255 )
     ,(byte)0
     ,(byte)50
     ,(byte)100
     ,(byte)255
   };
   Byte[] ubyArrExpecteds =
   {
     (byte)1,
     (byte)0,
     (byte)0
     ,(byte)2
     ,(byte)127
     ,(byte)0
     ,(byte)2
     ,(byte)255
     ,(byte)-0
     ,(byte)2
     ,(byte)255
     ,(byte)-0
     ,(byte)2
     ,(byte)255
     ,(byte)-0
     ,(byte)2
     ,(byte)255
     ,(byte)-0
     ,(byte)2
     ,(byte)2
     ,(byte)2
     ,(byte)3   
     ,(byte)255
     ,(byte)-0
     ,(byte)2
     ,(byte)255
     ,(byte)-0
     ,(byte)2
     ,(byte)255
     ,(byte)0
     ,(byte)50
     ,(byte)100
     ,(byte)255
     ,(byte)0
     ,(byte)50
     ,(byte)100
     ,(byte)255
     ,(byte)0
     ,(byte)50
     ,(byte)100
     ,(byte)255
     ,(byte)0
     ,(byte)50
     ,(byte)100
     ,(byte)255
     ,(byte)0
     ,(byte)50
     ,(byte)100
     ,(byte)255
   };
   printoutCoveredMethods();   
   try
     {
     do
       {
       strBaseLoc="Loc_1100fw_";
       for ( int ia=0 ;ia<varArrInputs.Length ;ia++ )
	 {
	 strLoc = strBaseLoc + ia.ToString();
	 ++iCountTestcases;
	 ubyResult = Convert.ToByte( varArrInputs[ia] );
	 if ( ubyResult != ubyArrExpecteds[ia] )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1473gd!  ia=="+ ia +" ,ubyResult=="+ ubyResult );
	   }
	 ++iCountTestcases;
	 ubyResult = Convert.ToByte( varArrInputs[ia], new NumberFormatInfo() );
	 if ( ubyResult != ubyArrExpecteds[ia] )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1473gd!  ia=="+ ia +" ,ubyResult=="+ ubyResult );
	   }
	 }
       strLoc="Loc_1320po";
       strBaseLoc="Loc_1200ur_";
       for ( int ia=0 ;ia<varArrOverflowExcInputs.Length ;ia++ )
	 {
	 strLoc = strBaseLoc + ia.ToString();
	 ++iCountTestcases;
	 try
	   {
	   ubyResult = Convert.ToByte( varArrOverflowExcInputs[ia] );
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1640ke!  ia=="+ ia +" ,ubyResult=="+ ubyResult );
	   }
	 catch ( OverflowException)
	   {}
	 catch ( Exception excep )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1275mn!  ia=="+ ia +",ubyResult=="+ ubyResult +" ,excep=="+ excep );
	   }
	 }
       strBaseLoc="Loc_1300jd_";
       for ( int ia=0 ;ia<varArrFormatExcInputs.Length ;ia++ )
	 {
	 strLoc = strBaseLoc + ia.ToString();
	 ++iCountTestcases;
	 try
	   {
	   ubyResult = Convert.ToByte( varArrFormatExcInputs[ia] );
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1557fn!  ia=="+ ia +" ,ubyResult=="+ ubyResult );
	   }
	 catch ( OverflowException)
	   {}
	 catch ( FormatException)
	   {}
	 catch ( Exception excep )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1493cs!  ia=="+ ia +",ubyResult=="+ ubyResult +" ,excep=="+ excep );
	   }
	 ++iCountTestcases;
	 try
	   {
	   ubyResult = Convert.ToByte( varArrFormatExcInputs[ia], new NumberFormatInfo() );
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1557fn!  ia=="+ ia +" ,ubyResult=="+ ubyResult );
	   }
	 catch ( OverflowException)
	   {}
	 catch ( FormatException)
	   {}
	 catch ( Exception excep )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_1493cs!  ia=="+ ia +",ubyResult=="+ ubyResult +" ,excep=="+ excep );
	   }
	 }
       ++iCountTestcases;
       try
	 {
	 ubyResult = Convert.ToByte( DateTime.Now );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_834tsd!   ,ubyResult=="+ ubyResult );
	 }
       catch ( InvalidCastException)
	 {}
       catch ( Exception excep )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_1493cs!  ,excep=="+ excep );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "" );
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +""+ s_strActiveBugNums );
     return false;
     }
   } 
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1565ToByte_allSigns cbA = new Co1565ToByte_allSigns();
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
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
} 
