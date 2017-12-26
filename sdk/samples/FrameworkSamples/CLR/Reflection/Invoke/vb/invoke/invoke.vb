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
'  File:      Invoke.vb
'
'  Summary:   Demonstrates how to use reflection invoke.
'
'  Warning:	  This sample shows how to invoke ANY method via reflection on ANY location.
'			  This can potentially be a security issue.
'			  When implementing this pattern in your code be sure to take appropriate security measures,
'				such as hosting inside an AppDomain with locked down security permission.
'
'=====================================================================

Option Explicit On 
Option Strict On


Imports System
Imports System.Globalization
Imports System.IO
Imports System.Text
Imports System.Threading
Imports System.Reflection
Imports System.Reflection.Emit
Imports Microsoft.VisualBasic

Namespace Microsoft.Samples

    Public NotInheritable Class App

        Private Sub New()

        End Sub


        Public Shared Sub Main(ByVal args() As String)

            If args.Length < 3 Then
                Usage()
                Return
            End If

            Dim assemblyObj As System.Reflection.Assembly
            Dim typeObj As Type

            Try
                ' Load the requested assembly andget the requested type
                assemblyObj = System.Reflection.Assembly.LoadFrom(args(0))
                typeObj = assemblyObj.GetType(args(1), True, True)
            Catch e As FileNotFoundException
                Console.WriteLine("Cannot load assembly: {0}", args(0))
                Return
            Catch e As TypeLoadException
                Console.WriteLine("Cannot load type: {0} from assembly: {1}", args(1), args(0))
                Return
            End Try

            ' Get the methods from the type
            Dim methods As MethodInfo() = typeObj.GetMethods()

            If methods Is Nothing Then
                Console.WriteLine("No Matching Types Found")
                Return
            End If

            ' Create a new array that holds only the args for the call
            Dim newArgs(args.Length - 4) As String
            If newArgs.Length <> 0 Then
                Array.Copy(args, 3, newArgs, 0, newArgs.Length)
            End If

            ' Try each of the type's methods for a match
            Dim failureExcuses As StringBuilder = New StringBuilder()
            Dim m As MethodInfo
            For Each m In methods
                Dim obj As Object = Nothing
                Try
                    obj = AttemptMethod(typeObj, m, args(2), newArgs)
                Catch E As CustomException
                    failureExcuses.Append(E.Message & vbCrLf)
                End Try
                If Not obj Is Nothing Then
                    Console.WriteLine(obj)
                    Return
                End If
            Next m

            Console.WriteLine("Suitable method not found!")
            Console.WriteLine("Here are the reasons:" & vbCrLf & failureExcuses.ToString())
        End Sub 'Main


        Private Shared Function AttemptMethod(ByVal typeObj As Type, ByVal method As MethodInfo, ByVal name As String, ByVal args() As String) As Object

            ' Name does not match?
            If String.Compare(method.Name, name, True, CultureInfo.InvariantCulture) <> 0 Then
                Throw New CustomException(method.DeclaringType.ToString() & "." & method.Name & ": Method Name Doesn't Match!")
            End If

            ' Wrong number of parameters?
            Dim param As ParameterInfo() = method.GetParameters()
            If param.Length <> args.Length Then
                Throw New CustomException(method.DeclaringType.ToString() & "." & method.Name & ": Method Signatures Don't Match!")
            End If

            ' Can we convert the strings to the right types?
            Dim newArgs(args.Length - 1) As Object
            Dim index As Integer
            For index = 0 To args.Length - 1
                Try
                    newArgs(index) = Convert.ChangeType(args(index), param(index).ParameterType, CultureInfo.CurrentCulture)
                Catch e As Exception
                    Throw New CustomException(method.DeclaringType.ToString() & "." & method.Name & ": Parameter Conversion Failed", e)
                End Try
            Next index

            ' Does the type need an instance?
            Dim instance As Object = Nothing
            If Not method.IsStatic Then
                instance = Activator.CreateInstance(typeObj)
            End If

            ' Invoke the method
            Dim retVal As Object = method.Invoke(instance, newArgs)
            If Not retVal Is Nothing Then
                Return retVal
            Else
                Return String.Empty
            End If
        End Function 'AttemptMethod

        Private Shared Sub Usage()
            Console.WriteLine(("Usage:" & ControlChars.CrLf & "   Invoke [Assembly] [Type] [Method] [Parameters]"))
        End Sub 'Usage

        Private Class CustomException
            Inherits Exception
            Public Sub New(ByVal m As String)
                MyBase.New(m)
            End Sub
            Public Sub New(ByVal m As String, ByVal n As Exception)
                MyBase.New(m, n)
            End Sub
        End Class ' CustomException

    End Class 'App
End Namespace