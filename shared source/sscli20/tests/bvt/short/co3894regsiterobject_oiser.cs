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
using System.Runtime.Serialization;
using System.Reflection;
public class Co3894RegsiterObject_OISer
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "ObjectManager.RegsiterObject(Object, Int64, SerializationInfo)";
 public static String s_strTFName        = "Co3894RegsiterObject_OISer.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   ObjectManager objmgr1 = null;
   ISurrogateSelector isur = null;
   StreamingContext sc1 = new StreamingContext(StreamingContextStates.All);
   ObjectIDGenerator objid1 = null;
   TestFixup tstfxp1;
   Int64 iRootID;
   Int64 iChildID;
   Int32 iValue;
   bool fFirstTime;
   MemberInfo[] members = null;
   SerializationInfo serInfo1;
   try {
   do
     {
     strLoc = "Loc_8345vdfv";
     tstfxp1 = new TestFixup();
     iValue = 10;
     objid1 = new ObjectIDGenerator();
     iRootID = objid1.GetId(tstfxp1, out fFirstTime);
     iChildID = objid1.GetId(iValue, out fFirstTime);
     members = FormatterServices.GetSerializableMembers(tstfxp1.GetType());
     objmgr1 = new ObjectManager(isur, sc1);
     objmgr1.RecordDelayedFixup(iRootID, "iFixupValue", iChildID);
     serInfo1 = new SerializationInfo(typeof(TestFixup), new FormatterConverter());
     objmgr1.RegisterObject(tstfxp1, iRootID, serInfo1);
     iCountTestcases++;
     if(objmgr1.GetObject(iRootID)==null)
       {
       iCountErrors++;
       Console.WriteLine("Err_0943fd! Null returned");
       }
     objmgr1.RegisterObject(iValue, iChildID);
     objmgr1.DoFixups();
     strLoc = "Loc_017ged";
     iCountTestcases++;
     if(tstfxp1.iFixupValue != iValue)
       {
       iCountErrors++;
       Console.WriteLine("Err_753cd! Expected value not returned, " + tstfxp1.iFixupValue.ToString() + ", expected, " + iValue.ToString());
       }
     strLoc = "Loc_0358vdf";
     try
       {
       iCountTestcases++;
       objmgr1.RegisterObject(null, iRootID, serInfo1);
       iCountErrors++;
       Console.WriteLine("Err_034cd! exception not thrown");
       }
     catch(ArgumentNullException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_03472fd! Unexpected exception, " + ex.ToString());
       }
     try
       {
       iCountTestcases++;
       objmgr1.RegisterObject(tstfxp1, -5, serInfo1);
       iCountErrors++;
       Console.WriteLine("Err_037csd! exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_710ca! Unexpected exception, " + ex.ToString());
       }
     try
       {
       iCountTestcases++;
       objmgr1.RegisterObject(tstfxp1, 5, null);
       Console.WriteLine("Loc_048cs! exception not thrown");
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_079cd! Unexpected exception, " + ex.ToString());
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
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
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co3894RegsiterObject_OISer cbA = new Co3894RegsiterObject_OISer();
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
   if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
   }
}
[Serializable]
  internal class TestFixup:ISerializable
{
 internal Int32 iFixupValue;
 public TestFixup(){}
 internal TestFixup(SerializationInfo ser, StreamingContext sc1){
 iFixupValue = (Int32)ser.GetValue("iFixupValue", typeof(Int32));
 }
 public void GetObjectData(SerializationInfo ser, StreamingContext sc1){
 }
}
[Serializable]
  internal class TestFixup_1:ISerializable {
 internal Int32 iFixupValue;
 public TestFixup_1(){}
 internal TestFixup_1(SerializationInfo ser, StreamingContext sc1){
 iFixupValue = (Int32)ser.GetValue("DOES_NOT_HAVE_TO_BE_FIELD_NAME", typeof(Int32));
 }
 public void GetObjectData(SerializationInfo ser, StreamingContext sc1){
 }
}
