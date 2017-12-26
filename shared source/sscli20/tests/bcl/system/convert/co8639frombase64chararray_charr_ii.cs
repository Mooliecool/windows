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
public class Co8639FromBase64CharArray_chArr_ii
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.FromBase64CharArray(Char[], Int32, Int32)";
 public static String s_strTFName        = "Co8639FromBase64CharArray_chArr_ii.cs";
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
   String str2;
   Char[] chArr;
   Byte[] returnValue;
   UInt32 threeByteRep;
   Random rand;
   Int32 numberOfTimes;
   Int32 count=0;
   Int32 iValue;
   StringBuilder builder;
   ArrayList exclusionList;
   Random random = new Random();
   try
     {
     strLoc = "Loc_934sdg";
     iCountTestcases++;
     str1 = "test";
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(returnValue.Length!=3){
     iCountErrors++;
     Console.WriteLine("Err_743rewg! Unexpected returned result, " + returnValue.Length);
     }else{
     threeByteRep = (uint)((returnValue[0]<<16) | (returnValue[1]<<8) | returnValue[2]);
     if(((threeByteRep>>18) != 45)
	|| (((threeByteRep<<14)>>26) != 30)
	|| (((threeByteRep<<20)>>26) != 44)
	|| (((threeByteRep<<26)>>26) != 45)
	){
     iCountErrors++;
     Console.WriteLine("Err_834sdg! Unexpected returned result");
     }			
     }
     if(!Convert.ToBase64String(returnValue).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_835sdg! Unexpected returned result");
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = null;			
     try{
     returnValue = Convert.FromBase64CharArray(null, 0, 3);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentNullException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_20478twsdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     chArr = str1.ToCharArray();
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.FromBase64CharArray(chArr, random.Next(Int32.MinValue, 0), chArr.Length);
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_2074tsdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.FromBase64CharArray(chArr, random.Next(1000, Int32.MaxValue), chArr.Length);
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_2074tsdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     try
       {
       returnValue = Convert.FromBase64CharArray(chArr, Int32.MaxValue, chArr.Length);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_2074tsdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     try
       {
       returnValue = Convert.FromBase64CharArray(chArr, Int32.MinValue, chArr.Length);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_2074tsdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     try{
     returnValue = Convert.FromBase64CharArray(chArr, (chArr.Length +5), chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_204375wdsg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.FromBase64CharArray(chArr, 0, random.Next(Int32.MinValue, 0));
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_24076tsdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.FromBase64CharArray(chArr, 0, random.Next(1000, Int32.MaxValue));
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_24076tsdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     try
       {
       returnValue = Convert.FromBase64CharArray(chArr, 0, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_24076tsdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     try
       {
       returnValue = Convert.FromBase64CharArray(chArr, 0, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_24076tsdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length+1);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_2407sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 1, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_21074sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, 0);
     if(returnValue.Length!=0){
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! Unexpected result returned, " + returnValue.Length);
     }
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_23407sfdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "No";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_879345d! No Exception returned");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_20746tdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "NoMore";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_9745sgd! No Exception returned");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_93476tsg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_974235sdg";
     iCountTestcases++;
     str1 = "2-34";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_934tsdg! No Exception returned");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_93746tsg";
     iCountTestcases++;
     exclusionList = new ArrayList();
     exclusionList.Add(43);
     for(int i=47;i<=57;i++)
       exclusionList.Add(i);
     for(int i=65;i<=90;i++)
       exclusionList.Add(i);
     for(int i=97;i<=122;i++)
       exclusionList.Add(i);
     rand = new Random();
     numberOfTimes = 10;
     for(int i=0; i<numberOfTimes; i++){
     count = 0;
     builder = new StringBuilder("abc");
     do{
     iValue = rand.Next(65535);
     if(!exclusionList.Contains(iValue)){
     builder.Insert(1, ((Char)iValue));
     count++;
     }
     }while(count==0);
     str1 = builder.ToString();
     if(str1.Length!=4)
       throw new Exception("Loc_54wgsg! wrong argumetns");
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_3947sdg! No Exception returned, " + str1);
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     }
     strLoc = "Loc_97346trsg";
     iCountTestcases++;
     iCountTestcases++;
     str1 = "abc=";
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(!Convert.ToBase64String(returnValue).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_83gsd! Unexpected returned result, " + Convert.ToBase64String(returnValue));
     }
     strLoc = "Loc_8374tsdg";
     iCountTestcases++;
     str1 = "No=n";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_39407g! No Exception returned, " + Convert.ToBase64String(returnValue));
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_204dsg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_8374tsdg";
     iCountTestcases++;
     str1 = "abc=====";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_39407g! No Exception returned, " + Convert.ToBase64String(returnValue));
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_204dsg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_97346trsg";
     iCountTestcases++;
     str1 = "ab==";
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(returnValue.Length !=1){
     iCountErrors++;
     Console.WriteLine("Err_83745sdg! Unexpected returned result, " + returnValue.Length);
     }
     String returnString = Convert.ToBase64String(returnValue);
     if(returnString.Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_983745sd! Unexpected returned result, " + returnString);
     }
     if(!returnString.Substring(0, 1).Equals(str1.Substring(0, 1))){
     iCountErrors++;
     Console.WriteLine("Err_832745sd! Unexpected returned result");
     }
     strLoc = "Loc_8374tsdg";
     iCountTestcases++;
     str1 = "a===";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_94375sdg! No Exception returned, " + Convert.ToBase64String(returnValue));
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_w9475tsg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_3847sg";
     iCountTestcases++;
     builder = new StringBuilder();
     for(int i=0; i<10000; i++)
       builder.Append('a');
     str1 = builder.ToString();
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(!Convert.ToBase64String(returnValue).Equals(str1)){
     iCountErrors++;
     Console.WriteLine("Err_89374sdg! Unexpected returned result, " + Convert.ToBase64String(returnValue));
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     str2 = str1;
     str1 = str1.PadLeft(rand.Next(1000, 2000), ' ');
     str1 = str1.PadLeft(rand.Next(1000, 2000), ' ');
     str1 = str1.Insert(str1.IndexOf('e'), new String(' ', rand.Next(1000, 2000)));
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(returnValue.Length !=3){
     iCountErrors++;
     Console.WriteLine("Err_837454sdg! Unexpected returned result, " + returnValue.Length);
     }
     if(!Convert.ToBase64String(returnValue).Equals(str2)){
     iCountErrors++;
     Console.WriteLine("Err_83gsd! Unexpected returned result, " + Convert.ToBase64String(returnValue));
     }
     strLoc = "Loc_3947tsfg";
     iCountTestcases++;
     str1 = "test";
     str2 = str1;
     str1 = str1.PadLeft(rand.Next(1000, 2000), (Char)9);
     str1 = str1.PadLeft(rand.Next(1000, 2000), (Char)9);
     str1 = str1.Insert(str1.IndexOf('e'), new String((Char)9, rand.Next(1000, 2000)));
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(returnValue.Length !=3){
     iCountErrors++;
     Console.WriteLine("Err_837454sdg! Unexpected returned result, " + returnValue.Length);
     }
     if(!Convert.ToBase64String(returnValue).Equals(str2)){
     iCountErrors++;
     Console.WriteLine("Err_83gsd! Unexpected returned result, " + Convert.ToBase64String(returnValue));
     }
     str1 = "test";
     str2 = str1;
     str1 = str1.PadLeft(rand.Next(1000, 2000), (Char)10);
     str1 = str1.PadLeft(rand.Next(1000, 2000), (Char)10);
     str1 = str1.Insert(str1.IndexOf('e'), new String((Char)10, rand.Next(1000, 2000)));
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(returnValue.Length !=3){
     iCountErrors++;
     Console.WriteLine("Err_837454sdg! Unexpected returned result, " + returnValue.Length);
     }
     if(!Convert.ToBase64String(returnValue).Equals(str2)){
     iCountErrors++;
     Console.WriteLine("Err_83gsd! Unexpected returned result, " + Convert.ToBase64String(returnValue));
     }
     str1 = "test";
     str2 = str1;
     str1 = str1.PadLeft(rand.Next(1000, 2000), (Char)13);
     str1 = str1.PadLeft(rand.Next(1000, 2000), (Char)13);
     str1 = str1.Insert(str1.IndexOf('e'), new String((Char)13, rand.Next(1000, 2000)));
     chArr = str1.ToCharArray();
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     if(returnValue.Length !=3){
     iCountErrors++;
     Console.WriteLine("Err_837454sdg! Unexpected returned result, " + returnValue.Length);
     }
     if(!Convert.ToBase64String(returnValue).Equals(str2)){
     iCountErrors++;
     Console.WriteLine("Err_83gsd! Unexpected returned result, " + Convert.ToBase64String(returnValue));
     }
     strLoc = "Loc_934sdg";
     iCountTestcases++;
     str1 = "abcdabc=abcd";			
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_879345d! No Exception returned");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_20746tdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     str1 = "abcdab==abcd";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_879345d! No Exception returned");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_20746tdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     str1 = "abcda===abcd";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_879345d! No Exception returned");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_20746tdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     str1 = "abcd====abcd";
     chArr = str1.ToCharArray();
     try{
     returnValue = Convert.FromBase64CharArray(chArr, 0, chArr.Length);
     iCountErrors++;
     Console.WriteLine("Err_879345d! No Exception returned");
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_20746tdg! Unexpected Exception returned, " + ex.GetType().Name);
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
   Co8639FromBase64CharArray_chArr_ii cbA = new Co8639FromBase64CharArray_chArr_ii();
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
