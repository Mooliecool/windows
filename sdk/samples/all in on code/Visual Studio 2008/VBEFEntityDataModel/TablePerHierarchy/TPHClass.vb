'****************************** Module Header ******************************'
' Module Name:    TPHClass.vb
' Project:        VBEFEntityDataModel
' Copyright (c) Microsoft Corporation.
'
' This example demonstrates how to establish table per hierarchy inheritance.
' A table-per-type model is a way to model inheritance where each entity is 
' mapped to a distinct table in the store. Then it shows how to query a list 
' of people, get the corresponding properties of Person, Student and 
' BusinessStudent.
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
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
#End Region


Namespace VBEFEntityDataModel.TablePerHierarchy

    Friend Class TPHClass

        ' Test the query method in TPHClass
        Public Shared Sub TPHTest()
            Query()
        End Sub

        ' Query a list of people, print out the properties of Person, 
        ' Student and BusinessStudent 
        Public Shared Sub Query()
            Using context As New EFTPHEntities()

                Dim people = From p In context.PersonSet _
                    Select p

                For Each p In people
                    Console.WriteLine("Student {0} {1}", p.LastName, p.FirstName)

                    If TypeOf p Is Student Then
                        Console.WriteLine("EnrollmentDate: {0}", _
                            DirectCast(p, Student).EnrollmentDate)
                    End If

                    If TypeOf p Is BusinessStudent Then
                        Console.WriteLine("BusinessCredits: {0}", _
                            DirectCast(p, BusinessStudent).BusinessCredits)
                    End If

                Next
            End Using
        End Sub
    End Class
End Namespace
