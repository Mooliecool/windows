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
public class Co8536ctor_void
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "UIntPtr.ctor(void *)";
 public static readonly String s_strTFName        = "Co8536ctor_void.cs";
 public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public unsafe virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   UIntPtr ip1;
   void* vd1;
   void* vd2;
   int* iptr1;
   Int32 iValue;          
   Int64 lValue;
   Boolean fValue;
   Char chValue;
   Byte btValue;
   SByte sbValue;
   Int16 sValue;
   UInt16 usValue;
   UInt32 uiValue;
   UInt64 ulValue;
   DateTime dt1;
   String strValue;
   Int32[] iArr = {1, 2, 3, 4, 5};
   MyEnum en1;
   String strReturned;
   try {
   strLoc = "Loc_743wg";
   iValue = 16;
   vd1 = &iValue;
   ip1 = new UIntPtr(vd1);
   vd2 = ip1.ToPointer();
   iCountTestcases++;
   if((*((int*)vd2)) != iValue){
   iCountErrors++;
   Console.WriteLine("Err_2975sf! Wrong value returned, " + (*((int*)vd2)));
   }
   strLoc = "Loc_0084wf";
   lValue = 16;
   vd1 = &lValue;
   ip1 = new UIntPtr(vd1);
   vd2 = ip1.ToPointer();
   iCountTestcases++;
   if((*((long*)vd2)) != lValue){
   iCountErrors++;
   Console.WriteLine("Err_974325sdg! Wrong value returned");
   }
   if (BitConverter.IsLittleEndian)
   {
	   strLoc = "Loc_0084wf";
	   lValue = 16;
	   vd1 = &lValue;
	   ip1 = new UIntPtr(vd1);
	   iptr1 = (int*)ip1.ToPointer();
	   iCountTestcases++;
	   if((*iptr1) != lValue)
	   {
		   iCountErrors++;
		   Console.WriteLine("Err_974325sdg! Wrong value returned! check the endiannees of this machine!!!, " + (*iptr1));
	   }
   }
   strLoc = "Loc_00845wsdg";
   lValue = Int64.MaxValue;
   vd1 = &lValue;
   ip1 = new UIntPtr(vd1);
   vd2 = ip1.ToPointer();
   iCountTestcases++;
   if((*((long*)vd2)) != lValue){
   iCountErrors++;
   Console.WriteLine("Err_94753sdg! Wrong value returned");
   }
   if (BitConverter.IsLittleEndian)
   {
	   strLoc = "Loc_875esfg";
	   lValue = Int64.MaxValue;
	   vd1 = &lValue;
	   ip1 = new UIntPtr(vd1);
	   iptr1 = (int*)ip1.ToPointer();
	   iCountTestcases++;
	   if((*iptr1) != -1)
	   {
		   iCountErrors++;
		   Console.WriteLine("Err_756wrg! Wrong value returned! , " + (*iptr1));
	   }
   }
   strLoc = "Loc_008742sf";
   fValue = true;
   vd1 = &fValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((Boolean*)ip1.ToPointer())) != fValue){
   iCountErrors++;
   Console.WriteLine("Err_984753sdg! Wrong value returned!");
   }
   strLoc = "Loc_735sdg";
   chValue = 'a';
   vd1 = &chValue;
   ip1 = new UIntPtr(vd1);
   iptr1 = (int*)ip1.ToPointer();
   iCountTestcases++;
   if((*((char*)ip1.ToPointer())) != chValue){
   iCountErrors++;
   Console.WriteLine("Err_9745sg! Wrong value returned!");
   }
   strLoc = "Loc_735sdg";
   btValue = 5;
   vd1 = &btValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((byte*)ip1.ToPointer())) != btValue){
   iCountErrors++;
   Console.WriteLine("Err_7453rsg! Wrong value returned!");
   }
   strLoc = "Loc_9743dg";
   sbValue = -5;
   vd1 = &sbValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((SByte*)ip1.ToPointer())) != sbValue){
   iCountErrors++;
   Console.WriteLine("Err_97345sg! Wrong value returned!");
   }
   strLoc = "Loc_9743dg";
   sValue = -5;
   vd1 = &sValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((Int16*)ip1.ToPointer())) != sValue){
   iCountErrors++;
   Console.WriteLine("Err_9374dg! Wrong value returned!");
   }
   strLoc = "Loc_9743dg";
   usValue = 5;
   vd1 = &usValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((UInt16*)ip1.ToPointer())) != usValue){
   iCountErrors++;
   Console.WriteLine("Err_9874sgd! Wrong value returned!");
   }
   strLoc = "Loc_9743dg";
   uiValue = 5;
   vd1 = &uiValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((UInt32*)ip1.ToPointer())) != uiValue){
   iCountErrors++;
   Console.WriteLine("Err_3463sg! Wrong value returned!");
   }
   strLoc = "Loc_9743dg";
   ulValue = 5;
   vd1 = &ulValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((UInt64*)ip1.ToPointer())) != ulValue){
   iCountErrors++;
   Console.WriteLine("Err_8274sdg! Wrong value returned!");
   }
   strLoc = "Loc_0007432sf";
   dt1 = DateTime.Now;
   vd1 = &dt1;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((DateTime*)ip1.ToPointer())) != dt1){
   iCountErrors++;
   Console.WriteLine("Err_9734sdg! Wrong value returned!");
   }
   strLoc = "Loc_20875sg";
   strValue = "Hello World";
   fixed(Char* chPValue = strValue){
   vd1 = chPValue;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((Char*)ip1.ToPointer())) != 'H'){
   iCountErrors++;
   Console.WriteLine("Err_874dsg! Wrong value returned!");
   }
   iCountTestcases++;
   if((*((Char*)ip1.ToPointer() + 2)) != 'l'){
   iCountErrors++;
   Console.WriteLine("Err_9347sdg! Wrong value returned!");
   }
   (*((Char*)ip1.ToPointer() + 2)) = 'm';
   iCountTestcases++;
   if((*((Char*)ip1.ToPointer() + 2)) != 'm'){
   iCountErrors++;
   Console.WriteLine("Err_075wrg! Wrong value returned!");
   }
   strReturned = "Hemlo World";
   if(strReturned != strValue){
   iCountErrors++;
   Console.WriteLine("Err_87453sg! We are playing with fire here!");
   }				
   }
   strLoc = "Loc_20875sg";
   strValue = "Hello World";
   fixed(int* iptr2 = iArr){
   vd1 = iptr2;
   ip1 = new UIntPtr(vd1);
   iCountTestcases++;
   if((*((int*)ip1.ToPointer())) != 1){
   iCountErrors++;
   Console.WriteLine("Err_9376dg! Wrong value returned!");
   }
   iCountTestcases++;
   if((*((int*)ip1.ToPointer() + 2)) != 3){
   iCountErrors++;
   Console.WriteLine("Err_94735ds! Wrong value returned!");
   }
   (*((int*)ip1.ToPointer() + 2)) = 25;
   iCountTestcases++;
   if((*((int*)ip1.ToPointer() + 2)) != 25){
   iCountErrors++;
   Console.WriteLine("Err_753tsdg! Wrong value returned!");
   }
   }
   strLoc = "Loc_907346sdg";
   en1 = MyEnum.ONE;
   vd1 = &en1;
   ip1 = new UIntPtr(vd1);
   vd2 = ip1.ToPointer();
   iCountTestcases++;
   if((*((MyEnum*)vd2)) != en1){
   iCountErrors++;
   Console.WriteLine("Err_9745sg! Wrong value returned, " + (*((MyEnum*)vd2)));
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
     Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8536ctor_void cbA = new Co8536ctor_void();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
enum MyEnum{
  ONE = 1,
  TWO = 2,
  FOUR = 4,
}
