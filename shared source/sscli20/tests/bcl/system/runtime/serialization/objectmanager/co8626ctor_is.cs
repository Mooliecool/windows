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
using System.Runtime.Serialization.Formatters.Binary;
public class Co8626ctor_IS
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "ObjectManager.ctor(ISurrogateSelector, StreamingContext)";
	public static String s_strTFName        = "Co8626ctor_IS.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		IFormatter formatter;
		SurrogateSelector selector;
		MySerializationSurrogate surrogate;
		StreamingContext context = new StreamingContext(StreamingContextStates.All);
		MemoryStream stream;
		Int32 iValue;
		A a;
		ObjectManager manager;
		ObjectIDGenerator generator;
		Int64 rootId;
		Int64 childId;
		Boolean firstTime;
		MemberInfo[] members;
		try {
			strLoc = "Loc_29457sdg";
			iCountTestcases++;
			selector = new SurrogateSelector();
			surrogate = new MySerializationSurrogate();
			selector.AddSurrogate(typeof(A), context, surrogate);
			formatter = new BinaryFormatter();
			formatter.SurrogateSelector = selector;
			stream = new MemoryStream();
			a = new A();
			a.I = 10;
			formatter.Serialize(stream, a);
			stream.Position = 0;
			A a1 = (A)formatter.Deserialize(stream);
			if(a1.I != 30){
				iCountErrors++;
				Console.WriteLine("Err_753ffd! Unexpected value returned, Value: <{0}>", a1.I);
			}
			strLoc = "Loc_8394tfsg";
			iCountTestcases++;
			generator = new ObjectIDGenerator();
			a = new A();
			iValue = 500;
			rootId = generator.GetId(a, out firstTime);
			childId = generator.GetId(iValue, out firstTime);
			selector = new SurrogateSelector();
			surrogate = new MySerializationSurrogate();
			selector.AddSurrogate(typeof(A), context, surrogate);
			manager = new ObjectManager(selector, context);
			members = FormatterServices.GetSerializableMembers(typeof(A));
			manager.RecordDelayedFixup(rootId, "Int32Twist", childId);
			try{
				SerializationInfo serInfo1 = new SerializationInfo(typeof(A), new FormatterConverter());
				manager.RegisterObject(a, rootId, serInfo1);
				manager.RegisterObject(iValue, childId);
				Console.WriteLine(a.I);
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine(ex);
			}
			strLoc = "Loc_87sgsfd";
			iCountTestcases++;
			generator = new ObjectIDGenerator();
			a = new A();
			iValue = 500;
			rootId = generator.GetId(a, out firstTime);
			childId = generator.GetId(iValue, out firstTime);
			manager = new ObjectManager(null, context);
			members = FormatterServices.GetSerializableMembers(typeof(A));
			manager.RecordFixup(rootId, members[0], childId);
			manager.RegisterObject(a, rootId);
			manager.RegisterObject(iValue, childId);
			if(a.I != 500){
				iCountErrors++;
				Console.WriteLine("Err_93745sg! Unexpected value returned, Value: <{0}>", a1.I);
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
		bool bResult = false;
		Co8626ctor_IS cbA = new Co8626ctor_IS();
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
class MySerializationSurrogate:ISerializationSurrogate
{
	public void GetObjectData(Object obj, SerializationInfo info, StreamingContext context){
		if(obj is A){
			info.AddValue("Int32Twist", 30);
		}else
			throw new ArgumentException("Wrong type called");
	}
	public Object SetObjectData(Object obj, SerializationInfo info, StreamingContext context, ISurrogateSelector selector){
		if(obj is A){
			((A)obj).I = info.GetInt32("Int32Twist");			
		}else
			throw new ArgumentException("Wrong type called");
		return obj;
	}
}
[Serializable]
class A
{
	int i;
	public A(){
		i = 5;
	}
	public Int32 I
	{
		get{return i;}
		set{i=value;}
	}
}
