'****************************** Module Header ******************************'
' Module Name:    TableMergingClass.vb
' Project:        VBEFEntityDataModel
' Copyright (c) Microsoft Corporation.
'
' This example demonstrates how merge tables into one entity and query the 
' fields from the two tables.
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


Namespace VBEFEntityDataModel.TableMerging
    Friend Class TableMergingClass

        ' Test all the methods in TableMergingClass
        Public Shared Sub TableMergingTest()
            Query()
        End Sub

        ' Query the first Person in the merged table 
        Public Shared Sub Query()
            Using context As New EFTblMergeEntities()

                Dim person As Person = (context.Person).First()

                Console.WriteLine("{0} " & vbLf & "{1} {2} " & vbLf & "{3}", _
                                  person.PersonID, _
                                  person.FirstName, _
                                  person.LastName, _
                                  person.Address)
            End Using
        End Sub
    End Class

End Namespace
