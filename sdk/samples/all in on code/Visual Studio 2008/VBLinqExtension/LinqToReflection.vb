'****************************** Module Header ******************************\
' Module Name:    LinqToReflection.vb
' Project:        VBLinqExtension
' Copyright (c) Microsoft Corporation.
'
' It is a simple LINQ to Reflection library to get the methods of the 
' exported class in an assembly grouped by return value type.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Reflection
Imports System.Windows.Forms
Imports System.IO
#End Region

Public Class LinqToReflection

    ''' <summary>
    ''' Get the methods of the exported class in an assembly grouped by 
    ''' return value type.
    ''' </summary>
    ''' <param name="assemblyPath">The path of the assembly file</param>
    ''' <returns>IEnumerable collection methods grouped by return value type</returns> 
    ''' 

    Public Shared Function GroupMethodsByReturnValue(ByVal assemblyPath As String) As IEnumerable(Of IGrouping(Of String, MethodInfo))
        ' Check if the assembly file exists
        If File.Exists(assemblyPath) Then
            ' Load the assembly
            Dim assembly As Assembly = assembly.LoadFrom(assemblyPath)

            ' BindingFlags to load all the static and instance methods
            Dim staticInstanceAll As BindingFlags = BindingFlags.Static Or BindingFlags.Instance Or BindingFlags.NonPublic Or BindingFlags.Public

            ' Get the methods of the exported class grouped by return 
            ' value type. 

            '********************************************************
            Dim query = From type In assembly.GetExportedTypes() _
               From method In type.GetMethods(staticInstanceAll) _
               Where method.DeclaringType Is type _
                Group method By method.ReturnType.FullName Into All = Group
            '********************************************************
            Return query
        Else
            Throw (New FileNotFoundException())
        End If
    End Function

End Class
