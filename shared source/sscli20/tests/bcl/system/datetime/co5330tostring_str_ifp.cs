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
using System.Text;
using System.Globalization;
public class Co5330Format
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.UnBox()";
 public static String s_strTFName        = "Co5330Format.cs";
 public static String s_strTFAbbrev      = "Cb5305";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public static DateTime dttmNow          = DateTime.Now;
 public static Random rand               = new Random( ( ( dttmNow.Second * 1000 ) + dttmNow.Millisecond ) );
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   try {
   do
     {
     DateTime dt1;
     String str1, strRec, strExp;
     strLoc = "Loc_29tyh";
     dt1 = new DateTime();
     iCountTestcases++;
     try
       {
       dt1.ToString(null , null);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_298th! Unexpected exception thrown : "+exc.ToString());
       }
     strLoc = "Loc_840fn";
     str1 = "dd%";
     iCountTestcases++;
     try
       {
       str1 = dt1.ToString(str1, null);
       iCountErrors++;
       printerr("Error_982hg! Expected FormatException , got value =="+str1);
       }
     catch (FormatException fExc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_747hg! Expected FormatException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_98htg";
     str1 = "dd\\";
     iCountTestcases++;
     try
       {
       str1 = dt1.ToString(str1, null);
       iCountErrors++;
       printerr("Error_98thg! Expected FormatException, got value=="+str1);
       }
     catch (FormatException fExc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_98y4t! Expected FormatException, got exc=="+exc.ToString());
       }
     strLoc = "Loc_928hf";
     dt1 = new DateTime(1999, 10, 8, 15, 24, 27);
     str1 =   "dd MM yy yy yyyy mm hh HH ss ss d ddd dddd";
     strExp = "08 10 99 99 1999 24 03 15 27 27 8 ";
     if(CultureInfo.CurrentCulture.DisplayName.Equals("English")) {
     strExp += "Fri Friday";
     }
     else {
     strExp += dt1.ToString("ddd", CultureInfo.CurrentCulture)+" ";
     strExp += dt1.ToString("dddd", CultureInfo.CurrentCulture);
     }
     strRec = dt1.ToString(str1, null);
     iCountTestcases++;
     if(!strRec.Equals(strExp))
       {
       iCountErrors++;
       printerr("Error_982yg! Expected=="+strExp+" , got=="+strRec);
       }
     strLoc = "Loc_9028f";
     dt1 = new DateTime(1999, 10, 8, 15, 24, 27);
     str1 =   "dd MMM MMMM yyy yy yyyy mmmmmmmmmm hhhhhhh HHHHHH ss ss d ddd ddddddddddd";
     if(CultureInfo.CurrentCulture.DisplayName.Equals("English")) {
     strExp = "08 Oct October 1999 99 1999 24 03 15 27 27 8 Fri Friday";
     }
     else {
     strExp = "08 ";
     strExp += dt1.ToString("MMM", CultureInfo.CurrentCulture) + " ";
     strExp += dt1.ToString("MMMM", CultureInfo.CurrentCulture) + " ";
     strExp += "1999 99 1999 24 03 15 27 27 8 ";
     strExp += dt1.ToString("ddd", CultureInfo.CurrentCulture) + " ";
     strExp += dt1.ToString("ddddddddddd", CultureInfo.CurrentCulture);
     }
     strRec = dt1.ToString(str1, null);
     iCountTestcases++;
     if(!strRec.Equals(strExp))
       {
       iCountErrors++;
       printerr("Error_92thg! Expected=="+strExp+" , got=="+strRec);
       }
     strLoc = "Loc_2908f";
     dt1 = new DateTime(1588, 4, 9, 11, 20, 30);
     str1 = "h:m:s tt";
     strExp = "11:20:30 AM";
     strRec = dt1.ToString(str1, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(!strRec.Equals(strExp))
       {
       iCountErrors++;
       printerr("Error_982ht! Expected=="+strExp+" , got=="+strRec);
       }
     strLoc = "Loc_2jfw1";
     dt1 = new DateTime(1588, 4, 9, 12, 0, 1);
     str1 = "h:m:s tt";
     strExp = "12:0:1 PM";
     strRec = dt1.ToString(str1, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(!strRec.Equals(strExp))
       {
       iCountErrors++;
       printerr("Error_848ht! Expected=="+strExp+" , got=="+strRec);
       }
     strLoc = "Loc_28thg";
     dt1 = new DateTime();
     str1 = "\"hi there\" % \'A\' \\ / / /";
     strExp = "hi there  A  / / /";
     strRec = dt1.ToString(str1, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(!strRec.Equals(strExp))
       {
       iCountErrors++;
       printerr("Error_984yt! Expected=="+strExp+" , got=="+strRec);
       }
     strLoc = "Loc_2908h";
     dt1 = new DateTime();
     str1 = "\"";
     iCountTestcases++;
     try
       {
       str1 = dt1.ToString(str1, null);
       iCountErrors++;
       printerr("Error_84yts! FormatException expected, got value=="+str1);
       }
     catch (FormatException fExc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_948th! FormatException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_98thy";
     dt1 = new DateTime();
     str1 = "\'";
     iCountTestcases++;
     try
       {
       str1 = dt1.ToString(str1, null);
       iCountErrors++;
       printerr("Error_9084g! FormatException expected, got value=="+str1);
       }
     catch (FormatException fExc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_09hrt! FormatException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_0ut3w";
     dt1 = new DateTime();
     str1 = "d";
     iCountTestcases++;
     Console.WriteLine( dt1.ToString(str1, null) );
     Console.WriteLine( dt1.ToShortDateString() );
     if(!dt1.ToString(str1, null).Equals(dt1.ToShortDateString()))
       {
       iCountErrors++;
       printerr("Error_209ut! d format string has incorrect output=="+dt1.ToString(str1, DateTimeFormatInfo.InvariantInfo));
       }
     str1 = "D";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals(dt1.ToLongDateString()))
       {
       iCountErrors++;
       printerr("Error_02ut4! D format string is incorrect");
       }
     str1 = "f";
     iCountTestcases++;
     strRec = dt1.ToString(str1, null);
     if(!strRec.Equals(dt1.ToLongDateString()+" "+dt1.ToShortTimeString()))
       {
       iCountErrors++;
       printerr("Error_209uf! f format string is incorrect, str=="+strRec);
       }
     str1 = "F";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals(dt1.ToLongDateString()+" "+dt1.ToLongTimeString()))
       {
       iCountErrors++;
       printerr("Error_98thg! F format string is incorrect");
       }
     str1 = "g";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals(dt1.ToShortDateString()+" "+dt1.ToShortTimeString()))
       {
       iCountErrors++;
       printerr("Error_948tt! g format string is incorrect");
       }
     str1 = "G";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals(dt1.ToShortDateString()+" "+dt1.ToLongTimeString()))
       {
       iCountErrors++;
       printerr("Error_2908t! G format string is incorrect");
       }
     str1 = "m";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("January 01"))
       {
       iCountErrors++;
       printerr("Error_9284f! m format string is incorrect");
       }
     str1 = "M";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("January 01"))
       {
       iCountErrors++;
       printerr("Error_2948g! M format string is incorrect");
       }
     str1 = "r";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("Mon, 01 Jan 0001 00:00:00 GMT"))
       {
       iCountErrors++;
       printerr("Error_9t8hg! r Format string is incorrect , dt=="+dt1.ToString(str1, null));
       }
     str1 = "R";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("Mon, 01 Jan 0001 00:00:00 GMT"))
       {
       iCountErrors++;
       printerr("Error_0924u! R format string is incorrect , dt=="+dt1.ToString(str1, null));
       }
     dt1 = new DateTime(1888, 10, 11, 12,13 , 14);
     str1 = "s";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("1888-10-11T12:13:14"))
       {
       iCountErrors++;
       printerr("Error_8427y! s format string is incorrect , dt=="+dt1.ToString(str1, null));
       }
     str1 = "t";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("12:13 PM"))
       {
       iCountErrors++;
       printerr("Error_284ht! t string incorrect, got string="+dt1.ToString(str1, null));
       }
     str1 = "T";
     iCountTestcases++;
     strRec = dt1.ToString(str1, null);
     if(!strRec.Equals("12:13:14 PM"))
       {
       iCountErrors++;
       printerr("Error_489th! T string incorrect, got string=="+strRec);
       }
     str1 = "u";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("1888-10-11 12:13:14Z"))
       {
       iCountErrors++;
       printerr("Error_849th! u string incorrect, got string=="+dt1.ToString(str1, null));
       }
     str1 = "y";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("October, 1888"))
       {
       iCountErrors++;
       printerr("Error_3984g! y string incorrect, got string=="+dt1.ToString(str1, null));
       }
     str1 = "Y";
     iCountTestcases++;
     if(!dt1.ToString(str1, null).Equals("October, 1888"))
       {
       iCountErrors++;
       printerr("Error_984yt! Y string incorrect, got string=="+dt1.ToString(str1, null));
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
   Console.WriteLine(exc_general.StackTrace);
   }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +"\\"+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+"\\"+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 private bool CompareDates(DateTime dt1 , DateTime dt2 , bool CompareType)
   {
   if ( dt1.Day != dt2.Day || dt1.Month != dt2.Month || dt1.Year != dt2.Year )
     return false ;
   if ( CompareType )
     {
     if ( dt1.Hour != dt2.Hour || dt1.Minute != dt2.Minute || dt1.Second != dt2.Second )
       return false ;  
     }
   return true ;                                      
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5330Format cbA = new Co5330Format();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
