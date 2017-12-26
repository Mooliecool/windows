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
using System.Threading;
using System;
public class Co3582AddDays
{
 internal static String strName = "DateTime.AddDays";
 internal static String strTest = "Co3582AddDays.cs";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   DateTime dt1;
   DateTime dt2;
   DateTime dtExp;
   TimeSpan ts1;
   double dbl1;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       dt1 = new DateTime(1998, 12, 16);
       dbl1 = 1.0;
       dt2 = dt1.AddDays(dbl1);
       dtExp = new DateTime(1998, 12, 17);
       ++iCountTestcases;
       if ( DateTime.Equals(dt2, dtExp) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  dt1==" + dt1  );
	 }
       dt1 = new DateTime(1998, 12, 16);
       dbl1 = 1.9D;
       dt2 = dt1.AddDays(dbl1);
       dtExp = new DateTime(1998, 12, 17, 21, 36, 0);
       ++iCountTestcases;
       if ( DateTime.Equals(dt2, dtExp) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_365wr!  dt2==" + dt2  );
	 }
       dt1 = new DateTime(1998, 12, 16);
       dbl1 = 441.24523342D;
       dt2 = dt1.AddDays(dbl1);
       ts1 = new TimeSpan(TimeSpan.TicksPerDay*441 + TimeSpan.TicksPerHour*5 + TimeSpan.TicksPerMinute*53 + TimeSpan.TicksPerSecond*8 + TimeSpan.TicksPerMillisecond*167);
       dtExp = dt1.Add(ts1);
       ++iCountTestcases;
       if ( DateTime.Equals(dt2, dtExp) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_063wr!  dt2==" + dt2 );
	 }
       dt1 = new DateTime(1998, 12, 17);
       dbl1 = -1.0D;
       dt2 = dt1.AddDays(dbl1);
       ++iCountTestcases;
       if (DateTime.Equals(dt2, new DateTime(1998, 12, 16)) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_636wy!  dt2==" + dt2 + ", dtExp == " + dtExp );
	 }
       try
	 {
	 dt1 = new DateTime(9999, 12, 31);
	 dbl1 = 1.0D;
	 ++iCountTestcases;
	 dt2 = dt1.AddDays(dbl1);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_367km! Exception not thrown" );
	 }
       catch(ArgumentException ex){}
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_380wy! Wrong Exception thrown == " + ex.ToString() );
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.Error.WriteLine ( "POINTTOBREAK: Error Err_103! strLoc=="+ strLoc + " ,exc_general=="+exc_general  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strPath + strTest + "  iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL! " + strPath + strTest + "   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3582AddDays oCbTest = new Co3582AddDays();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
