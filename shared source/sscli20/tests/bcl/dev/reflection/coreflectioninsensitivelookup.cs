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
using System.IO;
class TestClass {
 public static void Main(String[] args) {
 Environment.ExitCode = 1; 
 bool bResult = true;
 Console.WriteLine("ReflectionInsensitiveLookup: Test using reflection to do case-insensitive lookup with high chars.");
 TestClass tc = new TestClass();
 Assembly currAssembly = tc.GetType().Module.Assembly;
 String typeName = tc.GetType().FullName;
 Type tNormal = currAssembly.GetType(typeName);
 if (tNormal!=null) {
 Console.WriteLine("Found expected type.");
 } else {
 bResult = false;
 Console.WriteLine("Unable to load expected type.");
 }
 Type tInsensitive = currAssembly.GetType(typeName, false, true);
 if (tInsensitive!=null) {
 Console.WriteLine("Found expected insensitive type.");
 } else {	
 bResult = false;
 Console.WriteLine("Unable to load expected insensitive type.");
 }
 if (bResult) {
 Environment.ExitCode = 0;
 Console.WriteLine("Passed!");
 } else {
 Console.WriteLine("Failed!");
 }
 }
}
