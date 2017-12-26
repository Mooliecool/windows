'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBReflection
' Copyright (c) Microsoft Corporation.
' 
' Reflection provides objects (of type Type) that encapsulate assemblies, 
' modules and types. It allows us to
' 
' 1. Access attributes in your program's metadata.
' 2. Examine and instantiate types in an assembly.
' 3. Dynamically load and use types.
' 4. Emit new types at runtime.
' 
' This example demonstrates 2 and 3. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Reflection

#End Region


Module MainModule

    Sub Main()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Dynamically load the assembly.
        ' 

        Dim assembly As Assembly = assembly.LoadFrom("VBClassLibrary.dll")
        Debug.Assert(Not assembly Is Nothing)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Get a type and instantiate the type in the assembly.
        ' 

        Dim type As Type = assembly.GetType("VBClassLibrary.VBSimpleObject")
        Dim obj As Object = Activator.CreateInstance(type, New Object() {})


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Examine the type.
        ' 

        Console.WriteLine("Listing all the members of {0}", type)
        Console.WriteLine()

        Dim staticAll As BindingFlags = BindingFlags.Static Or _
            BindingFlags.NonPublic Or BindingFlags.Public
        Dim instanceAll As BindingFlags = BindingFlags.Instance Or _
            BindingFlags.NonPublic Or BindingFlags.Public

        ' Lists static fields first.
        Dim fi As FieldInfo() = type.GetFields(staticAll)
        Console.WriteLine("// Static Fields")
        PrintMembers(fi)

        ' Static properties.
        Dim pi As PropertyInfo() = type.GetProperties(staticAll)
        Console.WriteLine("// Static Properties")
        PrintMembers(pi)

        ' Static events.
        Dim ei As EventInfo() = type.GetEvents(staticAll)
        Console.WriteLine("// Static Events")
        PrintMembers(ei)

        ' Static methods.
        Dim mi As MethodInfo() = type.GetMethods(staticAll)
        Console.WriteLine("// Static Methods")
        PrintMembers(mi)

        ' Constructors.
        Dim ci As ConstructorInfo() = type.GetConstructors(instanceAll)
        Console.WriteLine("// Constructors")
        PrintMembers(ci)

        ' Instance fields.
        fi = type.GetFields(instanceAll)
        Console.WriteLine("// Instance Fields")
        PrintMembers(fi)

        ' Instance properites.
        pi = type.GetProperties(instanceAll)
        Console.WriteLine("// Instance Properties")
        PrintMembers(pi)

        ' Instance events.
        ei = type.GetEvents(instanceAll)
        Console.WriteLine("// Instance Events")
        PrintMembers(ei)

        ' Instance methods.
        mi = type.GetMethods(instanceAll)
        Console.WriteLine("// Instance Methods")
        PrintMembers(mi)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Use the type (Late Binding).
        ' 

        ' Call a public shared(static) method

        Console.WriteLine("Call the public method: GetStringLength")
        Dim method As MethodInfo = type.GetMethod("GetStringLength")
        ' Examine the method parameters
        Dim Params As ParameterInfo() = method.GetParameters()
        For Each Param As ParameterInfo In Params
            Console.WriteLine("Param={0}", Param.Name)
            Console.WriteLine(" Type={0}", Param.ParameterType)
            Console.WriteLine(" Position={0}", Param.Position)
        Next
        Dim result As Object = method.Invoke(Nothing, New Object() {"HelloWorld"})
        Console.WriteLine("Result={0}" & vbCrLf, result)

        ' Get the value of a private field

        Console.WriteLine("Get the value of the private field: fField")
        result = type.InvokeMember("fField", _
            BindingFlags.GetField Or _
            BindingFlags.Instance Or BindingFlags.NonPublic, _
            Nothing, obj, New Object() {})
        Console.WriteLine("Result={0}" & vbCrLf, result)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' There is no API to unload an assembly.
        ' http://blogs.msdn.com/suzcook/archive/2003/07/08/57211.aspx
        ' http://blogs.msdn.com/jasonz/archive/2004/05/31/145105.aspx
        ' 

    End Sub

    ''' <summary>
    ''' Print each member info
    ''' </summary>
    ''' <param name="members"></param>
    ''' <remarks></remarks>
    Sub PrintMembers(ByRef members As MemberInfo())
        For Each memberInfo As MemberInfo In members
            Console.WriteLine("{0}", memberInfo)
        Next
        Console.WriteLine()
    End Sub

End Module
