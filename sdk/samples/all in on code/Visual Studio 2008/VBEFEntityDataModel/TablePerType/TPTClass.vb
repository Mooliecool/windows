'****************************** Module Header ******************************'
' Module Name:    TPTClass.vb
' Project:        VBEFEntityDataModel
' Copyright (c) Microsoft Corporation.
'
' This example demonstrates how to establish table per type inheritance.
' The difference is that all of the entities are sourced from a single table, 
' with the discriminator column being used as the differentiator. Then it 
' shows how to query all the students in the context.
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


Namespace VBEFEntityDataModel.TablePerType
    Friend Class TPTClass

        ' Test the query method in TPTClass
        Public Shared Sub TPTTest()
            Query()
        End Sub

        ' Query the Student in the context 
        Public Shared Sub Query()
            Using context As New EFTPTEntities()
                Dim people = From p In context.People.OfType(Of Student)() _
                    Select p

                For Each s In people
                    Console.WriteLine("{0} {1} Degree: {2}", _
                                      s.LastName, _
                                      s.FirstName, _
                                      s.Degree)
                Next
            End Using
        End Sub
    End Class
End Namespace
