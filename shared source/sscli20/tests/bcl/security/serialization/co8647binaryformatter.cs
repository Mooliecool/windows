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
using System.Security;
using System.Security.Permissions; 
public class Co8647BinaryFormatter
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Security - BinaryFormatter";
	public static String s_strTFName        = "Co8647BinaryFormatter.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		Int32 iValue;
		A a1;
		V v1;
		S s1;
		V_I vi1;
		Surrogate_1 surrogate;
		SurrogateSelector selector;
		BinaryFormatter formatter;
		MemoryStream stream;
		StreamingContext context = new StreamingContext(StreamingContextStates.All);
		Object objRet;
		ReflectionPermission perm;
		try
		{
			strLoc = "Loc_9347sg";
			iCountTestcases++;
			iValue = 5;
			formatter = new BinaryFormatter();
			stream = new MemoryStream();
			try{
				Serialize(formatter, stream, iValue); 
				iCountErrors++;
				Console.WriteLine("Err_7349sg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_348tdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_2407sdg";
			iCountTestcases++;
			stream.Position = 0;
			formatter.Serialize(stream, iValue);
			stream.Position = 0;
			try{
				objRet = Deserialize(formatter, stream);
				if(iValue!=5)
				{
					iCountErrors++;
					Console.WriteLine("Err_24975tsdg! Wrong value returned");
				}
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_207tdsg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_3947tfdg";
			iCountTestcases++;
			a1 = new A(5);
			formatter = new BinaryFormatter();
			stream = new MemoryStream();
			try{
				Serialize(formatter, stream, a1); 
				iCountErrors++;
				Console.WriteLine("Err_39475sdg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_2047tsfgb! Wrong exception thrown, " + ex.GetType().Name);
			}
			stream.Position = 0;
			formatter.Serialize(stream, a1);
			stream.Position = 0;
			try{
				objRet = Deserialize(formatter, stream);
				iCountErrors++;
				Console.WriteLine("Err_9347tsfg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_0347tsfg! Wrong exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_34975sg";
			iCountTestcases++;
			v1 = new V(5);
			formatter = new BinaryFormatter();
			stream = new MemoryStream();
			try{
				Serialize(formatter, stream, v1); 
				iCountErrors++;
				Console.WriteLine("Err_98324sdg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_3047tfxg! Wrong exception thrown, " + ex.GetType().Name);
			}
			stream.Position = 0;
			formatter.Serialize(stream, v1);
			stream.Position = 0;
			try{
				objRet = Deserialize(formatter, stream);
				iCountErrors++;
				Console.WriteLine("Err_98324sdg! No exception thrown, ");
			}
			catch(SecurityException)
			{
			}
			catch(Exception ex)
			{
				iCountErrors++;
				Console.WriteLine("Err_29437tsg! Unexpected exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_34975sg";
			iCountTestcases++;
			s1 = new S(5);
			formatter = new BinaryFormatter();
			stream = new MemoryStream();
			try{
				Serialize(formatter, stream, s1); 
				iCountErrors++;
				Console.WriteLine("Err_3946tsdg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_9347tsdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			stream.Position = 0;
			formatter.Serialize(stream, s1);
			stream.Position = 0;
			try{
				objRet = Deserialize(formatter, stream);
				iCountErrors++;
				Console.WriteLine("Err_3946tsdg! No exception thrown, ");
			}
			catch(SecurityException)
			{
			}
			catch(Exception ex)
			{
				iCountErrors++;
				Console.WriteLine("Err_3947tsg! Exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_34975sg";
			iCountTestcases++;
			vi1 = new V_I(6);
			formatter = new BinaryFormatter();
			stream = new MemoryStream();
			try{
				Serialize(formatter, stream, vi1); 
				iCountErrors++;
				Console.WriteLine("Err_2075sdg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_294375sdg! Wrong exception thrown, " + ex.GetType().Name);
			}
			stream.Position = 0;
			formatter.Serialize(stream, vi1);
			stream.Position = 0;			
			try{
				objRet = Deserialize(formatter, stream);
				iCountErrors++;
				Console.WriteLine("Err_3946tsdg! No exception thrown, ");
			}
			catch(SecurityException)
			{
			}
			catch(Exception ex)
			{
				iCountErrors++;
				Console.WriteLine("Err_93475sdg! Exception thrown, " + ex.GetType().Name);
			}			
			strLoc = "Loc_34975sg";
			iCountTestcases++;
			selector = new SurrogateSelector();
			surrogate = new Surrogate_1();
			selector.AddSurrogate(typeof(A), context, surrogate);
			selector.AddSurrogate(typeof(V), context, surrogate);
			formatter = new BinaryFormatter();
			formatter.SurrogateSelector = selector;
			stream = new MemoryStream();
			a1 = new A(5);
			try{
				Serialize(formatter, stream, a1); 
				iCountErrors++;
				Console.WriteLine("Err_93475sdg! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_3947tsg! Wrong exception thrown, " + ex.GetType().Name);
			}
			stream.Position = 0;
			formatter.Serialize(stream, a1);
			stream.Position = 0;
			try{
				objRet = Deserialize(formatter, stream);
				iCountErrors++;
				Console.WriteLine("Err_3946tsdg! No exception thrown, ");
			}
			catch(SecurityException)
			{
			}
			catch(Exception ex)
			{
				iCountErrors++;
				Console.WriteLine("Err_39047tsg! Exception thrown, " + ex.GetType().Name);
			}
			strLoc = "Loc_3048sdg";
			iCountTestcases++;
			v1 = new V(7);
			formatter = new BinaryFormatter();
			formatter.SurrogateSelector = selector;
			stream = new MemoryStream();
			try{
				Serialize(formatter, stream, v1); 
				iCountErrors++;
				Console.WriteLine("Err_9347tsgf! No exception thrown, ");
			}catch(SecurityException){
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_2947tsgd! Wrong exception thrown, " + ex.GetType().Name);
			}
			stream.Position = 0;
			formatter.Serialize(stream, v1);
			stream.Position = 0;
			try{
				objRet = Deserialize(formatter, stream);
				iCountErrors++;
				Console.WriteLine("Err_3946tsdg! No exception thrown, ");
			}
			catch(SecurityException)
			{
			}
			catch(Exception ex)
			{
				iCountErrors++;
				Console.WriteLine("Err_93427twg! Exception thrown, " + ex.GetType().Name);
			}			
			perm = new ReflectionPermission(PermissionState.Unrestricted);
			perm.Deny();
			strLoc = "Loc_83245tsg";
			iCountTestcases++;
			a1 = new A(5);
			formatter = new BinaryFormatter();
			stream = new MemoryStream();
			try{
				formatter.Serialize(stream, a1);
				stream.Position = 0;
				objRet = formatter.Deserialize(stream);				
				if(((A)objRet).I!=5){
					iCountErrors++;
					Console.WriteLine("Err_3947tsdg! wrong value returned, " + (Int32)objRet);
				}
			}catch(Exception ex){
				iCountErrors++;
				Console.WriteLine("Err_27d! Exception thrown, " + ex.GetType().Name);
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
	private void Serialize(BinaryFormatter formatter, MemoryStream stream, Object value)
	{
		SecurityPermission perm = new SecurityPermission(SecurityPermissionFlag.SerializationFormatter);
		perm.Deny();
		formatter.Serialize(stream, value);
	}
	private Object Deserialize(BinaryFormatter formatter, MemoryStream stream)
	{
		SecurityPermission perm = new SecurityPermission(SecurityPermissionFlag.SerializationFormatter);
		perm.Deny();
		return formatter.Deserialize(stream);
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co8647BinaryFormatter cbA = new Co8647BinaryFormatter();
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
class A
{
	int i;
	public A(int i)
	{
		this.i=i;
	}
	public Int32 I{
		set {i=value;}
		get {return i;}
	}
}
[Serializable]
struct V
{
	int i;
	public V(int i)
	{
		this.i=i;
	}
	public Int32 I{
		set {i=value;}
		get {return i;}
	}
}
[Serializable]
class S:ISerializable
{
	int i;
	public S(int i){
		this.i=i;
	}
	private S(SerializationInfo info, StreamingContext context){
		i = info.GetInt32("IntValue");
	}
	public void GetObjectData(SerializationInfo info, StreamingContext context){
		info.AddValue("IntValue", i*50, typeof(int));
	}
	public Int32 I{
		set {i=value;}
		get {return i;}
	}	
}
[Serializable]
struct V_I:ISerializable
{
	int i;
	public V_I(int i){
		this.i=i;
	}
	private V_I(SerializationInfo info, StreamingContext context){
		i = info.GetInt32("IntValue");
	}
	public void GetObjectData(SerializationInfo info, StreamingContext context){
		info.AddValue("IntValue", i*50, typeof(int));
	}
	public Int32 I{
		set {i=value;}
		get {return i;}
	}	
}
class Surrogate_1:ISerializationSurrogate
{
	public void GetObjectData(Object obj, SerializationInfo info, StreamingContext context){
		if(obj.GetType().Equals(typeof(A))){
			info.AddValue("IntValueForA", ((A)obj).I*100, typeof(int));
		}else if(obj.GetType().Equals(typeof(V))){
			info.AddValue("IntValueForV", ((V)obj).I*500, typeof(int));
		}else
			throw new Exception("Err_874935sg! Unknown type");
	}
	public Object SetObjectData(Object obj, SerializationInfo info, StreamingContext context, ISurrogateSelector selector){
		Object objRet = obj;
		if(obj.GetType().Equals(typeof(A))){
			((A)obj).I = info.GetInt32("IntValueForA");
		}else if(obj.GetType().Equals(typeof(V))){
			FieldInfo[] members = obj.GetType().GetFields(BindingFlags.NonPublic | BindingFlags.Instance);
			members[0].SetValue(obj, info.GetInt32("IntValueForV"));
		}else
			throw new Exception("Err_34tsdg! Unknown type");		
		return objRet;
	}
}
