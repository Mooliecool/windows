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
using System.Collections;
public class Co6004IsReadOnly
{
   public static String s_strActiveBugNums = "";
   public static String s_strDtTmVer       = "";
   public static String s_strClassMethod   = "Array.IsReadOnly";
   public static String s_strTFName        = "Co6004IsReadOnly";
   public static String s_strTFAbbrev      = "Co6004";
   public static String s_strTFPath        = "";
   public bool runTest()
   {
      Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
      int iCountErrors = 0;
      int iCountTestcases = 0;
      String strLoc = "Loc_000oo";
	  Boolean bReadOnly = false;
      try {
      do
      {
         strLoc = "Loc_01ifs";
         iCountTestcases++;
         short[] in2Arr = new Int16[10];
		 bReadOnly = in2Arr.IsReadOnly;
         if(bReadOnly)
         {
            iCountErrors++;
            Console.WriteLine( s_strTFAbbrev+ " , Err_01ifs! , Array.IsReadOnly failed on single dimension array." );
         }
 		 strLoc = "Loc_02ifs";
         iCountTestcases++;
         int[] in4Arr = new Int32[0];
		 bReadOnly = in4Arr.IsReadOnly;
         if(bReadOnly)
         {
            iCountErrors++;
            Console.WriteLine( s_strTFAbbrev+ " , Err_02ifs! , Array.IsReadOnly failed on 0 size single dimension array." );
         }
		 int[][] i4Arr = new int[3][];
		 i4Arr[0] = new int[] {1, 2, 3};
		 i4Arr[1] = new int[] {1, 2, 3, 4, 5, 6};
		 i4Arr[2] = new int[] {1, 2, 3, 4, 5, 6, 7, 8, 9};
 		 strLoc = "Loc_03ifx";
         iCountTestcases++;
		 bReadOnly = i4Arr.IsReadOnly;
         if(bReadOnly)
         {
            iCountErrors++;
            Console.WriteLine( s_strTFAbbrev+ " , Err_03ifs! , Array.IsReadOnly failed on jagged array." );
         }
 		 strLoc = "Loc_04ifx";
         iCountTestcases++;
         Object [][] objArr = new Object[0][];
		 bReadOnly = objArr.IsReadOnly;
         if(bReadOnly)
         {
            iCountErrors++;
            Console.WriteLine( s_strTFAbbrev+ ", Err_04ifs! , Array.IsReadOnly failed on 0 size jagged array." );
         }
 		 strLoc = "Loc_05ifx";
         iCountTestcases++;
         Double[,] doArr = new Double[99, 78];
		 bReadOnly = doArr.IsReadOnly;
         if(bReadOnly)
         {
            iCountErrors++;
            Console.WriteLine( s_strTFAbbrev+ " , Err_05ifs! , Array.IsReadOnly failed on rectangle array." );
         }
 		 strLoc = "Loc_06ifx";
         iCountTestcases++;
         Hashtable[,] htArr = new Hashtable[0,0];
		 bReadOnly = htArr.IsReadOnly;
         if(bReadOnly)
         {
            iCountErrors++;
            Console.WriteLine( s_strTFAbbrev+ " , Err_06ifs! , Array.IsReadOnly failed on 0 size rectangle array." );
         }
 		 strLoc = "Loc_07ifx";
         iCountTestcases++;
         string[,,,,,] strArr = new string[6,5,4,3,2,1];
		 bReadOnly = strArr.IsReadOnly;
         if(bReadOnly)
         {
            iCountErrors++;
            Console.WriteLine( s_strTFAbbrev+ " , Err_07ifs! , Array.IsReadOnly failed on 6 dimensions rectangle array." );
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
         Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,inCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
         return false;
      }
   }
   public static void Main(String[] args)
   {
      bool bResult = false;
      Co6004IsReadOnly cbA = new Co6004IsReadOnly();
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
      if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
