//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//
/*=====================================================================
  File:      EmitAssembly.cs

  Summary:   Demonstrates how to use reflection emit.

=====================================================================*/


using System;
using System.Security.Permissions;
using System.Threading;
using System.Reflection;
using System.Reflection.Emit;
using System.Globalization;

namespace Microsoft.Samples
{
	public sealed class App
	{
		private App()
		{
		}

		private static void Usage()
		{
			Console.WriteLine("Usage: EmitAssembly {1|2|3}\n" + 
                "   1\tCreate & test a dynamic type\n" + 
                "   2\tCreate & Save a type (use TestEmittedAssembly.exe to test)\n" + 
                "   3\tCreate 2 dynamic assemblies & test them\r\n\r\n" + 
                "   There is a separate project available with this project, which can be used to test the emitted assembly in step 2. Simply load that project, and reference the emitted assembly, to test it and ensure it works.");
		}

		public static void Main(String[] args)
		{
			if (args.Length == 0 || !Char.IsDigit(args[0].ToCharArray()[0]))
			{
				Usage();
				return;
			}

			new SecurityPermission(PermissionState.Unrestricted).Demand();
			AssemblyBuilder assembly;

			switch (Int32.Parse(args[0], CultureInfo.InvariantCulture))
			{
				case 1:
					// Create the "HelloWorld" class
					Type helloWorldClass = CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Run);

					// Create an instance of the "HelloWorld" class.
					Object helloWorld = Activator.CreateInstance(helloWorldClass, new Object[] { "HelloWorld" });

					// Invoke the "GetGreeting" method of the "HelloWorld" class.
					Object obj = helloWorldClass.InvokeMember("GetGreeting", BindingFlags.InvokeMethod, null, helloWorld, null, CultureInfo.InvariantCulture);

					Console.WriteLine("HelloWorld.GetGreeting returned: \"" + obj + "\"");
					break;

				case 2:
					assembly = (AssemblyBuilder)CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Save).Assembly;
					assembly.Save("EmittedAssembly.dll");
					break;

				case 3:
					Type calleeClass = CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Run);
					Type mainClass = CreateCaller(Thread.GetDomain(), AssemblyBuilderAccess.Run, calleeClass);
					Object o = Activator.CreateInstance(mainClass);

					mainClass.GetMethod("main").Invoke(o, new Object[0]);
					break;

				default:
					Usage();
					break;
			}
		}

		// Create the callee transient dynamic assembly.
		private static Type CreateCallee(AppDomain appDomain, AssemblyBuilderAccess access)
		{
			// Create a simple name for the callee assembly.
			AssemblyName assemblyName = new AssemblyName();

			assemblyName.Name = "EmittedAssembly";

			// Create the callee dynamic assembly.
			AssemblyBuilder assembly = appDomain.DefineDynamicAssembly(assemblyName, access);

			// Create a dynamic module named "CalleeModule" in the callee assembly.
			ModuleBuilder module;

			if (access == AssemblyBuilderAccess.Run)
			{
				module = assembly.DefineDynamicModule("EmittedModule");
			}
			else
			{
				module = assembly.DefineDynamicModule("EmittedModule", "EmittedModule.mod");
			}

			// Define a public class named "HelloWorld" in the assembly.
			TypeBuilder helloWorldClass = module.DefineType("HelloWorld", TypeAttributes.Public);

			// Define a private String field named "Greeting" in the type.
			FieldBuilder greetingField = helloWorldClass.DefineField("Greeting", typeof(String), FieldAttributes.Private);

			// Create the constructor.
			Type[] constructorArgs = { typeof(String) };
			ConstructorBuilder constructor = helloWorldClass.DefineConstructor(MethodAttributes.Public, CallingConventions.Standard, constructorArgs);

			// Generate IL for the method. The constructor calls its superclass
			// constructor. The constructor stores its argument in the private field.
			ILGenerator constructorIL = constructor.GetILGenerator();

			constructorIL.Emit(OpCodes.Ldarg_0);

			ConstructorInfo superConstructor = typeof(Object).GetConstructor(new Type[0]);

			constructorIL.Emit(OpCodes.Call, superConstructor);
			constructorIL.Emit(OpCodes.Ldarg_0);
			constructorIL.Emit(OpCodes.Ldarg_1);
			constructorIL.Emit(OpCodes.Stfld, greetingField);
			constructorIL.Emit(OpCodes.Ret);

			// Create the GetGreeting method.
			MethodBuilder getGreetingMethod = helloWorldClass.DefineMethod("GetGreeting", MethodAttributes.Public, typeof(String), null);

			// Generate IL for GetGreeting.
			ILGenerator methodIL = getGreetingMethod.GetILGenerator();

			methodIL.Emit(OpCodes.Ldarg_0);
			methodIL.Emit(OpCodes.Ldfld, greetingField);
			methodIL.Emit(OpCodes.Ret);

			// Bake the class HelloWorld.
			return (helloWorldClass.CreateType());
		}

		// Create the caller transient dynamic assembly.
		private static Type CreateCaller(AppDomain appDomain, AssemblyBuilderAccess access, Type helloWorldClass)
		{
			// Create a simple name for the caller assembly.
			AssemblyName assemblyName = new AssemblyName();

			assemblyName.Name = "CallingAssembly";

			// Create the caller dynamic assembly.
			AssemblyBuilder assembly = appDomain.DefineDynamicAssembly(assemblyName, access);

			// Create a dynamic module named "CallerModule" in the caller assembly.
			ModuleBuilder module;

			if (access == AssemblyBuilderAccess.Run)
			{
				module = assembly.DefineDynamicModule("EmittedCallerModule");
			}
			else
			{
				module = assembly.DefineDynamicModule("EmittedCallerModule", "EmittedCallerModule.exe");
			}

			// Define a public class named MainClass.
			TypeBuilder mainClass = module.DefineType("MainClass", TypeAttributes.Public);

			// Create the method with name "main".
			MethodAttributes methodAttributes = (MethodAttributes.Static | MethodAttributes.Public);
			MethodBuilder mainMethod = mainClass.DefineMethod("main", methodAttributes, null, null);

			// Generate IL for the method.
			ILGenerator mainIL = mainMethod.GetILGenerator();

			// Define the greeting string constant and emit it.
			mainIL.Emit(OpCodes.Ldstr, "HelloWorld (test 3)");

			// Use the provided "HelloWorld" class
			// Find the constructor for the "HelloWorld" class.
			Type[] constructorArgs = { typeof(String) };
			ConstructorInfo constructor = helloWorldClass.GetConstructor(constructorArgs);

			// Instantiate the "HelloWorld" class.
			mainIL.Emit(OpCodes.Newobj, constructor);

			// Find the "GetGreeting" method of the "HelloWorld" class.
			MethodInfo getGreetingMethod = helloWorldClass.GetMethod("GetGreeting");

			// Call the "GetGreeting" method to obtain the greeting.
			mainIL.Emit(OpCodes.Call, getGreetingMethod);

			// Write the greeting  to the console.
			MethodInfo writeLineMethod = typeof(Console).GetMethod("WriteLine", new Type[] { typeof(String) });

			mainIL.Emit(OpCodes.Call, writeLineMethod);
			mainIL.Emit(OpCodes.Ret);

			// Bake the class. You can now create instances of this class if you needed to.
			return (mainClass.CreateType());
		}
	}
}

