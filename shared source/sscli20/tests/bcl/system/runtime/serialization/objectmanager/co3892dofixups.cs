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
public class Co3892DoFixups
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "ObjectManager.DoFixups()";
	public static String s_strTFName        = "Co3892DoFixups.cs";
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
		TestReference tst1;
		ThisImplementsIObjectReference tst2;
		try {
			do
			{
				strLoc="Loc_174cds";
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
				strLoc="Loc_5720xs";
				tst1 = new TestReference();
				tst2 = new ThisImplementsIObjectReference();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tst1, out fFirstTime);
				iChildID = objid1.GetId(tst2, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(typeof(TestReference));
				Console.WriteLine("<<" + members[0].Name + ">>");
				objmgr1 = new ObjectManager(isur, sc1);
				objmgr1.RecordFixup(iRootID, members[0], iChildID);
				objmgr1.RegisterObject(tst1, iRootID);
				objmgr1.RegisterObject(tst2, iChildID);
				try{
					iCountTestcases++;
					objmgr1.DoFixups();
					iCountErrors++;
					Console.WriteLine("Err_753fvdf! exception not thrown!!");
				}catch(SerializationException){
				}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_04523cd! Unexpected exception, " + ex.ToString());
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
		Co3892DoFixups cbA = new Co3892DoFixups();
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
public class TestFixup {
	public String strFixupValue;
}
[Serializable]
internal class TestReference {
	internal ThisImplementsIObjectReference fld1;
}
[Serializable]
internal class ThisImplementsIObjectReference:IObjectReference {
	internal Int32 iValue;
	public ThisImplementsIObjectReference(){
		iValue = 30;
	}
	public Object GetRealObject(StreamingContext sc1){
		return new ThisImplementsIObjectReference();
	}
}
