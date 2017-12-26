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
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co7062ctor_iiiiii_Calendar
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime (Int32,Int32,Int32,Int32,Int32,Int32,Calendar)"; 
 public static String s_strTFName        = "Co7062ctor_iiiiii_Calendar.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     DateTime dTest;
     HebrewCalendar hCal = new HebrewCalendar();
     JulianCalendar jCal = new JulianCalendar();
     HijriCalendar hiCal = new HijriCalendar ();
     GregorianCalendar gCal = new GregorianCalendar ();
     JapaneseCalendar jaCal = new JapaneseCalendar ();
     KoreanCalendar kCal = new KoreanCalendar ();
     ThaiBuddhistCalendar tCal = new ThaiBuddhistCalendar ();
     strLoc = "Loc_100vy";
     iCountTestcases++;
     dTest = new DateTime(5360,04,14,15,14,13,hCal); 	
     if (dTest.Year != 1600)
       {
       ++iCountErrors;	
       printerr( "Error_100aa! DateTime object was not set correctly");
       }
     if (dTest.Month != 1)
       {
       ++iCountErrors;	
       printerr( "Error_100bb! DateTime object was not set correctly");
       }
     if (dTest.Day != 1)
       {
       ++iCountErrors;	
       printerr( "Error_100cc! DateTime object was not set correctly");
       }
     if (dTest.Hour != 15)
       {
       ++iCountErrors;	
       printerr( "Error_100dd! DateTime object was not set correctly");
       }
     if (dTest.Minute != 14)
       {
       ++iCountErrors;	
       printerr( "Error_100ee! DateTime object was not set correctly");
       }
     if (dTest.Second != 13)
       {
       ++iCountErrors;	
       printerr( "Error_100ff! DateTime object was not set correctly");
       }
     if (dTest.Millisecond != 0)
       {
       ++iCountErrors;	
       printerr( "Error_100gg! DateTime object was not set correctly");
       }
     strLoc = "Loc_101vy";
     iCountTestcases++;
     dTest = new DateTime(1599,12,22,1,2,3,jCal); 	
     if (dTest.Year != 1600)
       {
       ++iCountErrors;	
       printerr( "Error_101aa! DateTime object was not set correctly");
       }
     if (dTest.Month != 1)
       {
       ++iCountErrors;	
       printerr( "Error_101bb! DateTime object was not set correctly");
       }
     if (dTest.Day != 1)
       {
       ++iCountErrors;	
       printerr( "Error_101cc! DateTime object was not set correctly");
       }
     if (dTest.Hour != 1)
       {
       ++iCountErrors;	
       printerr( "Error_101dd! DateTime object was not set correctly");
       }
     if (dTest.Minute != 2)
       {
       ++iCountErrors;	
       printerr( "Error_101ee! DateTime object was not set correctly");
       }
     if (dTest.Second != 3)
       {
       ++iCountErrors;	
       printerr( "Error_101ff! DateTime object was not set correctly");
       }
     if (dTest.Millisecond != 0)
       {
       ++iCountErrors;	
       printerr( "Error_101gg! DateTime object was not set correctly");
       }
     strLoc = "Loc_102vy";
     iCountTestcases++;
     dTest = new DateTime(1008,06,15,3,6,9,hiCal); 	
     if (dTest.Year != 1600)
       {
       ++iCountErrors;	
       printerr( "Error_102aa! DateTime object was not set correctly");
       }
     if (dTest.Month != 1)
       {
       ++iCountErrors;	
       printerr( "Error_102bb! DateTime object was not set correctly");
       }
     if (dTest.Day != 1)
       {
       ++iCountErrors;	
       printerr( "Error_102cc! DateTime object was not set correctly");
       }
     if (dTest.Hour != 3)
       {
       ++iCountErrors;	
       printerr( "Error_102dd! DateTime object was not set correctly");
       }
     if (dTest.Minute != 6)
       {
       ++iCountErrors;	
       printerr( "Error_102ee! DateTime object was not set correctly");
       }
     if (dTest.Second != 9)
       {
       ++iCountErrors;	
       printerr( "Error_102ff! DateTime object was not set correctly");
       }
     if (dTest.Millisecond != 0)
       {
       ++iCountErrors;	
       printerr( "Error_102gg! DateTime object was not set correctly");
       }
     strLoc = "Loc_103vy";
     iCountTestcases++;
     dTest = new DateTime(1600,1,1,8,16,24,gCal); 	
     if (dTest.Year != 1600)
       {
       ++iCountErrors;	
       printerr( "Error_103aa! DateTime object was not set correctly");
       }
     if (dTest.Month != 1)
       {
       ++iCountErrors;	
       printerr( "Error_103bb! DateTime object was not set correctly");
       }
     if (dTest.Day != 1)
       {
       ++iCountErrors;	
       printerr( "Error_103cc! DateTime object was not set correctly");
       }
     if (dTest.Hour != 8)
       {
       ++iCountErrors;	
       printerr( "Error_103dd! DateTime object was not set correctly");
       }
     if (dTest.Minute != 16)
       {
       ++iCountErrors;	
       printerr( "Error_103ee! DateTime object was not set correctly");
       }
     if (dTest.Second != 24)
       {
       ++iCountErrors;	
       printerr( "Error_103ff! DateTime object was not set correctly");
       }
     if (dTest.Millisecond != 0)
       {
       ++iCountErrors;	
       printerr( "Error_103gg! DateTime object was not set correctly");
       }
     strLoc = "Loc_104vy";
     iCountTestcases++;
     dTest = new DateTime(1,1,8,23,59,59,jaCal); 	
     if (dTest.Year != 1989)
       {
       ++iCountErrors;	
       printerr( "Error_104aa! DateTime object was not set correctly");
       }
     if (dTest.Month != 1)
       {
       ++iCountErrors;	
       printerr( "Error_104bb! DateTime object was not set correctly");
       }
     if (dTest.Day != 8)
       {
       ++iCountErrors;	
       printerr( "Error_104cc! DateTime object was not set correctly");
       }
     if (dTest.Hour != 23)
       {
       ++iCountErrors;	
       printerr( "Error_104dd! DateTime object was not set correctly");
       }
     if (dTest.Minute != 59)
       {
       ++iCountErrors;	
       printerr( "Error_104ee! DateTime object was not set correctly");
       }
     if (dTest.Second != 59)
       {
       ++iCountErrors;	
       printerr( "Error_104ff! DateTime object was not set correctly");
       }
     if (dTest.Millisecond != 0)
       {
       ++iCountErrors;	
       printerr( "Error_104gg! DateTime object was not set correctly");
       }
     strLoc = "Loc_105vy";
     iCountTestcases++;
     dTest = new DateTime(3933,1,1,0,0,0,kCal); 	
     if (dTest.Year != 1600)
       {
       ++iCountErrors;	
       printerr( "Error_105aa! DateTime object was not set correctly");
       }
     if (dTest.Month != 1)
       {
       ++iCountErrors;	
       printerr( "Error_105bb! DateTime object was not set correctly");
       }
     if (dTest.Day != 1)
       {
       ++iCountErrors;	
       printerr( "Error_105cc! DateTime object was not set correctly");
       }
     if (dTest.Hour != 0)
       {
       ++iCountErrors;	
       printerr( "Error_105dd! DateTime object was not set correctly");
       }
     if (dTest.Minute != 0)
       {
       ++iCountErrors;	
       printerr( "Error_105ee! DateTime object was not set correctly");
       }
     if (dTest.Second != 0)
       {
       ++iCountErrors;	
       printerr( "Error_105ff! DateTime object was not set correctly");
       }
     if (dTest.Millisecond != 0)
       {
       ++iCountErrors;	
       printerr( "Error_105gg! DateTime object was not set correctly");
       }
     strLoc = "Loc_106vy";
     iCountTestcases++;
     dTest = new DateTime(2143,1,1,9,9,9,tCal); 	
     if (dTest.Year != 1600)
       {
       ++iCountErrors;	
       printerr( "Error_106aa! DateTime object was not set correctly");
       }
     if (dTest.Month != 1)
       {
       ++iCountErrors;	
       printerr( "Error_106bb! DateTime object was not set correctly");
       }
     if (dTest.Day != 1)
       {
       ++iCountErrors;	
       printerr( "Error_106cc! DateTime object was not set correctly");
       }
     if (dTest.Hour != 9)
       {
       ++iCountErrors;	
       printerr( "Error_106dd! DateTime object was not set correctly");
       }
     if (dTest.Minute != 9)
       {
       ++iCountErrors;	
       printerr( "Error_106ee! DateTime object was not set correctly");
       }
     if (dTest.Second != 9)
       {
       ++iCountErrors;	
       printerr( "Error_106ff! DateTime object was not set correctly");
       }
     if (dTest.Millisecond != 0)
       {
       ++iCountErrors;	
       printerr( "Error_106gg! DateTime object was not set correctly");
       }
     strLoc = "Loc_524vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(0,03,25,0,0,0,hCal);
       iCountErrors++;
       printerr( "Error_200bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_512ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_200aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_333vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(10000,03,25,0,0,0,hCal);
       iCountErrors++;
       printerr( "Error_300bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_333ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_300aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_444vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(5000,0,25,0,0,0,hCal);
       iCountErrors++;
       printerr( "Error_400bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_444ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_400aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_555vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(5000,13,25,0,0,0,jCal);
       iCountErrors++;
       printerr( "Error_500bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_555ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_500aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_665vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,0,0,0,0,jCal);
       iCountErrors++;
       printerr( "Error_600bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_665ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_600aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_777vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,32,0,0,0,jCal);
       iCountErrors++;
       printerr( "Error_700bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_775ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_700aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_888vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,14,0,0,0,null);
       iCountErrors++;
       printerr( "Error_800bb! No exception thrown");
       }
     catch (ArgumentNullException argexc)
       {
       printinfo( "Info_885ad! Caught ArguementNullException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_800aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_995vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,1,-1,0,0,jCal);
       iCountErrors++;
       printerr( "Error_900bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_965ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_900aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_19vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,12,24,0,0,jCal);
       iCountErrors++;
       printerr( "Error_190bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_195ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_190aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_525vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,1,0,-1,0,jCal);
       iCountErrors++;
       printerr( "Error_290bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_295ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_290aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_137vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,12,0,60,0,jCal);
       iCountErrors++;
       printerr( "Error_130bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_135ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_130aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_122vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,1,0,0,-1,jCal);
       iCountErrors++;
       printerr( "Error_122bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_122ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_122aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_144vy";
     iCountTestcases++;
     try
       {
       dTest = new DateTime(2000,03,12,0,0,60,jCal);
       iCountErrors++;
       printerr( "Error_144bb! No exception thrown");
       }
     catch (ArgumentOutOfRangeException argexc)
       {
       printinfo( "Info_144ad! Caught ArguementOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_144aa! Wrong exception thrown: " + e.ToString());
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co7062ctor_iiiiii_Calendar cbA = new Co7062ctor_iiiiii_Calendar();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
