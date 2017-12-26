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
using System.Threading;
using System;
public class Co1528ctor_iiii
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "TimeSpan(int,int,int,int)";
 public static readonly String s_strTFName        = "Co1528ctor_iiii.cs";
 public static readonly String s_strTFAbbrev      = "Co1528";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   String strBaseLoc;
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   TimeSpan ts;
   TimeSpan ts4Exc;
   int nDay, nHour, nMin, nSec, nMillisecond;
   Decimal dTicks,dDay,dHour,dMin,dSec;
   dDay = (24 * 60 * 60 * 1000);
   dHour = (60 * 60 * 1000);
   dMin = (60 * 1000);
   dSec = (1000);
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_110va";
       for (int ndays = -10; ndays <= 10; ndays = ndays + 10)
	 {
	 for (int nhours = -40; nhours <= 40; nhours = nhours + 20)
	   {
	   for (int nmin = -100; nmin <= 100; nmin = nmin + 50)
	     {
	     for (int nsec = -100; nsec <= 100; nsec = nsec + 50)
	       {
	       iCountTestcases++;
	       dTicks = nsec * dSec + nmin * dMin + nhours * dHour + ndays * dDay;
	       if (dTicks < 0)
		 {
		 if (Decimal.Floor (dTicks/dDay) == dTicks/dDay)
		   nDay = ((IConvertible)(Decimal.Floor(dTicks/dDay))).ToInt32(null);
		 else
		   nDay = ((IConvertible)(Decimal.Floor(dTicks/dDay) + 1)).ToInt32(null);
		 dTicks = dTicks - nDay * dDay;
		 if (Decimal.Floor(dTicks/dHour) == dTicks/dHour)
		   nHour = ((IConvertible)(Decimal.Floor(dTicks/dHour))).ToInt32(null);
		 else
		   nHour = ((IConvertible)(Decimal.Floor(dTicks/dHour) + 1)).ToInt32(null);
		 dTicks = dTicks - nHour * dHour;
		 if (Decimal.Floor(dTicks/dMin) == dTicks/dMin)
		   nMin = ((IConvertible)(Decimal.Floor(dTicks/dMin))).ToInt32(null);
		 else
		   nMin = ((IConvertible)(Decimal.Floor(dTicks/dMin) + 1)).ToInt32(null);
		 dTicks = dTicks - nMin * dMin;
		 if (Decimal.Floor(dTicks/dSec) == dTicks/dSec)	
		   nSec = ((IConvertible)(Decimal.Floor(dTicks/dSec))).ToInt32(null);
		 else
		   nSec = ((IConvertible)(Decimal.Floor(dTicks/dSec) + 1)).ToInt32(null);
		 dTicks = dTicks - nSec * dSec;
		 nMillisecond = ((IConvertible)(dTicks)).ToInt32(null);
		 }
	       else
		 {
		 nDay = ((IConvertible)(Decimal.Floor(dTicks/dDay))).ToInt32(null);
		 dTicks = dTicks - nDay * dDay;
		 nHour = ((IConvertible)(Decimal.Floor(dTicks/dHour))).ToInt32(null);
		 dTicks = dTicks - nHour * dHour;
		 nMin = ((IConvertible)(Decimal.Floor(dTicks/dMin))).ToInt32(null);
		 dTicks = dTicks - nMin * dMin;
		 nSec = ((IConvertible)(Decimal.Floor(dTicks/dSec))).ToInt32(null);
		 dTicks = dTicks - nSec * dSec;
		 nMillisecond = ((IConvertible)(dTicks)).ToInt32(null);
		 }
	       try
		 {
		 ts = new TimeSpan (ndays, nhours, nmin, nsec);
		 if (ts.Days != nDay)
		   {
		   ++iCountErrors;
		   Console.WriteLine( s_strTFAbbrev +"Err_542yy! Day set wrong");
		   Console.WriteLine( "\tSentIn: (" + ndays + "," + nhours + "," + nmin + "," + nsec + ")"); 
		   Console.WriteLine( "\tExpected: (" + nDay + "," + nHour + "," + nMin + "," + nSec + "," + nMillisecond +")");
		   Console.WriteLine( "\tReceived: (" + ts.Days + "," + ts.Hours + "," + ts.Minutes + "," + ts.Seconds + "," + ts.Milliseconds + ")");  
		   }
		 if (ts.Hours != nHour)
		   {
		   ++iCountErrors;
		   Console.WriteLine( s_strTFAbbrev +"Err_542zz! Hour set wrong");
		   Console.WriteLine( "\tSentIn: (" + ndays + "," + nhours + "," + nmin + "," + nsec + ")"); 
		   Console.WriteLine( "\tExpected: (" + nDay + "," + nHour + "," + nMin + "," + nSec + "," + nMillisecond +")");
		   Console.WriteLine( "\tReceived: (" + ts.Days + "," + ts.Hours + "," + ts.Minutes + "," + ts.Seconds + "," + ts.Milliseconds + ")");  
		   }
		 if (ts.Minutes != nMin)
		   {
		   ++iCountErrors;
		   Console.WriteLine( s_strTFAbbrev +"Err_542xx! Minute set wrong");
		   Console.WriteLine( "\tSentIn: (" + ndays + "," + nhours + "," + nmin + "," + nsec + ")"); 
		   Console.WriteLine( "\tExpected: (" + nDay + "," + nHour + "," + nMin + "," + nSec + "," + nMillisecond +")");
		   Console.WriteLine( "\tReceived: (" + ts.Days + "," + ts.Hours + "," + ts.Minutes + "," + ts.Seconds + "," + ts.Milliseconds + ")");  
		   }						
		 if (ts.Seconds != nSec)
		   {
		   ++iCountErrors;
		   Console.WriteLine( s_strTFAbbrev +"Err_542gg! Second set wrong");
		   Console.WriteLine( "\tSentIn: (" + ndays + "," + nhours + "," + nmin + "," + nsec + ")"); 
		   Console.WriteLine( "\tExpected: (" + nDay + "," + nHour + "," + nMin + "," + nSec + "," + nMillisecond +")");
		   Console.WriteLine( "\tReceived: (" + ts.Days + "," + ts.Hours + "," + ts.Minutes + "," + ts.Seconds + "," + ts.Milliseconds + ")");  
		   }						    					
		 if (ts.Milliseconds != nMillisecond)
		   {
		   ++iCountErrors;
		   Console.WriteLine( s_strTFAbbrev +"Err_542aa! Milliseconds set wrong");
		   Console.WriteLine( "\tSentIn: (" + ndays + "," + nhours + "," + nmin + "," + nsec + ")"); 
		   Console.WriteLine( "\tExpected: (" + nDay + "," + nHour + "," + nMin + "," + nSec + "," + nMillisecond +")");
		   Console.WriteLine( "\tReceived: (" + ts.Days + "," + ts.Hours + "," + ts.Minutes + "," + ts.Seconds + "," + ts.Milliseconds + ")");  
		   }						
		 }
	       catch (ArgumentOutOfRangeException e)
		 {
		 ++iCountErrors;
		 Console.WriteLine( s_strTFAbbrev +"Err_102rn!  NO EXCEPTION SHOULD BE THROWN caught=="+ e.ToString());
		 Console.WriteLine( "\tSentIn: (" + ndays + "," + nhours + "," + nmin + "," + nsec + ")");
		 Console.WriteLine( "\tExpected: (" + nDay + "," + nHour + "," + nMin + "," + nSec + "," + nMillisecond +")");				
		 }
	       }
	     }
	   }
	 }
       strLoc="Loc_173da";
       iCountTestcases++;
       nDay = 10675199;
       nHour = 2;
       nMin = 48;
       nSec = 5;
       nMillisecond = 0;
       ts = new TimeSpan (nDay, nHour, nMin, nSec);
       if (ts.Days != nDay)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_541yy! Day set wrong");
	 }
       if (ts.Hours != nHour)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_541zz! Hour set wrong");
	 }
       if (ts.Minutes != nMin)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_541xx! Minute set wrong");
	 }						
       if (ts.Seconds != nSec)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_541gg! Second set wrong");
	 }						    					
       if (ts.Milliseconds != nMillisecond)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_541aa! Milliseconds set wrong");
	 }						
       strLoc="Loc_173db";
       iCountTestcases++;
       nDay = -10675199;
       nHour = -2;
       nMin = -48;
       nSec = -5;
       nMillisecond = 0;
       ts = new TimeSpan (nDay, nHour, nMin, nSec);
       if (ts.Days != nDay)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_521yy! Day set wrong");
	 }
       if (ts.Hours != nHour)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_521zz! Hour set wrong");
	 }
       if (ts.Minutes != nMin)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_521xx! Minute set wrong");
	 }						
       if (ts.Seconds != nSec)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_521gg! Second set wrong");
	 }						    					
       if (ts.Milliseconds != nMillisecond)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_521aa! Milliseconds set wrong");
	 }					
       strLoc="Loc_173dc";
       iCountTestcases++;
       nDay = 10675200;
       nHour = 2;
       nMin = -1392;
       nSec = 5;
       nMillisecond = 0;
       ts = new TimeSpan (nDay, nHour, nMin, nSec);	
       nDay = 10675199;
       nHour = 2;
       nMin = 48;
       nSec = 5;
       nMillisecond = 0;
       if (ts.Days != nDay)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_641yy! Day set wrong");
	 }
       if (ts.Hours != nHour)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_641zz! Hour set wrong");
	 }
       if (ts.Minutes != nMin)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_641xx! Minute set wrong");
	 }						
       if (ts.Seconds != nSec)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_641gg! Second set wrong");
	 }						    					
       if (ts.Milliseconds != nMillisecond)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_641aa! Milliseconds set wrong");
	 }					
       strLoc="Loc_173dd";
       iCountTestcases++;
       nDay = -10675200;
       nHour = 22;
       nMin = -48;
       nSec = -5;
       nMillisecond = 0;
       ts = new TimeSpan (nDay, nHour, nMin, nSec);
       nDay = -10675199;
       nHour = -2;
       nMin = -48;
       nSec = -5;
       nMillisecond = 0;
       if (ts.Days != nDay)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_741yy! Day set wrong");
	 }
       if (ts.Hours != nHour)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_741zz! Hour set wrong");
	 }
       if (ts.Minutes != nMin)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_741xx! Minute set wrong");
	 }						
       if (ts.Seconds != nSec)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_741gg! Second set wrong");
	 }						    					
       if (ts.Milliseconds != nMillisecond)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_741aa! Milliseconds set wrong");
	 }					
       strLoc="Loc_173du";
       iCountTestcases++;
       try
	 {
	 ts4Exc = new TimeSpan( Int32.MaxValue ,Int32.MaxValue ,Int32.MaxValue ,Int32.MaxValue );
	 ++iCountErrors;   
	 Console.WriteLine( s_strTFAbbrev +"Err_894tn!  Should have throw some exception.  ts4Exc=="+ ts4Exc );
	 }
       catch ( ArgumentOutOfRangeException argexc )
	 {
	 Console.WriteLine("INFO: (Co1528) Caught ArgumentOutOfRangeException");
	 }
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_899rn!  Wrong exception type.  excep1=="+ excep1 );
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
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1528ctor_iiii cbA = new Co1528ctor_iiii();
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
