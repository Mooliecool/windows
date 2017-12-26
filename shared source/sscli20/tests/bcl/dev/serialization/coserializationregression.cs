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
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
public class SerializationRegression {
 private static bool Bug() {
 ObjectManager objmgr1;
 StreamingContext sc1;
 ObjectIDGenerator objid1;
 TestFixup tstfxp1;
 Int64 iRootID;
 Int64 iChildID;
 String strValue;
 MemberInfo[] members;
 Boolean fFirstTime;
 sc1 = new StreamingContext(StreamingContextStates.All);
 tstfxp1 = new TestFixup();
 strValue = "Hello World";
 objid1 = new ObjectIDGenerator();
 iRootID = objid1.GetId(tstfxp1, out fFirstTime);
 iChildID = objid1.GetId(strValue, out fFirstTime);
 members = FormatterServices.GetSerializableMembers(tstfxp1.GetType()); 
 objmgr1 = new ObjectManager(null, sc1);
 objmgr1.RecordFixup(iRootID, members[0], iChildID);
 try {
 objmgr1.RegisterObject(strValue, iChildID);	
 return true;
 } catch(Exception ex){
 Console.WriteLine("Bug:Exception thrown, " + ex);
 return false;
 }
 }
 public static void Main(String[] args) {
 bool bResult=true;
 Environment.ExitCode=1; 
 bResult&=Bug();
 if (bResult) {
 Console.WriteLine("Passed!");
 Environment.ExitCode = 0;
 } else {
 Console.WriteLine("Failed!");
 }
 }
}
[Serializable]
  internal class TestFixup
{
 internal String strFixupValue = null;
}
