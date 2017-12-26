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
/*========================================================================

  File:    TypeResolve.cs

  Summary: This file implements "type resolution" sample.  This sample
           demonstrates how hosts can participate in the type resolution
           process by supplying and event handler that returns an assembly
           containing the requested type.
			 
========================================================================*/


using System;
using System.Reflection;
using System.Reflection.Emit;
using System.Threading;
using System.Runtime.Remoting;


class App {
   static Assembly TypeResolveHandler(Object sender, ResolveEventArgs e) {
      Console.WriteLine("In TypeResolveHandler");

      AssemblyName assemblyName = new AssemblyName();
      assemblyName.Name = "DynamicAssem";

      // Create a new assembly with one module
      AssemblyBuilder newAssembly =
         Thread.GetDomain().DefineDynamicAssembly(assemblyName, AssemblyBuilderAccess.Run);
      ModuleBuilder newModule = newAssembly.DefineDynamicModule("DynamicModule");

      // Define a public class named "ANonExistentType" in the assembly.
      TypeBuilder myType = newModule.DefineType("ANonExistentType", TypeAttributes.Public);

      // Define a method on the type to call
      MethodBuilder simpleMethod = myType.DefineMethod("SimpleMethod", MethodAttributes.Public, null, null);
      ILGenerator il = simpleMethod.GetILGenerator();
      il.EmitWriteLine("Method called in ANonExistentType");
      il.Emit(OpCodes.Ret);

      // Bake the type
      myType.CreateType();

      return newAssembly;
   }

   static void Main() {
      // Hook up the event handler
      Thread.GetDomain().AssemblyResolve +=new ResolveEventHandler(App.TypeResolveHandler);

      // Find a type that should be in our assembly but isn't
      ObjectHandle oh = Activator.CreateInstance("DynamicAssem", "ANonExistentType"); 

      Type mt = oh.Unwrap().GetType();

      // Construct an instance of a type
      Object objInstance = Activator.CreateInstance(mt);

      // Find a method in this type and call it on this object
      MethodInfo mi = mt.GetMethod("SimpleMethod");
      mi.Invoke(objInstance, null);
   }
}
