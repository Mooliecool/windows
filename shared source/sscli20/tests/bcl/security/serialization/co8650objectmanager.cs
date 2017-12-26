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
using System.Reflection;
using System.Runtime.Serialization;
using System.Security;
using System.Security.Permissions; 
public class Co8650ObjectManager
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Security - Object Manager";
	public static String s_strTFName        = "Co8650ObjectManager.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		ObjectManager objmgr1 = null;
		ISurrogateSelector isur = null;
		StreamingContext sc1 = new StreamingContext(StreamingContextStates.All);
		ObjectIDGenerator objid1 = null;
		TestFixup tstfxp1;
		Int64 iRootID;
		Int64 iChildID;
		bool fFirstTime;
		MemberInfo[] members = null;
		try
		{
			strLoc = "Loc_9347sg";
			iCountTestcases++;
			try{
				ConstructOM(); 
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_3946tsg";
			iCountTestcases++;
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
	private void ConstructOM()
	{
		SecurityPermission perm = new SecurityPermission(SecurityPermissionFlag.SerializationFormatter);
		perm.Deny();
		ObjectManager manager = new ObjectManager(null, new StreamingContext(StreamingContextStates.All));
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co8650ObjectManager cbA = new Co8650ObjectManager();
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
[Serializable]
public class TestFixup
{
	public String strFixupValue;
}
