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
public class Co8628RegisterObject_OISIM
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "ObjectManager.RegisterObject(Object, Int64, SerializationInfo, Int64, MemberInfo)";
	public static String s_strTFName        = "Co8628RegisterObject_OISIM.cs";
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
		ClsType cls1;
		ValType val1;
		Int32 iValue;
		MemberInfo[] valueMembers;
		MemberInfo[] classMembers;
		MemberInfo[] classMembers1;
		ClsType1 cls2;
		ClsType2 cls3;
		ClsType3 cls4;
		ValType1 val2;
		SerializationInfo info;
		try {
			strLoc = "Loc_97356tsg";			
			iCountTestcases++;
			cls1 = new ClsType();
			val1 = new ValType();
			iValue = 10;
			classMembers = FormatterServices.GetSerializableMembers(typeof(ClsType), context);
			valueMembers = FormatterServices.GetSerializableMembers(typeof(ValType), context);
			manager = new ObjectManager(selector, context);
			manager.RecordFixup(2, valueMembers[0], 3);
			manager.RecordFixup(1, classMembers[0], 2);
			manager.RegisterObject(cls1, 1);
			manager.RegisterObject(val1, 2);			
			manager.RegisterObject(iValue, 3);
			if(cls1.ValueType.I != 0){
				iCountErrors++;
				Console.WriteLine("Err_853rwtg! Change of behavioue, unexpected value returned, " + cls1.ValueType.I);
			}
			strLoc = "Loc_7934sgd";			
			iCountTestcases++;
			cls1 = new ClsType();
			val1 = new ValType();
			iValue = 10;
			classMembers = FormatterServices.GetSerializableMembers(typeof(ClsType), context);
			valueMembers = FormatterServices.GetSerializableMembers(typeof(ValType), context);
			manager = new ObjectManager(selector, context);
			manager.RecordFixup(2, valueMembers[0], 3);
			manager.RecordFixup(1, classMembers[0], 2);
			manager.RegisterObject(cls1, 1);
			manager.RegisterObject(val1, 2, null, 1, classMembers[0]);
			manager.RegisterObject(iValue, 3);
			if(cls1.ValueType.I != 10){
				iCountErrors++;
				Console.WriteLine("Err_23450sdg! Unexpected value returned, " + cls1.ValueType.I);
			}
			strLoc = "Loc_734rgt";			
			iCountTestcases++;
			cls2 = new ClsType1();
			cls3 = new ClsType2();
			iValue = 10;
			classMembers = FormatterServices.GetSerializableMembers(typeof(ClsType1), context);
			classMembers1 = FormatterServices.GetSerializableMembers(typeof(ClsType2), context);
			manager = new ObjectManager(selector, context);
			manager.RecordFixup(2, classMembers1[0], 3);
			manager.RecordFixup(1, classMembers[0], 2);
			manager.RegisterObject(cls2, 1);
			manager.RegisterObject(cls3, 2);			
			manager.RegisterObject(iValue, 3);
			if(cls2.Cls2.I != 10){
				iCountErrors++;
				Console.WriteLine("Err_987345sg! Change of behavioue, unexpected value returned, " + cls2.Cls2.I);
			}
			strLoc = "Loc_9743sg";			
			iCountTestcases++;
			cls4 = new ClsType3();
			val2 = new ValType1();
			iValue = 10;
			classMembers = FormatterServices.GetSerializableMembers(typeof(ClsType3), context);
			manager = new ObjectManager(selector, context);
			manager.RecordDelayedFixup(2, "SerializationTest", 3);
			manager.RecordFixup(1, classMembers[0], 2);
			manager.RegisterObject(cls4, 1);
			info = new SerializationInfo(typeof(ValType1), new FormatterConverter());
			manager.RegisterObject(val2, 2, info, 1, classMembers[0]);
			manager.RegisterObject(iValue, 3);
			try{
				manager.DoFixups();
			}catch(ArgumentException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_3497tsdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_8932745rsdf";			
			iCountTestcases++;
			cls1 = new ClsType();
			val1 = new ValType();
			iValue = 10;
			classMembers = FormatterServices.GetSerializableMembers(typeof(ClsType), context);
			valueMembers = FormatterServices.GetSerializableMembers(typeof(ValType), context);
			manager = new ObjectManager(selector, context);
			manager.RecordFixup(2, valueMembers[0], 3);
			manager.RecordFixup(1, classMembers[0], 2);
			manager.RegisterObject(cls1, 1);
			manager.RegisterObject(val1, 5, null, 1, classMembers[0]);
			manager.RegisterObject(iValue, 3);
			try{
				manager.DoFixups();
				iCountErrors++;
				Console.WriteLine("Err_47sg! Exception now thrown");
			}catch(SerializationException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_38945gd! Unexpected exception returned, " + ex.GetType().Name);
			}			
			strLoc = "Loc_8932745rsdf";			
			iCountTestcases++;
			cls1 = new ClsType();
			val1 = new ValType();
			iValue = 10;
			classMembers = FormatterServices.GetSerializableMembers(typeof(ClsType), context);
			valueMembers = FormatterServices.GetSerializableMembers(typeof(ValType), context);
			manager = new ObjectManager(selector, context);
			manager.RecordFixup(2, valueMembers[0], 3);
			manager.RecordFixup(1, classMembers[0], 2);
			manager.RegisterObject(cls1, 1);
			try{
				manager.RegisterObject(val1, 2, null, 2, classMembers[0]);
				iCountErrors++;
				Console.WriteLine("Err_8743gs! Exception now thrown");
			}catch(SerializationException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_832745wg! Unexpected exception returned, " + ex.GetType().Name);
			}			
			strLoc = "Loc_8734sdg";			
			iCountTestcases++;
			cls1 = new ClsType();
			val1 = new ValType();
			iValue = 10;
			classMembers = FormatterServices.GetSerializableMembers(typeof(ClsType1), context);
			valueMembers = FormatterServices.GetSerializableMembers(typeof(ValType), context);
			manager = new ObjectManager(selector, context);
			manager.RecordFixup(2, valueMembers[0], 3);
			manager.RecordFixup(1, classMembers[0], 2);
			manager.RegisterObject(cls1, 1);
			try{
				manager.RegisterObject(val1, 2, null, 1, classMembers[0]);
				iCountErrors++;
				Console.WriteLine("Err_47sg! Exception now thrown");
			}catch(ArgumentException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_38945gd! Unexpected exception returned, " + ex.GetType().Name);
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
		Co8628RegisterObject_OISIM cbA = new Co8628RegisterObject_OISIM();
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
class ClsType
{
	private ValType valueType;
	public ClsType()
	{
		valueType = new ValType();
		valueType.I = 5;
	}
	public ValType ValueType{
		get{return valueType;}
		set{valueType=value;}
	}
}
[Serializable]
struct ValType
{
	private Int32 i;
	public Int32 I{
		get{return i;}
		set{i=value;}
	}
}
[Serializable]
class ClsType1
{
	private ClsType2 cls2;
	public ClsType1()
	{
		cls2 = new ClsType2();
		cls2.I = 5;
	}
	public ClsType2 Cls2{
		get{return cls2;}
		set{cls2=value;}
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
[Serializable]
class ClsType3
{
	private ValType1 valueType;
	public ClsType3()
	{
		valueType = new ValType1();
		valueType.I = 5;
	}
	public ValType1 ValueType{
		get{return valueType;}
		set{valueType=value;}
	}
}
[Serializable]
struct ValType1:ISerializable
{
	private Int32 i;
	internal ValType1(SerializationInfo ser, StreamingContext sc1){
		i = (Int32)ser.GetValue("SerializationTest", typeof(Int32));
	}
	public void GetObjectData(SerializationInfo ser, StreamingContext sc1){
		Console.WriteLine("Do we ever call this??");
		ser.AddValue("SerializationTest", 30);
	}	
	public Int32 I{
		get{return i;}
		set{i=value;}
	}
}
