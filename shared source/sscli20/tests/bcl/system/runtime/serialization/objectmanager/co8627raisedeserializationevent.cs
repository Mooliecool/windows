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
using System.Resources;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
public class Co8627RaiseDeserializationEvent
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "ObjectManager.RaiseDeserializationEvent()";
	public static String s_strTFName        = "Co8627RaiseDeserializationEvent.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public Boolean runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ObjectManager manager;
		ISurrogateSelector selector = null;
		StreamingContext context = new StreamingContext(StreamingContextStates.All);
		ClsType1 cls1;
		ClsType2 cls2;
		Int32 iValue;
		MemberInfo[] classMembers1;
		MemberInfo[] classMembers2;
		try {
			strLoc = "Loc_734rgt";			
			iCountTestcases++;
			cls1 = new ClsType1();
			cls2 = new ClsType2();
			iValue = 10;
			classMembers1 = FormatterServices.GetSerializableMembers(typeof(ClsType1), context);
			classMembers2 = FormatterServices.GetSerializableMembers(typeof(ClsType2), context);
			manager = new ObjectManager(selector, context);
			manager.RecordFixup(2, classMembers2[0], 3);
			manager.RecordFixup(1, classMembers1[0], 2);
			manager.RegisterObject(cls1, 1);
			manager.RegisterObject(cls2, 2);			
			manager.RegisterObject(iValue, 3);
			manager.DoFixups();
			if(cls1.ClsTp2.I != 10){
				iCountErrors++;
				Console.WriteLine("Err_374sdg! Change of behavioue, unexpected value returned, " + cls1.ClsTp2.I);
			}
			manager.RaiseDeserializationEvent();
			if(cls1.ClsTp2.I != 15){
				iCountErrors++;
				Console.WriteLine("Err_987345sg! Change of behavioue, unexpected value returned, " + cls1.ClsTp2.I);
			}
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
		Boolean bResult = false;
		Co8627RaiseDeserializationEvent cbA = new Co8627RaiseDeserializationEvent();
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
class ClsType1:IDeserializationCallback
{
	private ClsType2 clsTp2;
	public ClsType1()
	{
		clsTp2 = new ClsType2();
		clsTp2.I = 5;
	}
	public void OnDeserialization(object sender){
		clsTp2.I = 15;
	}
	public  ClsType2 ClsTp2{
		get{return clsTp2;}
		set{clsTp2=value;}
	}
}
[Serializable]
class ClsType2
{
	private Int32 i;
	public Int32 I{
		get{return i;}
		set{i=value;}
	}
}
