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
public class Co3861RecordFixup
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "ObjectManager.RecordFixup(Int64, MemberInfo, Int64)";
	public static String s_strTFName        = "Co3861RecordFixup.cs";
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
		String strValue;
		bool fFirstTime;
		MemberInfo[] members = null;
		Test1 tst1;
		A a1;
		Int32 iOurMan;
		Boolean fChildFound;
		Test2 tst2;
		Test3 tst3;
		B b1;
		try {
			do
			{
				tstfxp1 = new TestFixup();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tstfxp1, out fFirstTime);
				strValue = "Hello World";
				iChildID = objid1.GetId(strValue, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(tstfxp1.GetType());
				objmgr1 = new ObjectManager(isur, sc1);
				objmgr1.RecordFixup(iRootID, members[0], iChildID);
				objmgr1.RegisterObject(tstfxp1, iRootID);
				objmgr1.RegisterObject(strValue, iChildID);
				iCountTestcases++;
				if(!tstfxp1.strFixupValue.Equals(strValue))
				{
					iCountErrors++;
					Console.WriteLine("Err_753cd! Expected value not returned, " + tstfxp1.strFixupValue + ", expected, " + strValue);
				}
				objmgr1.DoFixups();
				iCountTestcases++;
				if(!tstfxp1.strFixupValue.Equals(strValue))
				{
					iCountErrors++;
					Console.WriteLine("Err_90342ddvs! Expected value not returned, " + tstfxp1.strFixupValue + ", expected, " + strValue);
				}
				tst1 = new Test1();
				a1 = new A();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tst1, out fFirstTime);
				iChildID = objid1.GetId(a1, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(typeof(Test1));
				iOurMan = -1;
				fChildFound=false;
				for(int i=0; i<members.Length;i++){
					if(members[i].Name.Equals("a")){
						fChildFound=true;
						iOurMan=i;
						break;
					}
				}
				if(!fChildFound)
				throw new Exception("Loc_342ds! didn't find the member");
				objmgr1 = new ObjectManager(isur, sc1);
				objmgr1.RecordFixup(iRootID, members[iOurMan], iChildID);
				try{
					iCountTestcases++;
					objmgr1.RegisterObject(a1, iChildID);
					objmgr1.RegisterObject(tst1, iRootID);
					objmgr1.DoFixups();
					Console.WriteLine("Loc_0283! exception not thrown");
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_04523cd! Unexpected exception, " + ex.ToString());
				}
				tst1 = new Test1();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tst1, out fFirstTime);
				a1 = new A();
				iChildID = objid1.GetId(a1, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(typeof(Test1));
				iOurMan = -1;
				fChildFound=false;
				for(int i=0; i<members.Length;i++){
					if(members[i].Name.Equals("a")){
						fChildFound=true;
						iOurMan=i;
						break;
					}
				}
				if(!fChildFound)
				throw new Exception("Loc_342ds! didn't find the member");
				objmgr1 = new ObjectManager(isur, sc1);
				objmgr1.RecordFixup(iRootID, members[iOurMan], iChildID);
				try{
					iCountTestcases++;
					objmgr1.DoFixups();
					iCountErrors++;
					Console.WriteLine("Err_7453fd! exception not thrown");
					}catch(SerializationException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1204cd! Unexpected exception, " + ex.ToString());
				}
				tst1 = new Test1();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tst1, out fFirstTime);
				a1 = new A();
				iChildID = objid1.GetId(a1, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(typeof(Test1));
				iOurMan = -1;
				fChildFound=false;
				for(int i=0; i<members.Length;i++){
					if(members[i].Name.Equals("a")){
						fChildFound=true;
						iOurMan=i;
						break;
					}
				}
				if(!fChildFound)
				throw new Exception("Loc_342ds! didn't find the member");
				objmgr1 = new ObjectManager(isur, sc1);
				try{
					iCountTestcases++;
					objmgr1.RegisterObject(a1, iChildID);
					objmgr1.RegisterObject(tst1, iRootID);
					objmgr1.RecordFixup(iRootID, members[iOurMan], iChildID);
					objmgr1.DoFixups();
					Console.WriteLine("Loc_02843cd! exception not thrown");
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_0438fd! Unexpected exception, " + ex.ToString());
				}
				tst1 = new Test1();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tst1, out fFirstTime);
				a1 = new A();
				iChildID = objid1.GetId(a1, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(typeof(Test1));
				iOurMan = -1;
				fChildFound=false;
				for(int i=0; i<members.Length;i++){
					if(members[i].Name.Equals("a")){
						fChildFound=true;
						iOurMan=i;
						break;
					}
				}
				if(!fChildFound)
				throw new Exception("Loc_342fsd! didn't find the member");
				objmgr1 = new ObjectManager(isur, sc1);
				try{
					iCountTestcases++;
					objmgr1.RegisterObject(a1, iChildID);
					objmgr1.RegisterObject(tst1, iRootID);
					objmgr1.DoFixups();
					Console.WriteLine("Loc_94523fsd! exception not thrown");
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_0348dcs! Unexpected exception, " + ex.ToString());
				}
				tst2 = new Test2();
				tst3 = new Test3();
				b1 = new B();
				b1.StrValue = "Hello World";
				tst3.Set_B=b1;
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tst2, out fFirstTime);
				iChildID = objid1.GetId(tst3, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(typeof(Test2));
				iOurMan = -1;
				fChildFound=false;
				for(int i=0; i<members.Length;i++){
					if(members[i].Name.Equals("tst3")){
						fChildFound=true;
						iOurMan=i;
						break;
					}
				}
				if(!fChildFound)
				throw new Exception("Loc_024fd! didn't find the member");
				objmgr1 = new ObjectManager(isur, sc1);
				objmgr1.RecordFixup(iRootID, members[iOurMan], iChildID);
				objmgr1.RegisterObject(tst2, iRootID);
				objmgr1.RegisterObject(tst3, iChildID);
				iCountTestcases++;
				Console.WriteLine("Loc_4523vfd! we hve, " + tst2.StrValue);
				if(!tst2.StrValue.Equals("Hello World"))
				{
					iCountErrors++;
					Console.WriteLine("Err_038qm! Expected value not returned, " + tst2.StrValue);
				}
				tstfxp1 = new TestFixup();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tstfxp1, out fFirstTime);
				strValue = "Hello World";
				members = FormatterServices.GetSerializableMembers(tstfxp1.GetType());
				objmgr1 = new ObjectManager(null, sc1);
				objmgr1.RecordFixup(iRootID, members[0], iRootID);
				try{
					iCountTestcases++;
					objmgr1.RegisterObject(tstfxp1, iRootID);
					iCountErrors++;
					Console.WriteLine("Err_753cd! exception not thrown");
					}catch(ArgumentException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_04523cd! Unexpected exception, " + ex.ToString());
				}
				try
				{
					iCountTestcases++;
					objmgr1.RecordFixup(-1, members[0], iChildID);
					iCountErrors++;
					Console.WriteLine("Err_034cd! exception not thrown");
				}
				catch(ArgumentOutOfRangeException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_034cd! Unexpected exception, " + ex.ToString());
				}
				try
				{
					iCountTestcases++;
					objmgr1.RecordFixup(iRootID, members[0], -5);
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
					objmgr1.RecordFixup(iRootID, null, iChildID);
					iCountErrors++;
					Console.WriteLine("Err_048cs! exception not thrown");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_079cd! Unexpected exception, " + ex.ToString());
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.StackTrace);
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
		Co3861RecordFixup cbA = new Co3861RecordFixup();
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
[Serializable]
public class TestFixup
{
	public String strFixupValue;
}
[Serializable]
class Test1
{
	internal A a;
	internal Test1(){
		a = new A();
	}
}
[Serializable]
internal class A{}
[Serializable]
class Test2
{
	internal Test3 tst3;
	internal Test2(){
	}
	public String StrValue {
		get{return tst3.StrValue;}
	}
}
[Serializable]
class Test3
{
	internal B b;
	public String StrValue {
		get{return b.StrValue;}
	}
	public B Set_B{
		set{b=value;}
	}
}
[Serializable]
internal class B{
	internal String str1;
	public String StrValue {
		get{return str1;}
		set{str1 = value;}
	}
}
