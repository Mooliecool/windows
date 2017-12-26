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
using System.Resources;
public class Co5445Ctor_iostr
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "ResourceWriter ctor (Stream)"; 
 public static String s_strTFName        = "Co5445Ctor_iostr.cs";
 public static String s_strTFAbbrev      = "Co5445";
 public static String s_strTFPath        = Environment.CurrentDirectory;  
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   IDictionaryEnumerator idic;
   try
     {
     ResourceWriter resWriter;
     ResourceReader resReader;
     Stream stream = null;
     MemoryStream ms;
     FileStream fs;
     if(File.Exists(Environment.CurrentDirectory+"\\Co5445.resources"))
       File.Delete(Environment.CurrentDirectory+"\\Co5445.resources");
     strLoc = "Loc_20xcy";
     stream = null;
     iCountTestcases++;
     try {
     resWriter = new ResourceWriter(stream);
     iCountErrors++;
     printerr( "Error_29vc8! ArgumentNullException expected");
     } catch ( ArgumentNullException aExc) {
     } catch ( Exception exc) {
     iCountErrors++;
     printerr( "Error_10s9x! ArgumentNullException expected , got exc=="+exc.ToString());
     }
     strLoc = "Loc_3f98d";
     new FileStream("Co5445.resources", FileMode.Create).Close();
     fs = new FileStream("Co5445.resources", FileMode.Open, FileAccess.Read, FileShare.None);
     iCountTestcases++;
     try {
     resWriter = new ResourceWriter(fs);
     iCountErrors++;
     printerr( "Error_2c88s! ArgumentException expected");
     } catch (ArgumentException aExc) {
     } catch ( Exception exc) {
     iCountErrors++;
     printerr( "Error_2x0zu! ArgumentException expected, got exc=="+exc.ToString());
     }
     strLoc = "Loc_f0843";
     ms = new MemoryStream();
     resWriter = new ResourceWriter(ms);
     resWriter.AddResource("Harrison", "Ford");
     resWriter.AddResource("Mark", "Hamill");
     resWriter.Generate();
     ms.Position = 0;
     resReader = new ResourceReader(ms);
     idic = resReader.GetEnumerator();
     idic.MoveNext();
     iCountTestcases++;
     if(!idic.Value.Equals("Ford"))
       {
       iCountErrors++;
       printerr( "Error_2d0s9 Expected==Ford, value=="+idic.Value.ToString());
       }
     idic.MoveNext();
     iCountTestcases++;
     if(!idic.Value.Equals("Hamill"))
       {
       iCountErrors++;
       printerr( "Error_2ce80 Expected==Hamill, value=="+idic.Value.ToString());
       }
     strLoc = "Loc_20984";
     iCountTestcases++;
     if(idic.MoveNext())
       {
       iCountErrors++;
       printerr( "Error_f4094! Should have hit the end of the stream already");
       }
     fs.Close();
     strLoc = "Loc_04853fd";
     if(File.Exists(Environment.CurrentDirectory+"\\Co5445.resources"))
       File.Delete(Environment.CurrentDirectory+"\\Co5445.resources");
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
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5445Ctor_iostr cbA = new Co5445Ctor_iostr();
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
