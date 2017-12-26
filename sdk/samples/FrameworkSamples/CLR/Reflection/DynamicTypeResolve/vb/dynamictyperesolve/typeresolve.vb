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
'==========================================================================
'  File:    TypeResolve.vb

'
'  Summary: This file implements "type resolution" sample.  This sample
'           demonstrates how hosts can participate in the type resolution
'           process by supplying and event handler that returns an assembly
'           containing the requested type.
'			 
'=====================================================================

Option Explicit On 
Option Strict On


Imports System
Imports System.Reflection
Imports System.Reflection.Emit
Imports System.Threading
Imports System.Runtime.Remoting




Class App
    
    Shared Function TypeResolveHandler(sender As Object, e As ResolveEventArgs) As [Assembly]
        Console.WriteLine("In TypeResolveHandler")
        
        Dim assemblyName As New AssemblyName()
        assemblyName.Name = "DynamicAssem"
        
        ' Create a new assembly with one module
        Dim newAssembly As AssemblyBuilder = Thread.GetDomain().DefineDynamicAssembly(assemblyName, AssemblyBuilderAccess.Run)
        Dim newModule As ModuleBuilder = newAssembly.DefineDynamicModule("DynamicModule")
        
        ' Define a public class named "ANonExistentType" in the assembly.
        Dim myType As TypeBuilder = newModule.DefineType("ANonExistentType", TypeAttributes.Public)
        
        ' Define a method on the type to call
        Dim simpleMethod As MethodBuilder = myType.DefineMethod("SimpleMethod", MethodAttributes.Public, Nothing, Nothing)
        Dim il As ILGenerator = simpleMethod.GetILGenerator()
        il.EmitWriteLine("Method called in ANonExistentType")
        il.Emit(OpCodes.Ret)
        
        ' Bake the type
        myType.CreateType()
        
        Return newAssembly
    End Function 'TypeResolveHandler
    
    
    Shared Sub Main()
        ' Hook up the event handler
        AddHandler Thread.GetDomain().AssemblyResolve, AddressOf App.TypeResolveHandler
        
        ' Find a type that should be in our assembly but isn't
        Dim oh As ObjectHandle = Activator.CreateInstance("DynamicAssem", "ANonExistentType")
        
        Dim mt As Type = oh.Unwrap().GetType()
        
        ' Construct an instance of a type
        Dim objInstance As [Object] = Activator.CreateInstance(mt)
        
        ' Find a method in this type and call it on this object
        Dim mi As MethodInfo = mt.GetMethod("SimpleMethod")
        mi.Invoke(objInstance, Nothing)
    End Sub 'Main
End Class 'App
