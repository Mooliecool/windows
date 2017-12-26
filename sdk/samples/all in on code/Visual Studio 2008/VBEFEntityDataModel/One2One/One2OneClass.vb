'****************************** Module Header ******************************'
' Module Name:    One2OneClass.vb
' Project:        VBEFEntityDataModel
' Copyright (c) Microsoft Corporation.
'
' This example illustrates how to insert, update and query the two entities 
' with one to one association.
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


Namespace VBEFEntityDataModel.One2One
    Friend Class One2OneClass

        ' Test all the mothods in One2OneClass
        Public Shared Sub One2OneTest()

            InsertPersonWithPersonAddress()

            UpdatePerson()

        End Sub

        ' Insert new person with new personAddress 
        Public Shared Sub InsertPersonWithPersonAddress()

            Using context As New EFO2OEntities()

                Dim person As New Person() With _
                { _
                    .FirstName = "Lingzhi", _
                    .LastName = "Sun" _
                }

                ' The PersonID in PersonAddress will be 27 because 
                'it depends on person.PersonID 
                Dim personAddress As New PersonAddress() With _
                { _
                    .PersonID = 100, _
                    .Address = "Shanghai", _
                    .Postcode = "200021" _
                }

                ' Set navigation property (one-to-one)
                person.PersonAddress = personAddress

                context.AddToPerson(person)

                Try

                    Console.WriteLine("Inserting a person with " _
                       + "person address")

                    context.SaveChanges()

                    Query()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
               
            End Using
        End Sub


        ' Get all the people with their addresses 
        Public Shared Sub Query()

            Using context As New EFO2OEntities()

                Dim query = _
                    From p In context.Person.Include("PersonAddress") _
                    Select p

                Console.WriteLine("People with their addresses:")

                For Each p As Person In query

                    Console.WriteLine("{0} {1}", p.PersonID, p.LastName)

                    If Not p.PersonAddress Is Nothing Then
                        Console.WriteLine(" {0}", p.PersonAddress.Address)
                    End If

                Next

                Console.WriteLine()

            End Using
        End Sub


        ' Update one existing person
        Public Shared Sub UpdatePerson()

            Using context As New EFO2OEntities()

                Dim person As New Person()

                person.PersonID = 1

                context.AttachTo("Person", person)

                person.LastName = "Chen"

                person.PersonAddress = Nothing

                Try

                    Console.WriteLine("Modifying Person 1's LastName to {0}" _
                        + ", and PersonAddress to null", person.LastName)

                    context.SaveChanges()

                    Query()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub

    End Class

End Namespace
