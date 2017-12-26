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
public class Co3893GetObject_lng
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "ObjectManager.GetObject(Int64)";
	public static String s_strTFName        = "Co3893GetObject_lng.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ObjectManager objmgr1 = null;
		StreamingContext sc1 = new StreamingContext(StreamingContextStates.All);
		ObjectIDGenerator objid1 = null;
		TestFixup tstfxp1;
		Int64 iRootID;
		Int64 iChildID;
		String strValue;
		bool fFirstTime;
		MemberInfo[] members = null;
		try {
			do
			{
				strLoc="Loc_174cds";
				tstfxp1 = new TestFixup();
				objid1 = new ObjectIDGenerator();
				iRootID = objid1.GetId(tstfxp1, out fFirstTime);
				strValue = "Hello Universe";
				iChildID = objid1.GetId(strValue, out fFirstTime);
				members = FormatterServices.GetSerializableMembers(tstfxp1.GetType());
				objmgr1 = new ObjectManager(null, sc1);
				iCountTestcases++;
				if(objmgr1.GetObject(iRootID)!=null)
				{
					iCountErrors++;
					Console.WriteLine("Err_753cd! Expected value not returned, " + objmgr1.GetObject(iRootID));
				}
				objmgr1.RecordFixup(iRootID, members[0], iChildID);
				iCountTestcases++;
				if(objmgr1.GetObject(iRootID)!=null)
				{
					iCountErrors++;
					Console.WriteLine("Err_048fd! Expected value not returned, " + objmgr1.GetObject(iRootID));
				}
				objmgr1.RegisterObject(tstfxp1, iRootID);
				iCountTestcases++;
				if(objmgr1.GetObject(iRootID)==null)
				{
					iCountErrors++;
					Console.WriteLine("Err_0943fd! Null returned");
				}
				iCountTestcases++;
				if(!((TestFixup)(objmgr1.GetObject(iRootID))).strFixupValue.Equals("Hello World"))
				{
					iCountErrors++;
					Console.WriteLine("Err_047fe! wrong value returned returned, " + ((TestFixup)(objmgr1.GetObject(iRootID))).strFixupValue);
				}
				objmgr1.RegisterObject(strValue, iChildID);
				iCountTestcases++;
				if(objmgr1.GetObject(iChildID)==null)
				{
					iCountErrors++;
					Console.WriteLine("Err_90853vdf! Null returned");
				}
				iCountTestcases++;
				if(!objmgr1.GetObject(iChildID).Equals("Hello Universe"))
				{
					iCountErrors++;
					Console.WriteLine("Err_1084cs! wrong value returned returned, " + objmgr1.GetObject(iChildID));
				}
				try{
					iCountTestcases++;
					objmgr1.GetObject(-5);
					iCountErrors++;
					Console.WriteLine("Err_04872d! Exception not thrown");
					}catch(ArgumentOutOfRangeException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1083xs! Wrong Exception thrown, " + ex);
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
		Co3893GetObject_lng cbA = new Co3893GetObject_lng();
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
	internal String strFixupValue;
	public TestFixup(){
		strFixupValue = "Hello World";
	}
}
