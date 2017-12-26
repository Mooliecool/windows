'****************************** Module Header ******************************'
' Module Name:    TableMergingClass.vb
' Project:        VBEFEntityDataModel
' Copyright (c) Microsoft Corporation.
'
' This example demonstrates how split one table into two entities. Then it 
' shows how to insert records into the two entities and query the result.
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


Namespace VBEFEntityDataModel.TableSplitting
    Friend Class TableSplittingClass

        ' Insert and Query PersonWithPersonDetail
        Public Shared Sub TableSplittingTest()
            InsertQueryPersonWithPersonDetail()
        End Sub

        'Insert and Query PersonWithPersonDetail 
        Public Shared Sub InsertQueryPersonWithPersonDetail()

            ' Create the new Person entity
            Dim person As New Person()
            person.FirstName = "Liao"
            person.LastName = "Typot"

            ' Create the new PersonDetail entity
            Dim personDetail As New PersonDetail()
            personDetail.PersonCategory = 0
            personDetail.HireDate = System.DateTime.Now

            ' Set the PersonDetail to Person
            person.PersonDetail = personDetail

            Using context As New EFTblSplitEntitie()

                context.AddToPerson(person)

                Console.Out.WriteLine("Saving person {0}.", person.PersonID)

                ' Note that personDetail.PersonID is the same as 
                ' person.PersonID. This is why we love the Entity Framework. 
                Console.Out.WriteLine("Saving person detail {0}." & vbLf, _
                                      personDetail.PersonID)

                context.SaveChanges()

            End Using

            Using context As New EFTblSplitEntitie()

                ' Retrieve the newly inserted person
                Dim person2 As Person = (From p In context.Person _
                    Where p.PersonID = person.PersonID _
                    Select p).FirstOrDefault()

                Console.Out.WriteLine( _
                    "Retrieved person {0} with person detail '{1}'.", _
                    person2.PersonID, _
                    person2.PersonDetail)

                person.PersonDetailReference.Load()

                Console.Out.WriteLine( _
                    "Retrieved hiredate for person detail {0}.", _
                    person2.PersonDetail.HireDate)

            End Using
        End Sub
    End Class
End Namespace
