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
using System.Text;
using System.Reflection;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co8642ToBase64String_all: IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToBase64String", new Type[]{typeof(Byte[])}));
   list.Add(type.GetMethod("ToBase64String", new Type[]{typeof(Byte[]), typeof(Int32), typeof(Int32)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToBase64String: All Methods";
 public static String s_strTFName        = "Co8642ToBase64String_all.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str1;
   Byte[] returnValue;
   Random random = new Random();
   try
     {
     strLoc = "Loc_934sdg";
     iCountTestcases++;
     str1 = "test";
     returnValue = Convert.FromBase64String(str1);
     if(!Convert.ToBase64String(returnValue).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected returned result");
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     returnValue = null;			
     try{
     str1 = Convert.ToBase64String(returnValue);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentNullException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     returnValue = new Byte[4];
     for(int i=0; i<4; i++)	
       returnValue[i] = (Byte)(i+5);
     str1 = "BQYHCA==";
     if(!Convert.ToBase64String(returnValue).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_3845sdg! Unexpected returned result, " + Convert.ToBase64String(returnValue));
     }
     strLoc = "Loc_3467tsg";
     iCountTestcases++;
     str1 = "AAAA";
     returnValue = Convert.FromBase64String(str1);
     if(!Convert.ToBase64String(returnValue).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_23745w34ts! Unexpected returned result");
     }
     str1 = "AAAAAAAA";
     returnValue = Convert.FromBase64String(str1);
     if(!Convert.ToBase64String(returnValue).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_874325gd! Unexpected returned result");
     }						
     strLoc = "Loc_3956tg";
     iCountTestcases++;
     str1 = "test";
     returnValue = Convert.FromBase64String(str1);
     if(!Convert.ToBase64String(returnValue, 0, 3).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected returned result");
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     returnValue = null;			
     try{
     str1 = Convert.ToBase64String(returnValue, 0, 0);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentNullException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";			
     returnValue = Convert.FromBase64String(str1);
     for(int i=0; i<20; i++)
       {
       try
	 {
	 str1 = Convert.ToBase64String(returnValue, random.Next(Int32.MinValue, 0), 3);
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     try
       {
       str1 = Convert.ToBase64String(returnValue, Int32.MinValue, 3);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     try
       {
       str1 = Convert.ToBase64String(returnValue, Int32.MaxValue, 3);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     str1 = "test";			
     returnValue = Convert.FromBase64String(str1);
     for(int i=0; i<20; i++)
       {
       try
	 {
	 str1 = Convert.ToBase64String(returnValue, 1, random.Next(Int32.MinValue, 0));
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     try
       {
       str1 = Convert.ToBase64String(returnValue, 1, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     try
       {
       str1 = Convert.ToBase64String(returnValue, 1, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     str1 = "test";			
     returnValue = Convert.FromBase64String(str1);
     try{
     str1 = Convert.ToBase64String(returnValue, 0, 4);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     str1 = "test";			
     returnValue = Convert.FromBase64String(str1);
     try{
     str1 = Convert.ToBase64String(returnValue, 1, 3);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     str1 = "test";			
     returnValue = Convert.FromBase64String(str1);
     try{
     str1 = Convert.ToBase64String(returnValue, 0, -1);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     iCountTestcases++;
     str1 = "test";			
     returnValue = Convert.FromBase64String(str1);
     str1 = Convert.ToBase64String(returnValue, 0, 0);
     if(!str1.Equals(String.Empty)){
     iCountErrors++;
     Console.WriteLine("Err_8934sd! Unexpected returned result");
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
   Co8642ToBase64String_all cbA = new Co8642ToBase64String_all();
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
