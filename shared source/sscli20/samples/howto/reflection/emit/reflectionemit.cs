//------------------------------------------------------------------------------
// <copyright file="reflectionemit.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------


//=====================================================================
//  File:      EmitAssembly.cs
//
//  Summary:   Demonstrates how to use reflection emit.
//
//======================================================================



using System;
using System.Threading;
using System.Reflection;
using System.Reflection.Emit;

public class ReflectionEmitSample
{
  const string EMITTEDASSEMBLYNAME = "EmittedAssembly";
  const string EMITTEDASSEMBLYFILENAME = EMITTEDASSEMBLYNAME + ".dll";
  const string CALLINGASSEMBLYNAME = "CallingAssembly";
  
  /// <summary>
  /// Main entry point for sample.
  /// Gets input from user and then does various operations depending on user choice.
  /// </summary>
  /// <param name="args"></param>
  public static void Main(string[] args) 
  {
    string Response;
    AssemblyBuilder assembly;

    while (true)
    {
      Console.WriteLine("Enter one of the following options:\n");
      Console.Write(
        "   1\tCreate & test a dynamic type\n" +
        "   2\tCreate & save a type (use option 4 to test)\n" +
        "   3\tCreate 2 dynamic assemblies & test them\n" +
        "   4\tLoad saved assembly from option 2\n\n");
      Console.Write("   ");
      Response = Console.ReadLine();
      Console.WriteLine();

    switch (Response) 
    {
      case "1":
        Console.WriteLine("\nCreating helloWorldClass and invoking GetGreeting method.\n");
        // Create the "HelloWorld" class
        Type helloWorldClass = CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Run);         

        // Create an instance of the "HelloWorld" class.
        object helloWorld = Activator.CreateInstance(helloWorldClass, new object[] { "HelloWorld" });

        // Invoke the "GetGreeting" method of the "HelloWorld" class.
        object returnValue = helloWorldClass.InvokeMember("GetGreeting", 
                                                   BindingFlags.InvokeMethod, 
                                                   null, 
                                                   helloWorld, 
                                                   null);
        Console.WriteLine("HelloWorld.GetGreeting returned: {0}", returnValue);
        return;

      case "2":
        // Create assembly with HelloWorld type and save to disk.
        assembly = (AssemblyBuilder) CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Save).Assembly;
        assembly.Save(EMITTEDASSEMBLYFILENAME);
        Console.WriteLine("Assembly {0} saved to disk", EMITTEDASSEMBLYFILENAME);
        return;

      case "3":
        // Create callee and caller types.  Pass the callee to the caller and then
        // invoke the main method on the caller which creates an instance of the callee class
        // with a string constructor and invokes the GetGreeting method on the callee.
        Type calleeClass = CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Run);
        Type mainClass = CreateCaller(Thread.GetDomain(), AssemblyBuilderAccess.Run, calleeClass);

        Object o = Activator.CreateInstance(mainClass);
        mainClass.GetMethod("Main").Invoke(o, new Object[0]);
        return;

      case "4":
        if (!System.IO.File.Exists(EMITTEDASSEMBLYFILENAME))
        {
          Console.WriteLine("\nMust emit assembly using option 2 before using option 4\n");
          continue;
        } //if
        else
        {
          LoadAndRun();
          return;
        } //else

      default:
        Console.WriteLine("\n\n********** Please enter a valid option: 1 - 4 **********\n");
        continue;  // Get a valid response.
    } //switch
   } //while (true)
  } //Main()

  // Create the callee transient dynamic assembly.
  private static Type CreateCallee(AppDomain appDomain, AssemblyBuilderAccess access) 
  {
    // Create a simple name for the callee assembly.
    AssemblyName assemblyName = new AssemblyName();
    assemblyName.Name = EMITTEDASSEMBLYNAME;

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

    // Define a private string field named "Greeting" in the type.
    FieldBuilder greetingField = helloWorldClass.DefineField("Greeting", 
                                                              typeof(string), 
                                                              FieldAttributes.Private);

    // Create the constructor.  It requires a string which it stores in greetingField.
    Type[] constructorArgs = { typeof(string) };
    ConstructorBuilder constructor = helloWorldClass.DefineConstructor(
      MethodAttributes.Public, CallingConventions.Standard, constructorArgs);

    // Generate IL for the method. The constructor calls its superclass
    // constructor. The constructor stores its argument in the private field.
    ILGenerator constructorIL = constructor.GetILGenerator();
    constructorIL.Emit(OpCodes.Ldarg_0);
    ConstructorInfo superConstructor = typeof(Object).GetConstructor(new Type[0]);
    constructorIL.Emit(OpCodes.Call, superConstructor);
    constructorIL.Emit(OpCodes.Ldarg_0);
    constructorIL.Emit(OpCodes.Ldarg_1);
    // Store string argument in field.
    constructorIL.Emit(OpCodes.Stfld, greetingField);
    constructorIL.Emit(OpCodes.Ret);

    // Create the GetGreeting method.
    MethodBuilder getGreetingMethod = helloWorldClass.DefineMethod("GetGreeting", 
                                      MethodAttributes.Public, 
                                      typeof(string), 
                                      null);

    // Generate IL for GetGreeting.
    ILGenerator methodIL = getGreetingMethod.GetILGenerator();
    methodIL.Emit(OpCodes.Ldarg_0);
    methodIL.Emit(OpCodes.Ldfld, greetingField);
    methodIL.Emit(OpCodes.Ret);

    // Bake the class HelloWorld.
    return(helloWorldClass.CreateType());
  } //CreateCallee()

  // Create the caller transient dynamic assembly.
  private static Type CreateCaller(AppDomain appDomain, 
                                   AssemblyBuilderAccess access, 
                                   Type helloWorldClass) 
  {
    // Create a simple name for the caller assembly.
    AssemblyName assemblyName = new AssemblyName();
    assemblyName.Name = CALLINGASSEMBLYNAME;

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
    MethodBuilder mainMethod = mainClass.DefineMethod("Main", methodAttributes, null, null);

    // Generate IL for the method.
    ILGenerator mainIL = mainMethod.GetILGenerator();

    // Define the greeting string constant and emit it.  This gets consumed by the constructor.
    mainIL.Emit(OpCodes.Ldstr, "HelloWorld from dynamically created caller");

    // Use the provided "HelloWorld" class
    // Find the constructor for the "HelloWorld" class.
    Type[] constructorArgs = { typeof(string) };
    ConstructorInfo constructor = helloWorldClass.GetConstructor(constructorArgs);

    // Instantiate the "HelloWorld" class.
    mainIL.Emit(OpCodes.Newobj, constructor);

    // Find the "GetGreeting" method of the "HelloWorld" class.
    MethodInfo getGreetingMethod = helloWorldClass.GetMethod("GetGreeting");

    // Call the "GetGreeting" method to obtain the greeting.
    mainIL.Emit(OpCodes.Call, getGreetingMethod);

    // Write the greeting  to the console.
    MethodInfo writeLineMethod = typeof(Console).GetMethod("WriteLine", new Type[] { typeof(string) });
    mainIL.Emit(OpCodes.Call, writeLineMethod);
    //mainIL.EmitWriteLine(
    mainIL.Emit(OpCodes.Ret);

    // Bake the class. You can now create instances of this class.
    return(mainClass.CreateType());
  } //CreateCaller()

  private static void LoadAndRun()
  {
    Console.WriteLine("Loading assembly {0} using LoadFrom().", EMITTEDASSEMBLYFILENAME);

    Assembly a = Assembly.LoadFrom(EMITTEDASSEMBLYFILENAME);
    Type helloWorldClass = a.GetType("HelloWorld", true, true);
    object helloWorld = Activator.CreateInstance(helloWorldClass, new object[] { "HelloWorld" });

    object returnValue = helloWorldClass.InvokeMember("GetGreeting", 
      BindingFlags.InvokeMethod, 
      null, 
      helloWorld, 
      null);

    Console.WriteLine("\nHelloWorld.GetGreeting returned {0}", returnValue);
  } //LoadAndRun()

} //class ReflectionEmitSample

