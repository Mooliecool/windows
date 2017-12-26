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
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
class ValueTypeFixup
{	
 public static bool NestedValueTypes() {
 ObjectManager manager = new ObjectManager(null, new StreamingContext(StreamingContextStates.All));
 Val1 val1 = new Val1(2);
 manager.RegisterObject(val1, 1);
 manager.RegisterObject(val1.val, 2, null, 1, Val1.valFieldInfo);
 manager.RegisterObject(val1.val.Foo, 3, null, 2, Val2.fooFieldInfo);
 manager.RegisterObject(val1.val.Bar, 4, null, 2, Val2.barFieldInfo);
 manager.RecordFixup(4, Val3.aFieldInfo, 5);
 int newValue = 42;
 A a = new A(newValue);
 manager.RegisterObject(a, 5);
 manager.DoFixups();
 Val1 valPost = (Val1)manager.GetObject(1);
 return (valPost.Int32Value==newValue);
 }
 public static bool ValueTypesInISerializable() {
 return true;
 }
 public static void Main(String[] args) {
 bool bResult = true;
 Environment.ExitCode = 1; 
 bResult&=NestedValueTypes();
 bResult&=ValueTypesInISerializable();
 if (bResult) {
 Environment.ExitCode=0;
 Console.WriteLine("Passed!");
 } else {
 Console.WriteLine("Failed!");
 }
 }	
}
public class ISerializableRoot : ISerializable {
 Val1 val1;
 Val2 val2;
 public ISerializableRoot(SerializationInfo info, StreamingContext context) {
 val1 = (Val1)info.GetValue("Val1", typeof(Val1));
 val2 = (Val2)info.GetValue("Val2", typeof(Val2));
 }
 public void GetObjectData(SerializationInfo info, StreamingContext context) {
 info.AddValue("Val1", (Object)val1);
 info.AddValue("Val2", (Object)val2);
 }
}
[Serializable]
  public struct Val1
{        
 internal Val2 val;
 public static FieldInfo valFieldInfo = typeof(Val1).GetField("val", BindingFlags.NonPublic | BindingFlags.Instance);
 public Val1(Int32 iValue) {
 val = new Val2(iValue);
 }
 public Int32 Int32Value {
 get{return val.Int32Value;}
 }
 public override String ToString() {
 return Int32Value.ToString();
 }
}
[Serializable]
  public struct Val2
{        
 internal Val3 Foo;
 internal Val3 Bar;
 public static FieldInfo fooFieldInfo = typeof(Val2).GetField("Foo", BindingFlags.NonPublic | BindingFlags.Instance);
 public static FieldInfo barFieldInfo = typeof(Val2).GetField("Bar", BindingFlags.NonPublic | BindingFlags.Instance);
 public Val2(Int32 iValue) {
 Foo = new Val3(iValue);
 Bar = new Val3(iValue * 2);
 }
 public Int32 Int32Value {
 get {
 return Bar.Int32Value;}
 }
}
public struct Val3 {
 int i;
 internal A b;
 internal A a;
 public static FieldInfo aFieldInfo = typeof(Val3).GetField("a", BindingFlags.NonPublic | BindingFlags.Instance);
 public static FieldInfo bFieldInfo = typeof(Val3).GetField("b", BindingFlags.NonPublic | BindingFlags.Instance);
 public Val3(int i) {
 this.i = i;
 this.b = new A(i*5);
 a = new A(i);
 }
 public int Int32Value {
 get { 
 return a.Int32Value; }
 }
}
[Serializable]
  public class A
{
 internal Int32 i;
 A(){}
 public A(Int32 iVal)
   {
   i=iVal;
   }
 public Int32 Int32Value
   {
   get {
   return i;}
   }
}
