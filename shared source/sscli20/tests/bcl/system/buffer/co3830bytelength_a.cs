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
public class Co3830ByteLength_A
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Buffer.ByteLength(Array a)";
 public static String s_strTFName        = "Co3830ByteLength_A.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Int32[] arrIntFirst = null;
   Int32[] arrIntSec = null;
   Int16[] arrDifferentTypes = null;
   Object[] arrObjects = null;
   String[] arrStrings = null;
   Int32 iByteLength;
   try
     {
     strLoc = "Loc_386fsd";
     Console.WriteLine();
     arrIntFirst = new Int32[10];
     for(int i=0; i<arrIntFirst.Length; i++)
       {
       arrIntFirst[i] = i+10;
       }
     iByteLength = Buffer.ByteLength(arrIntFirst);
     iCountTestcases++;
     if(iByteLength != arrIntFirst.Length*4)
       {
       iCountErrors++;
       Console.WriteLine("Err_357cd! length value not correct=" + iByteLength.ToString());
       }
     strLoc = "Loc_0357vsd";
     arrStrings = new String[3];
     arrStrings[0] = "Due to excess alcohol levels in the blood stream,";
     arrStrings[1] = "this sentense would become a big blurrr";
     arrStrings[2] = "befoere our bery ueyrs";
     try
       {
       iCountTestcases++;
       iByteLength = Buffer.ByteLength(arrStrings);
       iCountErrors++;
       Console.WriteLine("Err_0457gfd! Exception not thrown");
       }
     catch(ArgumentException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_056csad! unexpected exception thrown," + ex.ToString());
       }
     strLoc = "Loc_0457vdf";
     arrDifferentTypes = new Int16[10];
     for(int i=0; i<arrDifferentTypes.Length; i++)
       {
       arrDifferentTypes[i] = (short)(i+10);
       }
     iByteLength = Buffer.ByteLength(arrDifferentTypes);
     iCountTestcases++;
     if(iByteLength != arrDifferentTypes.Length*2)
       {
       iCountErrors++;
       Console.WriteLine("Err_035qm! length value not correct=" + iByteLength.ToString());
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co3830ByteLength_A cbA = new Co3830ByteLength_A();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
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
