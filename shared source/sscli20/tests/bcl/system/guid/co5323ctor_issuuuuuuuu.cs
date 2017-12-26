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
public class Co5323Ctor_issuuuuuuuu
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Guid(i, s, s, u, u, u, u, u, u, u, u)";
 public static String s_strTFName        = "Co5323Ctor_issuuuuuuuu.cs";
 public static String s_strTFAbbrev      = "Co5323";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Random rand;
   try {
   do
     {
     Guid gui1;
     rand = new Random((int)DateTime.Now.Ticks);
     strLoc = "Loc_298hf";
     gui1 = new Guid(10, 11, 12, 0, 1, 2, 3,4, 5, 6, 7);
     iCountTestcases++;
     Console.WriteLine(gui1);
     if(!gui1.ToString().Equals("0000000a-000b-000c-0001-020304050607"))
       {
       iCountErrors++;
       printerr("Error_92thg! Incorrect value, got=="+gui1);
       }
     strLoc = "Loc_298yt";
     gui1 = new Guid(0, 0, 0, 0, 0,0,0,0,0,0,0);
     iCountTestcases++;
     if(!gui1.ToString().Equals("00000000-0000-0000-0000-000000000000"))
       {
       iCountErrors++;
       printerr("Error_019jt! Incorrect guid value, got=="+gui1);
       }
     strLoc = "Loc_20fhs";
     int a;
     short  b, c;
     Byte d, e, f, g, h, i , j, k;
     iCountTestcases++;
     for (int ii = 0 ; ii < 8000 ; ii++)
       {
       a = rand.Next(Int32.MinValue, Int32.MaxValue);
       b = (short)rand.Next(Int16.MinValue, Int16.MaxValue);
       c = (short)rand.Next(Int16.MinValue, Int16.MaxValue);
       d = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       e = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       f = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       g = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       h = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       i = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       j = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       k = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       gui1 = new Guid(a, b, c, d, e, f, g, h, i, j, k);
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
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5323Ctor_issuuuuuuuu cbA = new Co5323Ctor_issuuuuuuuu();
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
