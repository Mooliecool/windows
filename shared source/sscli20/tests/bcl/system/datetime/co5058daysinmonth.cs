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
public class Co5058DaysInMonth
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.DaysInMonth(int, int)";
 public static readonly String s_strTFName        = "Co5058DaysInMonth.cs";
 public static readonly String s_strTFAbbrev      = "Co5058";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc;
   DateTime dt1 ;
   DateTime dt2 ;
   String strInput1;
   String strInput2;
   Random random = new Random();
   Int32 randValue;
   int inYear = 0;
   int inMonth = 0;
   int [] inDays = {31,
		    28,
		    31,
		    30,
		    31,
		    30,
		    31,
		    31,
		    30,
		    31,
		    30,
		    31
   };
   int [] inDaysLY = {31,
		      29,
		      31,
		      30,
		      31,
		      30,
		      31,
		      31,
		      30,
		      31,
		      30,
		      31
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strLoc = "Loc_419lk";
     iCountTestcases++;
     try 
       {
       DateTime.DaysInMonth( 2000, 0);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_200mk! , days=="+DateTime.DaysInMonth (2000, 0));
       } 
     catch (ArgumentException aExc) {}
     catch (Exception exc) 
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_270qj! , exc=="+exc);
       }
     for(int i=0; i<20; i++)
       {
       try 
	 {
	 randValue = random.Next(Int32.MinValue, 0);
	 DateTime.DaysInMonth( 2000, randValue);
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_200mk! , days=="+DateTime.DaysInMonth (2000, randValue));
	 } 
       catch (ArgumentException aExc) {}
       catch (Exception exc) 
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_270qj! , exc=="+exc);
	 }
       }
     strLoc = "Loc_420lk";
     iCountTestcases++;
     for(int i=0; i<20; i++)
       {
       try 
	 {
	 DateTime.DaysInMonth( 2000, random.Next(13, Int32.MaxValue));
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_213mk! , days=="+DateTime.DaysInMonth (2000, 13));
	 } 
       catch (ArgumentException aExc) {}
       catch (Exception exc) 
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_280qj! , exc=="+exc);
	 }
       }
     strLoc = "Loc_430mk";
     iCountTestcases++;
     for(int i=0; i<20; i++)
       {
		 try 
		 {
			 DateTime.DaysInMonth( random.Next(10000, Int32.MaxValue), 2);
			 iCountErrors++;
			 Console.WriteLine( s_strTFAbbrev+ "Err_295mk! , expected exception not thrown==");
		 } 
		 catch (ArgumentOutOfRangeException e){}
		 catch (Exception exc) 
		 {
			 iCountErrors++;
			 Console.WriteLine( s_strTFAbbrev+ "Err_295qj! , exc=="+exc);
		 }
       }
     strLoc = "Loc_458mk";
     iCountTestcases++;
     for(int i=0; i<20; i++)
       {
		 try 
		 {
			 DateTime.DaysInMonth( random.Next(Int32.MinValue, 0), 2);
			 iCountErrors++;
			 Console.WriteLine( s_strTFAbbrev+ "Err_029mk! , expected exception not thrown==");
		 } 
		 catch (ArgumentOutOfRangeException e) {}
		 catch (Exception exc) 
		 {
			 iCountErrors++;
			 Console.WriteLine( s_strTFAbbrev+ "Err_029po! , exc=="+exc);
		 }
       }
     strBaseLoc = "Loc_1000hw_";
     inYear = 1999;
     for (inMonth = 1 ; inMonth <= 12; inMonth++)
       {
       strLoc = strBaseLoc+inMonth.ToString();
       iCountTestcases++;
       if(DateTime.DaysInMonth(inYear, inMonth) != inDays[inMonth-1])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_138nq! days=="+DateTime.DaysInMonth(inYear, inMonth));
	 }
       }
     strBaseLoc = "Loc_1100hv_";
     inYear = 2000;
     for (inMonth = 1 ; inMonth <= 12; inMonth++)
       {
       strLoc = strBaseLoc+inMonth.ToString();
       iCountTestcases++;
       if(DateTime.DaysInMonth(inYear, inMonth) != inDaysLY[inMonth-1])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_238ks! days=="+DateTime.DaysInMonth(inYear, inMonth));
	 }
       }
     strBaseLoc = "Loc_1150hv_";
     inYear = 1896;
     for (inMonth = 1 ; inMonth <= 12; inMonth++)
       {
       strLoc = strBaseLoc+inMonth.ToString();
       iCountTestcases++;
       if(DateTime.DaysInMonth(inYear, inMonth) != inDaysLY[inMonth-1])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_298kw! days=="+DateTime.DaysInMonth(inYear, inMonth));
	 }
       }
     strBaseLoc = "Loc_1200hv_";
     inYear = 200;
     for (inMonth = 1 ; inMonth <= 12; inMonth++)
       {
       strLoc = strBaseLoc+inMonth.ToString();
       iCountTestcases++;
       if(DateTime.DaysInMonth(inYear, inMonth) != inDays[inMonth-1])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_338ks! days=="+DateTime.DaysInMonth(inYear, inMonth));
	 }
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   Boolean bResult = false;
   Co5058DaysInMonth cbA = new Co5058DaysInMonth();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine(s_strTFName+ s_strTFPath);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
