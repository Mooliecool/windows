'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'
'=====================================================================
'  File:      EmitAssembly.vb
'
'  Summary:   Demonstrates how to use reflection emit.
'=====================================================================

Option Explicit On 
Option Strict On


Imports System
Imports System.Security.Permissions
Imports System.Globalization
Imports System.Threading
Imports System.Reflection
Imports System.Reflection.Emit
Imports Microsoft.VisualBasic

Namespace Microsoft.Samples

    Public NotInheritable Class App

        Private Sub New()

        End Sub

        Private Shared Sub Usage()
            Console.WriteLine(("Usage:" & ControlChars.CrLf & _
            "   1" & ControlChars.Tab & "Create & test a dynamic type" & ControlChars.CrLf & _
            "   2" & ControlChars.Tab & "Create & Save a type (use TestEmittedAssembly.exe to test)" & ControlChars.CrLf & _
            "   3" & ControlChars.Tab & "Create 2 dynamic assemblies & test them") & ControlChars.CrLf & _
            "   There is a separate project available with this project, which can be used to test the emitted assembly in step 2. Simply load that project, and reference the emitted assembly, to test it and ensure it works.")
        End Sub 'Usage

        <SecurityPermissionAttribute(SecurityAction.Demand, Unrestricted:=True)> _
        Public Shared Sub Main(ByVal args() As String)
            If args.Length = 0 Then
                Usage()
                Return
            ElseIf Not Char.IsDigit(args(0).ToCharArray()(0)) Then
                Usage()
                Return
            End If

            Dim [assembly] As AssemblyBuilder

            Select Case Int32.Parse(args(0), CultureInfo.CurrentCulture)
                Case 1
                    ' Create the "HelloWorld" class
                    Dim helloWorldClass As Type = CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Run)

                    ' Create an instance of the "HelloWorld" class.
                    Dim helloWorld As Object = Activator.CreateInstance(helloWorldClass, New Object() {"HelloWorld"})

                    ' Invoke the "GetGreeting" method of the "HelloWorld" class.
                    Dim obj As Object = helloWorldClass.InvokeMember("GetGreeting", BindingFlags.InvokeMethod, Nothing, helloWorld, Nothing, CultureInfo.CurrentCulture)
                    Console.WriteLine("HelloWorld.GetGreeting returned:  """ & obj.ToString() & """ ")

                Case 2
                    [assembly] = CType(CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Save).Assembly, AssemblyBuilder)
                    [assembly].Save("EmittedAssembly.dll")

                Case 3
                    Dim calleeClass As Type = CreateCallee(Thread.GetDomain(), AssemblyBuilderAccess.Run)
                    Dim mainClass As Type = CreateCaller(Thread.GetDomain(), AssemblyBuilderAccess.Run, calleeClass)

                    Dim o As Object = Activator.CreateInstance(mainClass)
                    mainClass.GetMethod("main").Invoke(o, New Object(-1) {})

                Case Else
                    Usage()
            End Select
        End Sub 'Main


        ' Create the callee transient dynamic assembly.
        Private Shared Function CreateCallee(ByVal appDomain As AppDomain, ByVal access As AssemblyBuilderAccess) As Type

            ' Create a simple name for the callee assembly.
            Dim assemblyName As New AssemblyName()
            assemblyName.Name = "EmittedAssembly"

            ' Create the callee dynamic assembly.
            Dim [assembly] As AssemblyBuilder = appDomain.DefineDynamicAssembly(assemblyName, access)

            ' Create a dynamic module named "CalleeModule" in the callee assembly.
            Dim [module] As ModuleBuilder
            If access = AssemblyBuilderAccess.Run Then
                [module] = [assembly].DefineDynamicModule("EmittedModule")
            Else
                [module] = [assembly].DefineDynamicModule("EmittedModule", "EmittedModule.mod")
            End If

            ' Define a public class named "HelloWorld" in the assembly.
            Dim helloWorldClass As TypeBuilder = [module].DefineType("HelloWorld", TypeAttributes.Public)

            ' Define a private String field named "Greeting" in the type.
            Dim greetingField As FieldBuilder = helloWorldClass.DefineField("Greeting", GetType(String), FieldAttributes.Private)

            ' Create the constructor.
            Dim constructorArgs As Type() = {GetType(String)}
            Dim constructor As ConstructorBuilder = helloWorldClass.DefineConstructor(MethodAttributes.Public, CallingConventions.Standard, constructorArgs)

            ' Generate IL for the method. The constructor calls its superclass
            ' constructor. The constructor stores its argument in the private field.
            Dim constructorIL As ILGenerator = constructor.GetILGenerator()
            constructorIL.Emit(OpCodes.Ldarg_0)
            Dim superConstructor As ConstructorInfo = GetType(Object).GetConstructor(Type.EmptyTypes)
            constructorIL.Emit(OpCodes.Call, superConstructor)
            constructorIL.Emit(OpCodes.Ldarg_0)
            constructorIL.Emit(OpCodes.Ldarg_1)
            constructorIL.Emit(OpCodes.Stfld, greetingField)
            constructorIL.Emit(OpCodes.Ret)

            ' Create the GetGreeting method.
            Dim getGreetingMethod As MethodBuilder = helloWorldClass.DefineMethod("GetGreeting", MethodAttributes.Public, GetType(String), Nothing)

            ' Generate IL for GetGreeting.
            Dim methodIL As ILGenerator = getGreetingMethod.GetILGenerator()
            methodIL.Emit(OpCodes.Ldarg_0)
            methodIL.Emit(OpCodes.Ldfld, greetingField)
            methodIL.Emit(OpCodes.Ret)

            ' Bake the class HelloWorld.
            Return helloWorldClass.CreateType()
        End Function 'CreateCallee


        ' Create the caller transient dynamic assembly.
        Private Shared Function CreateCaller(ByVal appDomain As AppDomain, ByVal access As AssemblyBuilderAccess, ByVal helloWorldClass As Type) As Type

            ' Create a simple name for the caller assembly.
            Dim assemblyName As New AssemblyName()
            assemblyName.Name = "CallingAssembly"

            ' Create the caller dynamic assembly.
            Dim [assembly] As AssemblyBuilder = appDomain.DefineDynamicAssembly(assemblyName, access)

            ' Create a dynamic module named "CallerModule" in the caller assembly.
            Dim [module] As ModuleBuilder
            If access = AssemblyBuilderAccess.Run Then
                [module] = [assembly].DefineDynamicModule("EmittedCallerModule")
            Else
                [module] = [assembly].DefineDynamicModule("EmittedCallerModule", "EmittedCallerModule.exe")
            End If

            ' Define a public class named MainClass.
            Dim mainClass As TypeBuilder = [module].DefineType("MainClass", TypeAttributes.Public)

            ' Create the method with name "main".
            Dim methodAttributes As MethodAttributes = methodAttributes.Static Or methodAttributes.Public
            Dim mainMethod As MethodBuilder = mainClass.DefineMethod("main", methodAttributes, Nothing, Nothing)

            ' Generate IL for the method.
            Dim mainIL As ILGenerator = mainMethod.GetILGenerator()

            ' Define the greeting string constant and emit it.
            mainIL.Emit(OpCodes.Ldstr, "HelloWorld (test 3)")

            ' Use the provided "HelloWorld" class
            ' Find the constructor for the "HelloWorld" class.
            Dim constructorArgs As Type() = {GetType(String)}
            Dim constructor As ConstructorInfo = helloWorldClass.GetConstructor(constructorArgs)

            ' Instantiate the "HelloWorld" class.
            mainIL.Emit(OpCodes.Newobj, constructor)

            ' Find the "GetGreeting" method of the "HelloWorld" class.
            Dim getGreetingMethod As MethodInfo = helloWorldClass.GetMethod("GetGreeting")

            ' Call the "GetGreeting" method to obtain the greeting.
            mainIL.Emit(OpCodes.Call, getGreetingMethod)

            ' Write the greeting  to the console.
            Dim writeLineMethod As MethodInfo = GetType(Console).GetMethod("WriteLine", New Type() {GetType(String)})
            mainIL.Emit(OpCodes.Call, writeLineMethod)
            mainIL.Emit(OpCodes.Ret)

            ' Bake the class. You can now create instances of this class if you needed to.
            Return mainClass.CreateType()
        End Function 'CreateCaller
    End Class 'App
End Namespace
