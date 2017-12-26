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
public class Co3658CompareTo
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "SignedByte.CompareTo(Obj)";
 public static readonly String s_strTFName        = "Co3658CompareTo.cs";
 public static readonly String s_strTFAbbrev      = "Cb3656";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   SByte Sbt1;
   SByte Sbt2;
   Object o1;
   try
     {
       Sbt1 = (SByte)100;
       Sbt2 = (SByte)100;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) != 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_320ye!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = SByte.MaxValue;
       Sbt2 = SByte.MaxValue;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) != 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_732xe!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = SByte.MinValue;
       Sbt2 = SByte.MinValue;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) != 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_106zx!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = (SByte)0;
       Sbt2 = (SByte)0;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) != 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_016se!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = (SByte)1;
       Sbt2 = (SByte)(-1);
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) <= 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_024zw!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = SByte.MaxValue;
       Sbt2 = SByte.MinValue;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) <= 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_023se!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = SByte.MaxValue;
       Sbt2 = (SByte)2;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) <= 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_163pq!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = (SByte)(-1);
       Sbt2 = (SByte)(-2);
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) <= 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_134dr!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = (SByte)(-1);
       Sbt2 = (SByte)1;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) >= 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_024xe!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = (SByte)5;
       Sbt2 = (SByte)6;
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) >= 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_053ze!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       Sbt1 = (SByte)(-5);
       Sbt2 = (SByte)(-3);
       o1= Sbt2;
       ++iCountTestcases;
       if ( Sbt1.CompareTo(o1) >= 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_103wp!  Sbt1.CompareTo(Sbt2)=="+ Sbt1.CompareTo(Sbt2) );
	 }
       ++iCountTestcases;
       try
	 {
	 Sbt1 = (SByte)(-5);
	 if ( Sbt1.CompareTo(null) <= 0 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_014cb!  Sbt1.CompareTo(null)=="+ Sbt1.CompareTo(null) );
	   }
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_035ap!  Sbt1.CompareTo(null) throwing=="+ ex );
	 }
       ++iCountTestcases;
       try
	 {
	 Sbt1 = (SByte)(-5);
         o1 = (Double)4;
         Sbt1.CompareTo(o1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_076xe!  not throwing when Object is not SignedByte");
	 }
       catch (ArgumentException ex){}
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_164hk!  wrong exception thrown=="+ ex );
	 }
     }
   catch (Exception exc_general)
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
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3658CompareTo oCbTest = new Co3658CompareTo();
   try
     {
     bResult = oCbTest.runTest();
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
