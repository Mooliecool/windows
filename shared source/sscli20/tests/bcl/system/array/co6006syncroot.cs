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
using System.Threading;
public class Co6006SyncRoot
{
   public static String s_strActiveBugNums = "";
   public static String s_strDtTmVer       = "";
   public static String s_strClassMethod   = "Array.SyncRoot";
   public static String s_strTFName        = "Co6006SyncRoot";
   public static String s_strTFAbbrev      = "Co6006";
   public static String s_strTFPath        = "";
   public int [] i4Arr = new int[10];
   public int [,] ii4Arr = new int[5,3];
   public Random rand;
   public bool runTest()
   {
      Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
      int iCountErrors = 0;
      int iCountTestcases = 0;
      String strLoc = "Loc_000oo";
	  Thread [] thds = new Thread[10];
	  rand = new Random();
	  try{
			for(int i=0; i< 10; i++)
			{
				thds[i] = new Thread( new ThreadStart( MyThreadStart ) );
			}
			for(int i=0; i< 10; i++)
			{
				thds[i].Start();
			}
			for(int i=0; i< 10; i++)
			{
				thds[i].Join();
			}
			iCountTestcases++;
			for(int i=0; i< i4Arr.Length-1; i++)
			{
				if( i4Arr[i] != i4Arr[i++] )
				{
					++iCountErrors;
					Console.WriteLine( s_strTFAbbrev+ " ,Err_01sr! , Array.SyncRoot failed on single dimension array." );
				}
			}
			iCountTestcases++;
			int k,j;
			for( j=0; j< 5 ; j++ )
			{
				for( k=0; k< 3 - 1; k++ )
				{
					if( ii4Arr[j,k] != ii4Arr[j,k+1] )
					{
						++iCountErrors;
						Console.WriteLine( s_strTFAbbrev+ " ,Err_02sr! , Array.SyncRoot failed on multi dimension array." );
					}
				}
				if( j < 4 )
				{
					if( ii4Arr[j,k] != ii4Arr[j+1,0] )
					{
						++iCountErrors;
						Console.WriteLine( s_strTFAbbrev+ " ,Err_02sr! , Array.SyncRoot failed on multi dimension array." );
					}
				}
			}
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
   public void MyThreadStart()
   {
		int iValue;
		lock( i4Arr.SyncRoot )
		{
			iValue = rand.Next(9999); 
			for( int i=0; i< i4Arr.Length; i++ )
			{
				i4Arr[i] = iValue;
			}
		}
		lock( ii4Arr.SyncRoot )
		{
			iValue = rand.Next(9999); 
			for( int j=0; j< 5 ; j++ )
			{
				for( int k=0; k< 3; k++ )
				{
					ii4Arr[j,k] = iValue;
				}
			}
		}
   }
   public static void Main(String[] args)
   {
      bool bResult = false;
      Co6006SyncRoot cbA = new Co6006SyncRoot();
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
