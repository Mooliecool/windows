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
using System.Collections;
using System.Globalization;
using System.IO;
public class Co8575NewGuid
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Guid.NewGuid()";
 public static String s_strTFName        = "Co8575NewGuid.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Guid gd = new Guid();
   Guid gd1;
   Guid gd2;
   Hashtable table;
   try
     {
     strLoc = "Loc_93475sdg";
     iCountTestcases++;
     gd1 = Guid.NewGuid();
     gd2 = Guid.NewGuid();
     if(gd1==gd2){
     iCountErrors++;
     Console.WriteLine("Err_734wdg! wrong result returned, ");
     }
     strLoc = "Loc_93475sdg";
     iCountTestcases++;
     table = new Hashtable();             
     try{
     for(int i=0; i<1000; i++) {
       gd = Guid.NewGuid();
       table.Add(gd, null);
       }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("GUID: "+gd);
     Console.WriteLine("Err_985tsdg! Unexpected exception thrown, " + ex.GetType().Name);
     }               
     iCountTestcases++;
     gd1 = Guid.NewGuid();
     gd2 = new Guid(gd1.ToByteArray());
     if(gd1!=gd2){
     iCountErrors++;
     Console.WriteLine("Err_932475tsgd! wrong result returned, ");
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8575NewGuid cbA = new Co8575NewGuid();
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
