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
using System.Resources;
using System.IO;
using System.Reflection;
using System.Collections;
public class Co5209AddResource_str_ubArr
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "ResourceWriter.AddResource(String,String)";
 public static String s_strTFName        = "Co5209AddResource_str_ubArr.cs";
 public static String s_strTFAbbrev      = "Co5209";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   Co5209AddResource_str_ubArr cc = new Co5209AddResource_str_ubArr();
   IDictionaryEnumerator idic;
   try {
   do
     {
     ResourceWriter resWriter;
     ResourceReader resReader;
     int statusBits = 0;
     int resultBits = 0;
     Byte[] ubArr = null;
     if(File.Exists(Environment.CurrentDirectory+"\\Co5209.resources"))
       File.Delete(Environment.CurrentDirectory+"\\Co5209.resources");
     strLoc = "Loc_204gh";
     resWriter = new ResourceWriter("Co5209.resources");
     strLoc = "Loc_209tj";
     try
       {
       iCountTestcases++;
       resWriter.AddResource(null, ubArr);
       iCountErrors++;
       printerr("Error_20fhs! Expected Exception not thrown");
       }
     catch (ArgumentException) {}
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_2t0jg! Unexpected exception exc=="+exc.ToString());
       }
     strLoc = "Loc_t4j80";
     ubArr = null;
     iCountTestcases++;
     try
       {
       resWriter.AddResource("key with null value", ubArr);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_59ufd! Unexpected exc=="+exc.ToString());
       }
     Byte[] ubArr1 = {(Byte)'v', (Byte)'a', (Byte)'l', (Byte)'u', (Byte)'e', (Byte)' ', (Byte)'1',};
     Byte[] ubArr2 = {(Byte)'v', (Byte)'a', (Byte)'l', (Byte)'u', (Byte)'e', (Byte)' ', (Byte)'2',};
     Byte[] ubArr3 = {(Byte)'v', (Byte)'a', (Byte)'l', (Byte)'u', (Byte)'e', (Byte)' ', (Byte)'3',};
     Byte[] ubArr4 = {(Byte)'v', (Byte)'a', (Byte)'l', (Byte)'u', (Byte)'e', (Byte)' ', (Byte)'4',};
     Byte[] ubArr5 = {(Byte)'v', (Byte)'a', (Byte)'l', (Byte)'u', (Byte)'e', (Byte)' ', (Byte)'5',};
     Byte[] ubArr6 = {(Byte)'v', (Byte)'a', (Byte)'l', (Byte)'u', (Byte)'e', (Byte)' ', (Byte)'6',};
     strLoc = "Loc_59ugd";
     resWriter.AddResource("key 1", ubArr1);
     resWriter.AddResource("key 2", ubArr2);
     resWriter.AddResource("key 3", ubArr3);
     resWriter.AddResource("key 4", ubArr4);
     resWriter.AddResource("key 5", ubArr5);
     resWriter.AddResource("key 6", ubArr6);
     strLoc = "Loc_230rj";
     iCountTestcases++;
     try
       {
       resWriter.AddResource("key 1", ubArr6);
       iCountErrors++;
       printerr("Error_2th8e! Names are not unique");
       }
     catch (ArgumentException) {}
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_298hg! Unexpected exception=="+exc.ToString());
       }
     resWriter.Generate();
     iCountTestcases++;
     if(!File.Exists(Environment.CurrentDirectory+"\\Co5209.resources"))
       {
       iCountErrors++;
       printerr("Error_23094! Expected file was not created");
       }
     resWriter.Close();
     strLoc = "Loc_30tud";
     resReader = new ResourceReader(Environment.CurrentDirectory+"\\Co5209.resources");
     strLoc = "Loc_0576cd";
     byte[] btTemp;
     Boolean fNotEqual;
     IDictionaryEnumerator resEnumerator = resReader.GetEnumerator();
     idic = resReader.GetEnumerator();
     while(idic.MoveNext())
       {
       if(idic.Key.Equals("key 1")) {
       btTemp = (byte[])idic.Value;
       fNotEqual = false;
       for(int i=0;i<btTemp.Length; i++) {
       if(btTemp[i]!=ubArr1[i])
	 fNotEqual = true;
       }
       if(!fNotEqual)
	 statusBits = statusBits | 0x1;
       } else if(idic.Key.Equals("key 2")) {
       btTemp = (byte[])idic.Value;
       fNotEqual = false;
       for(int i=0;i<btTemp.Length; i++) {
       if(btTemp[i]!=ubArr2[i])
	 fNotEqual = true;
       }
       if(!fNotEqual)
	 statusBits = statusBits | 0x2;
       } else if(idic.Key.Equals("key 3")) {
       btTemp = (byte[])idic.Value;
       fNotEqual = false;
       for(int i=0;i<btTemp.Length; i++) {
       if(btTemp[i]!=ubArr3[i])
	 fNotEqual = true;
       }
       if(!fNotEqual)
	 statusBits = statusBits | 0x4;
       } else if(idic.Key.Equals("key 4")) {
       btTemp = (byte[])idic.Value;
       fNotEqual = false;
       for(int i=0;i<btTemp.Length; i++) {
       if(btTemp[i]!=ubArr4[i])
	 fNotEqual = true;
       }
       if(!fNotEqual)
	 statusBits = statusBits | 0x8;
       } else if(idic.Key.Equals("key 5")) {
       btTemp = (byte[])idic.Value;
       fNotEqual = false;
       for(int i=0;i<btTemp.Length; i++) {
       if(btTemp[i]!=ubArr5[i])
	 fNotEqual = true;
       }
       if(!fNotEqual)
	 statusBits = statusBits | 0x10;
       } else if(idic.Key.Equals("key 6")) {
       btTemp = (byte[])idic.Value;
       fNotEqual = false;
       for(int i=0;i<btTemp.Length; i++) {
       if(btTemp[i]!=ubArr6[i])
	 fNotEqual = true;
       }
       if(!fNotEqual)
	 statusBits = statusBits | 0x20;
       }
       }
     resultBits = 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20;
     iCountTestcases++;
     if(statusBits != resultBits)
       {
       iCountErrors++;
       printerr("Error_238fh! The names are incorrect, StatusBits=="+statusBits);
       }
     strLoc = "Loc_t0dds";
     iCountTestcases++;
     if(idic.MoveNext())
       {
       iCountErrors++;
       printerr("Error_2398r! , There shouldn't have been more elementes : GetValue=="+idic.Value);
       }
     resReader.Close();
     strLoc = "Loc_957fd";
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
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
   Co5209AddResource_str_ubArr cbA = new Co5209AddResource_str_ubArr();
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
