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
using System.Reflection;
using Microsoft.Win32;
public class Co8521CompareTo_obj
{
 public static String s_strActiveBugNums			= "";
 public static readonly String s_strDtTmVer      = "";
 public static String s_strClassMethod			= "Guid.CompareTo(Object)";
 public static String s_strTFName				= "Co8521CompareTo_obj.cs";
 public static readonly String s_strTFAbbrev     = s_strTFName.Substring(0, 6);
 public static readonly String s_strTFPath       = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   Guid gd1;
   Guid gd2;
   Byte[] bytes;
   try
     {
     strLoc = "Loc_001oo";
     gd1 = Guid.NewGuid();
     strValue = "Hello World";
     iCountTestcases++;
     try {
     gd1.CompareTo(strValue);
     iCountErrors++;
     Console.WriteLine( "Err_765sgs! Exception now thrown");
     }catch(ArgumentException){
     }catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( "Error_94f8v! Unexpected Exception, got exc=="+exc.GetType().Name);
     }
     strLoc = "Loc_001oo";
     bytes = new Byte[16];
     for(int i=0; i<bytes.Length; i++)
       bytes[i] = (byte)i;
     gd1 = new Guid(bytes);
     gd2 = new Guid(bytes);
     iCountTestcases++;
     if(gd1.CompareTo(gd2)!=0){
     iCountErrors++;
     Console.WriteLine( "Error_8743dgs! wrong value returned");
     }
     iCountTestcases++;
     if(gd1.CompareTo(gd1)!=0){
     iCountErrors++;
     Console.WriteLine("Error_3276gg! wrong value returned");
     }
     bytes = new Byte[16];
     for(int i=0; i<bytes.Length; i++)
       bytes[i] = (byte)i;			
     gd1 = new Guid(bytes);
     bytes = new Byte[16];
     for(int i=0, j=16; i<bytes.Length; i++, j--)
       bytes[i] = (byte)j;			
     gd2 = new Guid(bytes);
     iCountTestcases++;
     if(gd1.CompareTo(gd2)>=0){
     iCountErrors++;
     Console.WriteLine("Error_9734sg! wrong value returned, \r\n {0} \r\n {1}", gd1.ToString(), gd2.ToString());
     }
     bytes = new Byte[16];
     for(int i=0; i<bytes.Length; i++)
       bytes[i] = (byte)i;			
     gd1 = new Guid(bytes);
     bytes = new Byte[16];
     for(int i=0, j=16; i<bytes.Length; i++, j--)
       bytes[i] = (byte)j;			
     gd2 = new Guid(bytes);
     iCountTestcases++;
     if(gd2.CompareTo(gd1)<=0){
     iCountErrors++;
     Console.WriteLine("Error_78934dg! wrong value returned");
     }
     bytes = new Byte[16];
     for(int i=0; i<bytes.Length; i++)
       bytes[i] = (byte)i;			
     gd1 = new Guid(bytes);
     iCountTestcases++;
     if(gd2.CompareTo(null)<=0){
     iCountErrors++;
     Console.WriteLine("Error_8473dsg! wrong value returned");
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
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8521CompareTo_obj cbA = new Co8521CompareTo_obj();
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
